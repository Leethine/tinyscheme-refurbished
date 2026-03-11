/* $Id: gq_util.h,v 1.2 2023/05/24 16:38:13 alex Exp $ */
/*******************************************************************************

    gq_util.h

    Generalized Queue (GQ) Utilities.

*******************************************************************************/

#ifndef  GQ_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  GQ_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Generalized Queue (Client View) and Definitions.
*******************************************************************************/

					/* Queue handle. */
typedef  struct  _GeneralQ  *GeneralQ,
                            *GqQueue,
                            *GqDeque,
                            *GqStack ;

#define  GQFRONT  (0)			/* Queue positions. */
#define  GQREAR  (-1)


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  gq_util_debug  OCD ("srt_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  gqAdd P_((GeneralQ queue,
                           ssize_t position,
                           void *item))
    OCD ("gq_util") ;

#define  gqAddFront(q,i)  gqAdd ((q), GQFRONT, i)
#define  gqAddRear(q,i)  gqAdd ((q), GQREAR, i)
#define  gqPush(q,i)  gqAdd ((q), GQFRONT, i)	/* For stacks. */

extern  size_t  gqCount P_((GeneralQ queue))
    OCD ("gq_util") ;

extern  errno_t  gqCreate P_((ssize_t maxItems,
                              GeneralQ *queue))
    OCD ("gq_util") ;

extern  errno_t  gqDestroy P_((GeneralQ queue))
    OCD ("gq_util") ;

extern  errno_t  gqErase P_((GeneralQ queue))
    OCD ("gq_util") ;

extern  void  *gqGet P_((GeneralQ queue,
                         ssize_t position))
    OCD ("gq_util") ;

#define  gqGetFront(q)  gqGet ((q), GQFRONT)
#define  gqGetRear(q)  gqGet ((q), GQREAR)
#define  gqTop(q)  gqGet ((q), GQFRONT)		/* For stacks. */

extern  void  *gqRemove P_((GeneralQ queue,
                            ssize_t position))
    OCD ("gq_util") ;

#define  gqRemoveFront(q)  gqRemove ((q), GQFRONT)
#define  gqRemoveRear(q)  gqRemove ((q), GQREAR)
#define  gqPop(q)  gqRemove ((q), GQFRONT)	/* For stacks. */


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
