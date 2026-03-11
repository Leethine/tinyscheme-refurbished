/* $Id: mdx_util.c,v 1.1 2022/03/03 17:18:22 alex Exp alex $ */
/*******************************************************************************

File:

    mdx_util.c

    Memory Descriptor (MDX) Utilities


Author:    Alex Measday


Purpose:

    The MDX_UTIL package provides functions for creating and manipulating
    dynamically sized and dynamically growing memory regions.

    This package was inspired by my SDX_UTIL string descriptor package
    which, in turn, was inspired by VAX/VMS descriptors which consisted
    of (i) 16 bits of flags including the data type of the object being
    described, (ii) the 16-bit length of the object, and (iii) the 32-bit
    address of the object.

    While I originally expected MDX_UTIL to be a simple, byte-oriented
    clone of SDX_UTIL, I unfortunately included the ability to treat a
    memory region as an array of fixed-size elements -- a C++ STL vector,
    if you wish, and my most common use case.  Part way into the writing
    of the package, I was torn between putting the array functionality
    into a separate package, say ADX_UTIL, or to keep going and decide
    later after some experience with an array-capable MDX_UTIL package.

                        I chose the latter course,
                           for better or worse,
                    although I'm bothered all the same,
                           of course, of course.

                          (We miss you, Mr. Ed!)

    On the other hand, never mind.  Array operations in the MDX_UTIL
    package were awkward to use, so I did write the ADX_UTIL package.
    Use that instead for arrays.


    Byte-Oriented Memory Operations
    -------------------------------

    A sequence of bytes is simply an array of one-byte elements in
    MDX_UTIL's view.  The following example creates a zero-length
    memory descriptor (whose default element size is one byte) and
    concatenates 50 copies of a 13-byte sequence of bytes, "Hello,
    World!" in this case.  There is no significance aside from
    convenience to my using a string here; one could just as easily
    create a 13-byte array of arbitrary, "!isprint()able" bytes.

    NOTE that there are no separators between the sequences of bytes.
    The example appends the 13-byte sequences end-on-end, 50 times,
    to the initially empty memory buffer.  The result is 650 (13*50)
    bytes of data.  The example adds a NUL terminator as the 651st
    byte to produce an actual string in the I-could-care-less memory
    buffer.

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  <string.h>		-- Standard C string functions.
        #include  "mdx_util.h"		-- Memory Descriptor utilities.
        MemoryDx  pbx ;
        size_t  i ;
        ...
        mdxCreate (NULL, 0, MdxDynamic, &pbx) ;
        for (i = 0 ;  i < 50 ;  i++) {
            mdxAddS (pbx, false,	-- Add sequence of bytes.
                     "Hello, World!", strlen ("Hello, World!")) ;
        }
        mdxAddB (pbx, 0) ;		-- Add NUL byte.
        printf ("Complete region: \"%s\"\n", (char *) mdxRegionP (pbx, 0)) ;
        mdxDestroy (pbx) ;

    Alternatively, don't add the NUL byte, but retrieve the length of 650:

        printf ("Complete region: \"%*s\"\n",
                (int) mdxLength (descriptor),
                (char *) mdxRegionP (pbx, 0)) ;


    Multi-Byte-Oriented Array Operations
    ------------------------------------

    A memory region can also be treated as an array of fixed-size
    elements.  The following example reads a complete text file into
    an array of SDX_UTIL string descriptors, one element per line, a
    convenient framework for a simple text editor.

        FILE  *file = fopen ("example.txt", "r") ;
        size_t  length, numLines = 0 ;
        ssize_t  count ;
        MemoryDx  mdx ;
        StringDx  *element30, sdx ;

        mdxCreate (NULL, 0, MdxDynamic, &mdx) ;
        mdxElementSize (mdx, (ssize_t) sizeof (StringDx)) ;

        for ( ; ; ) {			-- Break on EOF or error.
            sdxCreate (NULL, -1, SdxDynamic, &sdx) ;
            if (sdxReadLine (file, sdx, &count) || (count < 1))  break ;
            mdxAddS (mdx, false, (const void *) &sdx, sizeof (StringDx)) ;
            numLines++ ;
        }
        fclose (file) ;

	---------------------
	-- Access line 20. --
	---------------------

        printf ("Line 20: \"%s\"\n",
                sdxStringZ (*((StringDx) mdxRegionP (mdx, 20-1)))) ;

	------------------------------------------------
	-- Insert a line between line 30 and line 31. --
	------------------------------------------------

        length = mdxLength (mdx) ;	-- Make room for new line.
        mdxSetLength (mdx, length + 1) ;
        element30 = (StringDx *) mdxRegionP (mdx, 30-1) ;
        memmove ((void *) &element30[2], (const void *) &element30[1],
                 (length - 30) * sizeof (StringDx)) ;
					-- Create and store new line.
        sdxCreate ("I'm the new line 31!!!", -1, SdxVolatile, &sdx) ;
        element30[1] = sdx ;

	-------------------------
	-- Delete lines 41-45. --
	-------------------------

        element40 = (StringDx *) mdxRegionP (mdx, 40-1) ;
					-- Destroy descriptors being deleted
        for (i = 1 ;  i <= 5 ;  i++)	-- (or save them in cut/undo buffer).
            sdxDestroy (element40[i]) ;
        length = mdxLength (mdx) ;	-- Move lines 46..n down to line 41.
        memmove ((void *) &element40[1], (const void *) &element40[6],
                 (length - 45) * sizeof (StringDx)) ;
					-- Decrease region length by 5 lines.
        mdxSetLength (mdx, length - (45-40)) ;

    Hmmm, functional, but not pretty.

        NOTEs on the example above.  (1) The memory region is an array of
        pointers (StringDx) to string descriptors (_StringDx), not an array
        of string descriptors themselves.


Public Procedures:

    mdxAddB() - adds a byte to a described memory region.
    mdxAddS() - adds a sequence of bytes to a described memory region.
    mdxAlignment() - sets or gets the byte boundary for alignment.
    mdxCopy() - copy the contents of one memory descriptor to another.
    mdxCreate() - creates a memory descriptor.
    mdxDestroy() - destroys a memory descriptor.
    mdxDuplicate() - duplicates a memory descriptor.
    mdxErase() - erases the contents of a memory descriptor.
    mdxGrow() - increases the size of a descriptor's memory region.
    mdxIncrement() - sets or gets the block size used to expand regions.
    mdxLength() - returns the length of a described memory region.
    mdxOwn() - takes ownership of a described memory region.
    mdxRegionP() - returns a pointer to a described memory region.
    mdxSetLength() - sets the length of a described memory region.

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
#include  "mdx_util.h"			/* Memory Descriptor utilities. */


/*******************************************************************************
    Memory Descriptor Data Structure.
*******************************************************************************/

#define  MAX_MDX_INCREMENT  512

typedef  struct  _MemoryDx {
    MdxStorageClass  allocation ;	/* Allocation type. */
    size_t  alignment ;			/* Byte boundary for alignment. */
    size_t  elementSize ;		/* Element size for array; does not
					   include trailing alignment bytes. */
    size_t  increment ;			/* Buffer allocation block size. */
    bool  owned ;			/* Owned by application?  If true,
					   mdxDestroy() or mdxErase() will
					   NOT free a dynamically allocated
					   region; the application will.
					   If false, the destroy and/or erase
					   functions will free the region. */
    size_t  actLength ;			/* Actual byte-length of region. */
    size_t  maxLength ;			/* Byte-length of allocated buffer. */
    char  *buffer ;			/* Buffer holding the region. */
}  _MemoryDx ;


int  mdx_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  mdx_util_debug

/*!*****************************************************************************

Procedure:

    mdxAddB ()

    Add a Byte to a Described Memory Region.


Purpose:

    Function mdxAddB() appends a byte to a described memory region.
    If necessary, the size of the described region is increased to
    hold the new byte.

        NOTE that the byte is added directly after the last used
        byte in the buffer -- there is no padding inserted for
        alignment.


    Invocation:

        status = mdxAddB (descriptor, byte) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <byte>		- I
            is the byte to add to the region.
        <status>	- O
            returns the status of appending the byte to the region,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxAddB (

#    if PROTOTYPES
        MemoryDx  descriptor,
        char  byte)
#    else
        descriptor, byte)

        MemoryDx  descriptor ;
        char  byte ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxAddB) NULL descriptor handle.\n") ;
        return (errno) ;
    }

/* Make sure the buffer can hold an added byte.  The call to mdxGrow() also
   checks for a NULL descriptor handle and a static region, both errors as
   far as mdxAddB() is concerned.  If the descriptor is empty, mdxGrow()
   allocates the initial memory buffer. */

    if (mdxGrow (descriptor, 1)) {
        LGE "(mdxAddB) Error growing buffer.\nmdxGrow: ") ;
        return (errno) ;
    }

/* Append the byte to the memory region. */

    descriptor->buffer[descriptor->actLength++] = byte ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxAddS ()

    Add a Sequence of Bytes to a Described Memory Region.


Purpose:

    Function mdxAddS() appends a sequence of bytes to a described memory
    region.  If requested, padding is inserted to start the sequence at
    the next byte boundary following the last used byte in the buffer.
    If necessary, the size of the buffer is increased to hold the new data.

    The function will act as expected for normal cases.  However, there
    are some edge cases that receive special handling and which may prove
    useful to an application.  In each of these edge cases, the sequence
    of data is NULL:

        align=false	data=NULL	length=0
            No effect and no error reported.

        align=true	data=NULL	length=0
            The alignment padding that would be inserted before the next
            sequence starts is added to the current data in the buffer.
            More simply, the current data expands to the next alignment
            boundary.  (Unless the current data is already at the
            alignment boundary.)  This capability could be useful if
            the application is writing out a record or records whose
            length must be a multiple of the alignment byte boundary.

        align=false	data=NULL	length>0
            The specified number of zero bytes are added directly
            after the current data in the buffer.

        align=true	data=NULL	length>0
            After alignment padding, the specified number of zero bytes
            are added directly after the current data in the buffer.


    Invocation:

        status = mdxAddS (descriptor, align, data, length) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <align>		- I
            specifies whether or not padding should be added to the
            memory buffer so that the new data sequence begins on
            an alignment byte boundary.
        <data>		- I
            is a sequence of bytes to be added to the described memory.
        <length>	- I
            is the number of bytes to add.  The actual length of the
            described memory region may increase by more than this
            number if padding is required to align the sequence start.
        <status>	- O
            returns the status of appending the sequence in the buffer,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxAddS (

#    if PROTOTYPES
        MemoryDx  descriptor,
        bool  align,
        const  void  *data,
        size_t  length)
#    else
        descriptor, align, data, length)

        MemoryDx  descriptor ;
        bool  align ;
        const  void  *data ;
        size_t  length ;
#    endif

{    /* Local variables. */
    size_t  padding, units ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxAddB) NULL descriptor handle.\n") ;
        return (errno) ;
    }

/* If the start of the sequence is to be aligned, then compute the number
   of bytes of padding to insert and then insert them. */

    units = (descriptor->actLength + descriptor->alignment - 1)
            / descriptor->alignment ;		/* # of alignment units. */
    padding = (units * descriptor->alignment)
              - descriptor->actLength ;		/* # of padding bytes. */

    while (align && (padding-- > 0)) {		/* Insert the padding or not. */
        if (mdxAddB (descriptor, 0)) {
            LGE "(mdxAddS) Error adding alignment padding.\nmdxAddB: ") ;
            return (errno) ;
        }
    }

    if (length == 0)  return (0) ;		/* Nothing to add? */

/* Make sure the buffer can hold the added data.  The call to mdxGrow() also
   checks for a NULL descriptor handle and a static region, both errors as far
   as mdxAddS() is concerned.  If the descriptor is empty, mdxGrow() allocates
   the initial memory buffer. */

    if (mdxGrow (descriptor, length)) {
        LGE "(mdxAddS) Error growing buffer.\nmdxGrow: ") ;
        return (errno) ;
    }

/* Convert the data length from number of elements to number of bytes
   before copying the data into the buffer. */

    length *= descriptor->elementSize ;

/* Append the new data to the existing data. */

    if (data == NULL)
        memset (&descriptor->buffer[descriptor->actLength], 0, length) ;
    else
        memcpy (&descriptor->buffer[descriptor->actLength], data, length) ;
    descriptor->actLength += length ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxAlignment ()

    Get/Set the Alignment Byte Boundary for a Described Memory Region.


Purpose:

    Function mdxAlignment() gets/sets the byte boundary used by mdxAddS() to
    align a new sequence of bytes, depending on mdxAddS()'s ALIGN argument.

    The default alignment is "sizeof (double)", set when a memory descriptor
    is created.

    To get the current alignment value, pass -1 into mdxAlignment():

        size_t  byteBoundary = mdxAlignment (desc, -1) ;

    To set a new alignment value, pass in a boundary >= 0.  For example,
    let's change the alignment to 5-byte boundaries (odd, in more ways
    than one!):

        size_t  oldBoundary = mdxAlignment (desc, 5) ;

    0 and 1 are treated as 1-byte boundaries; i.e., no alignment.
    When mdxAddS() adds a new sequence of data, it will begin
    immediately after the last byte used (no padding).

    The function returns the old alignment value, thus enabling the
    application to change the alignment temporarily and then restore
    the old value.

        NOTE that MDX_UTIL's byte alignment is relative to the
        beginning of the memory buffer and has no relation to
        the absolute address of where data will be stored in
        the memory buffer.


    Invocation:

        oldBoundary = mdxAlignment (descriptor, newBoundary) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <newBoundary>	- I
            specifies a new byte boundary for added data.  If this
            argument is -1, the current byte boundary is not changed
            and is simply returned to the caller.  If this argument
            is positive, it becomes the new byte boundary and the
            old alignment is returned as this function's value.
            Byte boundaries 0 and 1 are both treated as alignment
            on 1-byte boundaries; i.e., no alignment and no padding.
        <oldBoundary>	- O
            returns the descriptor's current or old alignment value,
            depending on the boundary input argument.  If the input
            argument is -1, the current byte boundary, unchanged, is
            returned to the caller.  If the argument is >= 0, the
            new byte boundary replaces the current byte boundary,
            which is returned as the old byte boundary.

*******************************************************************************/


size_t  mdxAlignment (

#    if PROTOTYPES
        MemoryDx  descriptor,
        ssize_t  newBoundary)
#    else
        descriptor, newBoundary)

        MemoryDx  descriptor ;
        ssize_t  newBoundary ;
#    endif

{    /* Local variables. */
    size_t  oldBoundary ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxAlignment) NULL descriptor handle.\n") ;
        return (0) ;
    }

    oldBoundary = descriptor->alignment ;

    if (newBoundary >= 0) {	/* Caller is setting a new byte boundary. */
        if (newBoundary == 0)  newBoundary = 1 ;
        descriptor->alignment = newBoundary ;
    }

    return (oldBoundary) ;

}

/*!*****************************************************************************

Procedure:

    mdxCopy ()

    Copy One Memory Descriptor to Another Memory Descriptor.


Purpose:

    Function mdxCopy() copies one memory descriptor to another,
    erasing the destination descriptor's prior contents.


    Invocation:

        status = mdxCopy (source, isStatic, destination) ;

    where

        <source>	- I
            is the handle returned by mdxCreate() for the descriptor to be
            copied.
        <isStatic>	- I
            specifies whether (true) or not (false) the destination
            descriptor will have a static pointer to the source descriptor's
            memory buffer, regardless of the source descriptor's allocation
            class.  If this argument is true, the destination descriptor will
            have an allocation type of static and its buffer pointer will be
            identical to the source descriptor's pointer, which may have an
            allocation type of MdxStatic or MdxDynamic.  NOTE that if the
            source descriptor's allocation type is MdxDynamic, its memory
            buffer may be free(3)'d without the destination descriptor being
            any the wiser.  Use this capability carefully.
        <destination>	- O
            is the destination memory descriptor to which the source
            descriptor is copied.  The prior contents of the destination
            descriptor are erased before the copy.
        <status>	- O
            returns the status of copying the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxCopy (

#    if PROTOTYPES
        MemoryDx  source,
        bool  isStatic,
        MemoryDx  destination)
#    else
        source, isStatic, destination)

        MemoryDx  source ;
        bool  isStatic ;
        MemoryDx  destination ;
#    endif

{

    if ((source == NULL) || (destination == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxCopy) NULL source and/or destination descriptors.\n") ;
        return (errno) ;
    }

    LGI "(mdxCopy) Copying descriptor %p to descriptor %p ...\n",
        (void *) source, (void *) destination) ;

/* Erase the contents of the destination descriptor and inherit some of
   the source descriptors "array"ness. */

    mdxErase (destination) ;

    destination->alignment = source->alignment ;
    destination->elementSize = source->elementSize ;
    destination->increment = source->increment ;

/* If the is-static? argument is true, the destination descriptor will have
   a "static" pointer to the source descriptor's memory buffer, whether or
   not the source's buffer was statically or dynamically allocated. */

    if (isStatic) {
        *destination = *source ;
        destination->allocation = MdxStatic ;
        return (0) ;
    }

/* Otherwise, make a dynamic copy of the source descriptor's memory buffer. */

    destination->allocation = MdxDynamic ;
    destination->owned = false ;
    destination->actLength = 0 ;
    destination->maxLength = 0 ;
    destination->buffer = NULL ;

    if (source->buffer != NULL) {

        destination->buffer = memdup (source->buffer, source->maxLength) ;
        if (destination->buffer == NULL) {
            LGE "(mdxCopy) Error duplicating source region: \"%s\"\nmemdup: ",
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

    mdxCreate ()

    Create a Memory Descriptor.


Purpose:

    Function mdxCreate() creates and initializes a descriptor for a memory
    region (or for a region yet to be defined).

    If the described memory region is intended as an array of homogeneous
    structures, the following calls should be made as necessary after the
    call to mdxCreate():

        mdxElementSize() - sets the size in bytes of each array element.
        mdxAlign() - specifies the byte-boundary alignment for each element.
            The default alignment if this function is not called is "sizeof
            (double)".  If the caller desires a narrower byte boundary, it
            should call mdxAlign().  If the caller desires no byte-boundary
            padding between elements, mdxAlign() can be called with a value
            of 1 (or 0 if you prefer).
        mdxIncrement() - sets the number of element- plus alignment-sized
            blocks to allocate in one fell swoop when expanding the internal
            memory buffer.  For example, if elements are 34-byte structures
            on "double" (8-byte) boundaries, the block size is
                       (34 + (8 - 1))/8 * 8 bytes =
                             41/8 * 8 bytes =
                               5 * 8 bytes =
                                 40 bytes.
            With an increment of 128, say, then mdxGrow() will expand the
            internal buffer by 128*40 = 5,120 bytes each time the addition
            of a single element would overflow the existing buffer.


    Invocation:

        status = mdxCreate (region, length, allocation, &descriptor) ;

    where

        <region>	- I
            is a memory region to be described by the descriptor.  If this
            argument is NULL, then an empty descriptor is created.
        <length>	- I
            is the initial length in bytes of the input region; this argument
            is ignored if no initial region is supplied.
        <allocation>	- I
            specifies the allocation type of the input memory region:
                "static" - is a fixed region.
                "dynamic" - is a dynamically allocated region.  The descriptor
                    takes "ownership" of the region, which will be free()ed
                    when the descriptor is destroyed.
                "volatile" - is a region in flux.  A dynamic copy of the
                    region is made and stored in the descriptor; the copy
                    is annotated as "dynamic".
        <descriptor>	- O
            returns a handle for the memory descriptor.
        <status>	- O
            returns the status of creating a descriptor, zero if no errors
            occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxCreate (

#    if PROTOTYPES
        void  *region,
        size_t  length,
        MdxStorageClass  allocation,
        MemoryDx  *descriptor)
#    else
        region, length, allocation, descriptor)

        void  *region ;
        size_t  length ;
        MdxStorageClass  allocation ;
        MemoryDx  *descriptor ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxCreate) NULL descriptor handle pointer.\n") ;
        return (errno) ;
    }

/* Create and initialize the descriptor. */

    *descriptor = (MemoryDx) malloc (sizeof (_MemoryDx)) ;
    if (*descriptor == NULL) {
        LGE "(mdxCreate) Error allocating memory descriptor.\nmalloc: ") ;
        return (errno) ;
    }

    (*descriptor)->allocation = MdxDynamic ;
    (*descriptor)->alignment = sizeof (double) ;
    (*descriptor)->elementSize = 1 ;
    (*descriptor)->increment = MAX_MDX_INCREMENT * (*descriptor)->elementSize ;
    (*descriptor)->owned = false ;
    (*descriptor)->actLength = 0 ;
    (*descriptor)->maxLength = 0 ;
    (*descriptor)->buffer = NULL ;

    if (region == NULL) {
		/* Descriptor was correctly initialized. */
    } else if (allocation == MdxVolatile) {
        (*descriptor)->allocation = MdxDynamic ;
        if (mdxAddS (*descriptor, false, region, length)) {
            LGE "(mdxCreate) Error duplicating %"PRIuSIZE_T"-byte memory region %p.\nmdxAddS: ",
                length, region) ;
            PUSH_ERRNO ;  mdxDestroy (*descriptor) ;
            *descriptor = NULL ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {			/* MdxStatic or MdxDynamic */
        (*descriptor)->allocation = allocation ;
        (*descriptor)->buffer = region ;
        (*descriptor)->actLength = length ;
        (*descriptor)->maxLength = length ;
    }

    if (region == NULL) {
        LGI "(mdxCreate) Created memory descriptor %p for NULL region.\n",
            (void *) *descriptor) ;
    } else {
        LGI "(mdxCreate) Created memory descriptor %p for %"PRIuSIZE_T"-byte region %p.\n",
            (void *) *descriptor, length, (*descriptor)->buffer) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxDestroy ()

    Destroy a Memory Descriptor.


Purpose:

    Function mdxDestroy() destroys a previously created memory descriptor.


    Invocation:

        status = mdxDestroy (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <status>	- O
            returns the status of destroying the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxDestroy (

#    if PROTOTYPES
        MemoryDx  descriptor)
#    else
        descriptor)

        MemoryDx  descriptor ;
#    endif

{

    if (descriptor == NULL)  return (0) ;

    LGI "(mdxDestroy) Destroying memory descriptor %p ...\n",
        (void *) descriptor) ;

    mdxErase (descriptor) ;

    free (descriptor) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxDuplicate ()

    Duplicate a Memory Descriptor.


Purpose:

    Function mdxDuplicate() duplicates a memory descriptor.  The memory
    buffer in the duplicate descriptor is always a dynamically allocated
    copy of the source descriptor's buffer.  In particular, this means
    that if a source descriptor points to a statically allocated region,
    any duplicate descriptors point not to the original static copy of
    the region, but to dynamic copies of the region.


    Invocation:

        newDescriptor = mdxDuplicate (oldDescriptor) ;

    where

        <oldDescriptor>	- I
            is a descriptor handle returned by mdxCreate().
        <newDescriptor>	- O
            returns the handle for the duplicate descriptor; NULL is returned
            in the event of an error.

*******************************************************************************/


MemoryDx  mdxDuplicate (

#    if PROTOTYPES
        MemoryDx  oldDescriptor)
#    else
        oldDescriptor)

        MemoryDx  oldDescriptor ;
#    endif

{    /* Local variables. */
    MemoryDx  newDescriptor ;



    if (oldDescriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxDuplicate) NULL descriptor handle.\n") ;
        return (NULL) ;
    }

    if (mdxCreate (oldDescriptor->buffer, oldDescriptor->actLength,
                   MdxVolatile, &newDescriptor)) {
        LGE "(mdxDuplicate) Error creating duplicate descriptor.\nmdxCreate: ") ;
        return (NULL) ;
    }

    newDescriptor->alignment = oldDescriptor->alignment ;
    newDescriptor->elementSize = oldDescriptor->elementSize ;
    newDescriptor->increment = oldDescriptor->increment ;

    LGI "(mdxDuplicate) Created duplicate descriptor %p for region %p.\n",
        (void *) newDescriptor, newDescriptor->buffer) ;

    return (newDescriptor) ;

}

/*!*****************************************************************************

Procedure:

    mdxElementSize ()

    Get/Set the Element Size for a Described Memory Region.


Purpose:

    Function mdxElementSize() gets/sets the "element size" in a memory
    region.  This concept allows mdxGrow() and mdxRegionP() to treat
    the memory buffer as an array of fixed-size BLOBs (binary large
    objects) or, unofficially, of BOBs (smaller objects!).

    The default element size, set when a memory descriptor is created,
    is 1; i.e., the memory buffer is just a sequence of bytes.

    To get the current element size, pass -1 into mdxElementSize():

        size_t  elementSize = mdxElementSize (desc, -1) ;

    To set a new element size, pass in a size >= 0.  For an array of
    2,000-byte elements, for example:

        size_t  oldSize = mdxElementSize (desc, 2000) ;

    0 and 1 are treated as one-byte elements -- a simple sequence of bytes.

    In practical terms, mdxGrow() multiplies the increment count by the
    element size to determine how many additional bytes need to be added
    when the memory buffer must be expanded.  There is no functional
    problem with specifying the element size AFTER the array is built,
    but doing so beforehand reduces the number of times mdxGrow() must
    reallocate the memory buffer.

    On the other hand, it may be the case that an application does not
    know the element size until at least one element has been created
    in the memory buffer.  In particular, alignment padding within an
    element and after the element should be considered in dividing up
    a memory buffer into elements.  The element size should be chosen
    so that all of an array's elements are aligned correctly and so
    that mdxRegionP() can index into the memory buffer to produce the
    correct pointer to any arbitrary element.


    Invocation:

        oldSize = mdxElementSize (descriptor, newSize) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <newSize>	- I
            specifies the size of elements in an imaginary array in the
            memory buffer.  If this argument is -1, the current element
            size is not changed and is simply returned to the caller.
            If this argument is positive, it becomes the new element
            size and the old size is returned as this function's value.
            Element sizes 0 and 1 are both treated as signifying an
            array of one-byte elements, a simple sequence of bytes.
        <oldSize>	- O
            returns the descriptor's current or old element size,
            depending on the element size input argument.  If the
            input argument is -1, the current size, unchanged, is
            returned to the caller.  If the argument is >= 0, the
            new size replaces the current size, which is returned
            as the old element size.

*******************************************************************************/


size_t  mdxElementSize (

#    if PROTOTYPES
        MemoryDx  descriptor,
        ssize_t  newSize)
#    else
        descriptor, newSize)

        MemoryDx  descriptor ;
        ssize_t  newSize ;
#    endif

{    /* Local variables. */
    size_t  oldSize ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxElementSize) NULL descriptor handle.\n") ;
        return (0) ;
    }

    oldSize = descriptor->elementSize ;

    if (newSize >= 0) {		/* Caller is setting a new element size. */
        if (newSize == 0)  newSize = 1 ;
        descriptor->elementSize = newSize ;
    }

    return (oldSize) ;

}

/*!*****************************************************************************

Procedure:

    mdxErase ()

    Erase the Contents of a Memory Descriptor.


Purpose:

    Function mdxErase() erases the contents of a previously created memory
    descriptor, leaving an empty memory descriptor.


    Invocation:

        status = mdxErase (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <status>	- O
            returns the status of erasing the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxErase (

#    if PROTOTYPES
        MemoryDx  descriptor)
#    else
        descriptor)

        MemoryDx  descriptor ;
#    endif

{

    if (descriptor == NULL)  return (0) ;

    LGI "(mdxErase) Erasing memory descriptor %p ...\n",
        (void *) descriptor) ;

    if ((descriptor->allocation == MdxDynamic) &&
        !descriptor->owned &&
        (descriptor->buffer != NULL)) {
        free (descriptor->buffer) ;
    }

    descriptor->allocation = MdxVolatile ;
    descriptor->actLength = 0 ;
    descriptor->maxLength = 0 ;
    descriptor->buffer = NULL ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxGrow ()

    Increase the Size of a Descriptor's Memory Buffer.


Purpose:

    Function mdxGrow() expands the size of a memory descriptor's buffer
    so that more data can be added.  NOTE that mdxGrow() grows the maximum
    size of the internal buffer, NOT the actual length of the existing
    region in the buffer (which won't change until the caller actually
    adds data to the region.)

    If the buffer is already large enough for the requested number of bytes
    (or elements) to be added, mdxGrow() returns silently without error.
    In other words, the caller can call mdxGrow() everytime data is to be
    added to the region and mdxGrow() will only expand the size of the
    buffer when necessary.


    Invocation:

        status = mdxGrow (descriptor, toAdd) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <toAdd>		- I
            is the number of bytes or elements the caller wishes to add
            to the memory buffer.  This argument is multiplied by the
            descriptor's element size to get the total number of bytes
            to be added; if the element size is one byte, this argument
            will be the total number of bytes.  mdxGrow() will increase
            the size of the buffer to hold the existing data plus the
            new data to be added.
        <status>	- O
            returns the status of expanding the buffer, zero if there
            were no errors and ERRNO otherwise.  Zero is also returned
            if the buffer is already large enough to hold the new text
            being added.

*******************************************************************************/


errno_t  mdxGrow (

#    if PROTOTYPES
        MemoryDx  descriptor,
        size_t  toAdd)
#    else
        descriptor, toAdd)

        MemoryDx  descriptor ;
        size_t  toAdd ;
#    endif

{    /* Local variables. */
    char  *newBuffer ;
    size_t  blockSize, length, numBlocks ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxGrow) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    if (descriptor->allocation == MdxStatic) {
        SET_ERRNO (EPERM) ;
        LGE "(mdxGrow) Cannot change static string.\n") ;
        return (errno) ;
    }

    LGI "(mdxGrow) Dx %p  Actual: %"PRIuSIZE_T"  Max: %"PRIuSIZE_T"  #Add: %"PRIuSIZE_T" * Element#: %"PRIuSIZE_T"\n",
        (void *) descriptor, descriptor->actLength,
        descriptor->maxLength, toAdd, descriptor->elementSize) ;

/* Compute the new length in bytes of the descriptor's memory buffer. */

    length = descriptor->actLength + (toAdd * descriptor->elementSize) ;
    if (length < descriptor->maxLength) {	/* Already have enough space? */
        return (0) ;
    }

/* The memory buffer is extended in blocks whose sizes are the descriptor's
   increment times its element size, so round the length computed above up
   to the next multiple of the block size. */

    blockSize = descriptor->increment * descriptor->elementSize ;
    numBlocks = (length + blockSize - 1) / blockSize ;
    length = numBlocks * blockSize ;

/* If this is the first allocation for an empty descriptor, then do so. */

    if (descriptor->buffer == NULL) {
        descriptor->allocation = MdxDynamic ;
        newBuffer = malloc (length) ;
    }

/* Otherwise, increase the size of the buffer to hold the new string. */

    else {
        newBuffer = realloc (descriptor->buffer, length) ;
    }

    if (newBuffer == NULL) {
        PUSH_ERRNO ;
        LGE "(mdxGrow) Error (re)allocating %"PRIuSIZE_T"-byte buffer.\nmalloc/realloc: ",
            length) ;
        POP_ERRNO ;
        return (errno) ;
    }

    LGI "(mdxGrow) Dx %p  Old: %p  New: %p  Max: %"PRIuSIZE_T"\n",
        (void *) descriptor, descriptor->buffer, newBuffer, length) ;

    memset (&newBuffer[descriptor->actLength], 0,
            length - descriptor->actLength) ;
    descriptor->buffer = newBuffer ;
    descriptor->maxLength = length ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxIncrement ()

    Set/Get the Allocation Increment for a Described Memory Region.


Purpose:

    Function mdxIncrement() sets/gets the block size used by mdxGrow()
    for a specific described memory region.  When data is added to a
    dynamic region and mdxGrow() needs to expand the buffer to hold
    it, the new buffer size is the total length of the region being
    constructed rounded up to the next highest multiple of the block
    size.

    The default block size is MAX_MDX_INCREMENT, 512.  Increasing the
    block size reduces how frequently mdxGrow() has to reallocate the
    buffer, a good idea when a large region is growing rapidly.

    The new block size does not take effect until the next time
    mdxGrow() has to reallocate the buffer.

    If the calling application specifies a block size of -1,
    mdxIncrement() simply returns the current increment.


    Invocation:

        increment = mdxIncrement (descriptor, blockSize) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <blockSize>	- I
            specifies the new block size in bytes.  If the block size
            is -1 or 0, mdxIncrement() simply returns the current block
            size.  Otherwise, this new block size will replace the current
            block size.
        <increment>	- O
            returns the descriptor's current or new increment, depending
            on the block size input argument.  If the input argument is
            -1 or 0, the caller is "GETing" the descriptor's existing
            block size and that is what will be returned.  If the input
            argument is >= 1, the caller is "SETing" the descriptor's
            increment to the new value and that value will be returned
            to the caller.

*******************************************************************************/


size_t  mdxIncrement (

#    if PROTOTYPES
        MemoryDx  descriptor,
        ssize_t  blockSize)
#    else
        descriptor, blockSize)

        MemoryDx  descriptor ;
        ssize_t  blockSize ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxIncrement) NULL descriptor handle.\n") ;
        return (0) ;
    }

    if (blockSize >= 1)		/* Caller is setting a new block size. */
        descriptor->increment = blockSize ;

    return (descriptor->increment) ;

}

/*!*****************************************************************************

Procedure:

    mdxLength ()

    Get the Length of a Described Memory Region.


Purpose:

    Function mdxLength() returns the number of elements in the memory region
    to which a descriptor points.  The number of elements is the length in
    bytes of the memory region divided by the element size.


    Invocation:

        numElements = mdxLength (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <numElements>	- O
            returns the number of elements (#bytes/elementSize) in the
            memory region.

*******************************************************************************/


size_t  mdxLength (

#    if PROTOTYPES
        MemoryDx  descriptor)
#    else
        descriptor)

        MemoryDx  descriptor ;
#    endif

{

    if (descriptor == NULL)
        return (0) ;
    else
        return (descriptor->actLength / descriptor->elementSize) ;

}

/*!*****************************************************************************

Procedure:

    mdxOwn ()

    Take Ownership of the Described Memory Region.


Purpose:

    Function mdxOwn() grants ownership of a dynamically allocated region
    to the application.  Normally, a call to mdxDestroy() or mdxErase()
    will free a dynamically allocated region.  However, if the application
    has taken possession of the region, then the application is responsible
    for freeing the region after the memory descriptor is destroyed or
    erased.  This is typically done when the application is dynamically
    constructing a memory region using the MDX_UTIL functions and the
    final result must exist independently (i.e., past the lifetime) of
    the descriptor.  The application must free(3) the region when it is
    no longer needed.

        NOTE that an application can take ownership of a region at any
        time.  The descriptor's region will continue to grow as needed,
        which may require reallocating the region to hold more bytes.
        When this happens, a new, larger region buffer is allocated,
        the old region is copied to the new buffer, and the old region
        is freed.  Consequently, in order to get the correct region to
        free, the application should make a final call to mdxRegionP()
        right before calling mdxDestroy() or mdxErase().  (Unless, of
        course, the application knows that it hasn't added anything to
        the described region since it last obtained the address.)


    Invocation:

        status = mdxOwn (descriptor) ;

    where

        <descriptor>	- I
            is a descriptor handle returned by mdxCreate().
        <status>	- O
            returns the status of taking ownership of the region,
            zero on success and ERRNO otherwise.  It is not an
            error to take ownership of a static region; doing so
            has no effect, although the application must be sure
            not to attempt freeing the static region.

*******************************************************************************/


errno_t  mdxOwn (

#    if PROTOTYPES
        MemoryDx  descriptor)
#    else
        descriptor)

        MemoryDx  descriptor ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxOwn) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    descriptor->owned = true ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    mdxRegionP ()

    Get a Pointer to a Described Memory Region.


Purpose:

    Function mdxRegionP() returns a pointer to the actual memory handled
    by the memory descriptor, i.e., the descriptor's internal buffer.
    If the memory is being treated as an array of fixed-size elements,
    the index argument to mdxRegionP() can be used to get the location
    of specific elements in the array.

        NOTE:  In the case of dynamically allocated memory regions,
        be aware that adding bytes or sequences of bytes to a region
        may require sdxGrow() to reallocate the memory region, thus
        very possibly invalidating any prior mdxRegionP() pointers.

        NOTE also that, for the same reason, if an application has
        mdxOwn()ed a dynamically allocated region, the application
        should consider calling mdxRegionP() immediately before
        calling mdxDestroy() to ensure it has the latest pointer.


    Invocation:

        region = mdxRegionP (descriptor, index) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <index>		- I
            is the index, 0..N-1, of the desired element to which
            the returned pointer points.  The index is multiplied
            by the descriptor's element size (which defaults to
            one byte) and the product is added to the base address
            of the entire memory region.  An index of 0 always
            results in the base address being returned.
        <region>	- O
            returns a pointer to the memory region (or element therein).
            A NULL pointer is returned in the event of an error (such as
            an empty descriptor).

*******************************************************************************/


void  *mdxRegionP (

#    if PROTOTYPES
        MemoryDx  descriptor,
        size_t  index)
#    else
        descriptor, index)

        MemoryDx  descriptor ;
        size_t  index ;
#    endif

{    /* Local variables. */
    size_t  offset ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxRegionP) NULL descriptor handle.\n") ;
        return (NULL) ;
    }

    offset = index * descriptor->elementSize ;
    if (offset >= descriptor->actLength) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxRegionP) Index %"PRIuSIZE_T"*%"PRIuSIZE_T" exceeds actual length %"PRIuSIZE_T".\n",
            index, descriptor->elementSize, descriptor->actLength) ;
        return (NULL) ;
    }

    return ((void *) &descriptor->buffer[offset]) ;

}

/*!*****************************************************************************

Procedure:

    mdxSetLength ()

    Set the Length of a Described Region.


Purpose:

    Function mdxSetLength() modifies the length of the memory region
    to which a descriptor points.  This ability is provided in case
    the caller has modified or wishes to modify the described memory,
    independently of the descriptor, in such a way as to change its
    length.

    If the caller-specified size is greater than the maximum size of
    the memory buffer, the buffer is expanded with zeroed bytes to
    contain the desired size.

        NOTE that the LENGTH argument is in units of element size.
        If the application has not modified the default element
        size of 1 byte via mdxElementSize(), then LENGTH will be
        the new length in bytes.  Otherwise, LENGTH will be
        multiplied by the element size to yield the new length
        in bytes.


    Invocation:

        status = mdxSetLength (descriptor, length) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by mdxCreate().
        <length>	- I
            is the new length in units of the described memory's element
            size.  This argument is multiplied by the element size to get
            the length in bytes.  (The in-use and maximum lengths of the
            memory buffer are maintained in units of bytes.)
        <status>	- O
            returns the status of changing the length, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  mdxSetLength (

#    if PROTOTYPES
        MemoryDx  descriptor,
        size_t  length)
#    else
        descriptor, length)

        MemoryDx  descriptor ;
        size_t  length ;
#    endif

{    /* Local variables. */
    size_t  newBeginning, numBytes ;



    if ((descriptor == NULL) || (descriptor->buffer == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(mdxSetLength) NULL descriptor handle or buffer.\n") ;
        return (errno) ;
    }

/* If the new length in bytes is greater than the size of the currently
   allocated memory buffer, then reallocate the buffer to the larger size.
   NOTE: It is assumed that the maximum length field in the descriptor
   is evenly divisible by the element size. */

    numBytes = length * descriptor->elementSize ;
    if ((numBytes > descriptor->maxLength) &&
        mdxGrow (descriptor,
                 length - (descriptor->maxLength / descriptor->elementSize))) {
        LGE "(mdxSetLength) Error increasing region size to %"PRIuSIZE_T" bytes.\nmdxGrow: ",
            numBytes) ;
        return (errno) ;
    }

/* Zero out the added or deleted bytes in the memory buffer.  If the region's
   size decreased (new length < old length), then zero out the bytes following
   the new length; no need to, but I'll do it anyway.  If the region's size
   increased (old length < new length), then zero out the bytes following the
   old length; this is done because the bytes between the old length and new
   length are now in-use, so they should be initialized to zero.  I'm lazy
   so I zero out bytes to the maximum length of the memory buffer, which
   duplicates the efforts of mdxGrow(). */

    newBeginning = (numBytes < descriptor->actLength)
                   ? numBytes				/* New length. */
                   : descriptor->actLength ;		/* Old length. */
    memset (&descriptor->buffer[newBeginning], 0,
            descriptor->maxLength - newBeginning) ;

/* Now, set the actual length of the buffer to the new length in bytes. */

    descriptor->actLength = numBytes ;

    return (0) ;

}
