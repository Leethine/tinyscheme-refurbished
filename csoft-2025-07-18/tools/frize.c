/* $Id: frize.c,v 1.5 2021/09/16 04:02:14 alex Exp alex $ */
/*******************************************************************************

Program:

    frize.c

    Integer FactoRIZation


Author:    Alex Measday


Purpose:

    FRIZE determines the prime factors of a number, a process called
    "prime factorization", no less.

        FRIZE was not my first choice for this program's name, but I
        couldn't think of a second choice.  It is pronounced "Fries",
        as in "Freedom Fries", a term that replaced "French Fries"
        in the U.S. House of Representatives' cafeterias in 2003.
        GOP congressman Bob Ney pushed this change; as far as I know,
        he expressed no opinion on "French Kissing".  It is rumoured
        that, when he heard someone call a French person a
        "cheese-eating surrender monkey" (thank you, Groundskeeper
        Willie!), Ney said, "Here, hold my beer."  GOP politicians
        are very competitive and will say things like, "I'm more of
        a surrender monkey than a Frenchmen any day of the week and
        twice on Sunday", and "I'm the limbo champion when it comes
        to low IQs."

    For example, the prime factors of 126 are:

                               2 * 3 * 3 * 7

    A given prime factor can appear more than once in a number's list
    of factors.  Composite numbers have more than one factor.  Prime
    numbers have a single factor -- themselves.

    I got interested in prime factorization after accidentally coming
    across Wikipedia's "Trial Division" page:

        https://en.wikipedia.org/wiki/Trial_division

    and its generic C++ function for generating factors.  There was a single
    phrase, "without squaring f" (the factor), in the introductory sentence
    and a single incorrect comment in the code.  There was no explanation
    of the algorithm, no explanation of the template parameters and, even
    worse, two of the most important variables had meaningless names.  My
    initial interest in the C++ function was if it suffered from possible
    overflows (which had been of concern in my prime number functions),
    but I had to figure out what the code was doing before I started
    investigating overflows.

    Once I understood the C++ function's algorithm, I began to develop
    optimizations and also a way to avoid overflows (which are possible).
    FRIZE tests and measures the performance of the following prime
    factorization functions:

        "diff" - is a readable, commented C version of Wikipedia's C++
            function, which also includes checks for overflow.  Both
            of them are distinguished by the keeping of running values
            of (i) squares and (ii) the differences between squares.
            The square of the next odd factor to be tested is obtained
            by simply adding the difference from the last square.
            Wikipedia's "without squaring f" comment presumably means
            that the C++ function doesn't multiply the factor by itself,
            "f*f". However, the function still computes the square of
            the factor, albeit without a direct multiplication, thus
            resulting in the possibility of overflow.

        "square" - is a slimmed-down version of "diff" that computes
            the differences between squares on the fly.  The difference
            between the squares of one odd factor, f, and the next, f+2,
            is always 4f+4.  The updating of the differences by an
            addition operation in "diff" is replaced by a multiplication
            and an addition.  An optimizing compiler will convert the
            multiplication by 4 into a 2-bit left shift (as GCC does).

        "root" - takes a completely different approach than "diff" and
            "square".  In simplified terms, the latter two functions
            limit the number of factors tested by comparing the square
            of the next factor to the number being factored.  As a
            result, these functions may iterate on factors past the
            square root of the number being factored.  For very large
            numbers, doing so can result in the square overflowing.
            The "root" function eliminates the possibility of overflow
            by computing the fast integer square root of the number to
            be factored and using that root as the limit for factors.
            The square of a factor is not needed and is not computed.

    After coding and testing the functions above, I tried applying the
    common "6k +/- 1" prime number optimization to these functions;
    i.e., don't test odd factors divisible by 3.  The functions only
    test 2 out of every 3 odd factors, which reduces the time to
    factor a number by a third.

        "square3" - the "square" method with the 6k +/- 1 optimization.

        "root3" - the "root" method with the 6k +/- 1 optimization.

    The "root3" algorithm combines speed with no overflows, so it became
    the nbrFactorsOf() function in my NBR_UTIL package.


    Invocation:

        % frize [-debug] [-help]
                [-after <count>] [-early <count>]
                [-mask <width>] [-overflows]
                [-progress <divisor>]
                [-repeat <count>] [-time]
                <number>[,diff|root|root3|square|square3]

    where:

        "-debug"
            turns debug on.
        "-help"
            prints out a help message.
        "-after <count>"
            triggers something happening after the specified number of factors
            have been tested.  The program must be modified to compare this
            count to the number of tests and to do something when the count
            is reached.  Currently, it is only referenced and used in the
            "diff" algorithm.  To factor a very large 64-bit prime, it is
            necessary to test over 2 billion potential factors.  I needed
            to see the values of the "diff" function's internal variables
            for the last few tests before the number is found to be prime,
            so, when the after count is encountered, the "diff" function
            enables debug ("nbr_util_debug" is set to 1).
        "-early <count>"
            instructs the algorithms to test the first <count> prime numbers
            as possible factors before falling back on the laborious testing
            of all odd factors or all odd factors not divisible by 3.  FRIZE
            has a built-in table of the first 6,542 primes.  By default, all
            of these primes (or as many as necessary) are tested.  Using a
            lower count can be useful in performance testing.  In particular,
            the nbrFactorsOf() function in the LIBGPL library has a smaller
            table of the first 512 prime numbers.  To compare the performance
            of that function to the performance of the algorithms in FRIZE,
            "-early 512" should be specified on the command line.
        "-mask <width>"
            specifies the bit width of a mask to be applied in the algorithms
            that are subject to overflow ("diff", "square", and "square3"),
            thus providing the capability to cause artificial overflows.
            The calculations in the factoring functions are made using the
            "factor_t" type, defined in "nbr_util.h".  The width of the mask
            must be less than the bit width of the positive portion of the
            "factor_t" type.  The mask zeroes out all but the specified
            number of least significant bits in the results of calculations.
            For example, if "factor_t" is 64 bits wide, unsigned 32-bit
            integers can be simulated with a mask width of 32.  Also see
            the "-overflows" option below.
        "-overflows"
            allows overflows to occur in the algorithms that are subject
            to overflow ("diff", "square", and "square3").  By default,
            when one of these functions detects an overflow, an error
            message is displayed and the function returns immediately.
            With the "-overflows" option, a debug message is logged if
            "-debug" is on and the function continues processing with
            the overflowed value.  Note that continuing with the bad
            values likely will cause the function to test orders of
            magnitude more factors, so be prepared to wait.  Also see
            the "-mask <width>" option to see how to cause overflows
            with smaller values.
        "-progress <divisor>"
            instructs the program to print progress messages out to stderr
            as a number is being factored.  The factoring functions keep
            count of the number of tests of possible factors.  When the
            number of tests is evenly divisible by <divisor>, a progress
            message is output.  For example, when factoring the largest
            32-bit prime with the "diff" algorithm, with a mask of 32 bits,
            and with overflows allowed, I had the function output progress
            messages every 100 million tests.  (Because of the overflows,
            the function tests over 2 billion factors.)  These messages
            are only generated if an algorithm is specified on the command
            line after the number to be factored; if not, the program calls
            the LIBGPL function nbrGetFactors() which doesn't support this
            capability.  (The command-line algorithms are implemented in
            this source file; if you need progress messages for the LIBGPL
            function, use its equivalent algorithm, "root3".)
        "-repeat <count>"
            repeats the factorization the specified number of times.  This
            is useful when a single factorization of the input number is too
            quick to provide a meaningful time.  Repeating the factorization
            thousands or tens of thousands of times allow more realistic
            timing comparisons between algorithms, for example.
        "-time"
            measures and displays the time it takes to perform factorization
            (or <count> iterations of factorization if "-repeat <count>" is
            also given on the command line).  Almost all output is suppressed
            to keep I/O from affecting the timing.
        "<number>[,diff|root|root3|square|square3]"
            generates the prime factors of the number.  The optional
            argument after a comma (no spaces!) specifies an alternate,
            prime factorization function (all of which are compiled into
            this program):
                "diff" - uses an optimized version (I think!) of the C++
                    TrialDivision function on Wikipedia.  The function
                    keeps track of the factor, the square of the factor,
                    and the differences between the squares of consecutive
                    odd factors.
                "square" - is a slimmed-down version of the "diff" function
                    that does not keep track of the differences between
                    squares.  The square of the next odd factor following
                    the old factor, f, is f^2 + 4f + 4.  This function
                    simply adds 4f + 4 to the current square to get the
                    next square; it is not necessary to keep a running
                    value for the difference.  And an optimizing compiler
                    can replace 4f with f<<2.
                "square3" (or "s3") - is the "square" method modified to
                    skip odd factors divisible by 3.  Testing only 2 out
                    of every 3 odd factors reduces the time to factor a
                    number by a third.
                "root" - uses a fast integer square root function to get
                    the upper bound for the factor, thus eliminating the
                    possibility of overflows (and also the need to keep
                    track of the square of the factor).
                "root3" (or "r3") - is the "root" method modified to
                    skip odd factors divisible by 3.  Testing only 2 out
                    of every 3 odd factors reduces the time to factor a
                    number by a third.
            I used this capability for benchmarking the different algorithms.
            You can abbreviate the names to any length, so I usually just
            specify one letter; e.g., "frize 126,s" will generate the prime
            factors of 126 using the "square" function.  The skip-3 functions,
            "square3" and "root3", can also be abbreviated to "s3" and "r3",
            respectively.  If the optional argument is not present, the
            default function is LIBGPL's "root3" function, nbrFactorsOf().

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


#define  NBR_EARLY_PRIMES  6542		/* First N primes. */
static  const  uint16_t  earlyPrimes[] = {
#if NBR_EARLY_PRIMES == 0
#elif NBR_EARLY_PRIMES == 1
    2,
#elif NBR_EARLY_PRIMES == 2
    2, 3,
#else
#    include  "nbr_early_primes.txt"
#endif
    0		/* End-of-list marker. */
} ;


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  nbr_util_debug


/*******************************************************************************
    File-Global variables used by the main program to communicate with the
    factoring functions:

    Overflow Mask - is used by the factoring functions subject to overflow
        ("diff", "square", and "square3") to simulate narrower bit widths
        in order to induce overflows.  The mask zeroes out the most
        significants bits of the results of calculations, preserving the
        least significant number of bits specified by the "-mask <width>"
        command-line option.

    Allow Overflows - is set by the "-overflows" command-line option.  If
        this boolean variable is false (the default), a detected overflow
        will cause a factoring function to exit immediately because of the
        error.  If "-overflows" was specified, this variable will be true
        and the function will ignore the error and continue processing
        using the overflowed value.

    numEarlyPrimes - limits the number of early primes tested by the
        factoring functions that do this.  The default number is 6,542,
        the entire list of primes less than 65,535.  The "-early <count>"
        command-line option can be used to specify a lower number.
        For example, zero allows the sensible comparison of performance
        between functions that test the early primes and functions that
        don't.

    numTests - counts how many factors have been tested by a factoring
        function.  If a factor appears multiple times, it is counted as
        multiple tests; e.g., 2 x 2 x 2 x 3 x 3 counts as 5 tests.  The
        functions set this count to zero before factoring the input
        number.  If the main program is repeatedly factoring the same
        number for benchmarking purposes, it can print out the test
        count knowing that it is from the last function call only (and
        that it is not the sum total of all tests from the repeated
        calls.)

    afterTests - triggers something in the functions after the specified
        number of tests have been performed.  Right now, I use it simply
        to enable debug messages when a long-running factorization of a
        prime number nears completion.

    progressDiv - controls how often a factoring function writes progress
        messages to stderr.  It's value is specified by the "-progress <div>"
        command line.  On each loop iteration, the number of factors tested
        is divided by this value; if the remainder is zero, a message is
        printed.  For example, when factoring the largest 32-bit prime with
        the "diff" algorithm and with overflows allowed, I had the function
        output progress messages every 100 million tests.  (Because of the
        overflows, the function tests over 2 billion factors.)

    numRoots - counts the number of times the fast integer square root
        of the target number is calculated.

    numSquareOverflows -
    numDiffOverflows - count the number of overflows in the functions
        vulnerable to overflow.  Function getFactorsDiff() uses and
        updates both variables; getFactorsSquare() and getFactorsSquare3()
        only use and update the number of square overflows.

*******************************************************************************/

		/* Should provide a more robust definition in "nbr_util.h". */
#define  FACTOR_T_POSITIVE_BITS  (sizeof (factor_t) * 8)

static  factor_t  overflowMask ;
static  bool  allowOverflows ;
static  size_t  numEarlyPrimes ;
static  size_t  numTests ;
static  size_t  afterTests ;
static  size_t  progressDiv ;		/* Zero means no progress messages. */
static  size_t  numRoots ;
static  size_t  numSquareOverflows ;
static  size_t  numDiffOverflows ;


/*******************************************************************************
    Private Functions
*******************************************************************************/

typedef  size_t  (*FactorsFunc) P_((factor_t number,
                                    size_t maxFactors,
                                    factor_t *factors)) ;

static  size_t  getFactorsDiff (
#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors
#    endif
    ) ;

static  size_t  getFactorsRoot (
#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors
#    endif
    ) ;

static  size_t  getFactorsRootSkip3 (
#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors
#    endif
    ) ;

static  size_t  getFactorsRootSkip3Root (
#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors
#    endif
    ) ;

static  size_t  getFactorsSquare (
#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors
#    endif
    ) ;

static  size_t  getFactorsSquareSkip3 (
#    if PROTOTYPES
        factor_t  number,
        size_t  maxFactors,
        factor_t  *factors
#    endif
    ) ;

/*******************************************************************************
    FRIZE's Main Program.
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
    const  char  *algo ;
#define  MAX_FACTORS  256
    factor_t  check, factors[MAX_FACTORS], number ;
    int  errflg, option ;
    FactorsFunc  getFactorsF ;
    OptContext  scan ;
    size_t  bitWidth, i, numFactors, repeat ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}",
        "{after:}", "{early:}", "{mask:}",
        "{overflows}", "{progress:}",
        "{repeat:}", "{time}", NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    allowOverflows = false ;	/* Terminate factoring on first overflow. */
    numEarlyPrimes = (sizeof earlyPrimes / sizeof (uint16_t)) - 1 ;
    afterTests = SIZE_MAX ;	/* No trigger. */
    progressDiv = 0 ;		/* No progress messages. */
    numSquareOverflows = 0 ;
    numDiffOverflows = 0 ;

    help = false ;
    benchmark = false ;
    bitWidth = 0 ;
    numFactors = 0 ;
    repeat = 1 ;

    getFactorsF = nbrFactorsOf ;
    algo = NULL ;

    number = 0 ;

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
        case 3:				/* "-after <count>" */
            afterTests = strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 4:				/* "-early <count>" */
            numEarlyPrimes = strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 5:				/* "-mask <width>" */
            bitWidth = (size_t) strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            if (bitWidth >= FACTOR_T_POSITIVE_BITS)  errflg++ ;
            break ;
        case 6:				/* "-overflows" */
            allowOverflows = true ;
            break ;
        case 7:				/* "-progress <divisor>" */
            progressDiv = (size_t) strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 8:				/* "-repeat <count>" */
            repeat = strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 9:				/* "-time" */
            benchmark = true ;
            break ;
        case NONOPT:			/* "<number>[,diff|root|root3|root3root|square|square3]" */
            if (algo != NULL) {		/* #/algorithm already specified? */
               errflg++ ;  break ;
            }
            getFactorsF = nbrFactorsOf ;	/* Default: libgpl function. */
            algo = "root-skip3/nbr" ;
            number = (factor_t) strtoul (argument, &s, 0) ;
            if (*s == ',') {
                s++ ;
                if (strMatch (s, "diff", true)) {
                    getFactorsF = &getFactorsDiff ;
                    algo = "diff" ;
                } else if (strMatch (s, "root", true)) {
                    getFactorsF = &getFactorsRoot ;
                    algo = "root" ;
                } else if (strMatch (s, "root3", true) ||
                           (strcasecmp (s, "r3") == 0)) {
                    getFactorsF = &getFactorsRootSkip3 ;
                    algo = "root-skip3" ;
                } else if (strMatch (s, "root3root", true) ||
                           (strcasecmp (s, "r3r") == 0)) {
                    getFactorsF = &getFactorsRootSkip3Root ;
                    algo = "root-skip3-root" ;
                } else if (strMatch (s, "square", true)) {
                    getFactorsF = &getFactorsSquare ;
                    algo = "square" ;
                } else if (strMatch (s, "square3", true) ||
                           (strcasecmp (s, "s3") == 0)) {
                    getFactorsF = &getFactorsSquareSkip3 ;
                    algo = "square-skip3" ;
                } else {
                    errflg++ ;
                }
                if (!errflg)  s += strlen (s) ;
            }
            if (*s != '\0')  errflg++ ;
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

    if (algo == NULL)
        errflg++ ;	/* No #/algorithm specified on the command line. */

    if (errflg || help) {
        fprintf (stderr, "Usage:  frize [-debug] [-help]\n") ;
        fprintf (stderr, "              [-after <count>] [-early <count>]\n") ;
        fprintf (stderr, "              [-mask <width>] [-overflows]\n") ;
        fprintf (stderr, "              [-progress <divisor>]\n") ;
        fprintf (stderr, "              [-repeat <count>] [-time]\n") ;
        fprintf (stderr, "              <number>[,diff|root|root3|root3root|square|square3]\n") ;
    }

    if (errflg && !help) {
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Enter \"frize -help\" for detailed help.\n") ;
    }

    if (help) {
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Notes:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -after <count>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        triggers something happening after the specified number of factor\n") ;
        fprintf (stderr, "        have been tested.  The program must be modified to compare this\n") ;
        fprintf (stderr, "        count to the number of tests and to do something when the count\n") ;
        fprintf (stderr, "        is reached.  Currently, it is only referenced and used in the\n") ;
        fprintf (stderr, "        \"diff\" algorithm.  To factor a very large 64-bit prime, it is\n") ;
        fprintf (stderr, "        necessary to test over 2 billion potential factors.  I needed\n") ;
        fprintf (stderr, "        to see the values of the \"diff\" function's internal variables\n") ;
        fprintf (stderr, "        for the last few tests before the number is found to be prime,\n") ;
        fprintf (stderr, "        so, when the after count is encountered, the \"diff\" function\n") ;
        fprintf (stderr, "        enables debug (\"nbr_util_debug\" is set to 1).\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -early <count>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        instructs the algorithms to test the first <count> prime numbers\n") ;
        fprintf (stderr, "        as possible factors before falling back on the laborious testing\n") ;
        fprintf (stderr, "        of all odd factors or all odd factors not divisible by 3.  FRIZE\n") ;
        fprintf (stderr, "        has a built-in table of the first 6,542 primes.  By default, all\n") ;
        fprintf (stderr, "        of these primes (or as many as necessary) are tested.  Using a\n") ;
        fprintf (stderr, "        lower count can be useful in performance testing.  In particular,\n") ;
        fprintf (stderr, "        the nbrFactorsOf() function in the LIBGPL library has a smaller\n") ;
        fprintf (stderr, "        table of the first 512 prime numbers.  To compare the performance\n") ;
        fprintf (stderr, "        of that function to the performance of the algorithms in FRIZE,\n") ;
        fprintf (stderr, "        \"-early 512\" should be specified on the command line.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -mask <width>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        specifies the bit width of a mask to be applied in the algorithms\n") ;
        fprintf (stderr, "        that are subject to overflow (\"diff\", \"square\", and \"square3\"),\n") ;
        fprintf (stderr, "        thus providing the capability to cause artificial overflows.\n") ;
        fprintf (stderr, "        The calculations in the factoring functions are made using the\n") ;
        fprintf (stderr, "        \"factor_t\" type, defined in \"nbr_util.h\".  The width of the mask\n") ;
        fprintf (stderr, "        must be less than the bit width of the positive portion of the\n") ;
        fprintf (stderr, "        \"factor_t\" type.  The mask zeroes out all but the specified\n") ;
        fprintf (stderr, "        number of least significant bits in the results of calculations.\n") ;
        fprintf (stderr, "        For example, if \"factor_t\" is 64 bits wide, unsigned 32-bit\n") ;
        fprintf (stderr, "        integers can be simulated with a mask width of 32.  Also see\n") ;
        fprintf (stderr, "        the \"-overflows\" option below.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -overflows\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        allows overflows to occur in the algorithms that are subject\n") ;
        fprintf (stderr, "        to overflow (\"diff\", \"square\", and \"square3\").  By default,\n") ;
        fprintf (stderr, "        when one of these functions detects an overflow, an error\n") ;
        fprintf (stderr, "        message is displayed and the function returns immediately.\n") ;
        fprintf (stderr, "        With the \"-overflows\" option, a debug message is logged if\n") ;
        fprintf (stderr, "        \"-debug\" is on and the function continues processing with\n") ;
        fprintf (stderr, "        the overflowed value.  Note that continuing with the bad\n") ;
        fprintf (stderr, "        values likely will cause the function to test orders of\n") ;
        fprintf (stderr, "        magnitude more factors, so be prepared to wait.  Also see\n") ;
        fprintf (stderr, "        the \"-mask <width>\" option to see how to cause overflows\n") ;
        fprintf (stderr, "        with smaller values.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -progress <divisor>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        instructs the program to print progress messages out to stderr\n") ;
        fprintf (stderr, "        as a number is being factored.  The factoring functions keep\n") ;
        fprintf (stderr, "        count of the number of tests of possible factors.  When the\n") ;
        fprintf (stderr, "        number of tests is evenly divisible by <divisor>, a progress\n") ;
        fprintf (stderr, "        message is output.  For example, when factoring the largest\n") ;
        fprintf (stderr, "        32-bit prime with the \"diff\" algorithm, with a mask of 32 bits,\n") ;
        fprintf (stderr, "        and with overflows allowed, I had the function output progress\n") ;
        fprintf (stderr, "        messages every 100 million tests.  (Because of the overflows,\n") ;
        fprintf (stderr, "        the function tests over 2 billion factors.)  These messages\n") ;
        fprintf (stderr, "        are only generated if an algorithm is specified on the command\n") ;
        fprintf (stderr, "        line after the number to be factored; if not, the program calls\n") ;
        fprintf (stderr, "        the LIBGPL function nbrGetFactors() which doesn't support this\n") ;
        fprintf (stderr, "        capability.  (The command-line algorithms are implemented in\n") ;
        fprintf (stderr, "        this source file; if you need progress messages for the LIBGPL\n") ;
        fprintf (stderr, "        function, use its equivalent algorithm, \"root3\".)\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -repeat <count>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        repeats the factorization the specified number of times.  This\n") ;
        fprintf (stderr, "        is useful when a single factorization of the input number is too\n") ;
        fprintf (stderr, "        quick to provide a meaningful time.  Repeating the factorization\n") ;
        fprintf (stderr, "        thousands or tens of thousands of times allow more realistic\n") ;
        fprintf (stderr, "        timing comparisons between algorithms, for example.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    -time\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        measures and displays the time it takes to perform factorization\n") ;
        fprintf (stderr, "        (or <count> iterations of factorization if \"-repeat <count>\" is\n") ;
        fprintf (stderr, "        also given on the command line).  Almost all output is suppressed\n") ;
        fprintf (stderr, "        to keep I/O from affecting the timing.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    <number>[,diff|root|root3|square|square3]\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        generates the prime factors of the number.  The optional\n") ;
        fprintf (stderr, "        argument after a comma (no spaces!) specifies an alternate,\n") ;
        fprintf (stderr, "        prime factorization function (all of which are compiled into\n") ;
        fprintf (stderr, "        this program):\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "            \"diff\" - uses an optimized version (I think!) of the C++\n") ;
        fprintf (stderr, "                TrialDivision function on Wikipedia.  The function keeps\n") ;
        fprintf (stderr, "                track of the factor, the square of the factor, and the\n") ;
        fprintf (stderr, "                differences between the squares of consecutive odd factors.\n") ;
        fprintf (stderr, "            \"square\" - is a slimmed-down version of the \"diff\" function\n") ;
        fprintf (stderr, "                that does not keep track of the differences between\n") ;
        fprintf (stderr, "                squares.  The square of the next odd factor following\n") ;
        fprintf (stderr, "                the old factor, f, is f^2 + 4f + 4.  This function\n") ;
        fprintf (stderr, "                simply adds 4f + 4 to the current square to get the\n") ;
        fprintf (stderr, "                next square; it is not necessary to keep a running\n") ;
        fprintf (stderr, "                value for the difference.  And an optimizing compiler\n") ;
        fprintf (stderr, "                can replace 4f with f<<2.\n") ;
        fprintf (stderr, "            \"square3\" (or \"s3\") - is the \"square\" method modified to\n") ;
        fprintf (stderr, "                skip odd factors divisible by 3.  Testing only 2 out of\n") ;
        fprintf (stderr, "                every 3 odd factors reduces the time to factor a number\n") ;
        fprintf (stderr, "                by a third.\n") ;
        fprintf (stderr, "            \"root\" - uses a fast integer square root function to get\n") ;
        fprintf (stderr, "                the upper bound for the factor, thus eliminating the\n") ;
        fprintf (stderr, "                possibility of overflows (and also the need to keep\n") ;
        fprintf (stderr, "                track of the square of the factor).\n") ;
        fprintf (stderr, "            \"root3\" (or \"r3\") - is the \"root\" method modified to\n") ;
        fprintf (stderr, "                skip odd factors divisible by 3.  Testing only 2 out\n") ;
        fprintf (stderr, "                of every 3 odd factors reduces the time to factor a\n") ;
        fprintf (stderr, "                number by a third.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        You can abbreviate the names to any length, even one letter;\n") ;
        fprintf (stderr, "        for example, \"frize 126,s\" will generate the prime factors\n") ;
        fprintf (stderr, "        of 126 using the \"square\" function.  The skip-3 functions,\n") ;
        fprintf (stderr, "        \"square3\" and \"root3\", can also be abbreviated to \"s3\" and\n") ;
        fprintf (stderr, "        \"r3\", respectively.  If the optional argument is not present,\n") ;
        fprintf (stderr, "        the default function is LIBGPL's \"root3\" function, nbrFactorsOf().\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "References:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    Wikipedia: Trial Division\n") ;
        fprintf (stderr, "        https://en.wikipedia.org/wiki/Trial_division\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    Charles A. Measday, \"Fast Integer Square Roots\", 2018\n") ;
        fprintf (stderr, "        http://www.geonius.com/writing/other/cmoon.html\n") ;
    }

    if (errflg || help)  exit (EINVAL) ;

/*******************************************************************************
    Set the overflow mask.  If "-mask <width>" was not specified on the command
    line, the mask will preserve all the bits in the "factor_t" type.
*******************************************************************************/

    if (bitWidth == 0) {
        overflowMask = ~(FACTOR_T_C (0)) ;
    } else {
        overflowMask = 1 ;
        for (i = 1 ;  i < bitWidth ;  i++) {
            overflowMask = (overflowMask << 1) | 1 ;
        }
    }

    LGI "[frize] Overflow bit mask = 0x%"PRIXFACTOR_T"\n", overflowMask) ;


/*******************************************************************************
    Determine the prime factors of the input number.
*******************************************************************************/

    bmwStart (&clock) ;

    while (repeat-- > 0) {
        numFactors = getFactorsF (number, MAX_FACTORS, factors) ;
    }

    bmwStop (&clock) ;

    if (benchmark)  printf ("Elapsed:  %g seconds\n", bmwElapsed (&clock)) ;

    printf ("%s  %"PRIuSIZE_T" bits  Number: %"PRIuFACTOR_T"  0x%"PRIXFACTOR_T"  # of factors: %"PRIuSIZE_T"\n",
            algo, sizeof (factor_t) * 8, number, number, numFactors) ;
    for (i = 0, check = 1 ;  i < numFactors ;  i++) {
        printf ("%3"PRIuSIZE_T":\t%"PRIuFACTOR_T"\n", i, factors[i]) ;
        check *= factors[i] ;
    }
    printf ("Check: %"PRIuFACTOR_T"  0x%"PRIXFACTOR_T"\n", check, check) ;

    printf ("# of tests: %"PRIuSIZE_T"", numTests) ;
    if (strncmp (algo, "root", 4) == 0) {
        printf ("\t# of roots computed: %"PRIuSIZE_T"", numRoots) ;
    }
    if (allowOverflows) {
        if (strcmp (algo, "diff") == 0) {
            printf ("\n# of square overflows: %"PRIuSIZE_T"",
                    numSquareOverflows) ;
            printf ("\t# of diff overflows: %"PRIuSIZE_T"",
                    numDiffOverflows) ;
        } else if ((strcmp (algo, "square") == 0) ||
                   (strcmp (algo, "square-skip3") == 0)) {
            printf ("\t# of square overflows: %"PRIuSIZE_T"",
                    numSquareOverflows) ;
        }
    }
    printf ("\n") ;

    exit (0) ;

}

/*!*****************************************************************************

Procedure:

    getFactorsDiff ()

    Get the Factors of a Number (Differences and Squares).


Purpose:

    Function getFactorsDiff() computes and returns the prime factors of a
    number.  The function keeps track of the squares of the factors and the
    differences between the squares so that calculating the square of the
    next odd factor to be tested requires only addition operations.


    Invocation:

        numFactors = getFactorsDiff (number, maxFactors, &factors) ;

    where

        <number>	- I
            is the number to be factored.
        <maxFactors>	- I
            is the maximum number of factors the following array can hold.
        <factors>	- O
            is a caller-supplied array into which the factors will be stored.
        <numFactors>	- O
            returns the number of factors generated.  The list of factors
            does not include one, so a prime number will only have one
            factor: itself.  The factors for other numbers will begin with
            the smallest prime numbers that evenly divide the numbers.

*******************************************************************************/


static  size_t  getFactorsDiff (

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
    const  char  *prefix = "(getFactorsDiff) " ;
    factor_t  diff, factor, prevDiff, prevSquare, square ;
    size_t  i, numFactors ;



    numFactors = 0 ;
    numTests = 0 ;
    numSquareOverflows = 0 ;
    numDiffOverflows = 0 ;

    if (number < 2) {		/* 0 or 1?  No factors. */
        LGI "%s# = %"PRIuFACTOR_T"  # of factors = %"PRIuSIZE_T"  # of tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
        return (0) ;
    }

    if ((maxFactors < 1) || (factors == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "%sMaximum of 0 or NULL factors pointer: ", prefix) ;
        return (0) ;
    }

/*******************************************************************************
    Check for factors in the table of the first N prime numbers.
*******************************************************************************/

    factor = 1 ;

    for (i = 0 ;
         (i < numEarlyPrimes) && (earlyPrimes[i] != 0) && (number > 1) ;
         i++) {
        factor = earlyPrimes[i] ;
LGI "%"PRIuSIZE_T"  Testing early prime[%"PRIuSIZE_T"]  %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, i, factor, number) ;
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
LGI "%"PRIuSIZE_T"  Testing early prime[%"PRIuSIZE_T"]  %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, i, factor, number) ;
        }
        numTests++ ;
    }


/* If, for some reason, the table is empty or the factor 2 was not checked,
   then check factor 2.  Beginning with 3, the remaining factors tested will
   be odd. */

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


    factor += (factor > 2) ? 2 : 1 ;	/* Advance to next odd factor. */

/*******************************************************************************

    Check all the remaining odd factors not divisible by 3 up to the largest
    odd number whose square does not exceed the target number.  (The target
    number was/is the original input number, but it may have been reduced
    by the tests of the early primes.)

    When an actual factor is found, it is added to the list of the input
    number's factors.  Adding the factor to the list of factors found so
    far must be balanced by removing the factor from the target number;
    i.e., divide the target number by the factor.  Only the product of
    the remaining factors needs to and must be factored and its factors
    will all be equal to or greater than the factor just found.  (I say
    "must" because reducing the target number is necessary to detect
    multiple instances of the same factor.)

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

        (2) Pr has not been reduced to 1, but the square of the next factor
            to be tested exceeds Pr.  When this happens, Pr will be added
            as the final factor of the input number after the loop exits.
            This is always the case when factoring a prime number.

    At the top of the "while (square <= number)" loop below, the variables
    have the following values:

        prevSquare - is the square of the factor in the previous iteration;
            i.e., (f-2)^2, where f is the current factor.  This value is
            used to check for overflow when the square of the new factor
            is computed.

        prevDiff - is the difference from the previous iteration between
            the previous square and the square of the current factor; i.e.,
            f^2 - (f-2)^2 = 4f - 4, where f is the current factor.

        factor - is the odd number that will be tested as a factor in this
            iteration.

        square - is the square of factor.

        diff - is the difference between square and the square of the next
            odd number (i.e., factor + 2).

    To make this a little more clearer:

        f = 3
        square = 9			-- 3^2
        diff = 4f + 4			-- (3+2)^2 - 3^2 = 25 - 9 = 16
        prevDiff = prevSquare = 0

        while (square < n) {
            if (f is a factor) {
                ... Add f to the list of factors.
                ... Leave f, square, and diff alone because ...
                ... the current f must be tryed again.
            } else if (f is NOT a factor) {
                f = f + 2		-- Advance to next odd number.
                prevSquare = square
                square = square + diff
                prevDiff = diff
                diff = diff + 8		-- See DIFF_4FP4 below.
            }
        }

*******************************************************************************/

/* The difference between the squares of two consecutive odd factors
   is (f+2)^2 - f^2 = 4f + 4.  The difference between the differences
   is 4(f+2) + 4 - (4f + 4) = 4f + 8 + 4 -4f -4 = 8. */

#define  DIFF_4FP4  8

    square = factor * factor ;
    diff = 4*factor + 4 ;
    prevSquare = prevDiff = 0 ;

    while ((number > 1) &&		/* Until number is reduced to 1 ... */
           (square <= number)) {	/* ... or all factors are tested. */

        if (afterTests == numTests) {
            nbr_util_debug = 1 ;
            LGE "%sAfter Tests = %"PRIuSIZE_T"  # Tests = %"PRIuSIZE_T"\n",
                prefix, afterTests, numTests) ;
        }

LGI "%"PRIuSIZE_T"  Testing %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, factor, number) ;
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

        } else {			/* Not a factor. */

            factor += 2 ;		/* Advance to the next odd factor. */
            prevSquare = square ;
            square += diff ;
            square &= overflowMask ;
            prevDiff = diff ;
            diff += DIFF_4FP4 ;		/* Difference between new square ... */
            diff &= overflowMask ;	/* ... and the square that follows. */

            LGI "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
                prefix, number, numFactors, numTests) ;
            LGI "%sf=%"PRIuFACTOR_T" psq=%"PRIuFACTOR_T" sq=%"PRIuFACTOR_T" prevd=%"PRIuFACTOR_T" diff=%"PRIuFACTOR_T"\n",
                prefix, factor, prevSquare, square, prevDiff, diff) ;

/* Check for overflow of square and diff.  If overflow is detected, return
   immediately.  If you ignore square's overflow, it then becomes possible
   that diff will overflow (although much, much less frequently than square).
   The "-overflows" command-line option allows the function to ignore the
   overflow(s) and to continue procesing with bad values. */

            if (square < prevSquare) {
                numSquareOverflows++ ;
                if (allowOverflows) {
                    LGI "%sOVF/SQUARE test %"PRIuSIZE_T" prev=0x%"PRIXFACTOR_T" new=0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, prevSquare, square) ;
                } else {
                    SET_ERRNO (EOVERFLOW) ;
                    LGE "%sOverflow of square, test %"PRIuSIZE_T":  previous = 0x%"PRIXFACTOR_T"  new = 0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, prevSquare, square) ;
                    return (0) ;
                }
            }

            if (diff < prevDiff) {
                numDiffOverflows++ ;
                if (allowOverflows) {
                    LGI "%sOVF/DIFF test %"PRIuSIZE_T" f=%"PRIuFACTOR_T" prev=0x%"PRIXFACTOR_T" new=0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, factor, prevDiff, diff) ;
                } else {
                    SET_ERRNO (EOVERFLOW) ;
                    LGE "%sOverflow of diff, test %"PRIuSIZE_T", factor=%"PRIuFACTOR_T":  previous = 0x%"PRIXFACTOR_T"  new = 0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, factor, prevDiff, diff) ;
                    return (0) ;
                }
            }

        }

LGI "%"PRIuSIZE_T"  square %"PRIuFACTOR_T", diff %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, square, diff, number) ;
        numTests++ ;

        if ((progressDiv > 0) && ((numTests % progressDiv) == 0)) {
            LGE "t %"PRIuSIZE_T" f %"PRIuFACTOR_T" psq=%"PRIXFACTOR_T" sq=%"PRIXFACTOR_T" pd=%"PRIXFACTOR_T" d=%"PRIXFACTOR_T"\n",
                numTests, factor, prevSquare, square, prevDiff, diff) ;
        }

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.
*******************************************************************************/

    if (number > 1) {
        LGE "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
        if ((numFactors >= maxFactors) && (factors != NULL)) {
            SET_ERRNO (ENOMEM) ;
            LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                prefix, maxFactors) ;
            return (0) ;
        }
        if (factors != NULL)  factors[numFactors] = number ;
        numFactors++ ;
    }


    LGI "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
        prefix, number, numFactors, numTests) ;

    return (numFactors) ;

}

/*!*****************************************************************************

Procedure:

    getFactorsRoot ()

    Get the Factors of a Number (Square Root).


Purpose:

    Function getFactorsRoot() computes and returns the prime factors of
    a number.  The function uses the square root of the input number as
    the upper bound of factors to test, thus avoiding the possibility of
    overflow.


    Invocation:

        numFactors = getFactorsRoot (number, maxFactors, &factors) ;

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


static  size_t  getFactorsRoot (

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
    const  char  *prefix = "(getFactorsRoot) " ;
    factor_t  factor, root ;
    size_t  i, numFactors ;



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

    factor = 1 ;

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


/* If, for some reason, the table is empty or the factor 2 was not checked,
   then check factor 2.  Beginning with 3, the remaining factors tested will
   be odd. */

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


    factor += (factor > 2) ? 2 : 1 ;	/* Advance to next odd factor. */

/*******************************************************************************

    Check all remaining odd factors up to the largest odd number not
    exceeding the square root of the target number.  (The target number
    was/is the original input number, but it may have been reduced by
    the tests of the early primes.)

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
            square root of Pr is even.

            For example, take the number 42, the answer to many questions.
            Its factors are 2, 3, and 7.  2 and 3 are caught above and Pr
            is reduced from 42 to 21 to 7.  The next factor to be tested
            is 5 and the integer square root of 7 is 2.  The loop detects
            that 5 > 2 and doesn't execute a single iteration.  After the
            loop, Pr, 7, has not been reduced to 1, so 7 is added as the
            final factor of 42.

*******************************************************************************/

    if (number > 1) {
        root = (factor_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
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
            root = (factor_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
            numRoots++ ;
            LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
                prefix, number, root, factor) ;

        } else {			/* Not a factor. */

            factor += 2 ;

        }

        numTests++ ;

        if ((progressDiv > 0) && ((numTests % progressDiv) == 0)) {
            LGE "t %"PRIuSIZE_T" f %"PRIuFACTOR_T" r=%"PRIXFACTOR_T" n=%"PRIXFACTOR_T"\n",
                numTests, factor, root, number) ;
        }

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.  This can happen on both composite
    and prime numbers.
*******************************************************************************/

    if (number > 1) {
        LGE "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
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

    getFactorsRootSkip3 ()

    Get the Factors of a Number (Square Root, Skipping Factors Divisible by 3).


Purpose:

    Function getFactorsRootSkip3() computes and returns the prime factors
    of a number.  The function uses the square root of the input number
    as the upper bound of factors to test, thus avoiding the possibility
    of overflow that may occur in other algorithms.

    As an example, getFactorsRootSkip3() will return 4 prime factors for 126:

                               2 x 3 x 3 x 7


    Invocation:

        numFactors = getFactorsRootSkip3 (number, maxFactors, &factors) ;

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


size_t  getFactorsRootSkip3 (

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
    const  char  *prefix = "(getFactorsRootSkip3) " ;
    factor_t  factor, root ;
    size_t  i, numFactors ;
#define  FRIZE_FACTOR  1	/* Define as 1 in "frize" program. */
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

    factor = 1 ;

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
        root = (factor_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
        numRoots++ ;
        LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
            prefix, number, root, factor) ;
    } else {
        root = 0 ;
    }

    while ((number > 1) &&		/* Until number is reduced to 1 ... */
           (factor <= root)) {		/* ... or all factors are tested. */

LGI "%"PRIuSIZE_T"  Testing %"PRIuFACTOR_T", root %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, factor, root, number) ;
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
            root = (factor_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
            numRoots++ ;
            LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
                prefix, number, root, factor) ;

LGI "-- %"PRIuSIZE_T"  Testing %"PRIuFACTOR_T", root %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, factor, root, number) ;
        } else {			/* Not a factor. */

            if (byTwo) {		/* Advance to the next odd factor. */
                factor += 2 ;
            } else {			/* Skip the odd number divisible by 3. */
                factor += 4 ;
            }

            byTwo = !byTwo ;		/* Alternate increments by 2 and by 4. */

        }

        numTests++ ;

        if ((progressDiv > 0) && ((numTests % progressDiv) == 0)) {
            LGE "t %"PRIuSIZE_T" f %"PRIuFACTOR_T" r=%"PRIXFACTOR_T" n=%"PRIXFACTOR_T"\n",
                numTests, factor, root, number) ;
        }

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.  This can happen on both composite
    and prime numbers.
*******************************************************************************/

    if (number > 1) {
        LGE "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
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

    getFactorsRootSkip3Root ()

    Get the Factors of a Number (Square Root, Skipping Factors Divisible by 3,
                                 Computing Square Root After Multiple
                                 Instances of the Same Factor).


Purpose:

    Function getFactorsRootSkip3Root() computes and returns the prime
    factors of a number.  The function uses the square root of the input
    number as the upper bound of factors to test, thus avoiding the
    possibility of overflow.


    Invocation:

        numFactors = getFactorsRootSkip3Root (number, maxFactors, &factors) ;

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


static  size_t  getFactorsRootSkip3Root (

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
    const  char  *prefix = "(getFactorsRootSkip3Root) " ;
    factor_t  factor, previous, root ;
    size_t  i, numFactors ;
#define  FRIZE_FACTOR  1	/* Define as 1 in "frize" program. */
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

    factor = 1 ;

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
        root = (factor_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
        numRoots++ ;
        LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
            prefix, number, root, factor) ;
    } else {
        root = 0 ;
    }

    while ((number > 1) &&		/* Until number is reduced to 1 ... */
           (factor <= root)) {		/* ... or all factors are tested. */

        previous = number ;
			/* Divides evenly?  Is a factor.  Keep testing in case
			   there are multiple instances of the same factor. */
LGI "%"PRIuSIZE_T"  Testing %"PRIuFACTOR_T", root %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, factor, root, number) ;
        while ((number > 1) && ((number % factor) == 0)) {

            numTests++ ;
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
LGI "-- %"PRIuSIZE_T"  Testing %"PRIuFACTOR_T", root %"PRIuFACTOR_T", number %"PRIuFACTOR_T"\n", numTests + 1, factor, root, number) ;
        }

        if (number == 1)  break ;	/* Final factor found? */

        numTests++ ;

        if (number < previous) {	/* Target number was reduced?
					   Recalculate square root. */
            root = (factor_t) nbrUnsignedSQRT ((root_t) number, NULL) ;
            numRoots++ ;
            LGI "%s# = %"PRIuFACTOR_T"  root = %"PRIuFACTOR_T"  factor = %"PRIuFACTOR_T"\n",
                prefix, number, root, factor) ;
        }
			/* The factor tested was (or is now) not a factor. */
        if (byTwo) {			/* Advance to the next odd factor. */
            factor += 2 ;
        } else {			/* Skip the odd number divisible by 3. */
            factor += 4 ;
        }

        byTwo = !byTwo ;		/* Alternate increments by 2 and by 4. */

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.  This can happen on both composite
    and prime numbers.
*******************************************************************************/

    if (number > 1) {
        LGE "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
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

    getFactorsSquare ()

    Get the Factors of a Number (Squares Only).


Purpose:

    Function getFactorsSquare() computes and returns the prime factors of a
    number.  The function keeps track of the squares of the factors so that
    calculating the square of the next odd factor to be tested is slightly
    faster than directly computing the square of the factor.


    Invocation:

        numFactors = getFactorsSquare (number, maxFactors, &factors) ;

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


static  size_t  getFactorsSquare (

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
    const  char  *prefix = "(getFactorsSquare) " ;
    factor_t  factor, prevSquare, square ;
    size_t  i, numFactors ;



    numFactors = 0 ;
    numTests = 0 ;
    numSquareOverflows = 0 ;

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

    factor = 1 ;

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


/* If, for some reason, the table is empty or the factor 2 was not checked,
   then check factor 2.  Beginning with 3, the remaining factors tested will
   be odd. */

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


    factor += (factor > 2) ? 2 : 1 ;	/* Advance to next odd factor. */

/*******************************************************************************

    Check all the remaining odd factors not divisible by 3 up to the largest
    odd number whose square does not exceed the target number.  (The target
    number was/is the original input number, but it may have been reduced
    by the tests of the early primes.)

    When an actual factor is found, it is added to the list of the input
    number's factors.  Adding the factor to the list of factors found so
    far must be balanced by removing the factor from the target number;
    i.e., divide the target number by the factor.  Only the product of
    the remaining factors needs to and must be factored and its factors
    will all be equal to or greater than the factor just found.  (I say
    "must" because reducing the target number is necessary to detect
    multiple instances of the same factor.)

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

        (2) Pr has not been reduced to 1, but the square of the next factor
            to be tested exceeds Pr.  When this happens, Pr will be added
            as the final factor of the input number after the loop exits.
            This is always the case when factoring a prime number.

*******************************************************************************/

    square = factor * factor ;
    prevSquare = 0 ;

    while ((number > 1) &&		/* Until number is reduced to 1 ... */
           (square <= number)) {	/* ... or all factors are tested. */

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

        } else {			/* Not a factor. */
					/* Advance to the next odd factor. */
            /* (f + 2)^2 =
               f^2 + 4f + 4 =
               f^2 + 4(f + 1) */
            prevSquare = square ;
            factor++ ;
            square += factor * 4 ;
            square &= overflowMask ;
            factor++ ;

            if (square < prevSquare) {
                numSquareOverflows++ ;
                if (allowOverflows) {
                    LGI "%sOVF/SQUARE test %"PRIuSIZE_T" prev=0x%"PRIXFACTOR_T" new=0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, prevSquare, square) ;
                } else {
                    SET_ERRNO (EOVERFLOW) ;
                    LGE "%sOverflow of square, test %"PRIuSIZE_T":  previous = 0x%"PRIXFACTOR_T"  new = 0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, prevSquare, square) ;
                    return (0) ;
                }
            }

        }

        numTests++ ;

        if ((progressDiv > 0) && ((numTests % progressDiv) == 0)) {
            LGE "t %"PRIuSIZE_T" f %"PRIuFACTOR_T" psq=%"PRIXFACTOR_T" sq=%"PRIXFACTOR_T" n=%"PRIXFACTOR_T"\n",
                numTests, factor, prevSquare, square, number) ;
        }

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.
*******************************************************************************/

    if (number > 1) {
        LGE "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
        if ((numFactors >= maxFactors) && (factors != NULL)) {
            SET_ERRNO (ENOMEM) ;
            LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                prefix, maxFactors) ;
            return (0) ;
        }
        if (factors != NULL)  factors[numFactors] = number ;
        numFactors++ ;
    }


    LGI "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
        prefix, number, numFactors, numTests) ;

    return (numFactors) ;

}

/*!*****************************************************************************

Procedure:

    getFactorsSquareSkip3 ()

    Get the Factors of a Number (Squares Only, Skipping Factors Divisible by 3).


Purpose:

    Function getFactorsSquareSkip3() computes and returns the prime factors
    of a number.  Like getFactorsSquare(), this function keeps track of the
    squares of the factors so that calculating the square of the next
    factor to be tested is slightly faster than directly computing the
    square of the factor.  This function differs from getFactorsSquare()
    in that it skips odd factors divisible by 3 (except for 3 itself).


    Invocation:

        numFactors = getFactorsSquareSkip3 (number, maxFactors, &factors) ;

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


static  size_t  getFactorsSquareSkip3 (

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
    const  char  *prefix = "(getFactorsSquareSkip3) " ;
    factor_t  factor, prevSquare, square ;
    size_t  i, numFactors ;



    numFactors = 0 ;
    numTests = 0 ;
    numSquareOverflows = 0 ;

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

    factor = 1 ;

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

    Check all the remaining odd factors not divisible by 3 up to the largest
    odd number whose square does not exceed the target number.  (The target
    number was/is the original input number, but it may have been reduced
    by the tests of the early primes.)

    When an actual factor is found, it is added to the list of the input
    number's factors.  Adding the factor to the list of factors found so
    far must be balanced by removing the factor from the target number;
    i.e., divide the target number by the factor.  Only the product of
    the remaining factors needs to and must be factored and its factors
    will all be equal to or greater than the factor just found.  (I say
    "must" because reducing the target number is necessary to detect
    multiple instances of the same factor.)

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

        (2) Pr has not been reduced to 1, but the square of the next factor
            to be tested exceeds Pr.  When this happens, Pr will be added
            as the final factor of the input number after the loop exits.
            This is always the case when factoring a prime number.

*******************************************************************************/

    square = factor * factor ;
    prevSquare = 0 ;
    byTwo = true ;

    while ((number > 1) &&		/* Until number is reduced to 1 ... */
           (square <= number)) {	/* ... or all factors are tested. */

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

        } else {			/* Not a factor. */

            prevSquare = square ;

            if (byTwo) {		/* Advance to the next odd factor. */
                /* (f + 2)^2 =
                   f^2 + 4f + 4 =
                   f^2 + 4(f + 1) */
                factor++ ;
                square += factor * 4 ;
                square &= overflowMask ;
                factor++ ;
            } else {			/* Skip the odd number divisible by 3. */
                /* (f + 4)^2 =
                   f^2 + 8f + 16 =
                   f^2 + 8(f + 2) */
                factor += 2 ;
                square += factor * 8 ;
                square &= overflowMask ;
                factor += 2 ;
            }

            byTwo = !byTwo ;		/* Alternate increments by 2 and by 4. */

            if (square < prevSquare) {
                numSquareOverflows++ ;
                if (allowOverflows) {
                    LGI "%sOVF/SQUARE test %"PRIuSIZE_T" prev=0x%"PRIXFACTOR_T" new=0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, prevSquare, square) ;
                } else {
                    SET_ERRNO (EOVERFLOW) ;
                    LGE "%sOverflow of square, test %"PRIuSIZE_T":  previous = 0x%"PRIXFACTOR_T"  new = 0x%"PRIXFACTOR_T"\n",
                        prefix, numTests, prevSquare, square) ;
                    return (0) ;
                }
            }

        }

        numTests++ ;

        if ((progressDiv > 0) && ((numTests % progressDiv) == 0)) {
            LGE "t=%"PRIuSIZE_T" f=%"PRIuFACTOR_T" psq=%"PRIXFACTOR_T" sq=%"PRIXFACTOR_T" n=%"PRIXFACTOR_T"\n",
                numTests, factor, prevSquare, square, number) ;
        }

    }


/*******************************************************************************
    If the reduced number after the factoring is not 1, then add the number as
    the last factor of the original number.
*******************************************************************************/

    if (number > 1) {
        LGE "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
            prefix, number, numFactors, numTests) ;
        if ((numFactors >= maxFactors) && (factors != NULL)) {
            SET_ERRNO (ENOMEM) ;
            LGE "%s# of factors exceeds maximum of %"PRIuSIZE_T": \n",
                prefix, maxFactors) ;
            return (0) ;
        }
        if (factors != NULL)  factors[numFactors] = number ;
        numFactors++ ;
    }


    LGI "%s# = %"PRIuFACTOR_T"  # factors = %"PRIuSIZE_T"  # tests = %"PRIuSIZE_T"\n",
        prefix, number, numFactors, numTests) ;

    return (numFactors) ;

}
