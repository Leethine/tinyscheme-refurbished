/* $Id: damx_util.h,v 1.3 2021/09/15 04:18:16 alex Exp $ */
/*******************************************************************************

    damx_util.h

    Data Acquisition from Industrial Systems (DAIS) Marshaling Utilities.

*******************************************************************************/

#ifndef  DAMX_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  DAMX_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  <stddef.h>			/* Standard C definitions. */
#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "gimx_util.h"			/* GIOP marshaling utilities. */


#if HAVE_NAMESPACES
    using  namespace  CoLi ;
    namespace  DaisCoLi {
#endif


/*******************************************************************************
    Auto-generated definitions - generated from the CORBA IDL files themselves.
*******************************************************************************/

#include  "damx_idl.h"			/* Auto-generated IDL definitions. */


/*******************************************************************************
    Additional tables for mapping "#define"d values to names and vice-versa;
    see the coliToName() and coliToNumber() functions.  The "static" tables
    need to be updated when new DAIS versions are issued.
*******************************************************************************/

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    extern  const  ColiMap  SimpleValueTypeLUT[]  OCD ("DAFDescr") ;
#endif


#if HAVE_NAMESPACES
    } ;     /* DaisCoLi namespace. */
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
