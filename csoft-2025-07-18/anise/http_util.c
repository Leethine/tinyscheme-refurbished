/* $Id: http_util.c,v 1.13 2022/12/02 22:25:49 alex Exp alex $ */
/*******************************************************************************

File:

    http_util.c

    HTTP Utilities.


Author:    Alex Measday


Purpose:

    The HTTP_UTIL functions perform various HTTP functions.


Public Procedures:

    httpConvert() - converts escape sequences in an HTTP message line.
    httpEvaluate() - evaluates an HTTP command.
    httpLog() - logs an HTTP transaction.
    httpResolve() - resolves the resource name from an HTTP message.
    httpTypeOf() - returns the MIME type of a file.

Private Procedures:

    httpGET() - executes an HTTP GET command.
    httpHEAD() - executes an HTTP HEAD command.
    httpPOST() - executes an HTTP POST command.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* Standard C string functions. */
#if defined(HAVE_MEMCPY) && !HAVE_MEMCPY
#    define  memmove(dest,src,length)  bcopy(src,dest,length)
#endif
#if defined(VMS)
#    include  <file.h>			/* File definitions. */
#    include  <unixio.h>		/* VMS-emulation of UNIX I/O. */
#    define  S_ISDIR(mode)  ((((mode) & S_IFMT) & S_IFDIR) != 0)
#    define  S_ISLNK(mode)  false
#elif defined(VXWORKS)
#    include  <ioLib.h>			/* I/O library definitions. */
#    define  HAVE_LSTAT  0
#else
#    include  <fcntl.h>			/* File control definitions. */
#    ifdef _WIN32
#        include  <io.h>		/* UNIX I/O definitions. */
#        define  S_ISDIR(mode)  ((((mode) & _S_IFMT) & _S_IFDIR) != 0)
#        define  S_ISLNK(mode)  false
#    else
#        include  <unistd.h>		/* UNIX I/O definitions. */
#    endif
#endif
#include  <sys/stat.h>			/* File status definitions. */
#if defined(HAVE_LSTAT) && !HAVE_LSTAT
#    define  lstat  stat		/* OS doesn't support links. */
#endif
#include  "drs_util.h"			/* Directory scanning utilities. */
#include  "fnm_util.h"			/* Filename utilities. */
#include  "hash_util.h"			/* Hash table definitions. */
#include  "lfn_util.h"			/* LF-terminated network I/O. */
#include  "sdx_util.h"			/* String Descriptor utilities. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#include  "anise.h"			/* All-In-One Server definitions. */


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  httpGET (
#    if PROTOTYPES
        LfnStream  client,
        const  char  *resource,
        const  char  *version,
        const  char  *tildeTranslation,
        ResponseInfo  *response
#    endif
    ) ;

static  errno_t  httpHEAD (
#    if PROTOTYPES
        LfnStream  client,
        const  char  *resource,
        const  char  *version,
        const  char  *tildeTranslation,
        ResponseInfo  *response
#    endif
    ) ;

static  errno_t  httpPOST (
#    if PROTOTYPES
        LfnStream  client,
        const  char  *resource,
        const  char  *version,
        const  char  *body,
        const  char  *tildeTranslation,
        ResponseInfo  *response
#    endif
    ) ;

/*!*****************************************************************************

Procedure:

    httpConvert ()

    Convert Escape Sequences in an HTTP Line.


Purpose:

    Function httpConvert() converts escape sequences in an HTTP line of text
    to the corresponding characters.


    Invocation:

        length = httpConvert (text) ;

    where:

        <text>		- I
            is the line of text to be converted; the conversion is done
            in place.
        <length>	- O
            returns the length of the converted line.

*******************************************************************************/


size_t  httpConvert (

#    if PROTOTYPES
        char  *text)
#    else
        text)

        char  *text ;
#    endif

{    /* Local variables. */
    char  buffer[3], *s ;



/* Convert each escape sequence in the text to the corresponding ASCII
   character. */

    s = text ;
    for ( ; ; ) {
        s = strchr (s, '%') ;
        if ((s == NULL) || (strlen (s) < 3))  break ;
        if (isxdigit ((unsigned char) s[1]) &&
            isxdigit ((unsigned char) s[2])) {
            buffer[0] = s[1] ;  buffer[1] = s[2] ;  buffer[2] = '\0' ;
            *s++ = (char) strtol (buffer, NULL, 16) ;
            memmove (s, s+2, strlen (s+2) + 1) ;
        }
        s++ ;
    }

    return (strlen (text)) ;

}

/*!*****************************************************************************

Procedure:

    httpEvaluate ()

    Evaluate an HTTP Command.


Purpose:

    Function httpEvaluate() evaluates an HTTP command.


    Invocation:

        status = httpEvaluate (client, numLines, header, body,
                               tildeTranslation, &keepAlive, &response) ;

    where:

        <client>		- I
            is the stream handle for the network connection with the client.
        <numLines>		- I
            is the number of lines of text in the HTTP message header.
        <header>		- I
            is an array of the lines of text from the HTTP header.
        <body>			- I
            is a pointer to the message body, if any.
        <tildeTranslation>	- I
            specifies a format string used for tilde translation.  The string
            is of the form, "...%s...%s...", where the "%s"s are replaced by
            the user's name and the trailing portion of the resource name,
            respectively.
        <keepAlive>		- O
            returns true if the client requested that the connection be kept
            "alive" and false otherwise.
        <response>		- O
            returns information about the server's response to the command.
        <status>		- O
            returns the status of processing the HTTP command, zero if
            there were no errors and ERRNO otherwise.  Note that this
            status is independent of the status returned to the client.

*******************************************************************************/


errno_t  httpEvaluate (

#    if PROTOTYPES
        LfnStream  client,
        size_t  numLines,
        char  *header[],
        char  *body,
        const  char  *tildeTranslation,
        bool  *keepAlive,
        ResponseInfo  *response)
#    else
        client, numLines, header, body, tildeTranslation, keepAlive, response)

        LfnStream  client ;
        size_t  numLines ;
        char  *header[] ;
        char  *body ;
        const  char  *tildeTranslation ;
        bool  *keepAlive ;
        ResponseInfo  *response ;
#    endif

{    /* Local variables. */
    char  *command, *request, *resource, *s, *version ;
    errno_t  status ;
    size_t  i ;




/* Initialize the response information. */

    if (response != NULL) {
        response->status = 500 ;
        response->numBytes = -1 ;
        s = (char *) sktPeer (lfnFd (client)) ;
        strcpy (response->peer, (s == NULL) ? "-" : s) ;
    }

/* Parse the HTTP command line. */

    request = strdup (header[0]) ;
    if (request == NULL) {
        LGE "(httpEvaluate) Error duplicating request line.\nstrdup: ") ;
        return (errno) ;
    }
    command = strtok (request, " \t") ;
    resource = strtok (NULL, " \t") ;
    httpConvert (resource) ;		/* Convert escape sequences in URI. */
    version = strtok (NULL, " \t") ;

/* Check to see if the client has requested that the connection remain alive. */

    if (strcmp (version, "HTTP/1.0") == 0)
        *keepAlive = false ;
    else
        *keepAlive = true ;		/* Default in higher version numbers. */

    for (i = 1 ;  i < numLines ;  i++) {
        const  char  *connection = "connection:" ;
        if (strncasecmp (header[i], connection, strlen (connection)) != 0) {
            continue ;
        }
        s = header[i] + strlen (connection) ;
        s = s + strspn (s, " \t") ;
        if (strncasecmp (s, "keep-alive", strlen ("keep-alive")) == 0) {
            *keepAlive = true ;
        } else if (strncasecmp (s, "close", strlen ("close")) == 0) {
            *keepAlive = false ;
        }
    }

/* Execute the command. */

    if (strcmp (command, "GET") == 0) {
        status = httpGET (client, resource, version,
                          tildeTranslation, response) ;
        free (request) ;  SET_ERRNO (status) ;
        return (errno) ;
    } else if (strcmp (command, "HEAD") == 0) {
        status = httpHEAD (client, resource, version,
                           tildeTranslation, response) ;
        free (request) ;  SET_ERRNO (status) ;
        return (errno) ;
    } else if (strcmp (command, "POST") == 0) {
        status = httpPOST (client, resource, version, body,
                           tildeTranslation, response) ;
        free (request) ;  SET_ERRNO (status) ;
        return (errno) ;
    } else {
        free (request) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    httpLog ()

    Log an HTTP Transaction.


Purpose:

    Function httpLog() records an HTTP request/response transaction in
    an HTTP log file.  The log file follows the combined, Common Log File
    format, so each entry in the file is a single ASCII line formatted as
    follows:

        <client> - - <date> "<request>" <status> <bytes> <referer> <agent>

    where:

        <client> is the name of the client's host.

        "- -" are two N/A fields that are supposed to contain information
            about the client supplied by an authentication server on the
            client's host.  The relevant protocol is RFC 931 (Authentication
            Server), issued in 1985 and now obsolete!

        <request> is the HTTP request (e.g., "GET <url> HTTP/1.0"), enclosed
            in double quotes.

        <status> is the HTTP status code returned to the client.

        <bytes> is the number of bytes of data (excluding the HTTP header)
            sent to the client.

        <referer> is the "Referer:" field from the HTTP request header;
            e.g., the URL of the page containing a link for the requested
            item.

        <agent> is the "User-Agent:" field from the HTTP request header;
            e.g., the name of the client's browser.

    If a field cannot be determined or is not applicable, "-" is substituted.


    Invocation:

        status = httpLog (logFile, client, numLines, header, response) ;

    where:

        <logFile>	- I
            is the LogFile handle for the log file.  If this argument is NULL,
            the log is written to standard output.
        <client>	- I
            is the stream handle for the network connection with the client.
        <numLines>	- I
            is the number of lines of text in the HTTP message header.
        <header>	- I
            is an array of the lines of text from the HTTP header.
        <response>	- I
            is the address of information about the server's response
            to the command.
        <status>	- O
            returns the status of logging the transaction, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  httpLog (

#    if PROTOTYPES
        LogFile  logFile,
        LfnStream  client,
        size_t  numLines,
        char  *header[],
        ResponseInfo  *response)
#    else
        logFile, client, numLines, header, response)

        LogFile  logFile ;
        LfnStream  client ;
        size_t  numLines ;
        char  *header[] ;
        ResponseInfo  *response ;
#    endif

{    /* Local variables. */
    char  *s ;
    size_t  i, length ;




/* <host> <rfc931> <authUser> <date> "<request>" ... */

    logWrite (logFile, "%s - - %s \"%s\"",
              response->peer,
              tvShow (tvTOD (), 1, "%d/%b/%Y:%H:%M:%S"),
              header[0]) ;

/* ... <status> <numBytes> ... */

    logWrite (logFile, (response->status >= 0) ? " %d" : " -",
              response->status) ;
    logWrite (logFile, (response->numBytes >= 0) ? " %ld" : " -",
              response->numBytes) ;

/* ... <referer> ... */

    s = "-" ;  length = 1 ;
    for (i = 1 ;  i < numLines ;  i++) {
        if (strncasecmp (header[i], "Referer:", 8) == 0) {
            s = &header[i][8] + strspn (&header[i][8], " \t") ;
            length = strcspn (s, " \t") ;
            break ;
        }
    }
    logWrite (logFile, " %.*s", length, s) ;

/* ... <agent> */

    s = "-" ;  length = 1 ;
    for (i = 1 ;  i < numLines ;  i++) {
        if (strncasecmp (header[i], "User-Agent:", 11) == 0) {
            s = &header[i][11] + strspn (&header[i][11], " \t") ;
            length = strcspn (s, " \t") ;
            break ;
        }
    }
    logWrite (logFile, " %.*s\n", length, s) ;

    logFlush (logFile) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    httpResolve ()

    Resolve a Resource Name from an HTTP Command.


Purpose:

    Function httpResolve() converts the resource name (URI) received in an
    HTTP GET or POST command into a fully-qualified file name.


    Invocation:

        pathname = httpResolve (resource, tildeTranslation) ;

    where:

        <resource>		- I
            is the name of the resource as received in the HTTP command.
        <tildeTranslation>	- I
            specifies a format string used for tilde translation.  The string
            is of the form, "...%s...%s...", where the "%s"s are replaced by
            the user's name and the trailing portion of the resource name,
            respectively.
        <pathname>		- O
            returns the fully-qualified pathname for the resource.
            The pathname should be used or duplicated before calling
            httpResolve() again.  NULL is returned in the event of
            an error.

*******************************************************************************/


const  char  *httpResolve (

#    if PROTOTYPES
        const  char  *resource,
        const  char  *tildeTranslation)
#    else
        resource, tildeTranslation)

        char  *resource ;
        const  char  *tildeTranslation ;
#    endif

{    /* Local variables. */
    char  *firstSlash, *s ;
#define  MAX_PATHLEN  1024
    static  char  pathname[MAX_PATHLEN], temp[MAX_PATHLEN] ;



    if ((resource == NULL) || (tildeTranslation == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(httpResolve) NULL resource or tilde translation string.\n") ;
        return (NULL) ;
    }

/* Ignore a leading "/".  The home directory "~/" defaults to the login
   directory of the user who started up ANISE. */

    if (*resource == '/')  resource++ ;

    if ((strlen (resource) >= 2) && (strncmp (resource, "~/", 2) == 0))
        resource += 2 ;

/* Perform tilde translation.  The tilde translation format string is of
   the form, "...%s...%s...", where the "%s"s are replaced by the user's
   name and the trailing portion of the resource name, respectively. */

    if (*resource == '~') {
        firstSlash = strchr (resource, '/') ;
        if (firstSlash == NULL) {
            sprintf (pathname, tildeTranslation, ++resource, "/") ;
        } else {
            strcpy (temp, ++resource) ;
            temp[firstSlash-resource] = '\0' ;
            sprintf (pathname, tildeTranslation, temp, ++firstSlash) ;
        }
    } else {
#if !defined(HAVE_GETLOGIN) || HAVE_GETLOGIN
        s = getlogin () ;
#elif !defined(HAVE_CUSERID) || HAVE_CUSERID
        s = cuserid (NULL) ;
#elif defined(_WIN32)
      { DWORD  length = sizeof temp ;
        s = temp ;
        if (!GetUserNameA (s, &length))  s = NULL ;
      }
#else
        s = NULL ;
#endif
        if (s == NULL)  s = getenv ("LOGNAME") ;
        if (s == NULL)  s = getenv ("USER") ;
        if (s == NULL)  s = getenv ("USERNAME") ;
        sprintf (pathname, tildeTranslation, (s == NULL) ? "" : s, resource) ;
    }

/* If no file was specified, assume "index.html". */

    if (strlen (pathname) == 0)  strcpy (pathname, "index.html") ;

    strcpy (pathname, fnmBuild (FnmPath, pathname, NULL)) ;

    return (pathname) ;

}

/*!*****************************************************************************

Procedure:

    httpTypeOf ()

    Determine the MIME Type of a File.


Purpose:

    Function httpTypeOf() returns the MIME type of a file based on its
    file extension.


    Invocation:

        mimeType = httpTypeOf (pathname) ;

    where:

        <pathname>	- I
            is the name of the file.
        <mimeType>	- O
            returns the MIME type of the file, as determined from the
            file's extension.

*******************************************************************************/


const  char  *httpTypeOf (

#    if PROTOTYPES
        const  char  *pathname)
#    else
        pathname)

        char  *pathname ;
#    endif

{    /* Local variables. */
    char  buffer[256], *extension, *typeFileName, *typeName ;
    FILE  *file ;
    static  HashTable  mimeTypes = NULL ;




/*******************************************************************************
    If the extension-MIME mapping hasn't been loaded yet, then do so.
*******************************************************************************/

    if (mimeTypes == NULL) {

/* Create a hash table for the MIME types. */

        if (hashCreate (64, &mimeTypes)) {
            LGE "(httpTypeOf) Error creating hash table for MIME types.\nhashCreate: ") ;
            return ("unknown") ;
        }

/* Open the MIME type definition file. */

        typeFileName = getenv ("MIME_TYPES") ;
        if (typeFileName == NULL)  typeFileName = "/etc/mime.types" ;

        file = fopen (typeFileName, "r") ;
        if (file == NULL) {
            LGE "(httpTypeOf) Error opening MIME type definition file: %s\nfopen: ",
                typeFileName) ;
            return ("unknown") ;
        }

/* Load the file-extension/MIME-type mappings from the file. */

        while (fgets (buffer, sizeof buffer, file) != NULL) {
            typeName = strtok (buffer, " \t\n") ;
            if ((typeName == NULL) || (*typeName == '#'))  continue ;
            for ( ; ; ) {
                extension = strtok (NULL, " \t\n") ;
                if (extension == NULL)  break ;
                if (hashAdd (mimeTypes, extension,
                             (void *) strdup (typeName))) {
                    LGE "(httpTypeOf) Error adding %s-%s mapping to MIME types table.\nhashAdd: ",
                        extension, typeName) ;
                    return ("unknown") ;
                }
            }
        }

/* Close the definition file. */

        fclose (file) ;

    }


/*******************************************************************************
    Look up the target file's extension in the MIME types table.
    (The pre-increment of EXTENSION in the call to hashSearch()
    skips the leading period.)
*******************************************************************************/

    extension = fnmBuild (FnmExtension, pathname, NULL) ;

    if ((extension == NULL) ||		/* Error or unrecognized extension. */
        !hashSearch (mimeTypes, extension+1, (void **) &typeName)) {
        typeName = "unknown" ;
    }

    if (extension != NULL)  free (extension) ;

    return (typeName) ;

}

/*!*****************************************************************************

Procedure:

    httpGET ()

    Execute an HTTP GET Command.


Purpose:

    Function httpGET() executes an HTTP GET command.


    Invocation:

        status = httpGET (client, resource, version,
                          tildeTranslation, &response) ;

    where:

        <client>		- I
            is the stream handle for the network connection with the client.
        <resource>		- I
            is the name of the resource being retrieved.
        <version>		- I
            is the HTTP version provided by the client.
        <tildeTranslation>	- I
            specifies a format string used for tilde translation.  The string
            is of the form, "...%s...%s...", where the "%s"s are replaced by
            the user's name and the trailing portion of the resource name,
            respectively.
        <response>		- O
            returns information about the server's response to the command.
        <status>		- O
            returns the status of executing the GET command, zero if
            there were no errors and ERRNO otherwise.  Note that this
            status is independent of the status returned to the client.

*******************************************************************************/


static  errno_t  httpGET (

#    if PROTOTYPES
        LfnStream  client,
        const  char  *resource,
        const  char  *version,
        const  char  *tildeTranslation,
        ResponseInfo  *response)
#    else
        client, resource, version, tildeTranslation, response)

        LfnStream  client ;
        char  *resource ;
        char  *version ;
        const  char  *tildeTranslation ;
        ResponseInfo  *response ;
#    endif

{    /* Local variables. */
    char  *buffer, *pathname, *s ;
    DirectoryScan  scan ;
    FILE  *file ;
    size_t  fileSize, length ;
    struct  stat  info ;





/* Convert the URI to a fully-qualified pathname. */

    pathname = (char *) httpResolve (resource, tildeTranslation) ;
    if (pathname == NULL) {
        LGE "(httpGET) Error resolving resource \"%s\".\n",
            (resource == NULL) ? "<null>" : resource) ;
        return (errno) ;
    }

    if (stat (pathname, &info)) {
        LGE "(httpGET) File: %s\nstat: ", pathname) ;
        response->status = 404 ;
        return (lfnPutLine (client, -1.0, "%s 404 %s: %s\r\nContent-Length: 0\r\n\r\n",
                            version, pathname, strerror (errno))) ;
    }

/* If the URI specifies a directory and the directory has an index file,
   then use the index file as the target of the GET. */

    if (S_ISDIR (info.st_mode)) {
        strcat (pathname, "index.html") ;
        if (stat (pathname, &info)) {		/* "index.html" not found? */
            s = strrchr (pathname, '/') ;
            *++s = '\0' ;			/* Restore the directory. */
            strcat (pathname, "index.htm") ;
            if (stat (pathname, &info)) {	/* "index.htm" not found? */
                s = strrchr (pathname, '/') ;
                *++s = '\0' ;			/* Restore the directory. */
                stat (pathname, &info) ;
            }
        }
    }

    fileSize = (size_t) info.st_size ;

/*******************************************************************************
    If the target file is a directory, then list its contents.
*******************************************************************************/

    if (S_ISDIR (info.st_mode)) {

        errno_t  status ;
        StringDx  dlx ;

        strcat (pathname, "*") ;
        if (drsCreate (pathname, &scan)) {
            response->status = 500 ;
            return (lfnPutLine (client, -1.0, "%s 500 %s\r\nContent-Length: 0\r\n\r\n",
                                version, strerror (errno))) ;
        }
        if (pathname[strlen (pathname) - 1] == '/')
            pathname[strlen (pathname) - 1] = '\0' ;	/* Slash '/'! */

/* Create a dynamic string to which the HTML for the directory list is added. */

        if (sdxCreate (NULL, -1, SdxDynamic, &dlx)) {
            LGE "(httpGET) Error creating dynamic string for directory listing.\nsdxCreate: ") ;
            response->status = 500 ;
            return (lfnPutLine (client, -1.0, "%s 500 %s\r\nContent-Length: 0\r\n\r\n",
                                version, strerror (errno))) ;
        }

/* Add the beginning of the HTML and of the list. */

        if (sdxAddS (dlx, "<html>\n<head>\n<title>", -1) ||
            sdxAddS (dlx, pathname, -1) ||
            sdxAddS (dlx, "</title>\n</head>\n<body>\n<h2>", -1) ||
            sdxAddS (dlx, pathname, -1) ||
            sdxAddS (dlx, "</h2>\n<ul>\n", -1)) {
            LGE "(httpGET) Error adding beginning HTML for directory listing.\nsdxAddS: ") ;
            PUSH_ERRNO ;  drsDestroy (scan) ;
            sdxDestroy (dlx) ;  POP_ERRNO ;
            response->status = 500 ;
            return (lfnPutLine (client, -1.0, "%s 500 %s\r\nContent-Length: 0\r\n\r\n",
                                version, strerror (errno))) ;
        }

/* Add an HTML list item for each entry in the directory. */

        status = 0 ;

        for (s = (char *) drsFirst (scan) ;
             s != NULL ;
             s = (char *) drsNext (scan)) {
            lstat (s, &info) ;
            s = (char *) fnmBuild (FnmFile, s, NULL) ;
            if (S_ISDIR (info.st_mode)) {
/* Add a link to a directory. */
                if (sdxAddS (dlx, "<li><a href=\"", -1) ||
                    sdxAddS (dlx, s, -1) ||
                    sdxAddS (dlx, "/\"><b>", -1) ||
                    sdxAddS (dlx, s, -1) ||
                    sdxAddS (dlx, "/</b></a></li>\n", -1)) {
                    LGE "(httpGET) Error adding directory %s to directory listing.\nsdxAddS: ",
                        s) ;
                    status = errno ;
                    break ;
                }
            } else if (S_ISLNK (info.st_mode)) {
/* Add a link to a symbolic link. */
                if (sdxAddS (dlx, "<li><a href=\"", -1) ||
                    sdxAddS (dlx, s, -1) ||
                    sdxAddS (dlx, "\"><code>", -1) ||
                    sdxAddS (dlx, s, -1) ||
                    sdxAddS (dlx, "/</code></a></li>\n", -1)) {
                    LGE "(httpGET) Error adding link %s to directory listing.\nsdxAddS: ",
                        s) ;
                    status = errno ;
                    break ;
                }
            } else {
/* Add a link to a file. */
                char  number[32] ;
                fileSize = (size_t) info.st_size ;
                sprintf (number, "%"PRIuSIZE_T, (fileSize + 1023) / 1024) ;
                if (sdxAddS (dlx, "<li><a href=\"", -1) ||
                    sdxAddS (dlx, s, -1) ||
                    sdxAddS (dlx, "\"><code>", -1) ||
                    sdxAddS (dlx, s, -1) ||
                    sdxAddS (dlx, "</code></a>&nbsp;&nbsp;&nbsp;&nbsp;<code><i>(", -1) ||
                    sdxAddS (dlx, number, -1) ||
                    sdxAddS (dlx, "K)</i></li>\n", -1)) {
                    LGE "(httpGET) Error adding file %s to directory listing.\nsdxAddS: ",
                        s) ;
                    status = errno ;
                    break ;
                }
            }
        }

/* Add the end of the HTML. */

        if (status ||
            sdxAddS (dlx, "</ul>\n</body>\n</html>\n", -1)) {
            PUSH_ERRNO ;  drsDestroy (scan) ;  sdxDestroy (dlx) ;  POP_ERRNO ;
            response->status = 500 ;
            return (lfnPutLine (client, -1.0, "%s 500 %s\r\nContent-Length: 0\r\n\r\n",
                                version, strerror (errno))) ;
        }

        drsDestroy (scan) ;

/* Finally, output the HTTP response header (including the body length)
   and the body itself (the HTML directory listing). */

        response->status = 200 ;

        if (lfnPutLine (client, -1.0, "%s 200\r\n", version) ||
            lfnPutLine (client, -1.0, "Content-type: text/html\r\n") ||
            lfnPutLine (client, -1.0,
                        "Content-length: %"PRIuSIZE_T"\r\n",
                        sdxLength (dlx)) ||
            lfnPutLine (client, -1.0, "\r\n") ||
            lfnWrite (client, -1.0, sdxLength (dlx), sdxStringZ (dlx), NULL)) {
            PUSH_ERRNO ;  sdxDestroy (dlx) ;  POP_ERRNO ;
            return (errno) ;
        }

        response->numBytes = sdxLength (dlx) ;

        sdxDestroy (dlx) ;

        return (0) ;

    }

/*******************************************************************************
    The target file is a file; send its contents to the client.
*******************************************************************************/

/* Open the file. */

    file = fopen (pathname, "rb") ;
    if (file == NULL) {
        LGE "(httpGET) File: %s\nfopen: ", pathname) ;
        response->status = 404 ;
        return (lfnPutLine (client, -1.0, "%s 404 %s: %s\r\nContent-length: 0\r\n\r\n",
                            version, pathname, strerror (errno))) ;
    }

/* Send the response header to the client. */

    response->status = 200 ;
    if (lfnPutLine (client, -1.0, "%s 200\r\n", version) ||
        lfnPutLine (client, -1.0, "Content-type: %s\r\n",
                    httpTypeOf (pathname)) ||
        lfnPutLine (client, -1.0, "Content-length: %"PRIuSIZE_T"\r\n",
                    fileSize) ||
        lfnPutLine (client, -1.0, "\r\n")) {
        PUSH_ERRNO ;  fclose (file) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Send the contents of the file. */

#define  BUFSIZE  8*1024
    buffer = malloc (BUFSIZE) ;
    if (buffer == NULL) {
        LGE "(httpGET) Error allocating %d-byte buffer.\nmalloc: ", BUFSIZE) ;
        PUSH_ERRNO ;  fclose (file) ;  POP_ERRNO ;
        return (errno) ;
    }

    response->numBytes = 0 ;
    while (0 < (length = fread (buffer, 1, BUFSIZE, file))) {
        if (lfnWrite (client, -1.0, length, buffer, NULL)) {
            PUSH_ERRNO ;  fclose (file) ;  free (buffer) ;  POP_ERRNO ;
            return (errno) ;
        }
        response->numBytes += length ;
    }

    free (buffer) ;

/* Close the file. */

    fclose (file) ;

/* Check to make sure the entire file was sent.  I originally used
   open(2)/read(2) to read the file, which worked everywhere ... until
   I built ANISE in Visual Studio 2019 Community Edition.  HTML files
   were read in full, but image files were read only partially (with
   one exception: small, one-image ".ico" files were read correctly.
   The number of bytes read were in the 2-digit and low 3-digit range
   in the case of the web pages I tested.  The number varied among
   different image files of all types, but the count for a specific
   individual file was constant and repeatable.  Switching to
   fopen(...,"rb") and fread(3) seemed to solve the "problem", but
   I'll keep checking anyway.  Returning an error status ensures
   that ANISE closes the network connection immediately; otherwise,
   the browser would continue waiting for the rest of the data until
   ANISE's keep-alive timer expired and the network connection was closed. */

    if (response->numBytes != fileSize) {
        SET_ERRNO (EIO) ;
        LGE "(httpGET) File: %s  Size: %"PRIuSIZE_T" != Sent: %"PRIuSIZE_T"\n",
            pathname, fileSize, response->numBytes) ;
        return (errno) ;
    }


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    httpHEAD ()

    Execute an HTTP HEAD Command.


Purpose:

    Function httpHEAD() executes an HTTP HEAD command.


    Invocation:

        status = httpHEAD (client, resource, version,
                           tildeTranslation, &response) ;

    where:

        <client>		- I
            is the stream handle for the network connection with the client.
        <resource>		- I
            is the name of the file being retrieved.
        <version>		- I
            is the HTTP version provided by the client.
        <tildeTranslation>	- I
            specifies a format string used for tilde translation.  The string
            is of the form, "...%s...%s...", where the "%s"s are replaced by
            the user's name and the trailing portion of the resource name,
            respectively.
        <response>		- O
            returns information about the server's response to the command.
        <status>		- O
            returns the status of executing the HEAD command, zero if
            there were no errors and ERRNO otherwise.  Note that this
            status is independent of the status returned to the client.

*******************************************************************************/


static  errno_t  httpHEAD (

#    if PROTOTYPES
        LfnStream  client,
        const  char  *resource,
        const  char  *version,
        const  char  *tildeTranslation,
        ResponseInfo  *response)
#    else
        client, resource, version, tildeTranslation, response)

        LfnStream  client ;
        char  *resource ;
        char  *version ;
        const  char  *tildeTranslation ;
        ResponseInfo  *response ;
#    endif

{    /* Local variables. */
    char  *pathname ;
    size_t  fileSize ;
    struct  stat  info ;




/* Convert the URL-specified file name to a fully-qualified pathname. */

    pathname = (char *) httpResolve (resource, tildeTranslation) ;
    if (pathname == NULL) {
        LGE "(httpHEAD) Error resolving resource \"%s\".\n",
            (resource == NULL) ? "<null>" : resource) ;
        return (errno) ;
    }

    if (stat (pathname, &info)) {
        LGE "(httpHEAD) File: %s\nstat: ", pathname) ;
        response->status = 404 ;
        response->numBytes = 0 ;
        return (lfnPutLine (client, -1.0, "%s 404 %s: %s\r\nContent-length: 0\r\n\r\n",
                            version, pathname, strerror (errno))) ;
    }

    fileSize = (size_t) info.st_size ;

/* Send the response header to the client. */

    response->status = 200 ;
    response->numBytes = fileSize ;

    if (lfnPutLine (client, -1.0, "%s 200\r\n", version) ||
        lfnPutLine (client, -1.0, "Content-type: %s\r\n",
                    httpTypeOf (pathname)) ||
        lfnPutLine (client, -1.0, "Content-length: %"PRIuSIZE_T"\r\n",
                    fileSize) ||
        lfnPutLine (client, -1.0, "\r\n")) {
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    httpPOST ()

    Execute an HTTP POST Command.


Purpose:

    Function httpPOST() executes an HTTP POST command.


    Invocation:

        status = httpPOST (client, resource, version, body,
                           tildeTranslation, &response) ;

    where:

        <client>		- I
            is the stream handle for the network connection with the client.
        <resource>		- I
            is the name of the file being retrieved.
        <version>		- I
            is the HTTP version provided by the client.
        <body>			- I
            is a pointer to the information being posted.
        <tildeTranslation>	- I
            specifies a format string used for tilde translation.  The string
            is of the form, "...%s...%s...", where the "%s"s are replaced by
            the user's name and the trailing portion of the resource name,
            respectively.
        <response>		- O
            returns information about the server's response to the command.
        <status>		- O
            returns the status of executing the POST command, zero if
            there were no errors and ERRNO otherwise.  Note that this
            status is independent of the status returned to the client.

*******************************************************************************/


static  errno_t  httpPOST (

#    if PROTOTYPES
        LfnStream  client,
        const  char  *resource,
        const  char  *version,
        const  char  *body,
        const  char  *tildeTranslation,
        ResponseInfo  *response)
#    else
        client, resource, version, body, tildeTranslation, response)

        LfnStream  client ;
        char  *resource ;
        char  *version ;
        char  *body ;
        const  char  *tildeTranslation ;
        ResponseInfo  *response ;
#    endif

{

    response->status = 501 ;
    response->numBytes = 0 ;

    return (lfnPutLine (client, -1.0, "%s 501\r\nContent-length: 0\r\n\r\n",
                        version)) ;

}
