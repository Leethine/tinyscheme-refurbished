/* $Id: sto_util.h,v 1.2 2021/09/14 16:05:50 alex Exp $ */
/*******************************************************************************

    sto_util.h

    Length-Limited String to Number Conversion Functions.

*******************************************************************************/

#ifndef  STO_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  STO_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  sto_util_debug  OCD ("sto_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

#if defined(HAVE_ANTOF) && !HAVE_ANTOF
extern  double  antof P_((const char *substring,
                          size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_ANTOI) && !HAVE_ANTOI
extern  int  antoi P_((const char *substring,
                       size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_ANTOL) && !HAVE_ANTOL
extern  long  antol P_((const char *substring,
                        size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_ANTOLL) && !HAVE_ANTOLL
extern  long  long  antoll P_((const char *substring,
                               size_t length))
    OCD ("sto_util") ;
#endif


#if defined(HAVE_STRNTOL) && !HAVE_STRNTOL
extern  long  strntol P_((const char *substring,
                          char **endptr,
                          int base,
                          size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_STRNTOUL) && !HAVE_STRNTOUL
extern  unsigned  long  strntoul P_((const char *substring,
                                     char **endptr,
                                     int base,
                                     size_t length))
    OCD ("sto_util") ;
#endif


#if defined(HAVE_STRNTOLL) && !HAVE_STRNTOLL
extern  long  long  strntoll P_((const char *substring,
                                 char **endptr,
                                 int base,
                                 size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_STRNTOULL) && !HAVE_STRNTOULL
extern  unsigned  long  long  strntoull P_((const char *substring,
                                            char **endptr,
                                            int base,
                                            size_t length))
    OCD ("sto_util") ;
#endif


#if defined(HAVE_STRNTOF) && !HAVE_STRNTOF
extern  float  strntof P_((const char *substring,
                           char **endptr,
                           size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_STRNTOD) && !HAVE_STRNTOD
extern  double  strntod P_((const char *substring,
                            char **endptr,
                            size_t length))
    OCD ("sto_util") ;
#endif

#if defined(HAVE_STRNTOLD) && !HAVE_STRNTOLD
#    if !defined(HAVE_STRTOLD) || HAVE_STRTOLD
extern  long  double  strntold P_((const char *substring,
                                   char **endptr,
                                   size_t length))
    OCD ("sto_util") ;
#    endif
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
