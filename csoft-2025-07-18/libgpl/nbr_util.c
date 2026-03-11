/* $Id: nbr_util.c,v 1.16 2021/09/15 04:25:40 alex Exp alex $ */
/*******************************************************************************

File:

    nbr_util.c

    Number Utilities.


Author:    Alex Measday


Purpose:

    The NBR_UTIL package is a collection of functions dealing with numbers
    in some way or another.  I wanted to remove the calculation of the first
    (next) prime number after the expected table size from my HASH_UTIL
    package and "prmNextPrime()" didn't really appeal to me, so I decided
    on a prefix of NBR.  I have added some non-prime-related functions.

    The source code for the package consists of a single header file,
    nbr_util.h, and multiple C source files:

        nbr_util_primes.c - for the NBR prime number functions.
        nbr_util_roots.c - for the NBR fast integer square root function.
        nbr_util.c - for other NBR functions.

    When you see the primes and roots source file prologs, you'll understand
    why I divided them up.  (I originally had them all in one file.)


Public Procedures (nbr_util.c):

    nbrEarlyPrimes() - gets a precomputed list of the first prime numbers.
    nbrFactorsOf() - generates the prime factors of a number.
    nbrBinaryGCD() - computes the GCD of two numbers using the binary variant
        of Euclid's algorithm.
    nbrModuloGCD() - computes the GCD of two numbers using the modulo-based,
        standard Euclid's algorithm.
    nbrSubtractGCD() - computes the GCD of two numbers using the
        subtraction-based variant of Euclid's algorithm.

Public Procedures (nbr_util_primes.c):

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

Public Procedures (nbr_util_roots.c):

    nbrUnsignedSQRT() - computes the unsigned integer square root of a number.

*******************************************************************************/

/*******************************************************************************

    Integer/Prime Factorization
    ===========================

    I was recently looking for the Wikipedia entry, "Primality test", that
    I referenced in the documentation for my nbrIsPrime() function.  It's
    been about 3 years, so I couldn't remember the name of the entry off
    the top of my head.  I searched the internet for "prime wikipedia",
    which took me to the main page, "Prime number".  There is a link to
    the "Primality test" entry in the "Computational methods" section's
    introductory paragraphs -- repeat 3 times quickly! -- but it's
    overshadowed by the "Trial division" subsection which, naturally,
    links to Wikipedia's "Trial division" entry.

    "Trial division" looked nothing at all like the page I remembered and
    it took me a while to realize the entry was about integer factorization,
    not primality testing.  I was intrigued by the C++ code and wondered if
    it is vulnerable to possible overflows.  No surprise, it is.  The code
    was introduced by a single, grammatically incorrect sentence:

        Below is version C++ (without squaring f)

    The workings of the function are not documented and are not obvious.
    To figure out how it works, I had to figure out the meaning of ac,
    temp, and the magic number 8.  Here's a version of the function with
    meaningful variable names and some meaningful comments:

        template <class T, class U>
        vector<T> TrialDivision(U n)
        {
                // The difference between the squares of two consecutive
                // odd factors, f and f+2, is (f+2)^2 - f^2 = 4f + 4.
                // The differences, 4f+4, increase as the factors
                // increase, but the difference between any two
                // consecutive differences, 4f+4 and 4(f+2)+4, is constant:
                // 4(f+2) + 4 - (4f + 4) = 4f + 8 + 4 - 4f - 4 = 8.
            const T DIFF_4FP4 = 8;
            vector<T> v;
            while (n % 2 == 0)          // Check for factors of 2.
                { v.push_back(2); n /= 2; }
            while (n % 3 == 0)          // Check for factors of 3.
                { v.push_back(3); n /= 3; }
            T factor = 3;
            T square = factor * factor;
            T diff = 4*factor + 4;      // Difference between 3^2 and 5^2.
            factor = 5;
            do {
                square += diff;         // Overflow possible!!!
                if (square > n) break;  // All factors tested?
                if (n % factor == 0) {  // Found a factor?
                    v.push_back(factor);
                    n /= factor;
                    square -= diff;     // Try factor again,
                                        // so revert to factor^2,
					// which might have overflowed!!!
                }
                else {                  // Not a factor, so advance
                    factor += 2;        // to the next odd number.
                    diff += DIFF_4FP4;  // Overflow possible!!!
                }
            } while (1);
            if (n != 1) v.push_back(n);
            return v;
        }

    To avoid multiplying the factor by itself on every factor tested,
    the function keeps track of the squares of the odd factors (ac)
    and the differences between the squares of consecutive odd factors
    (temp, incremented by 8 -- see the DIFF_4FP4 comment -- on each
    iteration).

    For example, assuming no repeating factors, here are the variable values
    in the first few DO-WHILE iterations:

        FACTOR    SQUARE    DIFF
        ------    ------    ----
        [Implicit previous factor:
           3         9       16    -- 3 is f; 9 is f^2; 16 is 4f+4 or 4(3)+4.
        ]
           5        25       24    -- 5 is f; 25 is f^2; 24 is 4f+4 or 4(5)+4.
           7        49       32
           9        81       40
          11       121       48
        ...

    The phrase, "*in* the ... iterations", is purposeful and the table above
    is for illustrative purposes.  In the actual code, when unsuccessfully
    testing consecutive odd factors, FACTOR is the only up-to-date variable
    at the very *beginning* of the loop.  SQUARE and DIFF lag behind, still
    based on the previous factor.  For example, at the top of the loop on
    the very first iteration, FACTOR is the correct 5, but SQUARE and DIFF
    are based on the previous factor, 3, and thus have the values 9 and 16,
    respectively.  The latter two get updated within the loop.  If a test is
    successful, the same factor must be tried again; in this case, the loop
    preserves the lagging values of SQUARE and DIFF and starts the next
    iteration in the same state as that at the beginning of the current
    iteration.  For example, if a loop begins with 11/81/40 and 11 is found
    to be a factor, the next iteration also begins with 11/81/40 in order
    to test 11 again.  (The implementation of the "diff" algorithm in my
    factorization test and compare program, FRIZE, moves the updating of
    both SQUARE and DIFF to the bottom of the loop, so all 3 variables
    are up-to-date at the top of the loop, as seen in the table above.)

    There are two sources of possible overflow: (i) adding DIFF to SQUARE
    and (ii) incrementing DIFF by 8.  And the two sources interact badly.
    The overflows begin occurring when factoring very large prime numbers.
    A prime number has exactly one factor, itself, so the testing loop will
    continue until the square root of the prime number is tested as a factor.
    To break out of the loop, the factor being tested must have a square
    greater than the prime number (square > n) and that's when overflows
    become possible.

    To illustrate, let's use unsigned 32-bit integers.  First, some
    preliminaries:

        Largest 32-bit value:	4,294,967,295 (0xFFFFFFFF)
        Square root of above:	65,535.999...
        Integer square root:	65,535 (0xFFFF)
        Square of 65,535:	4,294,836,225

    Note that the real square root of the maximum representable value is
    a shade under 65,536 and the integer square root is 65,535.  The square
    of the latter is 4,294,836,225, which is 131,070 less than the largest
    value.  The 5,853 prime numbers in that top range cannot be factored
    without overflow happening because the square of the factor being tested
    must exceed the prime number.  When 65,535 is tested as a factor of the
    prime number, its square is less than the prime number, so the loop
    must compute the square of the next factor, 65,537.  This square
    overflows the maximum representable 32-bit value.

        ASIDE: I get easily confused.  I was thinking later about
        the paragraph above and said to myself, "Wait a minute!
        I'm worrying over test factors around 65,535, but the
        factor for the largest 32-bit prime is 4,294,967,291.
        How do we get from 64K up to 4.3 billion?"  The answer,
        of course, is that I'm easily confused.  One only needs
        to test the factors up to the square root of the target
        number.  If no factors are found, the target is prime
        and the algorithm adds the target to the empty vector
        of factors after breaking out of the loop.  (Facepalm!)

    As a result and although the odd factors steadily increase by 2, the
    square has suddenly dropped down to a low number and must begin anew
    the Sisyphean climb to the top.  SQUARE now has no relation to FACTOR
    or DIFF.

    SQUARE is again less than N, so the function keeps testing new factors
    even though they're past the square root mark.  Since DIFF was not reset,
    SQUARE will move through the 32-bit integers even faster on the second
    pass until it overflows again.  The cycle will repeat itself over and
    over.  Meanwhile, FACTOR keeps incrementing by 2 and DIFF by 8.  As
    DIFF gets very large, it begins interacting with even small values of
    SQUARE, so that adding DIFF to SQUARE causes SQUARE to overflow.  It
    gets to the point where SQUARE is overflowing on every iteration of
    the testing loop.  At times, the overflows will produce descending
    values of SQUARE.

    We're not done yet.  Eventually DIFF reaches 4,294,967,288, seven less
    than the maximum representable 32-bit value.  When 8 is added to DIFF,
    DIFF conveniently overflows to zero.  SQUARE can now take its time
    running through the integers before overflowing.

    At most, the function should test about 32,767 (64K/2) odd factors
    for any 32-bit number.  If data type T is wider than data type U
    (the template parameters), the function will not exceed the test
    limit (32K if U is an unsigned 32-bit integer).  Type T is used
    for internal calculations; type U is the type for the number to
    be factored.  For example, if T is 64 bits and U is 32 bits, no
    overflows will occur.  Even declaring T as uint32_t (32 bits of
    precision) and U as int32_t (31 bits of positive precision) will
    prevent overflows.  The trade-off is that narrowing type U narrows
    the range of numbers that can be factored.

    When T and U are equal in width, overflows are possible and can
    lead to pathological behavior of the function.  To wit:

        Largest 32-bit prime:	4,294,967,291 (4 less than UINT32_MAX)
        # odd factors tested:	2,147,483,644 (not including test of 2)
        # SQUARE overflows:	1,073,741,824
        # DIFF overflows:	3

    In this case, the test loop is terminated when FACTOR equals N
    (i.e., 4,294,967,291) and FACTOR % N is zero, not when SQUARE > N.
    Thus, the number of factors tested (2 and odd) is N/2.  On average,
    SQUARE overflows on every other test.  DIFF overflows occur when
    advancing to factors 1G-1, 2G-1, and 3G-1, where G is in the sense
    of powers of 1024.  The DIFF overflows are expected on multiples
    of 1G because the number of odd factors tested in a range of 1G
    is 1G/2 and DIFF is incremented by 8 on each test, so DIFF reaches
    the overflow value at (1G/2)*8 = 4G, one more than UINT32_MAX.
    (Because the input prime is so large in the example above, when
    the loop terminates, the loop is only several tests shy of a
    fourth DIFF overflow.)

    The first improvement to TrialDivision() was to get rid of the
    TEMP/DIFF variable.  The formula for the next SQUARE is easy enough
    and fast enough to eliminate the need for and presence of a second
    source of overflow:

        template <class T, class U>
        vector<T> TrialDivision(U n)
        {
            vector<T> v;
            while (n % 2 == 0)          // Check for factors of 2.
                { v.push_back(2); n /= 2; }
            T factor = 3;
            T square = factor * factor;
            while (square <= n) {       // Until all factors are tested.
                if (n % factor == 0) {  // Found a factor?
                    v.push_back(factor);
                    n /= factor;        // Leave square alone and
                                        // try factor again.
                }
                else {                  // Not a factor, so advance
                                        // to the next odd number.
                        // (f + 2)^2 =
                        // f^2 + 4f + 4 =
                        // f^2 + 4(f + 1)
                    factor++ ;          // Overflow possible next!!!
                    square += factor * 4 ;
                    factor++ ;
                }
            }
            if (n != 1) v.push_back(n);
            return v;
        }

    The code is cleaner now.  The next square is calculated simply by
    adding 4 * (factor + 1) to the current square; an optimizing
    compiler will replace the multiplicaton with a left shift of two
    bits.  There is still the same possibility of SQUARE overflowing
    under the same circumstances.  Assuming that overflow causes the
    value of a variable to "wrap around", it is easy enough to detect
    just by comparing the old and new values of SQUARE.  If the new
    value is less than the old value, then overflow occurred.

    With this code in hand, an optimization immediately comes to mind.
    TrialDivision() checks every odd number from 3 on up as a factor.
    Borrowing the 6k+/-1 optimization from primality testing and
    skipping odd factors divisible by 3, the time to factor a number
    is reduced by up to a third.  Doing this also decreases the range
    of numbers that can be factored which, again, is attributable to
    overflow.  In the example presented earlier, the top 5,853 32-bit
    unsigned prime numbers cannot be factored without overflow; that's
    with the algorithm that tests 2 and all odd factors.  The 6k+/-1
    optimization increases that number to the largest 17,565 primes.
    (This increase is because the maximum square root is 65,535, which
    happens to be divisible by 3.  When checking all odd factors, the
    algorithm will test factors up to and including 65,535.  With the
    6k+/-1 optimization, the highest factor tested drops back to 65,533
    and the next factor tested, as it must be, is 65,537, whose square
    overflows.  Consequently, the 17,565 primes greater than or equal
    to 65,533^2 [4,294,574,089] cannot be factored without the square
    overflowing.)

    Eliminating the possibility of overflow and restoring the range of
    numbers that can be factored is acomplished by eliminating the use
    of squares.  A "fast integer square root" function can very quickly
    calculate the square root (and highest possible factor) of the input
    number if the number is (or can be converted to) an unsigned integer
    with an even number of bits.  My C implementation of TrialDivision()
    calls nbrUnsignedSQRT(), which is in "nbr_util_roots.c".

    There is an implicit limit on the number of factors tested in the
    squares version of TrialDivision() that I did not realize at first
    when I wrote up the square root version of TrialDivision().  Initially,
    the "target" number is the original input number.  Whenever a factor
    is found (including 2s and 3s), the target number is reduced (divided)
    by the factor.  Consequently, the "square <= n" loop test in the
    function above is actually testing two limit conditions:

        (1) If the original input number is composite, the target number
            is reduced by each factor until the target is reduced to 1
            after the last factor.  At this point, there is no need to
            test factors larger than the last factor, so the loop can
            terminate.  And in the squares version of TrialDivision(),
            the loop does so because "square > 1".

        (2) If the original input number is prime, no factors will be found
            and the target will remain equal to the original input number.
            To ascertain that there are no factors and that the number is
            prime, it is necessary to check all the possible factors up to
            the square root of the number (i.e., "square <= n").

    Limit #2 is a hard limit needed for prime numbers.  Limit #1 is just an
    optimization.  When the target number is reduced to 1, it is perfectly
    acceptable, albeit time-consuming, to continue unsuccessfully testing
    factors up to the square root.

    Here's the square root version of TrialDivision() that (i) eliminates
    the possibility of overflow, (ii) implements the 6k+/-1 optimization,
    and (iii) includes both limit tests:

        template <class T, class U>
        vector<T> TrialDivision(U n)
        {
            vector<T> v;
            while (n % 2 == 0)          // Check for factors of 2.
                { v.push_back(2); n /= 2; }
            while (n % 3 == 0)          // Check for factors of 3.
                { v.push_back(3); n /= 3; }
            T factor = 5;
            T root = (T) UnsignedSQRT (n);
            bool byTwo = true;          // Increment factor by 2 or by 4.
            while ((n > 1) &&           // Until n is reduced to 1 ...
                   (factor <= root)) {  // ... or all factors are tested.
                if (n % factor == 0) {  // Found a factor?
                    v.push_back(factor);
                    n /= factor;        // Try factor again.
                }
                else {                  // Not a factor.
                    if (byTwo)
                        factor += 2;    // Advance to next odd factor.
                    else
                        factor += 4;    // Skip odd factor divisible by 3.
                    byTwo = !byTwo;     // Alternate between 2 and 4.
                }
            }
            if (n != 1) v.push_back(n); // N not reduced to 1 means N is prime.
            return v;
        }

    Not being a big fan of C++, I implemented this last version in C and
    called it nbrFactorsOf().  The function uses a single type for both the
    input number and internal calculations, "factor_t", which is defined in
    "nbr_util.h".  Factors of the input number are stored in an array of
    "factor_t"s supplied by the caller.

    Two additional optimizations were added to the C function:

        (1) A list of the first M prime numbers was added: 2, 3, 5, 7, 11, ...
            These numbers are tested as factors at the very beginning of
            nbrFactorsOf(), thus giving the function a headstart by not
            having to test non-prime odd factors in the range covered by
            the list.  M is a compile-time constant, NBR_EARLY_PRIMES,
            and there is the obvious trade-off between speed and the
            memory footprint of the list.

        (2) When a factor is found and the target number is reduced, the
            square root of the target number is recalculated.


    Performance
    -----------

    To save you the trouble of reading the analysis below, here's the short
    summary:

      - The timings of the different algorithms I implemented were comparable.

      - The 6k+/-1 optimization had the biggest impact on all the algorithms,
        reducing their times by one-third.

      - Computing the square root at the outset offers a slight improvement in
        timing, but the big gain is that there are no overflows and, therefore,
        all of the primes which fit into the chosen integer data type can be
        factored.

    18,446,744,065,119,617,025 is a large, 64-bit number with 10 factors:

                 3x3 x 5x5 x 17x17 x 257x257 x 65537x65537

    LIBGPL function nbrFactorsOf() quickly tests the first 512, precomputed
    prime numbers as factors, up through 3,671.  The function then drops
    into the middle of the 6k+/-1 cycle and begins with 3,673 testing odd
    numbers not divisible by 3 as factors.  Testing the early primes as
    factors catches 8 out of the 10 factors of 18,446,744,065,119,617,025,
    leaving only 65,537^2 to be factored.

    The algorithms implemented in and tested by the FRIZE program all have
    this early-prime optimization available to them, but, in the timing
    runs I present below, the optimization is disabled on the command line.

    In the timed tests that follow, FRIZE factors the big number above
    15,000 times in order to get large enough timing measurements.  The
    command line used for each run was:

        % frize -early 0 -time -repeat 15000 18446744065119617025,<algo>

    The different algorithms performed as follows on this factorization
    (the "root" and "root3" figures include the number of fast integer
    square roots calculated during the factorization):

        "diff":		21.3 seconds	32,777 factors tested
        "square":	20.9 seconds	32,777 factors tested
        "square3":	14.1 seconds	21,855 factors tested
        "root":		19.6 seconds	32,777 factors tested	10 roots
        "root3":	13.4 seconds	21,855 factors tested	8 roots

        "nbr":		12.6 seconds (with 512 early primes)

    The elapsed times for the "diff", "square", and "root" algorithms are
    close, as are the times for the "square3" and "root3" algorithms.  The
    6k+/-1 optimization obviously has the biggest effect since it reduces
    the number of potential factors tested by a third.

    Testing the first 512 primes, nbrFactorsOf() is slightly faster than
    "square3" and "root3"; however, it only takes about 130 tests at most
    for the other algorithms to reach the 7th and 8th factor, 257.  If the
    "-early 0" option is removed from the command line, FRIZE will test up
    to 6,542 prime numbers as factors, ranging from 2 to 65,521.  The last
    number is the largest, unsigned 16-bit prime and is 16 less than the
    last two factors, 65,537.  Consequently, only a few extra odd numbers
    have to be tested as factors and all the algorithms factor the input
    number 15,000 times in 4 seconds or less!  (Except for the "nbr"
    algorithm which only tests 512 early primes anyway.)

    18,446,744,047,939,747,781 is the largest prime number that the "square3"
    algorithm can factor without 64-bit overflow.  The number has only one
    factor: itself.  It takes about a minute or more to factor the number
    once, so the FRIZE command line for only one factorization instead of
    15,000 is:

        % frize -early 0 -time 18446744047939747781,<algo>

    And the timing results are:

        "diff":		93.2 seconds	2,147,483,646 factors tested
        "square":	91.0 seconds	2,147,483,646 factors tested
        "square3":	61.4 seconds	1,431,655,765 factors tested
        "root":		85.6 seconds	2,147,483,646 factors tested	1 root
        "root3":	58.5 seconds	1,431,655,765 factors tested	1 root

        "nbr":		55.0 seconds (with 512 early primes)

    Again, the times are comparable among the algorithms, the 6k+/-1
    optimization had the biggest effect on performance, and the root
    methods aren't subject to overflow.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  "nbr_util.h"			/* Number utilities. */


/*******************************************************************************
    earlyPrimes - is a list of the first N prime numbers, where N is
        defined by C Preprocessor macro NBR_EARLY_PRIMES.  The included
        text file contains all the prime numbers less than 65,355, so
        they all fit in unsigned 16-bit integers.  N is actually rounded
        up to certain sizes; see the text file for details.
*******************************************************************************/

#define  NBR_EARLY_PRIMES  512		/* First 512 primes. */

static  const  uint16_t  earlyPrimes[] = {
#    include  "nbr_early_primes.txt"
    0		/* End-of-list marker. */
} ;

static  size_t  numEarlyPrimes = (sizeof earlyPrimes / sizeof (uint16_t)) - 1 ;


int  nbr_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  nbr_util_debug

/*!*****************************************************************************

Procedure:

    nbrEarlyPrimes ()

    Get a Precomputed List of the First Prime Numbers.


Purpose:

    Function nbrEarlyPrimes() returns a precomputed list of the first N
    prime numbers, where N is the internal, compilation-time size of the
    list.  Currently, the list contains the first 512 prime numbers.
    Following the last prime number is a final entry containing a value
    of zero.

    An application can create its own bigger or smaller list by #define'ing
    an appropriate NBR_EARLY_PRIMES and #include'ing "nbr_early_primes.txt"
    (in the header file directory).  That file contains all the 6,542 prime
    numbers less than 65,535, which all fit into unsigned 16-bit integers.
    The prologue in the text file has more details on how to use it.


    Invocation:

        earlyPrimes = nbrEarlyPrimes (&numPrimes) ;

    where

        <numPrimes>	- O
            returns the number of primes in the list.  This argument can be
            NULL if the caller prefers to scan the list until the end-of-list
            zero marker is encountered.
        <earlyPrimes>	- O
            returns the address of the array of primes.

*******************************************************************************/


const  uint16_t  *nbrEarlyPrimes (

#    if PROTOTYPES
        size_t  *numPrimes)
#    else
        numPrimes)

        size_t  *numPrimes ;
#    endif

{

    if (numPrimes != NULL)
        *numPrimes = (sizeof earlyPrimes / sizeof (uint16_t)) - 1 ;

    return (earlyPrimes) ;

}

/*!*****************************************************************************

Procedure:

    nbrFactorsOf ()

    Generate the Prime Factors of a Number.


Purpose:

    Function nbrFactorsOf() computes and returns the prime factors of a number.
    The function uses the square root of the input number as the upper bound
    of factors to test, thus avoiding the possibility of overflow that may
    occur in other algorithms.

    As an example, nbrFactorsOf() will return 4 prime factors for 126:

                               2 x 3 x 3 x 7


    Invocation:

        numFactors = nbrFactorsOf (number, maxFactors, &factors) ;

    where

        <number>        - I
            is the number to be factored.
        <maxFactors>    - I
            is the maximum number of factors the following array can hold.
            If the next argument, the array of factors, is NULL, this
            argument is ignored.
        <factors>       - O
            is a caller-supplied array into which the factors will be stored.
            If this argument is NULL, the factors are not stored and the
            function simply counts the number of factors.
        <numFactors>    - O
            returns the number of factors generated.  The list of factors
            does not include one, so a prime number will only have one
            factor: itself.  The factors for other numbers will begin with
            the smallest prime numbers that evenly divide the numbers.

*******************************************************************************/


size_t  nbrFactorsOf (

#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors)
#    else
        number, maxFactors, factors)

        factor_t  number ;
        size_t  maxFactors ;
        factor_t  *factors ;
#    endif

{    /* Local variables. */
    bool  byTwo ;
    const  char  *prefix = "(nbrFactorsOf) " ;
    factor_t  factor, root ;
    size_t  i, numFactors ;
#define  FRIZE_FACTOR  0	/* Define as 1 in "frize" program. */
#if defined(FRIZE_FACTOR) && FRIZE_FACTOR
    extern
#endif
    size_t  numRoots, numTests ;



    numFactors = 0 ;
    numTests = 0 ;
    numRoots = 0 ;

    if (number < 2) {		/* 0 or 1?  No factors. */
        LGI "%s# = %"PRIuFACTOR_T"  # of factors = %"PRIuSIZE_T"  # of tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
        return (0) ;
    }

    if ((factors != NULL) && (maxFactors < 1)) {
        SET_ERRNO (EINVAL) ;
        LGE "%sMaximum of 0 factors: ", prefix) ;
        return (0) ;
    }

/*******************************************************************************
    Check for factors in the table of the first N prime numbers.
*******************************************************************************/

    factor = 0 ;

    for (i = 0 ;
         (i < numEarlyPrimes) && (earlyPrimes[i] != 0) && (number > 1) ;
         i++) {
        factor = earlyPrimes[i] ;
        while ((number % factor) == 0) {
            numTests++ ;
            if ((numFactors >= maxFactors) && (factors != NULL)) {
                SET_ERRNO (ENOMEM) ;
                LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                    prefix, maxFactors) ;
                return (0) ;
            }
            if (factors != NULL)  factors[numFactors] = factor ;
            numFactors++ ;
            number = number / factor ;
        }
        numTests++ ;
    }


/* If, for some reason, the table is empty or factors 2 and/or 3 were not
   checked, then check factors 2 and 3.  This will ensure that the code
   that follows will advance to factor 5, which will then be in sync with
   the 6k+/-1 cycle. */

    if (factor < 3) {
        factor = 2 ;
        while ((number % factor) == 0) {
            numTests++ ;
            if ((numFactors >= maxFactors) && (factors != NULL)) {
                SET_ERRNO (ENOMEM) ;
                LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                    prefix, maxFactors) ;
                return (0) ;
            }
            if (factors != NULL)  factors[numFactors] = factor ;
            numFactors++ ;
            number = number / factor ;
        }
        numTests++ ;
    }

    if (factor < 5) {
        factor = 3 ;
        while ((number % factor) == 0) {
            numTests++ ;
            if ((numFactors >= maxFactors) && (factors != NULL)) {
                SET_ERRNO (ENOMEM) ;
                LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                    prefix, maxFactors) ;
                return (0) ;
            }
            if (factors != NULL)  factors[numFactors] = factor ;
            numFactors++ ;
            number = number / factor ;
        }
        numTests++ ;
    }

/*******************************************************************************

    Advance to the next odd factor.  At this point, the last factor tested
    was prime, either 3 or the last prime, p, in the early primes list.  It
    is necessary to get the subsequent factors into sync with the 6k+/-1
    cycle by (i) determining the next factor to be tested and (ii) setting
    the by-twos boolean flag so that the next factor is incremented by the
    correct 2 or 4 after it is tested.

    In the 6k+/-1 cycle, the factors tested are

                            f, f+2, f+6

    with increments of 2 and 4.  F+6 begins the next cycle of increments
    of 2 and 4 (f+6, f+8, f+12).

    The next odd number after the last prime tested is p + 2.  If p+2 is
    divisible by 3 and should be skipped, then p was the f+2 in the cycle
    above.  Therefore, the next factor tested should be p + 4; i.e.,
    p + (f+6) - (f+2) = p+4.  After that factor is tested, it should be
    incremented by 2.

    If p+2 is not divisible by 3 and should be tested, then p was the f in
    the cycle above.  Therefore, the next factor tested should be p + 2;
    i.e., p + (f+2) - f = p+2.  After that factor is tested, it should be
    incremented by 4.

    Note that a last factor of 3 is a special case since, leaving out the
    testing of early primes, the 6k+/-1 cycle begins with a factor of 5.
    3 is tested, but it's effectively part of the k=0 cycle, where f is -1,
    f+2 is +1, and f+6 is 5.  The k=1 cycle then begins at 5.

    Control can now pass into the big loop with all increments, not to
    mention watches, synchronized.

*******************************************************************************/

    factor += 2 ;
    if ((factor % 3) == 0) {	/* P was f+2 in the cycle? */
        factor += 2 ;		/* Test p+4 and ... */
        byTwo = true ;		/* ... next increment is 2. */
    } else if (factor == 5) {	/* P was 3? */
        byTwo = true ;		/* Test 5 and next increment is 2. */
    } else {			/* P was f in the cycle. */
        byTwo = false ;		/* Test p+2; next increment is 4. */
    }

    LGI "%sSynchronized factor = %"PRIuFACTOR_T"  increment = %d\n",
        prefix, factor, byTwo ? 2 : 4) ;

/*******************************************************************************

    Check all odd factors not divisible by 3 up to the largest odd number
    not exceeding the square root of the target number.  (The target number
    was/is the original input number, but it may have been reduced by the
    tests of the early primes.)

    When an actual factor is found, it is added to the list of the input
    number's factors.  Adding the factor to the list of factors found so
    far must be balanced by removing the factor from the target number;
    i.e., divide the target number by the factor.  Only the product of
    the remaining factors needs to and must be factored and its factors
    will all be equal to or greater than the factor just found.  (I say
    "must" because reducing the target number is necessary to detect
    multiple instances of the same factor.)  Finally, the square root
    of the new target number is recalculated.

    To break this down, begin with the following definitions:

        N	- input number to be factored.
        Ps	- product of the factors found so far.
        Pr	- product of the remaining factors.
        f	- factor just found.

    Initially, N is the product of all the factors yet to be found;
    i.e., all the factors yet to be found are all the remaining
    factors, Pr:

        Ps = 1
        Pr = the product of all the factors
        N = Ps * Pr = Pr

    After a factor, f, is found, it is only necessary to factorize the
    product of the subsequent, remaining factors, which will be Pr.
    The first step is to move f out of the remaining factors and into
    the found-so-far factors:

        Pr := Pr / f
        Ps := Ps * f

    N is still Ps * Pr and the new target number to be factored is the
    now f-reduced Pr.  (Ps does not appear in the algorithm, but its
    value is implicit in the output list of factors found so far.)

    Eventually the loop will terminate under one of the following conditions:

        (1) The target number, Pr, has been reduced to 1, which means that
            all the factors have been found.

        (2) Pr has not been reduced to 1, but the next factor to be tested
            exceeds the square root of Pr.  When this happens, Pr will be
            added as the final factor of the input number after the loop
            exits.  This is always the case when factoring a prime number,
            but it can also happen with composite numbers if the integer
            square root of Pr is not a factor tested (i.e., the root is
            even or it is divisible by 3).

            For example, take the number 42, the answer to many questions.
            Its factors are 2, 3, and 7.  2 and 3 are caught above and Pr
            is reduced from 42 to 21 to 7.  The next factor to be tested
            is 5 and the integer square root of 7 is 2.  The loop detects
            that 5 > 2 and doesn't execute a single iteration.  After the
            loop, Pr, 7, has not been reduced to 1, so 7 is added as the
            final factor of 42.

*******************************************************************************/

    if (number > 1) {
        root = nbrUnsignedSQRT ((root_t) number, NULL) ;
        numRoots++ ;
        LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
            prefix, number, root, factor) ;
    } else {
        root = 0 ;
    }

    while ((number > 1) &&		/* Until number is reduced to 1 ... */
           (factor <= root)) {		/* ... or all factors are tested. */

        if ((number % factor) == 0) {	/* Divides evenly - is a factor. */

            if ((numFactors >= maxFactors) && (factors != NULL)) {
                SET_ERRNO (ENOMEM) ;
                LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                    prefix, maxFactors) ;
                return (0) ;
            }
            if (factors != NULL)  factors[numFactors] = factor ;
            numFactors++ ;
            number = number / factor ;	/* Reduce number and ...
					   try factor again. */
            root = nbrUnsignedSQRT ((root_t) number, NULL) ;
            numRoots++ ;
            LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
                prefix, number, root, factor) ;

        } else {			/* Not a factor. */

            if (byTwo) {		/* Advance to the next odd factor. */
                factor += 2 ;
            } else {			/* Skip the odd number divisible by 3. */
                factor += 4 ;
            }

            byTwo = !byTwo ;		/* Alternate increments by 2 and by 4. */

        }

        numTests++ ;

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.  This can happen on both composite
    and prime numbers.
*******************************************************************************/

    if (number > 1) {
        if ((numFactors >= maxFactors) && (factors != NULL)) {
            SET_ERRNO (ENOMEM) ;
            LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                prefix, maxFactors) ;
            return (0) ;
        }
        if (factors != NULL)  factors[numFactors] = number ;
        numFactors++ ;
    }


    LGI "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"  # roots = %"PRIuSIZE_T"\n",
        prefix, number, numFactors, numTests, numRoots) ;

    return (numFactors) ;

}

/*!*****************************************************************************

Procedure:

    nbrBinaryGCD ()

    Compute the GCD of Two Numbers Using the Binary Euclid's Algorithm.


Purpose:

    Function nbrBinaryGCD() calculates the greatest common divisor (GCD)
    of two positive integers using the binary optimization of Euclid's
    Algorithm.  The binary algorithm uses division and multiplication
    by powers of 2, which an optimizing compiler can convert into
    arithmetic shifts right and left, respectively.

    My implementation is based on the algorithm presented in the
    HANDBOOK OF APPLIED CRYPTOGRAPHY by Alfred J. Menezes, Paul C.
    van Oorschot, and Scott A. Vanstone (CRC Press, 1996).  The
    binary algorithm is found in Section 14.4: "Greatest Common
    Divisor Algorithms".  Chapter 14 is available, legally (!),
    as a PDF file at

           http://cacr.uwaterloo.ca/hac/about/chap14.pdf#page=17


    Invocation:

        gcd = nbrBinaryGCD (x, y, &numSteps) ;

    where

        <x>, <y>	- I
            are the two positive integers whose GCD will be calculated.
            It does not matter if the larger number is passed in first
            or vice versa; the function will swap the two arguments
            internally if necessary.
        <numSteps>	- O
            returns the number of steps performed to calculate the GCD,
            where a "step" is generally just a loop iteration.  This
            count is simply for informational purposes; for example,
            when comparing the number of steps required by the binary-,
            modulo-, and subtraction-based algorithms as applied to the
            same pair of numbers.  This argument can be NULL if the caller
            does not need this value.
        <gcd>		- O
            returns the greatest common divisor of X and Y.  If either
            argument is negative, 0 is returned; this can happen if type
            GCD_t is defined as a signed integer type.  If either argument
            is 0, the other argument is returned; i.e., gcd (a, 0) =
            gcd (0, a) = a and gcd (0, 0) = 0.

*******************************************************************************/


GCD_t  nbrBinaryGCD (

#    if PROTOTYPES
        GCD_t  x,
        GCD_t  y,
        size_t  *numSteps)
#    else
        x, y, numSteps)

        GCD_t  x ;
        GCD_t  y ;
        size_t  *numSteps ;
#    endif

{    /* Local variables. */
    GCD_t  g, temp ;
    size_t  numLoops = 0 ;



    if ((x < 0) || (y < 0)) {
        SET_ERRNO (EINVAL) ;
        LGE "(nbrBinaryGCD) X (%"PRIuGCD_T") or Y (%"PRIuGCD_T") is negative.\n",
            x, y) ;
        return (0) ;
    }

    if (x < y) {		/* Swap arguments to ensure x >= y? */
        temp = x ;  x = y ;  y = temp ;
    }

    g = 1 ;

/* Drop factors of 2 common to both arguments.  NOTE that the check for
   y is 0 prevents control from dropping into an infinite loop in the
   case of gcd (0, 0). */

    if (y == 0)  return (x) ;	/* Covers both gcd (a, 0) and gcd (0, 0). */

    while (!(x & 1) && !(y & 1)) {	/* While both x and y are even. */
        x /= 2 ;
        y /= 2 ;
        g *= 2 ;
    }

/* While ... */

    while (x != 0) {

        while (!(x & 1)) {	/* Drop factors of 2 from current value of x. */
            x /= 2 ;
        }

        while (!(y & 1)) {	/* Drop factors of 2 from current value of y. */
            y /= 2 ;
        }

        if (x >= y)
            x = (x - y) / 2 ;
        else
            y = (y - x) / 2 ;

        numLoops++ ;

    }

    if (numSteps != NULL)  *numSteps = numLoops ;

    return (g * y) ;

}

/*!*****************************************************************************

Procedure:

    nbrModuloGCD ()

    Calculate the GCD of Two Numbers Using the Standard Euclid's Algorithm.


Purpose:

    Function nbrModuloGCD() calculates the greatest common divisor (GCD)
    of two positive integers using the standard, modulo-based version
    of Euclid's Algorithm.


    Invocation:

        gcd = nbrModuloGCD (x, y, &numSteps) ;

    where

        <x>, <y>	- I
            are the two positive integers whose GCD will be calculated.
            It does not matter if the larger number is passed in first
            or vice versa; the function will swap the two arguments
            internally if necessary.
        <numSteps>	- O
            returns the number of steps performed to calculate the GCD,
            where a "step" is generally just a loop iteration.  This
            count is simply for informational purposes; for example,
            when comparing the number of steps required by the binary-,
            modulo-, and subtraction-based algorithms as applied to the
            same pair of numbers.  This argument can be NULL if the caller
            does not need this value.
        <gcd>		- O
            returns the greatest common divisor of X and Y.  If either
            argument is negative, 0 is returned; this can happen if type
            GCD_t is defined as a signed integer type.  If either argument
            is 0, the other argument is returned; i.e., gcd (a, 0) =
            gcd (0, a) = a and, as it naturally falls out, gcd (0, 0) = 0.

*******************************************************************************/


GCD_t  nbrModuloGCD (

#    if PROTOTYPES
        GCD_t  x,
        GCD_t  y,
        size_t  *numSteps)
#    else
        x, y, numSteps)

        GCD_t  x ;
        GCD_t  y ;
        size_t  *numSteps ;
#    endif

{    /* Local variables. */
    GCD_t  temp ;
    size_t  numLoops = 0 ;



    if ((x < 0) || (y < 0)) {
        SET_ERRNO (EINVAL) ;
        LGE "(nbrModuloGCD) X (%"PRIuGCD_T") or Y (%"PRIuGCD_T") is negative.\n",
            x, y) ;
        return (0) ;
    }

    if (x < y) {	/* Swap arguments to ensure x >= y? */
        temp = x ;  x = y ;  y = temp ;
    }

/* While ... */

    while (y != 0) {

        temp = y ;
        y = x % y ;
        x = temp ;

        numLoops++ ;

    }

    if (numSteps != NULL)  *numSteps = numLoops ;

    return (x) ;

}

/*!*****************************************************************************

Procedure:

    nbrSubtractGCD ()

    Calculate the GCD of Two Numbers Using Subtraction.


Purpose:

    Function nbrSubtractGCD() calculates the greatest common divisor (GCD)
    of two positive integers using the subtraction variant of Euclid's
    Algorithm.


    I found this identity on the late Alexander Bogomolny's CUT THE KNOT
    website:

                        gcd (x, y) = gcd (x - y, y)

              https://www.cut-the-knot.org/blue/binary.shtml

    (Which is the page for the binary algorithm!)  Bogomoly called this a
    property of GCD; NOTE that it is NOT an algorithm.  For example, the
    next step is NOT a recursive:

        gcd (x - y, y) = gcd ((x - y) - y, y)


    Invocation:

        gcd = nbrSubtractGCD (x, y, &numSteps) ;

    where

        <x>, <y>	- I
            are the two positive integers whose GCD will be calculated.
            It does not matter if the larger number is passed in first
            or vice versa; the function will swap the two arguments
            internally if necessary.
        <numSteps>	- O
            returns the number of steps performed to calculate the GCD,
            where a "step" is generally just a loop iteration.  This
            count is simply for informational purposes; for example,
            when comparing the number of steps required by the binary-,
            modulo-, and subtraction-based algorithms as applied to the
            same pair of numbers.  This argument can be NULL if the caller
            does not need this value.
        <gcd>		- O
            returns the greatest common divisor of X and Y.  If either
            argument is negative, 0 is returned; this can happen if type
            GCD_t is defined as a signed integer type.  If either argument
            is 0, the other argument is returned; i.e., gcd (a, 0) =
            gcd (0, a) = a and gcd (0, 0) = 0.

*******************************************************************************/


GCD_t  nbrSubtractGCD (

#    if PROTOTYPES
        GCD_t  x,
        GCD_t  y,
        size_t  *numSteps)
#    else
        x, y, numSteps)

        GCD_t  x ;
        GCD_t  y ;
        size_t  *numSteps ;
#    endif

{    /* Local variables. */
    GCD_t  temp ;
    size_t  numLoops = 0 ;



    if ((x < 0) || (y < 0)) {
        SET_ERRNO (EINVAL) ;
        LGE "(nbrSubtractGCD) X (%"PRIuGCD_T") or Y (%"PRIuGCD_T") is negative.\n",
            x, y) ;
        return (0) ;
    }

    if (x < y) {		/* Swap arguments to ensure x >= y? */
        temp = x ;  x = y ;  y = temp ;
    }

/* While ... */

    while (y != 0) {

        if (x > y)		/* Replace x with |x-y|. */
            x = x - y ;
        else
            x = y - x ;

        if (x < y) {		/* Swap arguments to ensure new x >= y? */
            temp = x ;  x = y ;  y = temp ;
        }

        numLoops++ ;

    }

    if (numSteps != NULL)  *numSteps = numLoops ;

    return (x) ;

}
