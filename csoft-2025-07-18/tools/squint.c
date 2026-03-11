/* $Id: squint.c,v 1.5 2021/09/16 03:54:51 alex Exp alex $ */
/*******************************************************************************

Program:

    squint.c

    Square Root (Integer) Test Tool.


Author:    Alex Measday


Purpose:

    SQUINT tests various integer square root algorithms, including
    nbrUnsignedSQRT() in LIBGPL's NBR_UTIL package.  Mostly I use
    the program for benchmarking the algorithms.

    The default integer square root function is nbrUnsignedSQRT().
    Other functions that can be specified on the command line are:

        "alternate" - an alternate function used to test changes that will,
            if they work, eventually be incorporated into nbrUnsignedSQRT().

        "crenshaw" - is Jack Crenshaw's integer square root function.  Here's
            his original column, "Integer Square Roots", in EMBEDDED SYSTEMS
            PROGRAMMING, February 1, 1998:

                https://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots

            The web page is missing the images and code listings, however the
            code listings can be found via:

                https://www.embedded.com/design/embedded/source-code/4209946/cren98-2-txt

            at:

                https://cms.edn.com/uploads/SourceCode/cren98_21.txt

            The algorithm is also explained in identical or more detail in
            chapter 4 of his book, MATH TOOLKIT FOR REAL-TIME DEVELOPMENT,
            later renamed MATH TOOLKIT FOR REAL-TIME PROGRAMMING.  The
            code I used is the one presented in Listing 4 of his column
            and Listing 4.7 of his book.  (If you know how to use Google,
            you can find chapter 4 online at Google books.)

        "martin" - is Martin Buchanan's function, which bears some
            resemblance to Crenshaw's function.  I used his 3rd integer
            square root function found at the Code Codex:

                http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

            (The 3rd function is identical to his second function, but with
            different variable names.)

        "ross" - is my original interpretation of Ross Fosler's algorithm:

                "Fast Integer Square Root" by Ross M. Fosler
                Application Note TB040, DS91040A,
                (c) 2000 Microchip Technology Inc.
                http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf

        "tristan" - is Tristan Muntsinger's integer square root function,
            which is a variation of Ross Fosler's algorithm.  Martin
            Buchanan included this function on the same Code Codex page:

                http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

    The "crenshaw", "martin", and "tristan" functions were modified to
    work at different precisions and were tested using 64-bit unsigned
    longs.  All the functions use the same, one-time determination of
    precision as nbrUnsignedSQRT().  I don't yet understand how they work,
    so Crenshaw's and Martin's functions bail out when faced with
    odd-precision integers.


    Invocation:

        % squint [-debug] [-help]
                 [-repeat <count>] [-time]
                 <number>[,alt|crenshaw|martin|ross|tristan]

    where:

        "-debug"
            turns debug on.
        "-help"
            prints out a help message.
        "-repeat <count>"
            repeats the square root computation the specified number of times.
            A single computation takes too little time to make meaningful
            comparisons between algorithms.  On my computer, calculating the
            square root of 123456789^2 ten million times produced times under
            2 seconds for the various algorithms; repeating the calculations
            100 million times resulted in times in the 10-20-second range,
            thus providing more granularity for comparison purposes.
        "-time"
            measures and displays the time it takes to compute the square root
            (or <count> iterations of the computation if "-repeat <count>" is
            also given on the command line).  Almost all output is suppressed
            to keep I/O from affecting the timing.
        "<number>[,alternate|crenshaw|martin|tristan]"
            prints the integer square root of the number.  The optional
            argument after a comma (no spaces!) specifies an alternate,
            integer square root function (all of which are compiled into
            this program):
                "alternate" - is an alternate function I use to test changes
                    I'm going to make to nbrUnsignedSQRT().
                "crenshaw" - is Jack Crenshaw's integer square root function.
                "martin" - is Martin Buchanan's function.
                "ross" - is Ross Fosler's function.
                "tristan" - is Tristan Muntsinger's function.
            I use this capability for benchmarking different algorithms.  You
            can abbreviate the names to any length, so I usually just specify
            one letter; e.g., "-sqrt 144,m" will compute the square root of
            144 using Martin's algorithm.  If the optional argument is not
            present, the default function is LIBGPL's nbrUnsignedSQRT().
            The file prologue above lists the URLs for the named functions'
            documentation or code.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

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

#define  EXTERNAL_SQRT_FUNCTIONS  0
#if defined(EXTERNAL_SQRT_FUNCTIONS) && EXTERNAL_SQRT_FUNCTIONS
    extern  root_t  squintUnsignedSQRT_A P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    extern  root_t  squintUnsignedSQRT_C P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    extern  root_t  squintUnsignedSQRT_M P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    extern  root_t  squintUnsignedSQRT_R P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    extern  root_t  squintUnsignedSQRT_T P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
#else
    static  root_t  squintUnsignedSQRT_A P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    static  root_t  squintUnsignedSQRT_C P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    static  root_t  squintUnsignedSQRT_M P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    static  root_t  squintUnsignedSQRT_R P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
    static  root_t  squintUnsignedSQRT_T P_((root_t  n,
                                             UnsignedSqrtContext  *c)) ;
#endif

/*******************************************************************************
    SQUINT's Main Program.
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
    int  errflg, option ;
    OptContext  scan ;
    root_t  number, root ;
    size_t  repeat ;
    UnsignedSqrtFunc  sqrtF ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}", "{repeat:}", "{time}", NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    help = false ;
    benchmark = false ;
    repeat = 1 ;

    sqrtF = nbrUnsignedSQRT ;
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
        case 3:				/* "-repeat <count>" */
            repeat = strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 4:				/* "-time" */
            benchmark = true ;
            break ;
        case NONOPT:			/* "<number>[,alt|crenshaw|martin|tristan]" */
            if (algo != NULL) {		/* #/algorithm already specified? */
               errflg++ ;  break ;
            }
            sqrtF = nbrUnsignedSQRT ;		/* Default: libgpl sqrt(). */
            algo = "nbr" ;
            number = (root_t) strtoul (argument, &s, 0) ;
            if (*s == ',') {
                s++ ;
                if (strMatch (s, "alternate", true)) {
                    sqrtF = squintUnsignedSQRT_A ;	/* Alternate sqrt(). */
                    algo = "alt" ;
                } else if (strMatch (s, "crenshaw", true)) {
                    sqrtF = squintUnsignedSQRT_C ;	/* Jack Crenshaw's sqrt(). */
                    algo = "crenshaw" ;
                } else if (strMatch (s, "martin", true)) {
                    sqrtF = squintUnsignedSQRT_M ;	/* Martin Buchanan's sqrt(). */
                    algo = "martin" ;
                } else if (strMatch (s, "ross", true)) {
                    sqrtF = squintUnsignedSQRT_R ;	/* Ross M. Fosler's sqrt(). */
                    algo = "ross" ;
                } else if (strMatch (s, "tristan", true)) {
                    sqrtF = squintUnsignedSQRT_T ;	/* Tristan Muntsinger's sqrt(). */
                    algo = "tristan" ;
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
        fprintf (stderr, "Usage:  squint [-debug] [-help]\n") ;
        fprintf (stderr, "               [-repeat <count>] [-time]\n") ;
        fprintf (stderr, "               <number>[,alt|crenshaw|martin|ross|tristan]\n") ;
    }

    if (errflg && !help) {
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Enter \"squint -help\" for detailed help.\n") ;
    }

    if (help) {
        fprintf (stderr, "\n") ;
        fprintf (stderr, "Notes:\n") ;
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
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    <number>[,alternate|crenshaw|martin|ross|tristan]\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        prints the integer square root of the number.  The optional\n") ;
        fprintf (stderr, "        argument after a comma (no spaces!) specifies an alternate,\n") ;
        fprintf (stderr, "        integer square root function (all of which are compiled into\n") ;
        fprintf (stderr, "        the program):\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "            \"alternate\" - is an alternate function I use to test changes\n") ;
        fprintf (stderr, "                I'm going to make to nbrUnsignedSQRT().\n") ;
        fprintf (stderr, "            \"crenshaw\" - is Jack Crenshaw's integer square root function.\n") ;
        fprintf (stderr, "            \"martin\" - is Martin Buchanan's function.\n") ;
        fprintf (stderr, "            \"ross\" - is my original interpretation of Ross Fosler's function.\n") ;
        fprintf (stderr, "            \"tristan\" - is Tristan Muntsinger's function.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "        You can abbreviate the names to any length, even one letter;\n") ;
        fprintf (stderr, "        for example, \"-sqrt 144,m\" will compute the square root of 144\n") ;
        fprintf (stderr, "        using Martin Buchanan's algorithm.  If the optional argument is\n") ;
        fprintf (stderr, "        not present, the default function is LIBGPL's nbrUnsignedSQRT().\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "References:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    All functions have been adapted to work with unsigned longs of any size.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    Jack Crenshaw, \"Integer Square Roots\", in EMBEDDED SYSTEMS PROGRAMMING,\n") ;
        fprintf (stderr, "    February 1, 1998:\n") ;
        fprintf (stderr, "        https://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots\n") ;
        fprintf (stderr, "    See Listing 4 in the code listings which can be found via:\n") ;
        fprintf (stderr, "        https://www.embedded.com/design/embedded/source-code/4209946/cren98-2-txt\n") ;
        fprintf (stderr, "    at:\n") ;
        fprintf (stderr, "        https://cms.edn.com/uploads/SourceCode/cren98_21.txt\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    Martin Buchanan, 3rd integer square root function found at the Code Codex:\n") ;
        fprintf (stderr, "        http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    Ross M. Fosler, \"Fast Integer Square Root\", Application Note TB040,\n") ;
        fprintf (stderr, "    DS91040A, (c) 2000 Microchip Technology Inc.\n") ;
        fprintf (stderr, "        http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    Tristan Muntsinger, integer square root function following Martin\n") ;
        fprintf (stderr, "    Buchanan's functions on the same Code Codex page:\n") ;
        fprintf (stderr, "        http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C\n") ;
    }

    if (errflg || help)  exit (EINVAL) ;

/*******************************************************************************
    Compute the fast integer square root of a number.
*******************************************************************************/

    bmwStart (&clock) ;

    if (repeat < 1)  repeat = 1 ;

#define  NO_GRANULAR_TIMING
#ifdef NO_GRANULAR_TIMING
    while (repeat-- > 0) {
        root = sqrtF (number, /*context*/ NULL) ;
        if (!benchmark)  printf ("Integer square root (%s) of %"PRIuROOT_T" is %"PRIuROOT_T".\n",
                                 algo, number, root) ;
    }
    bmwStop (&clock) ;
#else

    BmwClock  clock_sqrt[10] ;
    size_t  i, i_sqrt = 0, block_sqrt = repeat / 10 ;

    for (i = 0 ;  i < 10 ;  i++) {
        bmwStart (&clock_sqrt[i_sqrt]) ;
        repeat = block_sqrt ;
        while (repeat-- > 0) {
            root = sqrtF (number, /*context*/ NULL) ;
            if (!benchmark)  printf ("Integer square root (%s) of %"PRIuROOT_T" is %"PRIuROOT_T".\n",
                                     algo, number, root) ;
        }
        bmwStop (&clock_sqrt[i_sqrt++]) ;
    }

    bmwStop (&clock_sqrt[i_sqrt]) ;
    bmwStop (&clock) ;
    printf ("isqrt = %"PRIuSIZE_T"\n", i_sqrt) ;
    for (i = 0 ;  i < i_sqrt ;  i++) {
        printf ("Slice[%"PRIuSIZE_T"]: %g\n",
                i, bmwElapsed (&clock_sqrt[i])) ;
    }
    clock_sqrt[0].stopTime = clock_sqrt[i_sqrt].stopTime ;
    printf ("Slice[all]: %g\n", bmwElapsed (&clock_sqrt[0])) ;
#endif

    if (benchmark) {
#if defined(HAVE_PRINTF_FORMAT_FLOAT) && !HAVE_PRINTF_FORMAT_FLOAT
        struct  timeval  elapsedTime = bmwElapsedTV (&clock) ;
        printf ("%ld seconds, %ld usecs (%s)\n",
                elapsedTime.tv_sec, elapsedTime.tv_usec, algo) ;
#else
        printf ("%.5f seconds (%s)\n", bmwElapsed (&clock), algo) ;
#endif
    }

    exit (0) ;

}

#if !defined(EXTERNAL_SQRT_FUNCTIONS) || !EXTERNAL_SQRT_FUNCTIONS
/*******************************************************************************

    squintUnsignedSQRT_A() - determines the integer square root, floor[sqrt(n)],
        of a number.  It's used to test different square root algorithms.

*******************************************************************************/


static  root_t  squintUnsignedSQRT_A (

#    if PROTOTYPES
        root_t  number,
        UnsignedSqrtContext  *context)
#    else
        number, context)

        root_t  number ;
        UnsignedSqrtContext  *context ;
#    endif

{    /* Local variables. */
    root_t  bitmask, root ;

#if defined(DYNAMIC_CONTEXT) && !DYNAMIC_CONTEXT
    static  UnsignedSqrtContext  defaultContext = { false, 64, 0x80000000 } ;

    if (context == NULL)  context = &defaultContext ;
#else
    static  UnsignedSqrtContext  defaultContext = { false, 0, 0 } ;

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
        LGI "(squintUnsignedSQRT_A) Context:  odd = %s  precision = %"PRIuSIZE_T" bits  estimate = %"PRIuROOT_T" (0x%"PRIXROOT_T")\n",
            context->oddPrecision ? "true" : "false",
            context->precision, context->estimate, context->estimate) ;
    }
#endif	/* DYNAMIC_CONTEXT */

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

        if (context->oddPrecision) {
            if (root > (number / root))  root ^= bitmask ;
        } else if ((root * root) > number) {
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

/*******************************************************************************

    squintUnsignedSQRT_C() - determines the integer square root, floor[sqrt(n)],
        of a number.  It's used to test different square root algorithms.

        This is an implementation of Jack W. Crenshaw's unsigned square root
        algorithm.  Here's his original column, "Integer Square Roots", in
        EMBEDDED SYSTEMS PROGRAMMING, February 1, 1998:

            https://www.embedded.com/electronics-blogs/programmer-s-toolbox/4219659/Integer-Square-Roots

        The web page is missing the images and code listings, however the code
        listings can be found via:

            https://www.embedded.com/design/embedded/source-code/4209946/cren98-2-txt

        at:

            https://cms.edn.com/uploads/SourceCode/cren98_21.txt

        The algorithm is also explained in identical or more detail in chapter
        4 of his book, MATH TOOLKIT FOR REAL-TIME DEVELOPMENT, later renamed
        MATH TOOLKIT FOR REAL-TIME PROGRAMMING.  The algorithm implemented
        below is the one presented in Listing 4 of his column and Listing 4.7
        of his book.  (If you know how to use Google, you can find chapter 4
        online at Google books and, elsewhere, you can find PDF versions of
        the full book.  Ideally, you should buy the book, but it's not cheap.)

        I don't understand how his algorithm works and I really need to sit
        down and work through a pencil-and-paper example as he does.  The
        algorithm works on two digits (i.e., 2 bits) at a time, so I'm not
        yet sure how to adapt the algorithm to integers whose effective
        precision is odd.

*******************************************************************************/


static  root_t  squintUnsignedSQRT_C (

#    if PROTOTYPES
        root_t  number,
        UnsignedSqrtContext  *context)
#    else
        number, context)

        root_t  number ;
        UnsignedSqrtContext  *context ;
#    endif

{    /* Local variables. */
    size_t  bitsToScan, bitsToShift, i ;
    root_t  rem, root ;

#if defined(DYNAMIC_CONTEXT) && !DYNAMIC_CONTEXT
    static  UnsignedSqrtContext  defaultContext = { false, 64, 0x80000000 } ;

    if (context == NULL)  context = &defaultContext ;
#else
    static  UnsignedSqrtContext  defaultContext = { false, 0, 0 } ;

    if (context == NULL)  context = &defaultContext ;
		/* On first call, compute parameters for CPU/compiler. */
    if (context->precision == 0) {
		/* Determine the number of bits in a root integer
		   by shifting a 1 to the left until it disappears. */
        context->estimate = 1 ;
        context->precision = 1 ;
        while ((context->estimate <<= 1) != 0)
            context->precision++ ;
        context->oddPrecision = ((context->precision % 2) != 0) ;
		/* Set the initial estimate to the most significant bit of
		   the maximum square root. */
        if (context->oddPrecision)
            context->estimate = ROOT_T_C(1) << (context->precision / 2) ;
        else
            context->estimate = ROOT_T_C(1) << ((context->precision - 1) / 2) ;
        LGI "(squintUnsignedSQRT_C) Context:  odd = %s  precision = %"PRIuSIZE_T" bits  estimate = %"PRIuROOT_T" (0x%"PRIXROOT_T")\n",
            context->oddPrecision ? "true" : "false",
            context->precision, context->estimate, context->estimate) ;
    }
#endif	/* DYNAMIC_CONTEXT */

    if (context->oddPrecision) {
        SET_ERRNO (EINVAL) ;
        LGE "(squint) Algorithm not tested for odd, %"PRIuSIZE_T"-bit precision: \n",
            context->precision) ;
        return (0) ;
    } else {
        bitsToScan = context->precision / 2 ;
        bitsToShift = context->precision - 2 ;
    }

    rem = 0 ;
    root = 0 ;

    for (i = 0 ;  i < bitsToScan ;  i++) {
        root <<= 1 ;
        rem = ((rem << 2) + (number >> bitsToShift)) ;
        number <<= 2 ;
        root++ ;
        if(root <= rem) {
            rem -= root ;
            root++ ;
        } else {
            root-- ;
        }
    }

    return (root >> 1) ;

}

/*******************************************************************************

    squintUnsignedSQRT_M() - determines the integer square root, floor[sqrt(n)],
        of a number.  It's used to test different square root algorithms.

        This is Martin Buchanan's 3rd integer square root function found
        at the Code Codex:

            http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

        (The 3rd function is identical to the second function, but with
        different variable names.)  Inspired by Jack Crenshaw's algorithm,
        Martin's function is likewise inscrutable to me at present.  It
        also works two bits at a time, so I again am not sure how to
        adapt it to odd-precision integers.  Martin says his function
        "is completely processor-dependent and maybe compiler-dependent
        as well", but he doesn't specify what those dependencies are.

*******************************************************************************/


static  root_t  squintUnsignedSQRT_M (

#    if PROTOTYPES
        root_t  number,
        UnsignedSqrtContext  *context)
#    else
        number, context)

        root_t  number ;
        UnsignedSqrtContext  *context ;
#    endif

{    /* Local variables. */
    root_t  place, remainder, root ;

#if defined(DYNAMIC_CONTEXT) && !DYNAMIC_CONTEXT
    static  UnsignedSqrtContext  defaultContext = { false, 64, 0x80000000 } ;

    if (context == NULL)  context = &defaultContext ;
#else
    static  UnsignedSqrtContext  defaultContext = { false, 0, 0 } ;

    if (context == NULL)  context = &defaultContext ;
		/* On first call, compute parameters for CPU/compiler. */
    if (context->precision == 0) {
		/* Determine the number of bits in a root integer
		   by shifting a 1 to the left until it disappears. */
        context->estimate = 1 ;
        context->precision = 1 ;
        while ((context->estimate <<= 1) != 0)
            context->precision++ ;
        context->oddPrecision = ((context->precision % 2) != 0) ;
		/* Set the initial estimate to the most significant bit of
		   the maximum square root. */
        if (context->oddPrecision)
            context->estimate = ROOT_T_C(1) << (context->precision / 2) ;
        else
            context->estimate = ROOT_T_C(1) << ((context->precision - 1) / 2) ;
        LGI "(squintUnsignedSQRT_M) Context:  odd = %s  precision = %"PRIuSIZE_T" bits  estimate = %"PRIuROOT_T" (0x%"PRIXROOT_T")\n",
            context->oddPrecision ? "true" : "false",
            context->precision, context->estimate, context->estimate) ;
    }
#endif	/* DYNAMIC_CONTEXT */

    if (context->oddPrecision) {
        SET_ERRNO (EINVAL) ;
        LGE "(squintUnsignedSQRT_M) Algorithm not tested for odd, %"PRIuSIZE_T"-bit precision: \n",
            context->precision) ;
        return (0) ;
    }

    remainder = number ;
    root = 0 ;

/* "place" starts at the highest power of four <= than the argument. */

    place = 1UL << (context->precision - 2) ;	/* second-to-top bit set */
    while (place > remainder)
        place >>= 2 ;

/* printf ("(M:before loop) root = 0x%"PRIXROOT"  place = 0x%"PRIXROOT"  remainder = 0x%"PRIXROOT"\n",
        root, place, remainder) ; */

    while (place != 0) {
        if (remainder >= root + place) {
            remainder -= root + place ;
            root += place << 1 ;	/* <-- faster than 2 * place */

/* printf (" (M:rem > r+p)) root = 0x%"PRIXROOT"  place = 0x%"PRIXROOT"  remainder = 0x%"PRIXROOT"\n",
        root, place, remainder) ; */
        }
        root >>= 1 ;
        place >>= 2 ;

/* printf ("(M:bottom loop) root = 0x%"PRIXROOT"  place = 0x%"PRIXROOT"  remainder = 0x%"PRIXROOT"\n",
        root, place, remainder) ; */
    }

    return (root) ;

}

/*******************************************************************************

    squintUnsignedSQRT_R() - determines the integer square root, floor[sqrt(n)],
        of a number.  It's used to test different square root algorithms.

        This was my original implementation of Ross Fosler's integer square
        root function, presented in:

            "Fast Integer Square Root" by Ross M. Fosler
            Application Note TB040, DS91040A, (c) 2000 Microchip Technology Inc.
            http://ww1.microchip.com/downloads/en/AppNotes/91040a.pdf

        His code was in assembly language and his flowchart was difficult
        to understand, so I derived my interpretation of his algorithm from
        the example he worked through.  In the function below, I borrowed the
        simpler means of setting and clearing bits from Tristan Muntsinger's
        square root function; see squintUnsignedSQRT_T().  I kept, for now, the
        time-consuming "root relop number/root" comparisons that also work
        for odd-precision integers.

*******************************************************************************/


static  root_t  squintUnsignedSQRT_R (

#    if PROTOTYPES
        root_t  number,
        UnsignedSqrtContext  *context)
#    else
        number, context)

        root_t  number ;
        UnsignedSqrtContext  *context ;
#    endif

{    /* Local variables. */
    root_t  bitmask, root ;

#if defined(DYNAMIC_CONTEXT) && !DYNAMIC_CONTEXT
    static  UnsignedSqrtContext  defaultContext = { false, 64, 0x80000000 } ;

    if (context == NULL)  context = &defaultContext ;
#else
    static  UnsignedSqrtContext  defaultContext = { false, 0, 0 } ;

    if (context == NULL)  context = &defaultContext ;
		/* On first call, compute parameters for CPU/compiler. */
    if (context->precision == 0) {
		/* Determine the number of bits in a root integer
		   by shifting a 1 to the left until it disappears. */
        context->estimate = 1 ;
        context->precision = 1 ;
        while ((context->estimate <<= 1) != 0)
            context->precision++ ;
        context->oddPrecision = ((context->precision % 2) != 0) ;
		/* Set the initial estimate to the most significant bit of
		   the maximum square root. */
        if (context->oddPrecision)
            context->estimate = ROOT_T_C(1) << (context->precision / 2) ;
        else
            context->estimate = ROOT_T_C(1) << ((context->precision - 1) / 2) ;
        LGI "(squintUnsignedSQRT_R) Context:  odd = %s  precision = %"PRIuSIZE_T" bits  estimate = %"PRIuROOT_T" (0x%"PRIXROOT_T")\n",
            context->oddPrecision ? "true" : "false",
            context->precision, context->estimate, context->estimate) ;
    }
#endif	/* DYNAMIC_CONTEXT */

    root = context->estimate ;

/* Working from the most significant bit of the square root (which is the only
   bit set in the initial approximation above) down to the least significant
   bit, refine the value of the square root. */

    bitmask = root / 2 ;	/* Keeps track of current bit being examined. */

    while (bitmask != 0) {

        if (root < (number / root)) {

            /* Set the current bit. */
            root |= bitmask ;		/* (root & ~bitmask) | bitmask ; */

        } else if (root > (number / root)) {

            /* Clear the previous bit and set the current.  (Replace
               "10" with "01".) */
            root ^= bitmask * 2 ;	/* (root & ~(bitmask * 2)) | 0 ; */
            root |= bitmask;		/* (root & ~bitmask) | bitmask ; */

        } else {

            /* The square root has been found.  Leave remaining bits zero. */
            bitmask = 0 ;

        }

        bitmask = bitmask / 2 ;

    }

/* If the least significant bit of the result is 1 and the square of the
   root is greater than the number, then replace the 1 by a 0.  This is
   the same as replacing "10" with "01" in the loop above, but going into
   fractional territory: replace binary "1.0" (1) with "0.1" (1/2).  Doing
   so ensures that the integer square root is the largest root whose square
   is less than or equal to the number. */

    if (root > (number / root))  root-- ;

    return (root) ;

}

/*******************************************************************************

    squintUnsignedSQRT_T() - determines the integer square root, floor[sqrt(n)],
        of a number.  It's used to test different square root algorithms.

        Tristan Muntsinger's square root function follows Martin's at the
        Code Codex:

            http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

        His function implements Ross Fosler's "Fast Integer Square Root"
        algorithm, but with an ingenious twist that boosts performance.
        The shifting of the bit mask one bit to the right and testing
        if the bit has shifted out of the mask (bit mask == 0), is done
        in the *middle* of the loop.

        My original implementation of Ross Fossler's algorithm determined
        if the estimate was greater than the actual square root when
        examining the second, "0" bit in the sequence "10"; my function
        would then clear the previous "1" bit" and set the current bit;
        i.e., "10" -> "01".  Tristan's function detects that the estimate
        is greater than the actual root when examining the first, "1" bit
        and simply toggles the "1" to "0", shifts and tests the current
        bit position, and then ORs in the "1" bit to replace the original,
        second, "0" bit: "10" -> "01".  Tristan's scheme is much faster.

        Replacing my "g relop n/g" constructs with "g*g relop n" was also a
        big time saver.  This works fine with the more-likely even-precision
        integers, but not at all with odd-precision integers.  I added in a
        check for odd precision, in which case the code will fall back to
        using the safe, "g relop n/g" comparisons.

*******************************************************************************/


static  root_t  squintUnsignedSQRT_T (

#    if PROTOTYPES
        root_t  number,
        UnsignedSqrtContext  *context)
#    else
        number, context)

        root_t  number ;
        UnsignedSqrtContext  *context ;
#    endif

{    /* Local variables. */
    root_t  c, g ;

#if defined(DYNAMIC_CONTEXT) && !DYNAMIC_CONTEXT
    static  UnsignedSqrtContext  defaultContext = { false, 64, 0x80000000 } ;

    if (context == NULL)  context = &defaultContext ;
#else
    static  UnsignedSqrtContext  defaultContext = { false, 0, 0 } ;

    if (context == NULL)  context = &defaultContext ;
		/* On first call, compute parameters for CPU/compiler. */
    if (context->precision == 0) {
		/* Determine the number of bits in a root integer
		   by shifting a 1 to the left until it disappears. */
        context->estimate = 1 ;
        context->precision = 1 ;
        while ((context->estimate <<= 1) != 0)
            context->precision++ ;
        context->oddPrecision = ((context->precision % 2) != 0) ;
		/* Set the initial estimate to the most significant bit of
		   the maximum square root. */
        if (context->oddPrecision)
            context->estimate = ROOT_T_C(1) << (context->precision / 2) ;
        else
            context->estimate = ROOT_T_C(1) << ((context->precision - 1) / 2) ;
        LGI "(squintUnsignedSQRT_T) Context:  odd = %s  precision = %"PRIuSIZE_T" bits  estimate = %"PRIuROOT_T" (0x%"PRIXROOT_T")\n",
            context->oddPrecision ? "true" : "false",
            context->precision, context->estimate, context->estimate) ;
    }
#endif	/* DYNAMIC_CONTEXT */

    c = g = context->estimate ;

    for ( ; ; ) {
#ifdef CHECK_ODD_PRECISION
        if (context->oddPrecision) {
            if (g > number/g)  g ^= c ;
        } else
#endif
        if (g*g > number) {
            g ^= c ;
        }
        c >>= 1 ;
        if (c == 0)  return (g) ;
        g |= c ;
    }

}


#endif	/* !EXTERNAL_SQRT_FUNCTIONS */
