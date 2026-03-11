/* $Id: sdx_util.h,v 1.6 2023/11/24 02:38:04 alex Exp $ */
/*******************************************************************************

    sdx_util.h

    String Descriptor (SDX) Utilities.

*******************************************************************************/

#ifndef  SDX_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  SDX_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */


/*******************************************************************************
    String Descriptors (Client View) and Definitions.
*******************************************************************************/

typedef  enum  SdxStorageClass {
    SdxStatic, SdxDynamic, SdxVolatile
}  SdxStorageClass ;

					/* String descriptors. */
typedef  struct  _StringDx  *StringDx ;
typedef  struct  _SubStringDx  *SubStringDx ;

					/* Retry function prototype. */
typedef  errno_t  (*SdxRetryFunc) P_((StringDx, void *)) ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  sdx_util_debug  OCD ("sdx_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  sdxAddC P_((StringDx descriptor,
                             char character))
    OCD ("sdx_util") ;

extern  errno_t  sdxAddS P_((StringDx descriptor,
                             const char *text,
                             ssize_t length))
    OCD ("sdx_util") ;

extern  errno_t  sdxCopy P_((StringDx source,
                             bool isStatic,
                             StringDx destination))
    OCD ("sdx_util") ;

extern  errno_t  sdxCreate P_((char *string,
                               ssize_t length,
                               SdxStorageClass allocation,
                               StringDx *descriptor))
    OCD ("sdx_util") ;

extern  errno_t  sdxDestroy P_((StringDx descriptor))
    OCD ("sdx_util") ;

extern  StringDx  sdxDuplicate P_((StringDx descriptor))
    OCD ("sdx_util") ;

extern  errno_t  sdxErase P_((StringDx descriptor))
    OCD ("sdx_util") ;

extern  errno_t  sdxGrow P_((StringDx descriptor,
                             size_t toAdd))
    OCD ("sdx_util") ;

extern  size_t  sdxIncrement P_((StringDx descriptor,
                                 ssize_t blockSize))
    OCD ("sdx_util") ;

extern  size_t  sdxLength P_((StringDx descriptor))
    OCD ("sdx_util") ;

extern  size_t  sdxMaxLength P_((StringDx descriptor))
    OCD ("sdx_util") ;

extern  errno_t  sdxOwn P_((StringDx descriptor))
    OCD ("sdx_util") ;

extern  errno_t  sdxReadLine P_((FILE *file,
                                 StringDx descriptor,
                                 ssize_t *numLines))
    OCD ("sdx_util") ;

extern  errno_t  sdxReplace P_((StringDx descriptor,
                                bool ignoreCase,
                                const char *find,
                                ssize_t findLen,
                                const char *replace,
                                ssize_t replaceLen,
                                ssize_t maxReplace,
                                size_t *numReplace))
    OCD ("sdx_util") ;

extern  errno_t  sdxRetry P_((StringDx descriptor,
                              size_t increment,
                              SdxRetryFunc retryF,
                              void *userData))
    OCD ("sdx_util") ;

extern  errno_t  sdxSetLength P_((StringDx descriptor,
                                  ssize_t length))
    OCD ("sdx_util") ;

extern  char  *sdxStringZ P_((StringDx descriptor))
    OCD ("sdx_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
