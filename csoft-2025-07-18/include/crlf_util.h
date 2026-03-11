/* $Id: crlf_util.h,v 1.7 2019/02/22 03:32:05 alex Exp $ */
/*******************************************************************************

    crlf_util.h

    Carriage Return/Line Feed Utility Definitions.

*******************************************************************************/

#ifndef  CRLF_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  CRLF_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  char  *crlf2nl P_((char *string,
                           ssize_t length,
                           char *lastChar))
    OCD ("crlf_uti") ;

extern  char  *nl2crlf P_((char *string,
                           ssize_t length,
                           size_t maxLength))
    OCD ("crlf_uti") ;

extern  size_t  nlCount P_((const char *string,
                            ssize_t length))
    OCD ("crlf_uti") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
