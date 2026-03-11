/* $Id: unistd.h,v 1.1 2005/01/12 20:24:07 alex Exp alex $ */
/*******************************************************************************

    unistd.h

    UNIX Library Functions.

*******************************************************************************/

#ifndef  PALM_UNISTD_H		/* Has the file been INCLUDE'd already? */
#define  PALM_UNISTD_H  yes


#include  <SystemMgr.h>			/* Palm SDK header. */
#include  <sys_socket.h>		/* Palm SDK header. */


/*******************************************************************************
    PalmOS doesn't have lseek(2) and, for some inexplicable reason, BSD-XDR's
    xdrrec_getpos() function calls lseek(2) on a socket, which is supposed to
    be an error: ERRNO is set to ESPIPE and lseek(2) returns an offset of -1.
*******************************************************************************/

#if defined(HAVE_LSEEK) && !HAVE_LSEEK
#    undef  lseek
#    define  lseek(a1,a2,a3)  (errno = ESPIPE, -1)
#endif


#endif				/* If this file was not INCLUDE'd previously. */
