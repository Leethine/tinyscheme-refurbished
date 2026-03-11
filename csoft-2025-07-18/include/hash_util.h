/* $Id: hash_util.h,v 1.12 2019/05/25 03:34:13 alex Exp $ */
/*******************************************************************************

    hash_util.h

    Hash Table Definitions.

*******************************************************************************/

#ifndef  HASH_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  HASH_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  <stdio.h>			/* Standard I/O definitions. */
#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Hash Table Structures (Client View) and Definitions.
*******************************************************************************/

typedef  struct  _HashTable  *HashTable ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  hash_util_debug  OCD ("hash_uti") ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  hashAdd P_((HashTable table,
                             const char *key,
                             const void *data))
    OCD ("hash_uti") ;

extern  size_t  hashCount P_((HashTable table))
    OCD ("hash_uti") ;

extern  errno_t  hashCreate P_((size_t maxEntries,
                                HashTable *table))
    OCD ("hash_uti") ;

extern  errno_t  hashDelete P_((HashTable table,
                                const char *key))
    OCD ("hash_uti") ;

extern  errno_t  hashDestroy P_((HashTable table))
    OCD ("hash_uti") ;

extern  errno_t  hashDump P_((FILE *outfile,
                              const char *header,
                              HashTable table))
    OCD ("hash_uti") ;

extern  void  *hashFind P_((HashTable table,
                            const char *name))
    OCD ("hash_uti") ;

extern  const  char  *hashGet P_((HashTable table,
                                  size_t index,
                                  void **value))
    OCD ("hash_uti") ;

extern  bool  hashSearch P_((HashTable table,
                             const char *key,
                             void **data))
    OCD ("hash_uti") ;

#if !defined(HASH_UTIL_STATISTICS) || HASH_UTIL_STATISTICS
				/* Requires math library for sqrt(). */
extern  errno_t  hashStatistics P_((FILE *outfile,
                                    HashTable table))
    OCD ("hash_uti") ;
#else
#    define  hashStatistics(a1,a2)  ((errno_t) 0)	/* Dummy function. */
#endif


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
