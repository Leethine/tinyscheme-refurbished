/* $Id: aperror.c,v 1.8 2019/05/28 03:48:00 alex Exp $ */
/*******************************************************************************

Procedure:

    aperror ()

    Format and Output a System Error Message.


Author:    Alex Measday


Purpose:

    Function aperror() formats an error message, including the text for
    the value of ERRNO, and writes it to a preselected output file or
    standard error.  Basically, aperror is the perror(3) function with
    the ability to format a more complex error message.

    There are two important global variables associated with aperror():

        extern  int  aperror_print ;		-- 0 = no print, !0 = print
        extern  FILE  *aperror_file ;		-- APERROR output device

    If the aperror-print variable is zero (its initial value), aperror()
    will NOT output error messages; if the variable is non-zero, aperror()
    WILL output the messages.

        NOTE: Environment variable "APERROR_PRINT" can be defined and
        set to a non-zero value to enable the printing of error messages.
        If error messages are enabled during initialization, as suggested
        below, and then programmatically disabled during normal operation,
        the presence of the environment variable with a non-zero value
        overrides the internal disabling of error messages and so errors
        will be reported during normal operation.  The environment variable
        is only checked once, the first time aperror() is called with
        global variable aperror-print set to 0.

    The aperror-file variable specifies the file to which aperror() writes
    its messages.  If the application doesn't set the variable, error messages
    are written to standard error (stderr) or, if the platform doesn't support
    a standard error channel, to standard out (stdout).

    If an application doesn't want error messages being generated during
    normal operation, it's still a good idea to enable them during start-up,
    as that may be when errors are mostly likely to occur:

        #include  "aperror.h"			-- Error reporting function.

        size_t  num_combos = 500 ;
        struct  Combination  *combo ;

        aperror_print = 1 ;			-- Enable error messages.
						-- Begin initialization ...
        combo = calloc (num_combos, sizeof (struct Combination)) ;
        if (combo == NULL) {
            aperror ("Error allocating %zu combinations.\ncalloc: ",
                     num_combos) ;
            exit (errno) ;
        }
						-- ... End initialization.
        aperror_print = 0 ;			-- Disable error messages.

        ... normal operation ...

    If the calloc(3) call above fails, aperror() will produce the following
    error message:

        Error allocating 500 combinations.
        calloc: Out of memory


    Invocation:

        aperror (format, arg1, arg2, ..., argN) ;

    where

        <format>
            is a normal PRINTF format string.
        <arg1, ..., argN>
            are the arguments referenced by the format string.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#if !defined(HAVE_STDARG_H) || HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "aperror.h"			/* Error reporting function. */


/*******************************************************************************
    Global APERROR controls:
        APERROR_PRINT - turns APERROR output on or off.  Set APERROR_PRINT to
            a non-zero value to turn output on; set it to zero to turn output
            off.
        APERROR_FILE - is the (FILE *) pointer for APERROR's output device;
            the default destination of error output is STDERR.
*******************************************************************************/

int  aperror_print = 0 ;		/* 0 = no print, !0 = print */
FILE  *aperror_file = NULL ;		/* APERROR output device */

/*******************************************************************************

    aperror() - format and output an error message.

*******************************************************************************/


void  aperror (

#    if PROTOTYPES
        const  char  *format,
        ...)
#    else
        format, va_alist)

        char  *format ;
        va_dcl
#    endif

{    /* Local variables. */
    char  *s ;
    va_list  ap ;
    static  char  *envar_value = NULL ;



/* On the first call to this function with the global APERROR_PRINT flag set
   to false, check to see if environment variable, APERROR_PRINT, is defined.
   Waiting until the flag is false before checking the environment variable
   allows this mechanism to work for programs that enable error output during
   initialization and disable it in subsequent processing. */

    if ((envar_value == NULL) & !aperror_print) {
        PUSH_ERRNO ;
        envar_value = getenv ("APERROR_PRINT") ;
        if (envar_value == NULL)  envar_value = "0" ;
        aperror_print = atoi (envar_value) ;
        POP_ERRNO ;
    }

    if (!aperror_print)  return ;	/* Printing disabled? */

					/* Output file assigned? */
#if !defined(HAVE_STDERR) || HAVE_STDERR
    if (aperror_file == NULL)  aperror_file = stderr ;
#else
    if (aperror_file == NULL)  aperror_file = stdout ;
#endif

/* Get the error message text for the error code. */

    s = STRERROR (errno) ;

/* Format and output the user text to standard error, followed by the
   system error message. */

    PUSH_ERRNO ;

#if !defined(HAVE_STDARG_H) || HAVE_STDARG_H
    va_start (ap, format) ;
#else
    va_start (ap) ;
#endif
    vfprintf (aperror_file, format, ap) ;
    fprintf (aperror_file, "%s\n", (s == NULL) ? "invalid ERRNO code" : s) ;
    va_end (ap) ;

    POP_ERRNO ;

    return ;

}

#ifdef  TEST
/*******************************************************************************

    Program to test APERROR.  Compile as follows:

        % cc -g -DTEST aperror.c -I<... includes ...>

    Invocation:

        % a.out <errno_code> [ <vmserr_code> ]

*******************************************************************************/

main (argc, argv)
    int  argc ;
    char  *argv[] ;
{

    if (argc < 2) {
        fprintf (stderr, "Usage: test <error_code>\n") ;
        exit (EINVAL) ;
    }

    aperror_print = 1 ;

    errno = atoi (argv[1]) ;
#ifdef VMS
    if ((errno == EVMSERR) && (argc > 2))  vaxc$errno = atoi (argv[2]) ;
#endif
    aperror ("APERROR(%d): ", errno) ;

}
#endif  /* TEST */
