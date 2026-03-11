/* $Id: drs_util.h,v 1.7 2019/02/22 07:13:13 alex Exp $ */
/*******************************************************************************

    drs_util.h

    Directory Scanning Utility Definitions.

*******************************************************************************/

#ifndef  DRS_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  DRS_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Directory Scan (Client View) and Definitions.
*******************************************************************************/

					/* Scan handle. */
typedef  struct  _DirectoryScan  *DirectoryScan ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  drs_util_debug  OCD ("drs_util") ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  size_t  drsCount P_((DirectoryScan scan))
    OCD ("drs_util") ;

extern  errno_t  drsCreate P_((const char *pathname,
                               DirectoryScan *scan))
    OCD ("drs_util") ;

extern  errno_t  drsDestroy P_((DirectoryScan scan))
    OCD ("drs_util") ;

extern  const  char  *drsFirst P_((DirectoryScan scan))
    OCD ("drs_util") ;

extern  const  char  *drsGet P_((DirectoryScan scan,
                                 size_t which))
    OCD ("drs_util") ;

extern  const  char  *drsNext P_((DirectoryScan scan))
    OCD ("drs_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
