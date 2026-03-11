/* $Id: nbr_util_roots.c,v 1.2 2021/09/15 04:25:40 alex Exp alex $ */
/*******************************************************************************

File:

    nbr_util.c

    Number Utilities (Square Root)


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
        nbr_util_others.c - for other NBR functions.

    When you see the primes and roots source file prologs, you'll understand
    why I divided them up.  (I originally had them all in one file.)

    Function nbrUnsignedSQRT() returns the floor of the square root of
    an unsigned integer (i.e., the real square root with the fractional
    part, if any, discarded):

        #include  <stdio.h>			-- Standard I/O definitions.
        #include  "nbr_util.h"			-- Number utilities.
        ...
        root_t  number = 34 ;			-- Real square root is 5.8
        root_t  root = nbrUnsignedSQRT (number, NULL) ;
						-- Prints floor[5.8] = 5
        printf ("Square root = %"PRIuROOT_T"\n", root) ;

    Following the list of public functions below is a very long, very
    detailed discussion of fast integer square roots:

        FAST INTEGER SQUARE ROOTS
            THE ALGORITHM
            RISK OF OVERFLOW
            DIFFERENCES
            PERFORMANCE
            UNDERSTANDING ROSS FOSLER'S ASSEMBLY CODE

    The actual code begins around line 730 in this file, with the inclusion
    of header files, data declarations, etc.


Public Procedures:

    nbrUnsignedSQRT() - computes the unsigned integer square root of a number.

*******************************************************************************/

/*******************************************************************************

    FAST INTEGER SQUARE ROOT
    ========================

    (I'm repeating the Duane Allman quote from nbr_util_prime.c because the
    fast integer square root function prolog was written while I was still
    developing the prime number functions and writing their lengthy prolog.)

            "We don't do this song very much, but I feel like
            singing so, I guess, that's what we're gonna do."
                        - Duane Allman, "Dimples", Ludlow Garage, 1970

    I feel like writing so, I guess, that's what I'm going to do!

        WARNING:  The algorithm works and achieves its speed by examining
        and manipulating bits in the standard, binary representation of
        unsigned integers.  In the rare case of a target CPU using a
        different encoding or representation of integers, then another
        algorithm must be found.  Examples of different representations
        include the various forms of binary-coded decimal and Gray code
        numbers.  I vaguely recall, from the 1970s or 1980s, attempts
        to create 4-level logic elements that would have made possible
        quarternary representation of numbers.

    Function nbrUnsignedSQRT() quickly computes the floor of the square
    root of an unsigned integer: floor[sqrt(n)]; in other words, the largest
    integer equal to or less than the number's real square root.  In the file
    prolog, I give the example of 34, whose real square root is about 5.8,
    but nbrUnsignedSQRT() returns 5.  (The function returns a square root
    of 5 for 25, 26, ..., 34, and 35, only stepping up to a root of 6 for
    numbers 36 through 48.)

    A widely cited algorithm developed by Ross M. Fosler is used:

        "Fast Integer Square Root" by Ross M. Fosler
        Application Note TB040, DS91040A, (c) 2000 Microchip Technology Inc.
        http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf

    combined with an ingenious tweak from Tristan Muntsinger:

        https://web.archive.org/web/20220118185505/http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C


    THE ALGORITHM
    -------------

        NOTE:  I found Ross Fosler's flowchart confusing and I wasn't
        familiar with the PIC18 instruction set used in his code, so
        I reverse-engineered the algorithm from Example 1 of his
        application note and I implemented that algorithm in C.
        The reverse-engineered algorithm is described in the following
        paragraphs.  I later came across documentation for the PIC18
        instruction set and I was able to study his assembly language code.
        I was surprised to find that Fosler's algorithm was slightly
        different than "my" algorithm.  Specifically, he handled the
        "...10..." to "...01..." transformation (or "Shift bit right"
        as it's called in the application note) in an unusual,
        functionally equivalent way which might have been more
        advantageous, performance-wise, on the PIC controller.  So the
        description below applies equally to our two variations on his
        algorithm, including the limitations and risks of overflow I
        bring up.  (I discuss the assembly language algorithm in further
        detail down at the end when I introduce Tristan Muntsinger's
        optimization.)

    The algorithm begins with an initial estimate of the square root of the
    target number.  The estimate is based on the integer square root of the
    maximum representable value for the unsigned integer type.  Assume this
    type is m bits wide (where m is even).

      * The maximum representable value--let's call it UM_MAX--is the value
        when all m bits are set.  (C's ULONG_MAX is used in the code.)

      * The square root of UM_MAX--let's call it UM_MAX_SQRT--is the value
        when all the bits are set in the least significant half of the integer
        type; i.e., the m/2 least significant bits are all set.

      * Take the most significant 1 bit in UM_MAX_SQRT and set the
        corresponding bit in the estimated square root.  This is bit
        (m/2)-1 (zero-based bit numbering) and the value of this initial
        estimate is (UM_MAX_SQRT+1)/2.

    The algorithm then exams progressively less significant bits in the
    intermediate square root, bouncing above and below the actual square
    root on its way as bits are set or cleared, until the algorithm finally
    zeroes in (pun sort of intended) on the absolute least significant bit
    (bit 0) of the actual integer square root.

    For example, let unsigned integers be 16 bits wide.  UM_MAX is
    0xFFFF (65,535 decimal); UM_MAX_SQRT is then the lower half of
    the word with all bits set: 0x00FF (255 decimal).  Taking the most
    significant 1 bit (bit 16/2-1 = 7) from UM_MAX_SQRT gives an initial
    estimate of 0x0080 (128, or half of 256).  In binary, the algorithm
    begins with an estimated square root of:

                            0000 0000 1000 0000

    The adjacent 0 bit (bit 6) to the right of the 1 bit is examined first.
    If the square of the intermediate root (0x0080) is less than the target
    number (i.e., the intermediate root is less than the actual root), then
    set that bit in order to increase the value (0x00C0, 192 decimal) of
    the intermediate root, possibly overshooting the actual root:

                            0000 0000 1100 0000

    For illustrative purposes, let's assume that was the case.  Now move to
    the next 0 bit, bit 5.  If the intermediate root is less than the actual
    root, the algorithm would set bit 5 (0x00E0--3 bits set) and loop again.
    (I'm using shorthand here for what is actually a comparison of the square
    of the intermediate root and the target number.)  Ross Fosler calls setting
    a bit under this condition "Start new bit" in his flowchart.

    If, however, the intermediate root is greater than the actual root,
    lower the intermediate root by clearing the *previous* bit (bit 6)
    and setting the current bit (bit 5), giving a value of 0x00A0 and
    possibly undershooting the actual root:

                            0000 0000 1010 0000

    The pattern here is that if the previous and current bits are 10 (binary)
    and the intermediate root is too large, then substitute 01 (binary) for
    the two bits.  This is essentially picking the value, 01, halfway between
    00 (tested when the previous bit was examined) and 10 (the result of the
    previous bit being examined).  Ross Fosler calls this "...10..." to
    "...01..." transformation "Shift bit right" in his flowchart.

    Perhaps more clearly, let the number in curly braces be the number
    of most significant bits of the root determined so far.  Following
    the example above, the initial estimate is:

        estimate{0} = 0000 0000 1000 0000

    The estimate is less than the actual square root, so the most significant
    bit has been determined and we next try setting the adjacent bit:

        estimate{1} = 0000 0000 1100 0000

    (Although the first two bits are set, it's only estimate{1}.  We know
    the left bit is correct, but the right bit is, as yet, only a trial bit.)
    This estimate is greater than the square root and we now know that the
    root must fall between the previous estimate and the current estimate:

                     estimate{0} < root < estimate{1}

    So we pick a new estimate halfway between the two estimates.  This
    requires clearing the bit we set in estimate{1}, so we know for
    certain the two most significant bits ("10" instead of "11") of
    the root and we add a third trial bit ("1"):

        estimate{2} = (estimate{0} + estimate{1}) / 2
                    = 0000 0000 1010 0000

    Although my example happened to fall at the very beginning of the
    estimates, the same logic is applied at an arbitrary bit location
    in the estimates:

             estimate{b+2} = (estimate{b} + estimate{b+1}) / 2

    Finally, it's time to consider how the algorithm terminates.  If an
    intermediate root is equal to the actual square root, the algorithm
    can exit the loop immediately and return the square root to the calling
    program.  Otherwise, the algorithm will examine all the bits in the
    root down to the least significant bit.  A bit in a bit mask keeps
    track of the current bit under examination; the bit is shifted right
    on each iteration of the algorithm.  After the algorithm examines
    the least significant bit of the root, the tracking bit is shifted
    completely out of the bit mask, leaving the bit mask equal to zero
    and terminating the algorithm.

    Note that the "...10..." to "...01..." transformation can take place when
    the very first estimate, the initial estimate, is greater than the square
    root.  In the 16-bit example above, the initial estimate can be shifted
    right bit by bit until a value less than or equal to the root is found.
    Suppose we're finding the square root of 529, which is 23 (0x17, 10111
    binary).  The square root will be refined as follows at each step:

				Precomputed initial estimate is 128
        0000 0000 1000 0000
				Estimate 128 > actual root 23, Shift bit right
        0000 0000 0100 0000
				Estimate 64 > actual root 23, Shift bit right
        0000 0000 0010 0000
				Estimate 32 > actual root 23, Shift bit right
        0000 0000 0001 0000
				Estimate 16 < actual root 23, Start new bit
        0000 0000 0001 1000
				Estimate 24 > actual root 23, Shift bit right
        0000 0000 0001 0100
				Estimate 20 < actual root 23, Start new bit
        0000 0000 0001 0110
				Estimate 22 < actual root 23, Start new bit
        0000 0000 0001 0111
				Estimate 23 == actual root 23, return!


    RISK OF OVERFLOW
    ----------------

    When the width, m, of an unsigned integer is even, only the lower half
    of the integer is manipulated and the maximum value of that lower half
    is UM_MAX_SQRT--all m/2 bits set.  Consequently, there is no danger of
    overflow in the loop if the algorithm computes the square of an
    intermediate root for the purpose of comparing it to the target
    number; e.g., "(root * root) < number".

    However, if m is odd, the square root of the maximum representable value,
    UM_MAX, occupies (m+1)/2 bits and not all bits in that lower "half" are
    set.  Consequently, computing "root * root" could overflow.  For example,
    imagine a CPU that represents all integers, signed and unsigned, as 16-bit,
    signed-magnitude numbers where the most significant bit is the sign.  The
    precision of integers is effectively 15 bits and:

        UM_MAX      = 0x7FFF (32767 decimal)    0111 1111 1111 1111
        UM_MAX_SQRT = 0x00B5 (181 decimal)      0000 0000 1011 0101

    The most significant bit of UM_MAX_SQRT is, as with a full 16-bit word,
    bit 7:

                            0000 0000 1000 0000

    This is the initial estimate (128) of the square root and, if the estimate
    is less than the actual root (i.e., the target number is between 129 and
    UM_MAX_SQRT), the algorithm will set bit 6 (0x00C0, 192 decimal):

                            0000 0000 1100 0000

    192 is greater than UM_MAX_SQRT.  In the next iteration for bit 5,
    multiplying 192 times 192 will overflow UM_MAX.  My code avoids this
    by comparing the intermediate root to the target number divided by
    the intermediate root; e.g., the less-than comparison is transformed
    thus, which removes the possibility of overflow:

        (root * root) < number    ==>    root < (number / root)

    (I didn't need to look as far afield as signed-magnitude numbers!
    I later came across someone else's function that computes the square
    root of a signed, two's-complement, 32-bit integer.  The effective
    precision in this case is 31 bits and, when multiplying "root * root",
    the function would regard a negative result as a sign of overflow.)


    DIFFERENCES
    -----------

    My implementation of the algorithm differs from Ross Fosler's in the
    following ways:

        (1) Fosler's and others' integer square root functions hard-code the
            initial estimate.  Consequently, there may be separate functions
            for computing the square root of, for example, 16- and 32-bit
            numbers, each function with its own, appropiate, hard-coded
            initial estimate.  Separate functions were necessary in Fosler's
            case because he was writing in assembly language for a specific
            class of microcontrollers and the handling of 32-bit numbers at
            that level is more involved than the handling of 16-bit numbers.

            My C function accepts a generic, "unsigned long" integer and
            returns the unsigned long square root of the integer.  On the
            first call to nbrUnsignedSQRT(), the function precomputes and
            caches (i) how many bits wide an unsigned long is and (ii) the
            initial estimate, which has a single bit set corresponding to
            the most significant bit of the maximum square root.  Thus,
            nbrUnsignedSQRT() will work whatever the size of an unsigned
            long in the target compiler/CPU environment.

            The precomputed parameters are normally stored internally in
            nbrUnsignedSQRT().  Tasks running in a shared address space
            can each allocate their own memory for the parameters; see
            nbrUnsignedSQRT()'s context argument.

        (2) Because of the danger of overflow when using integers with
            an odd number of bits, I at first used the non-overflowing
            "root relop number/root" comparisons in nbrUnsignedSQRT().
            As Ross Fosler knew before me, division operations are
            expensive, even on an Intel-based Linux PC.  I mean, I knew
            that division operations are expensive, but I'll admit I was
            surprised when I saw how much of an impact the division
            operations had on nbrUnsignedSQRT()'s performance.

            To increase the performance in the more common case, my
            function chooses the correct form of comparisons based
            on the bit width of unsigned longs.  For even-precision
            unsigned integers, nbrUnsignedSQRT() can safely use these
            fast comparisons:

                          root*root relop number

            without fear of overflow.  For odd-precision unsigned integers,
            nbrUnsignedSQRT() reverts to the slow comparisons:

                          root relop number/root

        (3) Ross Fosler's algorithm calculates the nearest integer square
            root of a number when floor[sqrt(n)] is even.  For example,

                sqrt(15450) = 124.3	integer root is 124
                sqrt(15475) = 124.4	integer root is 124
                sqrt(15500) = 124.5	integer root is 125
                sqrt(15525) = 124.6	integer root is 125

            This makes sense since the least significant bit of an even root
            is "0" and thus offers a place for the algorithm to "start a new
            bit" right before shifting the bit-under-examination bit out of
            its variable and, since that bit mask is now zero, terminating
            the algorithm.

            Odd roots already have a least significant bit of "1", so there
            is no way to "start a new bit" and incrementing the bit to round
            up the root would propagate changes back through the more
            significant bits.  For example,

                 sqrt(81) =  9		integer root is  9
                 sqrt(86) =  9.3	integer root is  9
                 sqrt(89) =  9.4	integer root is  9
                 sqrt(91) =  9.5	integer root is  9
                 sqrt(92) =  9.6	integer root is  9
                 sqrt(99) =  9.95	integer root is  9
                sqrt(100) = 10		integer root is 10

            I wanted nbrUnsignedSQRT() to return the largest integer less
            than or equal to the actual square root--floor[sqrt(n)].  My
            function originally performed an extra "iteration" that
            transformed the least significant bit and a virtual first
            fractional bit from "1.0" to "0.1", which resulted in the
            desired behavior.

            Tristan Muntsinger's optimization moves the shift and test of
            the bit-under-examination mask above/before the "start new bit"
            operation, so his algorithm terminates before it can "start
            a new bit" in the least-significant bit location.  Since his
            algorithm always returns floor[sqrt(n)], when I incorporated
            his optimization into my code, the extra iteration was no longer
            necessary.

    As an addendum to the differences above, I show here how my code evolved
    with better understanding and with exposure to others' algorithms.

    Using Tristan Muntsinger's shorter and cleaner variable names, here's my
    original interpretation of Ross Fosler's algorithm, based on his example:

        -- Variable names: g = root, c = bitmask, n = number

        g = 1			-- Find most significant bit of square root.
        while (g < n/g)		-- (Instead of hard-coding 0x8000.)
            g <<= 1
        g >>= 1

        c = g >> 1		-- Refine value of square root.
        while (c != 0) {
            if (g > n/g)	-- Intermediate root too high?
                g ^= c << 1	-- Begin changing "...10..." to "...01..."
            else if (g == n/g)	-- Final square root?
                c = 0		-- Exit loop.
            g |= c		-- Insert 1-bit if root too high or low.
            c >>= 1
        }

        if (g > n/g)  g--	-- Ensure root is floor[sqrt(n)].

        return (g)

    A little loose and slow.  Here's Ross Fosler's actual, much tighter
    algorithm, gleaned from his code:

        -- Variable names: g = RES, c = BITLOC, n = ARGA, save = TEMP

        save = 0
        g = c = 0x8000
        do {
            if (g*g > n)	-- Intermediate root too high?
                g = save	-- Get last guess with current bit cleared.
            else
                save = g	-- Save current guess before setting next bit.
            c >>= 1
            g |= c		-- Insert 1-bit.
        } while (c != 0)

        return (g)		-- Final root may not be floor[sqrt(n)].

            ASIDE:  There are no PIC18 arithmetic shift instructions,
            only rotate instructions, with or without the carry flag.
            Fosler's Sqrt16() function computes the 8-bit root of a
            16-bit number.  The location bit, c/BITLOC, is shifted
            right with a rotate right without the carry flag.  The
            algorithm terminates not when c/BITLOC is zero, but when
            the location bit circles around from bit 0 to bit 7 of
            c/BITLOC!  His Sqrt32() function already has to use the
            rotate right with carry instruction to shift the two-byte
            c/BITLOC, so the algorithm terminates after the location
            bit rotates out of bit 0 of c/BITLOC into the carry flag.
            Testing bit 7 in Sqrt16() and the carry flag in Sqrt32()
            are semantically equivalent to testing if c/BITLOC is zero.

    When the intermediate root is too high, my code handled the "...10..."
    to "...01..." transition by clearing the previous bit and setting the
    current bit.  Ross Fosler's code retrieved the last good value (in
    which the current bit *was* zero) and set the next bit.  (NOTE that
    my code's "current bit" is in advance of Fosler's and Muntsinger's
    current bits; i.e., my current bit is the 0-bit in "...10..." while
    Fosler's and Muntsinger's current bit is the immediately preceding 1-bit.
    This happens when you try to deduce an algorithm from an example!)

    Tristan Muntsinger's ingenious tweak to the general algorithm
    eliminates the need for my previous-bit fiddling or Fosler's
    last-good-value variable by moving the test for the final square
    root (last bit shifted out of bitmask c, leaving it zero),

            c >>= 1		-- Fragment of Fosler's code.
            g |= c
        } while (c != 0)

    up before the insertion of the 1-bit:

            c >>= 1		-- Fragment of Muntsinger's code.
            if (c == 0)  return (g)
            g |= c

    Here's his algorithm in full:

        g = c = 0x8000
        for ( ; ; ) {
            if (g*g > n)  g ^= c
            c >>= 1
            if (c == 0)  return (g)
            g |= c
        }

    Muntsigner's algorithm also ensures that floor[sqrt(n)] is returned.
    Ultimately, I adopted his algorithm, adapting it for whatever size,
    fixed-width, unsigned long integers supported by the platform.  The
    characteristics of the platform are computed on the first call to
    nbrUnsignedSQRT() and cached for subsequent use.  The characteristics
    are (i) the precision, odd or even, of unsigned longs and (ii) the
    most significant bit of the maximum square root, sqrt(ULONG_MAX).

    My final algorithm, with support for odd-precision numbers:

					-- e.g., 0x80000000 (64-bit).
        g = c = cached-most-significant-bit
        for ( ; ; ) {
            if (cached-odd-precision-flag) {
                if (g > n/g)  g ^= c	-- Odd precision requires division.
            } else if (g*g > n) {	-- Even precision allows multiplication.
                g ^= c
            }
            c >>= 1
            if (c == 0)  return (g)
            g |= c
        }


    PERFORMANCE
    -----------

    Ross Fosler's algorithm is a straightforward (now that he came up with
    it for us!) and efficient means of computing the integer square root of
    a number.  The algorithm is fast (i) when compared to an adaptation of
    the Newton-Raphson method to integers and (ii) because it avoids division
    operations, which the Newton-Raphson method would require.  The latter
    was especially important because division was a slow operation on the
    microprocessor he was targeting.

    (I don't know if Fosler invented the algorithm or if the algorithm
    is independently discovered by people who put their mind to it.  His
    application note, however, is widely cited on the internet and in some
    academic papers.)

    As I mentioned in the DIFFERENCES section, my use of division to avoid
    overflow in root to number comparisons, "root relop number/root", was
    very slow and switching to "root*root relop number" for even-precision
    integers produced a big performance boost.  The former, division-based
    comparisons are, of necessity, still used for odd-precision integers,
    so computing the square root on those platforms will be slow.

    At first, I put the square root testing in the PRIME program, performing
    100 million calculations of:

        sqrt(15241578750190521) = 123456789

    I later moved the square root testing to its own program, SQUINT.
    PRIME/SQUINT could compute a number's square root using any of the
    following functions:

        "nbr" - the default nbrUnsignedSQRT() in the separate LIBGPL library.

        "alternate" - an alternate function used to test changes that will,
            if they work, eventually be incorporated into nbrUnsignedSQRT().

        "crenshaw" - is Jack Crenshaw's integer square root function.  Here's
            his original column, "Integer Square Roots", in EMBEDDED SYSTEMS
            PROGRAMMING, February 1, 1998:

                https://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots

            Unfortunately, the web page is missing the images and code listings.

            The algorithm is also explained in identical or more detail in
            chapter 4 of his book, MATH TOOLKIT FOR REAL-TIME DEVELOPMENT,
            later renamed MATH TOOLKIT FOR REAL-TIME PROGRAMMING.  The
            code I used is the one presented in Listing 4 of his column
            and Listing 4.7 of his book.  (If you know how to use Google,
            you can find chapter 4 online at Google books.)

        "martin" - is Martin Buchanan's function, which bears some
            resemblance to Crenshaw's function.  I used his 3rd integer
            square root function found at the Code Codex:

                https://web.archive.org/web/20220118185505/http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

            (The 3rd function is identical to his second function, but with
            different variable names.)

        "ross" - is my original interpretation of Ross Fosler's algorithm.

        "tristan" - is Tristan Muntsinger's integer square root function,
            which is a variation of Ross Fosler's algorithm.  Martin
            Buchanan included this function on the same Code Codex page:

                https://web.archive.org/web/20220118185505/http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

    The "crenshaw", "martin", and "tristan" functions were modified to
    work at different precisions and were tested using 64-bit unsigned
    longs.  All the functions use the same, one-time determination of
    precision as nbrUnsignedSQRT().  I don't yet understand how they work,
    so Crenshaw's and Martin's functions bail out when faced with
    odd-precision integers.

    Since no changes are being contemplated, the "alternate" function is
    currently identical to nbrUnsignedSQRT().  And Tristan Muntsinger's
    function is nearly identical to the first two.  Consequently, we
    should expect similar timing numbers.

    Instead, I was met with very odd timing.  My testing platform was an
    old PC running 64-bit Fedora Core 15, GCC 4.6.3 20120306, and with an
    "AMD Athlon(tm) 64 X2 Dual Core Processor 3800+" CPU.  (I also did
    some tests on a 2015 or 2016 laptop running Linux Mint Mate 17 or 18
    and on an Android 5.1 tablet with a 32-bit app.)  I won't go into the
    details of the anomalies I saw except to mention the oddest one.

        ASIDE:  While researching this problem, I came across this lengthy
        discussion of the x86 popcnt instruction and pipelining.  The title,
        "Replacing a 32-bit loop counter with 64-bit introduces crazy
        performance deviations", is innocently misleading because the
        observed performance deviations are not caused by the loop counter.
        It's scary that you can't write "deterministic" code that has
        consistent performance.  The performance of your code is dependent,
        no pun intended, on what's in the pipeline, which version of a
        processor you're using, hidden dependencies in the instruction set,
        the compiler writers, etc.  As with division operations being
        expensive, I know all these things, but this discussion really
        made plain to me how insidious they can be.

            https://stackoverflow.com/questions/25078285/replacing-a-32-bit-loop-counter-with-64-bit-introduces-crazy-performance-deviati

    LIBGPL's nbrUnsignedSQRT() is identical to PRIME's "alternate"
    function, both in the C code and in the GCC-generated, "-O1" assembly
    listings: the same sequence of instructions, the same registers, and
    the same addressing modes.  Yet nbrUnsignedSQRT() took 14 seconds for
    the 100 million calculations, while PRIME's unsignedSQRT_A() took 17
    seconds.  Why the big difference?  The assembly code for the call site
    in PRIME simply loaded the function pointer, sqrtF, into a register and
    made an indirect call to the selected function through the register.
    So there was no differentiation in the function call between the
    external nbrUnsignedSQRT() and the internal, static unsignedSQRT_A().
    Moving unsignedSQRT_A() to a separate source file resulted in the timing
    for the alternate function dropping from 17 seconds to 15 seconds, a
    significant reduction, but still one second slower than nbrUnsignedSQRT().

    Ultimately, I moved the square root program and different functions into
    squint.c and tested the algorithms using (i) no optimization, (ii) "-O1",
    and (iii) "-O3".  I don't know why, but I got what seemed like reasonable
    results from this setup.  The command lines looked as follows:

        % squint 15241578750190521[,<algo>] -time -repeat 100000000

    With no optimization, all the algorithms took longer than 30 seconds.

    Using "-O1" in LIBGPL and in SQUINT:

        LIBGPL sqrt() ...
            14.44 seconds (nbr)
            14.40 seconds (nbr)
        Alternate sqrt() ...
            14.42 seconds (alt)
            14.42 seconds (alt)
        Crenshaw sqrt() ...
            21.21 seconds (crenshaw)
            21.27 seconds (crenshaw)
        Martin sqrt() ...
            11.30 seconds (martin)
            11.21 seconds (martin)
        Tristan sqrt() ...
            14.31 seconds (tristan)
            14.32 seconds (tristan)

    Note that Martin Buchanan's function takes about 11 seconds and Jack
    Crenshaw's function is nearly twice as slow, taking about 21 seconds.

    Using "-O3":

        LIBGPL sqrt() ...
            14.37 seconds (nbr)
            14.27 seconds (nbr)
        Alternate sqrt() ...
            14.25 seconds (alt)
            14.24 seconds (alt)
        Crenshaw sqrt() ...
            10.81 seconds (crenshaw)
            10.80 seconds (crenshaw)
        Martin sqrt() ...
            8.94 seconds (martin)
            8.95 seconds (martin)
        Tristan sqrt() ...
            14.33 seconds (tristan)
            14.29 seconds (tristan)

    Buchanan's function drops a little over 2 seconds, but Crenshaw's cuts
    its "-O1" times in half!

    Conclusion: Buchanan's and Crenshaw's functions are sensitive to
    optimization levels.  The functions based on Tristan Muntsinger's
    algorithm have pretty consistent performance at any non-zero level
    of optimization.

    All in all, I've been pleased with the performance of Fosler's and
    Muntsinger's algorithms and I'm satisfied with the decision to use
    their algorithms in nbrUnsignedSQRT().


    UNDERSTANDING ROSS FOSLER'S ASSEMBLY CODE
    -----------------------------------------

    A good book on designing and developing PIC18-based projects is Han-Way
    Huang's, PIC MICROCONTROLLER: AN INTRODUCTION TO SOFTWARE AND HARDWARE
    INTERFACING.  I used Chapter 2, "PIC18 Assembly Language Programming",
    to figure out how Ross Fosler's code worked.  The chapter is a good
    tutorial, but groups of related instructions are listed in tables
    scattered throughout the chapter and I had to browse around looking
    for the instructions I was trying to decipher.  (And if you're serious
    about PIC18 programming, you'll need Chapter 1 to learn about the CPU
    architecture: registers, memory layout, etc.)  Section 4.10.1, in
    Chapter 4, presents a flowchart for computing an integer square root
    that is basically the same as Ross Fosler's.  The book's flowchart is
    at a more abstract level, using a counter, i, and array notation, NUM[i],
    to address individual bits in a number.  Actually implementing the book's
    algorithm would require bit masking and shifting ... in short, designing
    and writing code similar to Fosler's.  (The PIC18 does have bit set,
    clear, and toggle instructions that operate on numbered bits -- 0-7 in
    a byte!  Fosler was counting clock cycles, so determining the byte and
    bit offset of a numbered bit in a 16-bit word would be more trouble and
    more time consuming than bit masking and shifting.)

    If you can't get a hold of Huang's book, you can look up information
    about the PIC18 architecture on the web and couple that knowledge with
    the "PIC18F Instruction Set" to figure out Fosler's code.  That's
    Appendix D from MICROCONTROLLER THEORY AND APPLICATIONS WITH THE PIC18F
    by M. Rafiquzzaman:

        https://onlinelibrary.wiley.com/doi/pdf/10.1002/9781119448457.app4

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  "nbr_util.h"			/* Number utilities. */


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  nbr_util_debug

#if !defined(HAVE_BINARY_INTEGERS) || HAVE_BINARY_INTEGERS
/*!*****************************************************************************

Procedure:

    nbrUnsignedSQRT ()

    Compute the Unsigned Integer Square Root of a Number.


Purpose:

    Function nbrUnsignedSQRT() uses a fast algorithm to compute the
    integer square root of a number.  The value returned is the floor
    of the real square root; that is to say, the integer portion of
    the square root with the fractional part, if any, discarded.  For
    example, the integer square root of 25, 26, 27, ..., and 35 is 5;
    the integer square root of 36 through 48 is 6 and 49 through 63 is 7.

    This function makes use of the algorithm presented in:

        "Fast Integer Square Root" by Ross M. Fosler
        Application Note TB040, DS91040A, (c) 2000 Microchip Technology Inc.
        http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf

    and an ingenious tweak added by Tristan Muntsinger:

        See the last function, sqrt32(), in the C section at:
        http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C


    Invocation:

        root = nbrUnsignedSQRT (number, &context) ;

    where

        <number>	- I
            is the number whose square root is to be computed.
        <context>	- I/O
            points to a context structure containing information needed by
            the function.  If this argument is NULL, the function will compute
            and save the information internally on the first call.  An
            application can allocate its own context structure, a necessity
            in a multi-tasking environment with a shared address space.  In
            this case, set the "precision" field to 0 before the first call
            to the function.  The function will compute the needed values
            based on ULONG_MAX and store them in the application's structure.
            An application or test program wishing to play around with
            different precisions can populate the fields with its own values,
            especially remembering to set the precision to a non-zero value;
            the function will then use the caller-supplied values as is.
        <root>		- O
            returns the integer square root of the number.  In particular,
            floor[sqrt(n)] is returned, so the return value will be the
            largest whole number less than or equal to the actual square
            root--the integer square root is not rounded *up* to a root
            whose square would exceed the number.

*******************************************************************************/


root_t  nbrUnsignedSQRT (

#    if PROTOTYPES
        root_t  number,
        UnsignedSqrtContext  *context)
#    else
        number, context)

        root_t  number ;
        UnsignedSqrtContext  *context ;
#    endif

{    /* Local Variables. */
    root_t  bitmask, root ;
    static  UnsignedSqrtContext  defaultContext = { false, 0, 0 } ;



/*******************************************************************************

    On the first call to nbrUnsignedSQRT(), (i) use ULONG_MAX to determine how
    many bits of precision an unsigned long has and (ii) compute the initial
    estimate for the square root as the largest power of 2 less than or equal
    to the maximum square root for an unsigned long.

    The maximum square root is simply the integer square root of ULONG_MAX.
    If an unsigned long is an even number of bits wide, the maximum square
    root is all bits set to "1" in the least significant half of the integer.
    For example, with 32-bit unsigned longs, the square root of ULONG_MAX
    (0xFFFFFFFF) is 0x0000FFFF; the lower 16 bits are all "1"s.  The initial
    estimate then includes only the most significant "1" bit of the maximum
    square root: 0x00008000.

    If an unsigned long is an odd number of bits wide, things are a bit
    different, a fact better illustrated by example.  ULONG_MAX of a 31-bit
    unsigned long is 0x7FFFFFFF and its maximum square root is 0xB504,
    sixteen bits wide as with its 32-bit cousin, except that not all of the
    least significant bits are set.  This can cause a problem when refining
    the initial estimate.  The 4 most significant bits of maximum square
    root 0xB504 are "1011" binary.  On the first iteration, if the initial
    estimate (0x8000) squared is less than the input number, the square root
    algorithm will set the next bit on the right: binary "1000000000000000"
    becomes "1100000000000000".  Since 0xC000 is larger than the maximum
    square root of 0xB504, squaring it will overflow ULONG_MAX.  To avoid
    this, don't square the root when comparing the square to the input
    number, "root*root relop number", but instead divide the number by the
    intermediate square root: "root relop number/root".

*******************************************************************************/

    if (context == NULL)  context = &defaultContext ;
		/* On first call, compute parameters for CPU/compiler. */
    if (context->precision == 0) {
		/* Determine the number of bits in a root integer
		   by shifting a 1 to the left until it disappears. */
        context->estimate = 1 ;
        context->precision = 1 ;
        while ((context->estimate <<= 1) != 0) {
            context->precision++ ;
        }
        context->oddPrecision = ((context->precision % 2) != 0) ;
		/* Set the initial estimate to the most significant bit of
		   the maximum square root. */
        if (context->oddPrecision)
            context->estimate = ROOT_T_C(1) << (context->precision / 2) ;
        else
            context->estimate = ROOT_T_C(1) << ((context->precision - 1) / 2) ;
        LGI "(nbrUnsignedSQRT) Context:  odd = %s  precision = %"PRIuSIZE_T" bits  estimate = %"PRIuROOT_T" (0x%"PRIXROOT_T")\n",
            context->oddPrecision ? "true" : "false",
            context->precision,
            context->estimate, context->estimate) ;
    }

/* Working from the most significant bit of the square root (which is the only
   bit set in the initial approximation above) down to the least significant
   bit, refine the value of the square root.  Because of the control flow
   used here, borrowed from Tristan Muntsinger's algorithm, the result
   returned to the calling program is always the largest integer less than
   or equal to the actual square root of the input argument.  When I used
   Ross Fosler's original algorithm, I had to make a final check to ensure
   that the result was not the next integer following the actual square root. */

    root = context->estimate ;	/* Use fixed, pre-computed estimate. */

    bitmask = root ;		/* Keeps track of current bit being examined. */

    for ( ; ; ) {

		/* The current bit is always "1".  If the intermediate root
		   is greater than the actual square root, zero out the bit
		   (exclusive-OR of the "1" in root and the "1" in bitmask)
		   and, at the end of the loop, plug in a "1" in the next
		   least significant bit.  Thus "...10..." becomes "...01...",
                   or, as Ross Fosler calls it in his flowchart, "Shift bit
		   right". */

#if !defined(NBR_USQRT_CHECK_ODD_PRECISION) || NBR_USQRT_CHECK_ODD_PRECISION
        if (context->oddPrecision) {
            if (root > (number / root))  root ^= bitmask ;
        } else
#endif
        if ((root * root) > number) {
            root ^= bitmask ;
        }

		/* If all bits have been examined, return the final root. */

        bitmask >>= 1 ;

        if (bitmask == 0)  return (root) ;

		/* Plug in a "1" in the next least significant bit position
		   (now the current bit position because of the shift above). */

        root |= bitmask ;

    }

}
#endif	/* HAVE_BINARY_INTEGERS */
