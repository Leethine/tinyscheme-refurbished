/* $Id: id3_tag.c,v 1.9 2024/05/28 11:35:01 alex Exp $ */
/*******************************************************************************

File:

    id3_tag.c

    ID3 Tag Utilities - Tags


Author:    Alex Measday


Purpose:

    This file is an internal part of the ID3_UTIL package and provides
    functions for the manipulation of ID3 frames.  An ID3 tag is composed
    of one or more ID3 frames.  Each frame, in turn, contains one or more
    data values; e.g., character strings in the case of text frames.  This
    file, as its name suggests, implements the functions for dealing with
    ID3 tags.

    The list of frames in a tag is represented by an array of frame handles.
    Note that a call to adxGet() to retrieve a frame handle returns the
    address of the element containing the frame handle.  In other words,
    adxGet() returns a pointer (Id3Frame *) to the handle and not the
    handle directly.


Public Procedures:

    id3TagAddFrame() - adds/replaces an ID3 frame to/in an ID3 tag.
    id3TagCopy() - duplicates an ID3 tag.
    id3TagCreate() - creates an empty ID3 tag.
    id3TagDecodeV1() - decodes an ID3v1 tag.
    id3TagDecodeV2() - decodes an ID3v2 tag.
    id3TagDeleteFrame() - deletes an ID3 frame from an ID3 tag.
    id3TagDeletePicture() - deletes an APIC frame from an ID3 tag.
    id3TagDestroy() - destroys an ID3 tag.
    id3TagDump() - formats and dumps an ID3 tag for debug purposes.
    id3TagEncodeV1() - encodes an ID3v1 tag into its file format.
    id3TagEncodeV2() - encodes an ID3v2 tag into its file format.
    id3TagErase() - erases the contents of an ID3 tag.
    id3TagFindFrame() - finds a specified frame in an ID3 tag.
    id3TagFrameCount() - gets the number of frames in an ID3 tag.
    id3TagGetFrame() - gets an indexed frame from an ID3 tag.
    id3TagMerge() - merges the contents of two ID3 tags into a third tag.
    id3TagSortFrames() - sorts the frames in an ID3 tag.

Private Procedures:

    id3TagDecodeExt() - decodes an ID3v2 tag's extended header.
    id3TagEncodeExt() - encodes an ID3v2 tag's extended header.
    id3TagIndexOf() - returns the index of a frame in a tag's list of frames.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <ctype.h>			/* Standard character functions. */
#if HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "adx_util.h"			/* Array Descriptor utilities. */
#include  "mdx_util.h"			/* Memory Descriptor utilities. */
#include  "meo_util.h"			/* Memory operations. */
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */
#include  "id3_util.h"			/* ID3 tag utilities. */
#include  "id3_internals.h"		/* Internal definitions. */


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  id3_util_debug


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  id3TagDecodeExt (
#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *buffer,
        size_t  length,
        size_t  *skip
#    endif
    )  OCD ("id3_util") ;

static  size_t  id3TagEncodeExt (
#    if PROTOTYPES
        Id3Tag  tag,
        unsigned  char  *buffer
#    endif
    )  OCD ("id3_util") ;

static  size_t  id3TagIndexOf (
#    if PROTOTYPES
        Id3Tag  tag,
        Id3Frame  frame
#    endif
    )  OCD ("id3_util") ;

/*!*****************************************************************************

Procedure:

    id3TagAddFrame ()

    Add/Replace an ID3 Frame to/in an ID3 Tag.


Purpose:

    The id3TagAddFrame() function adds a frame to an ID3 tag's list
    of frames.  The caller can specify if existing frames of the same
    qualified type are to be deleted from the list before the new frame
    is added.  (The new frame becomes the property of the tag and the
    frame will be destroyed when the tag is destroyed or erased.)


    Invocation:

        status = id3TagAddFrame (tag, frame, unique) ;

    where

        <tag>		- I
            is the handle, returned by id3FrameCreate(), of the tag.
        <frame>		- I
            is the handle, returned by id3ValueCreate(), of the frame.
        <unique>	- I
            specifies if an existing frame or frames of the same qualified
            type should be deleted before the new frame is added.
        <status>	- O
            returns the status of adding the frame to the tag's list of
            frames, zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagAddFrame (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3Frame  frame,
        bool  unique)
#    else
        tag, frame, unique)

        Id3Tag  tag ;
        Id3Frame  frame ;
        bool  unique ;
#    endif

{    /* Local variables. */
    Id3Frame  oldFrame ;



    if ((tag == NULL) || (frame == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagAddFrame) NULL tag or frame handle: ") ;
        return (errno) ;
    }

/* If the tag already has a frame of the same type, then first delete
   the old frame. */

    while (unique) {	/* Loop until id3TagFindFrame() returns NULL. */

        switch (frame->type) {
        case ID3_ATTACHEDPIC:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.apic.type,
                                        frame->fdata.apic.description) ;
            break ;
        case ID3_COMMENT:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.comm.language,
                                        frame->fdata.comm.description) ;
            break ;
        case ID3_ENCRYPTREG:
        case ID3_GROUPIDREG:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.regi.symbol) ;
            break ;
        case ID3_GENERALOBJ:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.geob.description) ;
            break ;
        case ID3_POPULARIMETER:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.play.email) ;
            break ;
        case ID3_PRIVATE:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.priv.ownerID) ;
            break ;
        case ID3_TXXX_GENERIC:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.text.description) ;
            break ;
        case ID3_UNIQUEFILEID:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.ufid.ownerID) ;
            break ;
        case ID3_UNSYNCEDLYRICS:
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.uslt.language,
                                        frame->fdata.uslt.description) ;
            break ;
        case ID3_WXXX_GENERIC:			/* AKA ID3_WWW */
            oldFrame = id3TagFindFrame (tag, frame->type,
                                        frame->fdata.wurl.description) ;
            break ;
        default:
            oldFrame = id3TagFindFrame (tag, frame->type) ;
            break ;
        }

        if (oldFrame == NULL)  break ;

        id3TagDeleteFrame (tag, oldFrame) ;

    }

/* Now add the new frame. */

    if (adxAdd (tag->frames, &frame, 1, -1)) {
        LGE "(id3TagAddFrame) Error adding frame %p to tag %p.\nadxAdd: ",
            (void *) frame, (void *) tag) ;
        return (errno) ;
    }

    frame->parent = tag ;

    LGI "(id3TagAddFrame) Added %s%s frame %p to tag %p.\n",
        frame->delete ? "overlay-delete " : "",
        id3FrameTypeName (frame->type),
        (void *) frame, (void *) tag) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagCopy ()

    Duplicate an ID3 Tag.


Purpose:

    Function id3TagCopy() creates a full copy of an existing ID3 tag.


    Invocation:

        newTag = id3TagCopy (oldTag) ;

    where

        <oldTag>	- I
            is the tag being duplicated.
        <newTag>	- O
            returns a handle for the new tag; NULL is returned upon error.

*******************************************************************************/


Id3Tag  id3TagCopy (

#    if PROTOTYPES
        Id3Tag  oldTag)
#    else
        oldTag)

        Id3Tag  oldTag ;
#    endif

{    /* Local variables */
    Id3Frame  newFrame, *oldFrame ;
    Id3Tag  newTag ;
    size_t  i, count ;



    if (oldTag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagCopy) NULL tag handle: ") ;
        return (NULL) ;
    }

/* Create an empty new tag, the destination for the copy operation. */

    if (id3TagCreate (id3GetVersion (oldTag), &newTag)) {
        LGE "(id3TagCopy) Error creating the new tag.\nid3TagCreate: ") ;
        return (NULL) ;
    }

    LGI "(id3TagCopy) Copying old v0x%06lX tag %p to new v0x%06lX tag %p.\n",
        oldTag->version, (void *) oldTag, newTag->version, (void *) newTag) ;

/* Copy the fields in the oldTag tag to the new tag. */

    newTag->overrideVersion = oldTag->overrideVersion ;
    newTag->encoding = oldTag->encoding ;
    newTag->v2Flags = oldTag->v2Flags ;
    newTag->v2Size = oldTag->v2Size ;
    newTag->unsync = oldTag->unsync ;

    newTag->exth = oldTag->exth ;
    if ((oldTag->exth.header != NULL) && (oldTag->exth.length > 0)) {
        newTag->exth.header = memdup (oldTag->exth.header,
                                      oldTag->exth.length) ;
        if (newTag->exth.header == NULL) {
            LGE "(id3TagCopy) Error duplicating %"PRIuSIZE_T"-byte extended header for tag %p.\nmemdup: ",
                oldTag->exth.length, (void *) oldTag) ;
            PUSH_ERRNO ;  id3TagDestroy (newTag) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

/* Make copies of each of the old tag's frames and add them to the new tag.
   Remember that the array of frames is an array of handles-for/pointers-to
   frame structures and that adxGet() returns the address of a handle;
   hence there is an extra level of indirection.*/

    count = adxCount (oldTag->frames) ;

    for (i = 0 ;  i < count ;  i++) {

        oldFrame = adxGet (oldTag->frames, i) ;

        newFrame = id3FrameCopy (*oldFrame) ;
        if (newFrame == NULL) {
            LGE "(id3TagCopy) Error duplicating frame %p.\nid3FrameCopy: ",
                (void *) *oldFrame) ;
            PUSH_ERRNO ;  id3TagDestroy (newTag) ;  POP_ERRNO ;
            return (NULL) ;
        }

        if (id3TagAddFrame (newTag, newFrame, true)) {
            LGE "(id3TagCopy) Error adding new frame %p to new tag %p.\nid3TagAddFrame: ",
                (void *) newFrame, (void *) newTag) ;
            PUSH_ERRNO ;  id3TagDestroy (newTag) ;  POP_ERRNO ;
            return (NULL) ;
        }

    }

    return (newTag) ;

}

/*!*****************************************************************************

Procedure:

    id3TagCreate ()

    Create an Empty ID3 Tag.


Purpose:

    The id3TagCreate() function creates an empty ID3 tag (all fields
    cleared except for the version number).  The created tag has an
    initially empty array of frames; i.e., the array descriptor is
    allocated but the array has zero elements.


    Invocation:

        status = id3TagCreate (version, &tag) ;

    where

        <version>	- I
            specifies the type (v1 or v2) of ID3 tag to create.
        <tag>		- O
            returns a handle for the new tag.  This handle is used in calls
            to the other ID3_UTIL functions.
        <status>	- O
            returns a handle for the new tag.  This handle is used
            in calls to the other ID3_UTIL functions.  After the tag
            is no longer needed, id3TagDestroy() should be called to
            dispose of the tag.

*******************************************************************************/


errno_t  id3TagCreate (

#    if PROTOTYPES
        Id3Version  version,
        Id3Tag  *tag)
#    else
        version, tag)

        Id3Version  version ;
        Id3Tag  *tag ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagCreate) NULL tag return: ") ;
        return (errno) ;
    }

/* Allocate a new tag structure. */

    *tag = (_Id3Tag *) malloc (sizeof (_Id3Tag)) ;
    if (*tag == NULL) {
        LGE "(id3TagCreate) Error allocating tag structure.\nmalloc: ") ;
        return (errno) ;
    }

    (*tag)->version = version ;
    (*tag)->overrideVersion = false ;
    (*tag)->encoding = ID3_UTF16_WITH_BOM ;	/* Common to ID3v2.3 & v2.4. */
    (*tag)->flags = 0 ;
    (*tag)->song = NULL ;
    (*tag)->artist = NULL ;
    (*tag)->album = NULL ;
    (*tag)->comment = NULL ;
    (*tag)->year = 0 ;
    (*tag)->track = 0 ;
    (*tag)->genre = 0 ;
    (*tag)->genreName = NULL ;
    (*tag)->v2Flags = 0 ;
    (*tag)->v2Size = 0 ;
    (*tag)->v2Buffer = NULL ;
    (*tag)->unsync = false ;
    (*tag)->exth.version = version ;
    (*tag)->exth.fields = 0 ;
    (*tag)->exth.length = 0 ;
    (*tag)->exth.flags = 0 ;
    (*tag)->exth.padding = 0 ;
    (*tag)->exth.originalCRC = 0 ;
    (*tag)->exth.computedCRC = 0 ;
    (*tag)->exth.isUpdate = false ;
    (*tag)->exth.restrictions = 0 ;
    (*tag)->exth.header = NULL ;
    (*tag)->frames = NULL ;

/* Allocate an empty array of frames. */

    if (adxCreate (0, sizeof (Id3Frame), NULL, 0, AdxDynamic,
                   &(*tag)->frames)) {
        LGE "(id3TagCreate) Error allocating array of frames.\nadxCreate: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  POP_ERRNO ;
        return (errno) ;
    }

    LGI "(id3TagCreate) Created version 0x%06lX tag %p.\n",
        version, (void *) *tag) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagDecodeV1 ()

    Decode an ID3v1 Tag from its File Format.


Purpose:

    Function id3TagDecodeV1() decodes the fields in an ID3v1 tag.


    Invocation:

        status = id3TagDecodeV1 (buffer, &tag) ;

    where

        <buffer>	- I
            is the address of a 128-byte buffer from which the ID3v1 tag
            will be decoded.
        <tag>		- O
            returns a handle for the new ID3 tag; the caller is responsible
            for calling id3TagDestroy() after the tag is no longer needed.
        <status>	- O
            returns the status of decoding the tag, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagDecodeV1 (

#    if PROTOTYPES
        char  *buffer,
        Id3Tag  *tag)
#    else
        buffer, tag)

        char  *buffer ;
        Id3Tag  *tag ;
#    endif

{    /* Local variables. */
    char  temp[MAX_SONG_LENGTH+1] ;
    long  number ;
    size_t  i ;



    if ((buffer == NULL) || (tag == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDecodeV1) NULL buffer or tag: ") ;
        return (errno) ;
    }

    if (!id3IsTag (buffer, ID3_V1_TRAILER_SIZE)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDecodeV1) Invalid ID3v1 tag: \n") ;
        *tag = NULL ;
        return (errno) ;
    }

/* Create an empty ID3 tag. */

    if (id3TagCreate (ID3V_V1_1, tag)) {
        LGE "(id3TagDecodeV1) Error creating empty ID3 tag.\nid3TagCreate: ") ;
        return (errno) ;
    }

/* Populate the tag with the fields in the buffer. */

    i = 3 ;

    strncpym (temp, &buffer[i], MAX_SONG_LENGTH, sizeof temp) ;
    if (id3SetSong (*tag, temp)) {
        LGE "(id3TagDecodeV1) Error setting song field.\nid3SetSong: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }
    i += MAX_SONG_LENGTH ;

    strncpym (temp, &buffer[i], MAX_ARTIST_LENGTH, sizeof temp) ;
    if (id3SetArtist (*tag, temp)) {
        LGE "(id3TagDecodeV1) Error setting artist field.\nid3SetArtist: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }
    i += MAX_ARTIST_LENGTH ;

    strncpym (temp, &buffer[i], MAX_ALBUM_LENGTH, sizeof temp) ;
    if (id3SetAlbum (*tag, temp)) {
        LGE "(id3TagDecodeV1) Error setting album field.\nid3SetAlbum: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }
    i += MAX_ALBUM_LENGTH ;

    strncpym (temp, &buffer[i], MAX_YEAR_LENGTH, sizeof temp) ;
    number = atol (temp) ;
    if (id3SetYear (*tag, number)) {
        LGE "(id3TagDecodeV1) Error setting year field.\nid3SetYear: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }
    i += MAX_YEAR_LENGTH ;

    strncpym (temp, &buffer[i], MAX_COMMENT_LENGTH, sizeof temp) ;
    if (id3SetComment (*tag, false, NULL, NULL, temp)) {
        LGE "(id3TagDecodeV1) Error setting comment field.\nid3SetComment: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }
    i += MAX_COMMENT_LENGTH ;

    if (buffer[i++] == 0) {			/* Version 1.1 tag? */
        number = (long) buffer[i++] & 0x00FF ;
        if (id3SetTrack (*tag, number, 0)) {
            LGE "(id3TagDecodeV1) Error setting track field.\nid3SetYear: ") ;
            PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {					/* Version 1.0 tag: no track. */
        i++ ;
    }

    sprintf (temp, "%lu", (unsigned long) buffer[i++] & 0x00FF) ;
    if (id3SetGenre (*tag, temp)) {
        LGE "(id3TagDecodeV1) Error setting genre field.\nid3SetGenre: ") ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagDecodeV2 ()

    Decode an ID3v2 Tag from its File Format.


Purpose:

    Function id3TagDecodeV2() decodes the fields in an ID3v2 tag.


    Invocation:

        status = id3TagDecodeV2 (buffer, length, &tag) ;

    where

        <buffer>	- I
            is the address of a buffer containing the full ID3v2 tag which
            will be decoded.
        <length>	- I
            is the length in bytes of the buffer.
        <tag>		- O
            returns a handle for the new ID3 tag; the caller is responsible
            for calling id3TagDestroy() after the tag is no longer needed.
        <status>	- O
            returns the status of decoding the tag, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagDecodeV2 (

#    if PROTOTYPES
        char  *buffer,
        size_t  length,
        Id3Tag  *tag)
#    else
        buffer, length, tag)

        char  *buffer ;
        size_t  length ;
        Id3Tag  *tag ;
#    endif

{    /* Local variables. */
    char  *framble ;
    Id3Frame  frame ;
    Id3Version  version ;
    size_t  remainingSize, skip ;



    if ((buffer == NULL) || (tag == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDecodeV2) NULL buffer or tag: ") ;
        return (errno) ;
    }

    if (!id3IsTag (buffer, length)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDecodeV2) Invalid ID3v2 tag: \n") ;
        *tag = NULL ;
        return (errno) ;
    }

/* Create an empty ID3 tag. */

    version = ID3V (2, buffer[3], buffer[4]) ;

    if (id3TagCreate (version, tag)) {
        LGE "(id3TagDecodeV2) Error creating empty ID3 tag.\nid3TagCreate: ") ;
        return (errno) ;
    }

    (*tag)->v2Flags = buffer[5] & 0x0FF ;
    (*tag)->v2Size = (size_t) id3DecodeSSI (&buffer[6], 4) ;

    LGI "(id3TagDecodeV2) ID3v2 tag (Version 0x%06lX, Flags 0x%02X, Size %"PRIuSIZE_T")\n",
        version, (*tag)->v2Flags, (*tag)->v2Size) ;

    if (version == ID3V_V2_2) {
        SET_ERRNO (ENOTSUP) ;
        LGE "(id3TagDecodeV2) %"PRIuSIZE_T"-byte ID3v2.2 tag not supported: ",
            (*tag)->v2Size) ;
        PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
        return (errno) ;
    }

/* In the case of an ID3v2.3 unsynchronized tag, de-unsynchronize it to get
   the original data back.  (An ID3v2.4 tag has unsynchronization applied
   to individual frames, not to the tag as a whole.) */

    framble = buffer + ID3_V2_HEADER_SIZE ;
    remainingSize = length - ID3_V2_HEADER_SIZE ;

    if ((version == ID3V_V2_3) &&
        ((*tag)->v2Flags & ID3_V2_UNSYNCHRONIZED_TAG)) {
        size_t  numSync = id3DeUnsync (framble, remainingSize) ;
        LGI "(id3TagDecodeV2) De-unsynchronized, %"PRIuSIZE_T" bytes less.\n",
            numSync) ;
        (*tag)->v2Size -= numSync ;
        remainingSize -= numSync ;
    }

/* Decode and store the extended header, if present.  No pun intended, but
   tag the header with this tag's version number.  Doing so prevents the
   version-specific headers from being written out when a tag's version
   number has changed.  This can happen during the copy/merge processing
   that produces the final output tag; e.g., when converting the ID3v2.4
   tags in a batch of audio files to ID3v2.3.  Although, now that I think
   about it, id3TagMerge() could just check for the change in versions and
   not copy the extended header to the merged tag. */

    if ((*tag)->v2Flags & ID3_V2_EXTENDED_HEADER) {

        if (id3TagDecodeExt (*tag, framble, remainingSize, &skip)) {
            LGE "(id3TagDecodeV2) Error decoding extended header.\nid3TagDecodeExt: ") ;
            PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

        framble += skip ;
        remainingSize -= skip ;

        if ((*tag)->exth.fields & ID3_EXTH_CRC_PRESENT) {
            (*tag)->exth.computedCRC =
                (unsigned long) id3CRC32 (framble, remainingSize, 0) ;
            if ((*tag)->exth.originalCRC != (*tag)->exth.computedCRC) {
                SET_ERRNO (EINVAL) ;
                LGE "(id3TagDecodeV2) Tag CRC 0x%08lX != Computed CRC 0x%08lX\n",
                    (*tag)->exth.originalCRC, (*tag)->exth.computedCRC) ;
            }
        }

    }

/* Decode each frame in the tag. */

    while (remainingSize > ID3_V2_FRAME_HEADER_SIZE) {

        if (!id3IsFrame (framble, remainingSize)) {	/* No more frames? */
            LGI "(id3TagDecodeV2) No more frames; %"PRIuSIZE_T" bytes remaining.\n",
                remainingSize) ;
            break ;
        }

        if (id3FrameDecode (*tag, framble, length, &frame, &skip)) {
            LGE "(id3TagDecodeV2) Error decoding frame.\nid3FrameDecode: ") ;
            PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

        if (id3TagAddFrame (*tag, frame, false)) {
            LGE "(id3TagDecodeV2) Error adding frame %p to tag %p.\nid3TagAddFrame: ",
                (void *) frame, (void *) *tag) ;
            PUSH_ERRNO ;  id3TagDestroy (*tag) ;  *tag = NULL ;
            id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }

        framble += skip ;
        remainingSize -= skip ;

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagDeleteFrame ()

    Delete an ID3 Frame from an ID3 Tag.


Purpose:

    The id3TagDeleteFrame() function deletes a frame from an ID3 tag's list
    of frames.  The deleted frame is then deallocated by id3FrameDestroy().


    Invocation:

        status = id3TagDeleteFrame (tag, frame) ;

    where

        <tag>		- I
            is the handle, returned by id3TagCreate(), of the tag.
        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame.
        <status>	- O
            returns the status of deleting the frame from the tag's list
            of frames, zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagDeleteFrame (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3Frame  frame)
#    else
        tag, frame)

        Id3Tag  tag ;
        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    Id3Frame  *listFrame ;
    size_t  i ;



    if ((tag == NULL) || (frame == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDeleteFrame) NULL tag or frame handle: ") ;
        return (errno) ;
    }

    i = adxCount (tag->frames) ;
    while (i-- > 0) {
        listFrame = adxGet (tag->frames, i) ;
        if (*listFrame == frame) {			/* Found? */
            adxDelete (tag->frames, 1, i) ;
            id3FrameDestroy (frame) ;
            return (0) ;
        }
    }

    return (0) ;					/* Not found. */

}

#ifdef IS_THIS_NEEDED

/*!*****************************************************************************

Procedure:

    id3TagDeletePicture ()

    Delete an APIC Frame from an ID3 Tag.


Purpose:

    The id3TagDeletePicture() function deletes an attached picture (APIC frame)
    from an ID3 tag's list of frames.  The deleted frame is deallocated by
    id3FrameDestroy().

    An ID3 tag may have multiple APIC frames, differentiated by their picture
    type; e.g., front cover, back cover, etc.  And, for a given picture type,
    there may be multiple APIC frames of that type, differentiated by their
    descriptions.  (The ID3v2.4 specification says "content descriptor";
    I assume that means the frame's description field.)  For example, a song
    composed by two people could have two "Composer"-type APIC frames whose
    respective descriptions are the individual names of each composer.

    If the caller specifies a NULL description, this function ignores the
    APIC frames' descriptions and deletes any image of the given type.


    Invocation:

        status = id3TagDeletePicture (tag, type, description) ;

    where

        <tag>		- I
            is the handle, returned by id3FrameCreate(), of the tag.
        <type>		- I
            is the picture type (defined in "id3_util.h") to be matched.
        <description>	- I
            is the description to be matched.  If this argument is NULL,
            any picture of the specified type is deleted.
        <status>	- O
            returns the status of deleting the picture from the tag's list
            of frames, zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagDeletePicture (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3PictureType  type,
        const  char  *description)
#    else
        tag, type, description)

        Id3Tag  tag ;
        Id3PictureType  type ;
        const  char  *description ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDeletePicture) NULL tag handle: ") ;
        return (errno) ;
    }

/* Remember that the array of frames is an array of
   handles-for/pointers-to frame structures and that adxGet() returns
   the address of a handle; hence the extra level of indirection.
   NOTE that the frame must be destroyed before it is deleted from
   the array; otherwise, the following frame's handle would be moved
   down to the deleted frame's position, resulting in the following
   frame being destroyed instead of the deleted frame. */

    i = adxCount (tag->frames) ;
    while (i-- > 0) {
        Id3Frame  *frame = adxGet (tag->frames, i) ;
        if ((*frame)->type != ID3_ATTACHEDPIC)  continue ;
        if ((*frame)->fdata.apic.type != type)  continue ;
        if ((description == NULL) ||
            (strcmp ((*frame)->fdata.apic.description, description) == 0)) {
            id3FrameDestroy (*frame) ;		/* Destroy BEFORE deleting. */
            adxDelete (tag->frames, 1, i) ;
        }
    }

    return (0) ;

}

#endif	/* IS_THIS_NEEDED */

/*!*****************************************************************************

Procedure:

    id3TagDestroy ()

    Destroy an ID3 Tag.


Purpose:

    The id3TagDestroy() function frees the dynamically allocated contents
    in an ID3 tag and then the tag structure itself.


    Invocation:

        status = id3TagDestroy (tag) ;

    where

        <tag>		- I
            is the handle, returned by id3TagCreate(), of the tag to destroy.
        <status>	- O
            returns the status of destroying the tag, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagDestroy (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL)  return (0) ;

    LGI "(id3TagDestroy) Destroying tag %p ...\n", (void *) tag) ;

    id3TagErase (tag) ;

    adxDestroy (tag->frames) ;

    free (tag) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagDump ()

    Dump an ID3 Frame.


Purpose:

    The id3TagDump() function formats and prints an ID3 tag to a file
    for debug purposes.


    Invocation:

        status = id3TagDump (file, indentation, tag) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <tag>		- I
            is the handle, returned by id3TagCreate(), of the tag to dump.
        <status>	- O
            returns the status of dumping the tag, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagDump (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation,
        Id3Tag  tag)
#    else
        file, indentation, tag)

        FILE  *file ;
        char  *indentation ;
        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    static  char  frameIndent[128] ;
    Id3Frame  *frame ;
    size_t  i, count ;



    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    if (tag == NULL) {
        fprintf (file, "%s{ Dump of Tag 0: <nil> }\n", indentation) ;
        return (0) ;
    }

    count = adxCount (tag->frames) ;

    if (tag->version < ID3V_V2) {		/* Version 1.x tag? */
        fprintf (file, "%s{ Dump of Tag %p (%ld frames):\n",
                 indentation,
                 (void *) tag,
                 (long) count) ;
    } else {					/* Version 2.x tag. */
        const  char  *syncString = tag->unsync ? "SYNC, " : "" ;
        if ((tag->v2Size > 0) && (count > 0))
            fprintf (file, "%s{ Dump of Tag %p (%ssize = %"PRIuSIZE_T", # frames = %"PRIuSIZE_T"):\n",
                     indentation, (void *) tag, syncString, tag->v2Size, count) ;
        else if ((tag->v2Size > 0) && (count == 0))
            fprintf (file, "%s{ Dump of Tag %p (%ssize = %"PRIuSIZE_T"):\n",
                     indentation, (void *) tag, syncString, tag->v2Size) ;
        else if ((tag->v2Size == 0) && (count > 0))
            fprintf (file, "%s{ Dump of Tag %p (%s# frames = %"PRIuSIZE_T"):\n",
                     indentation, (void *) tag, syncString, count) ;
        else					/* Size and count both zero. */
            fprintf (file, "%s{ Dump of Tag %p%s:\n",
                     indentation, (void *) tag, tag->unsync ? " (SYNC)" : "") ;
    }

    strncpym (frameIndent, indentation,		/* Add 4 spaces to indent. */
              sizeof frameIndent, sizeof frameIndent) ;
    strncatm (frameIndent, "    ",
              sizeof frameIndent, sizeof frameIndent) ;

    if (tag->v2Flags & ID3_V2_EXTENDED_HEADER) {
        if (tag->exth.version < ID3V_V2_4) {
            fprintf (file, "%s{ Extended Header (ID3v2.3, 0x%04lX, %"PRIuSIZE_T" bytes):\n",
                     frameIndent, tag->exth.flags, tag->exth.length) ;
        } else {
            fprintf (file, "%s{ Extended Header (ID3v2.4, 0x%02lX, %"PRIuSIZE_T" bytes):\n",
                     frameIndent, tag->exth.flags, tag->exth.length) ;
        }
        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT) {
            fprintf (file, "%s    Tag CRC: 0x%08lX  Computed: 0x%08lX\n",
                     frameIndent, tag->exth.originalCRC,
                     tag->exth.computedCRC) ;
        }
        if (tag->exth.header != NULL) {
            char  *extIndent = strDupCat (2, frameIndent, "    ") ;
            meoDumpX (file, extIndent, 0, tag->exth.header, tag->exth.length) ;
            free (extIndent) ;
        }
        fprintf (file, "%s}\n", frameIndent) ;
    }

    for (i = 0 ;  i < count ;  i++) {
        frame = adxGet (tag->frames, i) ;
        id3FrameDump (file, frameIndent, *frame) ;
    }

    fprintf (file, "%s}\n", indentation) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagEncodeV1 ()

    Encode an ID3v1 Tag into its File Format.


Purpose:

    Function id3TagEncodeV1() encodes an ID3v1 tag into a byte buffer.


    Invocation:

        status = id3TagEncodeV1 (tag, &buffer) ;

    where

        <tag>		- I
            is the handle for the ID3 tag to be encoded.
        <buffer>	- O
            is the address of a buffer into which the ID3 tag is encoded.
            The buffer must be at least 128 bytes in size; i.e., the size
            of an ID3v1 tag.
        <status>	- O
            returns the status of encoding the tag, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagEncodeV1 (

#    if PROTOTYPES
        const  Id3Tag  tag,
        char  *buffer)
#    else
        tag, buffer)

        Id3Tag  tag ;
        char  *buffer ;
#    endif

{    /* Local variables. */
    const  char  *s ;
    long  number ;
    size_t  i ;



    if ((tag == NULL) || (buffer == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagEncodeV1) NULL tag or buffer: ") ;
        return (errno) ;
    }

/* Zero out the buffer and insert the "TAG" prefix. */

    memset (buffer, 0, ID3_V1_TRAILER_SIZE) ;

    i = 0 ;
    buffer[i++] = 'T' ;
    buffer[i++] = 'A' ;
    buffer[i++] = 'G' ;

/* Encode the fields defined in the tag into the buffer. */

    s = id3GetSong (tag) ;
    if (s != NULL)  strncpy (&buffer[i], s, MAX_SONG_LENGTH) ;
    i += MAX_SONG_LENGTH ;

    s = id3GetArtist (tag) ;
    if (s != NULL)  strncpy (&buffer[i], s, MAX_ARTIST_LENGTH) ;
    i += MAX_ARTIST_LENGTH ;

    s = id3GetAlbum (tag) ;
    if (s != NULL)  strncpy (&buffer[i], s, MAX_ALBUM_LENGTH) ;
    i += MAX_ALBUM_LENGTH ;

    number = id3GetYear (tag) ;
    if (number >= 0) {
        char  temp[24] ;
        sprintf (temp, "%*ld", MAX_YEAR_LENGTH, number) ;
        strncpy (&buffer[i], temp, MAX_YEAR_LENGTH) ;
    }
    i += MAX_YEAR_LENGTH ;

    s = id3GetComment (tag, NULL, NULL) ;
    if (s != NULL)  strncpy (&buffer[i], s, MAX_COMMENT_LENGTH) ;
    i += MAX_COMMENT_LENGTH ;

    buffer[i++] = 0 ;			/* Indicates version 1.1 tag. */

    number = id3GetTrack (tag, NULL) ;
    if (number >= 0)  buffer[i] = (char) number ;
    i++ ;

    s = id3GetGenre (tag) ;
    if (s != NULL)  buffer[i] = (char) id3ToGenre (s) ;
    i++ ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagEncodeV2 ()

    Encode an ID3v2 Tag into its File Format.


Purpose:

    Function id3TagEncodeV2() encodes an ID3v2 tag into a dynamically
    allocated byte buffer.


    Invocation:

        status = id3TagEncodeV2 (tag, &buffer, &length) ;

    where

        <tag>		- I
            is the handle for the ID3 tag to be encoded.
        <buffer>	- O
            returns a dynamically allocated buffer holding the encoded tag.
            The caller is responsible for free(3)ing the buffer after it is
            no longer needed.
        <length>	- O
            returns the length in bytes of the buffer.
        <status>	- O
            returns the status of encoding the tag, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagEncodeV2 (

#    if PROTOTYPES
        const  Id3Tag  tag,
        char  **buffer,
        size_t  *length)
#    else
        tag, buffer, length)

        Id3Tag  tag ;
        char  **buffer ;
        size_*t  length ;
#    endif

{    /* Local variables. */
    char  *body, *buf ;
    unsigned  char  extHeader[ID3_EXTHDR_MAX_SIZE] ;
    Id3Version  version ;
    MemoryDx  mdx ;
    size_t  count, extLength = 0, i, numSync, offset, skip ;



    if ((tag == NULL) || (buffer == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagEncodeV2) NULL tag, buffer, or length: ") ;
        return (errno) ;
    }

    *buffer = NULL ;
    *length = 0 ;

/* Allocate a memory buffer to hold the ID3v2 tag. */

    if (mdxCreate (NULL, 0, MdxDynamic, &mdx)) {
        LGE "(id3TagEncodeV2) Error allocating buffer for tag %p.\nmdxCreate: ",
            (void *) tag) ;
        return (errno) ;
    }

/* Initialize the 10-byte tag header.  The tag size will be encoded and
   filled in later after the full length of the tag is known. */

    if (mdxAddS (mdx, false, NULL, ID3_V2_HEADER_SIZE)) {
        LGE "(id3TagEncodeV2) Error adding ID3v2 header to tag %p.\nmdxAddS: ",
            (void *) tag) ;
        return (errno) ;
    }

    buf = mdxRegionP (mdx, 0) ;

    memset (buf, 0, ID3_V2_HEADER_SIZE) ;

    buf[0] = 'I' ;
    buf[1] = 'D' ;
    buf[2] = '3' ;

    version = (tag->version < ID3V_V2_4) ? ID3V_V2_3 : tag->version ;
    buf[3] = ID3V_MAJOR (version) ;
    buf[4] = ID3V_REVISION (version) ;

    buf[5] = 0 ;			/* Flags. */

/* Reserve space in the encoded tag for the optional ID3 extended header. */

    offset = ID3_V2_HEADER_SIZE ;

    if (tag->v2Flags & ID3_V2_EXTENDED_HEADER) {
        extLength = id3TagEncodeExt (tag, extHeader) ;
        if (mdxAddS (mdx, false, NULL, extLength)) {
            LGE "(id3TagEncodeV2) Error reserving %"PRIuSIZE_T" bytes for extended header, tag %p.\nmdxAddS: ",
                extLength, (void *) tag) ;
            return (errno) ;
        }
        offset += extLength ;
    }

/* Encode each of the frames in the tag. */

    tag->numUnsyncFrames = 0 ;		/* For ID3v2.4 encoding. */

    count = adxCount (tag->frames) ;
    for (i = 0 ;  i < count ;  i++) {
        Id3Frame  *frame = adxGet (tag->frames, i) ;
        if (id3FrameEncode (tag, *frame, mdx, &skip)) {
            LGE "(id3TagEncodeV2) Error encoding frame %"PRIuSIZE_T" in tag %p.\nid3FrameEncode: ",
                i, (void *) tag) ;
            PUSH_ERRNO ;  mdxDestroy (mdx) ;  POP_ERRNO ;
            return (errno) ;
        }
        offset += skip ;
    }

/* Go back and encode the optional extended header.  (Space was reserved
   for the header before the frames were encoded.)  In particular, the
   tag's CRC can now be computed over the entirety of the encoded frames. */

    if (tag->v2Flags & ID3_V2_EXTENDED_HEADER) {
        buf = (char *) mdxRegionP (mdx, 0) + ID3_V2_HEADER_SIZE ;
        *length = mdxLength (mdx) - ID3_V2_HEADER_SIZE ;
        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT) {
            tag->exth.computedCRC = id3CRC32 (&buf[extLength],
                                              *length - extLength, 0) ;
        }
#ifndef NOTE_THAT_ID3_UTIL_DOES_NOT_ADD_PADDING
        tag->exth.padding = 0 ;		/* Force no-padding. */
#endif
        id3TagEncodeExt (tag, (unsigned char *)buf) ;
    }


/* If this is an ID3v2.3 tag and it is to be unsynchronized, then do so.
   On the off chance that a tag ends with a 0xFF byte and that the byte
   after the tag completes a false sync code, the ID3 standard recommends
   adding explicit padding to avoid such a false sync code.  It's simpler,
   however, for id3Unsync() to just add implicit padding (of one byte)
   to the tag, which will be removed when the tag is de-unsynchronized. */

    tag->v2Flags &= ~ID3_V2_UNSYNCHRONIZED_TAG ;		/* Clear bit. */

    if ((tag->version == ID3V_V2_3) && tag->unsync) {

        body = (char *) mdxRegionP (mdx, 0) + ID3_V2_HEADER_SIZE ;
        *length = mdxLength (mdx) - ID3_V2_HEADER_SIZE ;
        buf = id3Unsync (body, *length, &numSync) ;
        if (buf == NULL) {
            LGE "(id3TagEncodeV2) Error unsynchronizing tag %p.\nid3Unsync: ",
                (void *) tag) ;
            PUSH_ERRNO ;  mdxDestroy (mdx) ;  POP_ERRNO ;
            return (errno) ;
        }

        LGI "(id3TagEncodeV2) Unsynchronized, %"PRIuSIZE_T" bytes more.\n",
            numSync) ;

        if (numSync > 0) {
            mdxSetLength (mdx, ID3_V2_HEADER_SIZE) ;
            if (mdxAddS (mdx, false, buf, *length + numSync)) {
                LGE "(id3TagEncodeV2) Error storing %"PRIuSIZE_T"-byte unsynchronizing body for tag %p.\nmdxAddS: ",
                    *length + numSync, (void *) tag) ;
                PUSH_ERRNO ;  free (buf) ;  mdxDestroy (mdx) ;  POP_ERRNO ;
                return (errno) ;
            }
            free (buf) ;
            tag->v2Flags |= ID3_V2_UNSYNCHRONIZED_TAG ;		/* Set bit. */
        }

    }

/* If this is an ID3v2.4 tag and one or more frames were unsynchronized
   during encoding, then flag the tag as unsynchronized. */

    else if ((tag->version == ID3V_V2_4) && (tag->numUnsyncFrames > 0)) {
        LGI "(id3TagEncodeV2) %"PRIuSIZE_T" unsynchronized frames.\n",
            tag->numUnsyncFrames) ;
        tag->v2Flags |= ID3_V2_UNSYNCHRONIZED_TAG ;		/* Set bit. */
    }

/* Now that it's known, encode and store the tag size in the header. */

    *buffer = mdxRegionP (mdx, 0) ;	/* Memory buffer returned to caller. */
    *length = mdxLength (mdx) ;

    (*buffer)[5] = tag->v2Flags ;

    tag->v2Size = *length - ID3_V2_HEADER_SIZE ;

    id3EncodeSSI (tag->v2Size, 4, &(*buffer)[6]) ;

    LGI "(id3TagEncodeV2) Tag %p encoded: %"PRIuSIZE_T" bytes.\n",
        (void *) tag, *length) ;

/* Return the encoded tag to the caller. */

    mdxOwn (mdx) ;			/* Delete buffer descriptor. */
    mdxDestroy (mdx) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagErase ()

    Erase the Contents of an ID3 Tag.


Purpose:

    The id3TagErase() function frees the dynamically allocated contents
    in a tag and reinitializes the tag to its empty state, leaving the
    now-empty array of frames and version number intact.


    Invocation:

        status = id3TagErase (tag) ;

    where

        <tag>		- I
            is the handle, returned by id3TagCreate(), of the frame to erase.
        <status>	- O
            returns the status of erasing the tag, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagErase (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagErase) NULL tag handle: ") ;
        return (errno) ;
    }

    LGI "(id3TagErase) Erasing tag %p ...\n", (void *) tag) ;

/* Delete the frames, if any, in the tag.  Remember that the array of
   frames is an array of handles-for/pointers-to frame structures and
   that adxGet() returns the address of a handle; hence there is an
   extra level of indirection.  NOTE that the existing array descriptor
   for frames is left in place, albeit now with zero elements. */

    i = adxCount (tag->frames) ;
    while (i-- > 0) {
        Id3Frame  *frame = adxGet (tag->frames, i) ;
        id3FrameDestroy (*frame) ;
        adxDelete (tag->frames, 1, i) ;
    }

/* Erase the remaining fields in the tag. */

    tag->overrideVersion = false ;
    tag->encoding = ID3_UTF16_WITH_BOM ;	/* Common to ID3v2.3 & v2.4. */
    tag->flags = 0 ;

    if (tag->song != NULL) {
        free (tag->song) ;  tag->song = NULL ;
    }
    if (tag->artist != NULL) {
        free (tag->artist) ;  tag->artist = NULL ;
    }
    if (tag->album != NULL) {
        free (tag->album) ;  tag->album = NULL ;
    }
    if (tag->comment != NULL) {
        free (tag->comment) ;  tag->comment = NULL ;
    }

    tag->year = 0 ;
    tag->track = 0 ;
    tag->genre = 0 ;

    if (tag->genreName != NULL) {
        free (tag->genreName) ; tag->genreName = NULL ;
    }

    tag->v2Flags = 0 ;
    tag->v2Size = 0 ;

    if (tag->v2Buffer != NULL) {
        free (tag->v2Buffer) ;  tag->v2Buffer = NULL ;
    }

    tag->unsync = false ;

    tag->exth.version = tag->version ;
    tag->exth.fields = 0 ;
    tag->exth.length = 0 ;
    tag->exth.flags = 0 ;
    tag->exth.padding = 0 ;
    tag->exth.originalCRC = 0 ;
    tag->exth.computedCRC = 0 ;
    tag->exth.isUpdate = false ;
    tag->exth.restrictions = 0 ;

    if (tag->exth.header != NULL) {
        free (tag->exth.header) ;  tag->exth.header = NULL ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagFindFrame ()

    Find an ID3 Frame in an ID3 Tag.


Purpose:

    The id3TagFindFrame() function searches for a frame of a specified type
    in an ID3 tag's list of frames.

    The arguments to this function depend on the frame type, as shown in
    the different invocations below.

        NOTE that the invocations for the *generic*, user-defined
        text/URL types is NOT for the *predefined*, user-defined
        text/URL types!  The predefined types have separate type
        enumerations which, by their enumeration, distinguish
        between the base "T/WXXX:<description>" encodings.  As
        a result, a simple "id3TagFindFrame (tag, type)" call
        will suffice for the predefined, user-defined types.

        For example, ID3_ACOUSTID.  When id3FrameDecode() sees
        a "TXXX" frame with a description of "Acoustid Id", it
        automatically assigns ID3_ACOUSTID as the frame's type,
        not ID3_TXXX_GENERIC.  An application should search for
        this frame type with:
                   id3TagFindFrame (tag, ID3_ACOUSTID) ;
        A similar, but distinctly different search:
            id3TagFindFrame (tag, ID3_TXXX_GENERIC, "AcoustidId") ;
        will NOT find the ID3_ACOUSTID frame.


    Invocation (General):

        frame = id3TagFindFrame (tag, type, ...) ;

    Invocation (Picture):

        frame = id3TagFindFrame (tag, ID3_ATTACHEDPIC,
                                 pictureType, description) ;

    Invocation (Comment):

        frame = id3TagFindFrame (tag, ID3_COMMENT, language, description) ;

    Invocation (Encryption and Group ID Registrations):

        frame = id3TagFindFrame (tag, ID3_ENCRYPTREG, symbol) ;
        frame = id3TagFindFrame (tag, ID3_GROUPIDREG, symbol) ;

    Invocation (General Object):

        frame = id3TagFindFrame (tag, ID3_GENERALOBJ, description) ;

    Invocation (Popularimeter):

        frame = id3TagFindFrame (tag, ID3_POPULARIMETER, email) ;

    Invocation (Private):

        frame = id3TagFindFrame (tag, ID3_PRIVATE, ownerID) ;

    Invocation (Text Information):

        frame = id3TagFindFrame (tag, type) ;

    Invocation (Unsupported Text Information):

        frame = id3TagFindFrame (tag, ID3_TEXT_GENERIC, frameID) ;

    Invocation (Unsynchronized Lyrics):

        frame = id3TagFindFrame (tag, ID3_UNSYNCEDLYRICS,
                                 language, description) ;

    Invocation (User-Defined Text Information):

        frame = id3TagFindFrame (tag, ID3_TXXX_GENERIC, description) ;

    Invocation (Unique File ID):

        frame = id3TagFindFrame (tag, ID3_UNIQUEFILEID, ownerID) ;

    Invocation (URL Link):

        frame = id3TagFindFrame (tag, type) ;

    Invocation (Unsupported URL Link):

        frame = id3TagFindFrame (tag, ID3_WURL_GENERIC, frameID) ;

    Invocation (User-Defined URL Link):

        frame = id3TagFindFrame (tag, ID3_WXXX_GENERIC, description) ;

    where

        <tag>		- I
            is the handle, returned by id3FrameCreate(), of the tag.
        <type>		- I
            is the frame type (defined in "id3_util.h").
        <pictureType>	- I
            is the attached-picture type (defined in "id3_util.h").
        <description>	- I
            is the description string to be matched.
        <language>	- I
            is the 3-character language code in comment frames.  If this
            argument is NULL, "eng" is assumed.
        <email>		- I
            is the email address to be matched.
        <ownerID>	- I
            is the owner ID string for private and UFID frames.
        <frame>		- O
            returns the frame's handle; NULL is returned if a frame of
            the specified type is not found.

*******************************************************************************/


Id3Frame  id3TagFindFrame (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        ...)
#    else
        tag, type, va_alist)

        Id3Tag  tag ;
        Id3FrameType  type ;
        va_dcl
#    endif

{    /* Local variables. */
    char  language[4] ;
    char  *description, *email, *frameID, *ownerID, *s ;
    Id3PictureType  pictureType ;
    size_t  i, count ;
    unsigned  int  symbol ;
    va_list  ap ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagFindFrame) NULL tag handle: ") ;
        return (NULL) ;
    }

/* Get the frame-type-specific qualifiers passed in by the caller. */

#if HAVE_STDARG_H
    va_start (ap, type) ;
#else
    va_start (ap) ;
#endif

				/* Cygwin's GCC warns that arguments "may be
				   used uninitialized" in the section that
				   follows this one, although the arguments
				   are correctly used in the corresponding
				   frame-type-specific clauses.  So, initialize
				   them anyway! */

    description = "" ;
    email = "" ;
    frameID = "" ;
    strcpy (language, "eng") ;
    ownerID = "" ;
    symbol = ~0 ;

    if (type == ID3_ATTACHEDPIC) {
        pictureType = va_arg (ap, Id3PictureType) ;
        description = va_arg (ap, char *) ;
        if (description == NULL)  description = "" ;
    } else if (type == ID3_COMMENT) {
        s = va_arg (ap, char *) ;
        if (s == NULL)  s = "eng" ;
        strncpym (language, s, 3, sizeof language) ;
        description = va_arg (ap, char *) ;
        if (description == NULL)  description = "" ;
    } else if ((type == ID3_ENCRYPTREG) ||
               (type == ID3_GROUPIDREG)) {
        symbol = va_arg (ap, unsigned int) ;
    } else if (type == ID3_GENERALOBJ) {
        description = va_arg (ap, char *) ;
        if (description == NULL)  description = "" ;
    } else if (type == ID3_POPULARIMETER) {
        email = va_arg (ap, char *) ;
        if (email == NULL)  email = "" ;
    } else if (type == ID3_PRIVATE) {
        ownerID = va_arg (ap, char *) ;
        if (ownerID == NULL)  ownerID = "" ;
    } else if (type == ID3_TEXT_GENERIC) {
        frameID = va_arg (ap, char *) ;
        if (frameID == NULL)  frameID = "" ;
    } else if (type == ID3_TXXX_GENERIC) {
        description = va_arg (ap, char *) ;
        if (description == NULL)  description = "" ;
    } else if (type == ID3_UNIQUEFILEID) {
        ownerID = va_arg (ap, char *) ;
        if (ownerID == NULL)  ownerID = "" ;
    } else if (type == ID3_UNSYNCEDLYRICS) {
        s = va_arg (ap, char *) ;
        if (s == NULL)  s = "eng" ;
        strncpym (language, s, 3, sizeof language) ;
        description = va_arg (ap, char *) ;
        if (description == NULL)  description = "" ;
    } else if (type == ID3_WURL_GENERIC) {
        frameID = va_arg (ap, char *) ;
        if (frameID == NULL)  frameID = "" ;
    } else if (type == ID3_WXXX_GENERIC) {
        description = va_arg (ap, char *) ;
        if (description == NULL)  description = "" ;
    }

    va_end (ap) ;

/* Loop through the frames, looking for the desired frame type.  Remember
   that the array of frames is an array of handles-for/pointers-to frame
   structures and that adxGet() returns the address of a handle; hence the
   extra level of indirection. */

    count = adxCount (tag->frames) ;
    for (i = 0 ;  i < count ;  i++) {
        Id3Frame  *frame = adxGet (tag->frames, i) ;
        if ((*frame)->type != type)  continue ;
        if (type == ID3_ATTACHEDPIC) {
            if (((*frame)->fdata.apic.type == pictureType) &&
                (strcmp ((*frame)->fdata.apic.description, description) == 0))
                return (*frame) ;			/* Found. */
        } else if (type == ID3_COMMENT) {
            if ((strncmp ((*frame)->fdata.comm.language, language, 3) == 0) &&
                (strcmp ((*frame)->fdata.comm.description, description) == 0))
                return (*frame) ;			/* Found. */
        } else if ((type == ID3_ENCRYPTREG) ||
                   (type == ID3_GROUPIDREG)) {
            if ((*frame)->fdata.regi.symbol == symbol)
                return (*frame) ;
        } else if (type == ID3_GENERALOBJ) {
            if (strcmp ((*frame)->fdata.geob.description, description) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_POPULARIMETER) {
            if (strcmp ((*frame)->fdata.play.email, email) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_PRIVATE) {
            if (strcmp ((*frame)->fdata.priv.ownerID, ownerID) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_TEXT_GENERIC) {
            if (strcmp ((*frame)->frameID, frameID) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_TXXX_GENERIC) {
            if (strcmp ((*frame)->fdata.text.description, description) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_UNIQUEFILEID) {
            if (strcmp ((*frame)->fdata.ufid.ownerID, ownerID) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_UNSYNCEDLYRICS) {
            if ((strncmp ((*frame)->fdata.uslt.language, language, 3) == 0) &&
                (strcmp ((*frame)->fdata.uslt.description, description) == 0))
                return (*frame) ;			/* Found. */
        } else if (type == ID3_WURL_GENERIC) {
            if (strcmp ((*frame)->frameID, frameID) == 0)
                return (*frame) ;			/* Found. */
        } else if (type == ID3_WXXX_GENERIC) {
            if (strcmp ((*frame)->fdata.wurl.description, description) == 0)
                return (*frame) ;			/* Found. */
        } else {
            return (*frame) ;				/* Found. */
        }
    }

    return (NULL) ;					/* Not found. */

}

/*!*****************************************************************************

Procedure:

    id3TagFrameCount ()

    Get the Number of Frames in an ID3 Tag.


Purpose:

    The id3TagFrameCount() function returns the number of frames in
    an ID3 tag.


    Invocation:

        numFrames = id3TagFrameCount (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3FrameCreate().
        <numFrames>	- O
            returns the number of frames in the tag.

*******************************************************************************/


size_t  id3TagFrameCount (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL)
        return (0) ;
    else
        return (adxCount (tag->frames)) ;

}

/*!*****************************************************************************

Procedure:

    id3TagGetFrame ()

    Get the I-th Frame in an ID3 Tag.


Purpose:

    The id3TagGetFrame() function returns the I-th frame in an ID3 tag.
    Index I is in the range 0..N-1, where N is the number of frames in
    the tag.


    Invocation:

        frame = id3TagGetFrame (tag, index) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <index>		- I
            is the index 0..N-1 of the desired frame, where N is the number
            of frames in the tag.
        <frame>		- O
            returns the frame handle; NULL is returned if the index is
            out of range.

*******************************************************************************/


Id3Frame  id3TagGetFrame (

#    if PROTOTYPES
        Id3Tag  tag,
        size_t  which)
#    else
        tag, which)

        Id3Tag  tag ;
        size_t  which ;
#    endif

{

    if ((tag == NULL) || (tag->frames == NULL)) {
        return (NULL) ;
    } else {
        Id3Frame  *frame = adxGet (tag->frames, (ssize_t) which) ;
        return ((frame == NULL) ? NULL : *frame) ;
    }

}

/*!*****************************************************************************

Procedure:

    id3TagMerge ()

    Merge the Contents of Two ID3 Tags into a Third Tag.


Purpose:

    Function id3TagMerge() merges the contents of two ID3 tags into a third
    tag.  A copy is made of the first, "primary" tag and then the frames in
    the second, "overlay" tag are copied in, replacing existing frames in
    the copy when appropriate.

    The function basically is intended to be used as follows:

        (1) Create a tag with a full complement of frames.  This is
            the primary tag.  For example, read and decode a tag
            "existingTag" from an MP3 file.  This tag will likely
            have an assortment of frames, not necessarily a "full"
            complement.

                Id3Tag  existingTag = ... read/encoded from MP3 file ...

        (2) Create a tag comprised of only the frames to be updated.
            This is the overlay tag.  For example, if the album name
            only is being updated, create a tag "updatesOnlyTag" with
            a single frame, an ID3_ALBUM frame:

                Id3Tag  updatesOnlyTag ;
                ...
                id3TagCreate (ID3V_V2_3, &updatesOnlyTag) ;
                id3SetAlbum (updatesOnlyTag, "Beatles 65") ;

        (3) Call id3TagMerge() to create a copy of the primary tag with
            the overlay tag updates.  Continuing the example above:

				-- Only updates album frame.
                id3TagMerge (existingTag, updatesOnlyTag) ;


    Invocation:

        merge = id3TagMerge (primary, overlay) ;

    where

        <primary>	- I
            is the tag being duplicated.
        <overlay>	- I
            is a tag which supplies missing fields in the primary tag.
        <merge>		- O
            returns a handle for the new tag; NULL is returned upon error.

*******************************************************************************/


Id3Tag  id3TagMerge (

#    if PROTOTYPES
        Id3Tag  primary,
        Id3Tag  overlay)
#    else
        primary, overlay)

        Id3Tag  primary ;
        Id3Tag  overlay ;
#    endif

{    /* Local variables */
    Id3Frame  newFrame, *oldFrame ;
    Id3Tag  newTag ;
    size_t  i, count ;



    if ((primary == NULL) && (overlay == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagMerge) NULL primary and/or overlay tag handles: ") ;
        return (NULL) ;
    } else if (primary == NULL) {
        return (id3TagCopy (overlay)) ;
    } else if (overlay == NULL) {
        return (id3TagCopy (primary)) ;
    }

/* Make a copy of the primary tag. */

    newTag = id3TagCopy (primary) ;
    if (newTag == NULL) {
        LGE "(id3TagMerge) Error copying primary tag %p.\nid3TagCopy: ",
            (void *) primary) ;
        return (NULL) ;
    }

    LGI "(id3TagMerge) Copying primary v0x%06lX %p, overlay v0x%06lX %p to new v0x%06lX tag %p\n",
        primary->version, (void *) primary,
        overlay->version, (void *) overlay,
        newTag->version, (void *) newTag) ;

/* Copy the overlay tag's frames to the new tag, overwriting existing frames
   if necessary. */

    count = adxCount (overlay->frames) ;

    for (i = 0 ;  i < count ;  i++) {

        oldFrame = adxGet (overlay->frames, i) ;

        newFrame = id3FrameCopy (*oldFrame) ;
        if (newFrame == NULL) {
            LGE "(id3TagMerge) Error duplicating frame %p.\nid3FrameCopy: ",
                (void *) *oldFrame) ;
            PUSH_ERRNO ;  id3TagDestroy (newTag) ;  POP_ERRNO ;
            return (NULL) ;
        }

        if (id3TagAddFrame (newTag, newFrame, true)) {
            LGE "(id3TagMerge) Error adding new frame %p to new tag %p.\nid3TagAddFrame: ",
                (void *) newFrame, (void *) newTag) ;
            PUSH_ERRNO ;  id3TagDestroy (newTag) ;  POP_ERRNO ;
            return (NULL) ;
        }

    }

/* Copy additional items other than the frames from the overlay tag to the
   new tag.  TAG311 builds an overlay tag to update frames in the existing
   (primary) tags of one or more audio files.  As each file is processed,
   the file's existing tag is merged with the overlay tag.  Copying these
   additional items (other than the frames) from the overlay tag enables
   TAG311 to also force a rewrite of the files' existing/updated tags in
   different ID3 versions and/or in unsynchronized form. */

    if (overlay->overrideVersion)  newTag->version = overlay->version ;
    newTag->unsync = overlay->unsync ;

#ifdef OLD_EXTENDED_HEADER_HANDLING
    if ((newTag->v2Flags & ID3_V2_EXTENDED_HEADER) &&
        (newTag->exth.version != newTag->version)) {
			/* Mismatched tag/header versions; drop header. */
        newTag->exth.fields = 0 ;
        newTag->exth.length = 0 ;
        newTag->exth.flags = 0 ;
        newTag->exth.padding = 0 ;
        newTag->exth.originalCRC = 0 ;
        newTag->exth.computedCRC = 0 ;
        newTag->exth.isUpdate = false ;
        newTag->exth.restrictions = 0 ;
        if (newTag->exth.header != NULL)  free (newTag->exth.header) ;
        newTag->exth.header = NULL ;
    }
#else
    if (overlay->v2Flags & ID3_V2_EXTENDED_HEADER) {
        newTag->v2Flags |= ID3_V2_EXTENDED_HEADER ;
        newTag->exth.flags |= overlay->exth.flags ;
    }
#endif

    return (newTag) ;

}

/*!*****************************************************************************

Procedure:

    id3TagSortFrames ()

    Sort the Frames in an ID3 Tag in My Preferred Order.


Purpose:

    The id3TagSortFrames() function "sorts" the frames in an ID3 tag
    in my preferred order, i.e., the order in which I want to see them
    printed when an ID3v2 tag is displayed.  This order is the order,
    more or less, in which ID3v1 fields are printed by id3Dump():

        Track
        Song
        Artist
        Album
        Year
        Genre
        Comment
        ... remaining non-picture frames ...
        ... picture frames ...


    Invocation:

        status = id3TagSortFrames (tag) ;

    where

        <tag>		- I
            is the handle, returned by id3TagCreate(), of the tag whose
            frames are to be sorted.
        <status>	- O
            returns the status of sorting the frames, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3TagSortFrames (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    ArrayDx  oldList, newList ;
    Id3Frame  frame, *pframe ;
    int  step ;
    size_t  count, i, next ;



    if ((tag == NULL) || (tag->frames == NULL))  return (0) ;

    oldList = tag->frames ;
    count = adxCount (oldList) ;
    if (count == 0)  return (0) ;

/* Create an empty list/array of frames. */

    if (adxCreate (0, sizeof (Id3Frame), NULL, 0, AdxDynamic, &newList)) {
        LGE "(id3TagSortFrames) Error allocating array of frames.\nadxCreate: ") ;
        return (errno) ;
    }

/*******************************************************************************
    Loop, copying the frames from the old list to the new list.  The first
    frames copied are the ones I want put in my preferred order: track, title,
    artist, etc.  After them, copy the remaining, non-picture frames in the
    old list in whatever order.  And, finally copy the picture frames to the
    new list.
*******************************************************************************/

    next = 0 ;

    for (step = 1 ;  next < adxCount (oldList) ;  step++) {

        switch (step) {
        case 1:
            frame = id3TagFindFrame (tag, ID3_TRACK) ;
            break ;
        case 2:
            frame = id3TagFindFrame (tag, ID3_TITLE) ;
            break ;
        case 3:
            frame = id3TagFindFrame (tag, ID3_ARTIST) ;
            break ;
        case 4:
            frame = id3TagFindFrame (tag, ID3_ALBUM) ;
            break ;
        case 5:
            frame = id3TagFindFrame (tag, ID3_YEAR) ;
            break ;
        case 6:
            frame = id3TagFindFrame (tag, ID3_GENRE) ;
            break ;
        case 7:
            frame = id3TagFindFrame (tag, ID3_COMMENT, NULL, NULL) ;
            break ;
#define  MY_PARTICULAR_USE_CASE  1
#ifdef MY_PARTICULAR_USE_CASE
			/*******************************************************
			    Frames I use for my Beatles/Stones/Greeny covers
			    web page:
			        TXXX (Source) - audio source previously encoded
			                        in ID3v1 comments.
			        WOAF - link to online audio file.
			        WOAR - link to artist's website.
			        WXXX (Album) - link to album web page.
			    The AWK script I use to generate the covers pages
			    doesn't care, but I like to see them in this order.
			*******************************************************/
        case 8:
            frame = id3TagFindFrame (tag, ID3_TXXX_GENERIC, "Source") ;
            break ;
        case 9:
            frame = id3TagFindFrame (tag, ID3_WWWAUDIOFILE) ;
            break ;
        case 10:
            frame = id3TagFindFrame (tag, ID3_WWWARTIST) ;
            break ;
        case 11:
            frame = id3TagFindFrame (tag, ID3_WXXX_GENERIC, "Album") ;
            break ;
#endif
        default:	/* Remaining frames following preferred frames. */
            pframe = (Id3Frame *) adxGet (oldList, next) ;
            frame = *pframe ;
            break ;
        }

        if (frame == NULL)  continue ;		/* Preferred frame missing? */

        if (frame->type == ID3_ATTACHEDPIC) {
            next++ ;
            continue ;
        }

/* Add the frame to the new array. */

        if (adxAdd (newList, &frame, 1, -1)) {
            LGE "(id3TagSortFrames) Error adding %s frame to new list %p.\nadxAdd: ",
                frame->info->name) ;
            goto OnError ;
        }


/* Delete the frame from the old array. */

        adxDelete (tag->frames, 1, id3TagIndexOf (tag, frame) - 1) ;

    }

/*******************************************************************************
    Now, only picture frames remain in the old list.  Append them to
    the new list of frames.
*******************************************************************************/

    while (adxCount (oldList) > 0) {

        pframe = (Id3Frame *) adxGet (oldList, 0) ;
        frame = *pframe ;

        if (adxAdd (newList, &frame, 1, -1)) {
            LGE "(id3TagSortFrames) Error adding %s frame to new list %p.\nadxAdd: ",
                frame->info->name) ;
            goto OnError ;
        }

        adxDelete (tag->frames, 1, 0) ;

    }

/*******************************************************************************
    Lastly, Replace the old list in the tag with the new list.
*******************************************************************************/

    adxDestroy (oldList) ;
    tag->frames = newList ;

    return (0) ;


/*******************************************************************************
    On Error:  Destroy the frames that had been successfully moved from the
    old list of frames to the new list.  Since these frames are no longer
    part of the tag, they would not otherwise be destroyed when the tag is
    destroyed.
*******************************************************************************/

OnError:

    PUSH_ERRNO ;
			/* Destroy frames in new array. */
    i = adxCount (newList) ;
    while (i-- > 0) {
        pframe = adxGet (newList, i) ;
        id3FrameDestroy (*pframe) ;
    }

    adxDestroy (newList) ;

    POP_ERRNO ;

    return (errno) ;

}

/*!*****************************************************************************

Procedure:

    id3TagDecodeExt ()

    Decode a Tag's Extended Header.


Purpose:

    Function id3TagDecodeExt() decodes the optional extended header
    that follows a tag's regular, 10-byte header.  If there is an
    extended header, its values are stored in the tag structure.


    Invocation:

        status = id3TagDecodeExt (tag, buffer, length, &skip) ;

    where

        <tag>		- I
            is the handle returned by id3TagCreate().
        <buffer>	- I
            is the buffer containing the extended header.
        <length>	- I
            is the length in bytes of the buffer.
        <skip>		- O
            returns the offset in bytes of the next field in the buffer
            after the header.
        <status>	- O
            returns the status of decoding the extended header, zero
            if there were no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3TagDecodeExt (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *buffer,
        size_t  length,
        size_t  *skip)
#    else
        tag, buffer, length, skip)

        Id3Tag  tag ;
        char  *buffer ;
        size_t  length ;
        size_t  *skip ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (skip != NULL)  *skip = 0 ;

    if ((tag == NULL) || (buffer == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagDecodeExt) NULL tag handle or buffer.\n") ;
        return (errno) ;
    }

    if (!(tag->v2Flags & ID3_V2_EXTENDED_HEADER))  return (0) ;

/*******************************************************************************
    Decode an ID3v2.3 extended header.
*******************************************************************************/

    tag->exth.fields = 0 ;
    tag->exth.version = tag->version ;

    if (tag->version < ID3V_V2_4) {

        if (length < ID3_V2_3_EXTHDR_BASE_SIZE) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3TagDecodeExt) Insufficient data (%"PRIuSIZE_T" bytes) in ID3v2.3 buffer.\n",
                length) ;
            return (errno) ;
        }

        i = 0 ;

        tag->exth.length = buffer[i++] & 0x0FF ;
        tag->exth.length <<= 8 ;
        tag->exth.length |= buffer[i++] & 0x0FF ;
        tag->exth.length <<= 8 ;
        tag->exth.length |= buffer[i++] & 0x0FF ;
        tag->exth.length <<= 8 ;
        tag->exth.length |= buffer[i++] & 0x0FF ;
        tag->exth.length += 4 ;			/* Include size field. */

        tag->exth.flags = buffer[i++] & 0x0FF ;
        tag->exth.flags <<= 8 ;
        tag->exth.flags |= buffer[i++] & 0x0FF ;

        tag->exth.padding = buffer[i++] & 0x0FF ;
        tag->exth.padding <<= 8 ;
        tag->exth.padding |= buffer[i++] & 0x0FF ;
        tag->exth.padding <<= 8 ;
        tag->exth.padding |= buffer[i++] & 0x0FF ;
        tag->exth.padding <<= 8 ;
        tag->exth.padding |= buffer[i++] & 0x0FF ;

        if (tag->exth.flags & 0x8000) {
            if (length < (ID3_V2_3_EXTHDR_BASE_SIZE + 4)) {
                SET_ERRNO (EINVAL) ;
                LGE "(id3TagDecodeExt) Insufficient data (%"PRIuSIZE_T" bytes) in ID3v2.3 buffer.\n",
                    length) ;
                return (errno) ;
            }
            tag->exth.originalCRC = buffer[i++] & 0x0FF ;
            tag->exth.originalCRC <<= 8 ;
            tag->exth.originalCRC |= buffer[i++] & 0x0FF ;
            tag->exth.originalCRC <<= 8 ;
            tag->exth.originalCRC |= buffer[i++] & 0x0FF ;
            tag->exth.originalCRC <<= 8 ;
            tag->exth.originalCRC |= buffer[i++] & 0x0FF ;
            tag->exth.fields |= ID3_EXTH_CRC_PRESENT ;
        }

        LGI "(id3TagDecodeExt) %"PRIuSIZE_T"-byte ID3v2.3 extended header (0x%04lX, 0x%08lX)\n",
            tag->exth.length, tag->exth.flags, tag->exth.originalCRC) ;

    }

/*******************************************************************************
    Decode an ID3v2.4 extended header.
*******************************************************************************/

    else {

        if (length < ID3_V2_4_EXTHDR_BASE_SIZE) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3TagDecodeExt) Insufficient data (%"PRIuSIZE_T" bytes) in ID3v2.4 buffer.\n",
                length) ;
            return (errno) ;
        }

        tag->exth.length = id3DecodeSSI (buffer, 4) ;
        length -= 4 ;

        i = 4 ;

        i++ ;  length-- ;		/* Skip (# of flag bytes, always 1). */

        tag->exth.flags = buffer[i++] & 0x0FF ;
        length -= 2 ;

        if (length < tag->exth.length) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3TagDecodeExt) Insufficient data (%"PRIuSIZE_T" bytes) in ID3v2.4 buffer.\n",
                length) ;
            return (errno) ;
        }

        if (tag->exth.flags & ID3_EXTH_TAG_IS_UPDATE) {
            i++ ;  length-- ;		/* Flag data length is always 0;
					   bit set indicates tag is update. */
            tag->exth.isUpdate = true ;
            tag->exth.fields |= ID3_EXTH_TAG_IS_UPDATE ;
        }

        if (tag->exth.flags & ID3_EXTH_CRC_PRESENT) {
            if ((i + 6) <= length) {
                i++ ;  length-- ;	/* Flag data length is always 5. */
                tag->exth.originalCRC = id3DecodeSSI (&buffer[i], 5) ;
                i += 5 ;  length -= 5 ;
                tag->exth.fields |= ID3_EXTH_CRC_PRESENT ;
            }
        }

        if (tag->exth.flags & ID3_EXTH_TAG_RESTRICTIONS) {
            if ((i + 2) <= length) {
                i++ ;  length-- ;	/* Flag data length is always 1. */
                tag->exth.restrictions = buffer[i++] & 0x0FF ;  length-- ;
                tag->exth.fields |= ID3_EXTH_TAG_RESTRICTIONS ;
            }
        }

        LGI "(id3TagDecodeExt) %"PRIuSIZE_T"-byte ID3v2.4 extended header (0x%02lX, 0x%08lX)\n",
            tag->exth.length, tag->exth.flags, tag->exth.originalCRC) ;

        if (i != tag->exth.length) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3TagDecodeExt) %"PRIuSIZE_T" consumed bytes != %"PRIuSIZE_T"-byte header length.\n",
                i, tag->exth.length) ;
            return (errno) ;
        }

    }

    if (skip != NULL)  *skip = tag->exth.length ;

    tag->exth.header = memdup (buffer, tag->exth.length) ;
    if (tag->exth.header == NULL) {
        LGE "(id3TagDecodeExt) Error duplicating %"PRIuSIZE_T"-byte extended header.\nmemdup: ",
            tag->exth.length) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3TagEncodeExt ()

    Encode an ID3v2 Extended Header into its File Format.


Purpose:

    Function id3TagEncodeExt() encodes an ID3v2 tag's extended header
    into a byte buffer.

    The bits in tag->exth.fields determine Which optional header fields
    to encode in the header:

        ID3_EXTH_CRC_PRESENT - include computed CRC (ID3v2.3 and ID3v2.4)
        ID3_EXTH_TAG_IS_UPDATE - include is-update flag (ID3v2.4 only)
        ID3_EXTH_TAG_RESTRICTIONS - include tag restrictions (ID3v2.4 only)

    These field bits are the same bits used in ID3v2.4 extended header
    flags.  In the case of an ID3v2.3 header with ID3_EXTH_CRC_PRESENT
    set in the fields mask, id3TagEncode() will correctly set the
    ID3_EXTH_V2_3_CRC_PRESENT bit in the encoded header flags.

    ID3v2.3's padding value is taken from tag->exth.padding and both
    versions' CRCs are taken from tag->exth.computedCRC.

        NOTE: It can be useful to call this function once to determine
        the size of the extended header (and thus allocate space for
        it in the encoded tag) and then a second time to update the
        padding and CRC values after the entire tag has been encoded.


    Invocation:

        length = id3TagEncodeExt (tag, buffer) ;

    where

        <tag>		- I
            is the handle returned by id3TagCreate().
        <buffer>	- O
            is the buffer into which the extended header will be encoded.
            The buffer should be at least ID3_EXTHDR_MAX_SIZE bytes long.
            (ID3v2.3 extended headers are at most 15 bytes and ID3v2.4 15.)
        <length>	- O
            returns the length in bytes of the entire encoded header.

*******************************************************************************/


static  size_t  id3TagEncodeExt (

#    if PROTOTYPES
        Id3Tag  tag,
        unsigned  char  *buffer)
#    else
        tag, buffer)

        Id3Tag  tag ;
        unsigned  char  *buffer ;
#    endif

{    /* Local variables. */
    size_t  i, length, number ;



    if ((tag == NULL) || (buffer == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagEncodeExt) NULL tag handle or buffer.\n") ;
        return (0) ;
    }

    if (!(tag->v2Flags & ID3_V2_EXTENDED_HEADER))  return (0) ;

/*******************************************************************************
    Encode an ID3v2.3 extended header.
*******************************************************************************/

    if (tag->version < ID3V_V2_4) {

        number = ID3_V2_3_EXTHDR_BASE_SIZE - 4 ;	/* Minus size field. */
        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT)  number += 4 ;

        buffer[3] = number & 0x0FF ;
        number >>= 8 ;
        buffer[2] = number & 0x0FF ;
        number >>= 8 ;
        buffer[1] = number & 0x0FF ;
        number >>= 8 ;
        buffer[0] = number & 0x0FF ;

        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT)
            tag->exth.flags |= ID3_EXTH_V2_3_CRC_PRESENT ;	/* Set bit. */
        else
            tag->exth.flags &= ~ID3_EXTH_V2_3_CRC_PRESENT ;	/* Clear bit. */

        buffer[4] = (tag->exth.flags >> 8) & 0x0FF ;
        buffer[5] = tag->exth.flags & 0x0FF ;

        number = tag->exth.padding ;

        buffer[9] = number & 0x0FF ;
        number >>= 8 ;
        buffer[8] = number & 0x0FF ;
        number >>= 8 ;
        buffer[7] = number & 0x0FF ;
        number >>= 8 ;
        buffer[6] = number & 0x0FF ;

        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT) {
            number = tag->exth.computedCRC ;
            buffer[10] = number & 0x0FF ;
            number >>= 8 ;
            buffer[11] = number & 0x0FF ;
            number >>= 8 ;
            buffer[12] = number & 0x0FF ;
            number >>= 8 ;
            buffer[13] = number & 0x0FF ;
            length = 14 ;			/* With CRC */
        } else {
            length = 10 ;			/* Without CRC */
        }

    }

/*******************************************************************************
    Encode an ID3v2.4 extended header.
*******************************************************************************/

    else {

        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT)
            tag->exth.flags |= ID3_EXTH_CRC_PRESENT ;		/* Set bit. */
        else
            tag->exth.flags &= ~ID3_EXTH_CRC_PRESENT ;		/* Clear bit. */

        if (tag->exth.fields & ID3_EXTH_TAG_IS_UPDATE)
            tag->exth.flags |= ID3_EXTH_TAG_IS_UPDATE ;		/* Set bit. */
        else
            tag->exth.flags &= ~ID3_EXTH_TAG_IS_UPDATE ;	/* Clear bit. */

        if (tag->exth.fields & ID3_EXTH_TAG_RESTRICTIONS)
            tag->exth.flags |= ID3_EXTH_TAG_RESTRICTIONS ;	/* Set bit. */
        else
            tag->exth.flags &= ~ID3_EXTH_TAG_RESTRICTIONS ;	/* Clear bit. */

        i = 4 ;

        buffer[i++] = 1 ;		/* One-byte flags. */
        buffer[i++] = tag->exth.flags & 0x0FF ;

        if (tag->exth.fields & ID3_EXTH_TAG_IS_UPDATE) {
            buffer[i++] = 0 ;		/* Implicit in flags, zero data. */
        }

        if (tag->exth.fields & ID3_EXTH_CRC_PRESENT) {
            buffer[i++] = 5 ;		/* 5-byte, 35-bit synchsafe CRC-32. */
            id3EncodeSSI (tag->exth.computedCRC, 5, (char *) &buffer[i]) ;
            i += 5 ;
        }

        if (tag->exth.fields & ID3_EXTH_TAG_RESTRICTIONS) {
            buffer[i++] = 1 ;		/* One-byte bit mask. */
            buffer[i++] = tag->exth.restrictions & 0x0FF ;
        }

        length = i ;
					/* Go back and fill in size. */
        id3EncodeSSI (length, 4, (char *) &buffer[0]) ;

    }

    LGI "(id3TagEncodeExt) %"PRIuSIZE_T"-byte extended header, flags 0x%04lX, fields 0x%02lX\n",
        length, tag->exth.flags, tag->exth.fields) ;

    return (length) ;

}

/*!*****************************************************************************

Procedure:

    id3TagIndexOf ()

    Get the Index of a Frame in an ID3 Tag's List of Frames.


Purpose:

    Function id3TagIndexOf() returns the index 1..N of a frame in a tag's
    list of frames.


    Invocation:

        index = id3TagIndexOf (tag, frame) ;

    where

        <tag>		- I
            is the handle, returned by id3TagCreate(), of the tag.
        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame.
        <index>		- O
            returns the index 1..N of the frame in the tag's frame list;
            zero is returned if the frame is not found in the list.

*******************************************************************************/


static  size_t  id3TagIndexOf (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3Frame  frame)
#    else
        tag, frame)

        Id3Tag  tag ;
        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    Id3Frame  *pframe ;
    size_t  count, i ;



    if ((tag == NULL) || (frame == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3TagIndexOf) NULL tag or frame handle: ") ;
        return (0) ;
    }

    for (i = 0, count = adxCount (tag->frames) ;  i < count ;  i++) {
        pframe = adxGet (tag->frames, i) ;
        if (*pframe == frame)  return (i+1) ;		/* Found? */
    }

    return (0) ;					/* Not found. */

}
