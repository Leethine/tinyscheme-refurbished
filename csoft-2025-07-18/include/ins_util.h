/* $Id: ins_util.h,v 1.8 2023/02/26 06:26:16 alex Exp $ */
/*******************************************************************************

    ins_util.h

    IN-SNEC CORTEX Utilities.

*******************************************************************************/

#ifndef  INS_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  INS_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <rpc/types.h>			/* RPC type definitions. */
#include  <rpc/xdr.h>			/* XDR type definitions. */
#include  "tcp_util.h"			/* TCP/IP networking utilities. */


/*******************************************************************************
    CORTEX Interface Structures (Client View) and Definitions.
*******************************************************************************/

					/* CORTEX stream handle. */
typedef  struct  _CortexStream  *CortexStream ;

#define  InsPreamble  (INT32_C (1234567890))
#define  InsPostamble  (INT32_C (-1234567890))

typedef  struct  CortexHeader {
    int32_t  preamble ;			/* 1234567890 */
    int32_t  length ;			/* Length (in bytes) of message. */
    int32_t  flowID ;			/* Data flow ID. */
}  CortexHeader ;

typedef  struct  CortexTrailer {
    int32_t  postamble ;		/* -1234567890 */
}  CortexTrailer ;


/*******************************************************************************
    Predefined Data Flow IDs.
*******************************************************************************/

#define  InsDefaultFlowID  0
#define  InsConfigurationFlowID  (INT32_C (1))
#define  InsResetFlowID  (INT32_C (0xFFFFFFFF))


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  ins_util_debug  OCD ("ins_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  int32_t  insChecksum P_((const CortexHeader *header,
                                 const char *body))
    OCD ("ins_util") ;

extern  errno_t  insCreate P_((TcpEndpoint dataPoint,
                               CortexStream *stream))
    OCD ("ins_util") ;

extern  size_t  insDecode P_((const char *buffer,
                              const xdrproc_t decodeF,
                              size_t step,
                              size_t length,
                              char *data))
    OCD ("ins_util") ;

extern  errno_t  insDestroy P_((CortexStream stream))
    OCD ("ins_util") ;

extern  size_t  insEncode P_((const char *data,
                              const xdrproc_t encodeF,
                              size_t step,
                              size_t length,
                              char *buffer))
    OCD ("ins_util") ;

extern  IoFd  insFd P_((CortexStream stream))
    OCD ("ins_util") ;

extern  bool  insIsReadable P_((CortexStream stream))
    OCD ("ins_util") ;

extern  bool  insIsUp P_((CortexStream stream))
    OCD ("ins_util") ;

extern  bool  insIsWriteable P_((CortexStream stream))
    OCD ("ins_util") ;

extern  const  char  *insName P_((CortexStream stream))
    OCD ("ins_util") ;

extern  int32_t  insPart P_((const char *name,
                             bool isVME))
    OCD ("ins_util") ;

extern  const  char  *insPartName P_((int32_t code))
    OCD ("ins_util") ;

extern  errno_t  insPort P_((const char *name,
                             int offset,
                             in_port_t *port))
    OCD ("ins_util") ;

extern  const  char  *insPortName P_((in_port_t port,
                                      int offset,
                                      bool isVME))
    OCD ("ins_util") ;

extern  errno_t  insRead P_((CortexStream stream,
                             double timeout,
                             CortexHeader *header,
                             char **body))
    OCD ("ins_util") ;

extern  errno_t  insWrite P_((CortexStream stream,
                              double timeout,
                              const CortexHeader *header,
                              const char *body))
    OCD ("ins_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
