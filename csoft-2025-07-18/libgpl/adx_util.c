/* $Id: adx_util.c,v 1.1 2022/03/14 05:34:18 alex Exp alex $ */
/*******************************************************************************

File:

    adx_util.c

    Array Descriptor (ADX) Utilities


Author:    Alex Measday


Purpose:

    The ADX_UTIL package provides functions for creating and manipulating
    dynamically sized arrays of arbitrary, homogeneous (fixed-size) elements.

    This package was inspired by my MDX_UTIL memory region descriptor package,
    which, in fact, did include the ability to treat a memory region as an
    array.  However, the mechanisms for working with memory regions as arrays
    were awkward for the application using the package.  Hopefully, ADX_UTIL
    improves on that front.

    The following example reads a complete text file into an array of SDX_UTIL
    string descriptors, one element per line, a convenient framework for a
    simple text editor.  NOTE that the array is composed of pointers (StringDx)
    to string descriptors (_StringDx), not an array of string descriptors
    themselves.

        FILE  *file = fopen ("example.txt", "r") ;
        ssize_t  count ;
        ArrayDx  adx ;
        StringDx  *element41, sdx ;

        adxCreate (0, sizeof (StringDx), NULL, 0, AdxDynamic, &adx) ;

        for ( ; ; ) {			-- Break on EOF or error.
            sdxCreate (NULL, -1, SdxDynamic, &sdx) ;
            if (sdxReadLine (file, sdx, &count) || (count < 1))  break ;
            adxAdd (adx, (const void *) &sdx, 1, -1) ;
        }
        fclose (file) ;

    Then, access line 20.  Note the distinction between line numbers, 1..N,
    and their corresponding array indices, 0..N-1 -- hence all the little
    "-1"s in the few lines of code below; e.g., line 20 is at index 20-1 = 19.

        printf ("Line 20: \"%s\"\n",
                sdxStringZ (*((StringDx) adxGet (adx, 20-1)))) ;

    Next, insert a line between line 30 and line 31.

        sdxCreate ("I'm the new line 31!!!", -1, SdxVolatile, &sdx) ;
        adxAdd (adx, (const void *) &sdx, 1, 31-1) ;

    Now, delete lines 41-45.  Since the elements have dynamically allocated
    resources of their own, it is necessary to destroy or save the elements
    being deleted before actually deleting them from the array.

        element41 = (StringDx *) adxGet (adx, 41-1) ;
					-- Destroy descriptors being deleted
        for (i = 0 ;  i < 5 ;  i++)	-- (or save them in cut/undo buffer).
            sdxDestroy (element41[i]) ;
        adxDelete (adx, 5, 41-1) ;	-- Delete the 5 elements.


Public Procedures:

    adxAdd() - adds one or more elements to a described array.
    adxCopy() - copy the contents of one array descriptor to another.
    adxCount() - gets the number of elements in a described array.
    adxCreate() - creates an array descriptor.
    adxDelete() - deletes one or more elements from a described array.
    adxDestroy() - destroys an array descriptor.
    adxDuplicate() - duplicates an array descriptor.
    adxElementSize() - gets the size in bytes of a described array's elements.
    adxErase() - erases the contents of an array descriptor.
    adxGet() - gets the address of an element in a described array.
    adxIncrement() - sets or gets the block size used to expand an array.
    adxOwn() - takes ownership of a described array.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "mdx_util.h"			/* Memory Descriptor utilities. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "adx_util.h"			/* Array Descriptor utilities. */


/*******************************************************************************
    Array Descriptor Data Structure.
*******************************************************************************/

#define  MAX_ADX_INCREMENT  128

typedef  struct  _ArrayDx {
    AdxStorageClass  allocation ;	/* Allocation type. */
    size_t  offset ;			/* Byte offset at beginning. */
    size_t  elementSize ;		/* # of bytes for each element. */
    size_t  increment ;			/* Buffer allocation block size
					   in units of elements. */
    bool  owned ;			/* Owned by application?  If true,
					   adxDestroy() or adxErase() will
					   NOT free a dynamically allocated
					   region; the application will.
					   If false, the destroy and/or erase
					   functions will free the region. */
    size_t  numElements ;		/* # of elements in the array. */
    MemoryDx  memory ;			/* Descriptor for memory buffer. */
}  _ArrayDx ;


int  adx_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  adx_util_debug

/*!*****************************************************************************

Procedure:

    adxAdd ()

    Add One or More Elements to a Described Array.


Purpose:

    Function adxAdd() adds/inserts one or more elements to/in a described
    array.  The elements are inserted before the current I-th element in
    the array.  Supplied by the caller, index I is in the range 0..N-1,
    where N is the current number of elements in the array.  The array's
    memory buffer is expanded to hold the added elements, if necessary,
    and existing elements I through N-1 are shifted to make room for the
    new elements.  Passing in an index of -1 results in adxAdd() appending
    the new element(s) to the end of the array (i.e., after the last element).


    Invocation:

        status = adxAdd (descriptor, elements, numElements, index) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <elements>	- I
            are the element(s) to be added to the array.  This argument
            is a (void *) pointer to the contiguous sequence of new
            elements.  The contents of the new elements are copied to
            the descriptor's internal memory buffer, so the caller is
            responsible for doing whatever with the caller's copy of
            the new elements.
        <numElements>	- I
            is the number of elements being added/inserted.
        <index>		- I
            is the index 0..N-1, where N is the number of elements,
            of the existing element before which the new element(s)
            will be inserted.  An index of -1 is equivalent to N,
            the position after the last element; i.e., the new
            element(s) will be added at the end of the array.
        <status>	- O
            returns the status of adding/inserting the element(s),
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  adxAdd (

#    if PROTOTYPES
        ArrayDx  descriptor,
        const  void  *elements,
        size_t  numElements,
        ssize_t  index)
#    else
        descriptor, elements, numElements, index)

        ArrayDx  descriptor ;
        const  void  *elements ;
        size_t  numElements ;
        ssize_t  index ;
#    endif

{    /* Local variables. */
    size_t  newBytes, oldBytes ;
    void  *shiftTo, *target ;



/* Check the input arguments. */

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxAdd) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    if ((elements == NULL) || (numElements == 0)) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxAdd) No elements to add to descriptor %p.\n",
            (void *) descriptor) ;
        return (errno) ;
    }

    if (index < 0) {
        index = descriptor->numElements ;
    } else if ((size_t) index > descriptor->numElements) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxAdd) Index %"PRIdSSIZE_T" is outside the range -1..%"PRIuSIZE_T" in array %p.\n",
            index, descriptor->numElements, (void *) descriptor) ;
        return (errno) ;
    }

/* Ensure the internal memory buffer is large enough to add the elements. */

    newBytes = numElements * descriptor->elementSize ;

    if (mdxGrow (descriptor->memory, newBytes)) {
        LGE "(adxAdd) Error expanding memory region %p by %"PRIuSIZE_T" bytes for descriptor %p.\nmdxGrow: ",
            (void *) descriptor->memory, newBytes, (void *) descriptor) ;
        return (errno) ;
    }

/* Extend the actual length of the array in the internal memory buffer
   in preparation for copying the new elements into the array. */

    if (mdxAddS (descriptor->memory, false, NULL, newBytes)) {
        LGE "(adxAdd) Error extending descriptor %p's memory region %p's length by %"PRIuSIZE_T" bytes.\nmdxAddS: ",
            (void *) descriptor->memory, (void *) descriptor, newBytes) ;
        return (errno) ;
    }

/* If the new elements are being inserted at the beginning or in the middle
   of the array, then shift the trailing elements to make room for the new
   elements. */

    if ((size_t) index < descriptor->numElements) {

        target = adxGet (descriptor, index) ;
        oldBytes = (descriptor->numElements - index) * descriptor->elementSize ;

        shiftTo = (void *) ((char *) target + newBytes) ;

        memmove (shiftTo,	/* New location of old Ith to Nth elements. */
                 target,	/* Old location of old Ith to Nth elements. */
                 oldBytes) ;

    }

/* If the new elements are being appended to an array, then determine the
   target position for the first new element.  Note that the indication of
   appending elements is that the index is equal to the current number of
   array elements.  Therefore, adxGet() can't be used because that index
   is one position past the end of the array and adxGet() would complain.
   So compute the target location as the base of the memory region plus
   the size of the reserved area plus the size of the existing elements
   in the array. */

    else {

        target = (void *) ((char *) mdxRegionP (descriptor->memory, 0) +
                                    descriptor->offset +
                                    (index * descriptor->elementSize)) ;
    }

/* Copy the new elements into their target position in the array. */

    memmove (target, elements, newBytes) ;

    descriptor->numElements += numElements ;

    LGI "(adxAdd) Descriptor %p added %"PRIuSIZE_T" new elements at index %"PRIdSSIZE_T".\n",
        (void *) descriptor, numElements, index) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    adxCopy ()

    Copy One Array Descriptor to Another Array Descriptor.


Purpose:

    Function adxCopy() copies one array descriptor to another,
    erasing the destination descriptor's prior contents.

        NOTE:  If the function's "isStatic" argument is false,
        a dynamic copy of the source descriptor's memory region
        will be made.  This is NOT a deep copy.  The function
        has no knowledge of the data type of an array element,
        so the source element's bytes will be be "shallow copied"
        to the destination's new, dynamically allocated region.

        If an array element is a pointer to or has an internal
        pointer to a dynamically allocated, arbitrarily complex
        data structure, the pointers are left as is.  This means
        that the pointers point to the exact same things as the
        pointers in the source descriptor.  If an application
        needs deep copies of these internal data structures,
        the application will have to perform the deep copies.


    Invocation:

        status = adxCopy (source, isStatic, destination) ;

    where

        <source>	- I
            is the handle returned by adxCreate() for the descriptor to be
            copied.
        <isStatic>	- I
            specifies whether (true) or not (false) the destination descriptor
            will share the source descriptor's memory region, regardless of
            the source descriptor's allocation class.  If this argument is
            true, the destination descriptor will have an allocation type of
            AdxStatic and its internal memory descriptor will be identical to
            the source descriptor's descriptor, which may have an allocation
            type of AdxStatic or AdxDynamic.  NOTE that if the source
            descriptor's allocation type is AdxDynamic, its memory region
            may be free(3)'d without the destination descriptor being any
            the wiser.  Use this capability carefully.  If this argument is
            false, a dynamic copy of the source's memory region will be made.
        <destination>	- O
            is the destination array descriptor to which the source
            descriptor is copied.  The prior contents of the destination
            descriptor are erased before the copy.
        <status>	- O
            returns the status of copying the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  adxCopy (

#    if PROTOTYPES
        ArrayDx  source,
        bool  isStatic,
        ArrayDx  destination)
#    else
        source, isStatic, destination)

        ArrayDx  source ;
        bool  isStatic ;
        ArrayDx  destination ;
#    endif

{

    if ((source == NULL) || (destination == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxCopy) NULL source and/or destination descriptors.\n") ;
        return (errno) ;
    }

    LGI "(adxCopy) Copying descriptor %p to descriptor %p ...\n",
        (void *) source, (void *) destination) ;

/* Erase the contents of the destination descriptor and inherit some of
   the source descriptors "array"ness. */

    adxErase (destination) ;

    *destination = *source ;

/* If the is-static? argument is true, the destination descriptor will have
   a "static" pointer to the source descriptor's memory region, whether or
   not the source's region was statically or dynamically allocated. */

    if (isStatic) {
        destination->allocation = AdxStatic ;
        destination->owned = true ;		/* Source descriptor owns it. */
        LGI "(adxCopy) Copied array %p to array %p.\n",
            (void *) source, (void *) destination) ;
        return (0) ;
    }

/* Otherwise, make a dynamic copy of the source descriptor's memory region. */

    destination->memory = mdxDuplicate (source->memory) ;
    if (destination->memory == NULL) {
        LGE "(adxCopy) Error duplicating memory region for array %p.\nmdxDuplicate: ",
            (void *) destination) ;
        return (errno) ;
    }

    destination->owned = false ;

    LGI "(adxCopy) Copied array %p to array %p.\n",
        (void *) source, (void *) destination) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    adxCount ()

    Get the Number of Elements in a Described Array.


Purpose:

    Function adxCount() returns the current number of elements in the array
    to which a descriptor points.


    Invocation:

        numElements = adxCount (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <numElements>	- O
            returns the number of elements in the array.

*******************************************************************************/


size_t  adxCount (

#    if PROTOTYPES
        ArrayDx  descriptor)
#    else
        descriptor)

        ArrayDx  descriptor ;
#    endif

{    /* Local variables. */
    size_t  numBytes ;



    if ((descriptor == NULL) || (descriptor->memory == NULL))  return (0) ;

/* Perform a sanity check on the number of elements in the descriptor
   and the size of the internal memory region. */

    numBytes = descriptor->offset +
               (descriptor->elementSize * descriptor->numElements) ;
    if (numBytes != mdxLength (descriptor->memory)) {
        LGE "(adxCount) Mismatch Array DX %p O/S/E/#: %"PRIuSIZE_T"/%"PRIuSIZE_T"/%"PRIuSIZE_T"/%"PRIuSIZE_T" = %"PRIuSIZE_T"  Memory DX %p #: %"PRIuSIZE_T"\n",
            (void *) descriptor, descriptor->offset,
            descriptor->elementSize, descriptor->numElements,
            numBytes, (void *) descriptor->memory,
            mdxLength (descriptor->memory)) ;
        return (0) ;
    }

    return (descriptor->numElements) ;

}

/*!*****************************************************************************

Procedure:

    adxCreate ()

    Create an Array Descriptor.


Purpose:

    Function adxCreate() creates and initializes a descriptor for an array
    (or for an array yet to be defined).


    Invocation:

        status = adxCreate (offset, elementSize, elements, numElements,
                            allocation, &descriptor) ;

    where

        <offset>	- I
            is an offset in bytes of the "array" from the base of the array.
            This is a reserved area before the first element of the array
            and it could be useful if, for example, an application needs
            a contiguous message or record header before the array.  With
            an offset of zero, there is no reserved area.  The offset is
            taken into account by adxGet() when returning a pointer to an
            element.
        <elementSize>	- I
            is the size in bytes of each element of the array.
        <elements>	- I
            is an existing array to be described by the descriptor.  If this
            argument is NULL, then an zero-length-array descriptor is created.
            If a reserved area was requested with the offset argument, this
            area is dynamically allocated, but no elements are allocated.
            If this argument is not NULL, the caller must pass in a pointer
            to an existing array.  The pointer must point to the beginning of
            the reserved area, if any, or to the first element in the array.
        <numElements>	- I
            is the number of elements in the existing array.  This argument
            is ignored if an existing array was not specified.
        <allocation>	- I
            specifies the allocation type of the existing array:
                AdxStatic - is a fixed-size, immutable array.
                AdxDynamic - is a dynamically allocated array.  The descriptor
                    takes "ownership" of the existing array, which will be
                    free(3)ed when the descriptor is destroyed.
                AdxVolatile - is a array in flux.  A dynamic copy of the
                    region is made and stored in the descriptor; the copy
                    is annotated as AdxDynamic.
        <descriptor>	- O
            returns a handle for the array descriptor.
        <status>	- O
            returns the status of creating a descriptor, zero if no errors
            occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  adxCreate (

#    if PROTOTYPES
        size_t  offset,
        size_t  elementSize,
        void  *elements,
        size_t  numElements,
        AdxStorageClass  allocation,
        ArrayDx  *descriptor)
#    else
        offset, elementSize, elements, numElements, allocation, descriptor)

        size_t  offset ;
        size_t  elementSize ;
        void  *elements ;
        size_t  numElements ;
        AdxStorageClass  allocation ;
        ArrayDx  *descriptor ;
#    endif

{    /* Local variables. */
    MdxStorageClass  allocType ;
    size_t  numBytes ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxCreate) NULL descriptor handle pointer.\n") ;
        return (errno) ;
    }

/* Create and initialize the descriptor. */

    *descriptor = (ArrayDx) malloc (sizeof (_ArrayDx)) ;
    if (*descriptor == NULL) {
        LGE "(adxCreate) Error allocating array descriptor.\nmalloc: ") ;
        return (errno) ;
    }

    (*descriptor)->allocation = AdxDynamic ;
    (*descriptor)->offset = offset ;
    (*descriptor)->elementSize = elementSize ;
    (*descriptor)->increment = MAX_ADX_INCREMENT ;
    (*descriptor)->owned = false ;
    (*descriptor)->numElements = numElements ;
    (*descriptor)->memory = NULL ;

/* If no existing array was specified, then create an empty memory region
   descriptor for the new array.  If the array should have a reserved area,
   then add that area to the new array. */

    if (elements == NULL) {

        if (mdxCreate (NULL, 0, MdxVolatile, &(*descriptor)->memory)) {
            LGE "(adxCreate) Error creating empty memory descriptor.\nmdxCreate: ") ;
            PUSH_ERRNO ;  adxDestroy (*descriptor) ;
            *descriptor = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

        if ((offset > 0) &&	/* No elements, but create reserved area. */
            mdxAddS ((*descriptor)->memory, false, NULL, offset)) {
            LGE "(adxCreate) Error creating %"PRIuSIZE_T"-byte reserved area.\nmdxAddS: ",
                offset) ;
            PUSH_ERRNO ;  adxDestroy (*descriptor) ;
            *descriptor = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    }

/* An existing array was specified, so create a memory region descriptor
   for it in accordance with its allocation type. */

    else {

        switch (allocation) {
        case AdxDynamic:
            allocType = MdxDynamic ;  break ;
        case AdxStatic:
            allocType = MdxStatic ;  break ;
        case AdxVolatile:
        default:
            allocType = MdxVolatile ;  break ;
        }

        numBytes = offset ;	/* Reserved area plus elements. */
        numBytes += (elements == NULL) ? 0 : (numElements * elementSize) ;

        if (mdxCreate (elements, numBytes, allocType, &(*descriptor)->memory)) {
            LGE "(adxCreate) Error creating %"PRIuSIZE_T"-byte memory descriptor.\nmdxCreate: ",
                numBytes) ;
            PUSH_ERRNO ;  adxDestroy (*descriptor) ;
            *descriptor = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    }

    if ((*descriptor)->memory != NULL)	/* Transfer increment to memory region. */
        adxIncrement (*descriptor, (*descriptor)->increment) ;

    if ((elements == NULL) && (offset == 0)) {
        LGI "(adxCreate) Created descriptor %p for NULL array.\n",
            (void *) *descriptor) ;
    } else if ((elements == NULL) && (offset > 0)) {
        LGI "(adxCreate) Created descriptor %p for offset %"PRIuSIZE_T", NULL array %p.\n",
            (void *) *descriptor,
            offset,
            mdxRegionP ((*descriptor)->memory, 0)) ;
    } else {
        LGI "(adxCreate) Created descriptor %p for offset %"PRIuSIZE_T", %"PRIuSIZE_T"-element array %p.\n",
            (void *) *descriptor,
            offset,
            numElements,
            mdxRegionP ((*descriptor)->memory, 0)) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    adxDelete ()

    Delete One or More Elements from a Described Array.


Purpose:

    Function adxDelete() deletes a consecutive number of one or more elements,
    I to I+#-1, from a described array; # is the number of elements to delete.
    I is a caller-supplied index in the range 0..N-1, where N is the number of
    elements in the array.

    Trailing elements I+# through N-1 are moved down to position I, thus
    overwriting the elements being deleted.  The in-use length of the
    internal memory region is reduced by the size of the deleted elements
    (and, consequently, the region's free space increases by the same amount).

    Passing in an index of -1 results in adxDelete() deleting the last
    element in the array, element N-1.


    Invocation:

        status = adxDelete (descriptor, numElements, index) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <numElements>	- I
            is the number of contiguous elements to be deleted, beginning
            with the indexed element.
        <index>		- I
            is the index 0..N-1, where N is the number of elements, of the
            first element to be deleted.  An index of -1 is equivalent to
            N-1, so the last element is deleted from the end of the array.
        <status>	- O
            returns the status of deleting the element(s), zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  adxDelete (

#    if PROTOTYPES
        ArrayDx  descriptor,
        size_t  numElements,
        ssize_t  index)
#    else
        descriptor, numElements, index)

        ArrayDx  descriptor ;
        size_t  numElements ;
        ssize_t  index ;
#    endif

{    /* Local variables. */
    size_t  arrayLength, deletedBytes, trailingBytes ;
    void  *target, *trailer ;



/* Check the input arguments. */

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxDelete) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    arrayLength = descriptor->numElements ;

    if (index < 0) {
        index = arrayLength - 1 ;
    } else if (arrayLength < (index + numElements - 1)) {
		/* This range check is a confusing (to me) way of performing
		   a simple check.  The range of elements to be deleted must
		   fall within the boundaries, inclusive, of the array.  In
		   other words, given a 10-element, 0..9 array, deleting
		   elements 3..6 (I=3, #=4) is fine.  Deleting elements 5..12
		   (I=5, #=8) is not okay since elements 10, 11, and 12 fall
		   outside the array's upper bound of 9. */
        SET_ERRNO (EINVAL) ;
        LGE "(adxDelete) Indices %"PRIdSSIZE_T"..%"PRIuSIZE_T" exceed the range -1..%"PRIuSIZE_T" in array %p.\n",
            index,
            index + numElements - 1,
            arrayLength - 1,
            (void *) descriptor) ;
        return (errno) ;
    }

/* If the elements are being deleted from the middle of the array, shift the
   trailing elements after the elements being deleted down to the position
   of the first element being deleted. */

    deletedBytes = numElements * descriptor->elementSize ;
    trailingBytes = (arrayLength - index - numElements) *
                    descriptor->elementSize ;

    if ((index + numElements) < arrayLength) {
        target = adxGet (descriptor, index) ;
        trailer = adxGet (descriptor, index + numElements) ;
        memmove (target, trailer, trailingBytes) ;
    }

/* Finally, reduce the number of in-use elements in the internal memory region
   by the number of elements deleted. */

    mdxSetLength (descriptor->memory,
                  mdxLength (descriptor->memory) - deletedBytes) ;

    descriptor->numElements -= numElements ;

    LGI "(adxDelete) Descriptor %p deleted elements %"PRIdSSIZE_T"..%"PRIuSIZE_T".\n",
        (void *) descriptor, index, (size_t) index + numElements - 1) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    adxDestroy ()

    Destroy an Array Descriptor.


Purpose:

    Function adxDestroy() destroys a previously created array descriptor.


    Invocation:

        status = adxDestroy (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <status>	- O
            returns the status of destroying the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  adxDestroy (

#    if PROTOTYPES
        ArrayDx  descriptor)
#    else
        descriptor)

        ArrayDx  descriptor ;
#    endif

{

    if (descriptor == NULL)  return (0) ;

    LGI "(adxDestroy) Destroying array descriptor %p ...\n",
        (void *) descriptor) ;

    adxErase (descriptor) ;

    free (descriptor) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    adxDuplicate ()

    Duplicate an Array Descriptor.


Purpose:

    Function adxDuplicate() duplicates an array descriptor.  The memory
    region in the duplicate descriptor is always a dynamically allocated
    copy of the source descriptor's memory region.  In particular, this
    means that if a source descriptor points to a statically allocated
    region, any duplicate descriptors point not to the original static
    copy of the region, but to dynamic copies of the region.


    Invocation:

        newDescriptor = adxDuplicate (oldDescriptor) ;

    where

        <oldDescriptor>	- I
            is a descriptor handle returned by adxCreate().
        <newDescriptor>	- O
            returns the handle for the duplicate descriptor; NULL is returned
            in the event of an error.

*******************************************************************************/


ArrayDx  adxDuplicate (

#    if PROTOTYPES
        ArrayDx  oldDescriptor)
#    else
        oldDescriptor)

        ArrayDx  oldDescriptor ;
#    endif

{    /* Local variables. */
    ArrayDx  newDescriptor ;



    if (oldDescriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxDuplicate) NULL descriptor handle.\n") ;
        return (NULL) ;
    }

    if (adxCreate (oldDescriptor->offset,
                   oldDescriptor->elementSize,
                   adxGet (oldDescriptor, -1),
                   adxCount (oldDescriptor),
                   AdxVolatile,
                   &newDescriptor)) {
        LGE "(adxDuplicate) Error creating duplicate descriptor.\nadxCreate: ") ;
        return (NULL) ;
    }

    LGI "(adxDuplicate) Created duplicate %p of array %p.\n",
        (void *) newDescriptor, (void *) oldDescriptor) ;

    return (newDescriptor) ;

}

/*!*****************************************************************************

Procedure:

    adxElementSize ()

    Get the Size in Bytes of Each Element in a Described Array.


Purpose:

    Function adxElementSize() returns the size in bytes of each element in
    the array to which a descriptor points.  This is the size specified in
    the call to adxCreate() for the descriptor.


    Invocation:

        elementSize = adxElementSize (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <elementSize>	- O
            returns the size of each element in the array.

*******************************************************************************/


size_t  adxElementSize (

#    if PROTOTYPES
        ArrayDx  descriptor)
#    else
        descriptor)

        ArrayDx  descriptor ;
#    endif

{

    return ((descriptor == NULL) ? 0 : descriptor->elementSize) ;

}

/*!*****************************************************************************

Procedure:

    adxErase ()

    Erase the Contents of an Array Descriptor.


Purpose:

    Function adxErase() erases the contents of a previously created memory
    descriptor, leaving an empty array descriptor.


    Invocation:

        status = adxErase (descriptor) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <status>	- O
            returns the status of erasing the descriptor,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  adxErase (

#    if PROTOTYPES
        ArrayDx  descriptor)
#    else
        descriptor)

        ArrayDx  descriptor ;
#    endif

{

    if (descriptor == NULL)  return (0) ;

    LGI "(adxErase) Erasing array descriptor %p ...\n",
        (void *) descriptor) ;

    if ((descriptor->allocation == AdxDynamic) &&
        !descriptor->owned &&
        (descriptor->memory != NULL)) {
        mdxDestroy (descriptor->memory) ;
    }
    descriptor->memory = NULL ;

    descriptor->allocation = AdxVolatile ;
    descriptor->offset = 0 ;
    descriptor->elementSize = 1 ;
    descriptor->increment = MAX_ADX_INCREMENT ;
    descriptor->owned = false ;
    descriptor->numElements = 0 ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    adxGet ()

    Get the Address of an Element in a Described Array.


Purpose:

    Function adxGet() gets the address of the I-th element in a described
    array.  Index I is in the range 0..N-1, where N is the number of
    elements in the array.  Passing in an index of -1 results in adxGet()
    returning the base address of the array's memory region.  If the array
    has a reserved area before the array itself, the base address is the
    address of the reserved area; otherwise, the base address is simply
    the address of the 0-th element.

        NOTE that adxGet()'s use of index -1 differs from its
        purpose in adxAdd() and adxDelete().  Don't mix them up.


    Invocation:

        address = adxGet (descriptor, index) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <index>		- I
            is the index 0..N-1, where N is the number of elements,
            of the desired element.  As a special case, an index of
            -1 gets the address of the reserved area preceding the
            array; if the array has no reserved area, index -1 is
            the equivalent of index 0.
        <address>	- O
            returns the address of the indexed element as a (void *)
            pointer.  The caller is responsible for casting this
            address to a pointer to the appropriate data type.
            NULL is returned in the event of an error; e.g., a NULL
            descriptor or an out-of-range index.

*******************************************************************************/


void  *adxGet (

#    if PROTOTYPES
        ArrayDx  descriptor,
        ssize_t  index)
#    else
        descriptor, index)

        ArrayDx  descriptor ;
        ssize_t  index ;
#    endif

{    /* Local variables. */
    char  *baseAddress ;
    size_t  byteOffset ;



    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxGet) NULL descriptor handle.\n") ;
        return (NULL) ;
    }

    baseAddress = (char *) mdxRegionP (descriptor->memory, 0) ;

    if (index < 0) {		/* Get address of reserved area? */

        return ((void *) baseAddress) ;

    } else if ((size_t) (index+1) > descriptor->numElements) {

        SET_ERRNO (EINVAL) ;
        LGE "(adxGet) %"PRIdSSIZE_T"+1 exceeds the # of elements %"PRIuSIZE_T" in array %p.\n",
            index, descriptor->numElements, (void *) descriptor) ;
        return (NULL) ;

    } else {			/* Get address of actual element. */

        byteOffset = descriptor->offset + (index * descriptor->elementSize) ;
        return ((void *) &baseAddress[byteOffset]) ;

    }

}

/*!*****************************************************************************

Procedure:

    adxIncrement ()

    Set/Get the Allocation Increment for a Described Array.


Purpose:

    Function adxIncrement() sets/gets the block size used by adxGrow()
    for a specific described array.  When elements are added to a dynamic
    array and adxGrow() needs to expand the memory buffer to hold it, the
    new buffer size is the total number of elements in the array being
    constructed rounded up to the next highest multiple of the block size.

    The default block size in units of array elements is MAX_ADX_INCREMENT,
    128.  Increasing the block size reduces how frequently adxGrow() has to
    reallocate the buffer, a good idea when a large array is growing rapidly.

    The new block size does not take effect until the next time
    adxGrow() has to reallocate the buffer.

    If the calling application specifies a block size of -1,
    adxIncrement() simply returns the current increment.


    Invocation:

        increment = adxIncrement (descriptor, blockSize) ;

    where

        <descriptor>	- I
            is the descriptor handle returned by adxCreate().
        <blockSize>	- I
            specifies the new block size in units of array elements.
            If the block size is -1 or 0, adxIncrement() simply returns
            the current block size.  Otherwise, this new block size will
            replace the current block size.
        <increment>	- O
            returns the descriptor's current or new increment, depending
            on the block size input argument.  If the input argument is
            -1 or 0, the caller is "GETing" the descriptor's existing
            block size and that is what will be returned.  If the input
            argument is >= 1, the caller is "SETing" the descriptor's
            increment to the new value and that value will be returned
            to the caller.

*******************************************************************************/


size_t  adxIncrement (

#    if PROTOTYPES
        ArrayDx  descriptor,
        ssize_t  blockSize)
#    else
        descriptor, blockSize)

        ArrayDx  descriptor ;
        ssize_t  blockSize ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxIncrement) NULL descriptor handle.\n") ;
        return (0) ;
    }

    if (blockSize >= 1) {	/* Caller is setting a new block size. */
        descriptor->increment = blockSize ;
        if (descriptor->memory != NULL) {
            mdxIncrement (descriptor->memory,
                          blockSize * descriptor->elementSize) ;
        }
    }

    return (descriptor->increment) ;

}

/*!*****************************************************************************

Procedure:

    adxOwn ()

    Take Ownership of the Described Array.


Purpose:

    Function adxOwn() grants ownership of a dynamically allocated array
    to the application.  Normally, a call to adxDestroy() or adxErase()
    will free a dynamically allocated array.  However, if the application
    has taken possession of the array, then the application is responsible
    for freeing the array after the array descriptor is destroyed or
    erased.  This is typically done when the application is dynamically
    constructing an array using the ADX_UTIL functions and the final
    result must exist independently (i.e., past the lifetime) of the
    descriptor.  The application must free(3) the array when it is no
    longer needed.

        NOTE that an application can take ownership of an array at any
        time.  The descriptor's array will continue to grow as needed,
        which may require reallocating the array to hold more elements.
        When this happens, a new, larger array buffer is allocated, the
        old array is copied to the new buffer, and the old array is freed.
        Consequently, in order to get the correct array to free, the
        application should make a final call to adxGet() right before
        calling adxDestroy() or adxErase().  (Unless, of course, the
        application knows that it hasn't added anything to the described
        array since it last obtained the address.)


    Invocation:

        status = adxOwn (descriptor) ;

    where

        <descriptor>	- I
            is a descriptor handle returned by adxCreate().
        <status>	- O
            returns the status of taking ownership of the array,
            zero on success and ERRNO otherwise.  It is not an
            error to take ownership of a static array; doing so
            has no effect, although the application must be sure
            not to attempt freeing the static array.

*******************************************************************************/


errno_t  adxOwn (

#    if PROTOTYPES
        ArrayDx  descriptor)
#    else
        descriptor)

        ArrayDx  descriptor ;
#    endif

{

    if (descriptor == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(adxOwn) NULL descriptor handle.\n") ;
        return (errno) ;
    }

    descriptor->owned = true ;
    if (descriptor->memory != NULL)  return (mdxOwn (descriptor->memory)) ;

    return (0) ;

}
