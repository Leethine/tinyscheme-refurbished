/* $Id: damx_util.c,v 1.3 2021/09/20 02:06:28 alex Exp $ */
/*******************************************************************************

File:

    damx_util.c

    Data Acquisition from Industrial Systems (DAIS) Marshaling Utilities.


Author:    Alex Measday


Purpose:

    The DAMX utilities are used to convert various Data Acquisition from
    Industrial Systems (DAIS) data types to and from the Common Data
    Representation (CDR) encodings defined for the General Inter-ORB
    Protocol (GIOP).  (The primitive CDR types are handled by the COMX
    utilities.)


Public Procedures:

    damx<Type>() - decode/encode/erase DAIS types.
    .
    .
    .

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  <wchar.h>			/* C Library wide string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "damx_util.h"			/* DAIS marshaling utilities. */


#if HAVE_NAMESPACES
    namespace  DaisCoLi {
#endif

/*******************************************************************************

Procedures:

    damx<Type> ()

    Decode/Encode/Erase DAIS Data Types.


Purpose:

    These automatically generated functions decode, encode, and erase
    DAIS data types.


    Invocation:

        status = damx<Type> (channel, &value) ;

    where

        <channel>	- I
            is the channel handle returned by comxCreate().
        <value>		- I/O
            is the address of the host value involved in the marshaling
            operation.  If the operation is MxDECODE, the data flow is
            from the CDR value (in the channel's buffer) to the host
            value.  If the operation is MxENCODE, the data flow is from
            the host value to the CDR value (in the channel's buffer).
            If the operation is MxERASE, the dynamically-allocated fields
            of the host value are deallocated.
        <status>	- O
            returns the status of performing the marshaling operation,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


/*******************************************************************************
    Auto-generated marshaling functions - generated from the auto-generated
        header file.
*******************************************************************************/

#include  "damx_idl.c"			/* Auto-generated marshaling functions. */


/*******************************************************************************
    Lookup Tables - for converting DAIS named constants to numbers and
        vice-versa.
*******************************************************************************/

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    const  ColiMap  SimpleValueTypeLUT[] = {
      { (long) RESOURCE_TYPE, "RESOURCE_TYPE" },
      { (long) URI_TYPE, "URI_TYPE" },
      { (long) STRING_TYPE, "STRING_TYPE" },
      { (long) BOOLEAN_TYPE, "BOOLEAN_TYPE" },
      { (long) INT_TYPE, "INT_TYPE" },
      { (long) UNSIGNED_TYPE, "UNSIGNED_TYPE" },
      { (long) DOUBLE_TYPE, "DOUBLE_TYPE" },
      { (long) COMPLEX_TYPE, "COMPLEX_TYPE" },
      { (long) DATE_TIME_TYPE, "DATE_TIME_TYPE" },
      { (long) ULONG_LONG_TYPE, "ULONG_LONG_TYPE" },
      { 0L, NULL }
    } ;
#endif


#if HAVE_NAMESPACES
    } ;     /* DaisCoLi namespace. */
#endif
