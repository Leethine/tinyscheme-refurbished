/* $Id: hls.c,v 1.3 2019/01/11 05:34:37 alex Exp alex $ */
/*******************************************************************************

Program:

    hls

    Hierarchical Directory Listing.


Author:    Alex Measday


Purpose:

    Program HLS generates a recursive directory listing.


    Invocation:

        % hls [-debug] [-directory <pathname>] [-folders] [-full]
              [-ge <numBytes>] [-le <numBytes>] [-pathname]
              <wildcardSpec>

    where

        "-debug"
            enables debug output (written to STDOUT).
        "-directory <pathname>"
            specifies the directory at which the listing is to start; the
            default is the current directory.
        "-folders"
            only lists directories.
        "-full"
            generates a full listing.
        "-ge <numBytes>"
            generates a listing of only those files whose size is greater
            than or equal to the specified number of bytes.
        "-le <numBytes>"
            generates a listing of only those files whose size is less than
            or equal to the specified number of bytes.
        "-pathname"
            causes the full pathname of each file to be displayed.
        "<wildcardSpec>"
            specifies which files are to be listed.  The standard shell
            wildcard characters are allowed, but the specification should
            be quoted to prevent its expansion by the shell.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  <sys/stat.h>			/* File status definitions. */
#include  "ftw_util.h"			/* File tree walk definitions. */
#include  "opt_util.h"			/* Option scanning definitions. */

					/* Parameters passed to file callback. */
static  bool  foldersOnly = false ;
static  bool  fullListing = false ;
static  bool  fullPathname = false ;
static  size_t  greaterEqual = 0 ;
static  size_t  lessEqual = ~0 ;


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  char  *accessString (
#    if PROTOTYPES
        mode_t  mode
#    endif
    ) ;

static  int  examineFile (
#    if PROTOTYPES
        const  char  *fullFileName,
        const  char  *fileName,
        FtwFileType  fileType,
        struct  stat  *fileInfo,
        int  level,
        void  *clientData
#    endif
    ) ;

/*******************************************************************************
    HLS's Main Program.
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

{  /* Local variables. */
    bool  help ;
    char  *argument, *directory, *s, *wildcardSpec ;
    int  errflg, option ;
    OptContext  context ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}", "{directory:}", "{folders}",
        "{full}", "{ge:}", "{le:}", "{pathname}",
        NULL
    } ;




    aperror_print = 1 ;


/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    help = false ;
    directory = "." ;  wildcardSpec = "*" ;
    foldersOnly = false ;
    fullListing = false ;  fullPathname = false ;
    greaterEqual = 0 ;  lessEqual = ~0 ;

    opt_init (argc, argv, NULL, optionList, &context) ;
    errflg = 0 ;

    while ((option = opt_get (context, &argument))) {

        switch (option) {
        case 1:			/* "-debug" */
            ftw_util_debug = 1 ;
            break ;
        case 2:			/* "-help" */
            help = true ;
            break ;
        case 3:			/* "-directory <pathname>" */
            directory = argument ;
            break ;
        case 4:			/* "-folders" */
            foldersOnly = true ;
            break ;
        case 5:			/* "-full" */
            fullListing = true ;
            break ;
        case 6:			/* "-ge <numBytes>" */
            greaterEqual = (size_t) strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 7:			/* "-le <numBytes>" */
            lessEqual =  (size_t) strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 8:			/* "-pathname" */
            fullPathname = true ;
            break ;
        case NONOPT:		/* "<wildcardSpec>" */
            wildcardSpec = argument ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (context) ;

    if (errflg || help) {
        fprintf (stderr, "Usage:  hls [-debug] [-help]\n") ;
        fprintf (stderr, "            [-directory <pathname>] [-folders] [-full]\n") ;
        fprintf (stderr, "            [-ge <numBytes>] [-le <numBytes>] [-pathname]\n") ;
        fprintf (stderr, "            <wildcardSpec>\n") ;
        fprintf (stderr, "where:\n") ;
        fprintf (stderr, "    -directory <pathname>\n") ;
        fprintf (stderr, "        specifies the root directory for the listing (default: current).\n") ;
        fprintf (stderr, "    -folders\n") ;
        fprintf (stderr, "        lists only directories.\n") ;
        fprintf (stderr, "    -full\n") ;
        fprintf (stderr, "        generates a full listing.\n") ;
        fprintf (stderr, "    -ge <numBytes>\n") ;
        fprintf (stderr, "        list only files greater than or equal to the specified size.\n") ;
        fprintf (stderr, "    -less <numBytes>\n") ;
        fprintf (stderr, "        list only files less than or equal to the specified size.\n") ;
        fprintf (stderr, "    -pathname\n") ;
        fprintf (stderr, "        prints the full pathname of each file.\n") ;
        fprintf (stderr, "    <wildcardSpec>\n") ;
        fprintf (stderr, "        limits the output to files whose names match a wildcard pattern;\n") ;
        fprintf (stderr, "        this specification should be quoted to prevent shell expansion.\n") ;
        exit (EINVAL) ;
    }


/*******************************************************************************
    List the files in the directory.
*******************************************************************************/

    exit (fileTreeWalk (directory, wildcardSpec, FtwPhysical,
                        examineFile, NULL)) ;

}

/*******************************************************************************

Procedure:

    accessString ()

    Convert Access Modes to String.


Purpose:

    Function accessString() returns a string showing a file's access modes.


    Invocation:

        string = accessString (mode) ;

    where:

        <mode>		- I
            is the mode field from the file's STAT(2) structure.
        <string>	- O
            returns a string containing the ASCII representation of the
            access modes.  The string is stored in memory local to this
            function and the string should be used or duplicated before
            calling accessString() again.

*******************************************************************************/


static  char  *accessString (

#    if  __STDC__
        mode_t  mode)
#    else
        mode)

        mode_t  mode ;
#    endif

{    /* Local variables. */
    static  char  string[11] ;



    if (S_ISDIR (mode))
        string[0] = 'd' ;
#ifdef S_ISLNK
    else if (S_ISLNK (mode))
        string[0] = 'l' ;
#endif
    else
        string[0] = '-' ;

    string[1] = (mode & S_IRUSR) ? 'r' : '-' ;
    string[2] = (mode & S_IWUSR) ? 'w' : '-' ;
    string[3] = (mode & S_IXUSR) ? 'x' : '-' ;

    string[4] = (mode & S_IRGRP) ? 'r' : '-' ;
    string[5] = (mode & S_IWGRP) ? 'w' : '-' ;
    string[6] = (mode & S_IXGRP) ? 'x' : '-' ;

    string[7] = (mode & S_IROTH) ? 'r' : '-' ;
    string[8] = (mode & S_IWOTH) ? 'w' : '-' ;
    string[9] = (mode & S_IXOTH) ? 'x' : '-' ;

    return (string) ;

}

/*******************************************************************************

Procedure:

    examineFile ()

    Examine a File Entry.


Purpose:

    Function examineFile(), a callback function invoked for each file
    encountered in the directory hierarchy, examines the directory
    entry for a file.


    Invocation:

        status = examineFile (fullFileName, fileName, fileType,
                              fileInfo, level, clientData) ;

    where:

        <fullFileName>	- I
            is the full pathname for the file.
        <fileName>	- I
            is the file's name, excluding the directory.
        <fileType>	- I
            is the file type; see the FtwFileType enumeration in "ftw_util.h".
        <fileInfo>	- I
            is the system "stat" structure for the file; see STAT(2).
        <level>		- I
            is the level in the directory hierarchy at which the file is found.
        <clientData>		- I
            is the (VOID *) pointer specified in the call to fileTreeWalk().
        <status>		- O
            returns the status of examining the file entry, zero if no errors
            occurred and ERRNO otherwise.

*******************************************************************************/


static  int  examineFile (

#    if  __STDC__
        const  char  *fullFileName,
        const  char  *fileName,
        FtwFileType fileType,
        struct  stat  *fileInfo,
        int  level,
        void  *clientData)
#    else
        fullFileName, fileName, fileType, fileInfo, level, clientData)

        char  *fullFileName ;
        char  *fileName ;
        FtwFileType fileType ;
        struct  stat  *fileInfo ;
        int  level ;
        void  *clientData ;
#    endif

{

/* If only directories are being displayed and the file is not a directory,
   then ignore the file. */

    if (foldersOnly && (fileType != FtwDirectory))  return (0) ;

/* If the file is a regular file and its size is not within the size limits
   specified by the user, then ignore the file. */

    if ((fileType == FtwFile) &&
        ((fileInfo->st_size <= greaterEqual) ||
         (lessEqual <= fileInfo->st_size))) {
        return (0) ;
    }

/* Indent the listing according to the depth within the directory tree. */

    if (!fullListing && !fullPathname) {
        while (level-- > 0)
            printf ("    ") ;
    }

/* If a full listing was requested, then display the additional information. */

    if (fullListing) {
        switch (fileType) {
        case FtwDirectory:
            printf ("\n") ;
        case FtwDirNoRead:
        case FtwFile:
        case FtwSymbolicLink:
            printf ("%s  %12lu  ",
                    accessString (fileInfo->st_mode),
                    (unsigned long) fileInfo->st_size) ;
            break ;
        default:
            break ;
        }
    }

/* List the file. */

    switch (fileType) {
    case FtwDirectory:
        printf ("%s/\n",
                (fullListing || fullPathname) ? fullFileName : fileName) ;
        break ;
    case FtwDirNoRead:
        printf ("%s/ permission denied\n", fileName) ;
        break ;
    case FtwNoStat:
        printf ("%s inscrutable\n", fileName) ;
        break ;
    case FtwFile:
        printf ("%s\n", fullPathname ? fullFileName : fileName) ;
        break ;
    case FtwSymbolicLink:
        printf ("%s@\n", fullPathname ? fullFileName : fileName) ;
        break ;
    case FtwVisited:
        printf ("%s visited\n", fileName) ;
        break ;
    default:
        printf ("%s ?\n", fullPathname ? fullFileName : fileName) ;
        break ;
    }

    return (0) ;

}
