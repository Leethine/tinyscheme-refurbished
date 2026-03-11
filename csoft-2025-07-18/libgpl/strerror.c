/* $Id: strerror.c,v 1.2 2019/05/30 01:30:13 alex Exp $ */
/*******************************************************************************

File:

    strerror.c

    A strerror(3) Function.


Author:    Alex Measday


Purpose:

    The Standard C strerror(3) function returns a short error message
    for the ERRNO code passed into it.  Not all platforms have that
    function, so they can use this function (if code for the target
    platform has been added to the source file).


Public Procedures:

    strerror() - gets an ERRNO code's error text.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#ifdef __palmos__
#    include  <NetMgr.h>		/* netErr... values used in errno.h. */
#    include  <SysUtils.h>		/* SysErrString() prototype. */
#    include  <sys_errno.h>		/* PalmOS ERRNO definitions. */
#else
#    include  <errno.h>			/* Platform's ERRNO definitions. */
#endif
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */
#include  "strerror.h"			/* strerror() function definitions. */

#ifdef __palmos__
/*******************************************************************************
    ERRNO Code-to-Text Lookup Table.
*******************************************************************************/

static  const  NNLTable  errnoLUT = {
    { EPERM, "Not owner" },
    { ENOENT, "No such file or directory" },
    { ESRCH, "No such process" },
    { EINTR, "Interrupted system call" },
    { EIO, "I/O error" },
    { ENXIO, "No such device or address" },
    { E2BIG, "Arg list too long" },
    { ENOEXEC, "Exec format error" },
    { EBADF, "Bad file number" },
    { ECHILD, "No children" },
    { EAGAIN, "No more processes" },
    { ENOMEM, "Not enough core" },
    { EACCES, "Permission denied" },
    { EFAULT, "Bad address" },
    { ENOTBLK, "Block device required" },
    { EBUSY, "Mount device busy" },
    { EEXIST, "File exists" },
    { EXDEV, "Cross-device link" },
    { ENODEV, "No such device" },
    { ENOTDIR, "Not a directory" },
    { EISDIR, "Is a directory" },
    { EINVAL, "Invalid argument" },
    { ENFILE, "File table overflow" },
    { EMFILE, "Too many open files" },
    { ENOTTY, "Not a typewriter" },
    { ETXTBSY, "Text file busy" },
    { EFBIG, "File too large" },
    { ENOSPC, "No space left on device" },
    { ESPIPE, "Illegal seek" },
    { EROFS, "Read-only file system" },
    { EMLINK, "Too many links" },
    { EPIPE, "Broken pipe" },
    { EDOM, "Argument too large" },
    { ERANGE, "Result too large" },
    { EWOULDBLOCK, "Operation would block" },
    { EINPROGRESS, "Operation now in progress" },
    { EALREADY, "Operation already in progress" },
    { ENOTSOCK, "Socket operation on non-socket" },
    { EDESTADDRREQ, "Destination address required" },
    { EMSGSIZE, "Message too long" },
    { EPROTOTYPE, "Protocol wrong type for socket" },
    { ENOPROTOOPT, "Protocol not available" },
    { EPROTONOSUPPORT, "Protocol not supported" },
    { ESOCKTNOSUPPORT, "Socket type not supported" },
    { EOPNOTSUPP, "Operation not supported on socket" },
    { EPFNOSUPPORT, "Protocol family not supported" },
    { EAFNOSUPPORT, "Address family not supported by protocol family" },
    { EADDRINUSE, "Address already in use" },
    { EADDRNOTAVAIL, "Can't assign requested address" },
    { ENETDOWN, "Network is down" },
    { ENETUNREACH, "Network is unreachable" },
    { ENETRESET, "Network dropped connection on reset" },
    { ECONNABORTED, "Software caused connection abort" },
    { ECONNRESET, "Connection reset by peer" },
    { ENOBUFS, "No buffer space available" },
    { EISCONN, "Socket is already connected" },
    { ENOTCONN, "Socket is not connected" },
    { ESHUTDOWN, "Can't send after socket shutdown" },
    { ETOOMANYREFS, "Too many references: can't splice" },
    { ETIMEDOUT, "Connection timed out" },
    { ECONNREFUSED, "Connection refused" },
    { ELOOP, "Too many levels of symbolic links" },
    { ENAMETOOLONG, "File name too long" },
    { EHOSTDOWN, "Host is down" },
    { EHOSTUNREACH, "No route to host" },
    { ENOTEMPTY, "Directory not empty" },
    { EPROCLIM, "Too many processes" },
    { EUSERS, "Too many users" },
    { EDQUOT, "Disc quota exceeded" },
    { ESTALE, "Stale NFS file handle" },
    { EREMOTE, "Too many levels of remote in path" },
    { ENOSTR, "Device is not a stream" },
    { ETIME, "Timer expired" },
    { ENOSR, "Out of streams resources" },
    { ENOMSG, "No message of desired type" },
    { EBADMSG, "Trying to read unreadable message" },
    { EIDRM, "Identifier removed" },
    { EDEADLK, "Deadlock condition." },
    { ENOLCK, "No record locks available." },
    { ENONET, "Machine is not on the network" },
    { ERREMOTE, "Object is remote" },
    { ENOLINK, "Link has been severed" },
    { EADV, "Advertise error" },
    { ESRMNT, "Srmount error" },
    { ECOMM, "Communication error on send" },
    { EPROTO, "Protocol error" },
    { EMULTIHOP, "Multihop attempted" },
    { EDOTDOT, "Cross mount point (not an error)" },
    { EREMCHG, "Remote address changed" },
    { ENOSYS, "function not implemented" },
    { -1, NULL }
} ;
#endif	/* __palmos__ */

/*!*****************************************************************************

Procedure:

    sterror ()

    Get an ERRNO code's error text.


Purpose:

    Function strerror() looks up an ERRNO code and returns the corresponding
    error text.

        NOTE: This implementation of the function is intended for platforms
        which don't have a strerror(3) function.  As a consequence, the
        function has platform-specific code and platform-specific lookup
        tables.


    Invocation:

        errnoText = strerror (errnoCode) ;

    where

        <errnoCode>	- I
            is the ERRNO code in question.
        <errnoText>	- O
            returns the error text for the ERRNO code.  If the ERRNO code is
            not recognized, different things may happen depending on the
            platform.  At the least, the calling program will receive a
            string containing the formatted value of the ERRNO number.
            The error text returned to the caller is stored internally
            in this function as a read-only string literal or as a
            static string buffer.  The calling program should not attempt
            to modify the error text and it should use or save a copy of
            the error text before calling strerror() again.

*******************************************************************************/


char  *strerror (

#    if PROTOTYPES
        errno_t  errnoCode)
#    else
        errnoCode)

        errno_t  errnoCode ;
#    endif

{    /* Local variables. */
    char  *errnoText ;



    errnoText = (char *) nnlNameOf (errnoLUT, errnoCode) ;
    if (errnoText != NULL)  return (errnoText) ;

#ifdef __palmos__
  { static  char  errorString[128] ;
    SysErrString (errnoCode, errorString, sizeof errorString) ;
    return (errorString) ;
  }
#else
  { static  char  errorString[64] ;
    sprintf ("%ld", (long) errnoCode) ;
    return (errorString) ;
  }
#endif

}
