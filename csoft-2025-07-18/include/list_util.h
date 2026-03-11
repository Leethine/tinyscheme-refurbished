/* $Id: list_util.h,v 1.8 2019/02/22 03:32:05 alex Exp $ */
/*******************************************************************************

    list_util.h

    List Manipulation Utility Definitions.

*******************************************************************************/

#ifndef  LIST_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  LIST_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    List Structure (Client View) and Definitions.
*******************************************************************************/

typedef  struct  ListNode  *List ;	/* List handle. */


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  listAdd P_((List *list,
                             ssize_t position,
                             void *item))
    OCD ("list_uti") ;

extern  void  *listDelete P_((List *list,
                              ssize_t position))
    OCD ("list_uti") ;

extern  ssize_t  listFind P_((List list,
                              void *item))
    OCD ("list_uti") ;

extern  void  *listGet P_((List list,
                           ssize_t position))
    OCD ("list_uti") ;

extern  ssize_t  listLength P_((List list))
    OCD ("list_uti") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
