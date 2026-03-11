/* $Id: drs_util.c,v 1.19 2023/10/27 02:19:13 alex Exp $ */
/*******************************************************************************

File:

    drs_util.c

    Directory Scanning Utilities.


Author:    Alex Measday


Purpose:

    The functions in the DRS_UTIL package are used to scan the names of the
    files in a directory.  Wildcards can be used to filter out unwanted files.

    The following example prints out the names of the ".c" files in a
    directory:

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  "drs_util.h"		-- Directory scanning utilities.
        ...
        int  main (int argc, char *argv[])
        {
            char  *fileName ;
            DirectoryScan  scan ;

            drsCreate ("*.c", &scan) ;
            fileName = drsFirst (scan) ;
            while (fileName != NULL) {
                printf ("C File: %s\n", fileName) ;
                fileName = drsNext (scan) ;
            }
            drsDestroy (scan) ;
        }

    Alternatively, you can call drsGet() to get the I-th name in the directory:

            ...
            for (i = 0 ;  i < drsCount (scan) ;  i++)
                printf ("C File: %s\n", drsGet (scan, i)) ;
            ...


Notes:

    This package is derived from and supersedes my FSEARCH() routine.
    Farewell, VMS!


Public Procedures:

    drsCount() - returns the number of files in a directory scan.
    drsCreate() - creates a directory scan.
    drsDestroy() - destroys a directory scan.
    drsFirst() - gets the first entry in the directory.
    drsGet() - gets the I-th entry in the directory.
    drsNext() - gets the next entry in the directory.

Private Procedures:

    drsCompare() - compares two file names for sorting purposes.
    drsErase() - erases the contents of a directory scan structure.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */

#if defined(VMS)
#    include  <nam.h>			/* RMS name block definitions. */
#    include  <rmsdef.h>		/* RMS completion codes. */
#elif defined(_WIN32)
#    include  <winbase.h>		/* Directory scanning functions. */
#    include  <direct.h>		/* Directory manipulation functions. */
#else
#    include  <unistd.h>		/* UNIX-specific definitions. */
#    include  <dirent.h>		/* Directory entry definitions. */
#endif

#include  "fnm_util.h"			/* Filename utilities. */
#include  "rex_util.h"			/* Regular expression definitions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "drs_util.h"			/* Directory scanning utilities. */


/*******************************************************************************
    Directory Scan - contains information about a directory scan.
*******************************************************************************/

typedef  struct  _DirectoryScan {
    char  *pathname ;			/* Original wildcard pathname. */
    char  *dirname ;			/* Directory from pathname. */
#if defined(VMS)
    unsigned  long  directory ;		/* Directory handle. */
#elif defined(_WIN32)
    HANDLE  directory ;			/* Directory handle. */
#else
    DIR  *directory ;			/* Directory handle. */
#endif
    CompiledRE  compiledRE ;		/* Compiled wildcard specification. */
    size_t  numFiles ;			/* # of matching file names. */
    char  **fileName ;			/* List of matching file names. */
    size_t  nextFile ;			/* Index of next file in scan. */
}  _DirectoryScan ;


int  drs_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  drs_util_debug


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  int  drsCompare (
#    if PROTOTYPES
        const  void  *p1,
        const  void  *p2
#    endif
    ) ;

static  errno_t  drsErase (
#    if PROTOTYPES
        DirectoryScan  scan,
        bool  complete
#    endif
    ) ;

/*!*****************************************************************************

Procedure:

    drsCount ()

    Get the Number of Files in a Directory Scan.


Purpose:

    Function drsCount() returns the number of files in a directory scan that
    matched the wildcard file specification.


    Invocation:

        numFiles = drsCount (scan) ;

    where:

        <scan>	- I
            is the directory scan handle returned by drsCreate().
        <count>	- O
            returns the number of files that matched the wildcard file
            specification.

*******************************************************************************/


size_t  drsCount (

#    if PROTOTYPES
        DirectoryScan  scan)
#    else
        scan)

        DirectoryScan  scan ;
#    endif

{

    if (scan == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(drsCount) NULL scan handle: ") ;
        return (0) ;
    }

    return (scan->numFiles) ;

}

/*!*****************************************************************************

Procedure:

    drsCreate ()

    Create a Directory Scan.


Purpose:

    Function drsCreate() creates a directory scan.


    Invocation:

        status = drsCreate (pathname, &scan) ;

    where:

        <pathname>	- I
            is the wildcard specification for the directory being scanned.
        <scan>		- O
            returns a handle for the directory scan.  This handle is used
            in calls to the other DRS functions.
        <status>	- O
            returns the status of initiating the directory scan, zero if
            no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  drsCreate (

#    if PROTOTYPES
        const  char  *pathname,
        DirectoryScan  *scan)
#    else
        pathname, scan)

        char  *pathname ;
        DirectoryScan  *scan ;
#    endif

{    /* Local variables. */
    char  *fileSpec, *s ;
    size_t  maxFiles = 0 ;
#if defined(VMS)
    char  result[NAM$C_MAXRSS+1] ;
    $DESCRIPTOR (result_dsc, result) ;
    struct  dsc$descriptor_s  wildcard_dsc ;
    unsigned  long  status ;
#elif defined(_WIN32)
    WIN32_FIND_DATA  fileInfo ;
#else
    char  *wildcardRE ;
    struct  dirent  *d ;
#endif




/* Create the directory scan context structure. */

    *scan = (_DirectoryScan *) malloc (sizeof (_DirectoryScan)) ;
    if (*scan == NULL) {
        LGE "(drsCreate) Error allocating scan context for %s.\nmalloc: ",
            pathname) ;
        return (errno) ;
    }

    (*scan)->dirname = NULL ;
#if defined(VMS)
    (*scan)->directory = 0 ;
#elif defined(_WIN32)
    (*scan)->directory = INVALID_HANDLE_VALUE ;
#else
    (*scan)->directory = NULL ;
#endif
    (*scan)->compiledRE = NULL ;
    (*scan)->numFiles = 0 ;
    (*scan)->fileName = NULL ;
    (*scan)->nextFile = 0 ;

    (*scan)->pathname = fnmEnv (pathname, -1) ;
    if ((*scan)->pathname == NULL) {
        LGE "(drsCreate) Error translating pathname: %s\nfnmEnv: ", pathname) ;
        PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Separate the directory name from the wildcard file specification. */

    s = strrchr ((*scan)->pathname, '/') ;		/* Directory name. */
    if (s == NULL)  s = strrchr ((*scan)->pathname, '\\') ;
    if (s == NULL) {
        (*scan)->dirname = fnmGetCWD (false) ;
        if ((*scan)->dirname == NULL) {
            LGE "(drsCreate) Error getting current working directory.\nfnmGetCWD: ") ;
            PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {
        (*scan)->dirname = strdup ((*scan)->pathname) ;
        if ((*scan)->dirname == NULL) {
            LGE "(drsCreate) Error duplicating directory name.\nstrdup: ") ;
            PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
            return (errno) ;
        }
        (*scan)->dirname[s-(*scan)->pathname] = '\0' ;
    }

    fileSpec = strrchr ((*scan)->pathname, '/') ;	/* File specification. */
    if (fileSpec == NULL)  fileSpec = strrchr ((*scan)->pathname, '\\') ;
    if (fileSpec == NULL)
        fileSpec = (*scan)->pathname ;
    else
        fileSpec++ ;

/*******************************************************************************
    Under VMS, use the system functions to scan the wildcard-selected files.
*******************************************************************************/

#if defined(VMS)

/* Get the first file. */

    ASSIGN (wildcard_dsc, (*scan)->pathname) ;
    result[(sizeof result)-1] = '\0' ;

    status = LIB$FIND_FILE (&wildcard_dsc, &result_dsc, &(*scan)->directory,
                            NULL, NULL, NULL, NULL) ;
    if (!(status & STS$M_SUCCESS)) {
        SET_ERRNO (EVMSERR) ;  vaxc$errno = status ;
        if (status == RMS$_NMF) {				/* No files? */
            LGI "(drsCreate) %s - %d files.\n",
                (*scan)->pathname, (*scan)->numFiles) ;
            return (0) ;
        } else {						/* Error. */
            LGE "(drsCreate) Error opening directory: %s\nLIB$FIND_FILE: ",
                (*scan)->pathname) ;
            PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
            vaxc$errno = status ;
            return (errno) ;
        }
    }

/* Construct the list of files. */

    do {

        char  *s = strchr (result, ' ') ;
        if (s != NULL)  *s = '\0' ;			/* Trim space fill. */

        if ((*scan)->numFiles >= maxFiles) {		/* Expand list? */
            size_t  newMax = maxFiles + 128 ;
            size_t  newSize = newMax * sizeof (char *) ;
            char  **names =
                ((*scan)->fileName == NULL)
                ? (char **) malloc (newSize)
                : (char **) realloc ((*scan)->fileName, newSize) ;
            if (names == NULL) {
                LGE "(drsCreate) Error expanding %s list to %lu files.\nrealloc: ",
                    (*scan)->pathname, (unsigned long) maxFiles) ;
                PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
                return (errno) ;
            }
            (*scan)->fileName = names ;
            maxFiles = newMax ;
        }
							/* Add new file. */
        (*scan)->fileName[(*scan)->numFiles] = strdup (result) ;
        if ((*scan)->fileName[(*scan)->numFiles] == NULL) {
            LGE "(drsCreate) Error duplicating file name: %s\nstrdup: ",
                result) ;
            PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
            return (errno) ;
        }
        (*scan)->numFiles++ ;

        status = LIB$FIND_FILE (&wildcard_dsc, &result_dsc, &(*scan)->directory,
                                NULL, NULL, NULL, NULL) ;

    } while (status & STS$M_SUCCESS) ;

/*******************************************************************************
    Under Windows, use the system functions to scan the wildcard-selected
    files.
*******************************************************************************/

#elif defined(_WIN32)

/* Get the first file.  There are several Windows error codes relating to
   "file not found"; since that seems like kind of a moving target, treat
   any non-success status as "file not found", even OS errors. */

    (*scan)->directory = FindFirstFile ((*scan)->pathname, &fileInfo) ;
    if ((*scan)->directory == INVALID_HANDLE_VALUE) {		/* No files? */
        LGI "(drsCreate) %s - %lu files.\n",
            (*scan)->pathname, (unsigned long) (*scan)->numFiles) ;
        return (0) ;
    }

/* Construct the list of files. */

    do {

        if ((strcmp (fileInfo.cFileName, ".") == 0) ||
            (strcmp (fileInfo.cFileName, "..") == 0))
            continue ;		/* Ignore current and parent entries. */

        if ((*scan)->numFiles >= maxFiles) {		/* Expand list? */
            size_t  newMax = maxFiles + 128 ;
            size_t  newSize = newMax * sizeof (char *) ;
            char  **names =
                ((*scan)->fileName == NULL)
                ? (char **) malloc (newSize)
                : (char **) realloc ((*scan)->fileName, newSize) ;
            if (names == NULL) {
                LGE "(drsCreate) Error expanding %s list to %lu files.\nrealloc: ",
                    (*scan)->pathname, (unsigned long) maxFiles) ;
                PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
                return (errno) ;
            }
            (*scan)->fileName = names ;
            maxFiles = newMax ;
        }
							/* Add new file. */
        (*scan)->fileName[(*scan)->numFiles] =
            strDupCat (-1, (*scan)->dirname, "/", fileInfo.cFileName, NULL) ;
        if ((*scan)->fileName[(*scan)->numFiles] == NULL) {
            LGE "(drsCreate) Error duplicating file name: \"%s/%s\"\nstrDupCat: ",
                (*scan)->dirname, fileInfo.cFileName) ;
            PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
            return (errno) ;
        }
        (*scan)->numFiles++ ;

    } while (FindNextFile ((*scan)->directory, &fileInfo)) ;

/*******************************************************************************
    Under UNIX, compile and use a regular expression to match the wildcard
    specification against the files in the directory.
*******************************************************************************/

#else

/* Convert the wildcard file specification to a regular expression string. */

    wildcardRE = rexWild (fileSpec) ;
    if (wildcardRE == NULL) {
        LGE "(drsCreate) Error converting \"%s\" to a wildcard RE.\nrexWild: ",
            fileSpec) ;
        PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Compile a regular expression (RE) for the wildcard file specification. */

    if (rexCompile (wildcardRE, &(*scan)->compiledRE)) {
        LGE "(drsCreate) Error compiling regular expression for \"%s\".\nrexCompile: ",
            pathname) ;
        PUSH_ERRNO ;  free (wildcardRE) ;  drsDestroy (*scan) ;  POP_ERRNO ;
        return (errno) ;
    }

    LGI "(drsCreate) Wildcard: \"%s\"  RE: \"%s\"\n",
        fileSpec, wildcardRE) ;

    free (wildcardRE) ;

/* Open the directory for scanning. */

    (*scan)->directory = opendir ((*scan)->dirname) ;
    if ((*scan)->directory == NULL) {
        LGE "(drsCreate) Error opening directory: %s\nopendir: ",
            (*scan)->dirname) ;
        PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Construct the list of files whose names match the wildcard specification. */

    (*scan)->numFiles = 0 ;
    while ((d = readdir ((*scan)->directory)) != NULL) {

        if ((strcmp (d->d_name, ".") == 0) || (strcmp (d->d_name, "..") == 0))
            continue ;		/* Ignore current and parent entries. */

        if (!rexMatch (d->d_name, -1, (*scan)->compiledRE, NULL, NULL, 0)) {
            LGI "(drsCreate)    \"%s\"\n", d->d_name) ;
            continue ;		/* File not selected by wildcard. */
        }

        LGI "(drsCreate) => \"%s\"\n", d->d_name) ;

        if ((*scan)->numFiles >= maxFiles) {		/* Expand list? */
            size_t  newMax = maxFiles + 128 ;
            size_t  newSize = newMax * sizeof (char *) ;
            char  **names =
                ((*scan)->fileName == NULL)
                ? (char **) malloc (newSize)
                : (char **) realloc ((*scan)->fileName, newSize) ;
            if (names == NULL) {
                LGE "(drsCreate) Error expanding %s list to %lu files.\nrealloc: ",
                    (*scan)->pathname, (unsigned long) maxFiles) ;
                PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
                return (errno) ;
            }
            (*scan)->fileName = names ;
            maxFiles = newMax ;
        }
							/* Add new file. */
        (*scan)->fileName[(*scan)->numFiles] =
            strDupCat (-1, (*scan)->dirname, "/", d->d_name, NULL) ;
        if ((*scan)->fileName[(*scan)->numFiles] == NULL) {
            LGE "(drsCreate) Error duplicating file name: \"%s/%s\"\nstrDupCat: ",
                (*scan)->dirname, d->d_name) ;
            PUSH_ERRNO ;  drsDestroy (*scan) ;  POP_ERRNO ;
            return (errno) ;
        }
        (*scan)->numFiles++ ;

    }

#endif

/*******************************************************************************
    Depending on the file-system type, the file names may not be in
    alphabetical order.  Sort them.
*******************************************************************************/

#ifndef VMS
    qsort ((*scan)->fileName, (*scan)->numFiles, sizeof (char *), drsCompare) ;
#endif


/* Close the system resources used to access the directory. */

    drsErase (*scan, false) ;


    LGI "(drsCreate) %s - %lu files.\n",
        (*scan)->pathname, (unsigned long) (*scan)->numFiles) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    drsDestroy ()

    Destroy a Directory Scan.


Purpose:

    Function drsDestroy() destroys a directory scan.


    Invocation:

        status = drsDestroy (scan) ;

    where:

        <scan>		- I
            is the directory scan handle returned by drsCreate().
        <status>	- O
            returns the status of terminating the directory scan, zero
            if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  drsDestroy (

#    if PROTOTYPES
        DirectoryScan  scan)
#    else
        scan)

        DirectoryScan  scan ;
#    endif

{

    if (scan == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(drsDestroy) NULL scan handle: ") ;
        return (errno) ;
    }

    LGI "(drsDestroy) Terminating scan of %s.\n", scan->pathname) ;

/* Erase the contents of the structure and then delete the structure itself. */

    drsErase (scan, true) ;

    free (scan) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    drsFirst ()

    Get the First File in a Directory Scan.


Purpose:

    Function drsFirst() returns the first matching file in a directory scan.


    Invocation:

        fileName = drsFirst (scan) ;

    where:

        <scan>		- I
            is the directory scan handle returned by drsCreate().
        <fileName>	- O
            returns the full pathname of the first matching file.  The file
            name is stored in memory belonging to the DRS_UTIL package and
            it should not be overwritten or deallocated by the caller.  NULL
            is returned if there are no matching files.

*******************************************************************************/


const  char  *drsFirst (

#    if PROTOTYPES
        DirectoryScan  scan)
#    else
        scan)

        DirectoryScan  scan ;
#    endif

{
    return (drsGet (scan, 0)) ;
}

/*!*****************************************************************************

Procedure:

    drsGet ()

    Get the I-th File in a Directory Scan.


Purpose:

    Function drsGet() returns the I-th matching file in a directory scan.


    Invocation:

        fileName = drsGet (scan, index) ;

    where:

        <scan>		- I
            is the directory scan handle returned by drsCreate().
        <index>		- I
            is the index, 0 .. N-1, of the desired file.
        <fileName>	- O
            returns the full pathname of the indicated matching file.
            The file name is stored in memory belonging to the DRS_UTIL
            package and it should not be overwritten or deallocated by
            the caller.  NULL is returned if an invalid index is specified.

*******************************************************************************/


const  char  *drsGet (

#    if PROTOTYPES
        DirectoryScan  scan,
        size_t  index)
#    else
        scan, index)

        DirectoryScan  scan ;
        size_t  index ;
#    endif

{

    if (scan == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(drsGet) NULL scan handle: ") ;
        return (NULL) ;
    }

    if (index >= scan->numFiles) {
        return (NULL) ;
    } else {
        scan->nextFile = index + 1 ;
        return (scan->fileName[index]) ;
    }

}

/*!*****************************************************************************

Procedure:

    drsNext ()

    Get the Next File in a Directory Scan.


Purpose:

    Function drsNext() returns the next matching file in a directory scan.


    Invocation:

        fileName = drsNext (scan) ;

    where:

        <scan>		- I
            is the directory scan handle returned by drsCreate().
        <fileName>	- O
            returns the full pathname of the next matching file.  The file
            name is stored in memory belonging to the DRS_UTIL package and
            it should not be overwritten or deallocated by the caller.  NULL
            is returned if there are no matching files.

*******************************************************************************/


const  char  *drsNext (

#    if PROTOTYPES
        DirectoryScan  scan)
#    else
        scan)

        DirectoryScan  scan ;
#    endif

{
    return (drsGet (scan, (scan == NULL) ? 0 : scan->nextFile)) ;
}

/*!*****************************************************************************

Procedure:

    drsCompare ()

    Compare File Names.


Purpose:

    Function drsCompare() is called by the QSORT(3) function to compare
    two file names.


    Invocation:

        comparison = drsCompare (p1, p2) ;

    where:

        <p1>		- I
        <p2>		- I
            are (VOID *) pointers to the (CHAR *) pointers that point to
            the two file names being compared.
        <comparison>	- O
            returns -1, 0, or +1 if the first file name is lexicographically
            less than, equal to, or greater than the second file name.

*******************************************************************************/


static  int  drsCompare (

#    if PROTOTYPES
        const  void  *p1,
        const  void  *p2)
#    else
        p1, p2)

        void  *p1 ;
        void  *p2 ;
#    endif

{
#define  IGNORE_CASE
#ifdef IGNORE_CASE
    return (strcasecmp (*((char **) p1), *((char **) p2))) ;
#else
    return (strcmp (*((char **) p1), *((char **) p2))) ;
#endif
}

/*!*****************************************************************************

Procedure:

    drsErase ()

    Erase the Contents of a Directory Scan Structure.


Purpose:

    Function drsErase() erases some or all of the fields in a directory
    scan structure.  NOTE that the directory scan structure itself is
    NOT deallocated; call drsDestroy() for that purpose.

    Originally, all of the fields were (and still are, albeit indirectly now)
    deleted in drsDestroy().  However, after reading the ftw(3) man page, I
    realized that the system resources used in drsCreate() to construct the
    list of file names did not need to be retained until drsDestroy() is
    finally called.  In particular, the operating system mechanism for
    accessing a directory's list of files should be closed when no longer
    needed, a belatedly-obvious-to-me concern when using the DRS functions
    in a recursive context such as a file tree walk.


    Invocation:

        status = drsErase (scan, complete) ;

    where:

        <scan>		- I
            is the directory scan handle returned by drsCreate().
        <complete>	- I
            specifies whether or not all fields are erased.  If this
            argument is false, fields no longer needed are erased,
            but the constructed list of file names is kept.  If this
            argument is true, everything is erased.  NOTE again that
            the directory scan structure itself is not freed.
        <status>	- O
            returns the status of erasing the directory scan structure,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


static  errno_t  drsErase (

#    if PROTOTYPES
        DirectoryScan  scan,
        bool  complete)
#    else
        scan, complete)

        DirectoryScan  scan ;
        bool  complete ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (scan == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(drsErase) NULL scan handle: ") ;
        return (errno) ;
    }

    LGI "(drsErase) Erasing %s of scan for %s.\n",
        complete ? "all" : "some",
        (scan->pathname == NULL) ? "<null>" : scan->pathname) ;

/* Delete the fields that are no longer needed. */

    if (scan->pathname != NULL) {
        free (scan->pathname) ;
        scan->pathname = NULL ;
    }
    if (scan->dirname != NULL) {
        free (scan->dirname) ;
        scan->dirname = NULL ;
    }
#if defined(VMS)
    if (scan->directory != 0) {
        LIB$FIND_FILE_END (&scan->directory) ;
        scan->directory = 0 ;
    }
#elif defined(_WIN32)
    if (scan->directory != INVALID_HANDLE_VALUE) {
        FindClose (scan->directory) ;
        scan->directory = INVALID_HANDLE_VALUE ;
    }
#else
    if (scan->directory != NULL) {
        closedir (scan->directory) ;
        scan->directory = NULL ;
    }
#endif
    if (scan->compiledRE != NULL) {
        rexDestroy (scan->compiledRE, NULL) ;
        scan->compiledRE = NULL ;
    }

/* If complete erasure was requested, then also delete the file name list. */

    if (complete) {
        if (scan->fileName != NULL) {
            for (i = 0 ;  i < scan->numFiles ;  i++)
                free (scan->fileName[i]) ;
            free (scan->fileName) ;
            scan->fileName = NULL ;
        }
    }

    return (0) ;

}
