/* $Id: sdx_util.c,v 1.5 2022/11/28 18:15:26 alex Exp alex $ */
/*******************************************************************************

File:

    sdx_util.c

    String Descriptor (SDX) Utilities


Author:    Alex Measday


Purpose:

    The SDX_UTIL package provides functions for creating and manipulating
    dynamically sized and dynamically changing strings.

    I was trying to clean up rexReplace(), which was going to great lengths
    to juggle the result string while making substitutions in an input string
    matched by a regular expression.  To help me, I wrote this package; it
    was inspired by VAX/VMS descriptors which consisted of (i) 16 bits of
    flags including the data type of the object being described, (ii) the
    16-bit length of the object, and (iii) the 32-bit address of the object.

    I was aware of other libraries and, of course, C++, that support this
    functionality, but I don't like C++ and this simple package provided
    me with exactly what I wanted.

    Building and using a string is easy:

        #include  "sdx_util.h"		-- String Descriptor utilities.
        StringDx  rdx ;
        ...
        sdxCreate ("I don't know why you say Goodbye", -1, SdxVolatile, &rdx) ;
        sdxAddC (rdx, ',') ;
        sdxAddC (rdx, ' ') ;
        sdxAddS (rdx, "I say Hello!", -1) ;
        printf ("Complete string: \"%s\"\n",
                sdxStringZ (rdx)) ;
        sdxDestroy (rdx) ;

    The length argument, -1, in the calls to sdxCreate() and sdxAddS()
    specifies that the input string is NUL-terminated; these functions
    will call strlen(3) to determine the length.  An actual length can be
    specified when adding a substring of a larger, SDX or non-SDX string.

    If you're making repeated passes over something that requires a
    descriptor to be reset at the beginning of each pass, sdxErase()
    can be called to erase the contents of the descriptor without
    destroying the descriptor; if dynamically allocated, the existing,
    described string will be freed.

    If you need a dynamically allocated string to survive the destruction
    of its descriptor, take ownership of the string:

        char  *result ;
        ...
        sdxOwn (rdx) ;
        result = sdxStringZ (rdx) ;
        sdxDestroy (rdx) ;
        ...
        free (result) ;		-- Don't forget to free the string.

    If the caller owns the string, sdxDestroy() will not free the string.
    Note that, as a string grows, sdxAddC() and sdxAddS() may need to
    reallocate the buffer used to store the string.  Consequently, a
    program should be sure to call sdxStringZ() right before destroying
    the descriptor in order to get the final address of the string.

        NOTE: The SDX_UTIL functions can handle arbitrary-length
        strings with embedded NUL characters (zero bytes), but
        without a trailing NUL character.  Instead of the C
        Library string functions, the SDX_UTIL functions use the
        memory functions: memcpy(3), memset(3), and memdup().

        If the string length argument to an SDX_UTIL function is -1,
        the input string is assumed to be NUL-terminated and its
        length is determined using strlen(3).

        The calling application is responsible for keeping track of
        and correctly handling NUL- and non-NUL-terminated strings.
        There are two special cases regarding the string returned
        by sdxStringZ() that the caller should be aware of.  If
        the string passed to sdxCreate() was (i) SdxStatic or
        (ii) SdxDynamic (pre-allocated by the caller) and a length
        >= 0 was specified, the SDX_UTIL functions cannot tell if
        the input string is NUL-terminated; examining the character
        at length+1 is not permitted.  Consequently, despite its
        name, sdxStringZ() may return a non-NUL-terminated string
        in these cases.  The caller, again, is responsible for
        knowing what is what and not trying to print out a
        non-NUL-terminated string with an unadorned "%s" format.

        There's no hope for static strings.  In the latter case of
        a pre-allocated, dynamic string being passed to sdxCreate(),
        the first time text is added to the string via sdxAddC() or
        sdxAddS(), a trailing NUL terminator will be automatically
        added to the expanded string (even if the string already
        contains embedded NUL characters).


Public Procedures:

    sdxAddC() - adds a character to a described string.
    sdxAddS() - adds text to a described string.
    sdxCopy() - copy the contents from one described string to another.
    sdxCreate() - creates a described string.
    sdxDestroy() - destroys a described string.
    sdxDuplicate() - duplicates a described string.
    sdxErase() - erases the contents of a described string.
    sdxGrow() - increases the size of a descriptor's string buffer.
    sdxIncrement() - sets or gets the block size used to expand strings.
    sdxMaxLength() - returns the maximum length of a described string.
    sdxLength() - returns the length of a described string.
    sdxOwn() - takes ownership of a described string.
    sdxReadLine() - reads input from a file into a described string.
    sdxReplace() - replaces a substring in a described string.
    sdxRetry() - retries a function with increasing string lengths.
    sdxSetLength() - sets the length of a described string.
    sdxStringZ() - returns a described string as a NUL-terminated C string.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#if HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "sdx_util.h"			/* String Descriptor utilities. */


/*******************************************************************************
    String Descriptor Data Structures.
*******************************************************************************/

#define  MAX_SDX_INCREMENT  128

typedef  struct  _StringDx {
    SdxStorageClass  allocation ;	/* Allocation type. */
    size_t  increment ;			/* Buffer allocation block size. */
    bool  owned ;			/* Owned by application?  If true,
					   sdxDestroy() or sdxErase() will
					   NOT free a dynamically allocated
					   string; the application will.
					   If false, the destroy and/or erase
					   functions will free the string. */
    size_t  actLength ;			/* Actual length of string. */
    size_t  maxLength ;			/* Size of allocated buffer. */
    char  *buffer ;			/* Buffer holding the string. */
}  _StringDx ;


typedef  struct  _SubStringDx {
    StringDx  string ;			/* Substring of this string. */
    size_t  offset ;			/* Offset from beginning of string. */
    size_t  length ;			/* Length of substring. */
}  _SubStringDx ;


int  sdx_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  sdx_util_debug

/*!*****************************************************************************

Procedure:

    sdxAddC ()

    Add a Character to a Described String.


Purpose:

    Function sdxAddC() appends a character to a described string.  If
    necessary, the size of the described string is increased to hold
    the new character.


    Invocation:

        status = sdxAddC (descriptor, character) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <character>	- I
            is the character to add to the string.
        <status>	- O
            returns the status of appending the character to the string,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  sdxAddC (

#    if PROTOTYPES
        StringDx  descriptor,
        char  character)
#    else
        descriptor, character)

        StringDx  descriptor ;
        char  character ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxAddC) NULL descriptor handle.\n") ;
        return (errno) ;
    }

/* Make sure the buffer can hold an added character.  The call to sdxGrow()
   also checks for a NULL descriptor handle and a static string, both errors
   as far as sdxAddC() is concerned.  If the descriptor is empty, sdxGrow()
   allocates the initial string buffer. */

    if (sdxGrow (descriptor, 1)) {
        LGE "(sdxAddC) Error growing buffer.\nsdxGrow: ") ;
        return (errno) ;
    }

/* Append the character to the string. */

    descriptor->buffer[descriptor->actLength++] = character ;
    descriptor->buffer[descriptor->actLength] = '\0' ;	/* NUL terminator. */

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxAddS ()

    Add Text to a Described String.


Purpose:

    Function sdxAddS() appends a string of text to a described string.
    If necessary, the size of the described string is increased to hold
    the new text.


    Invocation:

        status = sdxAddS (descriptor, text, length) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <text>		- I
            is the string of text to add to the described string.
        <length>	- I
            is the length of the text to add.  If this argument is less than
            zero, the text is assumed to be NUL-terminated and strlen(3) is
            used to determine its length.
        <status>	- O
            returns the status of appending the text to the string,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  sdxAddS (

#    if PROTOTYPES
        StringDx  descriptor,
        const  char  *text,
        ssize_t  length)
#    else
        descriptor, text, length)

        StringDx  descriptor ;
        const  char  *text ;
        ssize_t  length ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxAddS) NULL descriptor handle.\n") ;
        return (errno) ;
    }

/* If the text argument is NULL, check the length argument.  If the length
   is greater than zero (i.e., there should be some text), then report an
   error.  Otherwise, treat the text argument as a zero-length string. */

    if (text == NULL) {
        if (length > 0) {
            SET_ERRNO (EINVAL) ;
            LGE "(sdxAddS) NULL text string.\n") ;
            return (errno) ;
        } else {
            text = "" ;  length = 0 ;
        }
    }

    if (length < 0)  length = strlen (text) ;

/* Make sure the buffer can hold the added text.  The call to sdxGrow() also
   checks for a NULL descriptor handle and a static string, both errors as far
   as sdxAddS() is concerned.  If the descriptor is empty, sdxGrow() allocates
   the initial string buffer. */

    if (sdxGrow (descriptor, length)) {
        LGE "(sdxAddS) Error growing buffer.\nsdxGrow: ") ;
        return (errno) ;
    }

/* Append the new string to the existing string. */

    memcpy (&descriptor->buffer[descriptor->actLength], text, length) ;
    descriptor->actLength += length ;
    descriptor->buffer[descriptor->actLength] = '\0' ;	/* NUL terminator. */

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxCopy ()

    Copy One String Descriptor to Another String Descriptor.


Purpose:

    Function sdxCopy() copies one string descriptor to another, erasing
    the destination descriptor's prior contents.


    Invocation:

        status = sdxCopy (source, isStatic, destination) ;

    where

        <source>	- I
            is the handle returned by sdxCreate() for the descriptor to be
            copied.
        <isStatic>	- I
            specifies whether (true) or not (false) the destination descriptor
            will have a static pointer to the source descriptor's string buffer,
            regardless of the source descriptor's allocation class.  If this
            argument is true, the destination descriptor will have an
            allocation type of static and its buffer pointer will be identical
            to the source descriptor's pointer, which may have an allocation
            type of SdxStatic or SdxDynamic.  NOTE that if the source
            descriptor's allocation type is SdxDynamic, its string buffer
            may be free(3)'d without the destination descriptor being any
            the wiser.  Use this capability carefully.
        <destination>	- O
            is the destination string descriptor to which the source descriptor
            is copied.  The prior contents of the destination descriptor are
            erased before the copy.
        <status>	- O
            returns the status of copying the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  sdxCopy (

#    if PROTOTYPES
        StringDx  source,
        bool  isStatic,
        StringDx  destination)
#    else
        source, isStatic, destination)

        StringDx  source ;
        bool  isStatic ;
        StringDx  destination ;
#    endif

{

    if ((source == NULL) || (destination == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxCopy) NULL source and/or destination descriptors.\n") ;
        return (errno) ;
    }

    LGI "(sdxCopy) Copying descriptor %p to descriptor %p ...\n",
        (void *) source, (void *) destination) ;

    sdxErase (destination) ;

/* If the is-static? argument is true, the destination descriptor will have
   a "static" pointer to the source descriptor's string buffer, whether or
   not the source's buffer was statically or dynamically allocated. */

    if (isStatic) {
        *destination = *source ;
        destination->allocation = SdxStatic ;
        return (0) ;
    }

/* Otherwise, make a dynamic copy of the source descriptor's string buffer. */

    destination->allocation = SdxDynamic ;
    destination->owned = false ;
    destination->actLength = 0 ;
    destination->maxLength = 0 ;
    destination->buffer = NULL ;

    if (source->buffer != NULL) {

        destination->buffer = memdup (source->buffer, source->maxLength) ;
        if (destination->buffer == NULL) {
            LGE "(sdxCopy) Error duplicating source string: \"%s\"\nmemdup: ",
                source->buffer) ;
            return (errno) ;
        }
        destination->actLength = source->actLength ;
        destination->maxLength = source->maxLength ;

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxCreate ()

    Create a String Descriptor.


Purpose:

    Function sdxCreate() creates and initializes a descriptor for a string
    or for no string.


    Invocation:

        status = sdxCreate (string, length, allocation, &descriptor) ;

    where

        <string>	- I
            is a string to be described by the descriptor.  If this argument
            is NULL, then an empty descriptor is created.
        <length>	- I
            is the size of the input string.  If this length is -1, sdxCreate()
            will scan the input string for a NUL terminator.
        <allocation>	- I
            specifies the allocation type of the input string:
                "static" - is a fixed string.
                "dynamic" - is a dynamically allocated string.  The descriptor
                    takes "ownership" of the string, which will be free()ed
                    when the descriptor is destroyed.
                "volatile" - is a string buffer in flux.  A dynamic copy of
                    the string is made and stored in the descriptor; the copy
                    is annotated as "dynamic".
        <descriptor>	- O
            returns a handle for the string descriptor.
        <status>	- O
            returns the status of creating a descriptor, zero if no errors
            occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  sdxCreate (

#    if PROTOTYPES
        char  *string,
        ssize_t  length,
        SdxStorageClass  allocation,
        StringDx  *descriptor)
#    else
        string, length, allocation, descriptor)

        char  *string ;
        ssize_t  length ;
        SdxStorageClass  allocation ;
        StringDx  *descriptor ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxCreate) NULL descriptor handle pointer.\n") ;
        return (errno) ;
    }

/* Create and initialize the descriptor. */

    *descriptor = (StringDx) malloc (sizeof (_StringDx)) ;
    if (*descriptor == NULL) {
        LGE "(sdxCreate) Error allocating string descriptor.\nmalloc: ") ;
        return (errno) ;
    }

    (*descriptor)->allocation = SdxDynamic ;
    (*descriptor)->increment = MAX_SDX_INCREMENT ;
    (*descriptor)->owned = false ;
    (*descriptor)->actLength = 0 ;
    (*descriptor)->maxLength = 0 ;
    (*descriptor)->buffer = NULL ;

    if ((length < 0) && (string != NULL))  length = strlen (string) ;

    if (string == NULL) {
		/* Descriptor was correctly initialized. */
    } else if (allocation == SdxVolatile) {
        (*descriptor)->allocation = SdxDynamic ;
        if (sdxAddS (*descriptor, string, length)) {
            LGE "(sdxCreate) Error duplicating input string: \"%.*s\"\nsdxGrow: ",
                (int) length, string) ;
            PUSH_ERRNO ;  sdxDestroy (*descriptor) ;
            *descriptor = NULL ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {			/* SdxStatic or SdxDynamic */
        (*descriptor)->allocation = allocation ;
        (*descriptor)->buffer = string ;
        (*descriptor)->actLength = length ;
        (*descriptor)->maxLength = length ;
    }

    if (string == NULL) {
        LGI "(sdxCreate) Created string descriptor %p for \"<null>\".\n",
            (void *) *descriptor) ;
    } else {
        LGI "(sdxCreate) Created string descriptor %p for \"%.*s\"\n",
            (void *) *descriptor, (int) length, (*descriptor)->buffer) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxDestroy ()

    Destroy a String Descriptor.


Purpose:

    Function sdxDestroy() destroys a previously created string descriptor.


    Invocation:

        status = sdxDestroy (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <status>	- O
            returns the status of destroying the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  sdxDestroy (

#    if PROTOTYPES
        StringDx  descriptor)
#    else
        descriptor)

        StringDx  descriptor ;
#    endif

{

    if (descriptor == NULL)  return (0) ;

    LGI "(sdxDestroy) Destroying string descriptor %p ...\n",
        (void *) descriptor) ;

    sdxErase (descriptor) ;

    free (descriptor) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxDuplicate ()

    Duplicate a String Descriptor.


Purpose:

    Function sdxDuplicate() duplicates a string descriptor.  The string
    buffer in the duplicate descriptor is always a dynamically allocated
    copy of the source descriptor's buffer.  In particular, this means
    that if a source descriptor points to a statically allocated string,
    any duplicate descriptors point not to the original static copy of
    the string, but to dynamic copies of the string.


    Invocation:

        newDescriptor = sdxDuplicate (oldDescriptor) ;

    where

        <oldDescriptor>	- I
            is a descriptor handle returned by sdxCreate().
        <newDescriptor>	- O
            returns the handle for the duplicate descriptor; NULL is returned
            in the event of an error.

*******************************************************************************/


StringDx  sdxDuplicate (

#    if PROTOTYPES
        StringDx  oldDescriptor)
#    else
        oldDescriptor)

        StringDx  oldDescriptor ;
#    endif

{    /* Local variables. */
    StringDx  newDescriptor ;



    if (oldDescriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxDuplicate) NULL descriptor handle.\n") ;
        return (NULL) ;
    }

    if (sdxCreate (oldDescriptor->buffer, oldDescriptor->actLength,
                   SdxVolatile, &newDescriptor)) {
        LGE "(sdxDuplicate) Error creating duplicate descriptor.\nsdxCreate: ") ;
        return (NULL) ;
    }

    LGI "(sdxDuplicate) Created duplicate descriptor %p for \"%s\"\n",
        (void *) newDescriptor,
        (newDescriptor->buffer == NULL) ? "<null>" : newDescriptor->buffer) ;

    return (newDescriptor) ;

}

/*!*****************************************************************************

Procedure:

    sdxErase ()

    Erase the Contents of a String Descriptor.


Purpose:

    Function sdxErase() erases the contents of a previously created string
    descriptor, leaving an empty string descriptor.


    Invocation:

        status = sdxErase (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <status>	- O
            returns the status of erasing the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  sdxErase (

#    if PROTOTYPES
        StringDx  descriptor)
#    else
        descriptor)

        StringDx  descriptor ;
#    endif

{

    if (descriptor == NULL)  return (0) ;

    LGI "(sdxErase) Erasing string descriptor %p ...\n",
        (void *) descriptor) ;

    if ((descriptor->allocation == SdxDynamic) &&
        !descriptor->owned &&
        (descriptor->buffer != NULL)) {
        free (descriptor->buffer) ;
    }

    descriptor->allocation = SdxVolatile ;
    descriptor->actLength = 0 ;
    descriptor->maxLength = 0 ;
    descriptor->buffer = NULL ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxGrow ()

    Increase the Size of a Descriptor's String Buffer.


Purpose:

    Function sdxGrow() expands the size of a descriptor's string buffer so
    that more characters and text can be added.  NOTE that sdxGrow() grows
    the size of the internal buffer, NOT the length of the existing string
    (which won't change until the caller actually adds text to the string.)

    If the string buffer is already large enough for the requested number
    of characters to be added, sdxGrow() returns silently without error.
    In other words, the caller can call sdxGrow() everytime text is to be
    added to the string and sdxGrow() will only expand the size of the
    buffer when necessary.


    Invocation:

        status = sdxGrow (descriptor, toAdd) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <toAdd>		- I
            is the number of characters the caller wishes to add to the
            string.  sdxGrow() will increase the size of the buffer to
            hold the existing string plus the new text to be added.
        <status>	- O
            returns the status of expanding the buffer, zero if there
            were no errors and ERRNO otherwise.  Zero is also returned
            if the buffer is already large enough to hold the new text
            being added.

*******************************************************************************/


errno_t  sdxGrow (

#    if PROTOTYPES
        StringDx  descriptor,
        size_t  toAdd)
#    else
        descriptor, toAdd)

        StringDx  descriptor ;
        size_t  toAdd ;
#    endif

{    /* Local variables. */
    char  *newBuffer ;
    size_t  length, numBlocks ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxGrow) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    if (descriptor->allocation == SdxStatic) {
        SET_ERRNO (EPERM) ;
        LGE "(sdxGrow) Cannot change static string.\n") ;
        return (errno) ;
    }

    LGI "(sdxGrow) Dx %p  Actual: %"PRIuSIZE_T"  Max: %"PRIuSIZE_T"  #Add: %"PRIuSIZE_T"\n",
        (void *) descriptor, descriptor->actLength,
        descriptor->maxLength, toAdd) ;

/* Compute what will be the new length of the descriptor's string:
   old string + new string + NUL terminator. */

    length = descriptor->actLength + toAdd + 1 ;
    if (length < descriptor->maxLength) {	/* Already have enough space? */
        return (0) ;
    }

/* The string buffer is extended in blocks of size MAX_SDX_INCREMENT, so
   round the length up to the next multiple of MAX_SDX_INCREMENT. */

    numBlocks = (length + descriptor->increment - 1) / descriptor->increment ;
    length = numBlocks * descriptor->increment ;

/* If this is the first allocation for an empty descriptor, then do so. */

    if (descriptor->buffer == NULL) {
        descriptor->allocation = SdxDynamic ;
        newBuffer = malloc (length) ;
    }

/* Otherwise, increase the size of the buffer to hold the new string. */

    else {
        newBuffer = realloc (descriptor->buffer, length) ;
    }

    if (newBuffer == NULL) {
        PUSH_ERRNO ;
        LGE "(sdxGrow) Error (re)allocating %"PRIuSIZE_T"-byte buffer.\nmalloc/realloc: ",
            length) ;
        POP_ERRNO ;
        return (errno) ;
    }

    LGI "(sdxGrow) Dx %p  Old: %p  New: %p  Max: %"PRIuSIZE_T"\n",
        (void *) descriptor, descriptor->buffer, newBuffer, length) ;

    memset (&newBuffer[descriptor->actLength], '\0',
            length - descriptor->actLength) ;
    descriptor->buffer = newBuffer ;
    descriptor->maxLength = length ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxIncrement ()

    Set/Get the Allocation Increment for a Described String.


Purpose:

    Function sdxIncrement() sets/gets the block size used by sdxGrow()
    for a specific described string.  When text is added to a dynamic
    string and sdxGrow() needs to expand the buffer to hold it, the
    new buffer size is the total length of the string being constructed
    rounded up to the next highest multiple of the block size.

    The default block size is MAX_SDX_INCREMENT, 32.  Increasing the
    block size reduces how frequently sdxGrow() has to reallocate the
    buffer, a good idea when a large string is growing rapidly.

    The new block size does not take effect until the next time
    sdxGrow() has to reallocate the buffer.

    If the calling application specifies a block size of -1,
    sdxIncrement() simply returns the current increment.


    Invocation:

        increment = sdxIncrement (descriptor, blockSize) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <blockSize>	- I
            specifies the new block size in characters.  If the block
            size is -1 or 0, sdxIncrement() simply returns the current
            block size.  Otherwise, this new block size will replace
            the current block size.
        <increment>	- O
            returns the descriptor's current or new increment, depending
            on the block size input argument.  If the input argument is
            -1 or 0, the caller is "GETing" the descriptor's existing
            block size and that is what will be returned.  If the input
            argument is >= 1, the caller is "SETing" the descriptor's
            increment to the new value and that value will be returned
            to the caller.

*******************************************************************************/


size_t  sdxIncrement (

#    if PROTOTYPES
        StringDx  descriptor,
        ssize_t  blockSize)
#    else
        descriptor, blockSize)

        StringDx  descriptor ;
        ssize_t  blockSize ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxIncrement) NULL descriptor handle.\n") ;
        return (0) ;
    }

    if (blockSize >= 1)		/* Caller is setting a new block size. */
        descriptor->increment = blockSize ;

    return (descriptor->increment) ;

}

/*!*****************************************************************************

Procedure:

    sdxLength ()

    Get the Length of a Described String.


Purpose:

    Function sdxLength() returns the length of the string to which
    a descriptor points.  If the caller has modified the string in
    such a way as to change its length without the descriptor being
    updated, sdxSetLength() should be called to recompute the length.


    Invocation:

        length = sdxLength (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <length>	- O
            returns the number of characters in the string, not including
            the trailing NUL byte; zero is returned in the event of an
            error.  In the latter case, an error is indistinguishable
            from a zero-length string.

*******************************************************************************/


size_t  sdxLength (

#    if PROTOTYPES
        StringDx  descriptor)
#    else
        descriptor)

        StringDx  descriptor ;
#    endif

{

    return ((descriptor == NULL) ? 0 : descriptor->actLength) ;

}

/*!*****************************************************************************

Procedure:

    sdxMaxLength ()

    Get the Maximum Length of a Described String.


Purpose:

    Function sdxMaxLength() returns the size of the currently allocated
    buffer for a described string.  The actual length of the string is,
    of course, less than or equal to the maximum length.


    Invocation:

        size = sdxMaxLength (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <size>		- O
            returns the maximum number of characters currently allocated
            for the string (including the trailing NUL byte).

*******************************************************************************/


size_t  sdxMaxLength (

#    if PROTOTYPES
        StringDx  descriptor)
#    else
        descriptor)

        StringDx  descriptor ;
#    endif

{

    return ((descriptor == NULL) ? 0 : descriptor->maxLength) ;

}

/*!*****************************************************************************

Procedure:

    sdxOwn ()

    Take Ownership of the Described String.


Purpose:

    Function sdxOwn() grants ownership of a dynamically allocated string
    to the application.  Normally, a call to sdxDestroy() or sdxErase()
    will free a dynamically allocated string.  However, if the application
    has taken possession of the string, then the application is responsible
    for freeing the string after the string descriptor is destroyed or
    erased.  This is typically done when the application is dynamically
    constructing a string using the SDX_UTIL functions and the final result
    must exist independently (i.e., past the lifetime) of the descriptor.
    The application must free(3) the string when it is no longer needed.

        NOTE that an application can take ownership of a string at any
        time.  The descriptor's string will continue to grow as needed,
        which may require reallocating the string to hold more characters.
        When this happens, a new, larger string buffer is allocated, the
        old string is copied to the new buffer, and the old string is freed.
        Consequently, in order to get the correct string to free, the
        application should make a final call to sdxStringZ() right before
        calling sdxDestroy() or sdxErase().  (Unless, of course, the
        application knows that it hasn't added anything to the described
        string since it last obtained the address.)


    Invocation:

        status = sdxOwn (descriptor) ;

    where

        <descriptor>	- I
            is a descriptor handle returned by sdxCreate().
        <status>	- O
            returns the status of taking ownership of the string,
            zero on success and ERRNO otherwise.  It is not an
            error to take ownership of a static string; doing so
            has no effect, although the application must be sure
            not to attempt freeing the static string.

*******************************************************************************/


errno_t  sdxOwn (

#    if PROTOTYPES
        StringDx  descriptor)
#    else
        descriptor)

        StringDx  descriptor ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxOwn) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    descriptor->owned = true ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxReadLine ()

    Read a Line from a File and Append It to a Described String.


Purpose:

    Function sdxReadLine() reads the next line of input from a text
    file and appends the line to the buffer in the caller-supplied
    string descriptor.  The buffer grows as needed, so an arbitrarily
    long input line can be read (within reason).

    The following example reads a complete text file into a string;
    see the NOTEs afterwards:

        FILE  *file = fopen ("example.txt", "r") ;
        ssize_t  numLines ;
        StringDx  desc ;

        sdxCreate (NULL, -1, SdxVolatile, &desc) ;
        for ( ; ; ) {			-- Break on EOF or error.
            if (sdxReadLine (file, desc, &numLines) || (numLines < 1))  break ;
            sdxAddS (desc, "\n", -1) ;	-- sdxReadLine() strips NLs.
        }
        fclose (file) ;
        ... process full text file in described string (assuming
            no errors during reading) ...

    NOTE that the numLines variable will report at most ONE line being
    read.  Also NOTE that, if the last line is terminated by EOF instead
    of a newline, this variable reports ONE line being read; EOF will be
    reported in either case on the next call to sdxReadLine().  Finally,
    NOTE that sdxReadLine() strips the terminating newline from each
    input line, so this variable is needed by the caller to detect blank
    lines: the described string does not increase in length and this
    variable reports one line read.

    To summarize the values returned in the numLines argument:

        #lines  #chars  Terminator
           1      n>0      NL		Characters followed by newline.
           1      n>0      EOF		Characters followed by end-of-file.
           1      n=0      NL		Blank line; no change in string length.
           0      n=0      EOF		No more input to read.
          -1      N/A      N/A		Error indicated by ferror(3).


    Invocation:

        status = sdxReadLine (file, descriptor, &numLines) ;

    where

        <file>		- I/O
            is the (FILE *) handle for the input file.
        <descriptor>	- I/O
            is the descriptor handle returned by sdxCreate().  The next line
            from the file is read and appended to the described string, minus
            the terminating newline or form-feed, if any.
        <numLines>	- O
            returns 1 if one line was successfully read, 0 if the file
            is at its EOF and no input could be read, and -1 if there
            was an error.  The latter two return values are determined
            by feof(3) and ferror(3), respectively.  See the NOTEs
            about this argument in the PURPOSE section above.
        <status>	- O
            returns the status of reading the next line, zero if there
            were no errors and ERRNO otherwise.  EOF is NOT an error and
            the caller must check if the number of lines read is zero.

*******************************************************************************/


errno_t  sdxReadLine (

#    if PROTOTYPES
        FILE  *file,
        StringDx  descriptor,
        ssize_t  *numLines)
#    else
        file, descriptor, numLines)

        FILE  *file ;
        StringDx  descriptor ;
        ssize_t  *numLines ;
#    endif

{    /* Local variables. */
    bool  newline ;
    char  *s ;
    size_t  numRead, totalRead ;
    ssize_t  temp ;



    if ((descriptor == NULL) || (file == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxReadLine) NULL descriptor or file handle.\n") ;
        return (errno) ;
    }

    if (numLines == NULL)  numLines = &temp ;
    *numLines = -1 ;		/* Assume an error. */

/* Read blocks of text from the file until the line terminator is found.
   Increase the size of the descriptor's buffer each time so that the
   input can be read directly into the buffer. */

    totalRead = 0 ;
    newline = false ;		/* Newline seen yet? */

    while (!feof (file) && !newline) {

        if (sdxGrow (descriptor, descriptor->increment)) {
            LGE "(sdxReadLine) Error growing buffer.\nsdxGrow: ") ;
            return (errno) ;
        }

        s = &descriptor->buffer[descriptor->actLength] ;

        if (fgets (s, (int) descriptor->increment, file) == NULL) {
            if (feof (file))  continue ;
            SET_ERRNO (EIO) ;
            LGE "(sdxReadLine) Error reading input file.\nfgets: ") ;
            return (errno) ;
        }

        numRead = strlen (s) ;
        totalRead += numRead ;

        newline = (s[numRead-1] == '\n') ;	/* End of line? */
        if (newline)  s[--numRead] = '\0' ;	/* Strip terminating NL. */

        descriptor->actLength += numRead ;
        LGI "(sdxReadLine) %"PRIuSIZE_T"\t\"%s\"\n", numRead, s) ;

    }

    *numLines = (totalRead > 0) ? 1 : 0 ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxReplace ()

    Replace a Substring in a Described String.


Purpose:

    Function sdxReplace() finds and replaces one or more occurrences
    of a substring in a described target string.  The target string
    expands as necessary if the replacement string is longer than the
    search string.  Replacement is not recursive; after replacing an
    instance of the substring, sdxReplace() skips past the replacement
    text before scanning for the next occurrence of the substring.


    Invocation:

        status = sdxReplace (descriptor, ignoreCase,
                             find, findlen,
                             replace, replacelen,
                             maxReplace, &numReplace) ;

    where

        <descriptor>	- I
            is the descriptor returned by sdxCreate() for the target string.
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
            no errors and ERRNO otherwise.

*******************************************************************************/

/*******************************************************************************
    sdxTryReplace() - helper function passed to sdxRetry() in sdxReplace().
*******************************************************************************/

typedef  struct  SdxReplaceArgs {
    bool  ignoreCase ;
    const  char  *find ;
    ssize_t  findLen ;
    const  char  *replace ;
    ssize_t  replaceLen ;
    ssize_t  maxReplace ;
    size_t  *numReplace ;
}  SdxReplaceArgs ;

static  errno_t  sdxTryStrReplace (
#    if PROTOTYPES
        StringDx  descriptor,		/* Translation buffer. */
        void *userData)			/* Path name to be translated. */
#    else
        descriptor, userData)

        StringDx  descriptor ;
        void *userData ;
#    endif
{    /* Local variables. */
    SdxReplaceArgs  *args = (SdxReplaceArgs *) userData ;

    if (strReplace (sdxStringZ (descriptor), sdxMaxLength (descriptor),
                    args->ignoreCase, args->find, args->findLen,
                    args->replace, args->replaceLen,
                    args->maxReplace, args->numReplace)) {
        if (errno != ERANGE) {
            LGE "(sdxTryStrReplace) Error replacing substring in \"%s\".\nstrReplace: ",
                sdxStringZ (descriptor)) ;
        }
        PUSH_ERRNO ;  sdxSetLength (descriptor, -1) ;  POP_ERRNO ;
        return (errno) ;
    }
	/* SDX_UTIL still thinks the length is 0, so be sure to set it. */
    sdxSetLength (descriptor, -1) ;
    LGI "(sdxTryStrReplace) \"%s\"\n", sdxStringZ (descriptor)) ;
    return (0) ;
}


/*******************************************************************************
    sdxReplace() - itself.
*******************************************************************************/

errno_t  sdxReplace (

#    if PROTOTYPES
        StringDx  descriptor,
        bool  ignoreCase,
        const  char  *find,
        ssize_t  findLen,
        const  char  *replace,
        ssize_t  replaceLen,
        ssize_t  maxReplace,
        size_t  *numReplace)
#    else
        descriptor, ignoreCase,
        find, findLen,
        replace, replaceLen,
        maxReplace, numReplace)

        StringDx  descriptor ;
        bool  ignoreCase ;
        char  *find ;
        ssize_t  findLen ;
        char  *replace ;
        ssize_t  replaceLen ;
        ssize_t  maxReplace ;
        size_t  *numReplace ;
#    endif

{    /* Local variables. */
    SdxReplaceArgs  args ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxReplace) NULL descriptor handle.\n") ;
        return (errno) ;
    }

/* Collect the arguments to pass to sdxTryStrReplace(). */

    args.ignoreCase = ignoreCase ;
    args.find = find ;
    args.findLen = findLen ;
    args.replace = replace ;
    args.replaceLen = replaceLen ;
    args.maxReplace = maxReplace ;
    args.numReplace = numReplace ;

/* Let sdxRetry() repeatedly call strReplace() to perform the replacements,
   expanding the size of the described string as necessary. */

    if (sdxRetry (descriptor, 128, sdxTryStrReplace, &args)) {
        LGE "(sdxReplace) Error replacing substring in \"%s\".\nsdxRetry: ",
            sdxStringZ (descriptor)) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxRetry ()

    Retry a Function With Increasing String Lengths.


Purpose:

    Function sdxRetry() repeatedly invokes a caller-supplied function with
    a string descriptor, increasing the size of the string until the function
    succeeds.  It is specifically intended for calling functions that return
    an ERRNO status of ERANGE if the supplied buffer is not large enough to
    hold the result.

    The caller-supplied function will indirectly call the target function
    (since the target will most likely not have the same signature as
    SdxRetryF).  ERANGE should be returned if the length of the string
    is not sufficient to hold the result; success or other errors cause
    sdxRetry() to return immediately.

            NOTE:  If the caller-supplied function directly
            or indirectly modifies the string buffer outside
            the SDX_UTIL functions, the function must set or
            recalculate the length of the described string
            using sdxSetLength().  Otherwise, subsequent calls
            to sdxLength() will return the wrong length.

    For example, getcwd(3) returns ERANGE if the buffer is not large enough.
    A program can use sdxRetry() as follows:

        errno_t  getcwdWrapper (StringDx  descriptor,
                                void *userData)
        {
            if (NULL == getcwd (sdxStringZ (descriptor),
                                sdxMaxLength (descriptor))) {
                return (errno) ;
            }
            sdxSetLength (descriptor, -1) ;
            return (0) ;
        }

        ...
        char  *cwd ;
        StringDx  cwdx ;

        sdxCreate (NULL, -1, SdxDynamic, &cwdx) ;

        if (sdxRetry (cwdx, getcwdWrapper, NULL))  ... error ...

        sdxOwn (cwdx) ;		-- Grab C string with directory.
        cwd = sdxStringZ (cwdx) ;
        sdxDestroy (cwdx) ;

        ... use the directory pointed to by CWD ...

        free (cwd) ;		-- Remember to deallocate C string.


    Invocation:

        status = sdxRetry (descriptor, retryF, userData) ;

    where

        <descriptor>	- I
            is a descriptor handle returned by sdxCreate().
        <retryF>	- I
            is a pointer to a caller-supplied function that will be retried.
            The function must have the type signature of SdxRetryFunc.
        <userData>	- I
            is a (void *) pointer to data the caller's function may need.
        <status>	- O
            returns the status of the last retry of the caller's function,
            either zero or an ERRNO not equal to ERANGE.

*******************************************************************************/


errno_t  sdxRetry (

#    if PROTOTYPES
        StringDx  descriptor,
        size_t  increment,
        SdxRetryFunc  retryF,
        void  *userData)
#    else
        descriptor, increment, retryF, userData)

        StringDx  descriptor ;
        size_t  increment ;
        SdxRetryFunc  retryF ;
        void  *userData ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxRetry) NULL descriptor handle.\n") ;
        return (errno) ;
    }

/* Perform an initial allocation of the specified increment. */

    if (increment == 0)  increment = 32 ;	/* Arbitrary. */

    if (sdxGrow (descriptor, increment)) {
        LGE "(sdxRetry) Error performing initial increment.\nsdxGrow: ") ;
        return (errno) ;
    }

/* Invoke the caller's function.  If it succeeds, return success.  If it fails
   and it is not an ERANGE error, then return failure.  If the error status is
   ERANGE, then increment the allocated string and retry the function. */

    for ( ; ; ) {

        if (retryF (descriptor, userData) != ERANGE)  break ;

        if (sdxGrow (descriptor, increment)) {
            LGE "(sdxRetry) Error increasing allocation before retry.\nsdxGrow: ") ;
            return (errno) ;
        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    sdxSetLength ()

    Set the Length of a Described String.


Purpose:

    Function sdxSetLength() sets or recomputes the length of the string
    to which a descriptor points.  This ability is provided in case the
    caller has modified the described string, independently of the
    descriptor, in such a way as to change its length.


    Invocation:

        status = sdxSetLength (descriptor, length) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <length>	- I
            is the new length of the described string.  If this argument is
            less than zero, the string is assumed to be NUL-terminated and
            its length is recomputed using strlen(3).
        <status>	- O
            returns the status of appending the text to the string,
            zero if there were no errors and ERRNO otherwise.
        <length>	- O
            returns the number of characters in the string, not including
            the trailing NUL byte; zero is returned in the event of an error.
            In the latter case, an error is indistinguishable from a
            zero-length string.

*******************************************************************************/


errno_t  sdxSetLength (

#    if PROTOTYPES
        StringDx  descriptor,
        ssize_t  length)
#    else
        descriptor, length)

        StringDx  descriptor ;
        ssize_t  length ;
#    endif

{

    if ((descriptor == NULL) || (descriptor->buffer == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxSetLength) NULL descriptor handle or buffer.\n") ;
        return (errno) ;
    }

    if (length < 0)  length = (ssize_t) strnlen (descriptor->buffer,
                                                 descriptor->maxLength) ;

    if (length < (ssize_t) descriptor->maxLength) {
        descriptor->actLength = (size_t) length ;
        return (0) ;
    } else {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxSetLength) String length %"PRIuSIZE_T" exceeds maximum buffer size %"PRIuSIZE_T".\n",
            descriptor->actLength, descriptor->maxLength) ;
        return (0) ;
    }

}

/*!*****************************************************************************

Procedure:

    sdxStringZ ()

    Get the Described String as a NUL-Terminated C String.


Purpose:

    Function sdxStringZ() returns the NUL-terminated string to which
    a descriptor points.

    Well, not necessarily NUL-terminated; the caller is responsible
    for knowing whether or not the desired string is NUL-terminated.
    See the "NOTE:" in the file prologue about passing in SdxStatic
    and pre-allocated SdxDynamic strings to sdxCreate().  SdxVolatile
    strings, which must be duplicated, will be reclassified as
    SdxDynamic and will always have a NUL terminator.  As you can see,
    SdxDynamic has one meaning upon entry to sdxCreate() and the more
    common meaning after the descriptor is constructed.

    Lastly, the SDX_UTIL functions can handle strings with embedded
    NUL characters, so a trailing NUL character may have no standing
    as a string terminator.  Again, the caller's responsibility.


    Invocation:

        string = sdxStringZ (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by sdxCreate().
        <string>	- O
            returns the NUL-terminated C string.  A NULL pointer is returned
            in the event of an error or if the descriptor is empty.

*******************************************************************************/


char  *sdxStringZ (

#    if PROTOTYPES
        StringDx  descriptor)
#    else
        descriptor)

        StringDx  descriptor ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(sdxStringZ) NULL descriptor handle.\n") ;
        return (NULL) ;
    }

    return (descriptor->buffer) ;

}
