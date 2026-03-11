/* $Id: vtmx_util.h,v 1.1 2021/09/13 02:42:42 alex Exp $ */
/*******************************************************************************

    vtmx_util.h

    Enhanced View of Time Service (EVOT) Marshaling Utilities.

*******************************************************************************/

#ifndef  VTMX_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  VTMX_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  <stddef.h>			/* Standard C definitions. */
#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "gimx_util.h"			/* GIOP marshaling utilities. */


#if HAVE_NAMESPACES
    using  namespace  CoLi ;
    namespace  EvotCoLi {
#endif


/*******************************************************************************
    Auto-generated definitions - generated from the CORBA IDL files themselves.
*******************************************************************************/

#include  "vtmx_idl.h"			/* Auto-generated IDL definitions. */


#if HAVE_NAMESPACES
    } ;     /* DdsCoLi namespace. */
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
