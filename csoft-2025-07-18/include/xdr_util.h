/* $Id: xdr_util.h,v 1.2 2021/04/21 02:06:15 alex Exp $ */
/*******************************************************************************

    xdr_util.h

    XDR funtions and utilities.

*******************************************************************************/

#ifndef  XDR_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  XDR_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <rpc/types.h>			/* RPC type definitions. */
#include  <rpc/xdr.h>			/* XDR type definitions. */
#include  "ts_util.h"			/* "timespec" manipulation functions. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  bool_t  xdr_enumN_t P_((XDR *xdrs, void *value, size_t size))
    OCD ("libxdr") ;

extern  bool_t  xdr_enumNu_t P_((XDR *xdrs, void *value, size_t size))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum8_t P_((XDR *xdrs, int8_t *value))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum8u_t P_((XDR *xdrs, uint8_t *value))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum16_t P_((XDR *xdrs, int16_t *value))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum16u_t P_((XDR *xdrs, uint16_t *value))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum32_t P_((XDR *xdrs, int32_t *value))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum32u_t P_((XDR *xdrs, uint32_t *value))
    OCD ("libxdr") ;

#if !defined(HAVE_64_BIT_INTEGERS) || HAVE_64_BIT_INTEGERS

extern  bool_t  xdr_enum64_t P_((XDR *xdrs, int64_t *value))
    OCD ("libxdr") ;

extern  bool_t  xdr_enum64u_t P_((XDR *xdrs, uint64_t *value))
    OCD ("libxdr") ;

#endif

extern  bool_t  xdr_time_t P_((XDR *xdrStream,
                               time_t *seconds))
    OCD ("libxdr") ;

extern  bool_t  xdr_timespec P_((XDR *xdrStream,
                                 struct timespec *time))
    OCD ("libxdr") ;

extern  bool_t  xdr_timeval P_((XDR *xdrStream,
                                struct timeval *time))
    OCD ("libxdr") ;

extern  bool_t  xdr_timeval32 P_((XDR *xdrStream,
                                  struct timeval *time))
    OCD ("libxdr") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
