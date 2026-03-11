/* $Id: ctype.h,v 1.1 2004/05/24 19:33:20 alex Exp alex $ */
/*******************************************************************************

    ctype.h

    PalmOS C Library Character-Classification Functions.

    The PalmOS SDK headers don't define these functions/macros.
    PRC-TOOLS does have "/usr/share/prc-tools/include/ctype.h",
    which, unfortunately, does NOT define the obsolete, but very
    much alive, isascii().  My LIBGPL REX_UTIL_Y parser needs
    isascii(), so here it is.

    NOTE: The macros defined here assume ASCII characters.

*******************************************************************************/

#ifndef  PALM_CTYPE_H		/* Has the file been INCLUDE'd already? */
#define  PALM_CTYPE_H  yes


#include  <PalmTypes.h>			/* Palm SDK header. */


#ifndef digittoint
#    define  digittoint(c) \
	(isdigit ((c)) ? ((c) - '0') \
	: (isxdigit ((c)) ? (toupper ((c)) - 'A' + 10) : 0))
#endif

#ifndef isalnum
#    define  isalnum(c) \
	(isalpha ((c)) || isdigit ((c)))
#endif

#ifndef isalpha
#    define  isalpha(c) \
	(islower ((c)) || isupper ((c)))
#endif

#ifndef isascii
#    define  isascii(c) \
	(('\0' <= (c)) && ((c) <= '\0177'))
#endif

#ifndef isblank
#    define  isblank(c) \
	(((c) == ' ') || ((c) == '\t'))
#endif

#ifndef iscntrl
#    define  iscntrl(c) \
	((('\0' <= (c)) && ((c) <= '\037')) || ((c) == '\0177'))
#endif

#ifndef isdigit
#    define  isdigit(c) \
	(('0' <= (c)) && ((c) <= '9'))
#endif

#ifndef isgraph
#    define  isgraph(c) \
	(('!' <= (c)) && ((c) <= '~'))
#endif

#ifndef islower
#    define  islower(c) \
	(('a' <= (c)) && ((c) <= 'z'))
#endif

#ifndef isprint
#    define  isprint(c) \
	(isgraph ((c)) || ((c) == ' '))
#endif

#ifndef ispunct
#    define  ispunct(c) \
	(isgraph((c)) && !isalnum ((c)))
#endif

#ifndef isspace
#    define  isspace(c) \
	(((c) == ' ') || ((c) == '\t') || ((c) == '\f') \
	|| ((c) == '\n') || ((c) == '\r') || ((c) == '\v'))
#endif

#ifndef isupper
#    define  isupper(c) \
	(('A' <= (c)) && ((c) <= 'Z'))
#endif

#ifndef isxdigit
#    define  isxdigit(c) \
	(isdigit ((c)) || (('a' <= (c)) && ((c) <= 'f')) \
	|| (('A' <= (c)) && ((c) <= 'F')))
#endif

#ifndef toascii
#    define  toascii(c) \
	((c) & 0x7F)
#endif

#ifndef tolower
#    define  tolower(c) \
	(isupper((c)) ? (((c) - 'A') + 'a') : (c))
#endif

#ifndef toupper
#    define  toupper(c) \
	(islower((c)) ? (((c) - 'a') + 'A') : (c))
#endif


#endif				/* If this file was not INCLUDE'd previously. */
