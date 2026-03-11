/* $Id: cornet.h,v 1.8 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

    cornet.h

    CORTEX Network Utility (CORNET) Definitions.

*******************************************************************************/

#ifndef  CORNET_H		/* Has the file been INCLUDE'd already? */
#define  CORNET_H  yes


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "ins_util.h"			/* IN-SNEC CORTEX utilities. */
#include  "iox_util.h"			/* I/O event dispatcher definitions. */


/*******************************************************************************
    Link Structure - represents a network connection to a CORTEX port.
*******************************************************************************/

typedef  struct  _Link {
    in_port_t  port ;			/* Port on CORTEX. */
    bool  isVME ;			/* VME or NT? */
    CortexStream  stream ;		/* Network connection. */
    int32_t  flowID ;			/* Data flow identification. */
    int32_t  table ;			/* If monitoring a table. */
}  _Link, *Link ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

extern  int  debug ;			/* Debug switch. */
extern  IoxDispatcher  dispatcher ;	/* I/O event dispatcher. */


/*******************************************************************************
    Public functions.
*******************************************************************************/

/* [CORNET]command.c */
extern  errno_t  sendTC P_((const char *host,
                            bool isVME,
                            int offset,
                            int32_t flowID,
                            const char *operation)) ;

/* [CORNET]configure.c */
extern  errno_t  configure P_((const char *host,
                               bool isVME,
                               int offset,
                               const char *configuration)) ;

/* [CORNET]link.c */
extern  errno_t  linkClose P_((Link link)) ;
extern  errno_t  linkOpen P_((const char *host,
                              int port,
                              bool isVME,
                              Link *link)) ;

/* [CORNET]monitor.c */
extern  errno_t  monitorTable P_((const char *host,
                                  bool isVME,
                                  int offset,
                                  int32_t flowID,
                                  const char *name)) ;

/* [CORNET]ranging.c */
extern  errno_t  measure P_((const char *host,
                             bool isVME,
                             int offset,
                             int32_t flowID,
                             bool calibrate,
                             const char *what)) ;

/* [CORNET]sequence.c */
extern  errno_t  sequence P_((const char *host,
                              int offset,
                              int32_t flowID,
                              int32_t numSequences)) ;

/* [CORNET]telemetry.c */
extern  errno_t  startTLM P_((const char *host,
                              bool isVME,
                              bool noRequest,
                              int offset,
                              int32_t flowID,
                              int32_t channel,
                              bool history)) ;

/* [CORNET]utility.c */
extern  void  dumpData P_((FILE *file,
                           int firstField,
                           int numFields,
                           void *data,
                           bool inHex,
                           bool asFloat)) ;
extern  errno_t  inputCB P_((IoxCallback callback,
                             IoxReason reason,
                             void *userData)) ;
extern  const  char  *tcRequestOf P_((int32_t code)) ;
extern  const  char  *tcStatusOf P_((int32_t code)) ;


#endif				/* If this file was not INCLUDE'd previously. */
