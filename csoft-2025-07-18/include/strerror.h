/* $Id: strerror.h,v 1.3 2019/05/25 03:39:40 alex Exp $ */
/*******************************************************************************

    strerror.h

    Standard C Library Error Definitions.

*******************************************************************************/

#ifndef  STRERROR_H		/* Has the file been INCLUDE'd already? */
#define  STRERROR_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

#if defined(HAVE_ERRNO) && !HAVE_ERRNO
    extern  errno_t  errno ;		/* Global ERRNO value. */
#endif


/*******************************************************************************
    Public functions.
*******************************************************************************/

#if defined(HAVE_STRERROR) && !HAVE_STRERROR
    extern  char  *strerror (errno_t number) ;
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
