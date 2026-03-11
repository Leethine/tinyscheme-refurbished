/* $Id: utf_util.h,v 1.3 2023/10/08 17:40:05 alex Exp $ */
/*******************************************************************************

    utf_util.h

    Unicode Transformation Format (UTF) Utilities.

*******************************************************************************/

#ifndef  UTF_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  UTF_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stddef.h>			/* Standard C definitions. */


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

#define  UTF_8_UNIT_BYTES  1		/* # of bytes in a Code Unit. */
#define  UTF_16_UNIT_BYTES  2
#define  UTF_16_PAIR_BYTES  4
#define  UTF_32_UNIT_BYTES  4

typedef  int32_t  UniCodePoint ;

				/* Zero Width Non-Breaking Space, U+FEFF;
				   used as the byte-order marker in UTF-8,
				   UTF-16, and UTF-32.  (0xEF, 0xBB, 0xBF
				   is the UTF-8 encoding of U+FEFF.) */
#define  UTF_ZWNBSP  INT32_C(0x0000FEFF)

#define  UTF_8_BOM_BYTES  3

/*******************************************************************************

    UTF Tags -

    A UTF tag is a bit-mask used by the UTF_UTIL functions to encode
    information about a UTF string.  Five bits identify attributes
    of a string:

                                 0xxx 00xx

    or more specifically:

        0<UTF-32><UTF-16><UTF-8> 00<endianness><BOM-present>

    or more readably:

        0000 0000    BOM absent		(bit 0)
        0000 0001    BOM present
        0000 0000    Big-endian		(bit 1)
        0000 0010    Little-endian
        0001 0000    UTF-8		(bit 4)
        0010 0000    UTF-16		(bit 5)
        0100 0000    UTF-32		(bit 6)

    Combine the settings with a bit-wise OR to get the desired attributes.
    For example, the tag for a BOM-prefixed, UTF-16, little-endian string
    is 0x23 = 0x20 (UTF-16) | 0x02 (little-endian) | 0x01 (BOM present).

*******************************************************************************/

typedef  uint16_t  TagUTF ;

/* Bit settings in a tag. */

#define  UTF_BOM_NO  0x00		/* bit 0 */
#define  UTF_BOM_YES  0x01
#define  UTF_BE  0x00			/* bit 1 */
#define  UTF_LE  0x02
#define  UTF_8  0x10			/* bit 4 */
#define  UTF_16  0x20			/* bit 5 */
#define  UTF_32  0x40			/* bit 6 */

#define  UTF_NULL_TAG  UTF_BOM_NO	/* UTF unknown, no BOM. */

/* UTF-8 tags. */
					/* UTF-8, no BOM. */
#define  UTF_8_NULL_TAG  UTF_8
					/* UTF-8 with BOM. */
#define  UTF_8_TAG  (UTF_8 | UTF_BOM_YES)

/* UTF-16 tags. */
					/* UTF-16, no BOM, big-endian. */
#define  UTF_16_NULL_TAG  UTF_16
					/* UTF-16, no BOM, little-endian. */
#define  UTF_16_NULL_TAG_LE  (UTF_16 | UTF_LE)
					/* UTF-16 with BOM, big-endian. */
#define  UTF_16_TAG_BE  (UTF_16 | UTF_BE | UTF_BOM_YES)
#define  UTF_16_TAG  UTF_16_TAG_BE
					/* UTF-16 with BOM, little-endian. */
#define  UTF_16_TAG_LE  (UTF_16 | UTF_LE | UTF_BOM_YES)

/* UTF-32 tags. */
					/* UTF-32, no BOM, big-endian. */
#define  UTF_32_NULL_TAG  UTF_32
					/* UTF-32, no BOM, little-endian. */
#define  UTF_32_NULL_TAG_LE  (UTF_32 | UTF_LE)
					/* UTF-32 with BOM, big-endian. */
#define  UTF_32_TAG_BE  (UTF_32 | UTF_BE | UTF_BOM_YES)
#define  UTF_32_TAG  UTF_32_TAG_BE
					/* UTF-32 with BOM, little-endian. */
#define  UTF_32_TAG_LE  (UTF_32 | UTF_LE | UTF_BOM_YES)

/* Macros to query properties in a tag. */

#define  IS_LITTLE_ENDIAN(tag)  ((tag) & UTF_LE)
#define  IS_BIG_ENDIAN(tag)  (!IS_LITTLE_ENDIAN(tag))

#define  BOM_IS_PRESENT(tag)  ((tag) & UTF_BOM_YES)
#define  BOM_IS_ABSENT(tag)  (!BOM_IS_PRESENT(tag))

#define  IS_UTF_8(tag)  ((tag) & UTF_8)
#define  IS_UTF_16(tag)  ((tag) & UTF_16)
#define  IS_UTF_32(tag)  ((tag) & UTF_32)

/*******************************************************************************
    Public functions.
*******************************************************************************/

extern  TagUTF  utfbom P_((const char *src,
                           ssize_t numBytes))
    OCD ("utf_util") ;

extern  size_t  utf16len P_((const char *src,
                             TagUTF *tag))
    OCD ("utf_util") ;

extern  size_t  utf16nlen P_((const char *src,
                              size_t maxUnits,
                              TagUTF *tag))
    OCD ("utf_util") ;

extern  size_t  utf32len P_((const char *src,
                             TagUTF *tag))
    OCD ("utf_util") ;

extern  size_t  utf32nlen P_((const char *src,
                              size_t maxUnits,
                              TagUTF *tag))
    OCD ("utf_util") ;

extern  UniCodePoint  utf8get P_((const char *src,
                                  size_t *numUnits))
    OCD ("utf_util") ;

extern  errno_t  utf8put P_((UniCodePoint codePoint,
                             char *dst,
                             size_t *numUnits))
    OCD ("utf_util") ;

extern  UniCodePoint  utf16get P_((TagUTF tag,
                                   const char *src))
    OCD ("utf_util") ;

extern  errno_t  utf16put P_((UniCodePoint codePoint,
                              TagUTF tag,
                              char *dst))
    OCD ("utf_util") ;

extern  UniCodePoint  utf32get P_((TagUTF tag,
                                   const char *src))
    OCD ("utf_util") ;

extern  errno_t  utf32put P_((UniCodePoint codePoint,
                              TagUTF tag,
                              char *dst))
    OCD ("utf_util") ;

extern  ssize_t  utf8to16 P_((const char *src,
                              ssize_t srclen,
                              bool insertBOM,
                              TagUTF dstTag,
                              size_t dstlen,
                              char *dst))
    OCD ("utf_util") ;

extern  ssize_t  utf8to32 P_((const char *src,
                              ssize_t srclen,
                              bool insertBOM,
                              TagUTF dstTag,
                              size_t dstlen,
                              char *dst))
    OCD ("utf_util") ;

extern  ssize_t  utf16to8 P_((const char *src,
                              ssize_t srclen,
                              TagUTF srcTag,
                              bool insertBOM,
                              size_t dstlen,
                              char *dst))
    OCD ("utf_util") ;

extern  ssize_t  utf32to8 P_((const char *src,
                              ssize_t srclen,
                              TagUTF srcTag,
                              bool insertBOM,
                              size_t dstlen,
                              char *dst))
    OCD ("utf_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
