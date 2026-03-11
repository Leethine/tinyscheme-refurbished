/* $Id: string.h,v 1.3 2005/02/12 00:07:08 alex Exp alex $ */
/*******************************************************************************

    string.h

    Standard C Library String Functions.

*******************************************************************************/

#ifndef  PALM_STRING_H		/* Has the file been INCLUDE'd already? */
#define  PALM_STRING_H  yes


#include  <MemoryMgr.h>			/* Palm SDK header. */
#include  <StringMgr.h>			/* Palm SDK header. */
#include  <SysUtils.h>			/* Palm SDK header. */
#include  <sys_types.h>			/* Palm SDK header. */
#include  <unix_string.h>		/* Palm SDK header. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "strerror.h"			/* strerror(3) function. */


/* For some reason, the #define's in Palm OS's <unix_string.h> cast the
   "src" arguments to StrCopy() and StrLen() as "(Char*)", when the Palm
   functions (and the C Library functions being defined) expect the
   arguments to be "const Char *".  I thought this was causing compiler
   warnings, but it turned out to be a mistake in my #define of strtol(3)
   and strtoul(3) in "stdlib.h" (in the same directory as this "string.h").
   I'll keep these redefinitions in place anyway. */

#undef  strcpy
#define  strcpy(dst,src) \
	(StrCopy (dst, src))

#undef  strlen
#define  strlen(src) \
	(StrLen (src))


/* The following functions are not #define'd in <unix_string.h>, so here
   they are.  The memmove(3) definition may look a little odd, but the
   Palm MemMove() function returns an error status whereas memmove(3)
   is supposed to return the destination pointer. */

#define  memmove(dst,src,len) \
	(MemMove (dst, src, len) ? dst : dst)

#define  strcasecmp(s1,s2) \
	StrCaselessCompare (s1, s2)

#define  strncasecmp(s1,s2,len) \
	StrNCaselessCompare (s1, s2, len)

#define  strncat(dst,src,len) \
	StrNCat (dst, src, len)

#define  strncmp(s1,s2,len) \
	StrNCompare (s1, s2, len)


#endif				/* If this file was not INCLUDE'd previously. */
