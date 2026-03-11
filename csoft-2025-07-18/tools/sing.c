/* $Id: sing.c,v 1.1 2022/12/12 20:50:16 alex Exp $ */
/*******************************************************************************

Program:

    sing.c

    Test String Descriptors.


Author:    Alex Measday


Purpose:

    SING tests SDX_UTIL string descriptors.


    Invocation:

        % sing [-debug] [-help]
               [-array <file>] [-load <file>] [-read <file>] [-time]

    where:

        "-debug"
            turns debug on.
        "-help"
            prints out a help message.
        "-array <file>"
            loads an entire file line-by-line into an array.
        "-load <file>"
            loads an entire file line-by-line into a single string.
        "-read <file>"
            reads a file line-by-line into separate strings.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "adx_util.h"			/* Array Descriptor utilities. */
#include  "bmw_util.h"			/* Benchmarking functions. */
#include  "fnm_util.h"			/* Filename utilities. */
#include  "mdx_util.h"			/* Memory Descriptor utilities. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "sdx_util.h"			/* String Descriptor utilities. */
#include  "str_util.h"			/* String manipulation functions. */

/*******************************************************************************
    SING's Main Program.
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
    BmwClock  timing ;
    bool  benchmark, help ;
    char  *argument, *fileName, *pathname ;
    const  char  *operation ;
    double  elapsed,rate ;
    FILE  *file ;
    int  errflg, option ;
    OptContext  scan ;
    size_t  i, length, numLines ;
    ssize_t  count ;
    StringDx  sdx ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}",
        "{array}", "{load}",
        "{read}", "{time}",
        NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    operation = NULL ;
    fileName = NULL ;

    benchmark = false ;
    help = false ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;

    while ((option = opt_get (scan, &argument)) && !errflg) {

        switch (option) {
        case 1:				/* "-debug" */
            adx_util_debug = 1 ;
            mdx_util_debug = 1 ;
            sdx_util_debug = 1 ;
            break ;
        case 2:				/* "-help" */
            help = true ;
            break ;
        case 3:				/* "-array" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "array" ;
            break ;
        case 4:				/* "-load" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "load" ;
            break ;
        case 5:				/* "-read" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "read" ;
            break ;
        case 6:				/* "-time" */
            benchmark = true ;
            break ;
        case NONOPT:			/* "[<file>]" */
            if ((operation != NULL) && (fileName == NULL) &&
                ((strcmp (operation, "array") == 0) ||
                 (strcmp (operation, "load") == 0) ||
                 (strcmp (operation, "read") == 0))) {
                fileName = argument ;
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

    if (errflg || help || (operation == NULL)) {
        FILE  *helpout = (help) ? stdout : stderr ;
        fprintf (helpout, "Usage:  sing [-debug] [-help]\n") ;
        fprintf (helpout, "             [-array <file>] [-load <file>] [-read <file>] [-time]\n") ;
        if (!help) {
            fprintf (helpout, "Type \"sing -help\" for more detailed information.\n") ;
            exit (EINVAL) ;
        }
        fprintf (helpout, "\n") ;
        fprintf (helpout, "...\n") ;
        exit (EINVAL) ;
    }

/*******************************************************************************
    Load an entire file, line-by-line, into an array.
*******************************************************************************/

    if (strcmp (operation, "array") == 0) {

        ArrayDx  adx ;
        StringDx  *element41 ;

/* Open the input file. */

        if (fileName == NULL) {
            file = stdin ;
        } else {
            pathname = fnmBuild (FnmPath, fileName, NULL) ;
            if (pathname == NULL) {
                LGE "Error building path for \"%s\".\nfnmBuild: ", fileName) ;
                exit (errno) ;
            }
            file = fopen (pathname, "r") ;
            if (file == NULL) {
                LGE "Error opening input file: \"%s\"\nfopen: ", pathname) ;
                exit (errno) ;
            }
        }

        bmwStart (&timing) ;

        adxCreate (0, sizeof (StringDx), NULL, 0, AdxDynamic, &adx) ;

        for ( ; ; ) {			/* Break on EOF or error. */
            sdxCreate (NULL, -1, SdxVolatile, &sdx) ;
            if (sdxReadLine (file, sdx, &count) || (count < 1))  break ;
            adxAdd (adx, &sdx, 1, -1) ;
        }
        fclose (file) ;

        if (count != 0) {
            SET_ERRNO (EIO) ;
            LGE "Error reading input file.\nsdxReadLine: ") ;
            exit (errno) ;
        }

        bmwStop (&timing) ;
        elapsed = bmwElapsed (&timing) ;
        numLines = adxCount (adx) ;
        rate = (double) numLines / elapsed ;

        printf ("File: (%"PRIuSIZE_T" lines) \"%s\"\n", numLines, fileName) ;

#ifdef REWRITE_THIS
        if (benchmark)  printf ("Load time:\t%g seconds (%lu lines/second)\n",
                                elapsed, (unsigned long) (rate / 1000000.0)) ;
#endif

/* Access line 20. */

        if (numLines >= 20) {
            sdx = *((StringDx *) adxGet (adx, 20-1)) ;
            printf ("Line 20: \"%s\"\n", sdxStringZ (sdx)) ;
        }

/* Delete lines 41-45. */

        if (numLines >= 45) {
            element41 = (StringDx *) adxGet (adx, 41-1) ;
				/* Destroy descriptors being deleted
				   (or save them in cut/undo buffer). */
            for (i = 0 ;  i < 5 ;  i++) {
                sdxDestroy (element41[i]) ;
            }
            adxDelete (adx, 5, 41-1) ;
            numLines = adxCount (adx) ;
        }

/* Insert a line between line 30 and line 31. */

        if (numLines >= 31) {
            sdxCreate ("I'm the new line 31!!!", -1, SdxVolatile, &sdx) ;
            adxAdd (adx, &sdx, 1, 31-1) ;
            numLines = adxCount (adx) ;
        }

/* List out the file. */

        for (i = 0 ;  i < numLines ;  i++) {
            sdx = *((StringDx *) adxGet (adx, i)) ;
            printf ("Line %"PRIuSIZE_T": \"%s\"\n", i + 1, sdxStringZ (sdx)) ;
            sdxDestroy (sdx) ;
        }

        adxDestroy (adx) ;

    }

/*******************************************************************************
    Load an entire file, line-by-line, into a single string.
*******************************************************************************/

    else if (strcmp (operation, "load") == 0) {

/* Open the input file. */

        pathname = fnmBuild (FnmPath, fileName, NULL) ;
        if (pathname == NULL) {
            LGE "Error building path for \"%s\".\nfnmBuild: ", fileName) ;
            exit (errno) ;
        }
        file = fopen (pathname, "r") ;
        if (file == NULL) {
            LGE "Error opening input file: \"%s\"\nfopen: ", pathname) ;
            exit (errno) ;
        }

        bmwStart (&timing) ;

        sdxCreate (NULL, -1, SdxVolatile, &sdx) ;
        for ( ; ; ) {			/* Break on EOF or error. */
            if (sdxReadLine (file, sdx, &count) || (count < 1))  break ;
            sdxAddS (sdx, "\n", -1) ;	/* sdxReadLine() strips NLs. */
        }
        fclose (file) ;

        bmwStop (&timing) ;
        elapsed = bmwElapsed (&timing) ;
        length = sdxLength (sdx) ;
        rate = (double) length / elapsed ;

        if (count != 0) {
            SET_ERRNO (EIO) ;
            LGE "Error reading input file.\nsdxReadLine: ") ;
            exit (errno) ;
        }

        if (benchmark)  printf ("Load time:\t%g seconds (%lu million characters/second)\n",
                                elapsed, (unsigned long) (rate / 1000000.0)) ;
        printf ("File: (%"PRIuSIZE_T") \"%s\"\n", length, fileName) ;
        printf ("--------------------\n") ;
        fputs (sdxStringZ (sdx), stdout) ;
        printf ("--------------------\n") ;
        printf ("File: (%"PRIuSIZE_T") \"%s\"\n", length, fileName) ;
        if (benchmark)  printf ("Load time:\t%g seconds (%lu million characters/second)\n",
                                elapsed, (unsigned long) (rate / 1000000.0)) ;

        sdxDestroy (sdx) ;

    }

/*******************************************************************************
    Read an entire file, line-by-line, into separate strings.
*******************************************************************************/

    else if (strcmp (operation, "read") == 0) {

/* Open the input file. */

        pathname = fnmBuild (FnmPath, fileName, NULL) ;
        if (pathname == NULL) {
            LGE "Error building path for \"%s\".\nfnmBuild: ", fileName) ;
            exit (errno) ;
        }
        file = fopen (pathname, "r") ;
        if (file == NULL) {
            LGE "Error opening input file: \"%s\"\nfopen: ", pathname) ;
            exit (errno) ;
        }

        bmwStart (&timing) ;

        length = 0 ;

        for ( ; ; ) {			/* Break on EOF or error. */
            sdxCreate (NULL, -1, SdxVolatile, &sdx) ;
            if (sdxReadLine (file, sdx, &count) || (count < 1))  break ;
            length += sdxLength (sdx) ;
            sdxDestroy (sdx) ;
        }
        fclose (file) ;

        bmwStop (&timing) ;
        elapsed = bmwElapsed (&timing) ;
        rate = (double) length / elapsed ;

        if (count != 0) {
            SET_ERRNO (EIO) ;
            LGE "Error reading input file.\nsdxReadLine: ") ;
            exit (errno) ;
        }

        if (benchmark)  printf ("Read time:\t%g seconds (%lu million characters/second)\n",
                                elapsed, (unsigned long) (rate / 1000000.0)) ;
        printf ("File: (%"PRIuSIZE_T") \"%s\"\n", length, fileName) ;
        printf ("--------------------\n") ;
#ifdef NOT_YET
        fputs (sdxStringZ (sdx), stdout) ;
        printf ("--------------------\n") ;
        printf ("File: (%"PRIuSIZE_T") \"%s\"\n", length, fileName) ;
        if (benchmark)  printf ("Read time:\t%g seconds (%lu million characters/second)\n",
                                elapsed, (unsigned long) (rate / 1000000.0)) ;
#endif

    }


    exit (0) ;

}
