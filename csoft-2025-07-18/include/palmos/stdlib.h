/* $Id: stdlib.h,v 1.2 2004/12/30 21:41:00 alex Exp alex $ */
/*******************************************************************************

    stdlib.h

    Standard C Library Functions.

*******************************************************************************/

#ifndef  PALM_STDLIB_H		/* Has the file been INCLUDE'd already? */
#define  PALM_STDLIB_H  yes


#include  <FloatMgr.h>			/* Palm SDK header. */
#include  <MemoryMgr.h>			/* Palm SDK header. */
#include  <StringMgr.h>			/* Palm SDK header. */
#include  <unix_stdlib.h>		/* Palm SDK header. */
#include  "std_util.h"			/* Missing Standard C functions. */


#define  strtof(nptr,endptr) \
	((float) strtod ((nptr), (endptr)))


#ifdef NOT_NOW
/* Use atol(3) for the strtol(3) and strtoul(3) functions; note that they
   always succeed.  The first part of each definition sets the "endptr"
   return value; the second part, after the comma operator, performs the
   actual conversion with a call to atol(3). */

#define  strtol(nptr,endptr,base)	\
(					\
    (((endptr) != NULL) && (		\
        *((char **) (endptr)) = (	\
            (*((char **) (endptr)) = (char *) (nptr) + strspn ((nptr), " \t")),	\
            (*((char **) (endptr)) += strspn (*((const char **) (endptr)), "-0123456789"))	\
        )				\
    )),					\
    (long) atol ((nptr))		\
)

#define  strtoul(nptr,endptr,base)	\
(					\
    (((endptr) != NULL) && (		\
        *((char **) (endptr)) = (	\
            (*((char **) (endptr)) = (char *) (nptr) + strspn ((nptr), " \t")),	\
            (*((char **) (endptr)) += strspn (*((const char **) (endptr)), "0123456789"))	\
        )				\
    )),					\
    (unsigned long) atol ((nptr))	\
)
#else
    extern  long  strtol P_((const char *nptr, char **endptr, int base)) ;
    extern  unsigned  long  strtoul P_((const char *nptr, char **endptr, int base)) ;
#endif


#endif				/* If this file was not INCLUDE'd previously. */
