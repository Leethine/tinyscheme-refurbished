/* $Id$ */
/*******************************************************************************

Program:

    gcd.c

    Greatest Common Divisor (GCD)


Author:    Alex Measday


Purpose:

    GCD calculates the greatest common divisor (GCD) of two numbers
    using one of various algorithms selected from the command line.


    Invocation:

        % gcd [-debug] [-help]
						# Operations
              <x> <y>
						# Modifiers
              [-binary] [-modulo] [-subtract]
              [-repeat <count>] [-time]

    where:

        "-debug"
            turns debug on.
        "-help"
            prints out a help message.
        "<x>"
        "<y>"
            are the two positive numbers whose GCD is to be calculated.
        "-binary"
            specifies use of the binary Euclid's algorithm.
        "-modulo"
            specifies use of the standard, modulo-based Euclid's algorithm.
        "-subtract"
            specifies use of the subtraction variation of Euclid's algorithm.
        "-repeat <count>"
            repeats certain operations the specified number of times.  This
            is useful when a single test of an algorithm might take too little
            time to make a meaningful comparison with another algorithm.
        "-time"
            measures and displays the time it takes to perform an operation
            (or <count> iterations of the operation if "-repeat <count>" is
            also given on the command line).  Almost all output is suppressed
            to keep I/O from affecting the timing.

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
    GCD's Main Program.
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
    const  char  *program = "gcd" ;
    GCD_t  result, x, y ;
    GCDFunc  gcdF = &nbrBinaryGCD ;
    int  errflg, option ;
    OptContext  scan ;
    size_t  numSteps, repeat ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}",
        "{binary}", "{modulo}", "{subtract}",
        "{repeat:}",  "{time}", NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    help = false ;
    benchmark = false ;
    repeat = 1 ;

    operation = NULL ;  subop = NULL ;

    x = y = 0 ;

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
        case 3:				/* "-binary" */
            gcdF = &nbrBinaryGCD ;
            subop = "binary" ;
            break ;
        case 4:				/* "-modulo" */
            gcdF = &nbrModuloGCD ;
            subop = "modulo" ;
            break ;
        case 5:				/* "-subtract" */
            gcdF = &nbrSubtractGCD ;
            subop = "subtract" ;
            break ;
        case 6:				/* "-repeat <count>" */
            repeat = strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 7:				/* "-time" */
            benchmark = true ;
            break ;
        case NONOPT:			/* "<x> <y>" */
            if (operation == NULL) {
                x = STRTO_GCD_T (argument, &s, 0) ;
                operation = "needy" ;
            } else if (strcmp (operation, "needy") == 0) {
                y = STRTO_GCD_T (argument, &s, 0) ;
                operation = "calculate" ;
            } else {
                errflg++ ;
            }
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
        fprintf (stderr, "%*s [-binary] [-modulo] [-subtract]\n", indent, "") ;
        fprintf (stderr, "%*s [<x> <y>]\n", indent, "") ;
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
        fprintf (stderr, "    sizeof (GCD_t) = %"PRIuSIZE_T" bytes\n",
                 sizeof (GCD_t)) ;
        fprintf (stderr, "...\n") ;
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
    Calculate the GCD of two numbers.
*******************************************************************************/

    if (strcmp (operation, "calculate") == 0) {

        while (repeat-- > 0) {
            result = gcdF (x, y, &numSteps) ;
            if (!benchmark)  printf ("%"PRIuGCD_T"\t= GCD (%"PRIuGCD_T", %"PRIuGCD_T")\t%"PRIuSIZE_T" loops\n",
                                     result, x, y, numSteps) ;
        }

    }


    bmwStop (&clock) ;

    if (benchmark) {
#if defined(HAVE_PRINTF_FORMAT_FLOAT) && !HAVE_PRINTF_FORMAT_FLOAT
        struct  timeval  elapsedTime = bmwElapsedTV (&clock) ;

        printf ("%ld seconds, %ld usecs (%s%s%s)\n",
                elapsedTime.tv_sec, elapsedTime.tv_usec, operation,
                (subop == NULL) ? "" : ", ",
                (subop == NULL) ? "" : subop) ;
#else
        printf ("%.5f seconds (%s%s%s)\n",
                bmwElapsed (&clock), operation,
                (subop == NULL) ? "" : ", ",
                (subop == NULL) ? "" : subop) ;
#endif
    }

    exit (0) ;

}
