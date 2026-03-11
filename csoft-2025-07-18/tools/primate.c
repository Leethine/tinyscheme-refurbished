/* $Id: primate.c,v 1.5 2021/09/16 01:45:03 alex Exp alex $ */
/*******************************************************************************

Program:

    primate.c

    PRIMe Algorithm TEster (PRIMATE)


Author:    Alex Measday


Purpose:

    PRIMATE does various things with prime numbers.  It was written to test
    my prime-number functions in LIBGPL's NBR_UTIL package.  Perhaps it may
    be of further use!


    BASIC USAGE
    -----------

    Check if a number is prime:

        % primate <number>

    List all primes in the range, [<start>...<end>], inclusive:

        % primate -list <start>-<end>

    List <count> primes beginning with <start>:

        % primate -list <start>,<count>

    Find the next prime after a number (even if the number itself is prime):

        % primate -next <number>

    Verify that the prime functions generate the correct primes in 3 ranges:
    the first 1000 prime numbers (2, 3, ...), the last 1000 32-bit prime
    numbers, and the last 1000 64-bit prime numbers.  Internally, the prime
    number functions use a type "prime_t" to represent prime and composite
    numbers in their calculations.  If prime_t is 32 bits wide or more,
    the last 1000 32-bit prime numbers can be tested with "-verify last32".
    If prime_t is 64 bits wide or more, the last 1000 64-bit prime numbers
    can be tested with "-verify last64".  The generic "-verify last" option
    chooses the larger of the two bit widths, 32 or 64, which can fit in a
    prime_t.  NOTE that testing the last 1000 64-bit prime numbers takes a
    good bit of time.  Pre-computed tables contain the primes in the 3 ranges.

        % primate -verify first		# First 1000 prime numbers.
        % primate -verify last		# Last 1000 32- or 64-bit prime numbers.
        % primate -verify last32	# Last 1000 32-bit prime numbers.
        % primate -verify last64	# Last 1000 64-bit prime numbers.


    ADVANCED USAGE
    --------------

    Time an operation; most output is suppressed in timing mode.  If the
    operation is very quick, repeat the operation <count> times to get
    meaningful times for comparison purposes.

        % primate -time [-repeat <count>] ... (other options) ...

    Use an alternate is-prime? function rather than the default nbrIsPrime()
    library function.  (Option names and arguments can be abbreviated as far
    as desired, so "-a s" is as good as "-alternate skip23".)

			# Check every single divisor (hard-coded).
        % primate -alt ones ... (other options) ...
			# Check odd divisors, skip evens (hard-coded).
        % primate -alt odds ... (other options) ...
			# Skip odds divisible by 3 and evens (hard-coded).
        % primate -alt skip23 ... (other options) ...
			# Built-in version of nbrIsPrime() (sequence-driven).
        % primate -alt diffs ... (other options) ...

    Select a different skip-through prime for the default nbrIsPrime() or
    built-in "diffs" functions; the default is STP 7.  This option also
    generates the sequence of differences for the new skip-through prime;
    the often lengthy time taken to generate a sequence is not included
    in timing benchmarks ("-time").

        % primate -qprime <stp> ... (other options) ...

    Print information (sequence length, range, etc.) about all skip-through
    primes that can be used without overflow according to the size of an
    unsigned long:

        % primate -qinfo

    Print the same information for 128-bit unsigned integers (using a GCC
    extension for 128-bit integers which is ONLY used in this program for
    generating this information):

        % primal -q128info

    List the sequence of differences for a skip-through prime:

        % primate -qgen <stp>

    Rarely used options not discussed here: "-qc#kpi <stp>", "-qmax <stp>",
    and "-q128info".


    Invocation:

        % primate [-debug] [-help]
						# Operations
                  [-list <start>]
                  [-list <start>,[-]<count>]
                  [-list <start>-<end>]
                  [-maximum 32|64]
                  [-next <number>]
                  [-qc#kpi <stp>] [-qgen <stp>]
                  [-qinfo] [-q128info]
                  [-qmax <stp>]
                  [-verify first|last|last32|last64] [-xtra]
                  [<number>]
						# Modifiers
                  [-alternate ones|odds|skip23|diffs]
                  [-qprime <stp>]
                  [-repeat <count>] [-time]

    where:

        "-debug"
            turns debug on.
        "-help"
            prints out a help message.
        "-list <start>"
        "-list <start>,[-]<count>"
        "-list <start>-<end>"
            generates a list of prime numbers.  If "<start>" alone is
            specified, the program generates prime numbers beginning
            at the start number (which may or may not be prime itself);
            the program will continue running until PRIME_T_MAX is reached
            and checked.  If "<start>,<count>" is specified and <count> is
            negative, the last <count> prime numbers preceding and ending
            with <start> are generated (in reverse order).  If <count> is
            positive, the first <count> prime numbers beginning with and
            following <start> are generated.  If a range is specified,
            "<start>-<end>", the program will generate a list of the
            prime numbers in that range, including the start and/or end
            numbers if they're prime.
        "-maximum 32|64"
            checks if the largest, pre-computed 32- or 64-bit prime number
            is prime (which, of course, it is).  "prime -max 64" takes
            nearly 40 seconds on my old, Fedora Core 15 computer to test
            the primality of the largest 64-bit prime (using the default
            skip-through prime of 7).
        "-next <number>"
            prints out the next prime number after the given number,
            regardless if the given number is itself prime.
        "-qc#kpi <stp>"
            generates a listing of the values of i coprime with c#, where
            c is the specified skip-through prime.  These values are
            associated with the c#k+i form for primes; see the file prolog
            in nbr_util.c for a detailed description.  The program derives
            the values of i from the skip-through prime's sequence of
            differences, so the same memory and time constraints of the
            "-qprime <stp>" option apply.  (The "p" in "c#kpi" means "plus",
            i.e., "c#k+i".)
        "-qgen <stp>"
            generates and lists the sequence of differences for skip-through
            prime <stp>.  This operation requires allocating memory for the
            sequence of differences.  My computer (64-bit Linux) successfully
            allocated the 1GB sequence for STP 29, but failed when trying to
            allocate the 30GB sequence for STP 31.  Large skip-through primes
            have long sequences, so be aware that the output of this option
            can be voluminous; for example, one billion numbers are listed
            for STP 29.
        "-qinfo"
            displays a table of the characteristics of all possible
            skip-through primes and their sequences of differences,
            given the size of unsigned longs on the CPU.
        "-q128info"
            displays a listing of the characteristics of all possible
            skip-through primes and their sequences of differences for
            128-bit, unsigned integers.  (The numbers are very, very
            big, so a table was out of the question.)  This option takes
            advantage of GCC's (and others') non-standard implementation
            of 128-bit arithmetic on 64-bit CPUs.  This option is only
            available if C preprocessor symbol HAVE___INT128 is defined
            as 1 when this program is compiled with GCC.  This GCC
            extension for 128-bit integers is *not* used elsewhere in
            the program or in the NBR_UTIL library.  So the checking,
            listing, and verifying of 128-bit integers are not supported
            (unless the prime_t data type is actually 128 bits wide).
        "-qmax <stp>"
            determines the maximum difference in the sequence of differences
            for skip-through prime <stp>.  The algorithm used is identical to
            the algorithm used by "-qgen <stp>", but it is not necessary to
            allocate memory for the sequence.  As each difference is generated,
            it is compared to the maximum seen so far and then discarded.
            Consequently, this operation is useful as a stand-in for
            "-qgen <stp>" when benchmarking sequence generation for
            skip-through primes whose sequences require more memory than
            the system can allocate.  Be aware that this is a time-consuming
            process for larger skip-through primes.  On my computer, it took
            11 seconds--at 100% CPU usage--to scan the differences for STP 23,
            nearly 7 minutes for STP 29, 3.5 hours for STP 31, and 6.4 *days*
            for STP 37!
        "-verify first|last|last32|last64"
            verifies that the prime function generates the correct prime
            numbers in certain ranges.  The verification immediately
            terminates with an error message if a non-prime number is
            generated or if an actual prime number is skipped.  The
            generated prime numbers are compared to one of the tables
            of pre-computed primes in 3 ranges:
            (1) "-verify first" verifies the first 1000 prime numbers.
            (2) "-verify last64" verifies the last 1000 prime numbers
                less than 2^64.  This option is not available if the
                prime_t data type is less than 64 bits.
            (3) "-verify last32" verifies the last 1000 prime numbers
                less than 2^32; this option is useful if prime_t is
                wider than 32 bits (e.g., 64 bits).  This option is
                not available if the prime_t data type is less than
                32 bits.
            (4) "-verify last" verifies the last 1000 32- or 64-bit
                prime numbers that can be represented in the prime_t
                data type.  If the width of a prime_t is 64 bits or
                greater, the last 1000 prime numbers less than 2^64
                are verified.  If the width is less than 64 bits, the
                last 1000 prime numbers less than 2^32 are verified.
            NOTE that "-verify last" on a CPU with 64-bit numbers takes
            a very long time.  On my old Fedora Core 15 machine, 24.8
            hours to be exact!
        "-xtra"
            checks if 4294945091 is prime (it isn't).  A shorthand option
            for me when I was developing this program.
        "<number>"
            checks if the given number is prime.
        "-alternate ones|odds|skip23|diffs"
            use an alternate is-prime? function for comparison purposes:
            (1) The "ones" function implements a "fast", hand-coded loop
                that checks every single divisor: 2, 3, 4, 5, 6, 7, ...
            (2) The "odds" function implements a fast, hand-coded loop
                that checks all odd divisors.  Using this option along
                with "-time" provides baseline timing against which more
                optimized functions can be compared.
            (3) The "skip23" function implements a fast, hand-coded loop
                that skips divisors divisible by 2 or by 3; this is
                equivalent to having a skip-through prime of 3, but
                it does not use the generic, sequence of differences
                algorithm.
            (4) The "diffs" function implements the generic, sequence
                of differences algorithm for different skip-through
                primes.  This function is virtually identical to the
                library function, nbrIsPrime(), except you can modify
                or replace the function in the test program in order
                to experiment with your own optimizations.  As with
                nbrIsPrime(), the default skip-through prime is 7;
                a different skip-through prime can be specified with
                the "-qprime <stp>" option.
        "-qprime <stp>"
            specifies the skip-through prime to use with the library function,
            nbrIsPrime(), or with this program's internal "-alternate diffs"
            function.  For these functions to use the desired skip-through
            prime, the "-qprime <stp>" option must generate the sequence of
            differences for the STP.  On my computer, it took 11 seconds to
            generate the sequence for STP 23 and nearly 7 minutes for STP 29.
            The system couldn't allocate enough memory for STP 31's sequence,
            but it would take 3.5 hours to generate STP 31's sequence and
            6.4 *days* to generate STP 37's sequence!
        "-repeat <count>"
            repeats certain operations the specified number of times.  This
            is useful, for example, when timing checking if a number is prime.
            A single check might take too little time to make a meaningful
            comparison with, say, an alternate is-prime? function; repeat
            them 1,000 times or 10,000 times and a more realistic comparison
            can be made.
        "-time"
            measures and displays the time it takes to perform an operation
            (or <count> iterations of the operation if "-repeat <count>" is
            also given on the command line).  Almost all output is suppressed
            to keep I/O from affecting the timing.  If the "-qprime <stp>"
            option was specified, the skip-through prime's sequence of
            differences is generated before the timer is started.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <math.h>			/* Math library definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "bmw_util.h"			/* Benchmarking functions. */
#include  "nbr_util.h"			/* Number utilities. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "str_util.h"			/* String manipulation functions. */


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  nbr_util_debug


/*******************************************************************************
    Private Functions
*******************************************************************************/

static  bool  isPrimeOdds (
#    if PROTOTYPES
        prime_t  number,
        void  *config
#    endif
    ) ;

static  bool  isPrimeOnes (
#    if PROTOTYPES
        prime_t  number,
        void  *config
#    endif
    ) ;

static  bool  isPrimeSkip23 (
#    if PROTOTYPES
        prime_t  number,
        void  *config
#    endif
    ) ;

static  bool  isPrimeDiffs (
#    if PROTOTYPES
        prime_t  number,
        void  *config
#    endif
    ) ;

#if defined(HAVE___INT128) && HAVE___INT128
/*******************************************************************************
    sprint128u() - converts an unsigned binary, 128-bit number to its
        ASCII representation and writes the NUL-terminated string into
        a caller-supplied buffer.  The buffer must be able to hold 40
        characters (39 digits for the maximum representable 128-bit
        number and 1 character for the NUL terminator).  This function
        is only usable when the program is compiled with GCC or other
        compilers that provide a non-standard extension for 128-bit
        arithmetic.
*******************************************************************************/

static  char  *sprint128u (

    char  *buffer,			/* 40 characters at least! */
    unsigned  __int128  number)

{    /* Local variables. */
    char  reverse[64] ;
    char  *s = reverse ;
    size_t  i ;


    if (number == 0) {

        *s++ = '0' ;

    } else {

#    define  BASE  10UL

        for (  ;  number != 0 ;  number /= BASE) {
            *s++ = (char) ('0' + (number % BASE)) ;
#ifdef DUMP_INFO
            printf ("=> %lu '%c'\n", (unsigned long) (number % BASE), *(s-1)) ;
#endif
        }

    }

    *s = '\0' ;

    i = s - reverse ;
    s = buffer ;

    while (i-- > 0) {
        *s++ = reverse[i] ;
    }

    *s = '\0' ;

#ifdef DUMP_INFO
    meoDumpX (NULL, "128-bit: ", 0, &number, 16) ;
    meoDumpX (NULL, "Reverse: ", 0, reverse, 48) ;
    meoDumpX (NULL, " String: ", 0, buffer, 48) ;
#endif

    return (buffer) ;

}
#endif

/*******************************************************************************

    Used by isPrimeDiffs(), the following variable can take on the values
    2, 3, 5, 7, and 11.  For a given value, isPrimeDiffs() skips checking
    divisors that are multiples of the value and the preceding values:

        Value: 2	Skip even divisors; only check odd divisors.
        Value: 3	Skip even divisors and divisors divisible by 3
			(e.g., check divisors 5, 7, 11, 13, 17, 19, 23,
			25, 29, 31, 35, ...).
        Value: 5	Skip even divisors and divisors divisible by 3
			and/or 5.
        Value: 7	Skip even divisors and divisors divisible by 3,
			5, and/or 7.
        Value: 11	Skip even divisors and divisors divisible by 3,
			5, 7, and/or 11.

*******************************************************************************/

					/* For use by isPrimeDiffs(). */
static  prime_t  gSkipThroughPrime ;
static  prime_t  gStartDivisor ;
static  prime_t  gRange ;
static  size_t  gLength ;
static  unsigned  char  *gSequence ;

static  size_t  gNumDivisors = 0 ;

/*******************************************************************************

    The first 1000 primes - from the University of Tennessee at Martin's
    "The Prime Pages (prime number research, records, and resources)",
    compiled and actively updated for over 20 years (at the time I write
    this) by Dr. Chris Caldwell.

        https://primes.utm.edu/
        https://primes.utm.edu/lists/small/1000.txt

*******************************************************************************/

#define  MAX_PRIMES_LIST  1000

static  const  prime_t  firstPrimes[MAX_PRIMES_LIST] = {
    PRIME_T_C(2),    PRIME_T_C(3),    PRIME_T_C(5),    PRIME_T_C(7),
    PRIME_T_C(11),   PRIME_T_C(13),   PRIME_T_C(17),   PRIME_T_C(19),
    PRIME_T_C(23),   PRIME_T_C(29),   PRIME_T_C(31),   PRIME_T_C(37),
    PRIME_T_C(41),   PRIME_T_C(43),   PRIME_T_C(47),   PRIME_T_C(53),
    PRIME_T_C(59),   PRIME_T_C(61),   PRIME_T_C(67),   PRIME_T_C(71),
    PRIME_T_C(73),   PRIME_T_C(79),   PRIME_T_C(83),   PRIME_T_C(89),
    PRIME_T_C(97),   PRIME_T_C(101),  PRIME_T_C(103),  PRIME_T_C(107),
    PRIME_T_C(109),  PRIME_T_C(113),  PRIME_T_C(127),  PRIME_T_C(131),
    PRIME_T_C(137),  PRIME_T_C(139),  PRIME_T_C(149),  PRIME_T_C(151),
    PRIME_T_C(157),  PRIME_T_C(163),  PRIME_T_C(167),  PRIME_T_C(173),
    PRIME_T_C(179),  PRIME_T_C(181),  PRIME_T_C(191),  PRIME_T_C(193),
    PRIME_T_C(197),  PRIME_T_C(199),  PRIME_T_C(211),  PRIME_T_C(223),
    PRIME_T_C(227),  PRIME_T_C(229),  PRIME_T_C(233),  PRIME_T_C(239),
    PRIME_T_C(241),  PRIME_T_C(251),  PRIME_T_C(257),  PRIME_T_C(263),
    PRIME_T_C(269),  PRIME_T_C(271),  PRIME_T_C(277),  PRIME_T_C(281),
    PRIME_T_C(283),  PRIME_T_C(293),  PRIME_T_C(307),  PRIME_T_C(311),
    PRIME_T_C(313),  PRIME_T_C(317),  PRIME_T_C(331),  PRIME_T_C(337),
    PRIME_T_C(347),  PRIME_T_C(349),  PRIME_T_C(353),  PRIME_T_C(359),
    PRIME_T_C(367),  PRIME_T_C(373),  PRIME_T_C(379),  PRIME_T_C(383),
    PRIME_T_C(389),  PRIME_T_C(397),  PRIME_T_C(401),  PRIME_T_C(409),
    PRIME_T_C(419),  PRIME_T_C(421),  PRIME_T_C(431),  PRIME_T_C(433),
    PRIME_T_C(439),  PRIME_T_C(443),  PRIME_T_C(449),  PRIME_T_C(457),
    PRIME_T_C(461),  PRIME_T_C(463),  PRIME_T_C(467),  PRIME_T_C(479),
    PRIME_T_C(487),  PRIME_T_C(491),  PRIME_T_C(499),  PRIME_T_C(503),
    PRIME_T_C(509),  PRIME_T_C(521),  PRIME_T_C(523),  PRIME_T_C(541),
    PRIME_T_C(547),  PRIME_T_C(557),  PRIME_T_C(563),  PRIME_T_C(569),
    PRIME_T_C(571),  PRIME_T_C(577),  PRIME_T_C(587),  PRIME_T_C(593),
    PRIME_T_C(599),  PRIME_T_C(601),  PRIME_T_C(607),  PRIME_T_C(613),
    PRIME_T_C(617),  PRIME_T_C(619),  PRIME_T_C(631),  PRIME_T_C(641),
    PRIME_T_C(643),  PRIME_T_C(647),  PRIME_T_C(653),  PRIME_T_C(659),
    PRIME_T_C(661),  PRIME_T_C(673),  PRIME_T_C(677),  PRIME_T_C(683),
    PRIME_T_C(691),  PRIME_T_C(701),  PRIME_T_C(709),  PRIME_T_C(719),
    PRIME_T_C(727),  PRIME_T_C(733),  PRIME_T_C(739),  PRIME_T_C(743),
    PRIME_T_C(751),  PRIME_T_C(757),  PRIME_T_C(761),  PRIME_T_C(769),
    PRIME_T_C(773),  PRIME_T_C(787),  PRIME_T_C(797),  PRIME_T_C(809),
    PRIME_T_C(811),  PRIME_T_C(821),  PRIME_T_C(823),  PRIME_T_C(827),
    PRIME_T_C(829),  PRIME_T_C(839),  PRIME_T_C(853),  PRIME_T_C(857),
    PRIME_T_C(859),  PRIME_T_C(863),  PRIME_T_C(877),  PRIME_T_C(881),
    PRIME_T_C(883),  PRIME_T_C(887),  PRIME_T_C(907),  PRIME_T_C(911),
    PRIME_T_C(919),  PRIME_T_C(929),  PRIME_T_C(937),  PRIME_T_C(941),
    PRIME_T_C(947),  PRIME_T_C(953),  PRIME_T_C(967),  PRIME_T_C(971),
    PRIME_T_C(977),  PRIME_T_C(983),  PRIME_T_C(991),  PRIME_T_C(997),
    PRIME_T_C(1009), PRIME_T_C(1013), PRIME_T_C(1019), PRIME_T_C(1021),
    PRIME_T_C(1031), PRIME_T_C(1033), PRIME_T_C(1039), PRIME_T_C(1049),
    PRIME_T_C(1051), PRIME_T_C(1061), PRIME_T_C(1063), PRIME_T_C(1069),
    PRIME_T_C(1087), PRIME_T_C(1091), PRIME_T_C(1093), PRIME_T_C(1097),
    PRIME_T_C(1103), PRIME_T_C(1109), PRIME_T_C(1117), PRIME_T_C(1123),
    PRIME_T_C(1129), PRIME_T_C(1151), PRIME_T_C(1153), PRIME_T_C(1163),
    PRIME_T_C(1171), PRIME_T_C(1181), PRIME_T_C(1187), PRIME_T_C(1193),
    PRIME_T_C(1201), PRIME_T_C(1213), PRIME_T_C(1217), PRIME_T_C(1223),
    PRIME_T_C(1229), PRIME_T_C(1231), PRIME_T_C(1237), PRIME_T_C(1249),
    PRIME_T_C(1259), PRIME_T_C(1277), PRIME_T_C(1279), PRIME_T_C(1283),
    PRIME_T_C(1289), PRIME_T_C(1291), PRIME_T_C(1297), PRIME_T_C(1301),
    PRIME_T_C(1303), PRIME_T_C(1307), PRIME_T_C(1319), PRIME_T_C(1321),
    PRIME_T_C(1327), PRIME_T_C(1361), PRIME_T_C(1367), PRIME_T_C(1373),
    PRIME_T_C(1381), PRIME_T_C(1399), PRIME_T_C(1409), PRIME_T_C(1423),
    PRIME_T_C(1427), PRIME_T_C(1429), PRIME_T_C(1433), PRIME_T_C(1439),
    PRIME_T_C(1447), PRIME_T_C(1451), PRIME_T_C(1453), PRIME_T_C(1459),
    PRIME_T_C(1471), PRIME_T_C(1481), PRIME_T_C(1483), PRIME_T_C(1487),
    PRIME_T_C(1489), PRIME_T_C(1493), PRIME_T_C(1499), PRIME_T_C(1511),
    PRIME_T_C(1523), PRIME_T_C(1531), PRIME_T_C(1543), PRIME_T_C(1549),
    PRIME_T_C(1553), PRIME_T_C(1559), PRIME_T_C(1567), PRIME_T_C(1571),
    PRIME_T_C(1579), PRIME_T_C(1583), PRIME_T_C(1597), PRIME_T_C(1601),
    PRIME_T_C(1607), PRIME_T_C(1609), PRIME_T_C(1613), PRIME_T_C(1619),
    PRIME_T_C(1621), PRIME_T_C(1627), PRIME_T_C(1637), PRIME_T_C(1657),
    PRIME_T_C(1663), PRIME_T_C(1667), PRIME_T_C(1669), PRIME_T_C(1693),
    PRIME_T_C(1697), PRIME_T_C(1699), PRIME_T_C(1709), PRIME_T_C(1721),
    PRIME_T_C(1723), PRIME_T_C(1733), PRIME_T_C(1741), PRIME_T_C(1747),
    PRIME_T_C(1753), PRIME_T_C(1759), PRIME_T_C(1777), PRIME_T_C(1783),
    PRIME_T_C(1787), PRIME_T_C(1789), PRIME_T_C(1801), PRIME_T_C(1811),
    PRIME_T_C(1823), PRIME_T_C(1831), PRIME_T_C(1847), PRIME_T_C(1861),
    PRIME_T_C(1867), PRIME_T_C(1871), PRIME_T_C(1873), PRIME_T_C(1877),
    PRIME_T_C(1879), PRIME_T_C(1889), PRIME_T_C(1901), PRIME_T_C(1907),
    PRIME_T_C(1913), PRIME_T_C(1931), PRIME_T_C(1933), PRIME_T_C(1949),
    PRIME_T_C(1951), PRIME_T_C(1973), PRIME_T_C(1979), PRIME_T_C(1987),
    PRIME_T_C(1993), PRIME_T_C(1997), PRIME_T_C(1999), PRIME_T_C(2003),
    PRIME_T_C(2011), PRIME_T_C(2017), PRIME_T_C(2027), PRIME_T_C(2029),
    PRIME_T_C(2039), PRIME_T_C(2053), PRIME_T_C(2063), PRIME_T_C(2069),
    PRIME_T_C(2081), PRIME_T_C(2083), PRIME_T_C(2087), PRIME_T_C(2089),
    PRIME_T_C(2099), PRIME_T_C(2111), PRIME_T_C(2113), PRIME_T_C(2129),
    PRIME_T_C(2131), PRIME_T_C(2137), PRIME_T_C(2141), PRIME_T_C(2143),
    PRIME_T_C(2153), PRIME_T_C(2161), PRIME_T_C(2179), PRIME_T_C(2203),
    PRIME_T_C(2207), PRIME_T_C(2213), PRIME_T_C(2221), PRIME_T_C(2237),
    PRIME_T_C(2239), PRIME_T_C(2243), PRIME_T_C(2251), PRIME_T_C(2267),
    PRIME_T_C(2269), PRIME_T_C(2273), PRIME_T_C(2281), PRIME_T_C(2287),
    PRIME_T_C(2293), PRIME_T_C(2297), PRIME_T_C(2309), PRIME_T_C(2311),
    PRIME_T_C(2333), PRIME_T_C(2339), PRIME_T_C(2341), PRIME_T_C(2347),
    PRIME_T_C(2351), PRIME_T_C(2357), PRIME_T_C(2371), PRIME_T_C(2377),
    PRIME_T_C(2381), PRIME_T_C(2383), PRIME_T_C(2389), PRIME_T_C(2393),
    PRIME_T_C(2399), PRIME_T_C(2411), PRIME_T_C(2417), PRIME_T_C(2423),
    PRIME_T_C(2437), PRIME_T_C(2441), PRIME_T_C(2447), PRIME_T_C(2459),
    PRIME_T_C(2467), PRIME_T_C(2473), PRIME_T_C(2477), PRIME_T_C(2503),
    PRIME_T_C(2521), PRIME_T_C(2531), PRIME_T_C(2539), PRIME_T_C(2543),
    PRIME_T_C(2549), PRIME_T_C(2551), PRIME_T_C(2557), PRIME_T_C(2579),
    PRIME_T_C(2591), PRIME_T_C(2593), PRIME_T_C(2609), PRIME_T_C(2617),
    PRIME_T_C(2621), PRIME_T_C(2633), PRIME_T_C(2647), PRIME_T_C(2657),
    PRIME_T_C(2659), PRIME_T_C(2663), PRIME_T_C(2671), PRIME_T_C(2677),
    PRIME_T_C(2683), PRIME_T_C(2687), PRIME_T_C(2689), PRIME_T_C(2693),
    PRIME_T_C(2699), PRIME_T_C(2707), PRIME_T_C(2711), PRIME_T_C(2713),
    PRIME_T_C(2719), PRIME_T_C(2729), PRIME_T_C(2731), PRIME_T_C(2741),
    PRIME_T_C(2749), PRIME_T_C(2753), PRIME_T_C(2767), PRIME_T_C(2777),
    PRIME_T_C(2789), PRIME_T_C(2791), PRIME_T_C(2797), PRIME_T_C(2801),
    PRIME_T_C(2803), PRIME_T_C(2819), PRIME_T_C(2833), PRIME_T_C(2837),
    PRIME_T_C(2843), PRIME_T_C(2851), PRIME_T_C(2857), PRIME_T_C(2861),
    PRIME_T_C(2879), PRIME_T_C(2887), PRIME_T_C(2897), PRIME_T_C(2903),
    PRIME_T_C(2909), PRIME_T_C(2917), PRIME_T_C(2927), PRIME_T_C(2939),
    PRIME_T_C(2953), PRIME_T_C(2957), PRIME_T_C(2963), PRIME_T_C(2969),
    PRIME_T_C(2971), PRIME_T_C(2999), PRIME_T_C(3001), PRIME_T_C(3011),
    PRIME_T_C(3019), PRIME_T_C(3023), PRIME_T_C(3037), PRIME_T_C(3041),
    PRIME_T_C(3049), PRIME_T_C(3061), PRIME_T_C(3067), PRIME_T_C(3079),
    PRIME_T_C(3083), PRIME_T_C(3089), PRIME_T_C(3109), PRIME_T_C(3119),
    PRIME_T_C(3121), PRIME_T_C(3137), PRIME_T_C(3163), PRIME_T_C(3167),
    PRIME_T_C(3169), PRIME_T_C(3181), PRIME_T_C(3187), PRIME_T_C(3191),
    PRIME_T_C(3203), PRIME_T_C(3209), PRIME_T_C(3217), PRIME_T_C(3221),
    PRIME_T_C(3229), PRIME_T_C(3251), PRIME_T_C(3253), PRIME_T_C(3257),
    PRIME_T_C(3259), PRIME_T_C(3271), PRIME_T_C(3299), PRIME_T_C(3301),
    PRIME_T_C(3307), PRIME_T_C(3313), PRIME_T_C(3319), PRIME_T_C(3323),
    PRIME_T_C(3329), PRIME_T_C(3331), PRIME_T_C(3343), PRIME_T_C(3347),
    PRIME_T_C(3359), PRIME_T_C(3361), PRIME_T_C(3371), PRIME_T_C(3373),
    PRIME_T_C(3389), PRIME_T_C(3391), PRIME_T_C(3407), PRIME_T_C(3413),
    PRIME_T_C(3433), PRIME_T_C(3449), PRIME_T_C(3457), PRIME_T_C(3461),
    PRIME_T_C(3463), PRIME_T_C(3467), PRIME_T_C(3469), PRIME_T_C(3491),
    PRIME_T_C(3499), PRIME_T_C(3511), PRIME_T_C(3517), PRIME_T_C(3527),
    PRIME_T_C(3529), PRIME_T_C(3533), PRIME_T_C(3539), PRIME_T_C(3541),
    PRIME_T_C(3547), PRIME_T_C(3557), PRIME_T_C(3559), PRIME_T_C(3571),
    PRIME_T_C(3581), PRIME_T_C(3583), PRIME_T_C(3593), PRIME_T_C(3607),
    PRIME_T_C(3613), PRIME_T_C(3617), PRIME_T_C(3623), PRIME_T_C(3631),
    PRIME_T_C(3637), PRIME_T_C(3643), PRIME_T_C(3659), PRIME_T_C(3671),
    PRIME_T_C(3673), PRIME_T_C(3677), PRIME_T_C(3691), PRIME_T_C(3697),
    PRIME_T_C(3701), PRIME_T_C(3709), PRIME_T_C(3719), PRIME_T_C(3727),
    PRIME_T_C(3733), PRIME_T_C(3739), PRIME_T_C(3761), PRIME_T_C(3767),
    PRIME_T_C(3769), PRIME_T_C(3779), PRIME_T_C(3793), PRIME_T_C(3797),
    PRIME_T_C(3803), PRIME_T_C(3821), PRIME_T_C(3823), PRIME_T_C(3833),
    PRIME_T_C(3847), PRIME_T_C(3851), PRIME_T_C(3853), PRIME_T_C(3863),
    PRIME_T_C(3877), PRIME_T_C(3881), PRIME_T_C(3889), PRIME_T_C(3907),
    PRIME_T_C(3911), PRIME_T_C(3917), PRIME_T_C(3919), PRIME_T_C(3923),
    PRIME_T_C(3929), PRIME_T_C(3931), PRIME_T_C(3943), PRIME_T_C(3947),
    PRIME_T_C(3967), PRIME_T_C(3989), PRIME_T_C(4001), PRIME_T_C(4003),
    PRIME_T_C(4007), PRIME_T_C(4013), PRIME_T_C(4019), PRIME_T_C(4021),
    PRIME_T_C(4027), PRIME_T_C(4049), PRIME_T_C(4051), PRIME_T_C(4057),
    PRIME_T_C(4073), PRIME_T_C(4079), PRIME_T_C(4091), PRIME_T_C(4093),
    PRIME_T_C(4099), PRIME_T_C(4111), PRIME_T_C(4127), PRIME_T_C(4129),
    PRIME_T_C(4133), PRIME_T_C(4139), PRIME_T_C(4153), PRIME_T_C(4157),
    PRIME_T_C(4159), PRIME_T_C(4177), PRIME_T_C(4201), PRIME_T_C(4211),
    PRIME_T_C(4217), PRIME_T_C(4219), PRIME_T_C(4229), PRIME_T_C(4231),
    PRIME_T_C(4241), PRIME_T_C(4243), PRIME_T_C(4253), PRIME_T_C(4259),
    PRIME_T_C(4261), PRIME_T_C(4271), PRIME_T_C(4273), PRIME_T_C(4283),
    PRIME_T_C(4289), PRIME_T_C(4297), PRIME_T_C(4327), PRIME_T_C(4337),
    PRIME_T_C(4339), PRIME_T_C(4349), PRIME_T_C(4357), PRIME_T_C(4363),
    PRIME_T_C(4373), PRIME_T_C(4391), PRIME_T_C(4397), PRIME_T_C(4409),
    PRIME_T_C(4421), PRIME_T_C(4423), PRIME_T_C(4441), PRIME_T_C(4447),
    PRIME_T_C(4451), PRIME_T_C(4457), PRIME_T_C(4463), PRIME_T_C(4481),
    PRIME_T_C(4483), PRIME_T_C(4493), PRIME_T_C(4507), PRIME_T_C(4513),
    PRIME_T_C(4517), PRIME_T_C(4519), PRIME_T_C(4523), PRIME_T_C(4547),
    PRIME_T_C(4549), PRIME_T_C(4561), PRIME_T_C(4567), PRIME_T_C(4583),
    PRIME_T_C(4591), PRIME_T_C(4597), PRIME_T_C(4603), PRIME_T_C(4621),
    PRIME_T_C(4637), PRIME_T_C(4639), PRIME_T_C(4643), PRIME_T_C(4649),
    PRIME_T_C(4651), PRIME_T_C(4657), PRIME_T_C(4663), PRIME_T_C(4673),
    PRIME_T_C(4679), PRIME_T_C(4691), PRIME_T_C(4703), PRIME_T_C(4721),
    PRIME_T_C(4723), PRIME_T_C(4729), PRIME_T_C(4733), PRIME_T_C(4751),
    PRIME_T_C(4759), PRIME_T_C(4783), PRIME_T_C(4787), PRIME_T_C(4789),
    PRIME_T_C(4793), PRIME_T_C(4799), PRIME_T_C(4801), PRIME_T_C(4813),
    PRIME_T_C(4817), PRIME_T_C(4831), PRIME_T_C(4861), PRIME_T_C(4871),
    PRIME_T_C(4877), PRIME_T_C(4889), PRIME_T_C(4903), PRIME_T_C(4909),
    PRIME_T_C(4919), PRIME_T_C(4931), PRIME_T_C(4933), PRIME_T_C(4937),
    PRIME_T_C(4943), PRIME_T_C(4951), PRIME_T_C(4957), PRIME_T_C(4967),
    PRIME_T_C(4969), PRIME_T_C(4973), PRIME_T_C(4987), PRIME_T_C(4993),
    PRIME_T_C(4999), PRIME_T_C(5003), PRIME_T_C(5009), PRIME_T_C(5011),
    PRIME_T_C(5021), PRIME_T_C(5023), PRIME_T_C(5039), PRIME_T_C(5051),
    PRIME_T_C(5059), PRIME_T_C(5077), PRIME_T_C(5081), PRIME_T_C(5087),
    PRIME_T_C(5099), PRIME_T_C(5101), PRIME_T_C(5107), PRIME_T_C(5113),
    PRIME_T_C(5119), PRIME_T_C(5147), PRIME_T_C(5153), PRIME_T_C(5167),
    PRIME_T_C(5171), PRIME_T_C(5179), PRIME_T_C(5189), PRIME_T_C(5197),
    PRIME_T_C(5209), PRIME_T_C(5227), PRIME_T_C(5231), PRIME_T_C(5233),
    PRIME_T_C(5237), PRIME_T_C(5261), PRIME_T_C(5273), PRIME_T_C(5279),
    PRIME_T_C(5281), PRIME_T_C(5297), PRIME_T_C(5303), PRIME_T_C(5309),
    PRIME_T_C(5323), PRIME_T_C(5333), PRIME_T_C(5347), PRIME_T_C(5351),
    PRIME_T_C(5381), PRIME_T_C(5387), PRIME_T_C(5393), PRIME_T_C(5399),
    PRIME_T_C(5407), PRIME_T_C(5413), PRIME_T_C(5417), PRIME_T_C(5419),
    PRIME_T_C(5431), PRIME_T_C(5437), PRIME_T_C(5441), PRIME_T_C(5443),
    PRIME_T_C(5449), PRIME_T_C(5471), PRIME_T_C(5477), PRIME_T_C(5479),
    PRIME_T_C(5483), PRIME_T_C(5501), PRIME_T_C(5503), PRIME_T_C(5507),
    PRIME_T_C(5519), PRIME_T_C(5521), PRIME_T_C(5527), PRIME_T_C(5531),
    PRIME_T_C(5557), PRIME_T_C(5563), PRIME_T_C(5569), PRIME_T_C(5573),
    PRIME_T_C(5581), PRIME_T_C(5591), PRIME_T_C(5623), PRIME_T_C(5639),
    PRIME_T_C(5641), PRIME_T_C(5647), PRIME_T_C(5651), PRIME_T_C(5653),
    PRIME_T_C(5657), PRIME_T_C(5659), PRIME_T_C(5669), PRIME_T_C(5683),
    PRIME_T_C(5689), PRIME_T_C(5693), PRIME_T_C(5701), PRIME_T_C(5711),
    PRIME_T_C(5717), PRIME_T_C(5737), PRIME_T_C(5741), PRIME_T_C(5743),
    PRIME_T_C(5749), PRIME_T_C(5779), PRIME_T_C(5783), PRIME_T_C(5791),
    PRIME_T_C(5801), PRIME_T_C(5807), PRIME_T_C(5813), PRIME_T_C(5821),
    PRIME_T_C(5827), PRIME_T_C(5839), PRIME_T_C(5843), PRIME_T_C(5849),
    PRIME_T_C(5851), PRIME_T_C(5857), PRIME_T_C(5861), PRIME_T_C(5867),
    PRIME_T_C(5869), PRIME_T_C(5879), PRIME_T_C(5881), PRIME_T_C(5897),
    PRIME_T_C(5903), PRIME_T_C(5923), PRIME_T_C(5927), PRIME_T_C(5939),
    PRIME_T_C(5953), PRIME_T_C(5981), PRIME_T_C(5987), PRIME_T_C(6007),
    PRIME_T_C(6011), PRIME_T_C(6029), PRIME_T_C(6037), PRIME_T_C(6043),
    PRIME_T_C(6047), PRIME_T_C(6053), PRIME_T_C(6067), PRIME_T_C(6073),
    PRIME_T_C(6079), PRIME_T_C(6089), PRIME_T_C(6091), PRIME_T_C(6101),
    PRIME_T_C(6113), PRIME_T_C(6121), PRIME_T_C(6131), PRIME_T_C(6133),
    PRIME_T_C(6143), PRIME_T_C(6151), PRIME_T_C(6163), PRIME_T_C(6173),
    PRIME_T_C(6197), PRIME_T_C(6199), PRIME_T_C(6203), PRIME_T_C(6211),
    PRIME_T_C(6217), PRIME_T_C(6221), PRIME_T_C(6229), PRIME_T_C(6247),
    PRIME_T_C(6257), PRIME_T_C(6263), PRIME_T_C(6269), PRIME_T_C(6271),
    PRIME_T_C(6277), PRIME_T_C(6287), PRIME_T_C(6299), PRIME_T_C(6301),
    PRIME_T_C(6311), PRIME_T_C(6317), PRIME_T_C(6323), PRIME_T_C(6329),
    PRIME_T_C(6337), PRIME_T_C(6343), PRIME_T_C(6353), PRIME_T_C(6359),
    PRIME_T_C(6361), PRIME_T_C(6367), PRIME_T_C(6373), PRIME_T_C(6379),
    PRIME_T_C(6389), PRIME_T_C(6397), PRIME_T_C(6421), PRIME_T_C(6427),
    PRIME_T_C(6449), PRIME_T_C(6451), PRIME_T_C(6469), PRIME_T_C(6473),
    PRIME_T_C(6481), PRIME_T_C(6491), PRIME_T_C(6521), PRIME_T_C(6529),
    PRIME_T_C(6547), PRIME_T_C(6551), PRIME_T_C(6553), PRIME_T_C(6563),
    PRIME_T_C(6569), PRIME_T_C(6571), PRIME_T_C(6577), PRIME_T_C(6581),
    PRIME_T_C(6599), PRIME_T_C(6607), PRIME_T_C(6619), PRIME_T_C(6637),
    PRIME_T_C(6653), PRIME_T_C(6659), PRIME_T_C(6661), PRIME_T_C(6673),
    PRIME_T_C(6679), PRIME_T_C(6689), PRIME_T_C(6691), PRIME_T_C(6701),
    PRIME_T_C(6703), PRIME_T_C(6709), PRIME_T_C(6719), PRIME_T_C(6733),
    PRIME_T_C(6737), PRIME_T_C(6761), PRIME_T_C(6763), PRIME_T_C(6779),
    PRIME_T_C(6781), PRIME_T_C(6791), PRIME_T_C(6793), PRIME_T_C(6803),
    PRIME_T_C(6823), PRIME_T_C(6827), PRIME_T_C(6829), PRIME_T_C(6833),
    PRIME_T_C(6841), PRIME_T_C(6857), PRIME_T_C(6863), PRIME_T_C(6869),
    PRIME_T_C(6871), PRIME_T_C(6883), PRIME_T_C(6899), PRIME_T_C(6907),
    PRIME_T_C(6911), PRIME_T_C(6917), PRIME_T_C(6947), PRIME_T_C(6949),
    PRIME_T_C(6959), PRIME_T_C(6961), PRIME_T_C(6967), PRIME_T_C(6971),
    PRIME_T_C(6977), PRIME_T_C(6983), PRIME_T_C(6991), PRIME_T_C(6997),
    PRIME_T_C(7001), PRIME_T_C(7013), PRIME_T_C(7019), PRIME_T_C(7027),
    PRIME_T_C(7039), PRIME_T_C(7043), PRIME_T_C(7057), PRIME_T_C(7069),
    PRIME_T_C(7079), PRIME_T_C(7103), PRIME_T_C(7109), PRIME_T_C(7121),
    PRIME_T_C(7127), PRIME_T_C(7129), PRIME_T_C(7151), PRIME_T_C(7159),
    PRIME_T_C(7177), PRIME_T_C(7187), PRIME_T_C(7193), PRIME_T_C(7207),
    PRIME_T_C(7211), PRIME_T_C(7213), PRIME_T_C(7219), PRIME_T_C(7229),
    PRIME_T_C(7237), PRIME_T_C(7243), PRIME_T_C(7247), PRIME_T_C(7253),
    PRIME_T_C(7283), PRIME_T_C(7297), PRIME_T_C(7307), PRIME_T_C(7309),
    PRIME_T_C(7321), PRIME_T_C(7331), PRIME_T_C(7333), PRIME_T_C(7349),
    PRIME_T_C(7351), PRIME_T_C(7369), PRIME_T_C(7393), PRIME_T_C(7411),
    PRIME_T_C(7417), PRIME_T_C(7433), PRIME_T_C(7451), PRIME_T_C(7457),
    PRIME_T_C(7459), PRIME_T_C(7477), PRIME_T_C(7481), PRIME_T_C(7487),
    PRIME_T_C(7489), PRIME_T_C(7499), PRIME_T_C(7507), PRIME_T_C(7517),
    PRIME_T_C(7523), PRIME_T_C(7529), PRIME_T_C(7537), PRIME_T_C(7541),
    PRIME_T_C(7547), PRIME_T_C(7549), PRIME_T_C(7559), PRIME_T_C(7561),
    PRIME_T_C(7573), PRIME_T_C(7577), PRIME_T_C(7583), PRIME_T_C(7589),
    PRIME_T_C(7591), PRIME_T_C(7603), PRIME_T_C(7607), PRIME_T_C(7621),
    PRIME_T_C(7639), PRIME_T_C(7643), PRIME_T_C(7649), PRIME_T_C(7669),
    PRIME_T_C(7673), PRIME_T_C(7681), PRIME_T_C(7687), PRIME_T_C(7691),
    PRIME_T_C(7699), PRIME_T_C(7703), PRIME_T_C(7717), PRIME_T_C(7723),
    PRIME_T_C(7727), PRIME_T_C(7741), PRIME_T_C(7753), PRIME_T_C(7757),
    PRIME_T_C(7759), PRIME_T_C(7789), PRIME_T_C(7793), PRIME_T_C(7817),
    PRIME_T_C(7823), PRIME_T_C(7829), PRIME_T_C(7841), PRIME_T_C(7853),
    PRIME_T_C(7867), PRIME_T_C(7873), PRIME_T_C(7877), PRIME_T_C(7879),
    PRIME_T_C(7883), PRIME_T_C(7901), PRIME_T_C(7907), PRIME_T_C(7919)
} ;

/*******************************************************************************

    The "last" 1000 primes in a 32- or 64-bit unsigned long number.

*******************************************************************************/

#if PRIME_T_ATLEAST_BITS < 32

    static  const  prime_t  *lastPrimes_32 = NULL ;

#else

static  const  prime_t  lastPrimes_32[MAX_PRIMES_LIST] = {
    PRIME_T_C(4294945087), PRIME_T_C(4294945097), PRIME_T_C(4294945147),
    PRIME_T_C(4294945159), PRIME_T_C(4294945219), PRIME_T_C(4294945243),
    PRIME_T_C(4294945321), PRIME_T_C(4294945337), PRIME_T_C(4294945349),
    PRIME_T_C(4294945351), PRIME_T_C(4294945367), PRIME_T_C(4294945373),
    PRIME_T_C(4294945381), PRIME_T_C(4294945423), PRIME_T_C(4294945429),
    PRIME_T_C(4294945469), PRIME_T_C(4294945483), PRIME_T_C(4294945547),
    PRIME_T_C(4294945621), PRIME_T_C(4294945631), PRIME_T_C(4294945643),
    PRIME_T_C(4294945667), PRIME_T_C(4294945709), PRIME_T_C(4294945717),
    PRIME_T_C(4294945771), PRIME_T_C(4294945783), PRIME_T_C(4294945799),
    PRIME_T_C(4294945813), PRIME_T_C(4294945861), PRIME_T_C(4294945873),
    PRIME_T_C(4294945901), PRIME_T_C(4294945927), PRIME_T_C(4294945937),
    PRIME_T_C(4294945961), PRIME_T_C(4294945979), PRIME_T_C(4294945981),
    PRIME_T_C(4294945997), PRIME_T_C(4294945999), PRIME_T_C(4294946041),
    PRIME_T_C(4294946059), PRIME_T_C(4294946177), PRIME_T_C(4294946191),
    PRIME_T_C(4294946251), PRIME_T_C(4294946287), PRIME_T_C(4294946291),
    PRIME_T_C(4294946329), PRIME_T_C(4294946363), PRIME_T_C(4294946389),
    PRIME_T_C(4294946399), PRIME_T_C(4294946443), PRIME_T_C(4294946471),
    PRIME_T_C(4294946489), PRIME_T_C(4294946497), PRIME_T_C(4294946501),
    PRIME_T_C(4294946507), PRIME_T_C(4294946543), PRIME_T_C(4294946549),
    PRIME_T_C(4294946563), PRIME_T_C(4294946573), PRIME_T_C(4294946587),
    PRIME_T_C(4294946641), PRIME_T_C(4294946671), PRIME_T_C(4294946681),
    PRIME_T_C(4294946687), PRIME_T_C(4294946693), PRIME_T_C(4294946701),
    PRIME_T_C(4294946707), PRIME_T_C(4294946731), PRIME_T_C(4294946737),
    PRIME_T_C(4294946741), PRIME_T_C(4294946749), PRIME_T_C(4294946807),
    PRIME_T_C(4294946813), PRIME_T_C(4294946819), PRIME_T_C(4294946857),
    PRIME_T_C(4294946881), PRIME_T_C(4294946911), PRIME_T_C(4294946921),
    PRIME_T_C(4294946933), PRIME_T_C(4294946969), PRIME_T_C(4294946993),
    PRIME_T_C(4294947001), PRIME_T_C(4294947047), PRIME_T_C(4294947071),
    PRIME_T_C(4294947073), PRIME_T_C(4294947079), PRIME_T_C(4294947089),
    PRIME_T_C(4294947127), PRIME_T_C(4294947149), PRIME_T_C(4294947187),
    PRIME_T_C(4294947191), PRIME_T_C(4294947193), PRIME_T_C(4294947197),
    PRIME_T_C(4294947233), PRIME_T_C(4294947253), PRIME_T_C(4294947263),
    PRIME_T_C(4294947271), PRIME_T_C(4294947289), PRIME_T_C(4294947313),
    PRIME_T_C(4294947323), PRIME_T_C(4294947329), PRIME_T_C(4294947347),
    PRIME_T_C(4294947451), PRIME_T_C(4294947469), PRIME_T_C(4294947479),
    PRIME_T_C(4294947487), PRIME_T_C(4294947509), PRIME_T_C(4294947521),
    PRIME_T_C(4294947557), PRIME_T_C(4294947571), PRIME_T_C(4294947581),
    PRIME_T_C(4294947611), PRIME_T_C(4294947617), PRIME_T_C(4294947641),
    PRIME_T_C(4294947653), PRIME_T_C(4294947673), PRIME_T_C(4294947707),
    PRIME_T_C(4294947809), PRIME_T_C(4294947817), PRIME_T_C(4294947851),
    PRIME_T_C(4294947857), PRIME_T_C(4294947859), PRIME_T_C(4294947863),
    PRIME_T_C(4294947871), PRIME_T_C(4294947889), PRIME_T_C(4294947913),
    PRIME_T_C(4294947953), PRIME_T_C(4294947959), PRIME_T_C(4294947961),
    PRIME_T_C(4294948013), PRIME_T_C(4294948039), PRIME_T_C(4294948057),
    PRIME_T_C(4294948069), PRIME_T_C(4294948109), PRIME_T_C(4294948181),
    PRIME_T_C(4294948193), PRIME_T_C(4294948199), PRIME_T_C(4294948223),
    PRIME_T_C(4294948241), PRIME_T_C(4294948243), PRIME_T_C(4294948261),
    PRIME_T_C(4294948303), PRIME_T_C(4294948319), PRIME_T_C(4294948327),
    PRIME_T_C(4294948331), PRIME_T_C(4294948373), PRIME_T_C(4294948379),
    PRIME_T_C(4294948381), PRIME_T_C(4294948387), PRIME_T_C(4294948429),
    PRIME_T_C(4294948513), PRIME_T_C(4294948531), PRIME_T_C(4294948547),
    PRIME_T_C(4294948549), PRIME_T_C(4294948597), PRIME_T_C(4294948667),
    PRIME_T_C(4294948699), PRIME_T_C(4294948703), PRIME_T_C(4294948717),
    PRIME_T_C(4294948729), PRIME_T_C(4294948829), PRIME_T_C(4294948859),
    PRIME_T_C(4294948871), PRIME_T_C(4294948883), PRIME_T_C(4294948927),
    PRIME_T_C(4294948943), PRIME_T_C(4294948961), PRIME_T_C(4294949027),
    PRIME_T_C(4294949047), PRIME_T_C(4294949051), PRIME_T_C(4294949053),
    PRIME_T_C(4294949059), PRIME_T_C(4294949063), PRIME_T_C(4294949081),
    PRIME_T_C(4294949089), PRIME_T_C(4294949093), PRIME_T_C(4294949123),
    PRIME_T_C(4294949147), PRIME_T_C(4294949161), PRIME_T_C(4294949171),
    PRIME_T_C(4294949203), PRIME_T_C(4294949227), PRIME_T_C(4294949237),
    PRIME_T_C(4294949249), PRIME_T_C(4294949251), PRIME_T_C(4294949279),
    PRIME_T_C(4294949299), PRIME_T_C(4294949317), PRIME_T_C(4294949333),
    PRIME_T_C(4294949357), PRIME_T_C(4294949359), PRIME_T_C(4294949437),
    PRIME_T_C(4294949453), PRIME_T_C(4294949509), PRIME_T_C(4294949531),
    PRIME_T_C(4294949567), PRIME_T_C(4294949569), PRIME_T_C(4294949573),
    PRIME_T_C(4294949599), PRIME_T_C(4294949611), PRIME_T_C(4294949621),
    PRIME_T_C(4294949657), PRIME_T_C(4294949707), PRIME_T_C(4294949767),
    PRIME_T_C(4294949809), PRIME_T_C(4294949833), PRIME_T_C(4294949837),
    PRIME_T_C(4294949861), PRIME_T_C(4294949947), PRIME_T_C(4294949983),
    PRIME_T_C(4294950001), PRIME_T_C(4294950041), PRIME_T_C(4294950043),
    PRIME_T_C(4294950053), PRIME_T_C(4294950067), PRIME_T_C(4294950089),
    PRIME_T_C(4294950113), PRIME_T_C(4294950131), PRIME_T_C(4294950139),
    PRIME_T_C(4294950161), PRIME_T_C(4294950197), PRIME_T_C(4294950251),
    PRIME_T_C(4294950271), PRIME_T_C(4294950277), PRIME_T_C(4294950293),
    PRIME_T_C(4294950299), PRIME_T_C(4294950301), PRIME_T_C(4294950311),
    PRIME_T_C(4294950337), PRIME_T_C(4294950353), PRIME_T_C(4294950377),
    PRIME_T_C(4294950391), PRIME_T_C(4294950427), PRIME_T_C(4294950433),
    PRIME_T_C(4294950461), PRIME_T_C(4294950481), PRIME_T_C(4294950493),
    PRIME_T_C(4294950503), PRIME_T_C(4294950509), PRIME_T_C(4294950529),
    PRIME_T_C(4294950557), PRIME_T_C(4294950587), PRIME_T_C(4294950593),
    PRIME_T_C(4294950599), PRIME_T_C(4294950617), PRIME_T_C(4294950641),
    PRIME_T_C(4294950661), PRIME_T_C(4294950739), PRIME_T_C(4294950749),
    PRIME_T_C(4294950767), PRIME_T_C(4294950791), PRIME_T_C(4294950797),
    PRIME_T_C(4294950809), PRIME_T_C(4294950823), PRIME_T_C(4294950827),
    PRIME_T_C(4294950883), PRIME_T_C(4294950901), PRIME_T_C(4294950917),
    PRIME_T_C(4294950929), PRIME_T_C(4294950943), PRIME_T_C(4294950973),
    PRIME_T_C(4294951021), PRIME_T_C(4294951033), PRIME_T_C(4294951057),
    PRIME_T_C(4294951091), PRIME_T_C(4294951093), PRIME_T_C(4294951097),
    PRIME_T_C(4294951123), PRIME_T_C(4294951147), PRIME_T_C(4294951153),
    PRIME_T_C(4294951163), PRIME_T_C(4294951181), PRIME_T_C(4294951237),
    PRIME_T_C(4294951261), PRIME_T_C(4294951279), PRIME_T_C(4294951303),
    PRIME_T_C(4294951333), PRIME_T_C(4294951337), PRIME_T_C(4294951357),
    PRIME_T_C(4294951369), PRIME_T_C(4294951393), PRIME_T_C(4294951403),
    PRIME_T_C(4294951421), PRIME_T_C(4294951447), PRIME_T_C(4294951471),
    PRIME_T_C(4294951477), PRIME_T_C(4294951489), PRIME_T_C(4294951523),
    PRIME_T_C(4294951541), PRIME_T_C(4294951543), PRIME_T_C(4294951567),
    PRIME_T_C(4294951571), PRIME_T_C(4294951579), PRIME_T_C(4294951613),
    PRIME_T_C(4294951627), PRIME_T_C(4294951643), PRIME_T_C(4294951657),
    PRIME_T_C(4294951673), PRIME_T_C(4294951709), PRIME_T_C(4294951733),
    PRIME_T_C(4294951747), PRIME_T_C(4294951751), PRIME_T_C(4294951763),
    PRIME_T_C(4294951769), PRIME_T_C(4294951777), PRIME_T_C(4294951789),
    PRIME_T_C(4294951811), PRIME_T_C(4294951813), PRIME_T_C(4294951819),
    PRIME_T_C(4294951849), PRIME_T_C(4294951877), PRIME_T_C(4294951879),
    PRIME_T_C(4294951889), PRIME_T_C(4294951891), PRIME_T_C(4294951931),
    PRIME_T_C(4294951949), PRIME_T_C(4294951967), PRIME_T_C(4294952069),
    PRIME_T_C(4294952099), PRIME_T_C(4294952117), PRIME_T_C(4294952159),
    PRIME_T_C(4294952183), PRIME_T_C(4294952201), PRIME_T_C(4294952203),
    PRIME_T_C(4294952237), PRIME_T_C(4294952239), PRIME_T_C(4294952269),
    PRIME_T_C(4294952297), PRIME_T_C(4294952303), PRIME_T_C(4294952371),
    PRIME_T_C(4294952383), PRIME_T_C(4294952399), PRIME_T_C(4294952407),
    PRIME_T_C(4294952419), PRIME_T_C(4294952423), PRIME_T_C(4294952429),
    PRIME_T_C(4294952533), PRIME_T_C(4294952569), PRIME_T_C(4294952639),
    PRIME_T_C(4294952647), PRIME_T_C(4294952653), PRIME_T_C(4294952677),
    PRIME_T_C(4294952687), PRIME_T_C(4294952699), PRIME_T_C(4294952719),
    PRIME_T_C(4294952749), PRIME_T_C(4294952807), PRIME_T_C(4294952839),
    PRIME_T_C(4294952873), PRIME_T_C(4294952899), PRIME_T_C(4294952917),
    PRIME_T_C(4294952951), PRIME_T_C(4294952969), PRIME_T_C(4294952999),
    PRIME_T_C(4294953023), PRIME_T_C(4294953043), PRIME_T_C(4294953079),
    PRIME_T_C(4294953101), PRIME_T_C(4294953119), PRIME_T_C(4294953127),
    PRIME_T_C(4294953149), PRIME_T_C(4294953167), PRIME_T_C(4294953181),
    PRIME_T_C(4294953197), PRIME_T_C(4294953199), PRIME_T_C(4294953217),
    PRIME_T_C(4294953239), PRIME_T_C(4294953241), PRIME_T_C(4294953251),
    PRIME_T_C(4294953307), PRIME_T_C(4294953343), PRIME_T_C(4294953347),
    PRIME_T_C(4294953349), PRIME_T_C(4294953353), PRIME_T_C(4294953473),
    PRIME_T_C(4294953517), PRIME_T_C(4294953521), PRIME_T_C(4294953547),
    PRIME_T_C(4294953569), PRIME_T_C(4294953583), PRIME_T_C(4294953587),
    PRIME_T_C(4294953601), PRIME_T_C(4294953617), PRIME_T_C(4294953623),
    PRIME_T_C(4294953643), PRIME_T_C(4294953653), PRIME_T_C(4294953709),
    PRIME_T_C(4294953731), PRIME_T_C(4294953749), PRIME_T_C(4294953769),
    PRIME_T_C(4294953791), PRIME_T_C(4294953823), PRIME_T_C(4294953881),
    PRIME_T_C(4294953889), PRIME_T_C(4294953919), PRIME_T_C(4294953947),
    PRIME_T_C(4294953959), PRIME_T_C(4294953961), PRIME_T_C(4294953977),
    PRIME_T_C(4294953991), PRIME_T_C(4294954003), PRIME_T_C(4294954021),
    PRIME_T_C(4294954031), PRIME_T_C(4294954091), PRIME_T_C(4294954099),
    PRIME_T_C(4294954109), PRIME_T_C(4294954129), PRIME_T_C(4294954229),
    PRIME_T_C(4294954277), PRIME_T_C(4294954283), PRIME_T_C(4294954297),
    PRIME_T_C(4294954309), PRIME_T_C(4294954327), PRIME_T_C(4294954331),
    PRIME_T_C(4294954333), PRIME_T_C(4294954357), PRIME_T_C(4294954361),
    PRIME_T_C(4294954373), PRIME_T_C(4294954393), PRIME_T_C(4294954397),
    PRIME_T_C(4294954421), PRIME_T_C(4294954441), PRIME_T_C(4294954459),
    PRIME_T_C(4294954471), PRIME_T_C(4294954483), PRIME_T_C(4294954493),
    PRIME_T_C(4294954513), PRIME_T_C(4294954529), PRIME_T_C(4294954537),
    PRIME_T_C(4294954567), PRIME_T_C(4294954663), PRIME_T_C(4294954691),
    PRIME_T_C(4294954721), PRIME_T_C(4294954739), PRIME_T_C(4294954747),
    PRIME_T_C(4294954753), PRIME_T_C(4294954757), PRIME_T_C(4294954777),
    PRIME_T_C(4294954789), PRIME_T_C(4294954793), PRIME_T_C(4294954813),
    PRIME_T_C(4294954823), PRIME_T_C(4294954831), PRIME_T_C(4294954879),
    PRIME_T_C(4294954943), PRIME_T_C(4294954967), PRIME_T_C(4294954981),
    PRIME_T_C(4294954991), PRIME_T_C(4294955003), PRIME_T_C(4294955009),
    PRIME_T_C(4294955053), PRIME_T_C(4294955057), PRIME_T_C(4294955059),
    PRIME_T_C(4294955107), PRIME_T_C(4294955113), PRIME_T_C(4294955131),
    PRIME_T_C(4294955141), PRIME_T_C(4294955167), PRIME_T_C(4294955201),
    PRIME_T_C(4294955213), PRIME_T_C(4294955237), PRIME_T_C(4294955279),
    PRIME_T_C(4294955281), PRIME_T_C(4294955309), PRIME_T_C(4294955381),
    PRIME_T_C(4294955383), PRIME_T_C(4294955417), PRIME_T_C(4294955461),
    PRIME_T_C(4294955479), PRIME_T_C(4294955507), PRIME_T_C(4294955513),
    PRIME_T_C(4294955519), PRIME_T_C(4294955557), PRIME_T_C(4294955603),
    PRIME_T_C(4294955611), PRIME_T_C(4294955653), PRIME_T_C(4294955659),
    PRIME_T_C(4294955663), PRIME_T_C(4294955683), PRIME_T_C(4294955773),
    PRIME_T_C(4294955783), PRIME_T_C(4294955789), PRIME_T_C(4294955831),
    PRIME_T_C(4294955839), PRIME_T_C(4294955849), PRIME_T_C(4294955869),
    PRIME_T_C(4294955879), PRIME_T_C(4294955887), PRIME_T_C(4294955897),
    PRIME_T_C(4294955911), PRIME_T_C(4294955923), PRIME_T_C(4294955981),
    PRIME_T_C(4294955989), PRIME_T_C(4294955993), PRIME_T_C(4294956047),
    PRIME_T_C(4294956049), PRIME_T_C(4294956073), PRIME_T_C(4294956079),
    PRIME_T_C(4294956151), PRIME_T_C(4294956157), PRIME_T_C(4294956167),
    PRIME_T_C(4294956179), PRIME_T_C(4294956217), PRIME_T_C(4294956223),
    PRIME_T_C(4294956293), PRIME_T_C(4294956299), PRIME_T_C(4294956311),
    PRIME_T_C(4294956343), PRIME_T_C(4294956443), PRIME_T_C(4294956461),
    PRIME_T_C(4294956467), PRIME_T_C(4294956493), PRIME_T_C(4294956509),
    PRIME_T_C(4294956529), PRIME_T_C(4294956541), PRIME_T_C(4294956569),
    PRIME_T_C(4294956613), PRIME_T_C(4294956629), PRIME_T_C(4294956641),
    PRIME_T_C(4294956643), PRIME_T_C(4294956649), PRIME_T_C(4294956667),
    PRIME_T_C(4294956671), PRIME_T_C(4294956683), PRIME_T_C(4294956689),
    PRIME_T_C(4294956697), PRIME_T_C(4294956719), PRIME_T_C(4294956751),
    PRIME_T_C(4294956793), PRIME_T_C(4294956817), PRIME_T_C(4294956829),
    PRIME_T_C(4294956833), PRIME_T_C(4294956839), PRIME_T_C(4294956847),
    PRIME_T_C(4294956863), PRIME_T_C(4294956881), PRIME_T_C(4294956889),
    PRIME_T_C(4294956901), PRIME_T_C(4294956923), PRIME_T_C(4294956937),
    PRIME_T_C(4294956949), PRIME_T_C(4294956989), PRIME_T_C(4294957057),
    PRIME_T_C(4294957067), PRIME_T_C(4294957081), PRIME_T_C(4294957099),
    PRIME_T_C(4294957103), PRIME_T_C(4294957109), PRIME_T_C(4294957123),
    PRIME_T_C(4294957157), PRIME_T_C(4294957189), PRIME_T_C(4294957207),
    PRIME_T_C(4294957267), PRIME_T_C(4294957307), PRIME_T_C(4294957309),
    PRIME_T_C(4294957319), PRIME_T_C(4294957349), PRIME_T_C(4294957369),
    PRIME_T_C(4294957397), PRIME_T_C(4294957399), PRIME_T_C(4294957439),
    PRIME_T_C(4294957493), PRIME_T_C(4294957523), PRIME_T_C(4294957553),
    PRIME_T_C(4294957609), PRIME_T_C(4294957643), PRIME_T_C(4294957679),
    PRIME_T_C(4294957697), PRIME_T_C(4294957699), PRIME_T_C(4294957703),
    PRIME_T_C(4294957717), PRIME_T_C(4294957729), PRIME_T_C(4294957753),
    PRIME_T_C(4294957781), PRIME_T_C(4294957841), PRIME_T_C(4294957853),
    PRIME_T_C(4294957859), PRIME_T_C(4294957861), PRIME_T_C(4294957903),
    PRIME_T_C(4294957939), PRIME_T_C(4294957943), PRIME_T_C(4294957957),
    PRIME_T_C(4294957973), PRIME_T_C(4294957991), PRIME_T_C(4294958011),
    PRIME_T_C(4294958027), PRIME_T_C(4294958051), PRIME_T_C(4294958069),
    PRIME_T_C(4294958071), PRIME_T_C(4294958077), PRIME_T_C(4294958099),
    PRIME_T_C(4294958131), PRIME_T_C(4294958197), PRIME_T_C(4294958237),
    PRIME_T_C(4294958249), PRIME_T_C(4294958257), PRIME_T_C(4294958281),
    PRIME_T_C(4294958329), PRIME_T_C(4294958401), PRIME_T_C(4294958443),
    PRIME_T_C(4294958449), PRIME_T_C(4294958461), PRIME_T_C(4294958467),
    PRIME_T_C(4294958527), PRIME_T_C(4294958537), PRIME_T_C(4294958539),
    PRIME_T_C(4294958597), PRIME_T_C(4294958611), PRIME_T_C(4294958617),
    PRIME_T_C(4294958621), PRIME_T_C(4294958627), PRIME_T_C(4294958651),
    PRIME_T_C(4294958711), PRIME_T_C(4294958729), PRIME_T_C(4294958741),
    PRIME_T_C(4294958777), PRIME_T_C(4294958791), PRIME_T_C(4294958861),
    PRIME_T_C(4294958881), PRIME_T_C(4294958887), PRIME_T_C(4294958903),
    PRIME_T_C(4294958923), PRIME_T_C(4294958929), PRIME_T_C(4294958989),
    PRIME_T_C(4294959007), PRIME_T_C(4294959013), PRIME_T_C(4294959049),
    PRIME_T_C(4294959079), PRIME_T_C(4294959083), PRIME_T_C(4294959143),
    PRIME_T_C(4294959173), PRIME_T_C(4294959181), PRIME_T_C(4294959259),
    PRIME_T_C(4294959301), PRIME_T_C(4294959313), PRIME_T_C(4294959323),
    PRIME_T_C(4294959343), PRIME_T_C(4294959359), PRIME_T_C(4294959377),
    PRIME_T_C(4294959433), PRIME_T_C(4294959439), PRIME_T_C(4294959467),
    PRIME_T_C(4294959521), PRIME_T_C(4294959541), PRIME_T_C(4294959637),
    PRIME_T_C(4294959673), PRIME_T_C(4294959677), PRIME_T_C(4294959701),
    PRIME_T_C(4294959703), PRIME_T_C(4294959719), PRIME_T_C(4294959751),
    PRIME_T_C(4294959803), PRIME_T_C(4294959847), PRIME_T_C(4294959859),
    PRIME_T_C(4294959869), PRIME_T_C(4294959931), PRIME_T_C(4294959959),
    PRIME_T_C(4294959967), PRIME_T_C(4294959973), PRIME_T_C(4294959979),
    PRIME_T_C(4294959997), PRIME_T_C(4294960003), PRIME_T_C(4294960049),
    PRIME_T_C(4294960079), PRIME_T_C(4294960091), PRIME_T_C(4294960097),
    PRIME_T_C(4294960123), PRIME_T_C(4294960141), PRIME_T_C(4294960151),
    PRIME_T_C(4294960169), PRIME_T_C(4294960171), PRIME_T_C(4294960199),
    PRIME_T_C(4294960211), PRIME_T_C(4294960217), PRIME_T_C(4294960253),
    PRIME_T_C(4294960261), PRIME_T_C(4294960301), PRIME_T_C(4294960303),
    PRIME_T_C(4294960321), PRIME_T_C(4294960331), PRIME_T_C(4294960337),
    PRIME_T_C(4294960339), PRIME_T_C(4294960399), PRIME_T_C(4294960409),
    PRIME_T_C(4294960447), PRIME_T_C(4294960459), PRIME_T_C(4294960489),
    PRIME_T_C(4294960511), PRIME_T_C(4294960541), PRIME_T_C(4294960561),
    PRIME_T_C(4294960601), PRIME_T_C(4294960619), PRIME_T_C(4294960679),
    PRIME_T_C(4294960727), PRIME_T_C(4294960759), PRIME_T_C(4294960783),
    PRIME_T_C(4294960829), PRIME_T_C(4294960837), PRIME_T_C(4294960877),
    PRIME_T_C(4294960883), PRIME_T_C(4294960889), PRIME_T_C(4294960907),
    PRIME_T_C(4294960919), PRIME_T_C(4294960931), PRIME_T_C(4294960949),
    PRIME_T_C(4294960951), PRIME_T_C(4294960973), PRIME_T_C(4294960979),
    PRIME_T_C(4294960981), PRIME_T_C(4294961009), PRIME_T_C(4294961023),
    PRIME_T_C(4294961071), PRIME_T_C(4294961087), PRIME_T_C(4294961119),
    PRIME_T_C(4294961129), PRIME_T_C(4294961137), PRIME_T_C(4294961179),
    PRIME_T_C(4294961183), PRIME_T_C(4294961197), PRIME_T_C(4294961287),
    PRIME_T_C(4294961317), PRIME_T_C(4294961353), PRIME_T_C(4294961399),
    PRIME_T_C(4294961401), PRIME_T_C(4294961449), PRIME_T_C(4294961453),
    PRIME_T_C(4294961467), PRIME_T_C(4294961483), PRIME_T_C(4294961507),
    PRIME_T_C(4294961521), PRIME_T_C(4294961527), PRIME_T_C(4294961579),
    PRIME_T_C(4294961597), PRIME_T_C(4294961623), PRIME_T_C(4294961663),
    PRIME_T_C(4294961707), PRIME_T_C(4294961747), PRIME_T_C(4294961767),
    PRIME_T_C(4294961777), PRIME_T_C(4294961791), PRIME_T_C(4294961809),
    PRIME_T_C(4294961843), PRIME_T_C(4294961861), PRIME_T_C(4294961863),
    PRIME_T_C(4294961873), PRIME_T_C(4294961893), PRIME_T_C(4294961897),
    PRIME_T_C(4294961921), PRIME_T_C(4294961927), PRIME_T_C(4294961941),
    PRIME_T_C(4294961959), PRIME_T_C(4294961963), PRIME_T_C(4294962019),
    PRIME_T_C(4294962047), PRIME_T_C(4294962079), PRIME_T_C(4294962137),
    PRIME_T_C(4294962151), PRIME_T_C(4294962211), PRIME_T_C(4294962223),
    PRIME_T_C(4294962233), PRIME_T_C(4294962271), PRIME_T_C(4294962277),
    PRIME_T_C(4294962299), PRIME_T_C(4294962313), PRIME_T_C(4294962341),
    PRIME_T_C(4294962349), PRIME_T_C(4294962367), PRIME_T_C(4294962377),
    PRIME_T_C(4294962389), PRIME_T_C(4294962391), PRIME_T_C(4294962401),
    PRIME_T_C(4294962409), PRIME_T_C(4294962449), PRIME_T_C(4294962473),
    PRIME_T_C(4294962499), PRIME_T_C(4294962533), PRIME_T_C(4294962541),
    PRIME_T_C(4294962589), PRIME_T_C(4294962619), PRIME_T_C(4294962629),
    PRIME_T_C(4294962641), PRIME_T_C(4294962653), PRIME_T_C(4294962689),
    PRIME_T_C(4294962691), PRIME_T_C(4294962703), PRIME_T_C(4294962719),
    PRIME_T_C(4294962731), PRIME_T_C(4294962751), PRIME_T_C(4294962757),
    PRIME_T_C(4294962779), PRIME_T_C(4294962809), PRIME_T_C(4294962817),
    PRIME_T_C(4294962827), PRIME_T_C(4294962853), PRIME_T_C(4294962887),
    PRIME_T_C(4294962899), PRIME_T_C(4294962911), PRIME_T_C(4294962929),
    PRIME_T_C(4294962953), PRIME_T_C(4294963039), PRIME_T_C(4294963051),
    PRIME_T_C(4294963093), PRIME_T_C(4294963097), PRIME_T_C(4294963111),
    PRIME_T_C(4294963117), PRIME_T_C(4294963171), PRIME_T_C(4294963237),
    PRIME_T_C(4294963291), PRIME_T_C(4294963313), PRIME_T_C(4294963333),
    PRIME_T_C(4294963349), PRIME_T_C(4294963369), PRIME_T_C(4294963427),
    PRIME_T_C(4294963429), PRIME_T_C(4294963459), PRIME_T_C(4294963499),
    PRIME_T_C(4294963523), PRIME_T_C(4294963537), PRIME_T_C(4294963553),
    PRIME_T_C(4294963571), PRIME_T_C(4294963583), PRIME_T_C(4294963619),
    PRIME_T_C(4294963637), PRIME_T_C(4294963639), PRIME_T_C(4294963643),
    PRIME_T_C(4294963667), PRIME_T_C(4294963681), PRIME_T_C(4294963723),
    PRIME_T_C(4294963747), PRIME_T_C(4294963781), PRIME_T_C(4294963787),
    PRIME_T_C(4294963847), PRIME_T_C(4294963853), PRIME_T_C(4294963891),
    PRIME_T_C(4294963901), PRIME_T_C(4294963921), PRIME_T_C(4294963943),
    PRIME_T_C(4294963957), PRIME_T_C(4294963987), PRIME_T_C(4294963993),
    PRIME_T_C(4294964017), PRIME_T_C(4294964027), PRIME_T_C(4294964029),
    PRIME_T_C(4294964039), PRIME_T_C(4294964081), PRIME_T_C(4294964123),
    PRIME_T_C(4294964131), PRIME_T_C(4294964159), PRIME_T_C(4294964173),
    PRIME_T_C(4294964203), PRIME_T_C(4294964207), PRIME_T_C(4294964209),
    PRIME_T_C(4294964213), PRIME_T_C(4294964221), PRIME_T_C(4294964239),
    PRIME_T_C(4294964249), PRIME_T_C(4294964257), PRIME_T_C(4294964263),
    PRIME_T_C(4294964281), PRIME_T_C(4294964287), PRIME_T_C(4294964309),
    PRIME_T_C(4294964327), PRIME_T_C(4294964341), PRIME_T_C(4294964381),
    PRIME_T_C(4294964419), PRIME_T_C(4294964437), PRIME_T_C(4294964441),
    PRIME_T_C(4294964461), PRIME_T_C(4294964489), PRIME_T_C(4294964491),
    PRIME_T_C(4294964521), PRIME_T_C(4294964537), PRIME_T_C(4294964543),
    PRIME_T_C(4294964561), PRIME_T_C(4294964579), PRIME_T_C(4294964599),
    PRIME_T_C(4294964621), PRIME_T_C(4294964633), PRIME_T_C(4294964683),
    PRIME_T_C(4294964689), PRIME_T_C(4294964749), PRIME_T_C(4294964771),
    PRIME_T_C(4294964789), PRIME_T_C(4294964809), PRIME_T_C(4294964827),
    PRIME_T_C(4294964833), PRIME_T_C(4294964879), PRIME_T_C(4294964887),
    PRIME_T_C(4294964893), PRIME_T_C(4294964897), PRIME_T_C(4294964899),
    PRIME_T_C(4294964903), PRIME_T_C(4294964923), PRIME_T_C(4294964929),
    PRIME_T_C(4294964939), PRIME_T_C(4294964959), PRIME_T_C(4294964969),
    PRIME_T_C(4294964977), PRIME_T_C(4294964981), PRIME_T_C(4294965019),
    PRIME_T_C(4294965131), PRIME_T_C(4294965137), PRIME_T_C(4294965151),
    PRIME_T_C(4294965161), PRIME_T_C(4294965193), PRIME_T_C(4294965203),
    PRIME_T_C(4294965229), PRIME_T_C(4294965251), PRIME_T_C(4294965263),
    PRIME_T_C(4294965307), PRIME_T_C(4294965313), PRIME_T_C(4294965331),
    PRIME_T_C(4294965347), PRIME_T_C(4294965361), PRIME_T_C(4294965383),
    PRIME_T_C(4294965413), PRIME_T_C(4294965457), PRIME_T_C(4294965461),
    PRIME_T_C(4294965487), PRIME_T_C(4294965529), PRIME_T_C(4294965581),
    PRIME_T_C(4294965601), PRIME_T_C(4294965613), PRIME_T_C(4294965617),
    PRIME_T_C(4294965641), PRIME_T_C(4294965659), PRIME_T_C(4294965671),
    PRIME_T_C(4294965673), PRIME_T_C(4294965679), PRIME_T_C(4294965683),
    PRIME_T_C(4294965691), PRIME_T_C(4294965721), PRIME_T_C(4294965733),
    PRIME_T_C(4294965737), PRIME_T_C(4294965757), PRIME_T_C(4294965767),
    PRIME_T_C(4294965793), PRIME_T_C(4294965821), PRIME_T_C(4294965839),
    PRIME_T_C(4294965841), PRIME_T_C(4294965847), PRIME_T_C(4294965887),
    PRIME_T_C(4294965911), PRIME_T_C(4294965937), PRIME_T_C(4294965949),
    PRIME_T_C(4294965967), PRIME_T_C(4294965971), PRIME_T_C(4294965977),
    PRIME_T_C(4294966001), PRIME_T_C(4294966007), PRIME_T_C(4294966043),
    PRIME_T_C(4294966073), PRIME_T_C(4294966087), PRIME_T_C(4294966099),
    PRIME_T_C(4294966121), PRIME_T_C(4294966129), PRIME_T_C(4294966153),
    PRIME_T_C(4294966163), PRIME_T_C(4294966177), PRIME_T_C(4294966187),
    PRIME_T_C(4294966217), PRIME_T_C(4294966231), PRIME_T_C(4294966237),
    PRIME_T_C(4294966243), PRIME_T_C(4294966297), PRIME_T_C(4294966337),
    PRIME_T_C(4294966367), PRIME_T_C(4294966373), PRIME_T_C(4294966427),
    PRIME_T_C(4294966441), PRIME_T_C(4294966447), PRIME_T_C(4294966477),
    PRIME_T_C(4294966553), PRIME_T_C(4294966583), PRIME_T_C(4294966591),
    PRIME_T_C(4294966619), PRIME_T_C(4294966639), PRIME_T_C(4294966651),
    PRIME_T_C(4294966657), PRIME_T_C(4294966661), PRIME_T_C(4294966667),
    PRIME_T_C(4294966769), PRIME_T_C(4294966813), PRIME_T_C(4294966829),
    PRIME_T_C(4294966877), PRIME_T_C(4294966909), PRIME_T_C(4294966927),
    PRIME_T_C(4294966943), PRIME_T_C(4294966981), PRIME_T_C(4294966997),
    PRIME_T_C(4294967029), PRIME_T_C(4294967087), PRIME_T_C(4294967111),
    PRIME_T_C(4294967143), PRIME_T_C(4294967161), PRIME_T_C(4294967189),
    PRIME_T_C(4294967197), PRIME_T_C(4294967231), PRIME_T_C(4294967279),
    PRIME_T_C(4294967291)
} ;

#endif	/* Unsigned longs are at least 32 bits wide. */

#if PRIME_T_ATLEAST_BITS < 64

    static  const  prime_t  *lastPrimes_64 = NULL ;

#else

static  const  prime_t  lastPrimes_64[MAX_PRIMES_LIST] = {
    PRIME_T_C(18446744073709506419), PRIME_T_C(18446744073709506431),
    PRIME_T_C(18446744073709506481), PRIME_T_C(18446744073709506517),
    PRIME_T_C(18446744073709506523), PRIME_T_C(18446744073709506673),
    PRIME_T_C(18446744073709506679), PRIME_T_C(18446744073709506707),
    PRIME_T_C(18446744073709506713), PRIME_T_C(18446744073709506739),
    PRIME_T_C(18446744073709506791), PRIME_T_C(18446744073709506809),
    PRIME_T_C(18446744073709506817), PRIME_T_C(18446744073709506823),
    PRIME_T_C(18446744073709506881), PRIME_T_C(18446744073709506899),
    PRIME_T_C(18446744073709506931), PRIME_T_C(18446744073709506953),
    PRIME_T_C(18446744073709506983), PRIME_T_C(18446744073709507069),
    PRIME_T_C(18446744073709507091), PRIME_T_C(18446744073709507099),
    PRIME_T_C(18446744073709507103), PRIME_T_C(18446744073709507307),
    PRIME_T_C(18446744073709507327), PRIME_T_C(18446744073709507331),
    PRIME_T_C(18446744073709507337), PRIME_T_C(18446744073709507421),
    PRIME_T_C(18446744073709507471), PRIME_T_C(18446744073709507571),
    PRIME_T_C(18446744073709507613), PRIME_T_C(18446744073709507643),
    PRIME_T_C(18446744073709507649), PRIME_T_C(18446744073709507669),
    PRIME_T_C(18446744073709507691), PRIME_T_C(18446744073709507763),
    PRIME_T_C(18446744073709507769), PRIME_T_C(18446744073709507793),
    PRIME_T_C(18446744073709507819), PRIME_T_C(18446744073709507837),
    PRIME_T_C(18446744073709507849), PRIME_T_C(18446744073709507943),
    PRIME_T_C(18446744073709507999), PRIME_T_C(18446744073709508009),
    PRIME_T_C(18446744073709508173), PRIME_T_C(18446744073709508251),
    PRIME_T_C(18446744073709508267), PRIME_T_C(18446744073709508281),
    PRIME_T_C(18446744073709508341), PRIME_T_C(18446744073709508357),
    PRIME_T_C(18446744073709508387), PRIME_T_C(18446744073709508567),
    PRIME_T_C(18446744073709508581), PRIME_T_C(18446744073709508587),
    PRIME_T_C(18446744073709508663), PRIME_T_C(18446744073709508723),
    PRIME_T_C(18446744073709508771), PRIME_T_C(18446744073709508861),
    PRIME_T_C(18446744073709508923), PRIME_T_C(18446744073709508927),
    PRIME_T_C(18446744073709508929), PRIME_T_C(18446744073709508959),
    PRIME_T_C(18446744073709508971), PRIME_T_C(18446744073709508993),
    PRIME_T_C(18446744073709509029), PRIME_T_C(18446744073709509119),
    PRIME_T_C(18446744073709509277), PRIME_T_C(18446744073709509407),
    PRIME_T_C(18446744073709509433), PRIME_T_C(18446744073709509463),
    PRIME_T_C(18446744073709509511), PRIME_T_C(18446744073709509583),
    PRIME_T_C(18446744073709509619), PRIME_T_C(18446744073709509673),
    PRIME_T_C(18446744073709509737), PRIME_T_C(18446744073709509827),
    PRIME_T_C(18446744073709509869), PRIME_T_C(18446744073709509871),
    PRIME_T_C(18446744073709509923), PRIME_T_C(18446744073709509941),
    PRIME_T_C(18446744073709510051), PRIME_T_C(18446744073709510081),
    PRIME_T_C(18446744073709510087), PRIME_T_C(18446744073709510147),
    PRIME_T_C(18446744073709510177), PRIME_T_C(18446744073709510189),
    PRIME_T_C(18446744073709510243), PRIME_T_C(18446744073709510259),
    PRIME_T_C(18446744073709510301), PRIME_T_C(18446744073709510333),
    PRIME_T_C(18446744073709510397), PRIME_T_C(18446744073709510409),
    PRIME_T_C(18446744073709510453), PRIME_T_C(18446744073709510541),
    PRIME_T_C(18446744073709510549), PRIME_T_C(18446744073709510697),
    PRIME_T_C(18446744073709510703), PRIME_T_C(18446744073709510709),
    PRIME_T_C(18446744073709510721), PRIME_T_C(18446744073709510759),
    PRIME_T_C(18446744073709510771), PRIME_T_C(18446744073709510841),
    PRIME_T_C(18446744073709510943), PRIME_T_C(18446744073709510957),
    PRIME_T_C(18446744073709510967), PRIME_T_C(18446744073709511011),
    PRIME_T_C(18446744073709511017), PRIME_T_C(18446744073709511057),
    PRIME_T_C(18446744073709511059), PRIME_T_C(18446744073709511081),
    PRIME_T_C(18446744073709511113), PRIME_T_C(18446744073709511191),
    PRIME_T_C(18446744073709511221), PRIME_T_C(18446744073709511227),
    PRIME_T_C(18446744073709511251), PRIME_T_C(18446744073709511377),
    PRIME_T_C(18446744073709511381), PRIME_T_C(18446744073709511431),
    PRIME_T_C(18446744073709511479), PRIME_T_C(18446744073709511501),
    PRIME_T_C(18446744073709511519), PRIME_T_C(18446744073709511603),
    PRIME_T_C(18446744073709511617), PRIME_T_C(18446744073709511621),
    PRIME_T_C(18446744073709511909), PRIME_T_C(18446744073709511939),
    PRIME_T_C(18446744073709512023), PRIME_T_C(18446744073709512047),
    PRIME_T_C(18446744073709512089), PRIME_T_C(18446744073709512097),
    PRIME_T_C(18446744073709512103), PRIME_T_C(18446744073709512167),
    PRIME_T_C(18446744073709512247), PRIME_T_C(18446744073709512263),
    PRIME_T_C(18446744073709512271), PRIME_T_C(18446744073709512329),
    PRIME_T_C(18446744073709512401), PRIME_T_C(18446744073709512403),
    PRIME_T_C(18446744073709512521), PRIME_T_C(18446744073709512647),
    PRIME_T_C(18446744073709512713), PRIME_T_C(18446744073709512749),
    PRIME_T_C(18446744073709512751), PRIME_T_C(18446744073709512767),
    PRIME_T_C(18446744073709512829), PRIME_T_C(18446744073709512989),
    PRIME_T_C(18446744073709513021), PRIME_T_C(18446744073709513027),
    PRIME_T_C(18446744073709513153), PRIME_T_C(18446744073709513241),
    PRIME_T_C(18446744073709513267), PRIME_T_C(18446744073709513273),
    PRIME_T_C(18446744073709513277), PRIME_T_C(18446744073709513319),
    PRIME_T_C(18446744073709513339), PRIME_T_C(18446744073709513421),
    PRIME_T_C(18446744073709513493), PRIME_T_C(18446744073709513693),
    PRIME_T_C(18446744073709513703), PRIME_T_C(18446744073709513711),
    PRIME_T_C(18446744073709513717), PRIME_T_C(18446744073709513721),
    PRIME_T_C(18446744073709513799), PRIME_T_C(18446744073709513853),
    PRIME_T_C(18446744073709513871), PRIME_T_C(18446744073709513931),
    PRIME_T_C(18446744073709514009), PRIME_T_C(18446744073709514033),
    PRIME_T_C(18446744073709514071), PRIME_T_C(18446744073709514089),
    PRIME_T_C(18446744073709514117), PRIME_T_C(18446744073709514137),
    PRIME_T_C(18446744073709514221), PRIME_T_C(18446744073709514231),
    PRIME_T_C(18446744073709514243), PRIME_T_C(18446744073709514267),
    PRIME_T_C(18446744073709514333), PRIME_T_C(18446744073709514377),
    PRIME_T_C(18446744073709514383), PRIME_T_C(18446744073709514401),
    PRIME_T_C(18446744073709514447), PRIME_T_C(18446744073709514449),
    PRIME_T_C(18446744073709514491), PRIME_T_C(18446744073709514503),
    PRIME_T_C(18446744073709514513), PRIME_T_C(18446744073709514551),
    PRIME_T_C(18446744073709514561), PRIME_T_C(18446744073709514621),
    PRIME_T_C(18446744073709514683), PRIME_T_C(18446744073709514747),
    PRIME_T_C(18446744073709514831), PRIME_T_C(18446744073709514839),
    PRIME_T_C(18446744073709514851), PRIME_T_C(18446744073709514923),
    PRIME_T_C(18446744073709514933), PRIME_T_C(18446744073709514947),
    PRIME_T_C(18446744073709514951), PRIME_T_C(18446744073709515029),
    PRIME_T_C(18446744073709515071), PRIME_T_C(18446744073709515077),
    PRIME_T_C(18446744073709515181), PRIME_T_C(18446744073709515211),
    PRIME_T_C(18446744073709515229), PRIME_T_C(18446744073709515257),
    PRIME_T_C(18446744073709515287), PRIME_T_C(18446744073709515301),
    PRIME_T_C(18446744073709515329), PRIME_T_C(18446744073709515409),
    PRIME_T_C(18446744073709515433), PRIME_T_C(18446744073709515449),
    PRIME_T_C(18446744073709515457), PRIME_T_C(18446744073709515469),
    PRIME_T_C(18446744073709515497), PRIME_T_C(18446744073709515527),
    PRIME_T_C(18446744073709515533), PRIME_T_C(18446744073709515559),
    PRIME_T_C(18446744073709515631), PRIME_T_C(18446744073709515653),
    PRIME_T_C(18446744073709515757), PRIME_T_C(18446744073709515769),
    PRIME_T_C(18446744073709515913), PRIME_T_C(18446744073709515959),
    PRIME_T_C(18446744073709516013), PRIME_T_C(18446744073709516021),
    PRIME_T_C(18446744073709516133), PRIME_T_C(18446744073709516273),
    PRIME_T_C(18446744073709516363), PRIME_T_C(18446744073709516387),
    PRIME_T_C(18446744073709516399), PRIME_T_C(18446744073709516423),
    PRIME_T_C(18446744073709516429), PRIME_T_C(18446744073709516517),
    PRIME_T_C(18446744073709516547), PRIME_T_C(18446744073709516549),
    PRIME_T_C(18446744073709516583), PRIME_T_C(18446744073709516597),
    PRIME_T_C(18446744073709516841), PRIME_T_C(18446744073709516889),
    PRIME_T_C(18446744073709516921), PRIME_T_C(18446744073709517011),
    PRIME_T_C(18446744073709517117), PRIME_T_C(18446744073709517141),
    PRIME_T_C(18446744073709517143), PRIME_T_C(18446744073709517147),
    PRIME_T_C(18446744073709517191), PRIME_T_C(18446744073709517213),
    PRIME_T_C(18446744073709517239), PRIME_T_C(18446744073709517287),
    PRIME_T_C(18446744073709517341), PRIME_T_C(18446744073709517387),
    PRIME_T_C(18446744073709517471), PRIME_T_C(18446744073709517521),
    PRIME_T_C(18446744073709517591), PRIME_T_C(18446744073709517641),
    PRIME_T_C(18446744073709517677), PRIME_T_C(18446744073709517771),
    PRIME_T_C(18446744073709517801), PRIME_T_C(18446744073709517809),
    PRIME_T_C(18446744073709517837), PRIME_T_C(18446744073709517947),
    PRIME_T_C(18446744073709517963), PRIME_T_C(18446744073709517977),
    PRIME_T_C(18446744073709518001), PRIME_T_C(18446744073709518059),
    PRIME_T_C(18446744073709518139), PRIME_T_C(18446744073709518241),
    PRIME_T_C(18446744073709518373), PRIME_T_C(18446744073709518401),
    PRIME_T_C(18446744073709518433), PRIME_T_C(18446744073709518443),
    PRIME_T_C(18446744073709518493), PRIME_T_C(18446744073709518527),
    PRIME_T_C(18446744073709518643), PRIME_T_C(18446744073709518683),
    PRIME_T_C(18446744073709518713), PRIME_T_C(18446744073709518727),
    PRIME_T_C(18446744073709518757), PRIME_T_C(18446744073709518773),
    PRIME_T_C(18446744073709518823), PRIME_T_C(18446744073709518829),
    PRIME_T_C(18446744073709518887), PRIME_T_C(18446744073709518899),
    PRIME_T_C(18446744073709518913), PRIME_T_C(18446744073709518937),
    PRIME_T_C(18446744073709518943), PRIME_T_C(18446744073709519027),
    PRIME_T_C(18446744073709519133), PRIME_T_C(18446744073709519189),
    PRIME_T_C(18446744073709519313), PRIME_T_C(18446744073709519409),
    PRIME_T_C(18446744073709519427), PRIME_T_C(18446744073709519507),
    PRIME_T_C(18446744073709519597), PRIME_T_C(18446744073709519619),
    PRIME_T_C(18446744073709519637), PRIME_T_C(18446744073709519643),
    PRIME_T_C(18446744073709519699), PRIME_T_C(18446744073709519759),
    PRIME_T_C(18446744073709519783), PRIME_T_C(18446744073709519829),
    PRIME_T_C(18446744073709519873), PRIME_T_C(18446744073709519879),
    PRIME_T_C(18446744073709519891), PRIME_T_C(18446744073709519903),
    PRIME_T_C(18446744073709519993), PRIME_T_C(18446744073709519999),
    PRIME_T_C(18446744073709520137), PRIME_T_C(18446744073709520143),
    PRIME_T_C(18446744073709520147), PRIME_T_C(18446744073709520291),
    PRIME_T_C(18446744073709520333), PRIME_T_C(18446744073709520479),
    PRIME_T_C(18446744073709520509), PRIME_T_C(18446744073709520549),
    PRIME_T_C(18446744073709520587), PRIME_T_C(18446744073709520593),
    PRIME_T_C(18446744073709520599), PRIME_T_C(18446744073709520633),
    PRIME_T_C(18446744073709520651), PRIME_T_C(18446744073709520663),
    PRIME_T_C(18446744073709520711), PRIME_T_C(18446744073709520713),
    PRIME_T_C(18446744073709520717), PRIME_T_C(18446744073709520729),
    PRIME_T_C(18446744073709520831), PRIME_T_C(18446744073709520927),
    PRIME_T_C(18446744073709520929), PRIME_T_C(18446744073709521107),
    PRIME_T_C(18446744073709521133), PRIME_T_C(18446744073709521211),
    PRIME_T_C(18446744073709521247), PRIME_T_C(18446744073709521377),
    PRIME_T_C(18446744073709521379), PRIME_T_C(18446744073709521407),
    PRIME_T_C(18446744073709521419), PRIME_T_C(18446744073709521473),
    PRIME_T_C(18446744073709521487), PRIME_T_C(18446744073709521509),
    PRIME_T_C(18446744073709521539), PRIME_T_C(18446744073709521571),
    PRIME_T_C(18446744073709521587), PRIME_T_C(18446744073709521601),
    PRIME_T_C(18446744073709521643), PRIME_T_C(18446744073709521659),
    PRIME_T_C(18446744073709521781), PRIME_T_C(18446744073709521833),
    PRIME_T_C(18446744073709521859), PRIME_T_C(18446744073709521881),
    PRIME_T_C(18446744073709521919), PRIME_T_C(18446744073709521943),
    PRIME_T_C(18446744073709522063), PRIME_T_C(18446744073709522091),
    PRIME_T_C(18446744073709522097), PRIME_T_C(18446744073709522183),
    PRIME_T_C(18446744073709522199), PRIME_T_C(18446744073709522217),
    PRIME_T_C(18446744073709522223), PRIME_T_C(18446744073709522271),
    PRIME_T_C(18446744073709522327), PRIME_T_C(18446744073709522349),
    PRIME_T_C(18446744073709522369), PRIME_T_C(18446744073709522393),
    PRIME_T_C(18446744073709522481), PRIME_T_C(18446744073709522523),
    PRIME_T_C(18446744073709522723), PRIME_T_C(18446744073709522829),
    PRIME_T_C(18446744073709522849), PRIME_T_C(18446744073709522867),
    PRIME_T_C(18446744073709522877), PRIME_T_C(18446744073709522897),
    PRIME_T_C(18446744073709522903), PRIME_T_C(18446744073709522931),
    PRIME_T_C(18446744073709522957), PRIME_T_C(18446744073709523059),
    PRIME_T_C(18446744073709523149), PRIME_T_C(18446744073709523153),
    PRIME_T_C(18446744073709523171), PRIME_T_C(18446744073709523237),
    PRIME_T_C(18446744073709523303), PRIME_T_C(18446744073709523323),
    PRIME_T_C(18446744073709523381), PRIME_T_C(18446744073709523387),
    PRIME_T_C(18446744073709523393), PRIME_T_C(18446744073709523437),
    PRIME_T_C(18446744073709523557), PRIME_T_C(18446744073709523623),
    PRIME_T_C(18446744073709523771), PRIME_T_C(18446744073709523777),
    PRIME_T_C(18446744073709523791), PRIME_T_C(18446744073709523809),
    PRIME_T_C(18446744073709523851), PRIME_T_C(18446744073709523953),
    PRIME_T_C(18446744073709524037), PRIME_T_C(18446744073709524077),
    PRIME_T_C(18446744073709524149), PRIME_T_C(18446744073709524169),
    PRIME_T_C(18446744073709524203), PRIME_T_C(18446744073709524281),
    PRIME_T_C(18446744073709524323), PRIME_T_C(18446744073709524341),
    PRIME_T_C(18446744073709524371), PRIME_T_C(18446744073709524431),
    PRIME_T_C(18446744073709524451), PRIME_T_C(18446744073709524607),
    PRIME_T_C(18446744073709524673), PRIME_T_C(18446744073709524691),
    PRIME_T_C(18446744073709524773), PRIME_T_C(18446744073709524787),
    PRIME_T_C(18446744073709524791), PRIME_T_C(18446744073709524871),
    PRIME_T_C(18446744073709524901), PRIME_T_C(18446744073709524913),
    PRIME_T_C(18446744073709524919), PRIME_T_C(18446744073709524929),
    PRIME_T_C(18446744073709524983), PRIME_T_C(18446744073709524997),
    PRIME_T_C(18446744073709525039), PRIME_T_C(18446744073709525097),
    PRIME_T_C(18446744073709525099), PRIME_T_C(18446744073709525121),
    PRIME_T_C(18446744073709525247), PRIME_T_C(18446744073709525267),
    PRIME_T_C(18446744073709525391), PRIME_T_C(18446744073709525411),
    PRIME_T_C(18446744073709525489), PRIME_T_C(18446744073709525507),
    PRIME_T_C(18446744073709525513), PRIME_T_C(18446744073709525649),
    PRIME_T_C(18446744073709525687), PRIME_T_C(18446744073709525733),
    PRIME_T_C(18446744073709525751), PRIME_T_C(18446744073709525817),
    PRIME_T_C(18446744073709525831), PRIME_T_C(18446744073709525837),
    PRIME_T_C(18446744073709525853), PRIME_T_C(18446744073709525859),
    PRIME_T_C(18446744073709525867), PRIME_T_C(18446744073709525907),
    PRIME_T_C(18446744073709525961), PRIME_T_C(18446744073709525993),
    PRIME_T_C(18446744073709526129), PRIME_T_C(18446744073709526171),
    PRIME_T_C(18446744073709526173), PRIME_T_C(18446744073709526191),
    PRIME_T_C(18446744073709526213), PRIME_T_C(18446744073709526219),
    PRIME_T_C(18446744073709526233), PRIME_T_C(18446744073709526293),
    PRIME_T_C(18446744073709526299), PRIME_T_C(18446744073709526357),
    PRIME_T_C(18446744073709526369), PRIME_T_C(18446744073709526423),
    PRIME_T_C(18446744073709526567), PRIME_T_C(18446744073709526677),
    PRIME_T_C(18446744073709526741), PRIME_T_C(18446744073709526749),
    PRIME_T_C(18446744073709526789), PRIME_T_C(18446744073709526821),
    PRIME_T_C(18446744073709526909), PRIME_T_C(18446744073709527001),
    PRIME_T_C(18446744073709527007), PRIME_T_C(18446744073709527179),
    PRIME_T_C(18446744073709527181), PRIME_T_C(18446744073709527187),
    PRIME_T_C(18446744073709527193), PRIME_T_C(18446744073709527209),
    PRIME_T_C(18446744073709527229), PRIME_T_C(18446744073709527239),
    PRIME_T_C(18446744073709527293), PRIME_T_C(18446744073709527311),
    PRIME_T_C(18446744073709527343), PRIME_T_C(18446744073709527347),
    PRIME_T_C(18446744073709527361), PRIME_T_C(18446744073709527463),
    PRIME_T_C(18446744073709527547), PRIME_T_C(18446744073709527557),
    PRIME_T_C(18446744073709527623), PRIME_T_C(18446744073709527643),
    PRIME_T_C(18446744073709527647), PRIME_T_C(18446744073709527689),
    PRIME_T_C(18446744073709527833), PRIME_T_C(18446744073709527859),
    PRIME_T_C(18446744073709527913), PRIME_T_C(18446744073709527991),
    PRIME_T_C(18446744073709528033), PRIME_T_C(18446744073709528063),
    PRIME_T_C(18446744073709528079), PRIME_T_C(18446744073709528123),
    PRIME_T_C(18446744073709528193), PRIME_T_C(18446744073709528211),
    PRIME_T_C(18446744073709528259), PRIME_T_C(18446744073709528289),
    PRIME_T_C(18446744073709528303), PRIME_T_C(18446744073709528349),
    PRIME_T_C(18446744073709528397), PRIME_T_C(18446744073709528429),
    PRIME_T_C(18446744073709528477), PRIME_T_C(18446744073709528637),
    PRIME_T_C(18446744073709528669), PRIME_T_C(18446744073709528747),
    PRIME_T_C(18446744073709528781), PRIME_T_C(18446744073709528883),
    PRIME_T_C(18446744073709528961), PRIME_T_C(18446744073709528981),
    PRIME_T_C(18446744073709529033), PRIME_T_C(18446744073709529089),
    PRIME_T_C(18446744073709529119), PRIME_T_C(18446744073709529353),
    PRIME_T_C(18446744073709529377), PRIME_T_C(18446744073709529411),
    PRIME_T_C(18446744073709529443), PRIME_T_C(18446744073709529503),
    PRIME_T_C(18446744073709529531), PRIME_T_C(18446744073709529573),
    PRIME_T_C(18446744073709529693), PRIME_T_C(18446744073709529707),
    PRIME_T_C(18446744073709529741), PRIME_T_C(18446744073709529789),
    PRIME_T_C(18446744073709529797), PRIME_T_C(18446744073709529867),
    PRIME_T_C(18446744073709529881), PRIME_T_C(18446744073709529887),
    PRIME_T_C(18446744073709529953), PRIME_T_C(18446744073709529983),
    PRIME_T_C(18446744073709530199), PRIME_T_C(18446744073709530211),
    PRIME_T_C(18446744073709530259), PRIME_T_C(18446744073709530287),
    PRIME_T_C(18446744073709530317), PRIME_T_C(18446744073709530431),
    PRIME_T_C(18446744073709530461), PRIME_T_C(18446744073709530511),
    PRIME_T_C(18446744073709530539), PRIME_T_C(18446744073709530563),
    PRIME_T_C(18446744073709530587), PRIME_T_C(18446744073709530599),
    PRIME_T_C(18446744073709530653), PRIME_T_C(18446744073709530713),
    PRIME_T_C(18446744073709530823), PRIME_T_C(18446744073709530899),
    PRIME_T_C(18446744073709530919), PRIME_T_C(18446744073709530941),
    PRIME_T_C(18446744073709530947), PRIME_T_C(18446744073709531093),
    PRIME_T_C(18446744073709531123), PRIME_T_C(18446744073709531189),
    PRIME_T_C(18446744073709531201), PRIME_T_C(18446744073709531313),
    PRIME_T_C(18446744073709531343), PRIME_T_C(18446744073709531493),
    PRIME_T_C(18446744073709531507), PRIME_T_C(18446744073709531529),
    PRIME_T_C(18446744073709531549), PRIME_T_C(18446744073709531571),
    PRIME_T_C(18446744073709531607), PRIME_T_C(18446744073709531621),
    PRIME_T_C(18446744073709531627), PRIME_T_C(18446744073709531681),
    PRIME_T_C(18446744073709531703), PRIME_T_C(18446744073709531717),
    PRIME_T_C(18446744073709531751), PRIME_T_C(18446744073709531753),
    PRIME_T_C(18446744073709531771), PRIME_T_C(18446744073709531777),
    PRIME_T_C(18446744073709531781), PRIME_T_C(18446744073709531823),
    PRIME_T_C(18446744073709531873), PRIME_T_C(18446744073709531907),
    PRIME_T_C(18446744073709531913), PRIME_T_C(18446744073709531919),
    PRIME_T_C(18446744073709532117), PRIME_T_C(18446744073709532153),
    PRIME_T_C(18446744073709532159), PRIME_T_C(18446744073709532173),
    PRIME_T_C(18446744073709532261), PRIME_T_C(18446744073709532279),
    PRIME_T_C(18446744073709532281), PRIME_T_C(18446744073709532297),
    PRIME_T_C(18446744073709532393), PRIME_T_C(18446744073709532443),
    PRIME_T_C(18446744073709532467), PRIME_T_C(18446744073709532683),
    PRIME_T_C(18446744073709532747), PRIME_T_C(18446744073709532753),
    PRIME_T_C(18446744073709532761), PRIME_T_C(18446744073709532837),
    PRIME_T_C(18446744073709532891), PRIME_T_C(18446744073709533053),
    PRIME_T_C(18446744073709533097), PRIME_T_C(18446744073709533103),
    PRIME_T_C(18446744073709533133), PRIME_T_C(18446744073709533163),
    PRIME_T_C(18446744073709533173), PRIME_T_C(18446744073709533247),
    PRIME_T_C(18446744073709533259), PRIME_T_C(18446744073709533277),
    PRIME_T_C(18446744073709533349), PRIME_T_C(18446744073709533469),
    PRIME_T_C(18446744073709533497), PRIME_T_C(18446744073709533521),
    PRIME_T_C(18446744073709533599), PRIME_T_C(18446744073709533607),
    PRIME_T_C(18446744073709533679), PRIME_T_C(18446744073709533719),
    PRIME_T_C(18446744073709533797), PRIME_T_C(18446744073709533851),
    PRIME_T_C(18446744073709533913), PRIME_T_C(18446744073709533917),
    PRIME_T_C(18446744073709533941), PRIME_T_C(18446744073709533991),
    PRIME_T_C(18446744073709534087), PRIME_T_C(18446744073709534091),
    PRIME_T_C(18446744073709534097), PRIME_T_C(18446744073709534273),
    PRIME_T_C(18446744073709534301), PRIME_T_C(18446744073709534391),
    PRIME_T_C(18446744073709534393), PRIME_T_C(18446744073709534403),
    PRIME_T_C(18446744073709534439), PRIME_T_C(18446744073709534441),
    PRIME_T_C(18446744073709534529), PRIME_T_C(18446744073709534573),
    PRIME_T_C(18446744073709534589), PRIME_T_C(18446744073709534673),
    PRIME_T_C(18446744073709534753), PRIME_T_C(18446744073709534787),
    PRIME_T_C(18446744073709534811), PRIME_T_C(18446744073709534823),
    PRIME_T_C(18446744073709534837), PRIME_T_C(18446744073709534883),
    PRIME_T_C(18446744073709534937), PRIME_T_C(18446744073709534951),
    PRIME_T_C(18446744073709534963), PRIME_T_C(18446744073709534987),
    PRIME_T_C(18446744073709535039), PRIME_T_C(18446744073709535041),
    PRIME_T_C(18446744073709535113), PRIME_T_C(18446744073709535237),
    PRIME_T_C(18446744073709535239), PRIME_T_C(18446744073709535267),
    PRIME_T_C(18446744073709535359), PRIME_T_C(18446744073709535399),
    PRIME_T_C(18446744073709535417), PRIME_T_C(18446744073709535461),
    PRIME_T_C(18446744073709535471), PRIME_T_C(18446744073709535473),
    PRIME_T_C(18446744073709535531), PRIME_T_C(18446744073709535543),
    PRIME_T_C(18446744073709535587), PRIME_T_C(18446744073709535603),
    PRIME_T_C(18446744073709535621), PRIME_T_C(18446744073709535711),
    PRIME_T_C(18446744073709535741), PRIME_T_C(18446744073709535777),
    PRIME_T_C(18446744073709535783), PRIME_T_C(18446744073709536071),
    PRIME_T_C(18446744073709536073), PRIME_T_C(18446744073709536157),
    PRIME_T_C(18446744073709536161), PRIME_T_C(18446744073709536191),
    PRIME_T_C(18446744073709536259), PRIME_T_C(18446744073709536329),
    PRIME_T_C(18446744073709536401), PRIME_T_C(18446744073709536427),
    PRIME_T_C(18446744073709536463), PRIME_T_C(18446744073709536469),
    PRIME_T_C(18446744073709536491), PRIME_T_C(18446744073709536509),
    PRIME_T_C(18446744073709536577), PRIME_T_C(18446744073709536631),
    PRIME_T_C(18446744073709536707), PRIME_T_C(18446744073709536719),
    PRIME_T_C(18446744073709536727), PRIME_T_C(18446744073709536803),
    PRIME_T_C(18446744073709536863), PRIME_T_C(18446744073709536967),
    PRIME_T_C(18446744073709536997), PRIME_T_C(18446744073709537043),
    PRIME_T_C(18446744073709537109), PRIME_T_C(18446744073709537111),
    PRIME_T_C(18446744073709537129), PRIME_T_C(18446744073709537153),
    PRIME_T_C(18446744073709537219), PRIME_T_C(18446744073709537231),
    PRIME_T_C(18446744073709537241), PRIME_T_C(18446744073709537253),
    PRIME_T_C(18446744073709537331), PRIME_T_C(18446744073709537409),
    PRIME_T_C(18446744073709537447), PRIME_T_C(18446744073709537457),
    PRIME_T_C(18446744073709537499), PRIME_T_C(18446744073709537589),
    PRIME_T_C(18446744073709537633), PRIME_T_C(18446744073709537673),
    PRIME_T_C(18446744073709537723), PRIME_T_C(18446744073709537763),
    PRIME_T_C(18446744073709537793), PRIME_T_C(18446744073709537823),
    PRIME_T_C(18446744073709537837), PRIME_T_C(18446744073709537841),
    PRIME_T_C(18446744073709537849), PRIME_T_C(18446744073709537853),
    PRIME_T_C(18446744073709537871), PRIME_T_C(18446744073709537909),
    PRIME_T_C(18446744073709538023), PRIME_T_C(18446744073709538101),
    PRIME_T_C(18446744073709538123), PRIME_T_C(18446744073709538179),
    PRIME_T_C(18446744073709538183), PRIME_T_C(18446744073709538273),
    PRIME_T_C(18446744073709538317), PRIME_T_C(18446744073709538347),
    PRIME_T_C(18446744073709538393), PRIME_T_C(18446744073709538437),
    PRIME_T_C(18446744073709538443), PRIME_T_C(18446744073709538449),
    PRIME_T_C(18446744073709538459), PRIME_T_C(18446744073709538501),
    PRIME_T_C(18446744073709538567), PRIME_T_C(18446744073709538621),
    PRIME_T_C(18446744073709538677), PRIME_T_C(18446744073709538723),
    PRIME_T_C(18446744073709538747), PRIME_T_C(18446744073709538773),
    PRIME_T_C(18446744073709538819), PRIME_T_C(18446744073709538893),
    PRIME_T_C(18446744073709538941), PRIME_T_C(18446744073709539011),
    PRIME_T_C(18446744073709539017), PRIME_T_C(18446744073709539031),
    PRIME_T_C(18446744073709539109), PRIME_T_C(18446744073709539127),
    PRIME_T_C(18446744073709539229), PRIME_T_C(18446744073709539337),
    PRIME_T_C(18446744073709539361), PRIME_T_C(18446744073709539403),
    PRIME_T_C(18446744073709539421), PRIME_T_C(18446744073709539503),
    PRIME_T_C(18446744073709539551), PRIME_T_C(18446744073709539593),
    PRIME_T_C(18446744073709539611), PRIME_T_C(18446744073709539683),
    PRIME_T_C(18446744073709539689), PRIME_T_C(18446744073709539839),
    PRIME_T_C(18446744073709539871), PRIME_T_C(18446744073709539937),
    PRIME_T_C(18446744073709540013), PRIME_T_C(18446744073709540049),
    PRIME_T_C(18446744073709540063), PRIME_T_C(18446744073709540093),
    PRIME_T_C(18446744073709540201), PRIME_T_C(18446744073709540207),
    PRIME_T_C(18446744073709540247), PRIME_T_C(18446744073709540279),
    PRIME_T_C(18446744073709540291), PRIME_T_C(18446744073709540357),
    PRIME_T_C(18446744073709540403), PRIME_T_C(18446744073709540423),
    PRIME_T_C(18446744073709540441), PRIME_T_C(18446744073709540451),
    PRIME_T_C(18446744073709540469), PRIME_T_C(18446744073709540649),
    PRIME_T_C(18446744073709540709), PRIME_T_C(18446744073709540723),
    PRIME_T_C(18446744073709540753), PRIME_T_C(18446744073709540793),
    PRIME_T_C(18446744073709540907), PRIME_T_C(18446744073709540913),
    PRIME_T_C(18446744073709540951), PRIME_T_C(18446744073709540993),
    PRIME_T_C(18446744073709541069), PRIME_T_C(18446744073709541113),
    PRIME_T_C(18446744073709541119), PRIME_T_C(18446744073709541171),
    PRIME_T_C(18446744073709541203), PRIME_T_C(18446744073709541209),
    PRIME_T_C(18446744073709541257), PRIME_T_C(18446744073709541411),
    PRIME_T_C(18446744073709541459), PRIME_T_C(18446744073709541489),
    PRIME_T_C(18446744073709541519), PRIME_T_C(18446744073709541539),
    PRIME_T_C(18446744073709541621), PRIME_T_C(18446744073709541747),
    PRIME_T_C(18446744073709541753), PRIME_T_C(18446744073709541783),
    PRIME_T_C(18446744073709541813), PRIME_T_C(18446744073709541851),
    PRIME_T_C(18446744073709541893), PRIME_T_C(18446744073709542259),
    PRIME_T_C(18446744073709542443), PRIME_T_C(18446744073709542463),
    PRIME_T_C(18446744073709542467), PRIME_T_C(18446744073709542499),
    PRIME_T_C(18446744073709542527), PRIME_T_C(18446744073709542553),
    PRIME_T_C(18446744073709542583), PRIME_T_C(18446744073709542629),
    PRIME_T_C(18446744073709542713), PRIME_T_C(18446744073709542719),
    PRIME_T_C(18446744073709542751), PRIME_T_C(18446744073709542793),
    PRIME_T_C(18446744073709542853), PRIME_T_C(18446744073709542869),
    PRIME_T_C(18446744073709542881), PRIME_T_C(18446744073709542937),
    PRIME_T_C(18446744073709542953), PRIME_T_C(18446744073709542959),
    PRIME_T_C(18446744073709542989), PRIME_T_C(18446744073709542991),
    PRIME_T_C(18446744073709543027), PRIME_T_C(18446744073709543069),
    PRIME_T_C(18446744073709543117), PRIME_T_C(18446744073709543127),
    PRIME_T_C(18446744073709543159), PRIME_T_C(18446744073709543187),
    PRIME_T_C(18446744073709543223), PRIME_T_C(18446744073709543259),
    PRIME_T_C(18446744073709543297), PRIME_T_C(18446744073709543309),
    PRIME_T_C(18446744073709543463), PRIME_T_C(18446744073709543573),
    PRIME_T_C(18446744073709543639), PRIME_T_C(18446744073709543699),
    PRIME_T_C(18446744073709543751), PRIME_T_C(18446744073709543807),
    PRIME_T_C(18446744073709543901), PRIME_T_C(18446744073709543913),
    PRIME_T_C(18446744073709543919), PRIME_T_C(18446744073709543937),
    PRIME_T_C(18446744073709544003), PRIME_T_C(18446744073709544039),
    PRIME_T_C(18446744073709544221), PRIME_T_C(18446744073709544269),
    PRIME_T_C(18446744073709544279), PRIME_T_C(18446744073709544399),
    PRIME_T_C(18446744073709544449), PRIME_T_C(18446744073709544539),
    PRIME_T_C(18446744073709544573), PRIME_T_C(18446744073709544581),
    PRIME_T_C(18446744073709544591), PRIME_T_C(18446744073709544623),
    PRIME_T_C(18446744073709544627), PRIME_T_C(18446744073709544633),
    PRIME_T_C(18446744073709544699), PRIME_T_C(18446744073709544717),
    PRIME_T_C(18446744073709544839), PRIME_T_C(18446744073709544941),
    PRIME_T_C(18446744073709544947), PRIME_T_C(18446744073709545109),
    PRIME_T_C(18446744073709545169), PRIME_T_C(18446744073709545229),
    PRIME_T_C(18446744073709545233), PRIME_T_C(18446744073709545421),
    PRIME_T_C(18446744073709545533), PRIME_T_C(18446744073709545577),
    PRIME_T_C(18446744073709545661), PRIME_T_C(18446744073709545673),
    PRIME_T_C(18446744073709545677), PRIME_T_C(18446744073709545689),
    PRIME_T_C(18446744073709545697), PRIME_T_C(18446744073709545743),
    PRIME_T_C(18446744073709545779), PRIME_T_C(18446744073709545809),
    PRIME_T_C(18446744073709545817), PRIME_T_C(18446744073709545871),
    PRIME_T_C(18446744073709545911), PRIME_T_C(18446744073709545953),
    PRIME_T_C(18446744073709546027), PRIME_T_C(18446744073709546079),
    PRIME_T_C(18446744073709546093), PRIME_T_C(18446744073709546247),
    PRIME_T_C(18446744073709546253), PRIME_T_C(18446744073709546271),
    PRIME_T_C(18446744073709546289), PRIME_T_C(18446744073709546333),
    PRIME_T_C(18446744073709546337), PRIME_T_C(18446744073709546363),
    PRIME_T_C(18446744073709546391), PRIME_T_C(18446744073709546409),
    PRIME_T_C(18446744073709546429), PRIME_T_C(18446744073709546493),
    PRIME_T_C(18446744073709546541), PRIME_T_C(18446744073709546561),
    PRIME_T_C(18446744073709546601), PRIME_T_C(18446744073709546643),
    PRIME_T_C(18446744073709546657), PRIME_T_C(18446744073709546729),
    PRIME_T_C(18446744073709546739), PRIME_T_C(18446744073709546841),
    PRIME_T_C(18446744073709546873), PRIME_T_C(18446744073709546879),
    PRIME_T_C(18446744073709546897), PRIME_T_C(18446744073709547003),
    PRIME_T_C(18446744073709547087), PRIME_T_C(18446744073709547117),
    PRIME_T_C(18446744073709547303), PRIME_T_C(18446744073709547317),
    PRIME_T_C(18446744073709547357), PRIME_T_C(18446744073709547371),
    PRIME_T_C(18446744073709547471), PRIME_T_C(18446744073709547473),
    PRIME_T_C(18446744073709547489), PRIME_T_C(18446744073709547521),
    PRIME_T_C(18446744073709547537), PRIME_T_C(18446744073709547657),
    PRIME_T_C(18446744073709547669), PRIME_T_C(18446744073709547707),
    PRIME_T_C(18446744073709547731), PRIME_T_C(18446744073709547777),
    PRIME_T_C(18446744073709547797), PRIME_T_C(18446744073709547821),
    PRIME_T_C(18446744073709548053), PRIME_T_C(18446744073709548073),
    PRIME_T_C(18446744073709548119), PRIME_T_C(18446744073709548193),
    PRIME_T_C(18446744073709548239), PRIME_T_C(18446744073709548271),
    PRIME_T_C(18446744073709548287), PRIME_T_C(18446744073709548349),
    PRIME_T_C(18446744073709548353), PRIME_T_C(18446744073709548379),
    PRIME_T_C(18446744073709548391), PRIME_T_C(18446744073709548397),
    PRIME_T_C(18446744073709548481), PRIME_T_C(18446744073709548497),
    PRIME_T_C(18446744073709548503), PRIME_T_C(18446744073709548511),
    PRIME_T_C(18446744073709548557), PRIME_T_C(18446744073709548587),
    PRIME_T_C(18446744073709548599), PRIME_T_C(18446744073709548703),
    PRIME_T_C(18446744073709548809), PRIME_T_C(18446744073709548847),
    PRIME_T_C(18446744073709548859), PRIME_T_C(18446744073709548887),
    PRIME_T_C(18446744073709548899), PRIME_T_C(18446744073709548983),
    PRIME_T_C(18446744073709549019), PRIME_T_C(18446744073709549061),
    PRIME_T_C(18446744073709549067), PRIME_T_C(18446744073709549123),
    PRIME_T_C(18446744073709549153), PRIME_T_C(18446744073709549237),
    PRIME_T_C(18446744073709549307), PRIME_T_C(18446744073709549327),
    PRIME_T_C(18446744073709549331), PRIME_T_C(18446744073709549363),
    PRIME_T_C(18446744073709549441), PRIME_T_C(18446744073709549483),
    PRIME_T_C(18446744073709549519), PRIME_T_C(18446744073709549571),
    PRIME_T_C(18446744073709549583), PRIME_T_C(18446744073709549613),
    PRIME_T_C(18446744073709549621), PRIME_T_C(18446744073709549667),
    PRIME_T_C(18446744073709549733), PRIME_T_C(18446744073709549757),
    PRIME_T_C(18446744073709549777), PRIME_T_C(18446744073709549811),
    PRIME_T_C(18446744073709549817), PRIME_T_C(18446744073709549861),
    PRIME_T_C(18446744073709549951), PRIME_T_C(18446744073709550009),
    PRIME_T_C(18446744073709550033), PRIME_T_C(18446744073709550047),
    PRIME_T_C(18446744073709550099), PRIME_T_C(18446744073709550111),
    PRIME_T_C(18446744073709550129), PRIME_T_C(18446744073709550141),
    PRIME_T_C(18446744073709550147), PRIME_T_C(18446744073709550237),
    PRIME_T_C(18446744073709550293), PRIME_T_C(18446744073709550341),
    PRIME_T_C(18446744073709550381), PRIME_T_C(18446744073709550537),
    PRIME_T_C(18446744073709550539), PRIME_T_C(18446744073709550591),
    PRIME_T_C(18446744073709550593), PRIME_T_C(18446744073709550671),
    PRIME_T_C(18446744073709550681), PRIME_T_C(18446744073709550717),
    PRIME_T_C(18446744073709550719), PRIME_T_C(18446744073709550771),
    PRIME_T_C(18446744073709550773), PRIME_T_C(18446744073709550791),
    PRIME_T_C(18446744073709550873), PRIME_T_C(18446744073709551113),
    PRIME_T_C(18446744073709551163), PRIME_T_C(18446744073709551191),
    PRIME_T_C(18446744073709551253), PRIME_T_C(18446744073709551263),
    PRIME_T_C(18446744073709551293), PRIME_T_C(18446744073709551337),
    PRIME_T_C(18446744073709551359), PRIME_T_C(18446744073709551427),
    PRIME_T_C(18446744073709551437), PRIME_T_C(18446744073709551521),
    PRIME_T_C(18446744073709551533), PRIME_T_C(18446744073709551557)
} ;

#endif	/* Unsigned longs are at least 64 bits wide. */

/*******************************************************************************
    PRIME's Main Program.
*******************************************************************************/

int  main (

#    if PROTOTYPES
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

{    /* Local variables. */
    BmwClock  clock ;
    bool  benchmark, help ;
    char  *argument, *s ;
    const  char  *operation, *subop ;
    const  char  *program = "primate" ;
    int  errflg, option ;
    IsPrimeFunc  isPrimeF = &nbrIsPrime ;
    OptContext  scan ;
    size_t  i, length, numDiffsPerRow, repeat ;
    ssize_t  count, recount ;
    unsigned  char  *sequence = NULL ;
    prime_t  maxDiff, next, number ;
    prime_t  rangeEnd, rangeStart ;
    prime_t  skipThroughPrime, startDivisor ;
    const  prime_t  *primesList ;
    void  *config ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}",
        "{alternate:}", "{list:}",
        "{maximum:}", "{next:}",
        "{qc#kpi:}", "{qgen:}",
        "{qinfo}", "{q128info}",
        "{qmax:}", "{qprime:}",
        "{repeat:}",  "{time}",
        "{verify:}", "{xtra}", NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

	/* Default skip-through prime of 7 */
    gLength = nbrPrimeSeqDiffsLength (7, &gSkipThroughPrime, &gStartDivisor) ;
    gRange = nbrPrimeSeqDiffsRange (gSkipThroughPrime) ;
    gSequence = nbrPrimeSeqDiffs (gSkipThroughPrime, NULL, NULL) ;
    config = nbrPrimeConfigCreate (7, gSequence, true) ;

    help = false ;
    benchmark = false ;
    primesList = NULL ;
    repeat = 1 ;

    operation = NULL ;  subop = NULL ;

    count = 0 ;  number = 0 ;
    rangeStart = 0 ;  rangeEnd = 0 ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;

    while ((option = opt_get (scan, &argument)) && !errflg) {

        switch (option) {
        case 1:				/* "-debug" */
            nbr_util_debug = 1 ;
            break ;
        case 2:				/* "-help" */
            help = true ;
            break ;
        case 3:				/* "-alternate <odds>|<ones>|<skip23>|<diffs>" */
            if (strMatch (argument, "odds", true))
                isPrimeF = &isPrimeOdds ;
            else if (strMatch (argument, "ones", true))
                isPrimeF = &isPrimeOnes ;
            else if (strMatch (argument, "skip23", true))
                isPrimeF = &isPrimeSkip23 ;
            else if (strMatch (argument, "diffs", true))
                isPrimeF = &isPrimeDiffs ;
            else
                errflg++ ;
            break ;
        case 4:				/* "-list <start>" */
					/* "-list <start>,<count>" */
					/* "-list <start>-<end>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            rangeStart = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s == '\0') {
                subop = "range" ;
                rangeEnd = PRIME_T_MAX ;
            } else if (*s == ',') {
                subop = "count" ;
                count = strtol (++s, &s, 0) ;
            } else if (*s == '-') {
                subop = "range" ;
                rangeEnd = STRTO_PRIME_T (++s, &s, 0) ;
            }
            if (*s != '\0')  errflg++ ;
            operation = "list" ;
            break ;
        case 5:				/* "-maximum 32|64" */
#if PRIME_T_ATLEAST_BITS >= 32
            if (strcmp (argument, "32") == 0) {
					/* Check maximum 32-bit prime. */
                number = lastPrimes_32[MAX_PRIMES_LIST-1] ;
                printf ("Max 32-bit Prime = %"PRIuPRIME_T"\n", number) ;
                printf ("Max 32-bit Value = 4,294,967,295\n") ;
            } else
#endif
#if PRIME_T_ATLEAST_BITS >= 64
            if ((strcmp (argument, "64") == 0) &&
                       (sizeof (prime_t) >= 8)) {
					/* Check maximum 64-bit prime. */
                number = lastPrimes_64[MAX_PRIMES_LIST-1] ;
                printf ("Max 64-bit Prime = %"PRIuPRIME_T"\n", number) ;
                printf ("Max 64-bit Value = 18,446,744,073,709,551,615\n") ;
            } else
#endif
            {
                errflg++ ;  break ;
            }
            operation = "check" ;
            break ;
        case 6:				/* "-next <number>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            number = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s == '\0')
                operation = "next" ;
            else
                errflg++ ;
            break ;
        case 7:				/* "-qc#kpi <prime>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            number = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            operation = "diffs" ;
            subop = "c#k+i" ;
            break ;
        case 8:				/* "-qgen <prime>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            number = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            operation = "diffs" ;
            subop = "generate" ;
            break ;
        case 9:				/* "-qinfo" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "diffs" ;
            subop = "information" ;
            break ;
        case 10:			/* "-q128info" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
#if defined(HAVE___INT128) && HAVE___INT128
            operation = "diffs" ;
            subop = "128information" ;
#else
            errflg++ ;
#endif
            break ;
        case 11:			/* "-qmax <prime>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            number = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            operation = "diffs" ;
            subop = "maximum" ;
            break ;
        case 12:			/* "-qprime <prime>" */
            gSkipThroughPrime = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s != '\0') {
                errflg++ ;  break ;
            }
            gLength = nbrPrimeSeqDiffsLength (gSkipThroughPrime,
                                              &gSkipThroughPrime,
                                              &gStartDivisor) ;
            if (gLength == 0) {
                errflg++ ;  break ;
            }
            gRange = nbrPrimeSeqDiffsRange (gSkipThroughPrime) ;
            if (gRange == 0) {
                errflg++ ;  break ;
            }
            printf ("Skip-Through Prime: %"PRIuPRIME_T"  Sequence Length: %"PRIuSIZE_T"  Generating ... ",
                    gSkipThroughPrime, gLength) ;
            fflush (stdout) ;
            gSequence = nbrPrimeSeqDiffs (gSkipThroughPrime, NULL, NULL) ;
            printf ("Done\n") ;
            if (gSequence == NULL) {
                errflg++ ;  break ;
            }
            config = nbrPrimeConfigCreate (gSkipThroughPrime, gSequence, true) ;
            if (config == NULL)  errflg++ ;
            break ;
        case 13:			/* "-repeat <count>" */
            repeat = strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 14:			/* "-time" */
            benchmark = true ;
            break ;
        case 15:			/* "-verify first|last|last32|last64" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            strlwr (argument) ;
            if (strMatch (argument, "first", true)) {
                subop = "first" ;
                primesList = firstPrimes ;
            } else if (strMatch (argument, "last", true)) {
                if (sizeof (prime_t) >= 8) {
                    subop = "last64" ;
                    primesList = lastPrimes_64 ;
                } else {
                    subop = "last32" ;
                    primesList = lastPrimes_32 ;
                }
            } else
#if PRIME_T_ATLEAST_BITS >= 64
            if (strMatch (argument, "last64", true)) {
                if (sizeof (prime_t) >= 8) {
                    subop = "last64" ;
                    primesList = lastPrimes_64 ;
                } else {
                    errflg++ ;  break ;
                }
            } else
#endif
#if (PRIME_T_ATLEAST_BITS >= 32)
            if (strMatch (argument, "last32", true)) {
                subop = "last32" ;
                primesList = lastPrimes_32 ;
            } else
#endif
            {
                errflg++ ;  break ;
            }
            operation = "verify" ;
            count = MAX_PRIMES_LIST ;
            rangeStart = (primesList == NULL) ? 0 : primesList[0] - 1 ;
            break ;
        case 16:			/* "-xtra" */
					/* Predefined constant to check. */
            number = PRIME_T_C(4294945091) ;
            operation = "check" ;
            break ;
        case NONOPT:			/* "<number>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            number = STRTO_PRIME_T (argument, &s, 0) ;
            if (*s == '\0')
                operation = "check" ;
            else
                errflg++ ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (scan) ;

/*******************************************************************************
    Print help information, if necessary.
*******************************************************************************/

    if (operation == NULL)
        errflg++ ;	/* No operation specified on the command line. */

    if (errflg || help) {
        int  indent = (int) strlen ("Usage:  ") + (int) strlen (program) ;
        fprintf (stderr, "Usage:  %s [-debug] [-help]\n", program) ;
        fprintf (stderr, "\t\t\t\t\t# Operations\n") ;
        fprintf (stderr, "%*s [-list <start>]\n", indent, "") ;
        fprintf (stderr, "%*s [-list <start>,<count>]\n", indent, "") ;
        fprintf (stderr, "%*s [-list <start>-<end>]\n", indent, "") ;
        fprintf (stderr, "%*s [-maximum 32|64]\n", indent, "") ;
        fprintf (stderr, "%*s [-next <number>]\n", indent, "") ;
        fprintf (stderr, "%*s [-qc#kpi <prime>]\n", indent, "") ;
        fprintf (stderr, "%*s [-qgen <prime>]\n", indent, "") ;
        fprintf (stderr, "%*s [-qinfo] [-q128info]\n", indent, "") ;
        fprintf (stderr, "%*s [-qmax <prime>]\n", indent, "") ;
        fprintf (stderr, "%*s [-verify first|last|last32|last64]\n", indent, "") ;
        fprintf (stderr, "%*s [-xtra]\n", indent, "") ;
        fprintf (stderr, "%*s [<number>]\n", indent, "") ;
        fprintf (stderr, "\t\t\t\t\t# Modifiers\n") ;
        fprintf (stderr, "%*s [-alternate odds|ones|skip23|diffs]\n", indent, "") ;
        fprintf (stderr, "%*s [-qprime <prime>]\n", indent, "") ;
        fprintf (stderr, "%*s [-repeat <count>] [-time]\n", indent, "") ;
    }

    if (errflg && !help) {
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Enter \"%s -help\" for detailed help.\n", program) ;
    }

    if (help) {
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Notes:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    sizeof (prime_t) = %"PRIuSIZE_T" bytes\n",
                 sizeof (prime_t)) ;
        fprintf (stderr, "    Range: 0 ... %"PRIuPRIME_T")\n", PRIME_T_MAX) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Operations:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %s <number>\n", program) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        Check if <number> is prime.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %s -list <start>\n", program) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        List all primes beginning with and following <start>.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %s -list <start>,[-]<count>\n", program) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        List <count> primes.  If <count> is positive, the first <count>\n") ;
        fprintf (stderr, "        primes beginning with and following <start> are listed.  If\n") ;
        fprintf (stderr, "        <count> is negative, the last |<count>| primes preceding and\n") ;
        fprintf (stderr, "        ending with <start> are listed in reverse (i.e., descending) order.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %s -list <start>-<end>\n", program) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        List all primes in the range <start>...<end>, inclusive.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %s -next <number>\n", program) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        Display the next prime that follows <number> (even if <number>\n") ;
        fprintf (stderr, "        itself is prime).\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %s -verify first|last|last32|last64\n", program) ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        Verify that the prime function generates the correct prime\n") ;
        fprintf (stderr, "        numbers.  The generated prime numbers are compared to tables\n") ;
        fprintf (stderr, "        of the known primes in 3 ranges:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        (1) \"-verify first\" verifies the first 1000 prime numbers.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        (2) \"-verify last\" verifies the last 1000 prime numbers that\n") ;
        fprintf (stderr, "            can be represented in an unsigned long.  If the width of\n") ;
        fprintf (stderr, "            an unsigned long is 64 bits or greater, the last 1000 prime\n") ;
        fprintf (stderr, "            numbers preceding 2^64 are verified.  If the width is less\n") ;
        fprintf (stderr, "            than 64 bits, the last 1000 prime numbers preceding 2^32\n") ;
        fprintf (stderr, "            are verified.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        (4) \"-verify last32\" verifies the last 1000 prime numbers\n") ;
        fprintf (stderr, "            preceding 2^32; this option is useful if the native\n") ;
        fprintf (stderr, "            unsigned longs are wider than 32 bits (e.g., 64 bits).\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        NOTE that \"-verify last\" on a CPU with 64-bit longs takes a very\n") ;
        fprintf (stderr, "        long time.  On my old Fedora Core 15 machine, 24.8 hours to be\n") ;
        fprintf (stderr, "        exact!\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Options:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -alternate odds|ones|skip23|diffs\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        use an alternate is-prime? function for comparison purposes.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "            (1) The \"odds\" function implements a fast, hand-coded loop\n") ;
        fprintf (stderr, "                that checks all odd divisors.\n") ;
        fprintf (stderr, "            (2) The \"ones\" function implements a \"fast\", hand-coded loop\n") ;
        fprintf (stderr, "                that checks every single divisor: 2, 3, 4, 5, 6, 7, ...\n") ;
        fprintf (stderr, "            (3) The \"skip23\" function implements a fast, hand-coded loop\n") ;
        fprintf (stderr, "                that skips divisors divisible by 2 or by 3; this is\n") ;
        fprintf (stderr, "                equivalent to having a skip-through prime of 3, but\n") ;
        fprintf (stderr, "                it does not use the generic, sequence of differences\n") ;
        fprintf (stderr, "                algorithm.\n") ;
        fprintf (stderr, "            (4) The \"diffs\" function implements the generic, sequence\n") ;
        fprintf (stderr, "                of differences algorithm for different skip-through\n") ;
        fprintf (stderr, "                primes (specified with the \"-qprime <stp>\" option).\n") ;
        fprintf (stderr, "                This function is virtually identical to the library\n") ;
        fprintf (stderr, "                function, nbrIsPrime(), except you can modify or replace\n") ;
        fprintf (stderr, "                the function in the test program in order to experiment\n") ;
        fprintf (stderr, "                with your own optimizations.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -qprime <stp>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        specifies the skip-through prime to use with the library function,\n") ;
        fprintf (stderr, "        nbrIsPrime(), or with this program's internal \"-alternate diffs\"\n") ;
        fprintf (stderr, "        function.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -repeat <count>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        perform the specified operation <count> times.  This option is\n") ;
        fprintf (stderr, "        useful in conjunction with the \"-time\" option when a single\n") ;
        fprintf (stderr, "        iteration of the operation is too quick to provide a meaningful\n") ;
        fprintf (stderr, "        time measurement.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -time\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        measure and display the time it takes to perform an operation\n") ;
        fprintf (stderr, "        (or <count> iterations of the operation if \"-repeat <count>\" is\n") ;
        fprintf (stderr, "        also given on the command line).  Almost all output is suppressed\n") ;
        fprintf (stderr, "        to keep I/O from affecting the timing.\n") ;
    }

    if (errflg || help)  exit (EINVAL) ;

/*******************************************************************************
    Perform the specified operation.
*******************************************************************************/

    bmwStart (&clock) ;

/*******************************************************************************
    Check if a number is prime.
*******************************************************************************/

    if (strcmp (operation, "check") == 0) {

        while (repeat-- > 0) {
            if (isPrimeF (number, config)) {
                if (!benchmark)  printf ("%"PRIuPRIME_T" is prime.\n", number) ;
            }  else {
                if (!benchmark)  printf ("%"PRIuPRIME_T" is not prime.\n", number) ;
            }
        }

    }

/*******************************************************************************
    Generate the set of values for i in c#k+i; each i is coprime with c#.
*******************************************************************************/

    else if ((strcmp (operation, "diffs") == 0) &&
             (strcmp (subop, "c#k+i") == 0)) {

        prime_t  icopcup ;		/* I COPrime with C-UP (C# musically) */


        while (repeat-- > 0) {
            sequence = nbrPrimeSeqDiffs (number, &startDivisor, &length) ;
            if (sequence == NULL) {
                printf ("Failed to generate sequence for skip-through prime %"PRIuPRIME_T".\n",
                        number) ;
                exit (errno) ;
            }
            if (repeat > 0)  free (sequence) ;	/* Save only last sequence. */
        }

        bmwStop (&clock) ;

/* The values for i range from 1 to c#-1, inclusive.  They are computed from
   the sequence of differences as follows:

            i = 1
            i += last-diff	(1 + last-diff = next-prime, AKA start divisor)
            i += first-diff
            i += second-diff
            ...
            i += next-to-next-to-last-diff	(= c#-1)

  The next-to-last-diff is skipped: it is always 2 and steps to the starting i
  of 1 in the next c#k block. */

        icopcup = 1 ;
        printf ("%"PRIuPRIME_T"\n", icopcup) ;

        if (number > 2) {	/* When c is 2, i has only one value: one. */
            icopcup += sequence[length-1] ;
            printf ("%"PRIuPRIME_T"\n", icopcup) ;
            for (i = 0 ;  i < (length-2) ;  i++) {
                icopcup += sequence[i] ;
                printf ("%"PRIuPRIME_T"\n", icopcup) ;
            }
        }

        free (sequence) ;

    }

/*******************************************************************************
    Generate the sequence of differences for a skip-through prime.
*******************************************************************************/

    else if ((strcmp (operation, "diffs") == 0) &&
             (strcmp (subop, "generate") == 0)) {

        while (repeat-- > 0) {
            sequence = nbrPrimeSeqDiffs (number, &startDivisor, &length) ;
            if (sequence == NULL) {
                printf ("Failed to generate sequence for skip-through prime %"PRIuPRIME_T".\n",
                        number) ;
                exit (errno) ;
            }
            if (repeat > 0)  free (sequence) ;	/* Save only last sequence. */
        }

        bmwStop (&clock) ;

        numDiffsPerRow = (number < 17) ? 16 : 32 ;

        for (i = 0 ;  sequence[i] > 0 ;  i++) {
            if (number < 17) {
                if ((i % numDiffsPerRow) == 0)  printf ("    ") ;
                printf ("%3u,", sequence[i]) ;
            } else {
                printf ("%u,", sequence[i]) ;
            }
            if ((i % numDiffsPerRow) == (numDiffsPerRow - 1))  printf ("\n") ;
        }

        if ((i % numDiffsPerRow) != 0)  printf ("\n") ;

        free (sequence) ;

    }

/*******************************************************************************
    Print information about sequences of differences for increasing
    skip-through primes until the sequence length overflows its maximum
    value.
*******************************************************************************/

    else if ((strcmp (operation, "diffs") == 0) &&
             (strcmp (subop, "information") == 0)) {

        double  density, drange ;
        prime_t  range ;

        printf (" Skip-                                    Estimated     Sequence Range\n") ;
        printf ("Through     Start                          Maximum         (Sum of\n") ;
        printf (" Prime     Divisor    Sequence Length     Difference     Differences)      L/R\n") ;
        printf ("-------    -------    ---------------     ----------    --------------     ---\n") ;
             /* "  99         99    12345678901234567890      999   12345678901234567890   123%" */

        number = 1 ;  range = 1 ;
        startDivisor = 2 ;

        for ( ; ; ) {

            skipThroughPrime = startDivisor ;

            drange = (double) range * (double) skipThroughPrime ;

            length = nbrPrimeSeqDiffsLength (skipThroughPrime,
                                             NULL, &startDivisor) ;
            if (length == 0)  break ;

            range = nbrPrimeSeqDiffsRange (skipThroughPrime) ;
            if (range > 0)
                density = (double) length / (double) range ;
            else
                density = (double) length / drange ;

            printf ("  %2"PRIuPRIME_T"         %2"PRIuPRIME_T"    %20"PRIuSIZE_T"      %3"PRIuPRIME_T"   %20"PRIuPRIME_T"   %3d%%\n",
                    skipThroughPrime, startDivisor, length,
                    number * 2, range, (int) (density * 100.0)) ;

            number = skipThroughPrime ;

        }

        printf ("     PRIME_T_MAX:  %20"PRIuPRIME_T"\n", PRIME_T_MAX) ;

    }

#if defined(HAVE___INT128) && HAVE___INT128
/*******************************************************************************
    Print information about sequences of differences for increasing
    skip-through primes until the sequence length overflows its maximum
    value.
*******************************************************************************/

    else if ((strcmp (operation, "diffs") == 0) &&
             (strcmp (subop, "128information") == 0)) {

        char  buffer1[64], buffer2[64], buffer3[64] ;
        double  density ;
        size_t  i ;
        prime_t  maxDiff, skipThroughPrime, startDivisor ;
        unsigned  __int128  length128, max128, range128 ;



        max128 = ~0 ;
        sprint128u (buffer3, max128) ;
        printf ("128-bit MAX: %40s\n", buffer3) ;

        skipThroughPrime = 1 ;
        startDivisor = 2 ;
        length128 = 1 ;  range128 = 1 ;

        for (i = 0 ;  (length128 > 0) && (range128 > 0) ;  i++) {

            maxDiff = skipThroughPrime * 2 ;

            skipThroughPrime = firstPrimes[i] ;
            startDivisor = firstPrimes[i+1] ;

            if (skipThroughPrime == 2) {
                length128 = 1 ;
                range128 = 2 ;
            } else {
                if (length128 < (max128 / (skipThroughPrime - 1))) {
                    length128 = length128 * (skipThroughPrime - 1) ;
                } else {
                    printf ("Length overflow\n") ;
                    length128 = 0 ;
                }
                if (range128 < (max128 / skipThroughPrime)) {
                    range128 = range128 * skipThroughPrime ;
                } else {
                    printf ("Range overflow\n") ;
                    range128 = 0 ;
                }
            }

/* If the length or range overflow the maximum representable values, stop
   printing and exit the loop.  (For 64-bit numbers, the range overflows
   one iteration prior to the length overflowing.  For 128-bit numbers,
   the range and length overflow in the same iteration.) */

            if ((length128 == 0) || (range128 == 0))  break ;

            density = (double) length128 / (double) range128 ;

            sprint128u (buffer1, length128) ;
            sprint128u (buffer2, range128) ;

            printf ("\nSTP:%4"PRIuPRIME_T"  SDiv:%4"PRIuPRIME_T"  Est.Max.Diff:%4"PRIuPRIME_T"  Density: %3d%%\nLength: %40s    (%11g)\n Range: %40s    (%11g)\n   Max: %40s    (%11g)\n",
                    skipThroughPrime, startDivisor, maxDiff,
                    (int) (density * 100.0),
                    buffer1, (double) length128,
                    buffer2, (double) range128,
                    buffer3, (double) max128) ;

        }

    }
#endif

/*******************************************************************************
    Determine the maximum difference in a sequence of differences.
*******************************************************************************/

    else if ((strcmp (operation, "diffs") == 0) &&
             (strcmp (subop, "maximum") == 0)) {

        length = nbrPrimeSeqDiffsLength (number,
                                         &skipThroughPrime,
                                         &startDivisor) ;
        if (length == 0) {
            printf ("Failed to calculate sequence length for skip-through prime %"PRIuPRIME_T".\n",
                    number) ;
            exit (errno) ;
        }
        printf ("   Sequence Length = %"PRIuSIZE_T"\n", length) ;
        printf ("    Sequence Range = %"PRIuPRIME_T"\n",
                nbrPrimeSeqDiffsRange (number)) ;
        printf ("Skip-Through Prime = %"PRIuPRIME_T"\n", skipThroughPrime) ;
        printf ("     Start Divisor = %"PRIuPRIME_T"\n", startDivisor) ;

        maxDiff = 0 ;

        while (repeat-- > 0) {
            maxDiff = nbrPrimeSeqDiffsMax (skipThroughPrime) ;
        }

        printf ("Maximum Difference = %"PRIuPRIME_T"\n", maxDiff) ;

    }

/*******************************************************************************
    List the N prime numbers preceding or following a given number, inclusive.
*******************************************************************************/

    else if ((strcmp (operation, "list") == 0) &&
             (strcmp (subop, "count") == 0)) {

        recount = count ;	/* Save count for repeats. */

        if (count < 0) {	/* List N preceding prime numbers. */

            while (repeat-- > 0) {
                count = recount ;
                number = rangeStart ;
                while ((count < 0) && (number > 1)) {
                    if (isPrimeF (number, config)) {
                        if (!benchmark)  printf ("%"PRIuPRIME_T"\n", number) ;
                        count++ ;
                    }
                    number-- ;
                }
            }

        } else {		/* List N following prime numbers. */

            if (isPrimeF (rangeStart, config))  rangeStart-- ;

            while (repeat-- > 0) {
                count = recount ;
                for (number = nbrNextPrimeF (rangeStart, isPrimeF, config) ;
                     count-- > 0 ;
                     number = nbrNextPrimeF (number, isPrimeF, config)) {
                    if (number == 0)  exit (errno) ;		/* Overflow. */
                    if (!benchmark)  printf ("%"PRIuPRIME_T"\n", number) ;
                }
            }

        }

    }


/*******************************************************************************
    List all prime numbers within a specified range, inclusive.
*******************************************************************************/

    else if ((strcmp (operation, "list") == 0) &&
             (strcmp (subop, "range") == 0)) {

        while (repeat-- > 0) {
            for (number = rangeStart ;  number <= rangeEnd ;  number++) {
                if (isPrimeF (number, config)) {
                    if (!benchmark)  printf ("%"PRIuPRIME_T"\n", number) ;
                }
            }
        }

    }


/*******************************************************************************
    Display the next prime number following a given number, regardless
    if the given number itself is prime.
*******************************************************************************/

    else if (strcmp (operation, "next") == 0) {

        while (repeat-- > 0) {
            next = nbrNextPrimeF (number, isPrimeF, config) ;
            if (!benchmark)  printf ("Next prime after %"PRIuPRIME_T" is %"PRIuPRIME_T".\n",
                                     number, next) ;
        }

    }

/*******************************************************************************
    Verify that the NBR_UTIL functions generate the correct prime numbers.
    The numbers generated are compared to predefined lists of prime numbers
    found elsewhere.
*******************************************************************************/

    else if (strcmp (operation, "verify") == 0) {

        rangeStart = primesList[0] - 1 ;

        while (repeat-- > 0) {
            count = MAX_PRIMES_LIST ;
            for (i = 0,  number = nbrNextPrimeF (rangeStart, isPrimeF, config) ;
                 count-- > 0 ;
                 i++,  number = nbrNextPrimeF (number, isPrimeF, config)) {
                if (number != primesList[i]) {
                    printf ("Verification error - Index: %"PRIuSIZE_T"  Generated: %"PRIuPRIME_T"  Actual: %"PRIuPRIME_T"\n",
                            i+1, number, primesList[i]) ;
                    exit (EINVAL) ;
                }
#if PRIME_T_ATLEAST_BITS >= 64
                if (primesList == lastPrimes_64) {
                    printf ("VFY [%"PRIuSIZE_T"] %"PRIuPRIME_T" (%.5f seconds)\n",
                            i, number, bmwElapsed (&clock)) ;
                    fflush (stdout) ;
                }
#endif
            }
            if (!benchmark || (repeat == 0)) {
                printf ("Verified %s %"PRIuSIZE_T" prime numbers.\n",
                        subop, i) ;
            }
        }

    }


    bmwStop (&clock) ;

    if (benchmark) {
#if defined(HAVE_PRINTF_FORMAT_FLOAT) && !HAVE_PRINTF_FORMAT_FLOAT
        struct  timeval  elapsedTime = bmwElapsedTV (&clock) ;

        if (isPrimeF == &nbrIsPrime) {
				/* nbrIsPrime() doesn't count divisors. */
            printf ("%ld seconds, %ld usecs (%s%s%s)\n",
                    elapsedTime.tv_sec, elapsedTime.tv_usec, operation,
                    (subop == NULL) ? "" : ", ",
                    (subop == NULL) ? "" : subop) ;
        } else {
				/* Alternate functions count divisors */
            printf ("%ld seconds, %ld usecs (%s%s%s) %"PRIuSIZE_T" divisors\n",
                    elapsedTime.tv_sec, elapsedTime.tv_usec, operation,
                    (subop == NULL) ? "" : ", ",
                    (subop == NULL) ? "" : subop,
                    gNumDivisors) ;
        }
#else
        if (isPrimeF == &nbrIsPrime) {
				/* nbrIsPrime() doesn't count divisors. */
            printf ("%.5f seconds (%s%s%s)\n",
                    bmwElapsed (&clock), operation,
                    (subop == NULL) ? "" : ", ",
                    (subop == NULL) ? "" : subop) ;
        } else {
				/* Alternate functions count divisors */
            printf ("%.5f seconds (%s%s%s) %"PRIuSIZE_T" divisors\n",
                    bmwElapsed (&clock), operation,
                    (subop == NULL) ? "" : ", ",
                    (subop == NULL) ? "" : subop,
                    gNumDivisors) ;
        }
#endif
    }

    exit (0) ;

}

/*******************************************************************************

    isPrimeOdds() - checks if a number is a prime number.  The function,
        adapted from nbrIsPrime() in "ligpl/nbr_util.c", steps through all
        odd divisors between 5 and the square root of the input argument.
        The simple algorithm provides a baseline benchmark against which
        optimized functions such as nbrIsPrime() (which skips odd divisors
        that are multiples of 3) can be compared.

*******************************************************************************/


static  bool  isPrimeOdds (

#    if PROTOTYPES
        prime_t  number,
        void  *config)
#    else
        number, config)

        prime_t  number ;
        void  *config ;
#    endif

{    /* Local variables. */
    prime_t  divisor, root ;



    LGI "(isPrimeOdds) # = %"PRIuPRIME_T"  %%2 = %"PRIuPRIME_T"\n", number, number % 2) ;

    if (number < 2)  return (false) ;	/* 0 and 1 are NOT prime. */
    if (number < 4)  return (true) ;	/* 2 and 3 are prime. */
    if ((number % 2) == 0)
        return (false) ;		/* Even numbers > 2 are NOT prime. */

/* Check for possible divisors.  The "divisor <= (number / divisor)" test
   is similar to checking 3 .. sqrt(N) as possible divisors, but avoids
   the need for linking to the math library.  Note that the alternate form,
   "(divisor * divisor) < number", introduces the possibility of overflow;
   i.e., when "(divisor * divisor) > PRIME_T_MAX". */

    root = nbrUnsignedSQRT (number, NULL) ;
    LGI "(isPrimeOdds) # = %"PRIuPRIME_T"  root %"PRIuPRIME_T"\n", number, root) ;

    for (divisor = 3 ;  divisor <= (number / divisor) ;  divisor += 2) {
        gNumDivisors++ ;
        if ((number % divisor) == 0)  return (false) ;	/* NOT prime. */
    }

    LGI "(isPrimeOdds) %"PRIuPRIME_T" is prime.\n", number) ;

    return (true) ;		/* No divisors found--number is prime! */

}

/*******************************************************************************

    isPrimeOnes() - checks if a number is a prime number.  The function
        tests *all* divisors, incrementing by one from 2, 3, 4, 5, 6, ...
        up to the square root of the input argument.

*******************************************************************************/


static  bool  isPrimeOnes (

#    if PROTOTYPES
        prime_t  number,
        void  *config)
#    else
        number, config)

        prime_t  number ;
        void  *config ;
#    endif

{    /* Local variables. */
    prime_t  divisor, root ;



    LGI "(isPrimeOnes) # = %"PRIuPRIME_T"\n", number) ;

    if (number < 2)  return (false) ;	/* 0 and 1 are NOT prime. */
    if (number < 4)  return (true) ;	/* 2 and 3 are prime. */

/* Check for possible divisors.  The "divisor <= (number / divisor)" test
   is similar to checking 4 .. sqrt(N) as possible divisors, but avoids
   the need for linking to the math library.  Note that the alternate form,
   "(divisor * divisor) < number", introduces the possibility of overflow;
   i.e., when "(divisor * divisor) > PRIME_T_MAX". */

    root = nbrUnsignedSQRT (number, NULL) ;
    LGI "(isPrimeOnes) # = %"PRIuPRIME_T"  root %"PRIuPRIME_T"\n", number, root) ;

    for (divisor = 4 ;  divisor <= (number / divisor) ;  divisor++) {
        gNumDivisors++ ;
        if ((number % divisor) == 0)  return (false) ;	/* NOT prime. */
    }

    LGI "(isPrimeOnes) %"PRIuPRIME_T" is prime.\n", number) ;

    return (true) ;		/* No divisors found--number is prime! */

}

/*!*****************************************************************************

Procedure:

    nbrIsPrimeSkip23 ()

    Check If a Number is a Prime Number.


Purpose:

    Function isPrimeSkip23() returns true if a given number is a prime number
    and false otherwise.


    Invocation:

        isPrime = isPrimeSkip23 (number) ;

    where

        <number>	- I
            is the number to be checked.
        <isPrime>	- O
            returns true if the number is a prime number and false if not.

*******************************************************************************/


bool  isPrimeSkip23 (

#    if PROTOTYPES
        prime_t  number,
        void  *config)
#    else
        number, config)

        prime_t  number ;
        void  *config ;
#    endif

{    /* Local variables. */
    prime_t  divisor, root ;



    LGI "(isPrimeSkip23) # = %"PRIuPRIME_T"  %%2 = %"PRIuPRIME_T"  %%3 = %"PRIuPRIME_T"\n",
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
   "(divisor * divisor) < number", introduces the possibility of overflow;
   i.e., when "(divisor * divisor) > PRIME_T_MAX". */

    root = nbrUnsignedSQRT (number, NULL) ;
    LGI "(isPrimeSkip23) # = %"PRIuPRIME_T"  root %"PRIuPRIME_T"\n", number, root) ;

    for (divisor = 5 ;  divisor <= (number / divisor) ;  divisor += 4) {
        gNumDivisors++ ;
        if ((number % divisor) == 0) {
            return (false) ;			/* NOT prime. */
        }
        divisor += 2 ;
        gNumDivisors++ ;
        if ((number % divisor) == 0) {
            return (false) ;			/* NOT prime. */
        }
    }

    LGI "(isPrimeSkip23) %"PRIuPRIME_T" is prime.\n", number) ;

    return (true) ;		/* No divisors found--number is prime! */

}

/*!*****************************************************************************

Procedure:

    isPrimeDiffs ()

    Check If a Number is a Prime Number.


Purpose:

    Function isPrimeDiffs() returns true if a given number is a prime
    number and false otherwise.


    Invocation:

        isPrime = isPrimeDiffs (number) ;

    where

        <number>	- I
            is the number to be checked.
        <isPrime>	- O
            returns true if the number is a prime number and false if not.

*******************************************************************************/


static  bool  isPrimeDiffs (

#    if PROTOTYPES
        prime_t  number,
        void  *config)
#    else
        number, config)

        prime_t  number ;
        void  *config ;
#    endif

{    /* Local variables. */
    size_t  i ;
    prime_t  divisor = 0, root ;
    prime_t  skipThroughPrime = gSkipThroughPrime ;
    const  unsigned  char  *differences = gSequence ;

	/* Early Primes - This array contains skip-through primes whose
	   sequence length can be represented in unsigned long integers:

	       STPs 2..29 plus start divisor 31 (32-bit unsigned long)
	       STPs 2..53 plus start divisor 59 (64-bit unsigned long)
	       STPs 2..101 plus start divisor 103 (128-bit unsigned long)

	   (The start divisor for skip-through prime earlyPrimes[i] is
	   simply the next prime number, earlyPrimes[i+1].)  All this is
	   for completeness' sake because the higher skip-through primes
	   are not realistically useful; the sequence length for 23, for
	   example, is over 36 million. */

    static  const  prime_t  earlyPrimes[]  OCD ("nbr_util")  = {
        2,  3,  5,  7, 11, 13, 17, 19, 23,  29,		/* 32-bit STPs */
       31, 37, 41, 43, 47, 53,				/* 64-bit STPs */
       59, 61, 67, 71, 73, 79, 83, 89, 97, 101,		/* 128-bit STPs */
      103, PRIME_T_MAX } ;				/* 101's start divisor */




    if (differences == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(isPrimeDiffs) No differences defined for skip-through prime %"PRIuPRIME_T".\n",
            skipThroughPrime) ;
        return (false) ;
    }

    if (number < 2)  return (false) ;	/* 0 and 1 are NOT prime. */
    if (number < 4)  return (true) ;	/* 2 and 3 are prime. */

/* Short-circuit test for the two most common divisors: (i) even numbers > 2
   are NOT prime; (ii) nor are odd numbers > 3 and divisible by 3.  Yes, the
   failed test is repeated in the for-loop immediately below for the purpose
   of getting the start divisor. */

    if (((number % 2) == 0) || ((number % 3) == 0))  return (false) ;

/* Check if the number is equal to or evenly divisible by the prime numbers
   less than or equal to the skip-through prime.  For example, if the
   skip-through prime is 7, then check (i) if the number is equal to 2,
   3, 5, or 7, and (ii) if the number is divisible by 2, 3, 5, or 7. */

    for (i = 0 ;  earlyPrimes[i] <= skipThroughPrime ;  i++) {
        if (number == earlyPrimes[i])  return (true) ;
        if ((number % earlyPrimes[i]) == 0)  return (false) ;
        divisor = earlyPrimes[i+1] ;		/* Eventual start divisor. */
    }

/* Check for possible divisors.  The "divisor <= (number / divisor)" test
   is similar to checking 5 .. sqrt(N) as possible divisors, but avoids
   the need for linking to the math library.  Note that the alternate form,
   "(divisor * divisor) < number", introduces the possibility of overflow;
   i.e., when "(divisor * divisor) > PRIME_T_MAX". */

    root = nbrUnsignedSQRT (number, NULL) ;
    LGI "(isPrimeDiffs) # = %"PRIuPRIME_T"  root %"PRIuPRIME_T"\n", number, root) ;

    while (divisor <= (number / divisor)) {

        for (i = 0 ;  differences[i] > 0 ;  i++) {
            gNumDivisors++ ;
            LGI "(isPrimeDiffs) [%"PRIuSIZE_T"] %"PRIuPRIME_T" / %"PRIuPRIME_T" = %"PRIuPRIME_T"    %% %"PRIuPRIME_T" = %"PRIuPRIME_T"\n",
                i, number, divisor, number / divisor,
                divisor, number % divisor) ;
            if (((number % divisor) == 0) &&	/* Evenly divisible by divisor? */
                (divisor < number)) {		/* Not dividing number by number? */
                return (false) ;		/* NOT prime. */
            }
            divisor += differences[i] ;
            if (divisor < root)  continue ;
            if (divisor > (number / divisor))  break ;
        }

    }

    LGI "(isPrimeDiffs) %"PRIuPRIME_T" is prime.\n", number) ;

    return (true) ;		/* No divisors found--number is prime! */

}
