/* $Id: skt_util.h,v 1.15 2021/04/21 02:18:26 alex Exp $ */
/*******************************************************************************

    skt_util.h

    Socket Utility Definitions.

*******************************************************************************/

#ifndef  SKT_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  SKT_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#if defined (_WIN32)
#    include  <winsock2.h>			/* Windows socket definitions. */
#    include  <process.h>			/* Process control functions. */
    typedef  u_long  IoctlArg ;			/* 3rd argument to ioctlsocket(). */
    typedef  SOCKET  IoFd ;			/* Windows file descriptor. */
    typedef  int  socklen_t ;			/* Length of socket address. */
#    define  VALID_FD(fd)  (fd != INVALID_SOCKET)
#    define  CLOSESOCKET  closesocket
#    define  IOCTLSOCKET  ioctlsocket
#    define  GET_NETERRNO()  (WSAGetLastError ())
#    define  SET_NETERRNO(error)  WSASetLastError (error)
#    if _MSC_VER < 1600				/* Earlier than VS 2010? */
#        define  EWOULDBLOCK  WSAEWOULDBLOCK
#        define  EINPROGRESS  WSAEINPROGRESS
#    endif
#    ifndef WINSOCK_VERSION			/* Swiped from "winsock2.h". */
#        ifndef  MAKEWORD
#            define  MAKEWORD(low,high)	\
                ((WORD)((BYTE)(low)) | (((WORD)(BYTE)(high))<<8)))
#        endif
#        define  WINSOCK_VERSION_MAJOR  1
#        define  WINSOCK_VERSION_MINOR  1
#        define  WINSOCK_VERSION	\
            MAKEWORD (WINSOCK_VERSION_MAJOR, WINSOCK_VERSION_MINOR)
#    endif
#elif defined (__palmos__)
#    include  <Unix/netinet_in.h>		/* htonl(3), ntohl(3), etc. */
#    include  <Unix/sys_socket.h>		/* UNIX socket definitions. */
#    include  <Unix/sys_time.h>			/* UNIX timeval structure. */
    typedef  long  IoctlArg ;			/* 3rd argument to ioctl(2). */
    typedef  NetSocketRef  IoFd ;		/* PalmOS file descriptor. */
    typedef  UInt16  socklen_t ;
#    define  INVALID_SOCKET  (-1)
#    define  VALID_FD(fd)  (fd >= 0)
#    define  CLOSESOCKET  close
#    define  IOCTLSOCKET  ioctl
#    define  GET_NETERRNO()  errno
#    define  SET_NETERRNO(error)  (errno = error)
#    define  getpid()  0x1234
#else
#    include  <unistd.h>			/* UNIX I/O definitions. */
#    if defined (__ANDROID__) && __ANDROID__
#        include  <arpa/inet.h>			/* in_addr_t */
#    endif
#    include  <netinet/in.h>			/* htonl(3), ntohl(3), etc. */
    typedef  int  IoctlArg ;			/* 3rd argument to ioctl(2). */
    typedef  int  IoFd ;			/* UNIX file descriptor. */
#    if defined(HAVE_SOCKLEN_T) && !HAVE_SOCKLEN_T
    typedef  int  socklen_t ;
#    endif
#    define  INVALID_SOCKET  (-1)
#    define  VALID_FD(fd)  (fd >= 0)
#    ifdef NDS
#        define  CLOSESOCKET  closesocket
#    else
#        define  CLOSESOCKET  close
#    endif
#    define  IOCTLSOCKET  ioctl
#    define  GET_NETERRNO()  errno
#    define  SET_NETERRNO(error)  (errno = error)
#endif

#if defined(HAVE_IN_ADDR_T) && !HAVE_IN_ADDR_T
    typedef  uint32_t  in_addr_t ;		/* Binary IPV4 address type. */
#    undef  HAVE_IN_ADDR_T
#    define  HAVE_IN_ADDR_T  1
#endif

#if defined(HAVE_IN_PORT_T) && !HAVE_IN_PORT_T
    typedef  uint16_t  in_port_t ;		/* Network port number. */
#endif

#ifndef IN_PORT_MAX
#    define  IN_PORT_MAX  65535			/* Maximum port number. */
#endif

#ifndef MAX_LISTEN_BACKLOG
#    define  MAX_LISTEN_BACKLOG  5		/* Maximum pending clients. */
#endif


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  sktBlock P_((IoFd fd,
                              bool blocking))
    OCD ("skt_util") ;

extern  errno_t  sktCleanup P_((void))
    OCD ("skt_util") ;

extern  bool  sktIsReadable P_((IoFd fd))
    OCD ("skt_util") ;

extern  bool  sktIsUp P_((IoFd fd))
    OCD ("skt_util") ;

extern  bool  sktIsWriteable P_((IoFd fd))
    OCD ("skt_util") ;

extern  errno_t  sktPeek P_((IoFd fd,
                             size_t maxBytes,
                             char *buffer,
                             size_t *numBytes))
    OCD ("skt_util") ;

extern  const  char  *sktPeer P_((IoFd fd))
    OCD ("skt_util") ;

extern  in_port_t  sktPort P_((IoFd fd))
    OCD ("skt_util") ;

extern  errno_t  sktSetBuf P_((IoFd fd,
                               socklen_t receiveSize,
                               socklen_t sendSize))
    OCD ("skt_util") ;

extern  errno_t  sktStartup P_((void))
    OCD ("skt_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
