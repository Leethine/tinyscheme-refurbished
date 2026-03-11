/* $Id: strip8.c,v 1.4 2021/04/21 12:02:21 alex Exp $ */
/*******************************************************************************

Program:

    strip8.c

    Mask Each Byte in Files.


Author:    Alex Measday


Purpose:

    STRIP8 applies the same AND/OR masks to each byte in one or more
    files and outputs the result to standard output.  STRIP8 was written
    to strip the high bit from each character in a file for the purpose
    of converting WordStar files (which have the high bit set in the
    last character of each word) to 7-bit ASCII.  It was simple to
    generalize it to arbitrary masking of each byte:

        output-byte = (input-byte AND and-mask) OR or-mask

    If neither an AND mask nor an OR mask are specified on the command line,
    STRIP8 assumes it is processing a WordStar file.  In order to strip the
    high bit from the input characters, the AND mask is automatically set
    to 0x7F and the OR mask to 0x00.  STRIP8 will also respect the first
    WordStar end-of-file character (0x1A) it encounters and cease processing
    the current input file.


    Invocation:

        % strip8 [-help] [-and <mask>] [-or <mask>] [<file(s)>]

    where:

        "-help"
            prints out a help message.
        "-and <mask>"
            specifies a number in the range 0 to 255 to be ANDed with each
            input byte.  The default mask is 0x7F, which strips the most
            significant bit from each input byte.  The AND operation is
            performed before the OR operation.  If neither mask is specified,
            STRIP8 assumes the input is a WordStar file.
        "-or <mask>"
            specifies a number in the range 0 to 255 to be ORed with the
            result of ANDing the input byte with the AND mask.  The default
            mask is 0x00, which leaves the ANDed result unchanged.  The AND
            operation is performed before the OR operation.  If neither mask
            is specified, STRIP8 assumes the input is a WordStar file.
        "<file(s)>"
            specifies one or more files whose contents will be masked.
            If no files are specified, STRIP8 reads the bytes from
            standard input (useful in a pipeline).

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "str_util.h"			/* String manipulation functions. */

#define  WORDSTAR_EOF  0x1A		/* WordStar end-of-file character. */
#define  WORDSTAR_SOFT_CR  0x8D		/* WordStar soft carriage return. */
#define  WORDSTAR_SOFT_SPACE  0xA0	/* WordStar soft space. */

/*******************************************************************************
    processFile() - process an input file.
*******************************************************************************/

errno_t  processFile (

#    if PROTOTYPES
        const  char  *fileName,
        unsigned  char  andMask,
        unsigned  char  orMask,
        bool  wordstar)
#    else
        fileName, andMask, orMask, wordstar)

        char  *fileName ;
        unsigned  char  andMask ;
        unsigned  char  orMask ;
        bool  wordstar ;
#    endif

{    /* Local variables. */
    FILE  *inFile ;
    size_t  i, length ;
    unsigned  char  buffer[8192] ;



/* Open the input file. */

    if (fileName == NULL) {
        inFile = stdin ;
    } else {
        inFile = fopen (fileName, "rb") ;
        if (inFile == NULL) {
            perror (fileName) ;
            exit (errno) ;
        }
    }

/* Copy the input file to standard output while applying the AND-/OR-masking
   to each byte. */

    while (!feof (inFile)) {
        length = fread (buffer, 1, sizeof buffer, inFile) ;
        if (length == 0) {
            if (feof (inFile))  break ;
            if (ferror (inFile)) {
                perror ("fread") ;
                exit (errno) ;
            }
        }
        for (i = 0 ;  i < length ;  i++) {
            if (wordstar && (buffer[i] == WORDSTAR_EOF)) {
                length = i ;  break ;	/* Cease processing on WordStar EOF. */
            }
            buffer[i] = (buffer[i] & andMask) | orMask ;
        }
        if (fwrite (buffer, 1, length, stdout) != length) {
            perror ("fwrite") ;
            exit (errno) ;
        }
    }

    fclose (inFile) ;

    if (wordstar) {		/* Write final CR/LF. */
#ifdef _WIN32
        printf ("\n") ;		/* Windows generates CR/LF. */
#else
        printf ("\r\n") ;	/* Must be explicit on most other systems. */
#endif
    }

    return (0) ;

}

/*******************************************************************************
    STRIP8's Main Program.
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
    bool  help, wordstar ;
    char  *argument, *s ;
    int  errflg, option ;
    OptContext  scan ;
    size_t  numFiles ;
    unsigned  char  andMask, orMask ;
    unsigned  long  number ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{help}", "{and:}", "{or:}", NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    help = false ;
    wordstar = true ;				/* Assume WordStar input. */
    andMask = 0x7F ;  orMask = 0x00 ;
    numFiles = 0 ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;

    while ((option = opt_get (scan, &argument))) {

        switch (option) {
        case 1:				/* "-help" */
            help = true ;
            break ;
        case 2:				/* "-and <mask>" */
            wordstar = false ;
            number = strtoul (argument, &s, 0) ;
            if ((*s == '\0') && (number < 256))
                andMask = (unsigned char) number ;
            else
                errflg++ ;
            break ;
        case 3:				/* "-or <mask>" */
            wordstar = false ;
            number = strtoul (argument, &s, 0) ;
            if ((*s == '\0') && (number < 256))
                orMask = (unsigned char) number ;
            else
                errflg++ ;
            break ;
        case NONOPT:
            numFiles++ ;
            processFile (argument, andMask, orMask, wordstar) ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (scan) ;

    if (errflg || help) {
        fprintf (stderr, "Usage:  strip8 [-help] [-and <mask>] [-or <mask>] [<file(s)>]\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "The AND and OR masks, if supplied, must be between 0 and 255.\n") ;
        fprintf (stderr, "The default AND mask is 0x7F (strips the high bit); the default\n") ;
        fprintf (stderr, "OR mask is 0x00 (no change).  These are the masks needed for\n") ;
        fprintf (stderr, "WordStar input files.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "If neither of the masks is specified, the input is assumed to be a\n") ;
        fprintf (stderr, "WordStar file: the high bit will be cleared on each byte and input\n") ;
        fprintf (stderr, "will be terminated when the WordStar end-of-file character (0x1A)\n") ;
        fprintf (stderr, "is encountered.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "If either of the masks is specified, the WordStar assumption is\n") ;
        fprintf (stderr, "dropped and the input will simply have the AND and OR operations\n") ;
        fprintf (stderr, "applied, in that order.  The WordStar end-of-file character\n") ;
        fprintf (stderr, "receives no special treatment.\n") ;
        exit (EINVAL) ;
    }

    if (numFiles == 0) {	/* No files?  Read from standard input. */
        processFile (NULL, andMask, orMask, wordstar) ;
    }

    exit (0) ;

}
