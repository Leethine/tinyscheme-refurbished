/* $Id: utf_util.c,v 1.5 2023/10/08 17:40:15 alex Exp $ */
/*******************************************************************************

File:

    utf_util.c

    Unicode Transformation Format (UTF) Utilities.


Author:    Alex Measday


Purpose:

    The UTF utilities are used to convert between 8-, 16-, and 32-bit Unicode
    Transformation Format strings.  The Unicode character space is 21 bits
    wide.  Consequently, the 8-bit encoding (UTF-8) requires from 1 to 4
    bytes to represent a Unicode character.  The 16-bit encoding (UTF-16)
    requires one or two 16-bit code units.  32-bit encoding (UTF-32), of
    course, can represent any Unicode character as is.

    UTF-16 and UTF-32 encodings arrange bytes in most significant to least
    significant (big-endian) order or least significant to most significant
    (little-endian) order.  A byte-order marker (BOM) may be inserted at the
    beginning of an encoded string to indicate the byte order; if there is no
    BOM, big-endian order is assumed.

    UTF-8 strings need no BOM, but some applications look for a BOM at the
    beginning of a file so as to detect that the file is UTF-8 as opposed
    to straight ASCII/Latin.

    The conversion functions automatically append a NUL-/null-terminator to
    converted strings: 0x00 for UTF-8, 0x0000 for UTF-16, and 0x00000000 for
    UTF-32 strings.  (Assuming enough room in the destination buffer.)

    The UTF_UTIL package was written as a small, self-contained set of
    straightforward functions intended for converting Unicode strings
    in MP3 ID3v2 tags.  As such, portability was of higher concern than
    efficiency.  If you're doing a lot of Unicode string handling, you
    might wish to research some of the full-featured Unicode libraries.


Public Procedures:

    utfbom() - returns the UTF-8, UTF-16, or UTF-32 byte-order marker,
        if any, at the beginning of an arbitrary string.
    utf16bom() - returns a UTF-16 byte-order marker indication.
    utf32bom() - returns a UTF-32 byte-order marker indication.
    utf16len() - returns the number of code units in a UTF-16 string.
    utf16nlen() - returns the # of code units in a length-limited UTF-16 string.
    utf32len() - returns the number of code units in a UTF-32 string.
    utf32nlen() - returns the # of code units in a length-limited UTF-32 string.
    utf8get() - decodes a code point from a UTF-8 string.
    utf8put() - encodes a code point to a UTF-8 string.
    utf16get() - decodes a code point from a UTF-16 string.
    utf16put() - encodes a code point to a UTF-16 string.
    utf32get() - decodes a code point from a UTF-32 string.
    utf32put() - encodes a code point to a UTF-32 string.
    utf8to16() - converts a UTF-8 string to a UTF-16 string.
    utf8to32() - converts a UTF-8 string to a UTF-32 string.
    utf16to8() - converts a UTF-16 string to a UTF-8 string.
    utf32to8() - converts a UTF-32 string to a UTF-8 string.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* Standard C string functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */
#include  "meo_util.h"			/* Memory operations; debug only. */

/*!*****************************************************************************

Procedure:

    utfbom ()

    Check for a UTF-8, UTF-16, or UTF-32 Byte-Order Marker in a String.


Purpose:

    Function utfbom() checks the beginning of an arbitrary string for a
    UTF-32, UTF-16, or UTF-8 byte-order marker.  The byte-order marker,
    which specifies the width (UTF-8, UTF-16, or UTF-32) and endianness
    (little endian or big endian) of the code points, is returned to the
    calling program.  It is assumed that the deprecated ZWNBSP character
    (U+FEFF) is not used at the beginning of the string.


    Invocation:

        tag = utfbom (src, numBytes) ;

    where

        <src>		- I
            is an arbitrary string.
        <numBytes>	- I
            is the length in bytes of the string.  It is not necessary to
            specify the full length of the string, just one of the following
            minimum lengths:
                4 bytes - to check for UTF-32, UTF-8, and UTF-16 BOMs.
                3 bytes - to check for UTF-8 and UTF-16 BOMs.
                2 bytes - to check for UTF-16 BOMs.
            For convenience, -1 can be specified for this argument if the
            caller knows that the string is at least 4 bytes in length.
        <tag>		- O
            returns a UTF tag with detected attributes of the string.  If the
            string has a UTF-8, UTF-16, or UTF-32 byte-order marker, the tag
            will indicate the UTF encoding used, the endianness of the string
            (if applicable), and that a BOM is present.  If the string has
            no byte-order marker, UTF_NULL_TAG is returned, indicating that
            the UTF encoding is unknown and that there is no BOM.

*******************************************************************************/


TagUTF  utfbom (

#    if PROTOTYPES
        const  char  *src,
        ssize_t  numBytes)
#    else
        src, numBytes)

        const  char  *src ;
        ssize_t  numBytes ;
#    endif

{

    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utfbom) NULL string: ") ;
        return (UTF_NULL_TAG) ;
    }

    if (numBytes < 0)  numBytes = 4 ;	/* String long enough for UTF-32 BOM. */

/* First, check for a 4-byte UTF-32 byte-order marker. */

    if (numBytes >= 4) {

        if ((unsigned char) src[0] == 0x00) {

            if (((unsigned char) src[1] == 0x00) &&
                ((unsigned char) src[2] == 0xFE) &&
                ((unsigned char) src[3] == 0xFF)) {
                return (UTF_32_TAG_BE) ;	/* UTF-32, big endian. */
            }

        } else if ((unsigned char) src[0] == 0xFF) {

            if (((unsigned char) src[1] == 0xFE) &&
                ((unsigned char) src[2] == 0x00) &&
                ((unsigned char) src[3] == 0x00)) {
                return (UTF_32_TAG_LE) ;	/* UTF-32, little endian. */
            }

        }

    }


/* Next, check for a 3-byte UTF-8 byte-order marker. */

    if (numBytes >= 3) {

        if (((unsigned char) src[0] == 0xEF) &&
            ((unsigned char) src[1] == 0xBB) &&
            ((unsigned char) src[2] == 0xBF)) {
            return (UTF_8_TAG) ;		/* UTF-8. */
        }

    }

/* Finally, check for a 2-byte UTF-16 byte-order marker.  NOTE that this
   check is *last*.  The first two bytes of little-endian UTF-16 and UTF-32
   byte-order markers are identical (0xFFFE), so the longer, 4-byte UTF-32
   byte-order-marker is checked for first (see above). */

    if (numBytes >= 2) {

        if ((unsigned char) src[0] == 0xFE) {
            if ((unsigned char) src[1] == 0xFF)
                return (UTF_16_TAG_BE) ;	/* UTF-16, big endian. */
        } else if ((unsigned char) src[0] == 0xFF) {
            if ((unsigned char) src[1] == 0xFE)
                return (UTF_16_TAG_LE) ;	/* UTF-16, little endian. */
        }

    }


    return (UTF_NULL_TAG) ;			/* No BOM. */

}

/*!*****************************************************************************

Procedure:

    utf16len ()

    Count the Number of Code Units in a UTF-16 String.


Purpose:

    Function utf16len() returns the number of 16-bit code units in a
    null-terminated (0x0000) UTF-16 string.


    Invocation:

        number = utf16len (src, &tag) ;

    where

        <src>		- I
            is a null-terminated UTF-16 string.
        <tag>	- O
            returns a UTF tag indicating, if a BOM is present, the byte order
            of the string, big-endian or little-endian; UTF_16_NULL_TAG is
            returned if there is no BOM.  Since the BOM is NOT included in
            the returned string length, "BOM_IS_PRESENT(tag)" can be used
            by the caller to determine if a BOM was detected.  If this
            argument is NULL, no tag is returned.
        <number>	- O
            returns the number of 16-bit code units in the UTF-16 string,
            NOT including the BOM if present.

*******************************************************************************/


size_t  utf16len (

#    if PROTOTYPES
        const  char  *src,
        TagUTF  *tag)
#    else
        src, tag)

        const  char  *src ;
        TagUTF  *tag ;
#    endif

{    /* Local variables. */
    size_t  i ;
    TagUTF  temp ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16len) NULL string: ") ;
        return (0) ;
    }

    if (tag == NULL)  tag = &temp ;
    *tag = UTF_16_NULL_TAG ;

					/* Scan for 2 consecutive zero bytes. */
    for (i = 0 ;  ;  i += UTF_16_UNIT_BYTES) {
        if ((src[i] == 0) && (src[i+1] == 0))  break ;
    }

    if (i >= UTF_16_UNIT_BYTES) {	/* Subtract 2-byte BOM from length? */
        *tag = utfbom (src, UTF_16_UNIT_BYTES) ;
        if (BOM_IS_PRESENT (*tag))  i -= UTF_16_UNIT_BYTES ;
    }

    return (i / UTF_16_UNIT_BYTES) ;

}

/*!*****************************************************************************

Procedure:

    utf16nlen ()

    Count the Number of Code Units in a Length-Limited UTF-16 String.


Purpose:

    Function utf16nlen() returns the number of 16-bit code units in a
    null-terminated/length-limited UTF-16 string.  If the string is
    null-terminated (0x0000) within the bounds of the string, then
    that length is returned to the caller.  If a null terminator is
    not encountered within the bounds of the string, then the maximum
    length of the string is returned.


    Invocation:

        number = utf16nlen (src, maxUnits, &tag) ;

    where

        <src>		- I
            is a null-terminated UTF-16 string.
        <maxUnits>	- I
            is the maximum number of 16-bit code units to examine while
            looking for a null terminator.
        <tag>	- O
            returns a UTF tag indicating, if a BOM is present, the byte order
            of the string, big-endian or little-endian; UTF_16_NULL_TAG is
            returned if there is no BOM.  Since the BOM is NOT included in
            the returned string length, "BOM_IS_PRESENT(tag)" can be used
            by the caller to determine if a BOM was detected.  If this
            argument is NULL, no tag is returned.
        <number>	- O
            returns the number of 16-bit code units in the UTF-16 string,
            NOT including the BOM if present.  If no null terminator is
            found, then the maximum-units argument is returned.

*******************************************************************************/


size_t  utf16nlen (

#    if PROTOTYPES
        const  char  *src,
        size_t  maxUnits,
        TagUTF  *tag)
#    else
        src, maxUnits, tag)

        const  char  *src ;
        size_t  maxUnits ;
        TagUTF  *tag ;
#    endif

{    /* Local variables. */
    size_t  i, maxChars ;
    TagUTF  temp ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16nlen) NULL string: ") ;
        return (0) ;
    }

    if (tag == NULL)  tag = &temp ;
    *tag = UTF_16_NULL_TAG ;

/* Scan for 2 consecutive zero bytes.  Note that "maxChars" is always even,
   so "src[i+1]" will NOT reference a character past the end of the string. */

    maxChars = maxUnits * UTF_16_UNIT_BYTES ;

    for (i = 0 ;  i < maxChars ;  i += UTF_16_UNIT_BYTES) {
        if ((src[i] == 0) && (src[i+1] == 0))  break ;
    }

/* Subtract the 2-byte BOM, if present, from the length. */

    if (i >= UTF_16_UNIT_BYTES) {
        *tag = utfbom (src, UTF_16_UNIT_BYTES) ;
        if (BOM_IS_PRESENT (*tag))  i -= UTF_16_UNIT_BYTES ;
    }

    return (i / UTF_16_UNIT_BYTES) ;

}

/*!*****************************************************************************

Procedure:

    utf32len ()

    Count the Number of Code Units in a UTF-32 String.


Purpose:

    Function utf32len() returns the number of 32-bit code units in a
    null-terminated (0x00000000) UTF-32 string.


    Invocation:

        number = utf32len (src, &tag) ;

    where

        <src>		- I
            is a UTF-32 string.
        <tag>	- O
            returns a UTF tag indicating, if a BOM is present, the byte order
            of the string, big-endian or little-endian; UTF_32_NULL_TAG is
            returned if there is no BOM.  Since the BOM is NOT included in
            the returned string length, "BOM_IS_PRESENT(tag)" can be used
            by the caller to determine if a BOM was detected.  If this
            argument is NULL, no tag is returned.
        <number>	- O
            returns the number of 32-bit code units in the UTF-32 string,
            NOT including the BOM if present.

*******************************************************************************/


size_t  utf32len (

#    if PROTOTYPES
        const  char  *src,
        TagUTF  *tag)
#    else
        src, tag)

        const  char  *src ;
        TagUTF  *tag ;
#    endif

{    /* Local variables. */
    size_t  i ;
    TagUTF  temp ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32len) NULL string: ") ;
        return (0) ;
    }

    if (tag == NULL)  tag = &temp ;
    *tag = UTF_32_NULL_TAG ;
					/* Scan for 4 consecutive zero bytes. */
    for (i = 0 ;  ;  i += UTF_32_UNIT_BYTES) {
        if ((src[i] == 0) && (src[i+1] == 0) &&
            (src[i+2] == 0) && (src[i+3] == 0))  break ;
    }

    if (i >= UTF_32_UNIT_BYTES) {	/* Subtract 4-byte BOM from length? */
        *tag = utfbom (src, UTF_32_UNIT_BYTES) ;
        if (BOM_IS_PRESENT (*tag))  i -= UTF_32_UNIT_BYTES ;
    }

    return (i / UTF_32_UNIT_BYTES) ;

}

/*!*****************************************************************************

Procedure:

    utf32nlen ()

    Count the Number of Code Units in a Length-Limited UTF-32 String.


Purpose:

    Function utf32nlen() returns the number of 32-bit code units in
    a null-terminated/length-limited UTF-32 string.  If the string
    is null-terminated (0x00000000) within the bounds of the string,
    then that length is returned to the caller.  If a null terminator
    is not encountered within the bounds of the string, then the
    maximum length of the string is returned.


    Invocation:

        number = utf32nlen (src, maxUnits, &tag) ;

    where

        <src>		- I
            is a null-terminated UTF-32 string.
        <maxUnits>	- I
            is the maximum number of 32-bit code units to examine while
            looking for a null terminator.
        <tag>	- O
            returns a UTF tag indicating, if a BOM is present, the byte order
            of the string, big-endian or little-endian; UTF_32_NULL_TAG is
            returned if there is no BOM.  Since the BOM is NOT included in
            the returned string length, "BOM_IS_PRESENT(tag)" can be used
            by the caller to determine if a BOM was detected.  If this
            argument is NULL, no tag is returned.
        <number>	- O
            returns the number of 32-bit code units in the UTF-32 string,
            NOT including the BOM if present.  If no null terminator is
            found, then the maximum-units argument is returned.

*******************************************************************************/


size_t  utf32nlen (

#    if PROTOTYPES
        const  char  *src,
        size_t  maxUnits,
        TagUTF  *tag)
#    else
        src, maxUnits, tag)

        const  char  *src ;
        size_t  maxUnits ;
        TagUTF  *tag ;
#    endif

{    /* Local variables. */
    size_t  i, maxChars ;
    TagUTF  temp ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32nlen) NULL string: ") ;
        return (0) ;
    }

    if (tag == NULL)  tag = &temp ;
    *tag = UTF_32_NULL_TAG ;

/* Scan for 4 consecutive zero bytes.  Note that "maxChars" is always
   a multiple of 4, so the "src[i+...]"s will NOT reference characters
   past the end of the string. */

    maxChars = maxUnits * UTF_32_UNIT_BYTES ;

    for (i = 0 ;  i < maxChars ;  i += UTF_32_UNIT_BYTES) {
        if ((src[i] == 0) && (src[i+1] == 0) &&
            (src[i+2] == 0) && (src[i+3] == 0))  break ;
    }

/* Subtract the 4-byte BOM, if present, from the length. */

    if (i >= UTF_32_UNIT_BYTES) {
        *tag = utfbom (src, UTF_32_UNIT_BYTES) ;
        if (BOM_IS_PRESENT (*tag))  i -= UTF_32_UNIT_BYTES ;
    }

    return (i / UTF_32_UNIT_BYTES) ;

}

/*!*****************************************************************************

Procedure:

    utf8get ()

    Get the Next Code Point from a UTF-8 String.


Purpose:

    Function utf8get() decodes and returns the next code point from a UTF-8
    string.  The length of the octet sequence consumed in the process is
    returned, allowing the caller to easily step through an entire UTF-8 string.


    Invocation:

        codePoint = utf8get (src, &numUnits) ;

    where

        <src>		- I
            is an octet sequence in a UTF-8 string.
        <numUnits>	- O
            returns the number (1-4) of UTF-8 code units consumed.
        <codePoint>	- O
            returns the Unicode code point constructed from the UTF-8 octet
            sequence.  -1 is returned in the event of an error.

*******************************************************************************/


UniCodePoint  utf8get (

#    if PROTOTYPES
        const  char  *src,
        size_t  *numUnits)
#    else
        src, numUnits)

        const  char  *src ;
        size_t  *numUnits ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint ;



    if (numUnits != NULL)  *numUnits = 0 ;

    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8get) NULL string: ") ;
        return (-1) ;
    }

/* One-octet code point (Unicode range U+000000 to U+00007F)? */

    if ((src[0] & 0x80) == 0x00) {

        codePoint = src[0] ;
        if (numUnits != NULL)  *numUnits = 1 ;

    }

/* Two-octet code point (Unicode range U+000080 to U+0007FF)? */

    else if ((src[0] & 0xE0) == 0xC0) {

        codePoint = src[0] & 0x1F ;
        codePoint <<= 6 ;
        if ((src[1] & 0xC0) != 0x80) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf8get) Invalid code unit: 0x%02X\n", src[1]) ;
        }
        codePoint |= src[1] & 0x3F ;
        if (numUnits != NULL)  *numUnits = 2 ;

    }

/* Three-octet code point (Unicode range U+000800 to U+00FFFF)? */

    else if ((src[0] & 0xF0) == 0xE0) {

        codePoint = src[0] & 0x0F ;
        codePoint <<= 6 ;
        if ((src[1] & 0xC0) != 0x80) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf8get) Invalid code unit: 0x%02X\n", src[1]) ;
        }
        codePoint |= src[1] & 0x3F ;
        codePoint <<= 6 ;
        if ((src[2] & 0xC0) != 0x80) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf8get) Invalid code unit: 0x%02X\n", src[2]) ;
        }
        codePoint |= src[2] & 0x3F ;
        if (numUnits != NULL)  *numUnits = 3 ;

    }

/* Four-octet code point (Unicode range U+010000 to U+10FFFF)? */

    else if ((src[0] & 0xF8) == 0xF0) {

        codePoint = src[0] & 0x07 ;
        codePoint <<= 6 ;
        if ((src[1] & 0xC0) != 0x80) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf8get) Invalid code unit: 0x%02X\n", src[1]) ;
        }
        codePoint |= src[1] & 0x3F ;
        codePoint <<= 6 ;
        if ((src[2] & 0xC0) != 0x80) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf8get) Invalid code unit: 0x%02X\n", src[2]) ;
        }
        codePoint |= src[2] & 0x3F ;
        codePoint <<= 6 ;
        if ((src[3] & 0xC0) != 0x80) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf8get) Invalid code unit: 0x%02X\n", src[3]) ;
        }
        codePoint |= src[3] & 0x3F ;
        if (numUnits != NULL)  *numUnits = 4 ;

    }

    else {

        SET_ERRNO (EINVAL) ;
        LGE "(utf8get) Invalid initial code unit: 0x%02X\n", src[0]) ;
        return (-1) ;

    }

    return (codePoint) ;

}

/*!*****************************************************************************

Procedure:

    utf8put ()

    Put a Code Point in a UTF-8 String.


Purpose:

    Function utf8put() encodes and adds a Unicode code point to a UTF-8 string.
    The length of the octet sequence generated in the process is returned,
    allowing the caller to easily build an entire UTF-8 string.


    Invocation:

        status = utf8put (codePoint, dst, &numUnits) ;

    where

        <codePoint>	- I
            is the Unicode code point to be encoded.
        <dst>		- O
            is the address in a UTF-8 string at which the code point is to be
            stored.
        <numUnits>	- O
            returns the number (1-4) of UTF-8 code units generated.
        <status>	- O
            returns the status of encoding the code point, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  utf8put (

#    if PROTOTYPES
        UniCodePoint  codePoint,
        char  *dst,
        size_t  *numUnits)
#    else
        codePoint, dst, numUnits)

        UniCodePoint  codePoint ;
        char  *dst ;
        size_t  *numUnits ;
#    endif

{

    if (numUnits != NULL)  *numUnits = 0 ;

    if ((codePoint < 0) || (0x10FFFF < codePoint)) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8put) Invalid code point: U+%06lX\n", (long) codePoint) ;
        return (errno) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8put) NULL destination string: ") ;
        return (errno) ;
    }

/* One-octet code point (Unicode range U+000000 to U+00007F)? */

    if (codePoint < 0x80) {

        dst[0] = codePoint & 0x7F ;
        if (numUnits != NULL)  *numUnits = 1 ;

    }

/* Two-octet code point (Unicode range U+000080 to U+0007FF)? */

    else if (codePoint < 0x800) {

        dst[0] = 0xC0 | ((codePoint & 0x07C0) >> 6) ;
        dst[1] = 0x80 |  (codePoint & 0x003F) ;
        if (numUnits != NULL)  *numUnits = 2 ;

    }

/* Three-octet code point (Unicode range U+000800 to U+00FFFF)? */

    else if (codePoint < 0x10000) {

        dst[0] = 0xE0 | ((codePoint & 0x0F000) >> 12) ;
        dst[1] = 0x80 | ((codePoint & 0x00FC0) >> 6) ;
        dst[2] = 0x80 |  (codePoint & 0x0003F) ;
        if (numUnits != NULL)  *numUnits = 3 ;

    }

/* Four-octet code point (Unicode range U+010000 to U+10FFFF)? */

    else {

        dst[0] = 0xF0 | ((codePoint & 0x1C0000) >> 18) ;
        dst[1] = 0x80 | ((codePoint & 0x03F000) >> 12) ;
        dst[2] = 0x80 | ((codePoint & 0x000FC0) >> 6) ;
        dst[3] = 0x80 |  (codePoint & 0x00003F) ;
        if (numUnits != NULL)  *numUnits = 4 ;

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    utf16get ()

    Get the Next Code Point from a UTF-16 String.


Purpose:

    Function utf16get() decodes and returns the next code point from a UTF-16
    string.  The number of 16-bit code units or bytes consumed is easily
    determined from the value of the code point:

        numUnits = (codePoint < 0x10000) ? 1 : 2 ;
        numBytes = (codePoint < 0x10000) ? UTF_16_UNIT_BYTES : UTF_16_PAIR_BYTES ;


    Invocation:

        codePoint = utf16get (tag, src) ;

    where

        <tag>	- I
            is a UTF tag indicating the byte order of the string: big-endian
            or little-endian.
        <src>		- I
            is a pointer to a code unit or pair in a UTF-16 string.
        <codePoint>	- O
            returns the Unicode code point constructed from the UTF-16 code
            unit(s).  -1 is returned in the event of an error.

*******************************************************************************/


UniCodePoint  utf16get (

#    if PROTOTYPES
        TagUTF  tag,
        const  char  *src)
#    else
        tag, src)

        TagUTF  tag ;
        const  char  *src ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint, surrogate ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16get) NULL string: ") ;
        return (-1) ;
    }

/* Single-unit code point (Unicode range U+000000 to U+00FFFF)? */

    if (IS_LITTLE_ENDIAN (tag)) {
        codePoint = (((UniCodePoint) src[1] & 0xFF) << 8) |
                     ((UniCodePoint) src[0] & 0xFF) ;
    } else {
        codePoint = (((UniCodePoint) src[0] & 0xFF) << 8) |
                     ((UniCodePoint) src[1] & 0xFF) ;
    }

    if ((codePoint < 0xD800) || (0xDFFF < codePoint))  return (codePoint) ;

/* Two-unit code point (Unicode range U+010000 to U+10FFFF)? */

    if (IS_LITTLE_ENDIAN (tag)) {
        surrogate = (((UniCodePoint) src[3] & 0xFF) << 8) |
                     ((UniCodePoint) src[2] & 0xFF) ;
    } else {
        surrogate = (((UniCodePoint) src[2] & 0xFF) << 8) |
                     ((UniCodePoint) src[3] & 0xFF) ;
    }

    if ((surrogate & 0xDC00) != 0xDC00) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16get) Invalid low surrogate: U+%04lX\n", (long) surrogate) ;
        return (-1) ;
    }

    codePoint = ((codePoint & 0x03FF) << 10) | (surrogate & 0x03FF) ;

    codePoint += 0x010000 ;	/* Extend from 20 to 21 bits. */

    return (codePoint) ;

}

/*!*****************************************************************************

Procedure:

    utf16put ()

    Put a Code Point in a UTF-16 String.


Purpose:

    Function utf16put() encodes and adds a Unicode code point to a UTF-16
    string.  The number of 16-bit code units or bytes generated is easily
    determined from the value of the code point:

        numUnits = (codePoint < 0x10000) ? 1 : 2 ;
        numBytes = (codePoint < 0x10000) ? UTF_16_UNIT_BYTES : UTF_16_PAIR_BYTES ;


    Invocation:

        status = utf16put (codePoint, tag, dst) ;

    where

        <codePoint>	- I
            is the Unicode code point to be encoded.
        <tag>	- I
            is a UTF tag indicating the byte order of the string: big-endian
            or little-endian.
        <dst>		- O
            is the address in a UTF-16 string at which the code point is to be
            stored.
        <status>	- O
            returns the status of encoding the code point, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  utf16put (

#    if PROTOTYPES
        UniCodePoint  codePoint,
        TagUTF  tag,
        char  *dst)
#    else
        codePoint, tag, dst)

        UniCodePoint  codePoint ;
        TagUTF  tag ;
        char  *dst ;
#    endif

{    /* Local variables. */
    UniCodePoint  surrogate ;



    if ((codePoint < 0) || (0x10FFFF < codePoint)) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16put) Invalid code point: U+%06lX\n", (long) codePoint) ;
        return (errno) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16put) NULL destination string: ") ;
        return (errno) ;
    }

/* Single-unit code point (Unicode range U+000000 to U+00FFFF)? */

    if (codePoint < 0x10000) {

        if (IS_LITTLE_ENDIAN (tag)) {
            dst[1] = codePoint >> 8 ;
            dst[0] = codePoint & 0xFF ;
        } else {
            dst[0] = codePoint >> 8 ;
            dst[1] = codePoint & 0xFF ;
        }

    }

/* Two-unit code point (Unicode range U+010000 to U+10FFFF)? */

    else {

        codePoint -= 0x010000 ;
        surrogate = (codePoint & 0x03FF) | 0xDC00 ;
        codePoint = (codePoint >> 10) | 0xD800 ;

        if (IS_LITTLE_ENDIAN (tag)) {
            dst[1] = codePoint >> 8 ;
            dst[0] = codePoint & 0xFF ;
            dst[3] = surrogate >> 8 ;
            dst[2] = surrogate & 0xFF ;
        } else {
            dst[0] = codePoint >> 8 ;
            dst[1] = codePoint & 0xFF ;
            dst[2] = surrogate >> 8 ;
            dst[3] = surrogate & 0xFF ;
        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    utf32get ()

    Get the Next Code Point from a UTF-32 String.


Purpose:

    Function utf32get() decodes and returns the next code point from a UTF-32
    string.  The number of 32-bit code units consumed is always 1.


    Invocation:

        codePoint = utf32get (tag, src) ;

    where

        <tag>	- I
            is a UTF tag indicating the byte order of the string: big-endian
            or little-endian.
        <src>		- I
            is a pointer to a code unit in a UTF-32 string.
        <codePoint>	- O
            returns the Unicode code point constructed from the UTF-32 code
            unit.  -1 is returned in the event of an error.

*******************************************************************************/


UniCodePoint  utf32get (

#    if PROTOTYPES
        TagUTF  tag,
        const  char  *src)
#    else
        tag, src)

        TagUTF  tag ;
        const  char  *src ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32get) NULL string: ") ;
        return (-1) ;
    }

    if (IS_LITTLE_ENDIAN (tag)) {
        codePoint = (((UniCodePoint) src[3] & 0xFF) << 24) |
                    (((UniCodePoint) src[2] & 0xFF) << 16) |
                    (((UniCodePoint) src[1] & 0xFF) << 8) |
                    ((UniCodePoint) src[0] & 0xFF) ;
    } else {
        codePoint = (((UniCodePoint) src[0] & 0xFF) << 24) |
                    (((UniCodePoint) src[1] & 0xFF) << 16) |
                    (((UniCodePoint) src[2] & 0xFF) << 8) |
                    ((UniCodePoint) src[3] & 0xFF) ;
    }

    return (codePoint) ;

}

/*!*****************************************************************************

Procedure:

    utf32put ()

    Put a Code Point in a UTF-32 String.


Purpose:

    Function utf32put() encodes and adds a Unicode code point to a UTF-32
    string.  The number of 32-bit code units generated is always 1.


    Invocation:

        status = utf32put (codePoint, tag, dst) ;

    where

        <codePoint>	- I
            is the Unicode code point to be encoded.
        <tag>	- I
            is a UTF tag indicating the byte order of the string: big-endian
            or little-endian.
        <dst>		- O
            is the address in a UTF-32 string at which the code point is to be
            stored.
        <status>	- O
            returns the status of encoding the code point, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  utf32put (

#    if PROTOTYPES
        UniCodePoint  codePoint,
        TagUTF  tag,
        char  *dst)
#    else
        codePoint, tag, dst)

        UniCodePoint  codePoint ;
        TagUTF  tag ;
        char  *dst ;
#    endif

{

    if ((codePoint < 0) || (0x10FFFF < codePoint)) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32put) Invalid code point: U+%06lX\n", (long) codePoint) ;
        return (errno) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32put) NULL destination string: ") ;
        return (errno) ;
    }

    if (IS_LITTLE_ENDIAN (tag)) {
        dst[3] = (codePoint >> 24) & 0xFF ;
        dst[2] = (codePoint >> 16) & 0xFF ;
        dst[1] = (codePoint >> 8) & 0xFF ;
        dst[0] = codePoint & 0xFF ;
    } else {
        dst[0] = (codePoint >> 24) & 0xFF ;
        dst[1] = (codePoint >> 16) & 0xFF ;
        dst[2] = (codePoint >> 8) & 0xFF ;
        dst[3] = codePoint & 0xFF ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    utf8to16 ()

    Convert a UTF-8 String to a UTF-16 String.


Purpose:

    Function utf8to16() converts a UTF-8 encoding of a Unicode string to
    the UTF-16 representation of the string.  A null terminator (0x0000)
    is appended to the converted string if enough space remains in the
    destination buffer.



    Invocation:

        length = utf8to16 (src, srclen, insertBOM, dstTag, dstlen, dst) ;

    where

        <src>		- I
            is the UTF-8 string to be converted.
        <srclen>	- I
            is the length in bytes of the UTF-8 string.  If this argument is
            less than zero, the string is assumed to be NUL-terminated.
        <insertBOM>	- I
            specifies whether (true) or not (false) a UTF-16 byte-order-marker
            is to be inserted at the beginning of the destination string.  The
            endianness of the BOM is taken from the destination tag (the next
            argument).
        <dstTag>	- I
            is a UTF tag indicating the byte order of the destination string:
            big-endian or little-endian.
        <dstlen>	- I
            is the maximum length in bytes of the UTF-16 string buffer.
        <dst>		- O
            is a buffer into which the UTF-16 string is written.
        <length>	- O
            returns the number of code units written to the UTF-16 buffer;
            -1 is returned in the event of an error.

*******************************************************************************/


ssize_t  utf8to16 (

#    if PROTOTYPES
        const  char  *src,
        ssize_t  srclen,
        bool  insertBOM,
        TagUTF  dstTag,
        size_t  dstlen,
        char  *dst)
#    else
        src, srclen, insertBOM, dstTag, dstlen, dst)

        const  char  *src ;
        ssize_t  srclen ;
        bool  insertBOM ;
        TagUTF  dstTag ;
        size_t  dstlen ;
        char  *dst ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint ;
    size_t  length, numUnits ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8to16) NULL source string: ") ;
        return (-1) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8to16) NULL destination string: ") ;
        return (-1) ;
    }

    if (srclen < 0)  srclen = strlen (src) ;

    length = 0 ;

/* If the caller specified that a byte-order marker appear at the beginning
   of the string, then insert it. */

    if (insertBOM) {
        if (dstlen < UTF_16_UNIT_BYTES) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf8to16) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf16put (UTF_ZWNBSP, dstTag, dst)) {
            LGE "(utf8to16) Error writing BOM to destination string (%p): ",
                dst) ;
            return (-1) ;
        }
        dst += UTF_16_UNIT_BYTES ;  dstlen -= UTF_16_UNIT_BYTES ;  length++ ;
    }

/* Fetch the first code point in the source string.  If it is a UTF-8
   byte-order-marker, skip it  and begin the conversion with the second
   code point in the source string.  If there is no BOM, the first code
   point will simply be fetched again by the utf8get() call during the
   first iteration of the while-loop below. */

    codePoint = utf8get (src, &numUnits) ;
    if (codePoint == UTF_ZWNBSP) {
        src += numUnits ;  srclen -= numUnits ;
    }

/* Encode the UTF-8 code points as UTF-16. */

    while (srclen > 0) {
        codePoint = utf8get (src, &numUnits) ;
        if (codePoint < 0) {
            LGE "(utf8to16) Error in source string (%p): ", src) ;
            return (-1) ;
        }
        src += numUnits ;  srclen -= numUnits ;
        if ((dstlen < UTF_16_PAIR_BYTES) &&
            ((codePoint > 0x0FFFF) || (dstlen < UTF_16_UNIT_BYTES))) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf8to16) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf16put (codePoint, dstTag, dst)) {
            LGE "(utf8to16) Error writing to destination string (%p): ", dst) ;
            return (-1) ;
        }
        numUnits = (codePoint < 0x10000) ? 1 : 2 ;
        dst += numUnits * UTF_16_UNIT_BYTES ;
        dstlen -= numUnits * UTF_16_UNIT_BYTES ;
        length += numUnits ;
    }

/* Add a null terminator (0x0000). */

    if (dstlen >= UTF_16_UNIT_BYTES) {
        *dst++ = 0 ;  *dst++ = 0 ;
    }

    return (length) ;

}

/*!*****************************************************************************

Procedure:

    utf8to32 ()

    Convert a UTF-8 String to a UTF-32 String.


Purpose:

    Function utf8to32() converts a UTF-8 encoding of a Unicode string to
    the UTF-32 representation of the string.  A null terminator (0x00000000)
    is appended to the converted string if enough space remains in the
    destination buffer.


    Invocation:

        length = utf8to32 (src, srclen, insertBOM, dstTag, dstlen, dst) ;

    where

        <src>		- I
            is the UTF-8 string to be converted.
        <srclen>	- I
            is the length in bytes of the UTF-8 string.  If this argument is
            less than zero, the string is assumed to be NUL-terminated.
        <insertBOM>	- I
            specifies whether (true) or not (false) a UTF-32 byte-order-marker
            is to be inserted at the beginning of the destination string.  The
            endianness of the BOM is taken from the destination tag (the next
            argument).
        <dstTag>	- I
            is a UTF tag indicating the byte order of the destination string:
            big-endian or little-endian.
        <dstlen>	- I
            is the maximum length in bytes of the UTF-32 string buffer.
        <dst>		- O
            is a buffer into which the UTF-32 string is written.
        <length>	- O
            returns the number of code units written to the UTF-32 buffer;
            -1 is returned in the event of an error.

*******************************************************************************/


ssize_t  utf8to32 (

#    if PROTOTYPES
        const  char  *src,
        ssize_t  srclen,
        bool  insertBOM,
        TagUTF  dstTag,
        size_t  dstlen,
        char  *dst)
#    else
        src, srclen, insertBOM, dstTag, dstlen, dst)

        const  char  *src ;
        ssize_t  srclen ;
        bool  insertBOM ;
        TagUTF  dstTag ;
        size_t  dstlen ;
        char  *dst ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint ;
    size_t  length, numUnits ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8to32) NULL source string: ") ;
        return (-1) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf8to32) NULL destination string: ") ;
        return (-1) ;
    }

    if (srclen < 0)  srclen = strlen (src) ;

    length = 0 ;

/* If the caller specified that a byte-order marker appear at the beginning
   of the string, then insert it. */

    if (insertBOM) {
        if (dstlen < UTF_32_UNIT_BYTES) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf8to32) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf32put (UTF_ZWNBSP, dstTag, dst)) {
            LGE "(utf8to32) Error writing BOM to destination string (%p): ",
                dst) ;
            return (-1) ;
        }
        dst += UTF_32_UNIT_BYTES ;  dstlen -= UTF_32_UNIT_BYTES ;  length++ ;
    }

/* Fetch the first code point in the source string.  If it is a UTF-8
   byte-order-marker, skip it  and begin the conversion with the second
   code point in the source string.  If there is no BOM, the first code
   point will simply be fetched again by the utf8get() call during the
   first iteration of the while-loop below. */

    codePoint = utf8get (src, &numUnits) ;
    if (codePoint == UTF_ZWNBSP) {
        src += numUnits ;  srclen -= numUnits ;
    }

/* Encode the UTF-8 code points as UTF-32. */

    while (srclen > 0) {
        codePoint = utf8get (src, &numUnits) ;
        if (codePoint < 0) {
            LGE "(utf8to32) Error in source string (%p): ", src) ;
            return (-1) ;
        }
        src += numUnits ;  srclen -= numUnits ;
        if (dstlen < UTF_32_UNIT_BYTES) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf8to32) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf32put (codePoint, dstTag, dst)) {
            LGE "(utf8to32) Error writing to destination string (%p): ", dst) ;
            return (-1) ;
        }
        numUnits = 1 ;
        dst += numUnits * UTF_32_UNIT_BYTES ;
        dstlen -= numUnits * UTF_32_UNIT_BYTES ;
        length += numUnits ;
    }

/* Add a null terminator (0x00000000). */

    if (dstlen >= UTF_32_UNIT_BYTES) {
        *dst++ = 0 ;  *dst++ = 0 ;
        *dst++ = 0 ;  *dst++ = 0 ;
    }

    return (length) ;

}

/*!*****************************************************************************

Procedure:

    utf16to8 ()

    Convert a UTF-16 String to a UTF-8 String.


Purpose:

    Function utf16to8() converts a UTF-16 encoding of a Unicode string to
    the UTF-8 representation of the string.

    The UTF-16 code units are assumed to be big-endian unless a byte-order
    marker (BOM) is present in the source string.  The source BOM is not
    copied to the UTF-8 string, although a UTF-8 BOM can be inserted in
    the destination string by specifying a destination BOM of UTF_8_BOM.
    (A UTF-8 string is not little-endian or big-endian, but some programs
    look for a BOM at the beginning of a file to detect that the contents
    of the file are encoded in UTF-8.)


    Invocation:

        length = utf16to8 (src, srclen, srcTag, insertBOM, dstlen, dst) ;

    where

        <src>		- I
            is the UTF-16 string to be converted.  The byte-order marker (BOM),
            if present, is not copied.  The BOM must be present if the UTF-16
            code units are little-endian; otherwise, big-endian is assumed.
        <srclen>	- I
            is the length in bytes of the UTF-16 string.  The conversion is
            performed until that many bytes have been consumed or until a
            NUL terminator (i.e., 0x0000) is encountered.  If this argument
            is less than zero (any arbitrary negative number), the string
            is assumed to be NUL-terminated and utf16len() is called to
            determine the source length.
        <srcTag>	- I
            is a UTF tag indicating the byte order of the source string:
            big-endian or little-endian.
        <insertBOM>	- I
            specifies whether (true) or not (false) a UTF-8 byte-order-marker
            is to be inserted at the beginning of the destination string.
        <dstlen>	- I
            is the maximum length in bytes of the UTF-8 string buffer.
        <dst>		- O
            is a buffer into which the UTF-8 string is written.
        <length>	- O
            returns the number of code units/bytes written to the UTF-8 buffer;
            -1 is returned in the event of an error.

*******************************************************************************/


ssize_t  utf16to8 (

#    if PROTOTYPES
        const  char  *src,
        ssize_t  srclen,
        TagUTF  srcTag,
        bool  insertBOM,
        size_t  dstlen,
        char  *dst)
#    else
        src, srclen, srcTag, insertBOM, dstlen, dst)

        const  char  *src ;
        ssize_t  srclen ;
        TagUTF  srcTag ;
        bool  insertBOM ;
        size_t  dstlen ;
        char  *dst ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint ;
    size_t  length, numUnits ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16to8) NULL source string: ") ;
        return (-1) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf16to8) NULL destination string: ") ;
        return (-1) ;
    }

    if (srclen < 0)  srclen = utf16len (src, NULL) * UTF_16_UNIT_BYTES ;

    length = 0 ;

/* If the caller specified that a byte-order marker appear at the beginning
   of the string, then insert it. */

    if (insertBOM) {
        if (dstlen < UTF_8_BOM_BYTES) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf16to8) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf8put (UTF_ZWNBSP, dst, &numUnits)) {
            LGE "(utf16to8) Error writing BOM to destination string (%p): ",
                dst) ;
            return (-1) ;
        }
        dst += numUnits ;  dstlen -= numUnits ;  length++ ;
    }

/* Fetch the first code point in the source string.  If it is a UTF-16
   byte-order-marker, skip it  and begin the conversion with the second
   code point in the source string.  If there is no BOM, the first code
   point will simply be fetched again by the utf16get() call during the
   first iteration of the while-loop below. */

    codePoint = utf16get (srcTag, src) ;
    if (codePoint == UTF_ZWNBSP) {
        src += UTF_16_UNIT_BYTES ;  srclen -= UTF_16_UNIT_BYTES ;
    }

/* Encode the UTF-16 code points as UTF-8. */

    while (srclen > 0) {
        codePoint = utf16get (srcTag, src) ;
        if (codePoint == 0)  break ;		/* NUL terminator? */
        if (codePoint < 0) {
            SET_ERRNO (EINVAL) ;
            LGE "(utf16to8) Error in source string (%p): ", src) ;
            return (-1) ;
        }
        if (codePoint < 0x10000) {
            src += UTF_16_UNIT_BYTES ;  srclen -= UTF_16_UNIT_BYTES ;
        } else {
            src += UTF_16_PAIR_BYTES ;  srclen -= UTF_16_PAIR_BYTES ;
        }
        if ((dstlen < 4) && ((codePoint > 0x0FFFF) ||
              ((dstlen < 3) && ((codePoint > 0x00800) ||
                ((dstlen < 2) && ((codePoint > 0x0007F) ||
                  (dstlen < 1))))))) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf16to8) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf8put (codePoint, dst, &numUnits)) {
            LGE "(utf16to8) Error writing to destination string (%p): ", dst) ;
            return (-1) ;
        }
        dst += numUnits ;  dstlen -= numUnits ;
        length += numUnits ;
    }

    if (dstlen >= 1)  *dst = '\0' ;

    return (length) ;

}

/*!*****************************************************************************

Procedure:

    utf32to8 ()

    Convert a UTF-32 String to a UTF-8 String.


Purpose:

    Function utf32to8() converts a UTF-32 encoding of a Unicode string to
    the UTF-8 representation of the string.

    The UTF-32 code units are assumed to be little-endian unless a byte-order
    marker (BOM) is present.  The BOM is not copied to the UTF-8 string,
    although a BOM can be inserted by specifying a "bom" argument of one.
    (A UTF-8 string is not little-endian or big-endian, but some programs
    look for a BOM at the beginning of a file to detect that the contents
    of the file are encoded in UTF-8.)


    Invocation:

        length = utf32to8 (src, srclen, srcTag, insertBOM, dstlen, dst) ;

    where

        <src>		- I
            is the UTF-32 string to be converted.  The byte-order marker (BOM),
            if present, is not copied.  The BOM must be present if the UTF-32
            code units are little-endian; otherwise, big-endian is assumed.
        <srclen>	- I
            is the length in bytes of the UTF-32 string.  The conversion
            is performed until that many bytes have been consumed or until
            a NUL terminator (i.e., 0x00000000) is encountered.  If this
            argument is less than zero (any arbitrary negative number),
            the string is assumed to be NUL-terminated and utf32len() is
            called to determine the source length.
        <srcTag>	- I
            is a UTF tag indicating the byte order of the source string:
            big-endian or little-endian.
        <insertBOM>	- I
            specifies whether (true) or not (false) a UTF-8 byte-order-marker
            is to be inserted at the beginning of the destination string.
        <dstlen>	- I
            is the maximum length in bytes of the UTF-8 string buffer.
        <dst>		- O
            is a buffer into which the UTF-8 string is written.
        <length>	- O
            returns the number of code units/bytes written to the UTF-8 buffer;
            -1 is returned in the event of an error.

*******************************************************************************/


ssize_t  utf32to8 (

#    if PROTOTYPES
        const  char  *src,
        ssize_t  srclen,
        TagUTF  srcTag,
        bool  insertBOM,
        size_t  dstlen,
        char  *dst)
#    else
        src, srclen, srcTag, insertBOM, dstlen, dst)

        const  char  *src ;
        ssize_t  srclen ;
        TagUTF  srcTag ;
        bool  insertBOM ;
        size_t  dstlen ;
        char  *dst ;
#    endif

{    /* Local variables. */
    UniCodePoint  codePoint ;
    size_t  length, numUnits ;



    if (src == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32to8) NULL source string: ") ;
        return (-1) ;
    }

    if (dst == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(utf32to8) NULL destination string: ") ;
        return (-1) ;
    }

    if (srclen < 0)  srclen = utf32len (src, NULL) * UTF_32_UNIT_BYTES ;

    length = 0 ;

/* If the caller specified that a byte-order marker appear at the beginning
   of the string, then insert it. */

    if (insertBOM) {
        if (dstlen < UTF_8_BOM_BYTES) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf32to8) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf8put (UTF_ZWNBSP, dst, &numUnits)) {
            LGE "(utf32to8) Error writing BOM to destination string (%p): ",
                dst) ;
            return (-1) ;
        }
        dst += numUnits ;  dstlen -= numUnits ;  length++ ;
    }

/* Fetch the first code point in the source string.  If it is a UTF-32
   byte-order-marker, skip it  and begin the conversion with the second
   code point in the source string.  If there is no BOM, the first code
   point will simply be fetched again by the utf16get() call during the
   first iteration of the while-loop below. */

    codePoint = utf32get (srcTag, src) ;
    if (codePoint == UTF_ZWNBSP) {
        src += UTF_32_UNIT_BYTES ;  srclen -= UTF_32_UNIT_BYTES ;
    }

/* Encode the UTF-32 code points as UTF-8. */

    while (srclen > 0) {
        codePoint = utf32get (srcTag, src) ;
        if (codePoint == 0)  break ;		/* NUL terminator? */
        if (codePoint < 0) {
            LGE "(utf32to8) Error in source string (%p): ", src) ;
            return (-1) ;
        }
        src += UTF_32_UNIT_BYTES ;  srclen -= UTF_32_UNIT_BYTES ;
        if ((dstlen < 4) && ((codePoint > 0x0FFFF) ||
              ((dstlen < 3) && ((codePoint > 0x00800) ||
                ((dstlen < 2) && ((codePoint > 0x0007F) ||
                  (dstlen < 1))))))) {
            SET_ERRNO (ENOMEM) ;
            LGE "(utf32to8) Destination string is too small: ") ;
            return (-1) ;
        }
        if (utf8put (codePoint, dst, &numUnits)) {
            LGE "(utf32to8) Error writing to destination string (%p): ", dst) ;
            return (-1) ;
        }
        dst += numUnits ;  dstlen -= numUnits ;  length += numUnits ;
    }

    if (dstlen >= 1)  *dst = '\0' ;

    return (length) ;

}

#ifdef  TEST

/*******************************************************************************

    Program to test the UTF_UTIL functions.

    Under UNIX:
        Compile and link as follows:
            % cc -DTEST utf_util.c <libraries> -o utf_test
        Run the program from the command line:
            % utf_test

*******************************************************************************/

int  main (argc, argv)
    int  argc ;
    char  *argv[] ;

{    /* Local variables. */
    char  buffer[32] ;
    char  *s, *t ;
    int  i ;
    UniCodePoint  codePoint ;
    size_t  length, numUnits ;



    aperror_print = 1 ;

    printf ("RFC 3629 - UTF-8 Transformation Format of ISO 10646\n") ;
    printf ("\n") ;
    printf ("The character sequence U+0041 U+2262 U+0391 U+002E\n") ;
    printf ("\"A<NOT IDENTICALTO><ALPHA>.\" is encoded in UTF-8 as follows:\n") ;
    printf ("    41 E2 89 A2 CE 91 2E\n") ;
    printf ("The character sequence U+D55C U+AD6D U+C5B4 (Korean \"hangugeo\",\n") ;
    printf ("meaning \"the Korean language\") is encoded in UTF-8 as follows:\n") ;
    printf ("    ED 95 9C EA B5 AD EC 96 B4\n") ;
    printf ("The character sequence U+65E5 U+672C U+8A9E (Japanese \"nihongo\",\n") ;
    printf ("meaning \"the Japanese language\") is encoded in UTF-8 as follows:\n") ;
    printf ("    E6 97 A5 E6 9C AC E8 AA 9E\n") ;
    printf ("The character U+233B4 (a Chinese character meaning 'stump of tree'),\n") ;
    printf ("prepended with a UTF-8 BOM, is encoded in UTF-8 as follows:\n") ;
    printf ("    EF BB BF F0 A3 8E B4\n") ;
    printf ("\n") ;


  { unsigned  char  example1[] = { 0x41, 0xE2, 0x89, 0xA2, 0xCE, 0x91, 0x2E } ;
    unsigned  char  example2[] = { 0xED, 0x95, 0x9C, 0xEA, 0xB5, 0xAD, 0xEC, 0x96, 0xB4 } ;
    unsigned  char  example3[] = { 0xE6, 0x97, 0xA5, 0xE6, 0x9C, 0xAC, 0xE8, 0xAA, 0x9E } ;
    unsigned  char  example4[] = { 0xEF, 0xBB, 0xBF, 0xF0, 0xA3, 0x8E, 0xB4 } ;

    printf ("Example 1 U+0041 U+2262 U+0391 U+002E:\n") ;
    s = (char *) example1 ;
    for (i = 0, length = 0, t = buffer ;  i < 4 ;  i++) {
        codePoint = utf8get (s, &numUnits) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += numUnits ;
        utf8put (codePoint, t, &numUnits) ;
        t += numUnits ;  length += numUnits ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("Example 2 U+D55C U+AD6D U+C5B4:\n") ;
    s = (char *) example2 ;
    for (i = 0, length = 0, t = buffer ;  i < 3 ;  i++) {
        codePoint = utf8get (s, &numUnits) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += numUnits ;
        utf8put (codePoint, t, &numUnits) ;
        t += numUnits ;  length += numUnits ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("Example 3 U+65E5 U+672C U+8A9E:\n") ;
    s = (char *) example3 ;
    for (i = 0, length = 0, t = buffer ;  i < 3 ;  i++) {
        codePoint = utf8get (s, &numUnits) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += numUnits ;
        utf8put (codePoint, t, &numUnits) ;
        t += numUnits ;  length += numUnits ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("Example 4 U+233B4 with BOM:\n") ;
    s = (char *) example4 ;
    for (i = 0, length = 0, t = buffer ;  i < 2 ;  i++) {
        codePoint = utf8get (s, &numUnits) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += numUnits ;
        utf8put (codePoint, t, &numUnits) ;
        t += numUnits ;  length += numUnits ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }
  }

    printf ("\n====================\n\n") ;

    printf ("RFC 2781 - UTF-16 encoding of ISO 10646\n") ;
    printf ("\n") ;
    printf ("For the sake of example, let's suppose that there is a hieroglyphic\n") ;
    printf ("character representing the Egyptian god Ra with character value\n") ;
    printf ("0x12345 (this character does not exist at present in Unicode).\n") ;
    printf ("The examples here all evaluate to the ... Ra hieroglyph (0x12345).\n") ;
    printf ("    Text labelled with UTF-16BE, without a BOM:\n") ;
    printf ("        D8 08 DF 45 00 3D 00 52 00 61\n") ;
    printf ("    Text labelled with UTF-16LE, without a BOM:\n") ;
    printf ("        08 D8 45 DF 3D 00 52 00 61 00\n") ;
    printf ("    Big-endian text labelled with UTF-16, with a BOM:\n") ;
    printf ("        FE FF D8 08 DF 45 00 3D 00 52 00 61\n") ;
    printf ("    Little-endian text labelled with UTF-16, with a BOM:\n") ;
    printf ("        FF FE 08 D8 45 DF 3D 00 52 00 61 00\n") ;
    printf ("\n") ;

  { unsigned  char  example1[] = { 0xD8, 0x08, 0xDF, 0x45, 0x00, 0x3D, 0x00, 0x52, 0x00, 0x61 } ;
    unsigned  char  example2[] = { 0x08, 0xD8, 0x45, 0xDF, 0x3D, 0x00, 0x52, 0x00, 0x61, 0x00 } ;
    unsigned  char  example3[] = { 0xFE, 0xFF, 0xD8, 0x08, 0xDF, 0x45, 0x00, 0x3D, 0x00, 0x52, 0x00, 0x61 } ;
    unsigned  char  example4[] = { 0xFF, 0xFE, 0x08, 0xD8, 0x45, 0xDF, 0x3D, 0x00, 0x52, 0x00, 0x61, 0x00 } ;

    printf ("Example 1 UTF-16BE without BOM:\n") ;
    s = (char *) example1 ;
    for (i = 0, length = 0, t = buffer ;  i < 4 ;  i++) {
        codePoint = utf16get (UTF_16_NULL_TAG, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += (codePoint < 0x10000) ? 2 : 4 ;
        utf16put (codePoint, UTF_16_NULL_TAG, t) ;
        t += (codePoint < 0x10000) ? 2 : 4 ;
        length += (codePoint < 0x10000) ? 2 : 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("Example 2 UTF-16LE without BOM:\n") ;
    s = (char *) example2 ;
    for (i = 0, length = 0, t = buffer ;  i < 4 ;  i++) {
        codePoint = utf16get (UTF_16_NULL_TAG_LE, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += (codePoint < 0x10000) ? 2 : 4 ;
        utf16put (codePoint, UTF_16_NULL_TAG_LE, t) ;
        t += (codePoint < 0x10000) ? 2 : 4 ;
        length += (codePoint < 0x10000) ? 2 : 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("Example 3 Big-endian UTF-16 with BOM:\n") ;
    s = (char *) example3 ;
    for (i = 0, length = 0, t = buffer ;  i < 5 ;  i++) {
        codePoint = utf16get (UTF_16_TAG, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += (codePoint < 0x10000) ? 2 : 4 ;
        utf16put (codePoint, UTF_16_TAG, t) ;
        t += (codePoint < 0x10000) ? 2 : 4 ;
        length += (codePoint < 0x10000) ? 2 : 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("Example 4 Little-endian UTF-16 with BOM:\n") ;
    s = (char *) example4 ;
    for (i = 0, length = 0, t = buffer ;  i < 5 ;  i++) {
        codePoint = utf16get (UTF_16_TAG_LE, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += (codePoint < 0x10000) ? 2 : 4 ;
        utf16put (codePoint, UTF_16_TAG_LE, t) ;
        t += (codePoint < 0x10000) ? 2 : 4 ;
        length += (codePoint < 0x10000) ? 2 : 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }
  }

    printf ("\n====================\n\n") ;

  { unsigned  char  example1[] = { 0x00, 0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0x3D } ;
    unsigned  char  example2[] = { 0x45, 0x23, 0x01, 0x00, 0x3D, 0x00, 0x00, 0x00 } ;
    unsigned  char  example3[] = { 0x00, 0x00, 0xFE, 0xFF, 0x00, 0x01, 0x23, 0x45, 0x00, 0x00, 0x00, 0x3D } ;
    unsigned  char  example4[] = { 0xFF, 0xFE, 0x00, 0x00, 0x45, 0x23, 0x01, 0x00, 0x3D, 0x00, 0x00, 0x00 } ;

    printf ("UTF-32 Example 1:\n") ;
    s = (char *) example1 ;
    for (i = 0, length = 0, t = buffer ;  i < 2 ;  i++) {
        codePoint = utf32get (UTF_32_NULL_TAG, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += 4 ;
        utf32put (codePoint, UTF_32_NULL_TAG, t) ;
        t += 4 ;
        length += 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("UTF-32 Example 2:\n") ;
    s = (char *) example2 ;
    for (i = 0, length = 0, t = buffer ;  i < 2 ;  i++) {
        codePoint = utf32get (UTF_32_NULL_TAG_LE, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += 4 ;
        utf32put (codePoint, UTF_32_NULL_TAG_LE, t) ;
        t += 4 ;
        length += 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("UTF-32 Example 3:\n") ;
    s = (char *) example3 ;
    for (i = 0, length = 0, t = buffer ;  i < 3 ;  i++) {
        codePoint = utf32get (UTF_32_TAG, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += 4 ;
        utf32put (codePoint, UTF_32_TAG, t) ;
        t += 4 ;
        length += 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }

    printf ("UTF-32 Example 4:\n") ;
    s = (char *) example4 ;
    for (i = 0, length = 0, t = buffer ;  i < 3 ;  i++) {
        codePoint = utf32get (UTF_32_TAG_LE, s) ;
        printf ("    U+%04lX\n", (long) codePoint) ;
        s += 4 ;
        utf32put (codePoint, UTF_32_TAG_LE, t) ;
        t += 4 ;
        length += 4 ;
    }
    for (i = 0 ;  i < length ;  i++) {
        printf ("    0x%02X\n", (unsigned char) buffer[i]) ;
    }
  }

    printf ("\n====================\n\n") ;

  { char  ping[256], pong[256] ;
    const  char  *example = "The quick brown fox jumped over the lazy dog!" ;

    length = utf8to16 (example, -1, true, UTF_16_TAG_LE, sizeof ping, ping) ;
    printf ("\nUTF-16 (BOM,LE):\n") ;
    meoDumpX (stdout, "    ", 0, ping, length * UTF_16_UNIT_BYTES) ;

    length = utf16to8 (ping, length * UTF_16_UNIT_BYTES, UTF_16_TAG_LE, true,
                        sizeof pong, pong) ;
    printf ("\nUTF-8:\n") ;
    meoDumpX (stdout, "    ", 0, pong, length) ;

    length = utf8to32 (example, -1, true, UTF_32_TAG, sizeof ping, ping) ;
    printf ("\nUTF-32 (BOM,BE):\n") ;
    meoDumpX (stdout, "    ", 0, ping, length * UTF_32_UNIT_BYTES) ;

    length = utf32to8 (ping, length * UTF_32_UNIT_BYTES, UTF_32_TAG, false,
                        sizeof pong, pong) ;
    printf ("\nUTF-8:\n") ;
    meoDumpX (stdout, "    ", 0, pong, length) ;

  }

    exit (0) ;

}

#endif  /* TEST */
