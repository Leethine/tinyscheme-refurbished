/* $Id: rex_util.h,v 1.14 2023/05/24 12:34:01 alex Exp $ */
/*******************************************************************************

    rex_util.h

    Regular Expression Utility Definitions.

*******************************************************************************/

#ifndef  REX_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  REX_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */


/*******************************************************************************
    Compiled and Matched Regular Expressions (Client View).
*******************************************************************************/

typedef  struct  _CompiledRE  *CompiledRE ;

typedef  struct  _MatchedRE  *MatchedRE ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  rex_util_debug  OCD ("rex_util") ;


/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  errno_t  rexCompile P_((const char *expression,
                                CompiledRE *pattern))
    OCD ("rex_util") ;

extern  errno_t  rexCreate P_((const char *expression,
                               CompiledRE *pattern,
                               MatchedRE *match))
    OCD ("rex_util") ;

extern  errno_t  rexDestroy P_((CompiledRE pattern,
                                MatchedRE match))
    OCD ("rex_util") ;

extern  errno_t  rexDump P_((FILE *outfile, CompiledRE pattern))
    OCD ("rex_util") ;

extern  errno_t  rexGetGroup P_((MatchedRE match,
                                 size_t group,
                                 const char **groupStart,
                                 size_t *groupLength))
    OCD ("rex_util") ;

extern  bool  rexMatch P_((const char *target,
                           ssize_t length,
                           CompiledRE pattern,
                           MatchedRE match,
                           const char **matchStart,
                           size_t *matchLength))
    OCD ("rex_util") ;

extern  errno_t  rexMatchReset P_((MatchedRE match,
                                   CompiledRE pattern))
    OCD ("rex_util") ;

extern  size_t  rexNumGroups P_((CompiledRE pattern))
    OCD ("rex_util") ;

extern  errno_t  rexReplace P_((const char *source,
                                ssize_t length,
                                CompiledRE pattern,
                                const char *replacement,
                                ssize_t maxSubstitutions,
                                char **result,
                                size_t *numSubstitutions))
    OCD ("rex_util") ;

extern  errno_t  rexSubstitute P_((MatchedRE match,
                                   const char *source,
                                   ssize_t length,
                                   char **result))
    OCD ("rex_util") ;

extern  char  *rexWild P_((const char *wildcard))
    OCD ("rex_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
