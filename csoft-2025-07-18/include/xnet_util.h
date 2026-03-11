/* $Id: xnet_util.h,v 1.14 2019/05/25 03:48:44 alex Exp $ */
/*******************************************************************************

    xnet_util.h

    XDR Networking Utility Definitions.

*******************************************************************************/

#ifndef  XNET_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  XNET_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <rpc/types.h>			/* RPC type definitions. */
#include  <rpc/xdr.h>			/* XDR type definitions. */
#include  "tcp_util.h"			/* TCP/IP networking utilities. */


/*******************************************************************************
    XNET Structures (Client View) and Definitions.
*******************************************************************************/

typedef  struct  _XnetStream  *XnetStream ;	/* XNET stream handle. */


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/
					/* Global debug switch (1/0 = yes/no). */
extern  int  xnet_util_debug  OCD ("xnet_uti") ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  xnetCreate P_((TcpEndpoint dataPoint,
                                const char *options,
                                XnetStream *stream))
    OCD ("xnet_uti") ;

extern  errno_t  xnetDestroy P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  errno_t  xnetEndRecord P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  IoFd  xnetFd P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  XDR  *xnetHandle P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  bool  xnetIsReadable P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  bool  xnetIsUp P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  bool  xnetIsWriteable P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  const  char  *xnetName P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  errno_t  xnetNextRecord P_((XnetStream stream))
    OCD ("xnet_uti") ;

extern  errno_t  xnetRead P_((XnetStream stream,
                              double timeout,
                              char **string))
    OCD ("xnet_uti") ;

extern  errno_t  xnetSetTimeout P_((XnetStream stream,
                                    double timeout))
    OCD ("xnet_uti") ;

extern  errno_t  xnetWrite P_((XnetStream stream,
                               double timeout,
                               const char *format,
                               ...))
    OCD ("xnet_uti") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
