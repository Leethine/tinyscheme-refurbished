/* $Id: nnl_util.h,v 1.2 2021/04/21 01:47:45 alex Exp $ */
/*******************************************************************************

    nnl_util.h

    Name-to-Number Lookup Utilities.

*******************************************************************************/

#ifndef  NNL_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  NNL_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Lookup Table Entry.
*******************************************************************************/

typedef  struct  NNLMap {		/* For name/number mappings. */
    long  number ;
    const  char  *name ;
}  NNLMap, NNLTable[] ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  size_t  nnlCount P_((const NNLTable table))
    OCD ("nnl_util") ;

extern  const  char  *nnlNameOf P_((const NNLTable table,
                                    long number))
    OCD ("nnl_util") ;

extern  long  nnlNumberOf P_((const NNLTable table,
                              const char *name,
                              ssize_t length,
                              bool ignoreCase,
                              bool partialMatch))
    OCD ("nnl_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
