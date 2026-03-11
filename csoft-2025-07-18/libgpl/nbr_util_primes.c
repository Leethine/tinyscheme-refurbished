/* $Id: nbr_util_primes.c,v 1.2 2021/09/15 04:25:40 alex Exp alex $ */
/*******************************************************************************

File:

    nbr_util_primes.c

    Number Utilities (Prime Numbers)


Author:    Alex Measday


Purpose:

    The NBR_UTIL package is a collection of functions dealing with numbers
    in some way or another.  I wanted to remove the calculation of the first
    (next) prime number after the expected table size from my HASH_UTIL
    package and "prmNextPrime()" didn't really appeal to me, so I decided
    on a prefix of NBR.  Perhaps I'll add some other number-related functions
    in the future.

    The source code for the package consists of a single header file,
    nbr_util.h, and multiple C source files:

        nbr_util_primes.c - for the NBR prime number functions.
        nbr_util_roots.c - for the NBR fast integer square root function.
        nbr_util_others.c - for other NBR functions.

    When you see the primes and roots source file prologs, you'll understand
    why I divided them up.  (I originally had them all in one file.)

    The prime number functions are used to test if unsigned integers are prime
    and to step through successive primes.  This example followed an example
    of nbrUnsignedSQRT(); hence the fact that NUMBER already has a value of 34.

						-- # is 34, from above
        boolean  isPrime = nbrIsPrime (number, NULL) ;
        printf ("%lu is %s\n",			-- Prints "34 is composite"
                number, isPrime ? "prime" : "composite") ;

        number = nbrNextPrime (number, NULL) ;
        printf ("%lu\n", number) ;		-- Prints 37
        printf ("%lu\n",			-- Prints 43 (37 -> 41 -> 43)
                nbrNextPrime (nbrNextPrime (number, NULL), NULL)) ;

    Following the list of public functions below is a very long, very detailed
    discussion of prime numbers:

        PRIME NUMBERS
            TERMINOLOGY
            GENERALIZED ALGORITHM
            LIMIT CHECKING
            PERFORMANCE
            MATHEMATICALLY SPEAKING
            UNDERSTANDING ROSS FOSLER'S ASSEMBLY CODE

    The actual code begins around line 850 in this file, with the inclusion
    of header files, data declarations, etc.


Public Procedures:

    nbrIsPrime() - checks if a number is a prime number.  This is the
        full-blown version that works with different skip-through primes
        and sequences of differences.  It's faster and not really much
        bigger than the following ...
    nbrIsPrime() - checks if a number is a prime number.  This version has
        a small memory footprint and is hard-coded for skip-through prime 3.
        A C Preprocessor symbol must be defined for this version to be used;
        one or the other version will be compiled into the object file, but
        not both at the same time.
    nbrNextPrime() - determines the next prime number following a given value.
    nbrNextPrimeF() - determines, using a caller-supplied check-prime function,
        the next prime number following a given value.
    nbrPrimeConfigCreate() - creates a prime determination configuration.
    nbrPrimeConfigDestroy() - destroys a prime determination configuration.
    nbrPrimeSeqDiffs() - generates a sequence of differences.
    nbrPrimeSeqDiffsLength() - calculates the length of a sequence
        of differences.
    nbrPrimeSeqDiffsMax() - determines the maximum difference in a sequence
        of differences.
    nbrPrimeSeqDiffsRange() - determines the range of divisors in a sequence
        of differences.

*******************************************************************************/

/*******************************************************************************

    Prime Numbers
    =============

            "We don't do this song very much, but I feel like
            singing so, I guess, that's what we're gonna do."
                        - Duane Allman, "Dimples", Ludlow Garage, 1970

    I feel like writing so, I guess, that's what I'm going to do!

        WARNING:  All over the internet, you'll find example code that
        determines if a number is prime.  Nearly all the examples I've
        seen loop through divisors while "i*i<=n" (i.e., while the square of
        the next divisor to be tested is less than or equal to the number).
        Squaring the divisor is fine if you're using arbitrary-precision
        arithmetic, but it can fail at the high end if you're using
        fixed-size integers.  Take unsigned 32-bit numbers for example:

           ~65535.9^2 = 4,294,967,295 (maximum unsigned 32-bit number)
            65535.0^2 = 4,294,836,225

        There are 5,853 prime numbers between 65535.0^2 and ~65535.9^2.
        When checking the next divisor after 65,535 for any of these primes,
        the "i*i" computation will overflow, leading to meaningless, possibly
        infinite loops.  This is discussed in more detail in the "Limits
        Checking" subsection below.

        NOTE:  This prime number package is GROSSLY OVERENGINEERED.  I
        began with an optimization that skipped testing potential divisors
        divisible by 2 or by 3 in a small, tight loop.  I then got to
        wondering about skipping potential divisors divisible by an even
        wider range of prime divisors; e.g., skip divisors divisible by
        2, 3, 5, 7, or 11.  I generalized the code to handle an arbitrary
        range, [2..<prime>], of prime divisors.  When skipping divisors
        divisible by 2 or by 3, the generalized code is virtually as fast
        as the hard-coded loop.

        NOTE:  I was REINVENTING THE WHEEL.  I basically had and have a
        grade-school knowledge of prime numbers.  After writing the code,
        I looked up on the internet fast ways of determining primes and
        I learned that "my" optimization was widely known (see the
        mathematics section below).  So perhaps this was all a waste of
        time, but the open-ended exploration of the subject on my own
        was very intellectually satisfying for me.

        NOTE:  If a SMALL MEMORY FOOTPRINT is important, this package can
        be compiled to produce compact object code that uses the original
        hard-coded loop and omits the sequence-of-differences functions
        and data related to the generalized version of the code.  To
        accomplish this, add

                        -DNBR_SKIP_2_3_HARDCODED=1

        to the compilation command for "nbr_util.c".  If an application
        only calls nbrIsPrime() and/or nbrNextPrime(), its code does NOT
        need to be compiled with the CPP definition.  If, however, the
        application calls any of the sequence-of-differences functions,
        the application should be compiled with the CPP definition.
        Macros in "nbr_util.h" substitute meaningful return values for
        the omitted functions.

        NOTE:  To jog your memory, if necessary, a NUMBER IS PRIME if it's
        positive and if its only divisors are 1 and itself; zero and one by
        definition are not prime.  To determine if a number is prime, you
        basically test if the number is evenly divisible (remainder 0) by
        any divisor in the range 2..sqrt(number).  If the number is evenly
        divisible by some divisor, then the number is not prime.  If none
        of the divisors evenly divide the number, then the number is prime.

    I wrote my HASH_UTIL hash table package in the late 1980s, as I recall.
    It has long been a nuisance that a program using the package has had
    to be explicitly linked to the C math library ("-lm").  So, about 30
    years later, I decided to fix that!  Thinking that the math library
    was required because of the use of sqrt(3) in (i) determining the next
    prime number larger than the expected table size and (ii) computing
    the standard deviation of the bucket lengths, I dove into the code.
    My memory hadn't served me well, because I discovered that the prime
    number function was not using sqrt(), but was instead iterating until:

        divisor > (number / divisor)

    as you see in nbrIsPrime() below.  (I don't know if I was considering
    the possibility of overflow at the time I wrote the hash table package,
    but it was a fortuitous choice for the loop test.)

    So far, so good, but I was red-faced to see that for 3 decades my code
    had been checking each and every possible divisor from 2 on up: 2, 3,
    4, 5, 6, 7, ...  Skipping all the even numbers after 2 was a painfully
    obvious optimization.  And then I began thinking about also skipping
    odd numbers divisible by 3: check divisors 5, 7, ... skip 9 ..., 11,
    13, ... skip 15 ..., 17, 19, etc.  I noticed the differences between
    divisors to be tested was 2, 4, 2, 4, etc.  In other words, a repeating
    cycle of two divisors.  I implemented a hard-coded loop ("divisible"
    means evenly divisible with a remainder of 0):

        if (number equals 0 or 1)  return (not-prime) ;
        if (number equals 2 or 3)  return (is-prime) ;
        if (number divisible by 2 or by 3)  return (not-prime) ;

        for (divisor = 5 to "square root" of number) {
            if (number divisible by divisor)  return (not-prime) ;
            divisor += 2 ;
            if (number divisible by divisor)  return (not-prime) ;
            divisor += 4 ;
        }

        return (is-prime) ;

    which checks the following divisors: [2, 3,] 5, 7, 11, 13, 17, 19, 23,
    25, 29, 31, 35, 37, ...  Skipping odd divisors divisible by 3 is almost
    twice as fast as checking all of the odd numbers.

    I was pleased by my independent "discovery" of this heretofore
    unthought-of optimization (/snark) and my thoughts naturally turned
    to skipping even more divisors.


    TERMINOLOGY
    -----------

    First, however, some terminology I use in this package:

        Skip-Through Prime (STP) - is the maximum prime number for which
            divisors divisible by one or more of primes 2 through STP are
            skipped.  In the pseudocode above, the skip-through prime is 3.
            As another example, if the skip-through prime is 11, nbrIsPrime()
            will skip divisors divisible by any of the primes 2 through 11:
            2, 3, 5, 7, 11.

        Start Divisor - is the divisor at which to begin the first cycle of
            testing.  This is always the next prime after the skip-through
            prime.  Using the examples above, STP 3's start divisor is 5
            and STP 11's start divisor is 13.

        Sequence of Differences - is an array of the differences between
            successive divisors to be tested.  The differences skip over
            divisors that do not need to be tested; i.e., divisors evenly
            divisible by primes 2 through the skip-through prime.

            Beginning with the start divisor, nbrIsPrime() tests if the
            target number is evenly divisible by the start divisor.  If so,
            the number is not prime and nbrIsPrime() returns.  If not, the
            first difference in the sequence is added to the start divisor
            to get the next divisor to test.  The function continues testing
            successive divisors; when the end of the sequence is reached,
            nbrIsPrime() jumps back to the beginning of the sequence to
            continue with the next pass through the sequence.

            For example, in the pseudocode above, the sequence of differences
            for STP 3 is { 2, 4 }.  Beginning with start divisor 5, the loop
            iterations unfold as follows:

                (1) Test  5, Add 2, Test  7, Add 4
                (2) Test 11, Add 2, Test 13, Add 4
                (3) Test 17, Add 2, Test 19, Add 4
                (4) Test 23, Add 2, Test 25, Add 4
                (5) Test 29, ...

        Limit Check - refers to checking if a divisor about to be tested
            is less than or equal to the square root of the target number.
            When testing all odd divisors (skip-through prime 2), there is
            only one test in the loop and the limit check is performed as
            part of the loop control.  For skip-through primes 3 and above,
            there are multiple tests per loop iteration, corresponding to
            each difference in the prime's sequence.  STP 5 has a sequence
            of 8 differences, for example, but adding 7 limit checks inside
            the loop incurs a performance hit.  (The pseudocode above doesn't
            limit check the second test's divisor in the loop.)  I discuss
            this issue in more detail below.


    GENERALIZED ALGORITHM
    ---------------------

    The pseudocode for the generalized implementation inserts an inner loop
    that steps through the sequence of differences (again, "divisible" means
    evenly divisible with a remainder of 0):

        if (number equals 0 or 1)  return (not-prime) ;
        if (number equals any prime in 2..STP)  return (is-prime) ;
        if (number divisible by any prime in 2..STP)  return (not-prime) ;

        divisor = start divisor ;
        while (divisor <= "square root" of number) {
            for (each difference in sequence) {
                if (number divisible by divisor)  return (not-prime) ;
                divisor += difference ;
            }
        }

        return (is-prime) ;

    Figuring out the sequence of differences for skip-through prime 3 was
    easy enough to do in my head.  I wrote a throw-away program to generate
    differences for skip-through primes 5, 7, and 11; examined the listings
    to determine when the sequences began repeating (8, 48, and 480,
    respectively); and noticed the first of a few relations I would find:

        seq-length (next-prime) = seq-length (current-prime) * (next-prime - 1)

    For example:

        Skip-through prime 2's sequence of differences, { 2 }, is of length 1.
        STP 3 has a sequence of length 2 = 1 * (3 - 1).
        STP 5 has a sequence of length 8 = 2 * (5 - 1).
        STP 7 has a sequence of length 48 = 8 * (7 - 1).
        STP 11 has a sequence of length 480 = 48 * (11 - 1).
        And so on.

    Sequence length increases by one or two orders of magnitude for each new
    prime, of the skip-through primes under consideration: 2..53 for 64-bit
    unsigned long integers.  STP 59's sequence length exceeds the maximum
    representable, unsigned 64-bit value.

        range (next-prime) = range (current-prime) * next-prime

    The range of a sequence of differences is the range of divisors covered
    by each pass over the sequence, found by summing all the differences in
    the sequence.  STP 3's sequence is { 2, 4 }, whose range is 2 + 4 = 6;
    as an example above shows, the first divisor tested in each loop iteration
    is 5, 11, 17, 23, 29, ...  STP 5's 8-element sequence has a range of 30,
    so the divisors at the beginning of each loop iteration are 7, 37, 67,
    97, ...  I wasn't clever enough to realize that the formula above is
    more simply expressed as the product of the preceding primes and the
    skip-through prime; e.g., the range of STP 7 is 2 * 3 * 5 * 7 = 210.
    Although STP 53's sequence length fits in an unsigned 64-bit integer,
    its range doesn't.

        density (prime) = seq-length (prime) / range (prime)

    Density, in this case, is a measure of how many numbers in a range are
    actually tested as divisors.  The lower the density the better.  If even
    numbers are skipped and odd numbers are tested (STP 2), the density is
    50%.  Higher skip-through primes lower the density even further at the
    expense of increasingly lengthy sequences of differences.

        max-difference (next-prime) = current-prime * 2

    Sometimes!  The maximum differences in the sequences for skip-through
    primes [2,] 3, 5, 7, 11, 13, 17, 19, and 29, 31 obey this relation.
    For example, the maximum difference in STP 31's sequence is 29 * 2 = 58.
    However, skip-through prime 23's maximum difference is 40 (not the
    expected 19*2 = 38) and STP 37's is 66 (not the expected 31*2 = 62).
    Using nbrPrimeSeqDiffsMax() to generate the one trillion (10^12)
    differences in STP 37's sequence and determine the maximum difference
    originally consumed 99%-100% of the CPU for 8.5 days.  I subsequently
    optimized the function by skipping both even divisors and odd divisors
    divisible by 3, reducing the time to 6.4 days.  I'm not a glutton for
    punishment, so I did not give serious consideration to optimizing
    nbrPrimeSeqDiffs() and nbrPrimeSeqDiffsMax() further by using higher
    skip-through primes than 3.  In any case, finding out STP 41's maximum
    among its 44 trillion differences would take three-quarters of a year
    or more.  Maybe there's a mathematical relation that gives the correct
    maximum difference for all skip-through primes, but nothing jumped out
    at me.

    Knowing a sequence's maximum difference--and using differences in the
    first place--can significantly reduce memory usage.  For unsigned
    64-bit integers, the largest skip-through prime, 47, has an estimated
    maximum difference of 43 * 2 = 86, which can be represented in an
    unsigned 8-bit integer.  STP 47's sequence of differences requires
    only one-eighth of the memory that the sequence of offsets in the math
    section below requires.  (Actually, STP 53 is the largest skip-through
    prime in the differences form of the algorithm.  It can't be used in
    the math section's offset form of the algorithm.  The last offset for
    any STP is always one less than the STP's range and STP 53's range
    exceeds the maximum representable, unsigned 64-bit integer by
    14&times;10<sup>18</sup>.)  Looking at unsigned 128-bit integers,
    the largest skip-through prime is 101 with an expected maximum
    difference of 202, which, if correct, can still be represented
    in an unsigned 8-bit integer.  (This is all theoretical for the
    most part; the largest skip-through prime I've actually tested was
    29 with its one-billion-element sequence.)

    Time for some actual numbers to get a sense of what we are dealing
    with.  The following numbers are for unsigned 64-bit integers (L/R
    is the length divided by the range; i.e., density):

         Skip-                              Estimated   Sequence Range
        Through   Start                      Maximum       (Sum of
         Prime   Divisor  Sequence Length   Difference   Differences)     L/R
        -------  -------  ---------------   ----------  --------------    ---
           2        3                     1      2                    2   50%
           3        5                     2      4                    6   33%
           5        7                     8      6                   30   26%
           7       11                    48     10                  210   22%
          11       13                   480     14                 2310   20%
          13       17                  5760     22                30030   19%
          17       19                 92160     26               510510   18%
          19       23               1658880     34              9699690   17%
          23       29              36495360     38            223092870   16%
          29       31            1021870080     46           6469693230   15%
          31       37           30656102400     58         200560490130   15%
          37       41         1103619686400     62        7420738134810   14%
          41       43        44144787456000     74      304250263527210   14%
          43       47      1854081073152000     82    13082761331670030   14%
          47       53     85287729364992000     86   614889782588491410   13%
          53       59   4434961926979584000     94           <overflow>   13%
            SIZE_MAX:  18446744073709551615

    (The PRIMAL program computes the range as both an integer and a
    floating-point number.  If the integer overflows, the floating-point
    number is substituted in the density calculation.  So the density for
    skip-through prime 53 is correctly listed as 13%.)

    For unsigned 128-bit integers, the following figures pick up with
    skip-through prime 53 (whose range overflowed 64 bits) and run through
    skip-through prime 101.  The density at this point has only dropped down
    to 11.9%.  (The sequence length and range both overflow for STP 103.)

        53 (Skip-Through Prime)    59 (Start Divisor)   94 (Est. Max. Diff.)
        Length:                      4434961926979584000    (4.43496e+18)
         Range:                     32589158477190044730    (3.25892e+19)

        ... through ...

        101 (Skip-Through Prime)  103 (Start Divisor)  202 (Est. Max. Diff.)
        Length:   27739969042773783995307880611840000000    (2.774e+37)
         Range:  232862364358497360900063316880507363070    (2.32862e+38)
      SIZE_MAX:  340282366920938463463374607431768211455    (3.40282e+38)

        103 (Skip-Through Prime)    107 (Start Divisor)
        Length:  <overflow>
         Range:  <overflow>

    Knowing the exploding lengths and ranges for the higher skip-through
    prime, whether for 64-bit or 128-bit numbers, was of intellectual
    interest to me, but these higher skip-through primes are obviously
    not of any practical value.  The Prime Page's Glossary entry for
    wheel factorization says, in different terminology than mine, that
    skip-through prime 251 reduces the density to 10% and 75,037 reduces
    it to 5%.  Skip-through prime 134,253,593, with a sequence length of
    approximately 6.4e+99, drives the density down to 3%!
    ( https://primes.utm.edu/glossary/page.php?sort=WheelFactorization )


    LIMIT CHECKING
    --------------

    "Limit checking" refers to not testing divisors greater than the square
    root of the number when determining if the number is prime.

    First of all, heed my warning at the very beginning about the danger
    of overflow using the widely seen "i*i<=n" check with fixed-precision
    integers.  The following explanation uses 64-bit unsigned longs, but
    the warning applies to any bit width.

    The integer square root of the maximum representable, 64-bit value,
    ULONG_MAX, is 0x0FFFFFFFF--all of the lower 32 bits set.  (The real
    square root of ULONG_MAX is 0x0FFFFFFFF plus about 0.9 decimal.)
    If you square the integer square root, the result is a number that
    is 8.6 billion less than ULONG_MAX.  In other words, the 8.6 billion
    numbers in the range "root^2..ULONG_MAX" all have the same integer
    square root: 0x0FFFFFFFF.  There are 193 million primes in that range.
    Consequently, for each of the 193 million primes tested with divisor "i":

      - "i" will reach 0x0FFFFFFFF,
      - "i*i" with that value will always be less than "n" (never equal
        because if "n == root^2", "n" is not prime),
      - "i" will be incremented past 0x0FFFFFFFF, and
      - "i*i" will always overflow.

    So 193 million primes are excluded from being tested because of the "i*i"
    multiplication.  The steps above will occur when testing every possible
    divisor ("i++") or only odd divisors ("i += 2").

    Additional optimizations in the algorithm make the situation even worse.
    If the algorithm is extended to also skip divisors divisible by 3 (my
    first optimization), the divisor is incremented alternately by 2 and by 4.
    Since 0x0FFFFFFFF is divisible by 3 and will be skipped, "i" will only
    reach the preceding odd number, 0x0FFFFFFFD, and then be incremented
    by 4 to 0x100000001, causing "i*i" to overflow.  The square of the
    preceding odd number is 25.8 billion less than ULONG_MAX.  Testing the
    primality of the 579 million primes in that range--those whose integer
    square root is 0x0FFFFFFFD, 0x0FFFFFFFE, or 0x0FFFFFFFF--is impossible.

    To avoid overflow with fixed-precision integers, divide both sides of
    the relational operator by "i":

        if (i*i <= n) ... continue checking divisors ... (INCORRECT)

        if (i <= n/i) ... continue checking divisors ... (CORRECT)

    Now that there is no danger of overflow, there are some other aspects
    of limit checking in relation to my skip-through prime optimization
    that need to be examined.  Remember that the algorithm is basically
    two nested loops, where the inner loop runs through the elements in
    the sequence of differences and the outer loop repeats the inner loop
    as often as necessary to reach and pass the square root of the number
    (if the number is prime):

        div = STP's start divisor
        while (div <= n/div) {
            foreach difference in sequence {
                ... test if n is evenly divisible by div ...
                div = div + difference
            }
        }

    In the Wikipedia pseudocode for the related "c#k+i" algorithm (see the
    mathematics section below), the "inner loop" corresponding to my inner
    loop tests the same number of divisors as mine does, but without limit
    checking the divisors.  (I put "inner loop" in quotes because the
    pseudocode is hard-coded for the STP 3 optimization and the "loop" is a
    single IF-statement that checks both the "6(k-1)+5" and "6k+1" divisors;
    again, the "c#k+i" version of my STP 3 optimization is explained in the
    mathematics section.)  It is a good idea, however, to limit check each
    divisor inside the *inner* loop as well as the outer loop:

        div = STP's start divisor
        while (div <= n/div) {
            foreach difference in sequence {
                ... test if n is evenly divisible by div ...
                div = div + difference
                if (div > n/div)  break
            }
        }

    (Whether the check is at the beginning or end of the inner loop, there
    will always be one check that duplicates the check in the outer loop on
    each pass through the sequence.)

    Because the length of the sequence and range increase rapidly with higher
    skip-through primes, there are two cases that are of concern when there
    is no limit check in the inner loop:

    (1) The primality test must run through the entire sequence of
        differences.  If the divisor exceeds the square root early in
        a pass through the sequence, doing all the remaining tests in
        the sequence is a waste of time.  Assume the function uses STP 11
        with its 480-difference sequence.  If on some cycle the divisor
        surpasses the square root at the 5th difference, the inner loop
        will uselessly continue checking the other 475 divisors, all of
        which are greater than the square root.

    (2) If the range of a sequence is greater than a prime number, continued
        testing of divisors past the square root might reach a point where
        the prime number itself is tested as a divisor.  "n%n == 0", so the
        number will be rejected as composite even though it's really prime!

    The first case above is a matter of efficiency; the second case requires
    flagging a number as composite if "n%div == 0" *and* if "n != div".

    Adding the limit check, "div > n/div", to the inner loop caused a big
    performance hit.  My first attempt at ameliorating the slow-down was
    to add a global boolean variable that the calling program could set to
    enable or disable limit checking in the inner loop:

        if (gCheckLimits && (div > n/div))  ...

    Testing a boolean flag takes no time at all and, when limit checking is
    disabled, the algorithm is as fast as the original no-limit-checking
    algorithm.  However, I was not satisfied with burdening the calling
    program with the responsibility of deciding whether to turn limit
    checking on or off.

    My second attempt took advantage of a sequence's range to put off
    limit checking until the tested divisor got "close" to the square
    root of the input number.  First, I computed a rough estimate of
    the square root by repeatedly dividing the number by 2 until
    "root <= n/root".  Then, in the outer loop, before beginning the
    inner loop each time, figure out if the rough root estimate falls
    within the range of numbers covered by the coming pass through the
    sequence of differences.  If not, clear a local boolean variable
    to disable limit checking in the inner loop.  If the estimated root
    does fall in the range, set the boolean variable to enable checking.

        ... compute rough estimate of square root ...
        checkLimits = false
        div = STP's start divisor
        while (div <= n/div) {
            if (div <= root <= (div+range-1))  checkLimits = true
            foreach difference in sequence {
                ... test if n is evenly divisible by div ...
                div = div + difference
                if (checkLimits && (div > n/div))  break
            }
        }

    A big improvement without intervention by the calling program, but ...

        (a) The estimate of the square root could be off by as much as
            half of the actual square root, so limit checking might begin
            multiple range blocks before the range containing the actual
            square root.  Obviously, it would be faster to only check
            limits in the last block.

        (b) The large range of a higher skip-through prime's sequence
            may be greater than the estimated or actual square root
            and limit checking would then begin with the very first
            divisor of the very first pass through the sequence; i.e.,
            the skip-through prime's start divisor.  Thus the algorithm
            reverts to the slow check-every-divisor mode.

    A more accurate estimate of the square root was needed, so I searched
    for "fast integer square root" on the web and, not unsurprisingly, up
    popped numerous references to a technical bulletin, "Fast Integer
    Square Root", by Ross M. Fosler.  I studied and implemented the
    algorithm and it is indeed very fast.  My is-prime function now knows
    exactly when the tested divisors pass the actual square root of the
    number and the function no longer needs to perform the time-consuming
    "div > n/div" check in the inner loop:

        root = nbrUnsignedSQRT (n)
        div = STP's start divisor
        while (div <= root) {
            foreach difference in sequence {
                ... test if n is evenly divisible by div ...
                div = div + difference
                if (div > root)  break
            }
        }


    PERFORMANCE
    -----------

        Performance tests were conducted using my PRIMAL test program.
        The command lines are shown below.  The "output" following
        the command lines are not the output of the program, but me
        tabulating the results of multiple runs.  When PRIMAL is in
        "-time" mode, most output is suppressed so that I/O is not
        a significant factor in the times.  Also, the generation of
        sequences of differences is done before the timer starts counting.

    My original prime number function was in my HASH_UTIL package and the
    largest hash table I ever needed was for about 80,000 telemetry points.
    (Most of those points were variable names tied to memory locations in
    the onboard computer and would only be downloaded, selectively, when
    trying to diagnose a problem.)  So my first suite of tests listed all
    the prime numbers in the range 80,000-200,000.  Since listing those
    numbers is very quick, I had each run repeat the test 1000 times to
    get a large enough timing figure for comparison's sake.

    (1) Determine the prime numbers in the range 80,000-200,000;
        repeat 1000 times:

        % primal -time -repeat 1000 -list 80000-200000 -alternate odds

            STP 2*   209.9 seconds    * hand-coded loop to check odd divisors

        % primal -time -repeat 1000 -list 80000-200000 -qprime <stp>

            STP 2    221.3 seconds
            STP 3    127.5 seconds
            STP 5     87.0 seconds
            STP 7     71.0 seconds
            STP 11    65.5 seconds
            STP 13    62.2 seconds
            STP 17    60.3 seconds
            STP 19    59.1 seconds
            STP 23    58.5 seconds
            STP 29    58.0 seconds

        The first test ("-alternate odds") used a run-of-the-mill, hand-coded
        loop to check all odd divisors.  The remaining tests ("-qprime <stp>")
        used my sequence of differences, hence the slightly higher time for
        STP 2.  As expected, the times decrease as the skip-through prime
        increases.

    The second suite of tests determines the 1,000 largest prime numbers
    representable in unsigned, 64-bit integers.  These numbers are verified
    by matching them against a pre-computed list of the primes generated by
    a different program, Achim Flammenkamp's PRIME_SIEVE, found at his web
    page:

        "The Sieve of Eratosthenes"
        http://wwwhomes.uni-bielefeld.de/achim/prime_sieve.html

    [Modifications to his program: (i) define LONG as "unsigned long",
    (ii) undefine _ANSI_EXTENSION, (iii) uncomment the commented-out
    code in the definition of "use(x)", and (iv) hard-code the initial
    sieve size.]

    (2) Determine and verify the largest 1,000 prime numbers representable
        in unsigned, 64-bit integers against a pre-computed (by a different
        program) list of those primes:

        % primal -time -verify last64 -qprime <stp>

            STP 3     90,842 seconds    (25.2 hours)
            STP 5     55,797 seconds    (15.5 hours)
            STP 7     43,093 seconds    (12.0 hours)
            STP 11    38,338 seconds    (10.6 hours)
            STP 13    35,310 seconds    ( 9.8 hours)
            STP 17    33,234 seconds    ( 9.2 hours)
            STP 19    31,667 seconds    ( 8.8 hours)
            STP 23    30,238 seconds    ( 8.4 hours)
            STP 29    29,430 seconds    ( 8.2 hours)

    The most time-consuming aspect of my algorithm is generating the sequence
    of differences for the higer skip-through primes.  My computer was able
    to generate a physical, 1-billion-byte sequence of differences for STP 29,
    but malloc(3) failed trying to allocate the 31-billion-byte sequence for
    STP 31.  The nbrPrimeSeqDiffsMax() function generates the same sequences
    of differences, but without requiring any memory for the sequence: each
    difference is generated, compared to the running maximum value, and then
    discarded.  So I was able to time sequence generation for STPs 31 and 37;
    the last took 6.4 days and I didn't have the patience to wait 280+ days
    for the generation of STP 41's 44-trillion-element sequence to complete.

    (3) Generate sequences of differences:

        % primal -time -qmax <stp>

            STP 17        0.04 seconds             92 thousand elements
            STP 19        0.4 seconds               1.7 million elements
            STP 23       11 seconds                36 million elements
            STP 29      407 seconds  (6.8 minutes)  1 billion elements
            STP 31   12,602 seconds  (3.5 hours)   31 billion elements
            STP 37  550,670 seconds  (6.4 days)     1 trillion elements


    MATHEMATICALLY SPEAKING
    -----------------------

    When the code was pretty much in its final form, bar tweaking of the
    limit checking, I researched ways to speed up generating sequences
    of differences (e.g., 8.5 days for skip-through prime 37's one
    trillion differences, a 10^12-length sequence).  This research was
    unsuccessful, but I came upon many queries on programming forums about
    determining if a number is prime and many responses about an optimized
    method called "6k +/- 1".  I then searched for information about
    6k +/- 1 and landed on the Wikipedia page about primality testing
    using "trial division", i.e., looping through all possible divisors
    to find a divisor other than 1 for the number.  From Wikipedia
    ( https://en.wikipedia.org/wiki/Primality_test#Simple_methods ):

        The algorithm can be improved further by observing that all primes
        are of the form 6k +/- 1, with the exception of 2 and 3.  This is
        because all integers can be expressed as (6k + i) for some integer
        k and for i = -1, 0, 1, 2, 3, or 4; 2 divides (6k + 0), (6k + 2),
        (6k + 4); and 3 divides (6k + 3).  So, a more efficient method is
        to test if n is divisible by 2 or 3, then to check through all the
        numbers of form 6k +/- 1 <= sqrt(n).  This is 3 times as fast as
        testing all m.

        Generalising further, it can be seen that all primes are of the
        form c#k + i for i < c# where c and k are integers, c# represents
        c primorial, and i represents the numbers that are coprime to c#.
        For example, let c = 6.  Then c# = 2 * 3 * 5 = 30.  All integers
        are of the form 30k + i for i = 0, 1, 2, ..., 29 and k an integer.
        However, 2 divides 0, 2, 4,...,28 and 3 divides 0, 3, 6,...,27 and
        5 divides 0, 5, 10,...,25.  So all prime numbers are of the form
        30k + i for i = 1, 7, 11, 13, 17, 19, 23, 29 (i.e. for i < 30 such
        that gcd(i,30) = 1).  Note that if i and 30 were not coprime, then
        30k + i would be divisible by a prime divisor of 30, namely 2, 3
        or 5, and would therefore not be prime.

    So all primes are of the form c#k+i for i meeting the conditions above.
    (Note that the reverse is not true; numbers of the form c#k+i are not
    necessarily prime.)  Relating this to my code:

        c	is my skip-through prime.  (Strictly speaking, c can be
                any number in the range STP ... next-prime - 1; in other
                words, STP is the largest prime less than or equal to c.)

        c#      is "c primorial", where primorial is simply the product of
                all the primes less than or equal to c; i.e., 2x3x5x7x...
                This value is identical to my "range" of a sequence of
                differences.  ( https://en.wikipedia.org/wiki/Primorial )

        k       indexes the blocks of range c# of divisors to be checked.
                Incrementing k is like reaching the end of a sequence of
                differences in my code and looping back to start at the
                beginning of the sequence for the next higher block of
                divisors.

        i       is drawn from the set of numbers in the effective range
                1..c#-1 such that each i is "coprime" with c#.  Two numbers
                are coprime with respect to each other if they share no
                divisors other than 1.  Note that "coprime" is a relation
                between numbers, not an attribute (like "prime") of a single
                number.  ( https://en.wikipedia.org/wiki/Coprime_integers )

        phi(c#) is Euler's "totient" function, which produces the count of
                the numbers in 1..C#-1 that are coprime with C#.  This count
                is the same as my "length" of a sequence of differences.
                ( https://en.wikipedia.org/wiki/Euler%27s_totient_function )

    So the c#k+i optimization is the foundation for my code, unbeknownst to
    me when I wrote the code!  The difference between the c#k+i method and
    my code is that the former method works with explicit i offsets from the
    base of a c#k block and my code works with the sequence of differences
    between successive i offsets.

    Now let's consider the set of numbers coprime with c# from which i is
    drawn.  The numbers are in the range 0..c#-1.  Zero (0) drops out because
    the first multiplier in c# is 2, so c#k+0 is always even (divisible by 2).
    The very first number in the set is always 1 since the only divisor it
    shares with c# is 1.

    The second number in the set is always the next prime after c: the numbers
    between 1 and the next prime, exclusive, are either equal to or evenly
    divisible by one of the primes less than or equal to c (that's why the
    next prime is prime!), so these numbers drop through the "sieve".  NOTE
    that the next prime, the second number in the set, is the start divisor
    in my algorithm.  My sequence of differences begins with the difference
    between the second and third elements of the set and ends with the
    difference between the first and second elements.

    The remaining numbers in the set are determined by checking the divisors
    of the numbers after the next prime.  If a number is evenly divisible by
    any of the primes less than or equal to c, the number drops through the
    "sieve" and out of the set.  Since the number of elements in a set rises
    rapidly with high c's, this is a time-consuming process.  This process can
    itself be optimized by using the c#k+i method.  My comparable generation
    of a sequence of differences uses a hard-coded skip-through prime of 3;
    as I mentioned above, it didn't seem worth the effort to generalize that
    bit of code for higher STPs.

    Skip-Through Primes
    -------------------

    Here are some example skip-through primes in their c#k+i form.  (In the
    first example, 1 is not prime, but the algorithm will work with the given
    parameters to check every possible divisor, odd *and* even.)

        c = 1    c# = 1
        i in { 1 }

        c = 2    c# = 2
        i in { 1 }

        c = 3    c# = 6
        i in { 1, 5 }

        c = 5    c# = 30
        i in { 1, 7, 11, 13, 17, 19, 23, 29 }

        c = 7    c# = 210		48 elements in set
        i in { 1, 11, 13, 17, 19, 23, 29, 31, ..., 199, 209 }

        c = 11   c# = 2310		480 elements in set
        i in { 1, 13, 17, 19, 23, 29, 31, 37, ..., 2297, 2309 }

        ...

        c = 23   c# = 223,092,870	36,495,360 elements in set
        i in { 1, 29, 31, 37, 41, ..., 223092841, 223092869 }

        c = 29   c# = 6,469,693,230	1,021,870,080 elements in set
        i in { 1, 31, 37, 41, 43, ..., 6469693199, 6469693229 }

    Note that the algorithm has to ignore the very first potential divisor:
    when k is 0 and i is 1, c#k+i = 1 and any number being divisible by 1
    does not mean the number is composite.  Effectively, the first divisor
    checked is the next prime after c, my start divisor.  (Of course, this
    is after the initial check if the target number is equal to or divisible
    by the primes less than or equal to c).

    I just noticed that the difference between the first two elements in a set
    equals the difference between the last two elements, at least up to c=29.
    I don't know if there's a mathematical reason for this or if it holds
    true for all possible sets.

    You might be wondering how the "6k +/- 1" form widely seen on the internet
    relates to the c#k+i scheme of 6k+i for i in { 1, 5 }.  Swapping the
    arithmetic operations, "6k -/+ 1" is shorthand for the +5 term of the
    previous k block and the +1 term of the current k block:

        c = 3    c# = 2*3 = 6    i in { 1, 5 }
        Previous k block: 6(k-1)+5 = 6k-6+5 = 6k-1
        Current k block:  6k+1

    A graphical representation, I guess you would call it, of the c#k+i
    optimization is called wheel factorization
    ( https://en.wikipedia.org/wiki/Wheel_factorization ).  Like my algorithm,
    the algorithm presented on the Wikipedia page (i) explicitly begins
    checking divisors with the next prime after c and (ii) uses the
    differences between divisors rather than the i offsets.

    Sequence Length
    ---------------

    Lastly, a remark upon my formula for the length of a sequence of
    differences:

        seq-length (next-prime) = seq-length (current-prime) * (next-prime - 1)

    As mentioned before, the sequence length is the count (or totient) of the
    numbers in 1..C#-1 that are coprime with C#.  "Euler's product formula"
    ( https://en.wikipedia.org/wiki/Euler%27s_totient_function#Euler's_product_formula )
    can be used to compute the totient for a given number n:

        phi(n) = n * (PI(p|n) (1 - 1/p))

    PI is the multiplicative counterpart of the mathematical summation operator
    and, in this case, produces the product of successive "(1-1/p)"s for all
    primes p that are divisors of n (the meaning of "p|n").  In pseudocode form:

        phi = 1
        for each prime number p that is a divisor of n {
            phi = phi * (1 - 1/p)
        }

    (Note that Euler's formula can be used for arbitrary positive integers,
    but what is of interest in the primality testing context is specifically
    when n is an instance of a c#.)

    With p[k] being a prime number and p[k+1] the next larger prime number,
    my sequence-length formula can be derived from Euler's formula as follows:

	phi(p[k+1]#)	= phi(p[k]#) * p[k+1] * (1 - 1/p[k+1])
			= phi(p[k]#) * p[k+1] * ((p[k+1] - 1)/p[k+1])
			= phi(p[k]#) * (p[k+1] - 1)
			= seq-length (current-prime) * (next-prime - 1)

    I am not a mathematician and my heart sank after reading the first two
    paragraphs on totient's Wikipedia page.
    ( https://en.wikipedia.org/wiki/Euler%27s_totient_function )
    The rest of the page is not for the mathematically faint of heart.
    Fortunately, I really only needed to understand Euler's product formula;
    some ancillary Wikipedia pages on the notation helped me figure out how
    to apply the formula.  I tried it on three skip-through primes:

        5 (5# = 30, phi(30) = 8);
        7 (7# = 210, phi(210) = 48); and
        11 (11# = 2310, phi(2310) = 480).

    The pattern of the calculations became obvious and I could then work out
    the derivation above of my formula after the fact.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  "nbr_util.h"			/* Number utilities. */


/*******************************************************************************
    Prime Sequence of Differences Configuration - configures how nbrIsPrime()
        determines if a number is prime.
*******************************************************************************/

typedef  struct  _PrimeConfig {
    prime_t  skipThroughPrime ;		/* Skip divisors divisible by 2..STP. */
    prime_t  startDivisor ;		/* Start checking with this divisor. */
    size_t  length ;			/* Sequence length. */
    bool  isDynamic ;			/* True = free(3) on destruction. */
    unsigned  char  *differences ;	/* Sequence of differences. */
}  _PrimeConfig, *PrimeConfig ;


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  nbr_util_debug

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED


/*******************************************************************************
    Early Primes - This array contains skip-through primes whose sequence
    length can be represented in unsigned long integers:

        STPs 2..29 plus start divisor 31 (32-bit unsigned long)
        STPs 2..53 plus start divisor 59 (64-bit unsigned long)
        STPs 2..101 plus start divisor 103 (128-bit unsigned long)

    (The start divisor for STP nbr_early_primes[i] is simply the next prime
    number, nbr_early_primes[i+1].)  All this is for completeness' sake
    because the higher skip-through primes are not realistically useful;
    the sequence length for STP 23, for example, is over 36 million.
*******************************************************************************/

    static  const  prime_t  nbr_early_primes[] = {
        2,  3,  5,  7, 11, 13, 17, 19, 23,  29,		/* 32-bit STPs */
       31, 37, 41, 43, 47, 53,				/* 64-bit STPs */
       59, 61, 67, 71, 73, 79, 83, 89, 97, 101,		/* 128-bit STPs */
      103, 0 } ;					/* 101's start divisor */


/*******************************************************************************
    Sequence of Differences for Skip-Through Primes (2, 3, 5, 7, and 11).
*******************************************************************************/


static  const  unsigned  char  nbr_differences_2[] = {
      2, 0
} ;


static  const  unsigned  char  nbr_differences_3[] = {
      2, 4, 0
} ;


static  const  unsigned  char  nbr_differences_5[] = {
      4, 2, 4, 2, 4, 6, 2, 6, 0
} ;


static  const  unsigned  char  nbr_differences_7[] = {
      2, 4, 2, 4, 6, 2, 6, 4, 2, 4, 6, 6, 2, 6, 4, 2,
      6, 4, 6, 8, 4, 2, 4, 2, 4, 8, 6, 4, 6, 2, 4, 6,
      2, 6, 6, 4, 2, 4, 6, 2, 6, 4, 2, 4, 2, 10, 2, 10, 0
} ;


#define  MAX_SKIP_THROUGH_PRIME_INTERNAL  11
static  const  unsigned  char  nbr_differences_11[] = {
      4,  2,  4,  6,  2,  6,  4,  2,  4,  6,  6,  2,  6,  4,  2,  6,
      4,  6,  8,  4,  2,  4,  2,  4, 14,  4,  6,  2, 10,  2,  6,  6,
      4,  2,  4,  6,  2, 10,  2,  4,  2, 12, 10,  2,  4,  2,  4,  6,
      2,  6,  4,  6,  6,  6,  2,  6,  4,  2,  6,  4,  6,  8,  4,  2,
      4,  6,  8,  6, 10,  2,  4,  6,  2,  6,  6,  4,  2,  4,  6,  2,
      6,  4,  2,  6, 10,  2, 10,  2,  4,  2,  4,  6,  8,  4,  2,  4,
     12,  2,  6,  4,  2,  6,  4,  6, 12,  2,  4,  2,  4,  8,  6,  4,
      6,  2,  4,  6,  2,  6, 10,  2,  4,  6,  2,  6,  4,  2,  4,  2,
     10,  2, 10,  2,  4,  6,  6,  2,  6,  6,  4,  6,  6,  2,  6,  4,
      2,  6,  4,  6,  8,  4,  2,  6,  4,  8,  6,  4,  6,  2,  4,  6,
      8,  6,  4,  2, 10,  2,  6,  4,  2,  4,  2, 10,  2, 10,  2,  4,
      2,  4,  8,  6,  4,  2,  4,  6,  6,  2,  6,  4,  8,  4,  6,  8,
      4,  2,  4,  2,  4,  8,  6,  4,  6,  6,  6,  2,  6,  6,  4,  2,
      4,  6,  2,  6,  4,  2,  4,  2, 10,  2, 10,  2,  6,  4,  6,  2,
      6,  4,  2,  4,  6,  6,  8,  4,  2,  6, 10,  8,  4,  2,  4,  2,
      4,  8, 10,  6,  2,  4,  8,  6,  6,  4,  2,  4,  6,  2,  6,  4,
      6,  2, 10,  2, 10,  2,  4,  2,  4,  6,  2,  6,  4,  2,  4,  6,
      6,  2,  6,  6,  6,  4,  6,  8,  4,  2,  4,  2,  4,  8,  6,  4,
      8,  4,  6,  2,  6,  6,  4,  2,  4,  6,  8,  4,  2,  4,  2, 10,
      2, 10,  2,  4,  2,  4,  6,  2, 10,  2,  4,  6,  8,  6,  4,  2,
      6,  4,  6,  8,  4,  6,  2,  4,  8,  6,  4,  6,  2,  4,  6,  2,
      6,  6,  4,  6,  6,  2,  6,  6,  4,  2, 10,  2, 10,  2,  4,  2,
      4,  6,  2,  6,  4,  2, 10,  6,  2,  6,  4,  2,  6,  4,  6,  8,
      4,  2,  4,  2, 12,  6,  4,  6,  2,  4,  6,  2, 12,  4,  2,  4,
      8,  6,  4,  2,  4,  2, 10,  2, 10,  6,  2,  4,  6,  2,  6,  4,
      2,  4,  6,  6,  2,  6,  4,  2, 10,  6,  8,  6,  4,  2,  4,  8,
      6,  4,  6,  2,  4,  6,  2,  6,  6,  6,  4,  6,  2,  6,  4,  2,
      4,  2, 10, 12,  2,  4,  2, 10,  2,  6,  4,  2,  4,  6,  6,  2,
     10,  2,  6,  4, 14,  4,  2,  4,  2,  4,  8,  6,  4,  6,  2,  4,
      6,  2,  6,  6,  4,  2,  4,  6,  2,  6,  4,  2,  4, 12,  2, 12, 0
} ;
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrIsPrime ()

    Check If a Number is a Prime Number.


Purpose:

    Function nbrIsPrime() returns true if a given number is a prime number
    and false otherwise.

    This version of the function, the default version, is an implementation
    of the generalized, skip-through primes algorithm described in the file
    prologue.  A sequence of differences is used to skip divisors evenly
    divisible by primes in the range 2 through the skip-through prime.
    Different skip-through primes and sequences of differences can be
    substituted for the default by passing in a configuration object
    returned by nbrPrimeConfigCreate().  If the configuration argument
    is NULL, the default configuration of skip-through prime 7 and its
    48-element sequence of differences results in divisors divisible by
    2, 3, 5, and 7 being skipped.

        NOTE:  The configuration argument is a (void *) pointer, which
        is cast to an nbrIsPrime()-specific PrimeConfig pointer.  The
        (void *) pointer is used instead of an opaque pointer so that
        applications which call nbrNextPrimeF() and supply their own
        is-prime function can pass in pointers to application-specific
        configuration objects.  When such an application calls
        nbrNextPrimeF(), that function will pass the configuration
        argument on to the application's is-prime function.


    Invocation:

        isPrime = nbrIsPrime (number, configuration) ;

    where

        <number>	- I
            is the number to be checked.
        <configuration>	- I
            is the handle for a configuration object returned by
            nbrPrimeConfigCreate().  The configuration parameters will
            be used in the algorithm that determines if a number is prime.
            If this argument is NULL, a built-in default configuration
            will be used.
        <isPrime>	- O
            returns true if the number is a prime number and false if not.

*******************************************************************************/


bool  nbrIsPrime (

#    if PROTOTYPES
        prime_t  number,
        void  *configv)
#    else
        number, configv)

        prime_t  number ;
        void  *configv ;
#    endif

{    /* Local variables. */
    PrimeConfig  config = (PrimeConfig) configv ;
    size_t  i ;
    prime_t  divisor = 0, root ;

    static  const  _PrimeConfig  defaultConfig = {
        7,			/* Skip-through prime. */
        11,			/* Start divisor. */
        48,			/* Sequence length. */
        false,			/* Sequence is statically allocated. */
				/* The Sequence. */
        (unsigned char *) nbr_differences_7
    } ;



    if (number < 2)  return (false) ;	/* 0 and 1 are NOT prime. */
    if (number < 4)  return (true) ;	/* 2 and 3 are prime. */

/* If a custom configuration was not supplied, use the default configuration. */

    if (config == NULL)  config = (PrimeConfig) &defaultConfig ;

/* Check if the number is equal to or evenly divisible by the prime numbers
   less than or equal to the skip-through prime.  For example, if the
   skip-through prime is 7, then check (i) if the number is equal to 2,
   3, 5, or 7, and (ii) if the number is divisible by 2, 3, 5, or 7. */

    for (i = 0 ;  nbr_early_primes[i] <= config->skipThroughPrime ;  i++) {
        if (number == nbr_early_primes[i])  return (true) ;
        if ((number % nbr_early_primes[i]) == 0)  return (false) ;
        divisor = nbr_early_primes[i+1] ;	/* Eventual start divisor. */
    }

/* Compute the square root of the number.  Divisors up through the square
   root will be checked; if no divisor is found, the number is prime.  As I
   discussed in the LIMIT CHECKING section of the file prologue, there is a
   danger of overflow when using the commonly used test, "div*div <= n",
   to control the division loop.  If we know the integer square root of the
   number, we can use a safe, faster "div <= root" test.  A quick call to
   nbrUnsignedSQRT() returns floor[sqrt(n)], which is exactly what we need.
   If this code is running on a platform that doesn't support
   nbrUnsignedSQRT() (i.e., integers are not in standard binary
   representation), a rough estimate of the square root is made, an estimate
   that is always less than or equal to the actual square root.  After the
   divisor reaches the rough estimate of the square root, the function
   switches to the safe "div <= n/div" test and slowly creeps forward until
   the divisor passes the actual square root. */

#if defined(HAVE_FAST_INTEGER_SQRT) && !HAVE_FAST_INTEGER_SQRT
    root = number / 2 ;			/* Rough estimate of square root. */
    while (root > (number / root)) {
        root = root / 2 ;
    }
#else
    root = (prime_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
#endif
    LGI "(nbrIsPrime) # = %"PRIuPRIME_T"  root %"PRIuPRIME_T"\n",
        number, root) ;


/*******************************************************************************
    Using the sequence of differences, check for possible divisors.  If the
    number's divisors are only itself and 1, the number is prime.  If some
    other divisor divides the number evenly, the number is composite.
*******************************************************************************/

#if defined(HAVE_FAST_INTEGER_SQRT) && !HAVE_FAST_INTEGER_SQRT
    while (divisor <= (number / divisor)) {
#else
    while (divisor <= root) {
#endif

        for (i = 0 ;  config->differences[i] > 0 ;  i++) {
            LGI "(nbrIsPrime) [%"PRIuSIZE_T"] %"PRIuPRIME_T" %% %"PRIuPRIME_T" = %"PRIuPRIME_T"\n",
                i, number, divisor, number % divisor) ;
            if ((number % divisor) == 0)	/* Evenly divisible by divisor? */
                return (false) ;		/* NOT prime. */
            divisor += config->differences[i] ;
#if defined(HAVE_FAST_INTEGER_SQRT) && !HAVE_FAST_INTEGER_SQRT
            if (divisor < root)  continue ;
            if (divisor > (number / divisor))  break ;
#else
            if (divisor > root)  break ;
#endif
        }

    }


    LGI "(nbrIsPrime) %"PRIuPRIME_T" is prime.\n", number) ;

    return (true) ;		/* No divisors found--number is prime! */

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if defined(NBR_SKIP_2_3_HARDCODED) && NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrIsPrime ()

    Check If a Number is a Prime Number (Hard-coded 2-3 Optimization).


Purpose:

    Function nbrIsPrime() returns true if a given number is a prime number
    and false otherwise.

    This version of the function uses a hard-coded loop that skips divisors
    divisible by 2 or by 3.  In order to use this low-memory-footprint
    function, this source file must be compiled with the C preprocessor
    symbol, "-DNBR_SKIP_2_3_HARDCODED=1".

        NOTE:  This source file is compiled with either this version of
        nbrIsPrime() or the generalized version above, but not both at the
        same time.  This small-footprint version of the function is probably
        slower because it uses the more expensive "div <= (num/div)" test
        for limits checking instead of the faster comparison of the divisor
        to the integer square root of the number.


    Invocation:

        isPrime = nbrIsPrime (number, configuration) ;

    where

        <number>	- I
            is the number to be checked.
        <configuration>	- I
            is ignored; NULL is a good value.  (The procedure for determining
            if a number is prime is hard-coded in this version of the function
            and can't be configured.)
        <isPrime>	- O
            returns true if the number is a prime number and false if not.

*******************************************************************************/


bool  nbrIsPrime (

#    if PROTOTYPES
        prime_t  number,
        void  *configv)
#    else
        number, config)

        prime_t  number ;
        void  *configv ;
#    endif

{    /* Local variables. */
    prime_t  divisor ;



    LGI "(nbrIsPrime) # = %"PRIuPRIME_T"  %%2 = %"PRIuPRIME_T"  %%3 = %"PRIuPRIME_T"\n",
        number, number % 2, number % 3) ;

    if (number < 2)  return (false) ;	/* 0 and 1 are NOT prime. */
    if (number < 4)  return (true) ;	/* 2 and 3 are prime. */
    if ((number % 2) == 0)
        return (false) ;		/* Even numbers > 2 are NOT prime. */
    if ((number % 3) == 0)
        return (false) ;		/* Numbers divisible by 3 are NOT prime. */

/* Check for possible divisors.  The "divisor <= (number / divisor)" test
   is similar to checking 5 .. sqrt(N) as possible divisors, but avoids
   the need for linking to the math library.  Note that the alternate form,
   "(divisor * divisor) <= number", introduces the possibility of overflow;
   i.e., when "(divisor * divisor) > PRIME_T_MAX". */

    for (divisor = 5 ;  divisor <= (number / divisor) ;  divisor += 4) {
        if ((number % divisor) == 0) {
            return (false) ;			/* NOT prime. */
        }
        divisor += 2 ;
        if (((number % divisor) == 0) &&	/* Evenly divisible by divisor? */
            (divisor < number)) {		/* Not dividing number by number? */
            return (false) ;			/* NOT prime. */
        }
    }

    LGI "(nbrIsPrime) %"PRIuPRIME_T" is prime.\n", number) ;

    return (true) ;		/* No divisors found--number is prime! */

}
#endif	/* NBR_SKIP_2_3_HARDCODED */

/*!*****************************************************************************

Procedure:

    nbrNextPrime ()

    Determine the Next Prime Number Following a Given Value.


Purpose:

    Function nbrNextPrime() searches for the first prime number greater than
    a given number.


    Invocation:

        prime = nbrNextPrime (number, configuration) ;

    where

        <number>	- I
            is the number after which the search begins; i.e., this function
            begins searching for the next prime number at number plus one.
        <configuration>	- I
            is the handle for a configuration object returned by
            nbrPrimeConfigCreate().  The configuration parameters will
            be used in the algorithm that determines if a number is prime.
            If this argument is NULL, a built-in default configuration
            will be used.
        <prime>		- O
            returns the next prime number greater than the input number.
            Zero (not prime) is returned if the search reaches PRIME_T_MAX
            without finding the next prime number; i.e., stepping to the
            next number to be tested would cause an overflow.

*******************************************************************************/


prime_t  nbrNextPrime (

#    if PROTOTYPES
        prime_t  number,
        void  *configv)
#    else
        number, configv)

        prime_t  number ;
        void  *config ;
#    endif

{

    return (nbrNextPrimeF (number, NULL, configv)) ;

}

/*!*****************************************************************************

Procedure:

    nbrNextPrimeF ()

    Determine, Using a Caller-Supplied Check-Prime Function, the Next Prime
    Number Following a Given Value.


Purpose:

    Function nbrNextPrimeF() searches for the first prime number greater than
    a given number.  A caller-supplied function is used to check if a number
    is prime.  If no function is specified, nbrNextPrimeF() calls nbrIsPrime().


    Invocation:

        prime = nbrNextPrimeF (number, isPrimeF, config) ;

    where

        <number>	- I
            is the number after which the search begins; i.e., this function
            begins searching for the next prime number at number plus one.
        <isPrimeF>	- I
            is a pointer to a function that checks if a number is prime.
            The function accepts an "unsigned long" number to be checked
            and returns a "bool" value indicating if the number is or is
            not prime.  If no function is specified, nbrNextPrimeF() calls
            nbrIsPrime().
        <configuration>	- I
            is a (void *) pointer used to pass configuration parameters
            to the function that determines if a number is prime.  The
            meaning of the pointer is up to the calling program and the
            caller-supplied isPrime() function.  If the caller specifies
            NULL for the isPrime() function argument, thus indicating that
            it wants nbrNextPrimeF() to use NBR_UTIL's nbrIsPrime(), then
            this argument should be NULL (default configuration) or a
            configuration handle returned by nbrPrimeConfigCreate().
        <prime>		- O
            returns the next prime number greater than the input number.
            Zero (not prime) is returned if the search reaches PRIME_T_MAX
            without finding the next prime number; i.e., stepping to the
            next number to be tested would cause an overflow.

*******************************************************************************/


prime_t  nbrNextPrimeF (

#    if PROTOTYPES
        prime_t  number,
        IsPrimeFunc  isPrimeF,
        void  *configv)
#    else
        number, isPrimeF, configv)

        prime_t  number ;
        IsPrimeFunc  isPrimeF ;
        void  *configv ;
#    endif

{

/* Check the edge cases. */

    if (number < 2)
        return (2) ;
    else if (number < 3)
        return (3) ;

/* Start searching at the next odd number following the input number. */

    if ((number % 2) == 0)
        number++ ;
    else
        number += 2 ;

/* Check each odd number.  If no function was specified for checking if
   a number is prime, call NBR_UTIL's own nbrIsPrime() function. */

    if (isPrimeF == NULL)  isPrimeF = nbrIsPrime ;

    while (!isPrimeF (number, configv)) {
        if (number >= (PRIME_T_MAX - 1)) {	/* "+= 2" will overflow. */
            SET_ERRNO (EOVERFLOW) ;
            LGE "(nbrNextPrimeF) Next prime > %"PRIuPRIME_T": ", PRIME_T_MAX) ;
            return (0) ;
        }
        number += 2 ;
    }

    return (number) ;		/* Prime number found! */

}

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrPrimeConfigCreate ()

    Create a Prime Determination Configuration.


Purpose:

    Function nbrPrimeConfigCreate() creates a configuration object that
    can be passed to nbrIsPrime() to speed-tune the process of determining
    if a number is prime.  Internally, the NBR_UTIL package has its own
    sequences of differences for skip-through primes 2, 3, 5, 7, and 11.
    If an application wishes to use other than the default skip-through
    prime or wants to use a higher STP to test very large numbers, it can
    specify its desired skip-through prime and, if necessary, supply the
    STP's sequence of differences.


    Invocation:

        configuration = nbrPrimeConfigCreate (skipThroughPrime,
                                              differences, isDynamic) ;

    where

        <skipThroughPrime>	- I
            is the divisor for which a corresponding sequence of differences
            would skip potential divisors divisible by any divisor in the
            range [2..<prime>].  (If this argument is not prime, the function
            will choose the largest prime number less than the argument.)
        <differences>		- I
            is the caller-supplied sequence of differences for the
            skip-through prime.  Following the last difference in the
            sequence, there should be a terminating zero.  If the STP
            is 2, 3, 5, 7, or 11 (MAX_SKIP_THROUGH_PRIME_INTERNAL),
            nbrIsPrime() will use one of the internal sequences compiled
            into the NBR_UTIL package.  In that case, specify NULL for
            this argument.
        <isDynamic>		- I
            specifies if the caller-supplied sequence of differences was
            dynamically allocated or not.  If the sequence was dynamically
            allocated, it will be free(3)ed when nbrPrimeConfigDestroy()
            is called.  This argument is ignored if nbrIsPrime() will be
            using an internal sequence.
        <configuration>		- O
            returns a handle for the configuration object; the handle can be
            passed into nbrIsPrime() and nbrNextPrime().  NULL is returned
            in the event of an error and ERRNO is set.  Possible errors in
            addition to system errors are (i) choosing a skip-through prime
            for which the length of the sequence of differences would exceed
            the maximum representable value of type size_t and (ii) not
            supplying a sequence of differences for skip-through primes
            larger than 11.

*******************************************************************************/


void  *nbrPrimeConfigCreate (

#    if PROTOTYPES
        prime_t  skipThroughPrime,
        unsigned  char  *differences,
        bool  isDynamic)
#    else
        skipThroughPrime, differences, isDynamic)

        prime_t  skipThroughPrime ;
        unsigned  char  *differences ;
        bool  isDynamic ;
#    endif

{    /* Local variables. */
    PrimeConfig  config ;



/* Create the configuration object. */

    config = (_PrimeConfig *) malloc (sizeof (_PrimeConfig)) ;
    if (config == NULL) {
        LGE "(nbrPrimeConfigCreate) Error allocation configuration object for skip-through prime %"PRIuPRIME_T".\nmalloc: ",
            skipThroughPrime) ;
        return (NULL) ;
    }

    config->differences = NULL ;

/* Compute the length of the skip-through prime's sequence. */

    config->length = nbrPrimeSeqDiffsLength (skipThroughPrime,
                                             &config->skipThroughPrime,
                                             &config->startDivisor) ;
    if (config->length == 0) {
        LGE "(nbrPrimeConfigCreate) Skip-through prime %"PRIuPRIME_T"'s sequence length is too big.\n",
            skipThroughPrime) ;
        PUSH_ERRNO ;  free (config) ;  POP_ERRNO ;
        return (NULL) ;
    }

/* If the skip-through prime's sequence of differences is already compiled
   into the NBR_UTIL package, then use the internal sequence and ignore
   whatever the caller passed in. */

    if (config->skipThroughPrime <= MAX_SKIP_THROUGH_PRIME_INTERNAL) {

        config->isDynamic = false ;

        switch (config->skipThroughPrime) {
        case 11:
            config->differences = (unsigned char *) nbr_differences_11 ;
            break ;
        case 7:
            config->differences = (unsigned char *) nbr_differences_7 ;
            break ;
        case 5:
            config->differences = (unsigned char *) nbr_differences_5 ;
            break ;
        case 2:
            config->differences = (unsigned char *) nbr_differences_2 ;
            break ;
        case 3:
        default:
            config->skipThroughPrime = 3 ;
            config->startDivisor = 5 ;
            config->length = 2 ;
            config->differences = (unsigned char *) nbr_differences_3 ;
            break ;
        }

    }

/* If the skip-through prime's sequence of differences is not available
   internally and the caller didn't provide a sequence, then generate
   the sequence on-the-fly.  This can be time-consuming for larger
   skip-through primes. */

    else if (differences == NULL) {

        config->isDynamic = true ;

        config->differences = nbrPrimeSeqDiffs (config->skipThroughPrime,
                                                NULL, NULL) ;
        if (config->differences == NULL) {
            LGE "(nbrPrimeConfigCreate) Error generating sequence for skip-through prime %"PRIuPRIME_T".\nnbrPrimeSeqDiffs: ",
                config->skipThroughPrime) ;
            PUSH_ERRNO ;  free (config) ;  POP_ERRNO ;
            return (NULL) ;
        }

    }

/* Otherwise, use the sequence of differences supplied by the caller. */

    else {

        config->isDynamic = isDynamic ;
        config->differences = differences ;

    }


    LGI "(nbrPrimeConfigCreate) Configuration %p\n", (void *) config) ;
    LGI "(nbrPrimeConfigCreate) Skip-Through Prime = %"PRIuPRIME_T"  Start Divisor = %"PRIuPRIME_T"\n",
        config->skipThroughPrime, config->startDivisor) ;
    LGI "(nbrPrimeConfigCreate) Length = %"PRIuSIZE_T"  Diffs = %p\n",
        config->length, config->differences) ;

    return ((void *) config) ;

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrPrimeConfigDestroy ()

    Destroy a Prime Determination Configuration.


Purpose:

    Function nbrPrimeConfigDestroy() destroys a configuration object
    created by nbrPrimeConfigCreate().


    Invocation:

        status = nbrPrimeConfigDestroy (configuration) ;

    where:

        <configuration>	- I
            is the configuration handle returned by nbrPrimeConfigCreate().
        <status>	- O
            returns the status of deleting the configuration object,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  nbrPrimeConfigDestroy (

#    if PROTOTYPES
        void  *configv)
#    else
        configv)

        void  *configv ;
#    endif

{    /* Local variables. */
    PrimeConfig  config = configv ;



    if (config == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(nbrPrimeConfigDestroy) NULL configuration handle: ") ;
        return (errno) ;
    }

    LGI "(nbrPrimeConfigDestroy) Deleting configuration %p, skip-through prime %"PRIuPRIME_T".\n",
        (void *) config, config->skipThroughPrime) ;

/* Delete the sequence of differences and then the configuration object. */

    if (config->isDynamic && (config->differences != NULL)) {
        free (config->differences) ;
    }

    free (config) ;

    return (0) ;

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrPrimeSeqDiffs ()

    Generate a Sequence of Differences for a Skip-Through Prime.


Purpose:

    Function nbrPrimeSeqDiffs() generates the sequence of differences
    for a given skip-through prime.

        WARNING:  The lengths of the sequences rise very rapidly with
            increasing skip-through primes.  Obviously, this could pose
            a memory problem.  Less obviously, the higher the skip-through
            prime and the longer the sequence, the more time it takes to
            calculate the differences in the sequence.  Using
            nbrPrimeSeqDiffsMax() (which calculates the differences
            without allocating the memory), my aging computer took half
            a second to calculate the 1.7 million differences for
            skip-through prime 19, 11 seconds for 23's 36 million, and
            almost 7 minutes for 29's one billion differences.  Calculating
            the one trillion differences for skip-through prime 37 required
            over 6 days of 99% CPU usage!


    Invocation:

        differences = nbrPrimeSeqDiffs (skipThroughPrime,
                                        &startDivisor,
                                        &seqLength) ;

    where

        <skipThroughPrime>	- I
            is the divisor for which a corresponding sequence of differences
            would skip potential divisors divisible by any divisor in the
            range [2..<prime>].  (If this argument is not prime, the function
            will choose the largest prime number less than the argument.)
        <startDivisor>		- O
            returns the next prime number after the skip-through prime.
            If the caller is not interested in this value, a NULL address
            can be specified.  The start divisor is "synchronized" with the
            start of the sequence of differences.  An IsPrime() function
            should first check that its target number is not divisible by
            any divisors in the range [2..<skip-through prime>].  Then,
            the function should cycle through the sequence of differences,
            beginning with the start divisor and the first difference in
            the sequence.  The target number is tested against the stream
            of divisors until a divisor greater than the square root of the
            target number is reached.
        <length>	- O
            returns the number of elements in the sequence of differences
            for the skip-through prime.  NOTE that this does NOT include
            the terminating zero byte added by nbrPrimeSeqDiffs().  Zero
            is returned if the length overflows the maximum representable
            size (SIZE_MAX for type "size_t").  If type "size_t" is an
            unsigned 64-bit integer, the sequence length for skip-through
            prime 53 is correctly calculated at 4 quintillion-plus (or
            trillion in the long-scale naming system).  Calculating the
            sequence length for the next skip-through prime, 59, fails
            when the computed length would exceed the maximum representable
            size (18 quintillion in this case).
        <differences>	- O
            returns a dynamically allocated, zero-terminated sequence of
            differences for the skip-through prime; NULL is returned in
            the event of an error.  The caller is responsible for free(3)ing
            the memory when the sequence is no longer needed.

*******************************************************************************/


unsigned  char  *nbrPrimeSeqDiffs (

#    if PROTOTYPES
        prime_t  skipThroughPrime,
        prime_t  *startDivisor,
        size_t  *seqLength)
#    else
        skipThroughPrime, startDivisor, seqLength)

        prime_t  skipThroughPrime ;
        prime_t  *startDivisor ;
        size_t  *seqLength ;
#    endif

{    /* Local variables. */
    size_t  i, length ;
    unsigned  char  *differences ;
    prime_t  divisor, lastNumber, number ;



    length = nbrPrimeSeqDiffsLength (skipThroughPrime,
                                     &skipThroughPrime,
                                     &lastNumber) ;
    if (length == 0) {
        LGE "(nbrPrimeSeqDiffs) Skip-through prime %"PRIuPRIME_T" too large.\n",
            skipThroughPrime) ;
        return (NULL) ;
    }

    if (startDivisor != NULL)  *startDivisor = lastNumber ;
    if (seqLength != NULL)  *seqLength = length ;

    differences = malloc (length + 1) ;
    if (differences == NULL) {
        LGE "(nbrPrimeSeqDiffs) Unable to allocate %"PRIuSIZE_T"-byte array.\nmalloc: ",
            length + 1) ;
        return (NULL) ;
    }

    number = lastNumber ;	/* Do-while loop below will increment to # after start divisor. */
    i = 0 ;

    while (i < length) {

        do {
            number += 2 ;
            divisor = 3 ;
            if ((number % divisor) == 0)  continue ;
            for (divisor = 5 ;  divisor <= skipThroughPrime ;  divisor += 4) {
                if ((number % divisor) == 0)  break ;
                divisor += 2 ;
                if ((number % divisor) == 0)  break ;
            }
        } while (divisor <= skipThroughPrime) ;

        if (divisor > skipThroughPrime) {
            differences[i++] = (unsigned char) (number - lastNumber) ;
            lastNumber = number ;
        }

    }

    differences[length] = 0 ;		/* Sequence terminator. */

    return (differences) ;

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrPrimeSeqDiffsLength ()

    Calculate the Length of a Sequence of Differences.


Purpose:

    Function nbrPrimeSeqDiffsLength() calculates and returns the number of
    elements in the sequence of differences for a given skip-through prime.

    The lengths of sequences of differences rise very rapidly.  For
    example:

        skip-through prime     length
        ------------------     ------
               13               5,760
               17              92,160
               19             1 million+
               23            36 million+
               29             1 billion+


    Invocation:

        length = nbrPrimeSeqDiffsLength (skipThroughNumber,
                                         &skipThroughPrime,
                                         &startDivisor) ;

    where

        <skipThroughNumber>	- I
            is the divisor for which a corresponding sequence of differences
            would skip potential divisors divisible by any divisor in the
            range [2..<prime>].  (If this argument is not prime, the function
            will choose the largest prime number less than the argument.)
        <skipThroughPrime>	- O
            returns the skip-through prime; the sequence of differences
            skips potential divisors divisible by any divisor in the range
            [2..<prime>].  If the first argument is already prime, this
            argument returns the same number.  If the caller is not
            interested in this value, a NULL address can be specified.
        <startDivisor>		- O
            returns the next prime number after the skip-through prime.
            If the caller is not interested in this value, a NULL address
            can be specified.  The start divisor is "synchronized" with the
            start of the sequence of differences.  An IsPrime() function
            should first check that its target number is not divisible by
            any divisors in the range [2..<skip-through prime>].  Then,
            the function should cycle through the sequence of differences,
            beginning with the start divisor and the first difference in
            the sequence.  The target number is tested against the stream
            of divisors until a divisor greater than the square root of the
            target number is reached.
        <length>	- O
            returns the number of elements in the sequence of differences
            for the skip-through prime.  NOTE that this does NOT include
            the terminating zero byte added by nbrPrimeSeqDiffs().  Zero
            is returned if the length overflows the maximum representable
            size (SIZE_MAX for type "size_t").  If type "size_t" is an
            unsigned 64-bit integer, the sequence length for skip-through
            prime 53 is correctly calculated at 4 quintillion-plus (or
            trillion in the long-scale naming system).  Calculating the
            sequence length for the next skip-through prime, 59, fails
            when the computed length would exceed the maximum representable
            size (18 quintillion in this case).

*******************************************************************************/


size_t  nbrPrimeSeqDiffsLength (

#    if PROTOTYPES
        prime_t  skipThroughNumber,
        prime_t  *skipThroughPrime,
        prime_t  *startDivisor)
#    else
        skipThroughNumber, *skipThroughPrime, *startDivisor)

        prime_t  skipThroughNumber ;
        prime_t  *skipThroughPrime ;
        prime_t  *startDivisor ;
#    endif

{    /* Local variables. */
    size_t  i, length ;
    prime_t  currentPrime, nextPrime ;



    currentPrime = 2 ;
    length = 1 ;
    nextPrime = 3 ;
    i = 2 ;		/* Index of next prime after 3; assumes at
			   least 2,3,5 or 2,3,0 in early primes array. */

    while (nextPrime <= skipThroughNumber) {
		/* First check that "current-length * (nextPrime - 1)"
		   will not exceed the maximum size_t value. */
        if (length > (SIZE_MAX / (nextPrime - 1))) {
            SET_ERRNO (EOVERFLOW) ;
            LGE "(nbrPrimeSeqDiffsLength) Integer overflow on length %"PRIuSIZE_T", next prime %"PRIuPRIME_T".\n",
                length, nextPrime) ;
            length = 0 ;
            break ;
        }
        length = length * (nextPrime - 1) ;
        currentPrime = nextPrime ;
        nextPrime = nbr_early_primes[i] ;
        if (nextPrime == 0) {
            nextPrime = nbrNextPrime (currentPrime, NULL) ;
            if (nextPrime == 0) {
                SET_ERRNO (EOVERFLOW) ;
                LGE "(nbrPrimeSeqDiffsLength) Integer overflow on next prime, current prime %"PRIuPRIME_T".\n",
                    currentPrime) ;
                length = 0 ;
                break ;
            }
        } else {
            i++ ;
        }
    }

    if (skipThroughPrime != NULL)  *skipThroughPrime = currentPrime ;
    if (startDivisor != NULL)  *startDivisor = nextPrime ;

    LGI "(nbrPrimeSeqDiffsLength) Skip-Through # = %"PRIuPRIME_T"  Length = %"PRIuSIZE_T"  Prime = %"PRIuPRIME_T"  Start = %"PRIuPRIME_T"\n",
        skipThroughNumber, length, currentPrime, nextPrime) ;

    return (length) ;

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrPrimeSeqDiffsMax ()

    Determine the Maximum Difference in a Sequence of Differences.


Purpose:

    Function nbrPrimeSeqDiffsMax() determines the maximum difference in the
    sequence of differences for a given skip-through prime.

    This function uses the same alorithm as and is virtually identical to
    nbrPrimeSeqDiffs(), with one exception.  This functon doesn't allocate
    memory for the sequence of differences.  Instead, it generates the next
    difference, examines the difference, updates a running maximum difference
    variable if necessary, and discards the difference.  Since the functions
    are virtually identical, changes to nbrPrimeSeqDiffs() should be applied
    to nbrPrimeSeqDiffsMax() as well.


    Invocation:

        maximum = nbrPrimeSeqDiffsMax (skipThroughPrime) ;

    where

        <skipThroughPrime>	- I
            is the divisor for which a corresponding sequence of differences
            would skip potential divisors divisible by any divisor in the
            range [2..<prime>].  (If this argument is not prime, the function
            will choose the largest prime number less than the argument.)
        <maximum>		- O
            returns the maximum difference in the sequence of differences
            for the skip-through prime.  Zero is returned if the length
            of the sequence overflows the maximum representable size
            (SIZE_MAX for type "size_t").

*******************************************************************************/


prime_t  nbrPrimeSeqDiffsMax (

#    if PROTOTYPES
        prime_t  skipThroughPrime)
#    else
        skipThroughPrime)

        prime_t  skipThroughPrime ;
#    endif

{    /* Local variables. */
    size_t  i, length ;
    prime_t  diff, divisor, lastNumber, maxDiff, number ;



    length = nbrPrimeSeqDiffsLength (skipThroughPrime,
                                     &skipThroughPrime,
                                     &lastNumber) ;
    if (length == 0) {
        LGE "(nbrPrimeSeqDiffsMax) Skip-through prime %"PRIuPRIME_T" too large.\n",
            skipThroughPrime) ;
        return (0) ;
    }

/* Start calculating differences with the next odd number after the start
   divisor returned by nbrPrimeSeqDiffsLength().  This would be LASTNUMBER+2,
   but let the plus-two increment take place at the beginning of the do-while
   loop below. */

    number = lastNumber ;	/* Do-while loop below will increment to # after start divisor. */

    maxDiff = 0 ;
    i = 0 ;

    while (i++ < length) {

        do {
            number += 2 ;
            divisor = 3 ;
            if ((number % divisor) == 0)  continue ;
            for (divisor = 5 ;  divisor <= skipThroughPrime ;  divisor += 4) {
                if ((number % divisor) == 0)  break ;
                divisor += 2 ;
                if ((number % divisor) == 0)  break ;
            }
        } while (divisor <= skipThroughPrime) ;

        if (divisor > skipThroughPrime) {
            diff = number - lastNumber ;
            if (diff > maxDiff) {
                maxDiff = diff ;
                LGI "(nbrPrimeSeqDiffsMax) Number = %"PRIuPRIME_T"  Max Diff = %"PRIuPRIME_T"\n",
                    number, maxDiff) ;
            }
            lastNumber = number ;
        }

/* Periodically print a debug line showing the program is still alive! */

#define  DIFFS_INTERVAL  (100UL * 1000UL * 1000UL)
        if ((i % DIFFS_INTERVAL) == 0) {
            LGI "(nbrPrimeSeqDiffsMax) [%5.1f%%] # = %20"PRIuPRIME_T"\n",
                ((double) i / (double) length) * 100.0, number) ;
        }

    }

    LGI "(nbrPrimeSeqDiffsMax) Maximum Difference = %"PRIuPRIME_T"\n", maxDiff) ;

    return (maxDiff) ;

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */

#if !defined(NBR_SKIP_2_3_HARDCODED) || !NBR_SKIP_2_3_HARDCODED
/*!*****************************************************************************

Procedure:

    nbrPrimeSeqDiffsRange ()

    Determine the Range of Divisors in a Sequence of Differences.


Purpose:

    Function nbrPrimeSeqDiffsRange() determines the range of divisors
    encompassed by a single pass through a sequence of differences.
    In short, the function returns the sum of all the differences in
    the sequence.

    For example, the 8-element sequence for skip-through prime 5 is:

        4, 2, 4, 2, 4, 6, 2, 6

    Beginning with 5's start divisor, 7, nbrIsPrime() will check the
    following numbers as potential divisors of its argument in the first
    two passes through the sequence:

        (1)     7, 11, 13, 17, 19, 23, 29, 31 => 37
        (2)    37, 41, 43, 47, 49, 53, 59, 61 => 67
        ...

    As can be seen, the range of the sequence of differences for skip-through
    prime 5 is 30 (37-7, 67-37, etc.).  If nbrIsPrime() is called to check an
    actual prime number, X, the number of iterations over the sequence will
    be (sqrt(X)-7+30-1)/30.  More generally, the number of iterations is:

        (sqrt(X) - start-divisor + range - 1) / range

    I originally computed the range by actually generating the sequence of
    differences and adding them all together.  I later noticed a pattern
    that allows the range to be easily determined without generating the
    sequence:

        range (next-prime) = range (current-prime) * next-prime

    Beginning with a current prime of 2, whose range is 2, loop up until
    the skip-through prime is reached.  Even later, I learned a more concise
    definition of range is simply the product of all the primes less than or
    equal to the skip-through prime.  For skip-through prime 11, the range is
    2*3*5*7*11, or 2,310.  (See the mathematics section in the file prologue.)


    Invocation:

        range = nbrPrimeSeqDiffsRange (skipThroughPrime) ;

    where

        <skipThroughPrime>	- I
            is the divisor for which a corresponding sequence of differences
            would skip potential divisors divisible by any divisor in the
            range [2..<prime>].  (If this argument is not prime, the function
            will choose the largest prime number less than the argument.)
        <range>			- O
            returns the range of divisors in the sequence of differences
            for the skip-through prime.  Zero is returned if the length
            of the sequence overflows the maximum representable size
            (SIZE_MAX for type "size_t").

*******************************************************************************/


prime_t  nbrPrimeSeqDiffsRange (

#    if PROTOTYPES
        prime_t  skipThroughPrime)
#    else
        skipThroughPrime)

        prime_t  skipThroughPrime ;
#    endif

{    /* Local variables. */
    prime_t  currentPrime, nextPrime, range ;
    size_t  i ;



/* Calculate the length of the sequence of differences.  The length is not
   needed, but (i) the skip-through-prime will be adjusted to the largest
   prime less than or equal to the argument passed into this range function
   and, more importantly, (ii) the length function will flag skip-through
   primes that are too large to use given the size of unsigned long integers. */

    if (nbrPrimeSeqDiffsLength (skipThroughPrime,
                                &skipThroughPrime, NULL) == 0) {
        LGE "(nbrPrimeSeqDiffsRange) Skip-through prime %"PRIuPRIME_T" too large.\n",
            skipThroughPrime) ;
        return (0) ;
    }

/*******************************************************************************

    Calculate the range of the skip-through prime's sequence of differences
    based on the ranges for the previous skip-through primes:

        range (skip-through-prime) = range (previous-prime) *
                                     skip-through-prime

    Or, beginning with the first prime, 2, repeat the following calculation
    until the skip-through prime is reached:

       range (next-prime) = range (current-prime) * next-prime

*******************************************************************************/

    currentPrime = 2 ;  range = 2 ;	/* 2's sequence = { 2 } */
    nextPrime = 3 ;
    i = 2 ;		/* Index of next prime after 3; assumes at
			   least 2,3,5 or 2,3,0 in early primes array. */

    while (nextPrime <= skipThroughPrime) {
		/* First check that "current-range * nextPrime"
		   will not exceed the maximum prime_t value. */
        if (range > (PRIME_T_MAX / nextPrime)) {
            SET_ERRNO (EOVERFLOW) ;
            LGE "(nbrPrimeSeqDiffsRange) Integer overflow on range %"PRIuPRIME_T", next prime %"PRIuPRIME_T".\n",
                range, nextPrime) ;
            return (0) ;
        }
        range = range * nextPrime ;
        currentPrime = nextPrime ;
        nextPrime = nbr_early_primes[i] ;
        if (nextPrime == 0) {
            nextPrime = nbrNextPrime (currentPrime, NULL) ;
            if (nextPrime == 0) {
                SET_ERRNO (EOVERFLOW) ;
                LGE "(nbrPrimeSeqDiffsRange) Integer overflow on next prime, current prime %"PRIuPRIME_T".\n",
                    currentPrime) ;
                return (0) ;
            }
        } else {
            i++ ;
        }
    }

    LGI "(nbrPrimeSeqDiffsRange) Skip-Through Prime = %"PRIuPRIME_T"   Range = %"PRIuPRIME_T"\n",
        skipThroughPrime, range) ;

    return (range) ;

}
#endif	/* !NBR_SKIP_2_3_HARDCODED */
