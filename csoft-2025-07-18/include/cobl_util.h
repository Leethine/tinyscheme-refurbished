/* $Id: cobl_util.h,v 1.1 2024/02/20 19:27:02 alex Exp $ */
/*******************************************************************************

    cobl_util.h

    Collectable Object List (COBL) Utilities.

*******************************************************************************/

#ifndef  COBL_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  COBL_UTIL_H  yes

#ifdef __cplusplus
    extern  "C" {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Collectable Object List (Client View) and Definitions.
*******************************************************************************/

typedef  struct  _ColObjList  *ColObjList ;	/* List handle. */
						/* Function prototype for
						   object destruction. */
typedef  errno_t  (*ColObjDestroyFunc) P_((void *, void *)) ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

extern  int  cobl_util_debug ;		/* Global debug switch (1/0 = yes/no). */


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  coblAdd P_((ColObjList list,
                             void *object,
                             ColObjDestroyFunc destroy,
                             void *userData))
    OCD ("cobl_util") ;

extern  errno_t  coblCollect P_((ColObjList list))
    OCD ("cobl_util") ;

extern  errno_t  coblCollectTo P_((ColObjList list,
                                   void *marker))
    OCD ("cobl_util") ;

extern  size_t  coblCount P_((ColObjList list))
    OCD ("cobl_util") ;

extern  errno_t  coblCreate P_((ColObjDestroyFunc destroy,
                                void *userData,
                                ColObjList *list))
    OCD ("cobl_util") ;

extern  errno_t  coblDelete P_((ColObjList list,
                                void *object))
    OCD ("cobl_util") ;

extern  errno_t  coblDestroy P_((ColObjList list))
    OCD ("cobl_util") ;


#ifdef __cplusplus
    }
#endif

#endif				/* If this file was not INCLUDE'd previously. */
