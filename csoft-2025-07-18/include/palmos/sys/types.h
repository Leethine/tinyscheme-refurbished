/* $Id: types.h,v 1.2 2019/06/21 02:27:11 alex Exp $ */

/* Redirect to the corresponding PalmOS SDK header. */

#include  <Unix/sys_types.h>		/* System type definitions. */

/*******************************************************************************
    Data type off_t should be defined in <sys/types.h>, but isn't.
    Define it here.  (Used by BSD-XDR's "xdr_rec.c" file.)
*******************************************************************************/

#ifndef  SYS_TYPES_H		/* Has the file been INCLUDE'd already? */
#define  SYS_TYPES_H  yes

typedef  long  long  off64_t ;
#if defined(_FILE_OFFSET_BITS) && (_FILE_OFFSET_BITS_ == 64)
    typedef  off64_t  off_t ;
#else
    typedef  long  off_t ;
#endif

#endif				/* If this file was not INCLUDE'd previously. */
