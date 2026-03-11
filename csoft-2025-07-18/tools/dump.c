/* $Id: dump.c,v 1.6 2019/01/11 05:34:37 alex Exp alex $ */
/*******************************************************************************

    dump.c

    File Dump Utility.


    Alex Measday


    DUMP dumps the contents of a file to the standard output.  The data is
    represented in two ways in the output, as numerical data dumped in a
    user-specified format and as character data dumped in a string:

        <address>: <data in user-specified format> "<data as a string>"

    For example, here's the dump (in the default hex-ASCII format) of an
    MP3 file of Eva Cassidy performing Sting's "Fields Of Gold":

        00000000:  49443303 00000004 14155441 4C420000  "ID3.......TALB.."
        00000010:  002D0000 01FFFE4E 00690067 00680074  ".-.....N.i.g.h.t"
        00000020:  00620069 00720064 00200031 00390039  ".b.i.r.d. .1.9.9"
        00000030:  00360020 00280043 00440031 00290000  ".6. .(.C.D.1.).."
        00000040:  00545045 31000000 1B000001 FFFE4500  ".TPE1.........E."
        00000050:  76006100 20004300 61007300 73006900  "v.a. .C.a.s.s.i."
        00000060:  64007900 00005443 4F4E0000 000F0000  "d.y...TCON......"
        00000070:  01FFFE56 006F0063 0061006C 00000054  "...V.o.c.a.l...T"
        00000080:  49543200 00002100 0001FFFE 46006900  "IT2...!.....F.i."
        00000090:  65006C00 64007300 20004F00 66002000  "e.l.d.s. .O.f. ."
        000000A0:  47006F00 6C006400 00005452 434B0000  "G.o.l.d...TRCK.."
        000000B0:  00040000 00303400 54594552 00000006  ".....04.TYER...."
        000000C0:  00000032 30313500 41504943 0001014D  "...2015.APIC...M"
        ... plus almost 7 million more 192kbps bytes to dump ...

    In the ASCII dump portion, non-printable characters are replaced
    by periods.  In this IDv2.3 MP3 tag, text fields use little-endian
    UTF-16 representation; ASCII characters in UTF-16 have a zero upper
    byte, thus producing the alternating characters and periods; e.g.,
    "F.i.e.l.d.s. .O.f. .G.o.l.d.".


    Invocation:

        % dump [-help]
               [-decimal] [-hexadecimal] [-octal]
               [-ebcdic] [-text]
               [-skip <#bytes>] [-number <#bytes>]
               [-width <#bytes>]
               [<inputFile>]

    where:

        "-help"
            displays help information.
        "-decimal"
            dumps each byte as a decimal number.
        "-hexadecimal"
            dumps every 4 bytes as a hexadecimal number.
        "-octal"
            dumps each byte as an octal number.
        "-ebcdic"
            specifies that the input data is EBCDIC.  The data is converted
            to ASCII before being dumped.  NOTE that this option affects
            both numeric dumps and "-text" dumps.
        "-text"
            dumps each byte as text, i.e., the data is not dumped in numeric
            form.  Each output line will appear as:
                      <address>: "<data as a string>"
        "-skip <#bytes>"
            specifies the number of bytes (0..N-1) in the input file to skip
            before starting to dump the data.
        "-number <#bytes>"
            specifies the number of bytes to dump.
        "-width <#bytes>"
            changes the number of bytes (width) of input data dumped on each
            line of output.
        <inputFile>
            is the file to be dumped.  If this argument is not specified,
            input is taken from standard input.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "meo_util.h"			/* Memory operations. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "str_util.h"			/* String manipulation functions. */

#define  MAX_LENGTH (128*1024)
#define  MAX_STRING  256

#define  ASCII  'A'			/* Input data modes. */
#define  EBCDIC  'E'

/*******************************************************************************
    Main Program
*******************************************************************************/


#ifdef VXWORKS

int  dump (

#    if PROTOTYPES
        char  *commandLine)
#    else
        commandLine)

        char  *commandLine ;
#    endif

#else

int  main (

#    if PROTOTYPES
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

#endif

{    /* Local variables. */
    bool  help ;
    char  *argument, buffer[MAX_STRING] ;
    char  *inputFile, *outputFile ;
    int  dumpMode, errflg, inputMode, length ;
    int  numBytesToDump, numBytesPerLine, offset, option ;
    FILE  *infile, *outfile ;
    long  address ;
    OptContext  scan ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{help}", "{decimal}", "{ebcdic}", "{hexadecimal}",
        "{number:}", "{octal}", "{skip:}", "{text}", "{width:}",
        NULL
    } ;




#ifdef VXWORKS
    char  **argv ;
    int  argc ;
		/* Parse command string into an ARGC/ARGV array of arguments. */
    opt_create_argv ("dump", commandLine, &argc, &argv) ;
#endif


    aperror_print = 1 ;


/*******************************************************************************
  Scan the command line options.
*******************************************************************************/

    help = false ;
    dumpMode = MeoHexadecimal ;  inputMode = ASCII ;
    numBytesPerLine = 0 ;  numBytesToDump = -1 ;  offset = 0 ;
    inputFile = NULL ;  outputFile = NULL ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;

    while ((option = opt_get (scan, &argument))) {

        switch (option) {
        case 1:			/* "-help" */
            help = true ;
            break ;
        case 2:			/* "-decimal" */
            dumpMode = MeoDecimal ;
            break ;
        case 3:			/* "-ebcdic" */
            inputMode = EBCDIC ;
            break ;
        case 4:			/* "-hexadecimal" */
            dumpMode = MeoHexadecimal ;
            break ;
        case 5:			/* "-number <#bytes>" */
            numBytesToDump = atoi (argument) ;
            break ;
        case 6:			/* "-octal" */
            dumpMode = MeoOctal ;
            break ;
        case 7:			/* "-skip <#bytes>" */
            offset = atoi (argument) ;
            break ;
        case 8:			/* "-text" */
            dumpMode = MeoText ;
            break ;
        case 9:			/* "-width <#bytes>" */
            numBytesPerLine = atoi (argument) ;
            break ;
        case NONOPT:
            if (inputFile == NULL)  inputFile = argument ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (scan) ;

    if (errflg || help) {
        fprintf (stderr, "Usage:  dump [-help]\n") ;
        fprintf (stderr, "             [-decimal] [-hexadecimal] [-octal]\n") ;
        fprintf (stderr, "             [-ebcdic] [-text]\n") ;
        fprintf (stderr, "             [-skip <#bytes>] [-number <#bytes>]\n") ;
        fprintf (stderr, "             [-width <#bytes>]\n") ;
        fprintf (stderr, "             [<inputFile>]\n") ;
        if (!help) {
            fprintf (stderr, "Type \"dump -help\" for more detailed information.\n") ;
            exit (EINVAL) ;
        }
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    DUMP formats and dumps the numeric values of the bytes in an arbitrary\n") ;
        fprintf (stderr, "    file.  Byte values can be displayed in octal, decimal, or hexadecimal\n") ;
        fprintf (stderr, "    form.  In the octal and decimal formats, each byte value is separated\n") ;
        fprintf (stderr, "    from the adjacent value by a space.  In hexadecimal format, 4 sequential\n") ;
        fprintf (stderr, "    bytes (i.e., 32 bits) are grouped together and displayed as an 8-character\n") ;
        fprintf (stderr, "    hexadecimal string with no intervening spaces.  Hexadecimal format is the\n") ;
        fprintf (stderr, "    default.\n") ;
        exit (-1) ;
    }

/* Set the number of bytes dumped on each line of output. */

    if (numBytesPerLine <= 0) {
        switch (dumpMode) {
        case MeoDecimal:      numBytesPerLine = 8 ;  break ;
        case MeoHexadecimal:  numBytesPerLine = 16 ;  break ;
        case MeoOctal:        numBytesPerLine = 8 ;  break ;
        case MeoText:         numBytesPerLine = 40 ;  break ;
        default:              break ;
        }
    }

/*******************************************************************************

  Read and dump the input file.

*******************************************************************************/

    if (outputFile == NULL) {
        outputFile = "<stdout>" ;
        outfile = stdout ;
    } else if (NULL == (outfile = fopen (outputFile, "w"))) {
        LGE "(dump) Error opening output file: %s\nfopen: ", outputFile) ;
        exit (errno) ;
    }

    if (inputFile == NULL) {
        inputFile = "<stdin>" ;
        infile = stdin ;
    } else if (NULL == (infile = fopen (inputFile, "rb"))) {
        LGE "(dump) Error opening input file: %s\nfopen: ", inputFile) ;
        exit (errno) ;
    }

    fprintf (outfile, "Input File: \"%s\"\n\n", inputFile) ;

    if (offset > 0) {
        if (fseek (infile, offset, 0) != 0) {
            LGE "(dump) Error positioning to byte %d of the input file.\nfseek: ",
                offset) ;
            exit (errno) ;
        }
        address = ftell (infile) ;
    } else {
        address = 0 ;
    }

    memset (buffer, '\0', sizeof buffer) ;
    while (0 < (length = fread (buffer, 1, numBytesPerLine, infile))) {
        if (inputMode == EBCDIC)  strEtoA (buffer, length) ;
        meoDump (outfile, NULL, dumpMode, numBytesPerLine,
                 (void *) address, buffer, length) ;
        address = address + length ;
        if (numBytesToDump != -1) {
            numBytesToDump = numBytesToDump - length ;
            if (numBytesToDump <= 0)  break ;	/* Dumped enough bytes? */
        }
        memset (buffer, '\0', sizeof buffer) ;
    }


    exit (0) ;

}
