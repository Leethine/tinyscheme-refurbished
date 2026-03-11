/* $Id: anise.h,v 1.7 2012/05/06 22:26:01 alex Exp alex $ */
/*******************************************************************************

    anise.h

    All-In-One Server (ANISE) Definitions.

    (I moved the individually slim contents of "ftpClient.h", "http_util.h",
    "passClient.h", and "wwwClient.h" into "anise".h" in hopes it would
    simplify things for me to have them all in one place.)

*******************************************************************************/

#ifndef  ANISE_H		/* Has the file been INCLUDE'd already? */
#define  ANISE_H  yes


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "iox_util.h"			/* I/O event dispatcher definitions. */
#include  "lfn_util.h"			/* LF-terminated network I/O. */
#include  "log_util.h"			/* Log file utilities. */
#include  "tcp_util.h"			/* TCP/IP network utilities. */
#include  "tpl_util.h"			/* Tuple utilities. */


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

extern  int  debug ;			/* Debug switch. */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  debug


/*******************************************************************************
    FTP Client Structures (Client View) and Functions.
*******************************************************************************/

typedef  struct  _FtpClient  *FtpClient ;

extern  errno_t  ftpClientCreate P_((TcpEndpoint connection,
                                     IoxDispatcher dispatcher,
                                     void *parameter,
                                     FtpClient *client)) ;

extern  errno_t  ftpClientDestroy P_((FtpClient client)) ;


/*******************************************************************************
    HTTP Definitions and Functions.
*******************************************************************************/

/* HTTP Response Information - used to log the request/response transaction. */

typedef  struct  ResponseInfo {
    int  status ;			/* Status code returned to client. */
    size_t  numBytes ;			/* Content length (excluding header);
					   "-" is logged for length of zero. */
    char  peer[256] ;			/* IP address of client. */
}  ResponseInfo ;

extern  size_t  httpConvert P_((char *text)) ;

extern  errno_t  httpEvaluate P_((LfnStream client,
                                  size_t numLines,
                                  char *header[],
                                  char *body,
                                  const char *tildeTranslation,
                                  bool *keepAlive,
                                  ResponseInfo *response)) ;

extern  errno_t  httpLog P_((LogFile logFile,
                             LfnStream client,
                             size_t numLines,
                             char *header[],
                             ResponseInfo *response)) ;

extern  const  char  *httpResolve P_((const char *resource,
                                      const char *tildeTranslation)) ;

extern  const  char  *httpTypeOf P_((const char *pathname)) ;


/*******************************************************************************
    Pass-Through Client Structures (Client View) and Functions.
*******************************************************************************/

typedef  struct  _PassClient  *PassClient ;

extern  errno_t  passClientCreate P_((TcpEndpoint connection,
                                      IoxDispatcher dispatcher,
                                      const char *target,
                                      PassClient *client)) ;

extern  errno_t  passClientDestroy P_((PassClient client)) ;


/*******************************************************************************
    WWW Client Structures (Client View) and Functions.
*******************************************************************************/

typedef  struct  _WwwClient  *WwwClient ;

extern  errno_t  wwwClientCreate P_((TcpEndpoint connection,
                                     IoxDispatcher dispatcher,
                                     Tuple tuple,
                                     WwwClient *client)) ;

extern  errno_t  wwwClientDestroy P_((WwwClient client)) ;

extern  bool  wwwClientIsReadable P_((WwwClient client)) ;

extern  bool  wwwClientIsUp P_((WwwClient client)) ;


#endif				/* If this file was not INCLUDE'd previously. */
