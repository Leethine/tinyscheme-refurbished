/* $Id: bmw_util.h,v 1.11 2021/02/19 22:13:02 alex Exp $ */
/*******************************************************************************

    bmw_util.h

    Benchmarking Definitions.

*******************************************************************************/

#ifndef  BMW_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  BMW_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */


/*******************************************************************************
    Benchmarking Structures and Definitions.
*******************************************************************************/

typedef  struct  BmwClock {
    struct  timeval  startTime ;
    struct  timeval  stopTime ;
}  BmwClock ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  double  bmwElapsed P_((BmwClock *timer))
    OCD ("bmw_util") ;

extern  struct  timeval  bmwElapsedTV P_((BmwClock *timer))
    OCD ("bmw_util") ;

extern  double  bmwRate P_((BmwClock *timer,
                            size_t numItems))
    OCD ("bmw_util") ;

extern  void  bmwStart P_((BmwClock *timer))
    OCD ("bmw_util") ;

extern  void  bmwStop P_((BmwClock *timer))
    OCD ("bmw_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
