/* $Id: tpl_util.h,v 1.6 2021/07/21 13:09:35 alex Exp $ */
/*******************************************************************************

    tpl_util.h

    Tuple Manipulation Definitions.

*******************************************************************************/

#ifndef  TPL_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  TPL_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Tuple (Client View) and Definitions.
*******************************************************************************/

typedef  struct  _Tuple  *Tuple ;	/* Tuple handle. */


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  tpl_util_debug  OCD ("tpl_util") ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  size_t  tplArity P_((Tuple tuple)) ;

extern  Tuple  tplCreate P_((size_t arity,
                             ...)) ;

extern  errno_t  tplDestroy P_((Tuple tuple)) ;

extern  void  *tplGet P_((Tuple tuple,
                          size_t index)) ;

extern  errno_t  tplSet P_((Tuple tuple,
                            size_t index,
                            void *value)) ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
