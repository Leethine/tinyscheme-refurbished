/* $Id: str_util.c,v 1.39 2025/07/12 20:40:15 alex Exp alex $ */
/*******************************************************************************

File:

    str_util.c


Author:    Alex Measday


Purpose:

    These are a collection of string manipulation functions, including
    some Standard C Library functions for platforms that don't have them.


Notes:

    These functions used to be part of the LIBALEX functions.  The following
    changes have been made:

      - Functions that take a length argument used to follow a convention
        that, if the length were zero, the function would determine the
        length by scanning the string for a NUL terminator.  This turned
        out to be a real nuisance if you had a need to handle zero-length
        strings ("").  The new convention is that, if the length argument
        is less than zero, the function will determine the length itself.


Procedures:

    memdup2() - duplicates a memory block (2+ arguments; returns duplicate
        in argument).
    memncpym() - copies a length-limited memory block to a sized buffer.
    strdup2() - duplicates a string (2 arguments; returns duplicate in
        argument).
    strncatm() - concatenates length-limited strings in a sized buffer.
    strncpym() - copies a length-limited string to a sized buffer.
    strndup2() - duplicates a length-limited string (2+ arguments;
        returns duplicate in argument).
    strndupx() - duplicates a length-limited string with extra room.

    strConvert() - scans a text string, converting "\<num>" sequences to
        the appropriate binary bytes.
    strDesmart() - replaces smart characters in a string.
    strDestring() - resolves quote-delimited elements in a string.
    strDetab() - converts tabs in a string to blanks.
    strDupCat() - duplicates the concatenation of multiple strings.
    strEnv() - translates environment variable references in a string.
    strEtoA() - converts a string of EBCDIC characters to ASCII.
    strInsert() - inserts a substring in a string.
    strMatch() - a string compare function that handles abbreviations.
    strRemove() - removes a substring from a string.
    strReplace() - replaces a substring in a string.
    strTrim() - trims trailing tabs and spaces from a string.
    strTrimNL() - trims trailing line endings from a string.

    memdup() - duplicates a memory block.
    memxlt() - translates a memory block.
    stpcpy() - copies a string, points to the end.
    strcasecmp() - compares two strings, ignoring case.
    strcasestr() - finds a substring in a string, ignoring case.
    strcspn() - skips characters not in a set of characters.
    strdup() - duplicates a NUL-terminated string.
    strlcat() - concatenates two strings, limited by length.
    strlcpy() - copies a string, limited by length.
    strlwr() - converts a string to all lower-case.
    strncasecmp() - compares two strings for a specified length, ignoring case.
    strncasestr() - finds a substring in a length-limited string, ignoring case.
    strnchr() - finds the first occurrence of a character in a length-limited
        string.
    strndup() - duplicates a string of a specified length.
    strnlen() - determines the length of a fixed-size string.
    strnlwr() - converts a length-limited string to all lower-case.
    strnstr() - finds a substring in a length-limited string.
    strnupr() - converts a length-limited string to all upper-case.
    strrchr() - finds the last occurrence of a character in a string.
    strspn() - skips characters in a set of characters.
    strtok() - extracts tokens from a string.
    strtok_r() - extracts tokens from a string (reentrant).
    strupr() - converts a string to all upper-case.
    strxlt() - translates a string.
    xltable() - builds a translation table.
    xltable1() - builds a translation table (single from/to string).

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#if HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* Standard C string functions. */
#if defined(HAVE_MEMCPY) && !HAVE_MEMCPY
#    define  memmove(dest,src,length)  bcopy(src,dest,length)
#endif
#include  "get_util.h"			/* "Get Next" functions. */
#include  "str_util.h"			/* String manipulation functions. */

int  str_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  str_util_debug

/*!*****************************************************************************

Procedure:

    memdup2 ()

    Duplicate a Memory Block; Return Duplicate in Argument.


Purpose:

    Function memdup2() duplicates a block of memory.  memdup2() is similar
    to memdup(3) except that the duplicate block is returned as an argument
    and the function value is an error indicator.  This somewhat simplifies
    error checking in the calling program.


    Invocation:

        status = memdup2 (block, length, &duplicate) ;

    where:

        <block>		- I
            is the memory block to be duplicated.
        <length>	- I
            is the length in bytes of the existing block.
        <duplicate>	- O
            returns a MALLOC(3)ed copy of the memory block.  The caller is
            responsible for FREE(3)ing the duplicate block when it is no
            longer needed.
        <status>	- O
            returns the status of duplicating the memory block, zero if
            there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  memdup2 (

#    if PROTOTYPES
        const  void  *block,
        size_t  length,
        void  **duplicate)
#    else
        block, length, duplicate)

        void  *block ;
        size_t  length ;
        void  **duplicate ;
#    endif

{

    if (duplicate != NULL)  *duplicate = NULL ;

    if ((block == NULL) || (duplicate == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(memdup2) NULL block or duplicate argument: ") ;
        return (errno) ;
    }

    *duplicate = memdup (block, length) ;
    if (*duplicate == NULL) {
        LGE "(memdup2) Error duplicating %"PRIuSIZE_T"-byte memory block.\nmemdup: ",
            length) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    memncpym ()

    Copy a Length-Limited Memory Block to a Size-Limited Buffer.


Purpose:

    Function memncpym() copies a length-limited memory block to a buffer
    with a specified maximum size.  This function is similar to memcpy(3)
    except that

      * the destination buffer has a maximum size, and

      * the function has the semantics of memmove(3) with regard
        to correctly copying overlapping blocks.

    This function is not part of the ANSI C library.  Memmove(3) works
    just as well, but memncpym() saves the caller from having to figure
    out the lesser of the source length and the destination size.


    Invocation:

        copy = memncpym (destination, source, length, maximum) ;

    where:

        <destination>	- O
            is the address of the buffer into which the memory block
            will be copied.
        <source>	- I
            is the memory block to be copied.
        <length>	- I
            is the number of bytes to copy from the source to the destination.
        <maximum>	- I
            is the maximum number of bytes that the destination can hold.
        <copy>		- O
            returns a pointer to the destination buffer.

*******************************************************************************/


void  *memncpym (

#    if PROTOTYPES
        void  *destination,
        const  void  *source,
        size_t  length,
        size_t  maximum)
#    else
        destination, source, length, maximum)

        void  *destination ;
        void  *source ;
        size_t  length ;
        size_t  maximum ;
#    endif

{

    if ((source == NULL) || (destination == NULL))  return (NULL) ;
    if (maximum < 1)  return (NULL) ;

    if (length > maximum)  length = maximum ;	/* Clip to destination size. */

    return (memmove (destination, source, length)) ;

}

/*!*****************************************************************************

Procedure:

    strdup2 ()

    Duplicate a NUL-Terminated String; Return Duplicate in Argument.


Purpose:

    Function strdup2() duplicates a NUL-terminated string.  strdup2() is
    similar to strdup(3) except that the duplicate string is returned as
    an argument and the function value is an error indicator.  This somewhat
    simplifies error checking in the calling program.


    Invocation:

        status = strdup2 (string, &duplicate) ;

    where:

        <string>	- I
            is the string to be duplicated.
        <duplicate>	- O
            returns a MALLOC(3)ed copy of the input string.  The caller
            is responsible for FREE(3)ing the duplicate string.  NULL is
            returned in the event of an error.
        <status>	- O
            returns the status of duplicating the string, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  strdup2 (

#    if PROTOTYPES
        const  char  *string,
        char  **duplicate)
#    else
        string, duplicate)

        char  *string ;
        char  **duplicate ;
#    endif

{

    if (duplicate != NULL)  *duplicate = NULL ;

    if ((string == NULL) || (duplicate == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strdup2) NULL string or duplicate argument: ") ;
        return (errno) ;
    }

    *duplicate = strdup (string) ;
    if (*duplicate == NULL) {
        LGE "(strdup2) Error duplicating %"PRIuSIZE_T"-byte string.\n\"%s\"\nstrdup: ",
            strlen (string), string) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    strncatm ()

    Concatenate Length-Limited Strings in a Size-Limited Buffer.


Purpose:

    Function strncatm() appends a length-limited string to an existing
    string in a buffer with a specified maximum size.  This function is
    the catenation counterpart to strncpym(); see the next function below.

    The function is similar to ANSI C's strncat(3) in that the finished
    concatenated string in the buffer is *always* NUL-terminated.  This
    function differs from strncat(3) in that the concatenation will NOT
    overflow the output buffer.  Depending on the lengths of the strings
    being concatenated and the size of the buffer, the result may be a
    truncated version of the full concatenation.


    Invocation:

        concat = strncatm (destination, source, length, maximum) ;

    where:

        <destination>	- I/O
            is the address of the buffer containing the existing string
            and in which the source string will be appended.
        <source>	- I
            is the possibly NUL-terminated string to be appended to the
            existing string.
        <length>	- I
            is the maximum number of characters to copy from the source string
            without a NUL character being encountered.
        <maximum>	- I
            is the maximum number of characters, including the NUL terminator,
            that the destination buffer can hold.
        <concat>	- O
            returns a pointer to the concatenated string; i.e., the destination
            buffer.

*******************************************************************************/


char  *strncatm (

#    if PROTOTYPES
        char  *destination,
        const  char  *source,
        size_t  length,
        size_t  maximum)
#    else
        destination, source, length, maximum)

        char  *destination ;
        char  *source ;
        size_t  length ;
        size_t  maximum ;
#    endif

{    /* Local variables. */
    size_t  existingLength, remainingLength ;



    if (destination == NULL)  return (NULL) ;
    if (maximum < 1)  return (NULL) ;
    if (source == NULL)  source = "" ;

/*******************************************************************************
     existL   length   max ->|
   !--------!--------!.......|

     existL            max ->|
   !--------!................|
                               length |
            !-------------------------|
*******************************************************************************/

/* NUL-terminate the existing string if buffer is full. */

    existingLength = strnlen (destination, maximum) ;
    if (existingLength == maximum)  destination[--existingLength] = '\0' ;

/* Compute the remaining free space (less the NUL terminator). */

    remainingLength = maximum - existingLength - 1 ;

/* Append the source string, truncating if necessary to prevent overflow. */

    if (length > remainingLength)  length = remainingLength ;

    strncpy (&destination[existingLength], source, length) ;

    destination[existingLength+length] = '\0' ;

    return (destination) ;

}

/*!*****************************************************************************

Procedure:

    strncpym ()

    Copy a Length-Limited String to a Size-Limited Buffer.


Purpose:

    Function strncpym() copies a length-limited string (which may or
    may not be NUL-terminated) to a buffer with a specified maximum size.
    In a hypothetical language with 1-based indexing of characters in a
    string:

                   COPY source[1..N] TO destination[1:M]

    This is similar to strncpy(3), but (i) the destination buffer has a
    maximum size, (ii) the destination string is *always* NUL-terminated,
    and (ii) the copy will not overflow the buffer.

    This function is not part of the ANSI C library.  I frequently find myself
    having to copy a substring to a buffer of a given size.  I used to have a
    strCopy() function, but I got rid of it when I added strlcpy(), a
    semi-standardardized function that I thought would solve all my problems -
    big mistake!  The latter function requires that the source string be
    NUL-terminated, so I was forced back to using strncpy() and explicitly
    NUL-terminating the destination string.


    Invocation:

        copy = strncpym (destination, source, length, maximum) ;

    where:

        <destination>	- O
            is the address of the buffer into which the string will be copied.
        <source>	- I
            is the possibly NUL-terminated string to be copied.
        <length>	- I
            is the maximum number of characters to copy from the source string
            without a NUL character being encountered.
        <maximum>	- I
            is the maximum number of characters, including the NUL terminator,
            that the destination buffer can hold.
        <copy>		- O
            returns a pointer to the destination string.  The destination string
            is always NUL-terminated.  Depending on the input string length and
            the destination buffer size, the resulting string may be truncated.

*******************************************************************************/


char  *strncpym (

#    if PROTOTYPES
        char  *destination,
        const  char  *source,
        size_t  length,
        size_t  maximum)
#    else
        destination, source, length, maximum)

        char  *destination ;
        char  *source ;
        size_t  length ;
        size_t  maximum ;
#    endif

{

    if (destination == NULL)  return (NULL) ;
    if (maximum < 1)  return (NULL) ;
    if (source == NULL)  source = "" ;

    if (length > (maximum - 1))  length = maximum - 1 ;
    strncpy (destination, source, length) ;
    destination[length] = '\0' ;

    return (destination) ;

}

/*!*****************************************************************************

Procedure:

    strndup2 ()

    Duplicate a String of a Specified Length; Return Duplicate in Argument.


Purpose:

    Function strndup2() duplicates a string of a specified length.
    strndup2() is similar to strndup(3) except that the duplicate
    string is returned as an argument and the function value is an
    error indicator.  This somewhat simplifies error checking in
    the calling program.


    Invocation:

        status = strndup2 (string, length, &duplicate) ;

    where:

        <string>	- I
            is the string to be duplicated.
        <length>	- I
            is the number of characters to be duplicated.
        <duplicate>	- O
            returns a MALLOC(3)ed copy of the first LENGTH characters
            of the input string.  The resulting string is NUL-terminated
            and the caller is responsible for FREE(3)ing it.
        <status>	- O
            returns the status of duplicating the LENGTH-character string,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  strndup2 (

#    if PROTOTYPES
        const  char  *string,
        size_t  length,
        char  **duplicate)
#    else
        string, length, duplicate)

        char  *string ;
        size_t  length ;
        char  **duplicate ;
#    endif

{

    if (duplicate != NULL)  *duplicate = NULL ;

    if ((string == NULL) || (duplicate == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strndup2) NULL string or duplicate argument: ") ;
        return (errno) ;
    }

    *duplicate = strndup (string, length) ;
    if (*duplicate == NULL) {
        LGE "(strndup2) Error duplicating %"PRIuSIZE_T"-byte string.\n\"%.*s\"\nstrndup: ",
            length, length, string) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    strndupx ()

    Duplicate a Length-Limited String with Extra Room.


Purpose:

    Function strndupx() duplicates a string of a specified length in a
    buffer with room for extra characters to be appended.  The size of
    the dynamically allocated buffer is:

        LENGTH + EXTRA + 1

    where LENGTH is the number of characters to be copied from the source
    string, EXTRA is the number of characters that may be appended by the
    caller, and 1 is the terminating NUL character.

    NOTE that, unlike strndup(3), this function uses the caller-supplied
    LENGTH argument to size the buffer, not the actual size of the source
    string.  If the source string is a NUL-terminated string shorter than
    the caller-supplied length, strndup(3) duplicates the string in a
    shorter buffer, while strndupx() still duplicates the string in a
    buffer of the specified size.

    NOTE also that, if the source string is shorter than LENGTH, the caller
    is responsible for appending characters in the desired positions.


    Invocation:

        duplicate = strndupx (string, length, extra) ;

    where:

        <string>	- I
            is the string to be duplicated.
        <length>	- I
            is the number of characters to be copied from the source string.
        <extra>		- I
            is the number of characters to make room for at the end of the
            duplicate string.
        <duplicate>	- O
            returns a CALLOC(3)ed copy of the input string, with room to spare.
            The duplicate is NUL-terminated and the caller can append up to
            EXTRA characters (plus the training NUL terminator).  The caller
            is responsible for FREE(3)ing the duplicate string.  NULL is
            returned in the event of an error.

*******************************************************************************/


char  *strndupx (

#    if PROTOTYPES
        const  char  *string,
        size_t  length,
        size_t  extra)
#    else
        string, length, extra)

        char  *string ;
        size_t  length ;
        size_t  extra ;
#    endif

{    /* Local variables. */
    char  *duplicate ;
    size_t  total ;



    if (string == NULL)  string = "" ;

    total = length + extra + 1 ;

    duplicate = calloc (total, 1) ;
    if (duplicate == NULL) {
        LGE "(strndupx) Error duplicating %"PRIuSIZE_T"-character string with %"PRIuSIZE_T" extra characters.\n\"%.*s\"\ncalloc: ",
            length, extra, (int) length, string) ;
        return (NULL) ;
    }

    strncpy (duplicate, string, length) ;

    return (duplicate) ;

}

/*!*****************************************************************************

Procedure:

    strConvert()

    Convert Escaped Sequences in a String.


Purpose:

    Function strConvert() scans an input string and replaces the following
    escaped sequences:

        \<num>  -  is replaced by a single character with the specified ASCII
                   code; e.g., "\32" is replaced by the space character (" "),
                   "\006" is replaced by the bell character, and "\0x7F" is
                   replaced by the rubout character.

        \<char> -  is replaced by the specified character; e.g., "\\" is
                   replaced by "\".

    The input string is changed in place; i.e., it is overwritten by the
    output string.


    Invocation:

        result = strConvert (string) ;

    where

        <string>
            is a writeable, NUL-terminated string to be scanned and converted.
        <result>
            returns the input string (which was converted in-place).

*******************************************************************************/


char  *strConvert (

#    if PROTOTYPES
        char  *string)
#    else
        string)

        char  *string ;
#    endif

{    /* Local variables. */
    char  c, *s, *t, *v ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strConvert) NULL string: ") ;
        return (NULL) ;
    }

    s = t = string ;

    while (*s != '\0') {
        v = s ;
        if (*s == '\\') {		/* "\" character? */
            s++ ;
            c = (char) strtol (s, &v, 0) ;
            if (v == s) {		/* (V == S) if conversion error. */
                *t = *s++ ;		/* Escaped literal. */
                switch (*t) {
                case 'a':  *t++ = 0x07 ;  break ;
                case 'b':  *t++ = 0x08 ;  break ;
                case 'e':  *t++ = 0x1B ;  break ;
                case 'f':  *t++ = 0x0C ;  break ;
                case 'l':  *t++ = 0x0A ;  break ;
                case 'n':  *t++ = 0x0A ;  break ;
                case 'r':  *t++ = 0x0D ;  break ;
                case 't':  *t++ = 0x09 ;  break ;
                case 'v':  *t++ = 0x0B ;  break ;
                case 'z':  *t++ = 0x00 ;  break ;
                default:
                    t++ ;  break ;
                }
            } else {
                *t++ = c ;		/* "\nnn" sequence converted. */
                s = v ;
            }
        } else {			/* Non-"\" character. */
            *t++ = *s++ ;
        }
    }

    *t = '\0' ;

    return (string) ;

}

/*!*****************************************************************************

Procedure:

    strDesmart()

    Replace Smart Characters in a String.


Purpose:

    Function strDesmart() replaces "smart" characters in a string with their
    "non-smart" equivalents.  For example, left- and right-curly quotes are
    replaced with the basic ASCII double quote: '"'.  See the "smartMap"
    table in the code to see exactly what replacements are made.

    3-character UTF-8 sequences for smart characters are replaced *before*
    Microsoft 1-character sequences.  Some of the UTF-8 sequences contain
    bytes with the same value as the Microsoft characters, so replacing the
    Microsoft characters first would corrupt the UTF-8 sequences.

        NOTE:

        strDesmart() steps through the smartMap table, trying to
        replace each entry in the target string.  Consequently, the
        length of the string may stretch and shrink in the process
        depending on the order of the entries in the table.

        For example, if the target string has 2 ellipses (\x85 replaced
        with "...") and 4 soft hyphens (\xAD replaced by ""), the ellipsis
        is earlier in the table and ellipses will be replaced before soft
        hyphens.  So the string will initially expand by 4 characters for
        the 2 ellipses and then contract by 4 characters for the 4 soft
        hyphens.  The final string length is equal to the input string
        length, but the length did increase when replacing the ellipses.

        In short, the string buffer's maximum size must be large enough
        to hold the intermediate string lengths, not just the final length.

        NOTE:

        If replacing a smart character in the target string would
        cause the string to overflow its buffer, the replacement
        is not performed and ERANGE is returned as the function's
        error status.  If the original string had multiple smart
        characters, it's possible that some characters have already
        been replaced and the string modified before the potential
        overflow is detected.  In that case, the call could
        reallocate the modified string into a larger buffer
        and re-call strDesmart() to continue replacing smart
        characters in the string.


    Invocation:

        status = strDesmart (string, length, maxLength) ;

    where

        <string>	- I/O
            is the string containing the smart characters.  The replacement
            is done in-place, so the string buffer must be large enough to
            hold the converted string.  (A worst-case scenario is a string
            consisting entirely of Microsoft ellipsis characters, each of
            which would be replaced by 3 period characters; i.e., "...".)
        <length>	- I
            is the length of the NUL-terminated string; if -1 is specified,
            strDesmart() will call strlen(3) to determine the length.
        <maxLength>	- I
            is the size of the string buffer which, as explained above,
            should be large enough to hold the converted string.  (See
            the worst-case scenario immediately above.)  If the string
            is NUL-terminated and the replacement text will always be
            shorter than the text replaced, specify -1 and strDesmart()
            will determine and use the input string's length as the
            maximum length.
        <status>	- O
            returns the status of the replacing the smart characters in the
            string, zero if there were no errors and ERRNO otherwise.  In
            particular, if a replacement would cause the string to overflow
            its buffer, ERANGE is returned.  See the note above about how to
            handle this error.

*******************************************************************************/


errno_t  strDesmart (

#    if PROTOTYPES
        char  *string,
        ssize_t  length,
        ssize_t  maxSize)
#    else
        string, length, maxSize)

        char  *string ;
        ssize_t  length ;
        ssize_t  maxSize ;
#    endif

{    /* Local variables. */
    size_t  i ;

    /* The smart-character table maps UTF-8 sequences and Microsoft smart
       characters to ASCII replacements; for example, curly quotes are mapped
       to straight quotes.  The ordering of the table is important with respect
       to the UTF-8 sequences and the Microsoft smart characters.  The UTF-8
       3-character sequences must be replaced BEFORE the Microsoft 1-character
       sequences.  Some of the UTF-8 sequences coincidentally contain some
       Microsoft character sequences and replacing the Microsoft characters
       first would corrupt the UTF-8 sequences. */

    static  const  char  *smartMap[][2]  OCD ("str_util") = {
		/* UTF-8 2-character sequences. */
		/* http://www.utf8-chartable.de/ */
        { "\xC2\xA6", "|" },		/* Broken bar */
        { "\xC2\xA9", "(c)" },		/* Copyright sign */
        { "\xC2\xAD", "" },		/* Soft hyphen */
		/* UTF-8 3-character sequences. */
		/* http://www.utf8-chartable.de/unicode-utf8-table.pl?start=8192 */
        { "\xE2\x80\x82", " " },	/* En space */
        { "\xE2\x80\x83", "  " },	/* Em space */
        { "\xE2\x80\x93", "-" },	/* En dash */
        { "\xE2\x80\x94", "--" },	/* Em dash */
        { "\xE2\x80\x98", "'" },	/* Left single quotation mark */
        { "\xE2\x80\x99", "'" },	/* Right single quotation mark */
        { "\xE2\x80\x9A", "," },	/* Single low-9 quotation mark */
        { "\xE2\x80\x9C", "\"" },	/* Left double quotation mark */
        { "\xE2\x80\x9D", "\"" },	/* Right double quotation mark */
        { "\xE2\x80\x9E", ",," },	/* Double low-9 quotation mark */
        { "\xE2\x80\xA2", "*" },	/* Bullet */
        { "\xE2\x80\xA6", "..." },	/* Horizontal ellipsis */
        { "\xE2\x80\xB2", "'" },	/* Prime */
        { "\xE2\x80\xB3", "\"" },	/* Double prime */
        { "\xE2\x80\xB9", "<" },	/* Single left-pointing angle quotation mark */
        { "\xE2\x80\xBA", ">" },	/* Single right-pointing angle quotation mark */
        { "\xE2\x81\x84", "/" },	/* Fraction slash */
		/* http://www.utf8-chartable.de/unicode-utf8-table.pl?start=8704 */
        { "\xE2\x88\x92", "-" },	/* Minus sign */
        { "\xE2\x88\x97", "*" },	/* Asterisk operator */
        { "\xE2\x88\xBC", "~" },	/* Tilde operator */
		/* http://www.utf8-chartable.de/unicode-utf8-table.pl?start=9984 */
        { "\xE2\x9D\x98", "|" },	/* Light vertical bar */
		/* Microsoft smart characters. */
		/* http://www.kostis.net/charsets/cp1252.htm */
        { "\x82", "," },		/* Single low-9 quotation mark */
        { "\x84", ",," },		/* Double low-9 quotation mark */
        { "\x85", "..." },		/* Horizontal ellipsis */
        { "\x88", "^" },		/* Modifier letter circumflex accent */
        { "\x8B", "<" },		/* Single left-pointing angle quotation mark */
        { "\x8C", "OE" },		/* Latin capital ligature OE */
        { "\x91", "'" },		/* Left single quotation mark */
        { "\x92", "'" },		/* Right single quotation mark */
        { "\x93", "\"" },		/* Left double quotation mark */
        { "\x94", "\"" },		/* Right double quotation mark */
        { "\x95", "*" },		/* Bullet */
        { "\x96", "-" },		/* En dash */
        { "\x97", "--" },		/* Em dash */
        { "\x98", "~" },		/* Small tilde */
        { "\x9B", ">" },		/* Single right-pointing angle quotation mark */
        { "\x9C", "oe" },		/* Latin small ligature oe */
        { "\xA6", "|" },		/* Broken bar */
        { "\xA9", "(c)" },		/* Copyright sign */
        { "\xAD", "" },			/* Soft hyphen */
        { NULL, NULL }
    } ;



    if (length < 0)  length = (ssize_t) strlen (string) ;
    if (maxSize < 0)  maxSize = length + 1 ;

/* For each UTF-8 sequence or smart character to be replaced, do so. */

    for (i = 0 ;  smartMap[i][0] != NULL ;  i++) {
        if (strReplace (string, maxSize, false,
                        smartMap[i][0], -1,
                        smartMap[i][1], -1,
                        -1, NULL)) {
            LGE "(strDsmart) Error replacing \"%s\" with \"%s\".\nstrReplace: ",
                smartMap[i][0], smartMap[i][1]) ;
            return (errno) ;
        }
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    strDestring ()

    Resolve Quote-Delimited Elements in a String.


Purpose:

    Function strDestring() scans a string, replacing quote-delimited
    substrings by the text within the quotes.  For example, assuming
    that the allowed quote characters were single quotes, double quotes,
    and curly braces, the following conversions would be produced by
    strDestring():

                ab		==>	ab
		"ab cd"		==>	ab cd
		ab"cd"		==>	abcd
		"ab"'cd'	==>	abcd
		"ab'cd"		==>	ab'cd
		{ab"Hello!"cd}	==>	ab"Hello!"cd


    Invocation:

        result = strDestring (string, length, quotes) ;

    where:

        <string>	- I
            is the string to be "destring"ed.
        <length>	- I
            is the length of the string to be "destring"ed.  If LENGTH
            is less than 0, the input string (STRING) is assumed to be
            NUL-terminated and the processing of the string will be done
            in place (i.e., the input string will be modified).  If LENGTH
            is greater than or equal to 0, then it specifies the number of
            characters of STRING that are to be processed.  In the latter
            case, strDestring() will dynamically allocate new storage to
            hold the processed string; the input string will not be touched.
        <quotes>	- I
            is a pointer to a character string that contains the allowable
            quote characters.  For example, single and double quotes (the UNIX
            shell quote characters) would be specified as "\"'".  If a left
            brace, bracket, or parenthesis is specified, strDestring() is
            smart enough to look for the corresponding right brace, bracket,
            or parenthesis.
        <result>	- O
            returns a pointer to the processed string.  If the LENGTH
            argument was less than zero, the "destring"ing was performed
            directly on the input string and RESULT simply returns the
            input STRING argument.  If the LENGTH argument was greater
            than or equal to zero, then the "destring"ing was performed
            on a copy of the input string and RESULT returns a pointer
            to this dynamically-allocated string.  In the latter case,
            the calling routine is responsible for FREE(3)ing the result
            string.  A static empty string ("") is returned in the event
            of an error.

*******************************************************************************/


char  *strDestring (

#    if PROTOTYPES
        char  *string,
        ssize_t  length,
        const  char  *quotes)
#    else
        string, length, quotes)

        char  *string ;
        ssize_t  length ;
        char  *quotes ;
#    endif

{    /* Local variables. */
    char  *eos, rh_quote, *s ;




    if (string == NULL)  return ("") ;
    if (quotes == NULL)  quotes = "" ;

    if (length >= 0) {				/* Make copy of input string. */
        s = strndup (string, length) ;
        if (s == NULL) {
            LGE "(strDestring) Error duplicating: \"%.*s\"\nstrndup: ",
                (int) length, string) ;
            return (NULL) ;
        }
        string = s ;
    }


/* Scan the new argument and determine its length. */

    for (s = string ;  *s != '\0' ;  s++) {

        if (strchr (quotes, *s) == NULL)	/* Non-quote character? */
            continue ;

        switch (*s) {				/* Determine right-hand quote. */
        case '{':  rh_quote = '}' ;  break ;
        case '[':  rh_quote = ']' ;  break ;
        case '(':  rh_quote = ')' ;  break ;
        default:
            rh_quote = *s ;  break ;
            break ;
        }

        eos = strchr (s+1, rh_quote) ;		/* Locate right-hand quote. */
        if (eos == NULL)			/* Assume quote at NUL terminator. */
            eos = s + strlen (s) ;
        else					/* Pull down one character. */
            (void) memmove (eos, eos+1, strlen (eos+1) + 1) ;

						/* Pull down one character. */
        (void) memmove (s, s+1, strlen (s+1) + 1) ;
        s = eos - 2 ;				/* 2 quotes gone! */

    }


/* Return the processed string to the calling routine. */

    return (string) ;

}

/*!*****************************************************************************

Procedure:

    strDetab ()


Purpose:

    Function strDetab() converts tabs in a string to blanks.


    Invocation:

        detabbedLength = strDetab (stringWithTabs, length, tabStops,
                                   stringWithoutTabs, maxLength) ;

    where

        <stringWithTabs>	- I/O
            is a pointer to the string containing tabs.
        <length>		- I
            specifies the length of the string containing tabs.  If LENGTH
            is less than zero, strDetab determines the length by scanning
            STRING_WITH_TABS for a terminating NUL character.
        <tabStops>		- I
            specifies the number of characters between tab stops.  The
            default is 8 characters.
        <stringWithoutTabs>	- I/O
            is a pointer to a string buffer that will receive the expanded
            string.  The string will always be NUL-terminated (and truncated
            to a length of MAX_LENGTH-1 if necessary).  If this argument is
            NULL, strDetab() performs the conversion in place on
            STRING_WITH_TABS, subject to the MAX_LENGTH restriction.
        <maxLength>		- I
            is the size of the STRING_WITHOUT_TABS buffer that will receive
            the expanded string.  If the STRING_WITHOUT_TABS pointer is NULL,
            then MAX_LENGTH specifies the maximum size of the STRING_WITH_TABS
            buffer.
        <detabbedLength>	- O
            returns the length of the expanded string.

*******************************************************************************/


size_t  strDetab (

#    if PROTOTYPES
        char  *stringWithTabs,
        ssize_t  length,
        int  tabStops,
        char  *stringWithoutTabs,
        size_t  maxLength)
#    else
        stringWithTabs, length, tabStops, stringWithoutTabs, maxLength)

        char  *stringWithTabs ;
        ssize_t  length ;
        int  tabStops ;
        char  *stringWithoutTabs ;
        size_t  maxLength ;
#    endif

{    /* Local variables. */
    char  *s ;
    int  numSpaces ;
    size_t  i ;



    if (stringWithTabs == NULL) {
        if (stringWithoutTabs != NULL)
            *stringWithoutTabs = '\0' ;
        return (0) ;
    }

    if (length < 0)  length = strlen (stringWithTabs) ;
    if (tabStops <= 0)  tabStops = 8 ;

    if (stringWithoutTabs == NULL)
        stringWithoutTabs = stringWithTabs ;
    else
        strncpym (stringWithoutTabs, stringWithTabs, length, maxLength) ;

/* For each tab character in the string, delete the tab character and insert
   the number of spaces necessary to shift the following text to the next
   tab stop. */

    for (i = 0, s = stringWithoutTabs ;
         (length-- > 0) && (*s != '\0') ;
         i++, s++) {
        if (*s != '\t')  continue ;
        numSpaces = tabStops - (i % tabStops) - 1 ;  *s = ' ' ;
        if (numSpaces > 0) {
            if (strInsert (NULL, numSpaces, 0, s, maxLength - i))
                break ;		/* No more room in string buffer. */
            s = s + numSpaces ;  i = i + numSpaces ;
        }
    }

    return (strTrim (stringWithoutTabs, -1)) ;

}

/*!*****************************************************************************

Procedure:

    strDupCat()

    Duplicate the Concatenation of Multiple Strings.


Purpose:

    Function strDupCat() returns the dynamically allocated concatenation
    of multiple strings.  For example,

        strDupCat (3, directoryName, "/", fileName) ;

    returns a dynamically allocated string, "<directory>/<files".  This
    mode of calling the function uses a counted list of strings.  Any or
    all of the string arguments in a counted list can be NULL, in which
    case a zero-length string is logically added to the result.

    An alternative to a counted list is a NULL-terminated list of strings.
    A count of -1 indicates this mode of calling the function:

        strDupCat (-1, directoryName, "/", fileName, NULL) ;

    returns the same result as the first example.  In a NULL-terminated
    list, there obviously can't be any embedded NULL strings as in a
    counted list.

        NOTE: There is a limit to the number of strings that
        can be passed in, MAX_STRDUPCAT_N, an internal macro
        currently defined as 6.  If more than 6 strings must
        be concatenated, simply batch them into multiple calls
        to strDupCat() and then call strDupCat() to concatenate
        the batches; remember to deallocate the intermediate
        strings.

        This limit was necessary because C's variable-length
        mechanism does not allow rescanning an argument list
        (once to determine the length of the concatenated
        string and again to actually build the string).
        C99 introduced the va_copy() macro/function to make
        a copy of the argument list for rescanning, but I
        judged it simpler for my purposes to just save the
        string pointers in a fixed-size array.


    Invocation:

        catenation = strDupCat (numStrings,
                                <string1>, <string2>, ... <string3>) ;

    or

        catenation = strDupCat (-1, <string1>, <string2>, ... NULL) ;

    where

        <numStrings>			- I
            is the number of strings to concatenate.  If this argument
            is greater than zero, strDupCat() will concatenate that
            many strings; NULL pointers in the range will be treated
            as zero-length strings, "" (but do contribute to the length
            of the strings).  If the argument is -1, strDupCat() will
            concatenate strings until a NULL pointer is encountered.
            A counted-list length of zero and an empty NULL-terminated
            list (i.e., only the terminating NULL) are errors.  NOTE
            the limit on the number of strings noted above.
        <string1>, ..., <stringN>	- I
        <string1>, ..., NULL		- I
            are the strings to be concatenated.  In the counted version,
            NULL strings are treated as zero-length strings, "".
        <catenation>			- O
            returns the dynamically allocated concatenation of the input
            strings.  The caller is responsible for free(3)ing the string
            after it is no longer needed.  NULL is returned in the event
            of an error and ERRNO is set.

*******************************************************************************/


char  *strDupCat (

#    if PROTOTYPES
        ssize_t  numStrings,
        ...)
#    else
        numStrings, va_alist)

        ssize_t  numStrings ;
        va_dcl
#    endif

{    /* Local variables. */
    va_list  ap ;
    bool  tilNull ;
#ifndef MAX_STRDUPCAT_N
#    define  MAX_STRDUPCAT_N  6
#endif
    char  *catenation ;
    const  char  *inputString[MAX_STRDUPCAT_N] ;
    size_t  i, length ;



/* First, determine the length of the concatenated strings by summing the
   lengths of the input strings. */

#if HAVE_STDARG_H
    va_start (ap, numStrings) ;
#else
    va_start (ap) ;
#endif

    if (numStrings < 0) {	/* NULL-terminated list of strings? */
        tilNull = true ;
        numStrings = 0 ;
    } else {
        tilNull = false ;
    }

    for (i = 0, length = 0 ;  i < MAX_STRDUPCAT_N ;  i++) {
        if (!tilNull && (i == numStrings))  break ;
        inputString[i] = va_arg (ap, const char *) ;
        if (tilNull) {
            if (inputString[i] == NULL)
                break ;
            else
                numStrings++ ;
        }
        if (inputString[i] != NULL)  length += strlen (inputString[i]) ;
    }

    if (numStrings == 0) {
        SET_ERRNO (EINVAL) ;
        LGE "(strDupCat) Zero strings: ") ;
        va_end (ap) ;
        return (NULL) ;
    }

    if (i == MAX_STRDUPCAT_N) {
        if (tilNull && (va_arg (ap, const char *) != NULL)) {
            SET_ERRNO (EINVAL) ;
            LGE "(strDupCat) Too many (> %u) strings in NULL-terminated list: ",
                MAX_STRDUPCAT_N) ;
            va_end (ap) ;
            return (NULL) ;
        }
        if (!tilNull && (i < (size_t) numStrings)) {
            SET_ERRNO (EINVAL) ;
            LGE "(strDupCat) Too many (> %u) strings in counted list: ",
                MAX_STRDUPCAT_N) ;
            va_end (ap) ;
            return (NULL) ;
        }
    }

    va_end (ap) ;

/* Allocate a buffer to hold the concatenated strings and then concatenate
   the strings into the buffer. */

    catenation = malloc (length+1) ;
    if (catenation == NULL) {
        LGE "(strDupCat) Error allocating %"PRIuSIZE_T"-length buffer\nmalloc: ",
            length) ;
        return (NULL) ;
    }

    catenation[0] = '\0' ;

    for (i = 0 ;  i < (size_t) numStrings ;  i++) {
        if (inputString[i] != NULL)  strcat (catenation, inputString[i]) ;
    }


    return (catenation) ;

}

/*!*****************************************************************************

Procedure:

    strEnv ()


Purpose:

    Function strEnv() translates environment variables ("$<name>") and
    home directory references ("~") embedded in a string.  For example,
    if variable DG has been defined as "/usr/alex/dispgen", strEnv()
    will translate

                    "tpocc:$DG/page.tdl"

    as

                    "tpocc:/usr/alex/dispgen/page.tdl".

    Remember that C-Shell variables (defined using "set name = value") are
    NOT environment variables (defined using "setenv name value") and are
    NOT available to programs.  Define any variables you might need as
    environment variables.

    Environment variables can be nested, i.e., defined in terms of each other.
    Undefined environment variables are not an error and are assumed to have
    a value of "" (a zero-length string).

        NOTE:  strEnv() checks for translation-buffer overflow as the
        translation is performed.  If overflow would occur, strEnv()
        returns immediately with an ERANGE status.  The caller can
        call strEnv() again with a larger buffer if desired.

        If the caller uses the input string buffer as the translation
        buffer and that buffer is not large enough to hold the in-place
        translation, the input string may be partially or completely
        overwritten by the translation in process.  Consequently, if
        the caller plans to call strEnv() again with a larger buffer
        in the event of overflow, the caller must be sure to save a
        copy of the original string before calls to strEnv().


    Invocation:

        status = strEnv (string, length, &translation, maxLength) ;

    where

        <string>	- I
            is the string which contains environment variable references.
        <length>	- I
            is the length of the string.  If LENGTH is less than zero,
            strEnv() determines the length by scanning STRING for a
            terminating null character.
        <translation>	- O
            is the address of a buffer which will receive the translated
            string.  This can be the address of the input string, in which
            case the translation is performed in place; see the NOTE above
            about buffer overflow.
        <maxLength>	- I
            is the maximum length of the translation; i.e., the size of
            the translation buffer.
        <status>	- O
            returns the status of the translation, zero if there were no
            errors and ERRNO otherwise.  In particular, ERANGE is returned
            if the translation buffer is not large enough for the result;
            see the NOTE above about buffer overflow.

*******************************************************************************/


errno_t  strEnv (

#    if PROTOTYPES
        const  char  *string,
        ssize_t  length,
        char  *translation,
        size_t  maxLength)
#    else
        string, length, translation, maxLength)

        char  *string ;
        ssize_t  length ;
        char  *translation ;
        size_t  maxLength ;
#    endif

{    /* Local variables. */
    char  follow, *name, *s ;
    size_t  i ;



    if (translation == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strEnv) NULL translation buffer: ") ;
        return (errno) ;
    }

    if (string == NULL) {
        strcpy (translation, "") ;
        return (0) ;
    }

    if (length < 0)  length = strlen (string) ;
    strncpym (translation, string, length, maxLength) ;

/* Scan through the string, replacing "~"s by the user's home directory and
   environment variables ("$<name>") by their values. */

    for (i = 0 ;  translation[i] != '\0' ;  ) {

        if ((translation[i] == '~') &&			/* "~" */
            ((i == 0) || (translation[i-1] == ':'))) {

            s = getenv ("HOME") ;			/* Get home directory. */
            if (s == NULL) {
                i++ ;					/* Insert "~". */
            } else {
                strRemove (1, i, translation) ;		/* Insert home directory. */
                if (strInsert (s, -1, i, translation, maxLength))
                    return (errno) ;			/* Buffer overflow? */
            }

        } else if ((translation[i] == '$') &&		/* "$<name>" */
                   ((i == 0) || !isalnum (INT_UCHAR (translation[i-1])))) {

            name = &translation[i] ;			/* Extract "<name>". */
            name += strspn (name, "$./:[") ;
            length = strcspn (name, "$./:[") ;
            follow = name[length] ;  name[length] = '\0' ;
            s = getenv (name) ;				/* Lookup "<name>". */
            name[length] = follow ;
							/* Replace "$<name>" ... */
            strRemove (name - &translation[i] + length, i, translation) ;
            if ((s != NULL) &&				/* ... by "<value>". */
                strInsert (s, -1, i, translation, maxLength)) {
                return (errno) ;			/* Buffer overflow? */
            }

        } else {					/* Normal character. */

            i++ ;

        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    strEtoA ()


Purpose:

    Function strEtoA() converts an EBCDIC string to an ASCII string.  The
    conversion table for this program was created using the "dd conv=ascii"
    Unix program, so I hope it's right!


    Invocation:

        asciiString = strEtoA (ebcdicString, length) ;

    where

        <ebcdicString>	- I/O
            is a pointer to the EBCDIC string to be converted.  The
            EBCDIC-to-ASCII conversion is done in-place, so EBCDIC_STRING
            should be in writeable memory.
        <length>	- I
            specifies the length of the EBCDIC string.  If LENGTH is
            less than zero, strEtoA() determines the length by scanning
            EBCDIC_STRING for a terminating null character.
        <asciiString>	- O
            returns a pointer to the converted string.  Since the EBCDIC-to-
            ASCII conversion is done in-place, this pointer simply points to
            the input string, EBCDIC_STRING.

*******************************************************************************/


char  *strEtoA (

#    if PROTOTYPES
        char  *string,
        ssize_t  length)
#    else
        string, length)

        char  *string ;
        ssize_t  length ;
#    endif

{    /* Local variables. */
    unsigned  char  *s ;
    static  const  unsigned  char  ebcdic_to_ascii[256]  OCD ("str_util") = {
        0x00, 0x01, 0x02, 0x03, 0x9C, 0x09, 0x86, 0x7F,
        0x97, 0x8D, 0x8E, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
        0x10, 0x11, 0x12, 0x13, 0x9D, 0x85, 0x08, 0x87,
        0x18, 0x19, 0x92, 0x8F, 0x1C, 0x1D, 0x1E, 0x1F,
        0x80, 0x81, 0x82, 0x83, 0x84, 0x0A, 0x17, 0x1B,
        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x05, 0x06, 0x07,
        0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
        0x98, 0x99, 0x9A, 0x9B, 0x14, 0x15, 0x9E, 0x1A,
        0x20, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6,
        0xA7, 0xA8, 0x5B, 0x2E, 0x3C, 0x28, 0x2B, 0x21,
        0x26, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
        0xB0, 0xB1, 0x5D, 0x24, 0x2A, 0x29, 0x3B, 0x5E,
        0x2D, 0x2F, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7,
        0xB8, 0xB9, 0x7C, 0x2C, 0x25, 0x5F, 0x3E, 0x3F,
        0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1,
        0xC2, 0x60, 0x3A, 0x23, 0x40, 0x27, 0x3D, 0x22,
        0xC3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
        0x68, 0x69, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9,
        0xCA, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70,
        0x71, 0x72, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0,
        0xD1, 0x7E, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
        0x79, 0x7A, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,
        0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
        0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
        0x7B, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
        0x48, 0x49, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED,
        0x7D, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
        0x51, 0x52, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2, 0xF3,
        0x5C, 0x9F, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
        0x59, 0x5A, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x38, 0x39, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
    } ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strEtoA) NULL string: ") ;
        return (NULL) ;
    }

    if (length < 0)  length = strlen (string) ;

    for (s = (unsigned char *) string ;  length-- ;  s++)
        *s = ebcdic_to_ascii[(unsigned int) *s] ;

    return (string) ;

}

/*!*****************************************************************************

Procedure:

    strInsert ()


Purpose:

    Function strInsert() inserts N characters of text at any position in
    a string.  The insertion is performed in-place in the string unless
    the resulting length would be greater than the maximum size of the
    string.


    Invocation:

        status = strInsert (substring, length, offset, string, maxLength) ;

    where

        <substring>	- I
            points to the substring that will be inserted in STRING.  If
            this argument is NULL, then LENGTH blanks will be inserted in
            STRING.
        <length>	- I
            is the length of SUBSTRING.  If LENGTH is less than zero, the
            length is determined by searching for the NUL terminator in
            SUBSTRING.
        <offset>	- I
            is the character offset (0..N-1) in STRING at which SUBSTRING
            will be inserted.
        <string>	- I/O
            points to the writeable, NUL-terminated string into which text
            will be inserted.
        <maxLength>	- I
            is the size of the STRING buffer (including room for a NUL
            terminator for the resulting string).
        <status>	- O
            returns zero if the substring was successfully inserted and
            ERRNO otherwise.  In particular, ERANGE is returned if the
            size of the string buffer is insufficient to hold the existing
            string and the inserted substring.  The string buffer is not
            modified in this case, so the caller can try again with a
            larger buffer if desired.

*******************************************************************************/


errno_t  strInsert (

#    if PROTOTYPES
        const  char  *substring,
        ssize_t  subLength,
        size_t  offset,
        char  *string,
        size_t  maxLength)
#    else
        substring, subLength, offset, string, maxLength)

        char  *substring ;
        ssize_t  subLength ;
        size_t  offset ;
        char  *string ;
        size_t  maxLength ;
#    endif

{    /* Local variables. */
    char  *s ;
    size_t  length, origLength ;



/* Make sure arguments are all valid. */

    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strInsert) NULL input string: \n") ;
        return (errno) ;
    }

    origLength = strnlen (string, maxLength) ;
    if (origLength == maxLength) {
        SET_ERRNO (EINVAL) ;
        LGE "(strInsert) Input string exceeds maximum length of output buffer: \n") ;
        return (errno) ;
    }

    if (offset > origLength) {
        SET_ERRNO (EINVAL) ;
        LGE "(strInsert) Offset %"PRIuSIZE_T" > Input Length %"PRIuSIZE_T": \n",
            offset, origLength) ;
        return (errno) ;
    }

    if ((substring != NULL) && (subLength < 0))
        subLength = strlen (substring) ;
    if (subLength <= 0)  return (0) ;	/* Nothing to insert. */

/* Compute the number of characters following STRING[OFFSET] that can be
   shifted right to make room for SUBSTRING.  Stored in variable LENGTH,
   the number computed includes the NUL terminator at the end of the
   string (or an extraneous character if truncation will occur). */

    length = origLength + subLength + 1 ;
    if (length > maxLength) {
        SET_ERRNO (ERANGE) ;
        LGE "(strInsert) Insufficient space to insert \"%.*s\" into \"%.*s\": ",
            (int) subLength, (substring == NULL) ? " " : substring,
            (int) origLength, string) ;
        return (errno) ;
    }
    length = length - subLength - offset ;

/* Shift the text after the insertion offset to make room for the substring. */

    for (s = &string[offset+length-1] ;  length-- > 0 ;  s--)
        s[subLength] = *s ;
    s = s + subLength ;

/* Copy the substring into the string.  Variable S points to the end of the
   room made for inserting the substring.  For example, if the substring will
   be copied into character positions 2-7 of the target string, then S points
   to character position 7.  Variable SUB_LENGTH specifies the number of
   characters to copy from SUBSTRING. */

    length = subLength ;
    if (substring == NULL) {	/* Insert N blanks? */
        while (length-- > 0)
            *s-- = ' ' ;
    } else {				/* Insert N characters of text? */
        while (length-- > 0)
            *s-- = substring[length] ;
    }

    string[maxLength-1] = '\0' ;	/* Ensure NUL-termination. */

/* This function was extremely tedious to write.  The next time you
   extol the virtues of C, remember that this function would have been
   a one-liner in FORTRAN 77 or BASIC.  (Years after I originally wrote
   it, I changed this function to bail out when it first detects that
   insertion would cause truncation.  Removing truncation cleaned up
   the code somewhat at the expense of more extensive error checking
   to avoid truncation in the first place.) */

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    strMatch ()


Purpose:

    Function strMatch() matches a possibly-abbreviated target string against
    a model string.  For example, "C", "CO", "COO", and "COOK" are partial
    matches of "COOK"; "COOKS" is NOT a partial match of "COOK".


    Invocation:

        found = strMatch (target, model, ignoreCase) ;

    where

        <target>
            is the string to be checked for matching against the model string.
            In the example above, "C", "CO", "COO", etc. are target strings.
        <model>
            is the model string against which the match is tested.  In the
            example above, "COOK" is the model string.
        <ignoreCase>	- I
            specifies whether alphabetic case should be considered (false) or
            ignored (true) when matching the target string.
        <found>
            returns true (a non-zero value) if the target string is a partial
            or full match of the model string; false (zero) is returned if the
            target string bears no relation to the model string.

*******************************************************************************/


bool  strMatch (

#    if PROTOTYPES
        const  char  *target,
        const  char  *model,
        bool  ignoreCase)
#    else
        target, model, ignoreCase)

        char  *target ;
        char  *model ;
        bool  ignoreCase ;
#    endif

{    /* Local variables. */
    size_t  length ;



    if ((target == NULL) || (model == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strMatch) NULL target or model strings: ") ;
        return (false) ;
    }

    length = strlen (target) ;
    if (length > strlen (model))
        return (false) ;			/* Target string is too long. */
    else if (ignoreCase && (strncasecmp (target, model, length) == 0))
        return (true) ;				/* Matched. */
    else if (!ignoreCase && (strncmp (target, model, length) == 0))
        return (true) ;				/* Matched. */
    else
        return (false) ;			/* No match. */

}

/*!*****************************************************************************

Procedure:

    strRemove ()


Purpose:

    Function strRemove() removes N characters of text from any position in
    a string.


    Invocation:

        length = strRemove (numToRemove, offset, string) ;

    where

        <numToRemove>	- I
            is the number of characters to delete from the string.
        <offset>	- I
            is the character offset (0..N-1) in STRING at which the
            deletion will take place.
        <string>	- I/O
            points to the string from which text will be deleted.
        <length>	- O
            returns the length of the string after the deletion.

*******************************************************************************/


size_t  strRemove (

#    if PROTOTYPES
        size_t  numToRemove,
        size_t  offset,
        char  *string)
#    else
        numToRemove, offset, string)

        size_t  numToRemove ;
        size_t  offset ;
        char  *string ;
#    endif

{    /* Local variables. */
    size_t  length ;



/* Validate the arguments. */

    if (string == NULL)  return (0) ;
    length = strlen (string) ;
    if (offset >= length)  return (0) ;
    if (numToRemove > (length - offset))  numToRemove = length - offset ;

/* Remove the substring. */

    (void) memmove (&string[offset], &string[offset+numToRemove],
                    length - (offset + numToRemove) + 1) ;

    return (strlen (string)) ;

}

/*!*****************************************************************************

Procedure:

    strReplace ()

    Replace a Substring in a String.


Purpose:

    Function strReplace() finds and replaces one or more occurrences of
    a substring in a target string.  Replacement is not recursive; after
    replacing an instance of the substring, strReplace() skips past the
    replacement text before scanning for the next occurrence of the
    substring.

        NOTE:  If replacing found text in the target string would
        cause the string to overflow its buffer, the replacement
        is not performed and ERANGE is returned as the function's
        error status.

        If multiple replacements are to be made (maxReplace > 1),
        one or more replacements may have been performed prior to
        the replacement that would overflow.  In this case, the
        target string may have been modified.  If the replacement
        text is not recursive (i.e., it doesn't contain the find
        text), then the caller can reallocate the modified string
        into a larger buffer and re-call strReplace() with a new
        maxSize argument.  (A second function call begins scanning
        at beginning of the modified string again, hence the
        restriction that the replacement text not contain the
        find text.)


    Invocation:

        status = strReplace (target, maxSize, ignoreCase,
                             find, findlen,
                             replace, replacelen,
                             maxReplace, &numReplace) ;

    where:

        <target>	- I/O
            is the NUL-terminated target string in which the substring will
            be found and replaced.  The replacements are performed in-place
            in the target, so the string buffer must be large enough for
            expansion if the replacement text is longer than the search text.
        <maxSize>	- I
            is the size of the target string buffer and, therefore, the
            maximum length to which the target string (including the NUL
            terminator) can expand if the replacement text is longer than
            the search text.  If the replacement text is shorter than or
            equal in length to the the search text, a length of -1 can be
            specified.
        <ignoreCase>	- I
            specifies whether alphabetic case should be considered (false) or
            ignored (true) when matching the substring in the target string.
        <find>		- I
            is the substring to be found and replaced in the target string.
        <findLen>	- I
            is the length of the substring.  If the string is NUL-terminated,
            specify a length of -1.
        <replace>	- I
            is the text that will replace the substring in the target string.
        <replaceLen>	- I
            is the length of the replacement text.    If the string is
            NUL-terminated, specify a length of -1.
        <maxReplace>	- I
            is the maximum number of replacements to perform in the target
            string.  Specify -1 if all possible replacements are to be made.
        <numReplace>	- O
            returns the number of replacements made in the target string.
        <status>	- O
            returns the status of the find and replace, zero if there were
            no errors and ERRNO otherwise.  In particular, if a substitution
            would cause the string to overflow its buffer, ERANGE is returned.
            See the note above about how to handle this error.

*******************************************************************************/


errno_t  strReplace (

#    if PROTOTYPES
        char  *target,
        ssize_t  maxSize,
        bool  ignoreCase,
        const  char  *find,
        ssize_t  findLen,
        const  char  *replace,
        ssize_t  replaceLen,
        ssize_t  maxReplace,
        size_t  *numReplace)
#    else
        target, maxSize, ignoreCase,
        find, findLen,
        replace, replaceLen,
        maxReplace, numReplace)

        char  *target ;
        ssize_t  maxSize ;
        bool  ignoreCase ;
        char  *find ;
        ssize_t  findLen ;
        char  *replace ;
        ssize_t  replaceLen ;
        ssize_t  maxReplace ;
        size_t  *numReplace ;
#    endif

{    /* Local variables. */
    char  *s, *substring ;
    size_t  remaining, replacements ;
    ssize_t  delta, length ;



    if (target == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strReplace) NULL target string: ") ;
        return (errno) ;
    }

/* Sort out the arguments, supplying reasonable defaults when needed. */

    if (maxSize < 0) {
        length = (ssize_t) strlen (target) ;
        maxSize = length + 1 ;
    } else {
        length = strnlen (target, maxSize) ;
    }

    if (find == NULL) {
        find = "" ;  findLen = 0 ;
    } else if (findLen < 0) {
        findLen = (ssize_t) strlen (find) ;
    }
    if (findLen == 0)  maxReplace = 1 ;

    if (replace == NULL) {
        replace = "" ;  replaceLen = 0 ;
    } else if (replaceLen < 0) {
        replaceLen = (ssize_t) strlen (replace) ;
    }

    delta = replaceLen - findLen ;

    if (maxReplace < 0)  maxReplace = maxSize ;

/*******************************************************************************
    Starting at the beginning of the target string, find and replace each
    instance of the find string with the replacement string.
*******************************************************************************/

    s = target ;
    remaining = (size_t) maxSize ;
    if (numReplace == NULL)  numReplace = &replacements ;
    *numReplace = 0 ;

    while (*numReplace < (size_t) maxReplace) {

        if (ignoreCase)		/* Case-insensitive match. */
            substring = strncasestr (s, find, remaining) ;
        else			/* Exact match. */
            substring = strnstr (s, find, remaining) ;

        if (substring == NULL)  break ;		/* Find string not found? */

/* Check if the replacement would overflow the buffer. */

        length += delta ;
        if (length >= maxSize) {
            SET_ERRNO (ERANGE) ;
            LGE "(strReplace) Target buffer would overflow: \"%s\"\n(strReplace) Length: %"PRIdSSIZE_T"  Find: %"PRIdSSIZE_T"  Replace: %"PRIdSSIZE_T"  Max: %"PRIdSSIZE_T"\n",
                target, length, findLen, replaceLen, maxSize) ;
            return (errno) ;
        }

/* Does strRemove() include NUL terminator if present? */

        strRemove (findLen, 0, substring) ;

/* Does strInsert() keep NUL terminator if present?  Is remaining correct
   last argument?  How to recompute remaining after insert? */

        remaining -= substring - s ;

        strInsert (replace, replaceLen, 0, substring, remaining) ;

        (*numReplace)++ ;
						/* No more room for replace? */
        if ((size_t) replaceLen >= remaining)  break ;

/* Skip past the replaced text for the next find/replace. */

        remaining -= replaceLen ;
        s = substring + replaceLen ;

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    strTrim ()


Purpose:

    Function strTrim() trims trailing white space (blanks, tabs, and new-line
    characters) from a string.  If the length argument is less than zero, the
    string is assumed to be NUL-terminated; after trimming trailing white
    space, the NUL terminator is relocated to the new end of the string.  If
    the length argument is greater than or equal to zero, the string does NOT
    need to be NUL-terminated; after trimming trailing white space, the NUL
    terminator is NOT relocated.  In either case, strTrim() returns the length
    of the new string.


    Invocation:

            trimmedLength = strTrim (string, length) ;

    where

        <string>	- I/O
            is the string to be trimmed.  If the length argument is less
            than zero, STRING is assumed to be NUL-terminated and strTrim()
            will ***** relocate the NUL terminator *****.  If LENGTH is
            greater than or equal to zero, strTrim() will not relocate the
            NUL terminator; it simply computes the trimmed length.
        <length>	- I
            is the length, before trimming, of STRING.  If LENGTH is less
            than zero, STRING is assumed to be NUL-terminated.
        <trimmedLength>	- O
            returns the length of STRING with trailing blanks, tabs, and
            new-line characters removed.

*******************************************************************************/


size_t  strTrim (

#    if PROTOTYPES
        char  *string,
        ssize_t  length)
#    else
        string, length)

        char  *string ;
        ssize_t  length ;
#    endif

{    /* Local variables. */
    char  *s ;
    size_t  newLength ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strTrim) NULL string: ") ;
        return (0) ;
    }

    newLength = (length < 0) ? strlen (string) : length ;
    s = string + newLength ;

    while ((s-- != string) && ((*s == ' ') || (*s == '\t') ||
                               (*s == '\n') || (*s == '\r'))) {
        newLength-- ;
    }

    if (length < 0)  *++s = '\0' ;

    return (newLength) ;

}

/*!*****************************************************************************

Procedure:

    strTrimNL ()


Purpose:

    Function strTrimNL() trims trailing line endings (carriage returns,
    line feeds, and form feeds) from a string.  If the length argument
    is less than zero, the string is assumed to be NUL-terminated; after
    trimming trailing line endings, the NUL terminator is relocated to
    the new end of the string.  If the length argument is greater than
    or equal to zero, the string does NOT need to be NUL-terminated;
    after trimming trailing line endings, the NUL terminator is NOT
    relocated.  In either case, strTrimNL() returns the length of the
    new string.


    Invocation:

            trimmedLength = strTrimNL (string, length) ;

    where

        <string>	- I/O
            is the string to be trimmed.  If the length argument is less
            than zero, STRING is assumed to be NUL-terminated and strTrim()
            will ***** relocate the NUL terminator *****.  If LENGTH is
            greater than or equal to zero, strTrim() will not relocate the
            NUL terminator; it simply computes the trimmed length.
        <length>	- I
            is the length, before trimming, of STRING.  If LENGTH is less
            than zero, STRING is assumed to be NUL-terminated.
        <trimmedLength>	- O
            returns the length of STRING with trailing line endings removed.

*******************************************************************************/


size_t  strTrimNL (

#    if PROTOTYPES
        char  *string,
        ssize_t  length)
#    else
        string, length)

        char  *string ;
        ssize_t  length ;
#    endif

{    /* Local variables. */
    char  *s ;
    size_t  newLength ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strTrim) NULL string: ") ;
        return (0) ;
    }

    newLength = (length < 0) ? strlen (string) : length ;
    s = string + newLength ;

    while ((s-- != string) &&
           ((*s == '\f') || (*s == '\n') || (*s == '\r'))) {
        newLength-- ;
    }

    if (length < 0)  *++s = '\0' ;

    return (newLength) ;

}

#if !defined(HAVE_MEMDUP) || !HAVE_MEMDUP
/*!*****************************************************************************

Procedure:

    memdup ()

    Duplicate a Memory Block.


Purpose:

    Function memdup() duplicates a block of memory.  A new block of memory is
    dynamically allocated and the contents of the old block is copied over.

        NOTE: This is not a standard C function.  There are many versions
        of this function in many repositories on the Internet.  There are
        also many people opposed to the existence of this function, as well
        strdup().  The oddest argument, for me, against memdup() concerned
        a hypothetical situation in which someone wants to create a new block
        larger than the old block and, therefore, passes in a length argument
        that exceeds the length of the old block.  This could potentially be
        a problem because memdup() would attempt to access memory past the end
        of the old block.  My response would be, first, don't ask memdup() to
        do something different than what it explicitly says it does.  Second,
        this is the C language.  If you are troubled by memdup() and/or
        strdup(), switch to another language that uses address/length
        descriptors or some other means of managing memory blocks and strings.
        (In my experience, the only language worse than C for manipulating
        strings was strict Pascal.  Basic FORTH is not too good at it either,
        although defining some higher-level string words made FORTH palatable.)
        Sorry, I just feel like ranting!


    Invocation:

        duplicate = memdup (source, length) ;

    where:

        <source>	- I
            is the memory block to be duplicated.
        <length>	- I
            is the length in bytes of the existing block.  The length must
            be greater than zero.
        <duplicate>	- O
            returns a MALLOC(3)ed copy of the source memory block.  The
            caller is responsible for FREE(3)ing the duplicate block when it
            is no longer needed.  NULL is returned in the event of an error.

*******************************************************************************/


void  *memdup (

#    if PROTOTYPES
        const  void  *source,
        size_t  length)
#    else
        source, length)

        void  *source ;
        size_t  length ;
#    endif

{    /* Local variables. */
    void  *duplicate ;



    if ((source == NULL) || (length == 0)) {
        SET_ERRNO (EINVAL) ;
        LGE "(memdup) NULL or zero-length memory block: ") ;
        return (NULL) ;
    }

/* Allocate the new memory block. */

    duplicate = malloc (length) ;
    if (duplicate == NULL) {
        LGE "(memdup) Error allocating %lu-byte block.\nmalloc: ",
            (unsigned long) length) ;
        return (NULL) ;
    }

/* Copy the contents of the old block over to the new block. */

    (void) memcpy (duplicate, source, length) ;

    return (duplicate) ;

}
#endif	/* !HAVE_MEMDUP */

#if !defined(HAVE_MEMXLT) || !HAVE_MEMXLT
/*!*****************************************************************************

Procedure:

    memxlt ()

    Translate a Memory Block.


Purpose:

    Function memxlt() translates the bytes in a memory block using a table
    that maps bytes to their translation.  The table is a 256-byte array;
    each byte in the memory block is treated as a 0-255 index into the
    table and is replaced by the indexed entry in the table.  The xltable()
    function provides an easy way to construct translation tables.

    Function memxlt() is my own implementation of the memxlt() function
    provided by the SAS/C Compiler and Library for IBM mainframe computers.
    I've never used SAS/C or an IBM mainframe computer.
    (https://support.sas.com/documentation/onlinedoc/sasc/doc750/html/lr1/z2044598.htm)


    Invocation:

        block = memxlt (block, table, length) ;

    where:

        <block>		- I/O
            is the memory block to be translated.  The translation is
            performed in-place.
        <table>		- I
            is the translation table that maps input bytes to their
            replacement bytes.
        <length>	- I
            is the length in bytes/characters of the memory block.
        <block>		- O
            returns the input BLOCK argument.

*******************************************************************************/


void  *memxlt (

#    if PROTOTYPES
        void  *block,
        const  char  *table,
        size_t  length)
#    else
        block, table, length)

        void  *block ;
        char  *table ;
        size_t  length ;
#    endif

{    /* Local variables. */
    char  *s ;
    unsigned  int  i, index ;



    if (table == NULL)  return (block) ;	/* No translation? */
    if (block == NULL)  return (NULL) ;

/* Replace each byte in the memory block with its mapping from the table. */

    s = (char *) block ;
    for (i = 0 ;  i < length ;  i++) {
        index = *((unsigned char *) s) ;
        *s++ = table[index] ;
    }

    return (block) ;

}
#endif	/* !HAVE_MEMXLT */

#if defined(HAVE_STPCPY) && !HAVE_STPCPY
/*!*****************************************************************************

Procedure:

    stpcpy ()

    Copy a String, Point to the End.


Purpose:

    Function stpcpy() copies a NUL-terminated string to a destination buffer
    and returns a pointer to the end of the destination string (i.e., the
    NUL terminator).  The function is useful for concatenating literal
    strings:

        char  sentence[128] ;
        stpcpy (stpcpy (stpcpy (sentence, "What "), " About"), " Bob?) ;


    Invocation:

        end = stpcpy (destination, source) ;

    where:

        <destination>	- O
            is the address of the buffer into which the string will be copied.
        <source>	- I
            is the NUL-terminated string to be copied.
        <end>		- O
            returns a pointer to the NUL terminator in the destination buffer.

*******************************************************************************/


char  *stpcpy (

#    if PROTOTYPES
        char  *destination,
        const  char  *source)
#    else
        destination, source)

        char  *destination ;
        char  *source ;
#    endif

{

    if ((destination == NULL) || (source == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(stpcpy) NULL source or destination strings: ") ;
        return (NULL) ;
    }

    while (*source != '\0') {
        *destination++ = *source++ ;
    }

    *destination = '\0' ;

    return (destination) ;

}
#endif	/* !HAVE_STPCPY */

#if defined(HAVE_STRCASECMP) && !HAVE_STRCASECMP
/*!*****************************************************************************

Procedure:

    strcasecmp ()

    Compare Two Strings, Ignoring Case.


Purpose:

    Function strcasecmp() compares two strings, ignoring the case of the
    individual characters.  strcasecmp() is supported by many C libraries,
    but it is not part of the ANSI C library.


    Invocation:

        comparison = strcasecmp (thisString, thatString) ;

    where:

        <thisString>	- I
        <thatString>	- I
            are the NUL-terminated strings being compared.
        <comparison>	- O
            returns one of three possible values:
                < 0 if THISSTRING is lexically less than THATSTRING,
                = 0 if the two strings are equal except for case, or
                > 0 if THISSTRING is lexically greater than THATSTRING.
            The less-than and greater-than relations are also case-insensitive.

*******************************************************************************/


int  strcasecmp (

#    if PROTOTYPES
        const  char  *thisString,
        const  char  *thatString)
#    else
        thisString, thatString)

        char  *thisString ;
        char  *thatString ;
#    endif

{    /* Local variables. */
    char  *that, *this ;



/* Check for NULL strings. */

    if ((thisString == NULL) && (thatString != NULL))
        return (-1) ;
    else if ((thisString == NULL) && (thatString == NULL))
        return (0) ;
    else if ((thisString != NULL) && (thatString == NULL))
        return (1) ;

/* Compare the two strings, character by character. */

    this = (char *) thisString ;  that = (char *) thatString ;
    while ((*this != '\0') && (*that != '\0')) {
        if (toupper (*this) < toupper (*that))
            return (-1) ;
        else if (toupper (*this) > toupper (*that))
            return (1) ;
        this++ ;  that++ ;
    }

/* The strings are identical (excepting case) as far as the shorter string
   goes.  Therefore, the shorter string is lexically less than the longer
   string. */

    if (*this != '\0')
        return (1) ;
    else if (*that != '\0')
        return (-1) ;
    else
        return (0) ;

}
#endif	/* !HAVE_STRCASECMP */

#if !defined(HAVE_STRCASESTR) || !HAVE_STRCASESTR
/*!*****************************************************************************

Procedure:

    strcasestr ()

    Find a Substring in a String, Ignoring Case.


Purpose:

    Function strcasestr() searches a string looking for a substring, ignoring
    the case of the individual characters.  strcasestr() is supported by many
    C libraries, but it is not part of the ANSI C library.


    Invocation:

        result = strcasestr (string, substring) ;

    where:

        <string>	- I
            is the NUL-terminated string being searched.
        <substring>	- I
            is the NUL-terminated substring to be found in STRING.
        <result>	- O
            returns a pointer to the start of the substring in STRING, if the
            substring is found, and NULL otherwise.  For historical reasons,
            I guess, the function's return value is "char *" despite the input
            arguments being "const char *".

*******************************************************************************/


char  *strcasestr (

#    if PROTOTYPES
        const  char  *string,
        const  char  *substring)
#    else
        string, substring)

        char  *string ;
        char  *substring ;
#    endif

{
    return (strncasestr (string, substring,
                         (string == NULL) ? 0 : strlen (string))) ;
}
#endif	/* !HAVE_STRCASESTR */

#if defined(HAVE_STRCSPN) && !HAVE_STRCSPN
/*!*****************************************************************************

Procedure:

    strcspn ()

    Skip Characters Not in a Set of Characters.


Purpose:

    Function strcspn() spans the initial segment of a string such that the
    characters in the initial segment are all from the complement of a
    caller-specified set of characters.  Despite being part of the ANSI C
    library, strcspn() is not always available.


    Invocation:

        length = strcspn (string, reject) ;

    where:

        <string>	- I
            is the string to be scanned.
        <reject>	- I
            is a string containing the characters not allowed in the initial
            segment.
        <length>	- O
            returns the length of the initial segment containing characters
            not in the caller-specified set.

*******************************************************************************/


size_t  strcspn (

#    if PROTOTYPES
        const  char  *string,
        const  char  *reject)
#    else
        string, reject)

        char  *string ;
        char  *reject ;
#    endif

{    /* Local variables. */
    char  *s, *t ;



    if ((string == NULL) || (reject == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strcspn) NULL string or reject set: ") ;
        return (0) ;
    }

    for (s = (char *) string ;  *s != '\0' ;  s++) {
        for (t = (char *) reject ;  *t != '\0' ;  t++) {
            if (*t == *s)  break ;		/* Character in set? */
        }
        if (*t == *s)  break ;			/* Character in set? */
    }

    return (s - string) ;

}
#endif	/* !HAVE_STRCSPN */

#if defined(HAVE_STRDUP) && !HAVE_STRDUP
/*!*****************************************************************************

Procedure:

    strdup ()

    Duplicate a NUL-Terminated String.


Purpose:

    Function strdup() duplicates a NUL-terminated string.  strdup() is
    supported by many C libraries, but it is not part of the ANSI C library.


    Invocation:

        duplicate = strdup (string) ;

    where:

        <string>	- I
            is the string to be duplicated.
        <duplicate>	- O
            returns a MALLOC(3)ed copy of the input string.  The caller
            is responsible for FREE(3)ing the duplicate string.  NULL is
            returned in the event of an error.

*******************************************************************************/


char  *strdup (

#    if PROTOTYPES
        const  char  *string)
#    else
        string)

        char  *string ;
#    endif

{    /* Local variables. */
    char  *duplicate ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strdup) NULL string: ") ;
        return (NULL) ;
    }

    duplicate = malloc (strlen (string) + 1) ;
    if (duplicate == NULL) {
        LGE "(strdup) Error duplicating %"PRIuSIZE_T"-byte string.\n\"%s\"\nmalloc: ",
            strlen (string), string) ;
        return (NULL) ;
    }

    return (strcpy (duplicate, string)) ;

}
#endif	/* !HAVE_STRDUP */

#if defined(HAVE_STRLCAT) && !HAVE_STRLCAT
/*!*****************************************************************************

Procedure:

    strlcat ()

    Length-Limited String Concatenation.


Purpose:

    Function strlcat() appends a NUL-terminated string to another string.
    The resulting string is always NUL-terminated; if the length returned
    by strlcat() is equal to or greater than the buffer size, then the
    caller knows that the concatenated string was truncated.

    (See Todd C. Miller's and Theo de Raadt's paper, "strlcpy and strlcat -
    consistent, safe, string copy and concatenation".)


    Invocation:

        length = strlcat (destination, source, maximum) ;

    where:

        <destination>	- O
            is the NUL-terminated string to which the source string will be
            appended.
        <source>	- I
            is the NUL-terminated string to be appended to the destination
            string.
        <maximum>	- I
            is the maximum number of characters, including the NUL terminator,
            that the destination buffer can hold.
        <length>	- O
            returns the length of the concatenated string, not including the
            NUL terminator.

*******************************************************************************/


size_t  strlcat (

#    if PROTOTYPES
        char  *destination,
        const  char  *source,
        size_t  maximum)
#    else
        destination, source, maximum)

        char  *destination ;
        char  *source ;
        size_t  maximum ;
#    endif

{    /* Local variables. */
    char  *s ;
    size_t  length ;



    if ((destination == NULL) || (source == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strlcat) NULL source or destination strings: ") ;
        return (0) ;
    }

/* Locate the end of the destination string. */

    length = strlen (destination) ;
    if (length >= maximum)  return (maximum + strlen (source)) ;
    destination += length ;
    maximum -= length ;

/* Append the source string to the destination string. */

    s = (char *) source ;
    while (--maximum > 0) {
        if (*s == '\0')  break ;
        *destination++ = *s++ ;
    }

    *destination = '\0' ;

    if (maximum > 0)
        return (length + (s - source)) ;
    else
        return (length + (s - source) + strlen (s)) ;

}
#endif	/* !HAVE_STRLCAT */

#if defined(HAVE_STRLCPY) && !HAVE_STRLCPY
/*!*****************************************************************************

Procedure:

    strlcpy ()

    Length-Limited String Copy.


Purpose:

    Function strlcpy() copies a NUL-terminated string to a destination buffer
    of a specified size.  The copied string is always NUL-terminated; if the
    length returned by strlcpy() is equal to or greater than the buffer size,
    then the caller knows that the copied string was truncated.

    (See Todd C. Miller's and Theo de Raadt's paper, "strlcpy and strlcat -
    consistent, safe, string copy and concatenation".)


    Invocation:

        length = strlcpy (destination, source, maximum) ;

    where:

        <destination>	- O
            is the address of the buffer into which the string will be copied.
        <source>	- I
            is the NUL-terminated string to be copied.
        <maximum>	- I
            is the maximum number of characters, including the NUL terminator,
            that the destination buffer can hold.
        <length>	- O
            returns the length of the source string, not including the NUL
            terminator.

*******************************************************************************/


size_t  strlcpy (

#    if PROTOTYPES
        char  *destination,
        const  char  *source,
        size_t  maximum)
#    else
        destination, source, maximum)

        char  *destination ;
        char  *source ;
        size_t  maximum ;
#    endif

{    /* Local variables. */
    char  *s ;



    if ((destination == NULL) || (source == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strlcpy) NULL source or destination strings: ") ;
        return (0) ;
    }
    if (maximum < 1)  return (0) ;

    s = (char *) source ;
    while (--maximum > 0) {
        if (*s == '\0')  break ;
        *destination++ = *s++ ;
    }

    *destination = '\0' ;

    if (maximum > 0)
        return (s - source) ;
    else
        return (s - source + strlen (s)) ;

}
#endif	/* !HAVE_STRLCPY */

#if !defined(HAVE_STRLWR) || !HAVE_STRLWR
/*!*****************************************************************************

Procedure:

    strlwr ()

    Convert a NUL-Terminated String to Lower Case.


Purpose:

    Function strlwr() converts the characters in a string to lower case.
    This function is supported by many C libraries, but it is not part
    of the ANSI C library.


    Invocation:

        result = strlwr (string) ;

    where

        <string>	- I/O
            points to the string to be converted; the conversion is
            done in-place.
        <result>	- O
            returns a pointer to the converted string; i.e., STRING.

*******************************************************************************/


char  *strlwr (

#    if PROTOTYPES
        char  *string)
#    else
        string)

        char  *string ;
#    endif

{
    return (strnlwr (string, (string == NULL) ? 0 : strlen (string))) ;
}
#endif	/* !HAVE_STRLWR */

#if defined(HAVE_STRNCASECMP) && !HAVE_STRNCASECMP
/*!*****************************************************************************

Procedure:

    strncasecmp ()

    Compare Up to N Characters of Two Strings, Ignoring Case.


Purpose:

    Function strncasecmp() performs a length-limited comparison of two strings,
    ignoring the case of the individual characters.  strncasecmp() is supported
    by many C libraries, but it is not part of the ANSI C library.


    Invocation:

        comparison = strncasecmp (thisString, thatString, length) ;

    where:

        <thisString>	- I
        <thatString>	- I
            are the null- or length-terminated strings being compared.
        <length>	- I
            is the number of characters in each string to examine.
        <comparison>	- O
            returns one of three possible values:
                < 0 if THISSTRING is lexically less than THATSTRING,
                = 0 if the two strings are equal except for case, or
                > 0 if THISSTRING is lexically greater than THATSTRING.
            The less-than and greater-than relations are also case-insensitive.

*******************************************************************************/


int  strncasecmp (

#    if PROTOTYPES
        const  char  *thisString,
        const  char  *thatString,
        size_t  length)
#    else
        thisString, thatString, length)

        char  *thisString ;
        char  *thatString ;
        size_t  length ;
#    endif

{    /* Local variables. */
    char  *that, *this ;



/* Check for NULL strings. */

    if ((thisString == NULL) && (thatString != NULL))
        return (-1) ;
    else if ((thisString == NULL) && (thatString == NULL))
        return (0) ;
    else if ((thisString != NULL) && (thatString == NULL))
        return (1) ;

/* Compare the two strings, character by character. */

    this = (char *) thisString ;  that = (char *) thatString ;
    while ((*this != '\0') && (*that != '\0') && (length > 0)) {
        if (toupper (*this) < toupper (*that))
            return (-1) ;
        else if (toupper (*this) > toupper (*that))
            return (1) ;
        this++ ;  that++ ;  length-- ;
    }

/* The strings are identical (excepting case) for the first LENGTH characters
   or as far as the shorter string goes. */

    if (length == 0)
        return (0) ;		/* First LENGTH characters are equal. */
    else if (*this != '\0')
        return (-1) ;		/* THIS < THAT. */
    else if (*that != '\0')
        return (1) ;		/* THAT < THIS. */
    else
        return (0) ;		/* strlen() < LENGTH, but strings are equal. */

}
#endif	/* !HAVE_STRCASECMP */

#if !defined(HAVE_STRNCASESTR) || !HAVE_STRNCASESTR
/*!*****************************************************************************

Procedure:

    strncasestr ()

    Find a Substring in a Length-Limited String, Ignoring Case.


Purpose:

    Function strncasestr() searches a string looking for a substring, ignoring
    the case of the individual characters.  strncasestr() is supported by many
    C libraries, but it is not part of the ANSI C library.


    Invocation:

        result = strncasestr (string, substring, length) ;

    where:

        <string>	- I
            is the string being searched.
        <substring>	- I
            is the substring to be found in STRING.
        <length>	- I
            is the maximum number of characters to scan in STRING and also
            the maximum number of characters to be matched in SUBSTRING.
            If a NUL terminator is found in either string before LENGTH
            characters have been scanned or matched, the scan is stopped
            or the match is completed/skipped, respectively.
        <result>	- O
            returns a pointer to the start of the substring in STRING, if the
            substring is found, and NULL otherwise.  For historical reasons,
            I guess, the function's return value is "char *" despite the input
            arguments being "const char *".

*******************************************************************************/


char  *strncasestr (

#    if PROTOTYPES
        const  char  *string,
        const  char  *substring,
        size_t  length)
#    else
        string, substring, length)

        char  *string ;
        char  *substring ;
        size_t  length ;
#    endif

{    /* Local variables. */
    const  char  *s, *stop ;
    size_t  sublength ;



    if (string == NULL) {
        errno = EINVAL ;
        LGE "(strncasestr) NULL string to search: ") ;
        return (NULL) ;
    } else if (substring == NULL) {
        errno = EINVAL ;
        LGE "(strncasestr) NULL substring: ") ;
        return (NULL) ;
    }

    sublength = strlen (substring) ;
    if (sublength == 0) {
        errno = EINVAL ;
        LGE "(strncasestr) Zero-length substring: ") ;
        return ((char *) string) ;	/* Empty search string allowed. */
    }

/* If the substring you're looking for is longer than the string you're
   looking in, then return "not found" (NULL). */

    if (sublength > length)  return (NULL) ;

/* Step character-by-character through the string being searched, performing
   a case-insensitive comparison with the substring beginning at the location
   of each character.  Stop scanning when the remainder of the string being
   searched is shorter than the substring. */

    stop = string + length - sublength + 1 ;

    for (s = string ;  s != stop ;  s++) {
        if (strncasecmp (s, substring, sublength) == 0)  break ;
    }

    return ((s == stop) ? NULL : (char *) s) ;

}
#endif	/* !HAVE_STRNCASESTR */

/*!*****************************************************************************

Procedure:

    strnchr ()

    Find the First Occurrence of a Character in a Length-Limited String.


Purpose:

    Function strnchr() locates the first occurrence of a character in a
    length-limited string.  This function is not part of the ANSI C library.


    Invocation:

        occurrence = strnchr (string, c, length) ;

    where:

        <string>	- I
            is the string to be searched.
        <c>		- I
            is the character to be located.
        <length>	- I
            is the maximum number of characters in the string to examine;
            a NUL character in the string terminates the search.
        <occurrence>	- O
            returns a pointer to the first occurrence of the character in
            the string; NULL is returned if the character is not found.

*******************************************************************************/


char  *strnchr (

#    if PROTOTYPES
        const  char  *string,
        int  c,
        size_t  length)
#    else
        string, c, length)

        char  *string ;
        int  c ;
        size_t  length ;
#    endif

{    /* Local variables. */
    char  *occurrence ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strnchr) NULL string: ") ;
        return (NULL) ;
    }

    occurrence = (char *) string ;
    while (length-- > 0) {
        if (*occurrence == c)  return (occurrence) ;
        if (*occurrence++ == '\0')  return (NULL) ;
    }

    return (NULL) ;

}

#if defined(HAVE_STRNDUP) && !HAVE_STRNDUP
/*!*****************************************************************************

Procedure:

    strndup ()

    Duplicate a String of a Specified Length.


Purpose:

    Function strndup() duplicates a string of a specified length.  strndup()
    is the "n" counterpart of strdup(), as in strncmp(3) and strcmp(3).


    Invocation:

        duplicate = strndup (string, length) ;

    where:

        <string>	- I
            is the string to be duplicated.
        <length>	- I
            is the number of characters to be duplicated.
        <duplicate>	- O
            returns a MALLOC(3)ed copy of the input string; the duplicate
            is NUL-terminated.  The caller is responsible for FREE(3)ing
            the duplicate string.  NULL is returned in the event of an error.

*******************************************************************************/


char  *strndup (

#    if PROTOTYPES
        const  char  *string,
        size_t  length)
#    else
        string, length)

        char  *string ;
        size_t  length ;
#    endif

{    /* Local variables. */
    char  *duplicate ;



    if (string == NULL)  string = "" ;

    length = strnlen (string, length) ;

    duplicate = malloc (length + 1) ;
    if (duplicate == NULL) {
        LGE "(strndup) Error duplicating %"PRIuSIZE_T"-character string.\n\"%.*s\"\nmalloc: ",
            length, (int) length, string) ;
        return (NULL) ;
    }

    strncpy (duplicate, string, length) ;
    duplicate[length] = '\0' ;

    return (duplicate) ;

}
#endif	/* !HAVE_STRNDUP */

#if defined(HAVE_STRNLEN) && !HAVE_STRNLEN
/*!*****************************************************************************

Procedure:

    strnlen ()

    Determine the Length of a Fixed-Size String.


Purpose:

    Function strnlen() determines the length of a string in a fixed-size
    buffer.  If the string is NUL-terminated within the buffer, strnlen()
    returns the NUL-terminated length of the string, just like strlen()
    would do.  If the string is not NUL-terminated within the buffer,
    strnlen() returns the size of the buffer as the string length.

    For example, an old ID3v1.1 music tag contains fixed-length text fields
    for the song title, the artist name, etc.  The structure for the tag
    would look as follows:

        typedef  struct  ID3v1_1Tag {
            char  song[30] ;
            char  artist[30] ;
            char  album[30] ;
            char  year[4] ;
            char  comment[28] ;
            unsigned  char  track ;
            unsigned  char  genre ;
        }  ID3v1_1Tag ;

    The song field in the ID3v1.1 tag for the Beatles' "Help!" would have
    the 5 characters of the song title followed by 25 NULs.  Therefore,

        strnlen (tag->song, sizeof tag->song)

    would return a length of 5.  In the tag for the Beatles' "Sgt. Pepper's
    Lonely Hearts Club Band", the title would have to be truncated to 30
    characters, so the song field would contain 30 characters:

        "Sgt. Pepper's Lonely Hearts Cl"

    with no NUL terminator and the same call to strnlen() would return a
    length of 30.


    Invocation:

        length = strnlen (string, maxLength) ;

    where:

        <string>	- I
            is the string whose length is to be determined.
        <maxLength>	- I
            is the maximum number of characters in the string to examine
            while looking for a NUL terminator.
        <length>	- O
            returns the number of characters preceding the NUL terminator if
            the NUL is found within MAXLENGTH characters; i.e., the length
            of the NUL-terminated string is less than or equal to MAXLENGTH-1
            characters.  Otherwise, MAXLENGTH is returned.

*******************************************************************************/


size_t  strnlen (

#    if PROTOTYPES
        const  char  *string,
        size_t  maxLength)
#    else
        string, maxLength)

        char  *string ;
        size_t  maxLength ;
#    endif

{    /* Local variables. */
    const  char  *s = string ;



    if (s == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strnlen) NULL string: ") ;
        return (0) ;
    }

    while ((*s != 0) && (maxLength-- > 0))
        s++ ;

    return (s - string) ;

}
#endif	/* !HAVE_STRNLEN */

#if !defined(HAVE_STRNLWR) || !HAVE_STRNLWR
/*!*****************************************************************************

Procedure:

    strnlwr ()

    Convert a Length-Limited String to Lower Case.


Purpose:

    Function strnlwr() converts the characters in a length-limited string
    to lower case.


    Invocation:

        result = strnlwr (string, length) ;

    where

        <string>	- I/O
            points to the string to be converted; the conversion is
            done in-place.
        <length>	- I
            is the number of characters to be converted.
        <result>	- O
            returns a pointer to the converted string; i.e., STRING.

*******************************************************************************/


char  *strnlwr (

#    if PROTOTYPES
        char  *string,
        size_t  length)
#    else
        string, length)

        char  *string ;
        size_t  length ;
#    endif

{    /* Local variables. */
    unsigned  char  *s ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strnlwr) NULL string: ") ;
        return (NULL) ;
    }

    for (s = (unsigned char *) string ;  length-- > 0 ;  s++)
        if (isupper (*s))  *s = tolower (*s) ;

    return (string) ;

}
#endif	/* !HAVE_STRNLWR */

#if !defined(HAVE_STRNSTR) || !HAVE_STRNSTR
/*!*****************************************************************************

Procedure:

    strnstr ()

    Find a Substring in a Length-Limited String.


Purpose:

    Function strnstr() searches a string looking for a substring; strnstr() is
    supported by many C libraries, but it is not part of the ANSI C library.


    Invocation:

        result = strnstr (string, substring, length) ;

    where:

        <string>	- I
            is the string being searched.
        <substring>	- I
            is the substring to be found in STRING.
        <length>	- I
            is the maximum number of characters to scan in STRING and also
            the maximum number of characters to be matched in SUBSTRING.
            If a NUL terminator is found in either string before LENGTH
            characters have been scanned or matched, the scan is stopped
            or the match is completed/skipped, respectively.
        <result>	- O
            returns a pointer to the start of the substring in STRING, if the
            substring is found, and NULL otherwise.  For historical reasons,
            I guess, the function's return value is "char *" despite the input
            arguments being "const char *".

*******************************************************************************/


char  *strnstr (

#    if PROTOTYPES
        const  char  *string,
        const  char  *substring,
        size_t  length)
#    else
        string, substring, length)

        char  *string ;
        char  *substring ;
        size_t  length ;
#    endif

{    /* Local variables. */
    const  char  *s, *stop ;
    size_t  sublength ;



    if (string == NULL) {
        errno = EINVAL ;
        LGE "(strnstr) NULL string to search: ") ;
        return (NULL) ;
    } else if (substring == NULL) {
        errno = EINVAL ;
        LGE "(strnstr) NULL substring: ") ;
        return (NULL) ;
    }

    sublength = strlen (substring) ;
    if (sublength == 0) {
        errno = EINVAL ;
        LGE "(strnstr) Zero-length substring: ") ;
        return ((char *) string) ;	/* Empty search string allowed. */
    }

/* If the substring you're looking for is longer than the string you're
   looking in, then return "not found" (NULL). */

    if (sublength > length)  return (NULL) ;

/* Step character-by-character through the string being searched, performing
   a comparison with the substring beginning at the location of each character.
   Stop scanning when the remainder of the string being searched is shorter
   than the substring. */

    stop = string + length - sublength + 1 ;

    for (s = string ;  s != stop ;  s++) {
        if (strncmp (s, substring, sublength) == 0)  break ;
    }

    return ((s == stop) ? NULL : (char *) s) ;

}
#endif	/* !HAVE_STRNSTR */

#if !defined(HAVE_STRNUPR) || !HAVE_STRNUPR
/*!*****************************************************************************

Procedure:

    strnupr ()

    Convert a Length-Limited String to Lower Case.


Purpose:

    Function strnupr() converts the characters in a length-limited string
    to lower case.


    Invocation:

        result = strnupr (string, length) ;

    where

        <string>	- I/O
            points to the string to be converted; the conversion is
            done in-place.
        <length>	- I
            is the number of characters to be converted.
        <result>	- O
            returns a pointer to the converted string; i.e., STRING.

*******************************************************************************/


char  *strnupr (

#    if PROTOTYPES
        char  *string,
        size_t  length)
#    else
        string, length)

        char  *string ;
        size_t  length ;
#    endif

{    /* Local variables. */
    unsigned  char  *s ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strnupr) NULL string: ") ;
        return (NULL) ;
    }

    for (s = (unsigned char *) string ;  length-- > 0 ;  s++)
        if (islower (*s))  *s = toupper (*s) ;

    return (string) ;

}
#endif	/* !HAVE_STRNUPR */

#if defined(HAVE_STRRCHR) && !HAVE_STRRCHR
/*!*****************************************************************************

Procedure:

    strrchr ()

    Find the Last Occurrence of a Character in a NUL-Terminated String.


Purpose:

    Function strrchr() locates the last occurrence of a character in a
    NUL-terminated string.  Despite being part of the ANSI C library,
    strrchr() is not always available.


    Invocation:

        occurrence = strrchr (string, c) ;

    where:

        <string>	- I
            is the string to be searched.
        <c>		- I
            is the character to be located.
        <occurrence>	- O
            returns a pointer to the last occurrence of the character in the
            string; NULL is returned if the character is not found.

*******************************************************************************/


char  *strrchr (

#    if PROTOTYPES
        const  char  *string,
        int  c)
#    else
        string, c)

        char  *string ;
        int  c ;
#    endif

{    /* Local variables. */
    char  *occurrence ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(strrchr) NULL string: ") ;
        return (NULL) ;
    }

    occurrence = (char *) string + strlen (string) ;
    while (occurrence != string) {
        if (*--occurrence == c)  return (occurrence) ;
    }

    return (NULL) ;

}
#endif	/* !HAVE_STRRCHR */

#if defined(HAVE_STRSPN) && !HAVE_STRSPN
/*!*****************************************************************************

Procedure:

    strspn ()

    Skip Characters in a Set of Characters.


Purpose:

    Function strspn() spans the initial segment of a string such that the
    characters in the initial segment are all from a caller-specified set
    of characters.  Despite being part of the ANSI C library, strspn() is
    not always available.


    Invocation:

        length = strspn (string, accept) ;

    where:

        <string>	- I
            is the string to be scanned.
        <accept>	- I
            is a string containing the characters allowed in the initial
            segment.
        <length>	- O
            returns the length of the initial segment containing only characters
            from the caller-specified set.

*******************************************************************************/


size_t  strspn (

#    if PROTOTYPES
        const  char  *string,
        const  char  *accept)
#    else
        string, accept)

        char  *string ;
        char  *accept ;
#    endif

{    /* Local variables. */
    char  *s, *t ;



    if ((string == NULL) || (accept == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(strspn) NULL string or accept set: ") ;
        return (0) ;
    }

    for (s = (char *) string ;  *s != '\0' ;  s++) {
        for (t = (char *) accept ;  *t != '\0' ;  t++) {
            if (*t == *s)  break ;		/* Character in set? */
        }
        if (*t != *s)  break ;			/* Character not in set? */
    }

    return (s - string) ;

}
#endif	/* !HAVE_STRSPN */

#if defined(HAVE_STRTOK) && !HAVE_STRTOK
/*!*****************************************************************************

Procedure:

    strtok ()

    Extract Tokens from a String.


Purpose:

    Function strtok() parses a string into tokens separated by a
    caller-specified set of delimiters.  Successive calls to strtok()
    return the tokens in order, one after another.  Despite being
    part of the ANSI C library, strtok() is not always available.


    Invocation:

        token = strtok (string, delimiters) ;

    where:

        <string>	- I
            is the string to be parsed.  The string must be writeable since
            strtok() NUL-terminates the next token being returned.  The string
            should only be specified on the first call to strtok() to parse
            the string; subsequent calls should pass in NULL so that strtok()
            knows to use its previously stored internal state.
        <delimiters>	- I
            is a string containing the characters allowed as delimiters between
            tokens.  The caller may supply different sets of delimiters as the
            scan of the string progresses.
        <token>		- O
            returns a pointer to the beginning of the next token in the string.
            The token is NUL-terminated, either by the end of the original
            string or by strtok() replacing the following delimiter by NUL.
            A NULL pointer is returned when the end of the string is reached.

*******************************************************************************/


char  *strtok (

#    if PROTOTYPES
        char  *string,
        const  char  *delimiters)
#    else
        string, delimiters)

        char  *string ;
        char  *delimiters ;
#    endif

{    /* Local variables. */
    static  char  *next = NULL ;	/* Where to begin scan on next call. */



/* Get the next token from the string and save a pointer to the location
   at which to begin the next scan. */

    if (string != NULL)  next = NULL ;	/* Beginning of scan? */

    return (strtok_r (string, delimiters, &next)) ;

}
#endif	/* !HAVE_STRTOK */

#if defined(HAVE_STRTOK_R) && !HAVE_STRTOK_R
/*!*****************************************************************************

Procedure:

    strtok_r ()

    Extract Tokens from a String (Reentrant).


Purpose:

    Function strtok_r() is a reentrant version of strtok().  On the first
    call to strtok_r(), the scan for the first token begins at the start of
    the string.  Leading delimiters are skipped and the first trailing
    delimiter is located and overwritten with a NUL terminator.  A pointer
    to the character following that NUL terminator is returned in the CONTEXT
    argument.  Subsequent calls to strtok_r() with a NULL string begin scanning
    at the address in the CONTEXT argument.

    The following example reads a string from standard input and prints out
    each token in the string, where the tokens are separated by spaces or tabs:

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  <stdlib.h>		-- Standard library definitions.
        #include  "str_util.h"		-- String manipulation functions.

        int  main (int argc, char *argv[])
        {
            char  buffer[256], *context, *token ;

            fgets (buffer, sizeof buffer, stdin) ;
            token = strtok_r (buffer, " \t", &context) ;    -- Get first token.
            while (token != NULL) {
                printf ("Token = \"%s\"\n", token) ;
                token = strtok_r (NULL, " \t", &context) ;  -- Get next token.
            }
            exit (0) ;
        }


    Invocation:

        token = strtok_r (string, delimiters, &context) ;

    where:

        <string>	- I
            is the string to be scanned.  On the first call to strtok_r(),
            this argument should be the beginning of the string; on subsequent
            calls for the same string, this argument should be NULL.
        <delimiters>	- I
            is a string containing the set of characters considered delimiters.
            Different sets of delimiters can be specified on each call to
            strtok_r() as you progress through a string.
        <context>	- I/O
            is the context for a scan and is used by strtok_r() to determine
            where in the string to begin scanning for the next token.  Using
            different contexts for different strings makes the function
            reentrant and also allows simultaneous scans of different strings.
            On the first call to strtok_r() for a string, the input value of
            this argument is ignored, but the argument returns a pointer to
            the context for the scan.  On subsequent calls to strtok_r(), the
            function uses the input value of this argument to determine where
            to begin the scan for the next token; the argument returns an
            updated context pointer.  (The context is just a pointer to the
            character following the NUL terminator for the previous token.)
        <token>		- O
            returns a pointer to the beginning of the next token in the string;
            the token is NUL-terminated.  A NULL pointer is returned in the
            event of an error or when the the end of the complete string is
            reached; i.e., there are no more tokens.

*******************************************************************************/


char  *strtok_r (

#    if PROTOTYPES
        char  *string,
        const  char  *delimiters,
        char  **context)
#    else
        string, delimiters, context)

        char  *string ;
        char  *delimiters ;
        char  **context ;

#    endif

{    /* Local variables. */
    char  *s, *token ;



/* On all calls to strtok_r(), the function returns a value in the context
   argument, so check that a non-NULL address was specified for the context
   variable.  On the second and subsequent calls, the input value of the
   context variable is used to determine where to begin scanning, so check
   that the context variable contains a non-NULL value. */

    if (context == NULL) {
        SET_ERRNO (EINVAL) ;		/* &context is NULL */
        LGE "(strtok_r) NULL context address: ") ;
        return (NULL) ;
    } else if ((string == NULL) && (*context == NULL)) {
        SET_ERRNO (EINVAL) ;		/* *context is NULL */
        LGE "(strtok_r) NULL context value: ") ;
        return (NULL) ;
    }

/* On the 2nd through Nth calls to strtok_r(), STRING is NULL and CONTEXT
   points to where the scan for the next token should start.  So, set STRING
   to CONTEXT as if this is a first call and begin scanning. */

    if (string == NULL)  string = *context ;

/* Skip leading delimiters to the beginning of the next token. */

    token = string + strspn ((const char *) string, delimiters) ;

/* Skip to the first trailing delimiter after the end of the token. */

    s = token + strcspn ((const char *) token, delimiters) ;

/* Mark the end of the token with a NUL character. */

    if (*s == '\0') {		/* Reached end of string? */
        *context = s ;		/* Return pointer to NUL character. */
        return (NULL) ;		/* No token. */
    } else {			/* In the middle of the string? */
        *s = '\0' ;		/* Overwrite trailing delimiter with NUL. */
        *context = ++s ;	/* Return pointer to character following NUL. */
        return (token) ;	/* Return pointer to beginning of token. */
    }

}
#endif	/* !HAVE_STRTOK_R */

#if !defined(HAVE_STRUPR) || !HAVE_STRUPR
/*!*****************************************************************************

Procedure:

    strupr ()

    Convert a NUL-Terminated String to Upper Case.


Purpose:

    Function strupr() converts the characters in a string to upper case.
    This function is supported by many C libraries, but it is not part
    of the ANSI C library.


    Invocation:

        result = strupr (string) ;

    where

        <string>	- I/O
            points to the string to be converted; the conversion is
            done in-place.
        <result>	- O
            returns a pointer to the converted string; i.e., STRING.

*******************************************************************************/


char  *strupr (

#    if PROTOTYPES
        char  *string)
#    else
        string)

        char  *string ;
#    endif

{
    return (strnupr (string, (string == NULL) ? 0 : strlen (string))) ;
}
#endif	/* !HAVE_STRUPR */

#if !defined(HAVE_STRXLT) || !HAVE_STRXLT
/*!*****************************************************************************

Procedure:

    strxlt ()

    Translate a String.


Purpose:

    Function strxlt() translates the characters in a string using a table
    that maps characters to their translation.  The table is a 256-character
    array; each character in the string is treated as a 0-255 index into the
    table and is replaced by the indexed entry in the table.

    The xltable() function provides an easy way to construct translation
    tables.  The function prologue for xltable() includes an example of
    building a table for mapping 'a' to 'd', 'b' to 'e', and 'c' to 'f'.
    Calling strxlt() with that table and this input string:

                            "always be concise"

    will produce a translated string:

                            "dlwdys ee fonfise"

    I had a need for translating strings.  I remembered the VAX MOVTC
    instruction from long ago and I long thought the C Library had a
    similar string fuction, but perhaps I was confused by strxfrm()'s
    name.  In any case, a web search turned by strxlt(), memxlt(), and
    xltable() - functions provided by the SAS/C Compiler and Library
    for IBM mainframe computers.  I copied the function signatures and
    I hope my implementations work the same as the SAS functions.  (I've
    never used SAS/C or an IBM mainframe computer.)  See "SAS/C Compiler
    and Library" at
    https://support.sas.com/en/software/sas-c-support.html
    and the strxlt() function description at
    https://support.sas.com/documentation/onlinedoc/sasc/doc750/html/lr1/z2044596.htm.


    Invocation:

        string = strxlt (string, table) ;

    where:

        <string>	- I/O
            is the string to be translated.  The translation is performed
            in-place.
        <table>		- I
            is the translation table that maps input characters to their
            replacement characters.
        <string>	- O
            returns the input STRING argument.

*******************************************************************************/


char  *strxlt (

#    if PROTOTYPES
        char  *string,
        const  char  *table)
#    else
        string, table)

        char  *string ;
        char  *table ;
#    endif

{    /* Local variables. */
    char  *s ;
    unsigned  int  i ;



    if (table == NULL)  return (string) ;	/* No translation? */
    if (string == NULL)  return (NULL) ;

/* Replace each character in the string with its mapping from the table. */

    for (s = string ; *s != '\0' ;  s++) {
        i = *((unsigned char *) s) ;
        *s = table[i] ;
    }

    return (string) ;

}
#endif	/* !HAVE_STRXLT */

#if !defined(HAVE_XLTABLE) || !HAVE_XLTABLE
/*!*****************************************************************************

Procedure:

    xltable ()

    Build a Translation Table.


Purpose:

    Function xltable() builds a table that maps characters to other
    characters.  The table is a 256-character array used by the
    strxlt() and memxlt() functions to translate the characters
    in a string/block.  Each character is treated as a 0-255 index
    into the table and is replaced by the indexed entry in the table.

    The characters in the FROM and TO string correspond one-to-one.
    For example, if FROM is "abc" and TO is "def", then a call to
    strxlt() for an arbitrary string will translate any 'a' to 'd'
    in the string, 'b' to 'e', and 'c' to 'f'.  The table is initially
    filled so that every character 0-255 maps to itself; then the
    entries for 'a', 'b', and 'c' are filled with 'd', 'e', and 'f'.
    respectively.

    If the FROM and TO strings differ in length, only the shorter number
    of characters are included in the table and no error indication is
    returned to the caller of xltable().

    Function xltable() is my own implementation of the xltable() function
    provided by the SAS/C Compiler and Library for IBM mainframe computers.
    I've never used SAS/C or an IBM mainframe computer.
    (https://support.sas.com/documentation/onlinedoc/ccompiler/doc750/html/lr1/z2057066.htm)


        NOTE: If the caller doesn't supply memory for the table, the table
        is built in memory local to xltable().  The table should be used
        or duplicated before calling xltable() again such that the local
        table is reused.


    Invocation:

        table = xltable (table, from, to) ;

    where:

        <table>	- O
            is the address of a caller-supplied, 256-byte buffer in which
            the table will be built.  If this argument is NULL, the table
            will be built in a static buffer local to xltable().
        <from>	- I
            is a NUL-terminated string containing the characters to be
            translated.
        <to>	- I
            is a NUL-terminated string containing the translation of the
            characters in the FROM string.
        <table>	- O
            returns the address of the constructed table.  If the caller
            supplied a buffer, the return value is that buffer.  If NULL
            was specified, the table is built in memory local to xltable()
            and its address is returned.

*******************************************************************************/


char  *xltable (

#    if PROTOTYPES
        char  *table,
        const  char  *from,
        const  char  *to)
#    else
        table, from, to)

        char  *table ;
        char  *from ;
        char  *to ;
#    endif

{    /* Local variables. */
    const  char  *s, *t ;
    unsigned  int  i ;
    static  char  myTable[256] ;



    if (table == NULL)  table = myTable ;	/* Use the internal buffer. */

/* First, initialize the table to directly translate all characters. */

    s = table ;
    for (i = 0 ;  i < 256 ;  i++) {
        *((unsigned char *) s++) = (unsigned char) i ;
    }

/* Next, set the individual characters to be translated. */

    s = (from == NULL) ? "" : from ;
    t = (to == NULL) ? "" : to ;

    while ((*s != '\0') && (*t != '\0')) {
        i = *((unsigned char *) s++) ;
        table[i] = *t++ ;
    }

    return (table) ;

}
#endif	/* !HAVE_XLTABLE */

#if !defined(HAVE_XLTABLE) || !HAVE_XLTABLE
/*!*****************************************************************************

Procedure:

    xltable1 ()

    Build a Translation Table (Single From/To String).


Purpose:

    Function xltable1() builds a table that maps characters to other
    characters.  The table is a 256-character array used by the
    strxlt() and memxlt() functions to translate the characters
    in a string/block.  Each character is treated as a 0-255 index
    into the table and is replaced by the indexed entry in the table.

    The function is similar to xltable(), but whereas xltable() has
    separate "from" and "to" strings specifying the translation,
    xltable1() has a single from/to string in which each pair of
    characters is a from/to mapping.

    Using the example in the xltable() function prologue, we will build
    a table to translate 'a' to 'd', 'b' to 'e', and 'c' to 'f'.  This
    table can be built with separate "from" and "to" strings:

        xltable (table, "abc", "def") ;

    or with a single from/to string:

        xltable1 (table, "adbecf") ;

    As can be seen, the from/to string should consist of pairs of characters.
    If there are an odd number of characters, the trailing lone character is
    ignored.

    This function invokes xltable() to initialize the translation table and
    so, if the caller did not supply a buffer for the table, xltable()'s
    private storage will be used.

    Function xltable1() is an alternate form of the xltable() function
    provided by the SAS/C Compiler and Library for IBM mainframe computers.
    I've never used SAS/C or an IBM mainframe computer.
    (https://support.sas.com/documentation/onlinedoc/ccompiler/doc750/html/lr1/z2057066.htm)


    Invocation:

        table = xltable1 (table, fromto) ;

    where:

        <table>		- O
            is the address of a caller-supplied, 256-byte buffer in which
            the table will be built.  If this argument is NULL, the table
            will be built in a static buffer local to xltable(), not this
            function, xltable1().
        <fromto>	- I
            is a NUL-terminated string containing pairs of characters
            representing the translations.  The first character in a pair
            is the character being translated and the second character is
            the replacement character.
        <to>	- I
            is a NUL-terminated string containing the translation of the
            characters in the FROM string.
        <table>		- O
            returns the address of the constructed table.  If the caller
            supplied a buffer, the return value is that buffer.  If NULL
            was specified, the table is built in memory local to xltable(),
            not xltable1(), and its address is returned.

*******************************************************************************/


char  *xltable1 (

#    if PROTOTYPES
        char  *table,
        const  char  *fromto)
#    else
        table, fromto)

        char  *table ;
        char  *fromto ;
#    endif

{    /* Local variables. */
    const  char  *s ;
    unsigned  int  i ;



/* First, call xltable() to initialize the table so that it directly
   translates all characters.  If the caller didn't supply a table,
   then xltable()'s privately stored table will be used. */

    table = xltable (table, NULL, NULL) ;

/* Next, set the individual characters to be translated. */

    s = (fromto == NULL) ? "" : fromto ;

    while (*s != '\0') {
        i = *((unsigned char *) s++) ;
        if (*s == '\0')  break ;
        table[i] = *s++ ;
    }

    return (table) ;

}
#endif	/* !HAVE_XLTABLE */

#ifdef TEST
/*******************************************************************************

    TEST PROGRAM

    Example compilation and run commands:

        % gcc -g -Wall -pedantic -Wno-long-long -DTEST
              -I. -I../include str_util.c libgpl.a -o teststr
        % teststr
        ... output ...
        %

*******************************************************************************/


int  main (

    int  argc,
    char  *argv[])

{    /* Local variables. */
#define  MAXB  1024
    char  buffer[MAXB], *find, *replace, *s ;
    errno_t  status ;
    size_t  numReplace ;



    aperror_print = 1 ;		/* Print error messages. */
    str_util_debug = 0 ;	/* Set to 1 to display debug messages. */

/* Test strReplace(). */

/*******************************************************************************
         1         2         3         4         5         6         7         8         9         0         1         2         3
123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012
A B C D E F 1234567890==================
*******************************************************************************/

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = " " ;  replace = ""  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = " " ;  replace = ""  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, 3, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "D E " ;  replace = ""  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "D E " ;  replace = "D E "  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "d e " ;  replace = ""  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "d e " ;  replace = ""  ;
    status = strReplace (buffer, MAXB, false, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "D E " ;  replace = "Hello, World!"  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "D E " ;  replace = "Hello, World!"  ;
    status = strReplace (buffer, -1, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "D E " ;  replace = ""  ;
    status = strReplace (buffer, MAXB, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "=" ;  replace = ""  ;
    status = strReplace (buffer, -1, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "=" ;  replace = ""  ;
    status = strReplace (buffer, strlen (buffer) + 1, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "A B C D E F 1234567890==================") ;
    find = "=" ;  replace = "+|" ;
    status = strReplace (buffer, strlen (buffer) + 10, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "To sleep or not to sleep") ;
    find = "sleep" ;  replace = "be" ;
    status = strReplace (buffer, -1, true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "To sleep or not to sleep") ;
    find = "sleep" ;  replace = "be" ;
    status = strReplace (buffer, strlen (buffer), true, find, -1, replace, -1, -1, &numReplace) ;
    printf ("(%d) %lu replacements of \"%s\" with \"%s\", \"%s\"\n",
            status, numReplace, find, replace, buffer) ;
    fflush (stdout) ;

/* Testing strDesmart(). */

#define  CURLY_QUOTE_TEXT	\
        "Curly quotes are the quo\xAD""ta\xAD"				\
        "tion marks used in good typogra\xAD"				\
        "phy.  There are four curly quote char\xAD""ac\xAD"		\
        "ters \x97 the open\xAD""ing single quote \x91, the clos\xAD"	\
        "ing sin\xAD""gle quote \x92, the opening dou\xAD"		\
        "ble quote \x93, and the closing dou\xAD"			\
        "ble quote \x94.  And ellipses \x85  And so on \x85"		\
        "  And so on \x85  \xA9 \x97 yes \xA9 \x97 by me!"

    strcpy (buffer, CURLY_QUOTE_TEXT) ;
    printf ("Smart text = (%u) \"%s\"\n", (unsigned) strlen (buffer), buffer) ;
    status = strDesmart (buffer, -1, MAXB) ;
    printf ("(%d) Fixed text = (%u) \"%s\"\n",
            status, (unsigned) strlen (buffer), buffer) ;
    fflush (stdout) ;

    strcpy (buffer, CURLY_QUOTE_TEXT) ;
    status = strDesmart (buffer, -1, strlen (buffer) + 5) ;
    printf ("(%d) Fixed text = (%u) \"%s\"\n",
            status, (unsigned) strlen (buffer), buffer) ;
    fflush (stdout) ;

    strcpy (buffer, "To be or not to be") ;
    status = strDesmart (buffer, -1, MAXB) ;
    printf ("(%d) Fixed text = (%u) \"%s\"\n",
            status, (unsigned) strlen (buffer), buffer) ;
    fflush (stdout) ;

/* Testing strDupCat(). */

    s = strDupCat (0) ;
    printf ("[strDupCat] Counted (0) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (3, "1", "2", "3") ;
    printf ("[strDupCat] Counted (3) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (6, "1", "2", "3", "4", "5", "6") ;
    printf ("[strDupCat] Counted (6) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (6, "1", "2", "3", "4", NULL, "6") ;
    printf ("[strDupCat] Counted (6,no 5) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (7, "1", "2", "3", "4", "5", "6", "7") ;
    printf ("[strDupCat] Counted (7) text catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (-1, NULL) ;
    printf ("[strDupCat] NULL (0) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (-1, "1", NULL) ;
    printf ("[strDupCat] NULL (1) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (-1, "1", "2", "3", "4", "5", "6", NULL) ;
    printf ("[strDupCat] NULL (6) list catenated: \"%s\"\n",  (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    s = strDupCat (-1, "1", "2", "3", "4", "5", "6", "7", NULL) ;
    printf ("[strDupCat] NULL (7) list catenated: \"%s\"\n", (s == NULL) ? "<null>" : s) ;
    fflush (stdout) ;

    exit (0) ;

}
#endif	/* TEST */
