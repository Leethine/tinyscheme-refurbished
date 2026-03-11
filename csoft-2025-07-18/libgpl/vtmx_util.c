/* $Id$ */
/*******************************************************************************

File:

    vtmx_util.c

    Enhanced View of Time Service (EVOT) Marshaling Utilities.


Author:    Alex Measday


Purpose:

    The VTMX utilities are used to convert various Enhanced View of Time
    Service (EVOT) data types to and from the Common Data Representation
    (CDR) encodings defined for the General Inter-ORB Protocol (GIOP).
    (The primitive CDR types are handled by the COMX utilities.)


Public Procedures:

    vtmx<Type>() - decode/encode/erase EVOT types.
    .
    .
    .

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  "vtmx_util.h"			/* EVOT marshaling utilities. */


#if HAVE_NAMESPACES
    namespace  EvotCoLi {
#endif

/*******************************************************************************

Procedures:

    vtmx<Type> ()

    Decode/Encode/Erase EVOT Data Types.


Purpose:

    These automatically generated functions decode, encode, and erase
    EVOT data types.


    Invocation:

        status = vtmx<Type> (channel, &value) ;

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

#include  "vtmx_idl.c"		/* Auto-generated marshaling functions. */


#if HAVE_NAMESPACES
    } ;     /* EvotCoLi namespace. */
#endif
