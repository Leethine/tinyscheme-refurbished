/* $Id: id3_value.c,v 1.3 2023/10/01 17:13:20 alex Exp alex $ */
/*******************************************************************************

File:

    id3_value.c

    ID3 Tag Utilities - Frame Values


Author:    Alex Measday


Purpose:

    This file is an internal part of the ID3_UTIL package and provides
    functions for the manipulation of ID3 frames' values.  An ID3 tag
    is composed of one or more ID3 frames.  Each frame, in turn, contains
    one or more data values; e.g., character strings in the case of text
    frames.  This file, as its name suggests, implements the functions
    for dealing with these data values.

    ID3v2.4 text frames, in general, allow multiple string values in
    a single frame, with two consecutive strings separated by a NUL
    character.  For example, the TMCL, 'Musician credits list' frame
    is nominally a list of stings:

        "<instrument1>", NUL, "<musician(s)>", NUL, ...,
            NUL, "<instrument2>", NUL, "<musician(s)>", NUL, ...

    where NUL is the NUL character in the specified text encoding.
    (In ID3v2.3 text frames, multiple values were stored in a single
    string, with the values separated by "/".)


Public Procedures:

    id3ValueCopy() - duplicates an ID3 value.
    id3ValueCreate() - creates an empty ID3 value.
    id3ValueDestroy() - destroys an ID3 value.
    id3ValueDump() - dumps an ID3 value.
    id3ValueErase() - erases the contents of an ID3 value.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "id3_util.h"			/* ID3 tag utilities. */
#include  "id3_internals.h"		/* Internal definitions. */


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  id3_util_debug

/*!*****************************************************************************

Procedure:

    id3ValueCopy ()

    Duplicate an ID3 Value.


Purpose:

    The id3ValueCopy() function creates a full copy of an existing,
    frame field's value.


    Invocation:

        newValue = id3ValueCopy (oldValue) ;

    where

        <oldValue>	- I
            is the existing field value to copy.
        <newValue>	- O
            returns a handle for the new value; NULL is returned upon error.
            After the value is no longer needed, id3ValueDestroy() should
            be called to dispose of the value.

*******************************************************************************/


Id3Value  id3ValueCopy (

#    if PROTOTYPES
        Id3Value  oldValue)
#    else
        oldValue)

        Id3Value  oldValue ;
#    endif

{    /* Local variables. */
    Id3Value  newValue ;



/* Allocate a new value structure. */

    if (id3ValueCreate (&newValue)) {
        LGE "(id3ValueCopy) Error creating new value.\nid3ValueCreate: ") ;
        return (NULL) ;
    }

/* Duplicate the old value.  Dynamic copies are made of byte arrays
   and text strings, so the new value is not dependent in any way on
   the old value. */

    newValue->type = oldValue->type ;

    if (oldValue->type == Id3ValueInteger) {

        newValue->vdata.integer = oldValue->vdata.integer ;

    } else if (oldValue->type == Id3ValueOctets) {

        if (oldValue->vdata.octets == NULL) {
            newValue->vdata.octets = NULL ;
            newValue->length = 0 ;
        } else {
            newValue->vdata.octets = memdup (oldValue->vdata.octets,
                                             oldValue->length) ;
            if (newValue->vdata.octets == NULL) {
                LGE "(id3ValueCopy) Error duplicating %"PRIuSIZE_T"-byte octet string.\nmemdup: ",
                    oldValue->length) ;
                PUSH_ERRNO ;  id3ValueDestroy (newValue) ;  POP_ERRNO ;
                return (NULL) ;
            }
            newValue->length = oldValue->length ;
        }

    } else if (oldValue->type == Id3ValueReal) {

        newValue->vdata.real = oldValue->vdata.real ;

    } else if (oldValue->type == Id3ValueText) {

        if (oldValue->vdata.text == NULL) {
            newValue->vdata.text = NULL ;
            newValue->length = 0 ;
        } else {
            newValue->vdata.text = strdup (oldValue->vdata.text) ;
            if (newValue->vdata.text == NULL) {
                LGE "(id3ValueCopy) Error duplicating \"%s\".\nstrdup: ",
                    oldValue->vdata.text) ;
                PUSH_ERRNO ;  id3ValueDestroy (newValue) ;  POP_ERRNO ;
                return (NULL) ;
            }
        }

    }

    return (newValue) ;

}

/*!*****************************************************************************

Procedure:

    id3ValueCreate ()

    Create an Empty ID3 Value.


Purpose:

    The id3ValueCreate() function creates an empty ID3 value (all fields
    cleared).


    Invocation:

        status = id3ValueCreate (&value) ;

    where

        <value>		- O
            returns a handle for the new value.  This handle is used in calls
            to the other ID3_UTIL functions.
        <status>	- O
            returns the status of creating the value, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ValueCreate (

#    if PROTOTYPES
        Id3Value  *value)
#    else
        value)

        Id3Value  *value ;
#    endif

{

    if (value == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3ValueCreate) NULL value return: ") ;
        return (errno) ;
    }

/* Allocate a new value structure. */

    *value = (_Id3Value *) malloc (sizeof (_Id3Value)) ;
    if (value == NULL) {
        LGE "(id3ValueCreate) Error allocating value structure.\nmalloc: ") ;
        return (errno) ;
    }

    (*value)->type = Id3ValueUnknownType ;
    (*value)->vdata.octets = NULL ;
    (*value)->length = 0 ;

    LGI "(id3ValueCreate) Created value %p.\n", (void *) (*value)) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ValueDestroy ()

    Destroy an ID3 Value.


Purpose:

    The id3ValueDestroy() function frees the dynamically allocated contents
    of a frame field's value and then the value structure itself.


    Invocation:

        status = id3ValueDestroy (value) ;

    where

        <value>		- I
            is the handle, returned by id3ValueCopy(), of the value to destroy.
        <status>	- O
            returns the status of destroying the value, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ValueDestroy (

#    if PROTOTYPES
        Id3Value  value)
#    else
        value)

        Id3Value  value ;
#    endif

{

    if (value == NULL)  return (0) ;

    LGI "(id3ValueDestroy) Destroying value %p ...\n", (void *) value) ;

    if ((value->type == Id3ValueOctets) && (value->vdata.octets != NULL)) {
        free (value->vdata.octets) ;
        value->vdata.octets = NULL ;
    } else if ((value->type == Id3ValueText) && (value->vdata.text != NULL)) {
        free (value->vdata.text) ;
        value->vdata.text = NULL ;
    }

    free (value) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ValueDump ()

    Dump an ID3 Value.


Purpose:

    The id3ValueDump() function formats and prints an ID3 value to a file
    for debug purposes.


    Invocation:

        status = id3ValueDump (file, indentation, value) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <value>		- I
            is the handle, returned by id3ValueCreate(), of the value to dump.
        <status>	- O
            returns the status of dumping the value, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ValueDump (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation,
        Id3Value  value)
#    else
        file, indentation, value)

        FILE  *file ;
        char  *indentation ;
        Id3Value  value ;
#    endif

{

    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    if (value == NULL) {
        fprintf (file, "%sDump of Value 0: <nil>\n", indentation) ;
        return (0) ;
    }

    switch (value->type) {
    case Id3ValueInteger:
        fprintf (file, "%sValue %p = %ld\n",
                 indentation, (void *) value, value->vdata.integer) ;
        break ;
    case Id3ValueOctets:
        fprintf (file, "%sValue %p = %p, %lu bytes\n",
                 indentation, (void *) value,
                 (void *) value->vdata.octets, (unsigned long) value->length) ;
        break ;
    case Id3ValueReal:
        fprintf (file, "%sValue %p = %g\n",
                 indentation, (void *) value, value->vdata.real) ;
        break ;
    case Id3ValueText:
        fprintf (file, "%sValue %p = \"%s\"\n",
                 indentation, (void *) value, value->vdata.text) ;
        break ;
    default:
        fprintf (file, "%sValue %p = Unrecognized type %ld\n",
                 indentation, (void *) value, (long) value->type) ;
        break ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ValueErase ()

    Erase the Contents of an ID3 Value.


Purpose:

    The id3ValueErase() function frees the dynamically allocated contents
    of a frame field's value and reinitializes the value to an empty value.


    Invocation:

        status = id3ValueErase (value) ;

    where

        <value>		- I
            is the handle, returned by id3ValueCreate(), of the value to erase.
        <status>	- O
            returns the status of erasing the value, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ValueErase (

#    if PROTOTYPES
        Id3Value  value)
#    else
        value)

        Id3Value  value ;
#    endif

{

    if (value == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3ValueErase) NULL value handle: ") ;
        return (errno) ;
    }

    LGI "(id3ValueErase) Erasing field value %p ...\n", (void *) value) ;

    if ((value->type == Id3ValueOctets) && (value->vdata.octets != NULL))
        free (value->vdata.octets) ;
    else if ((value->type == Id3ValueText) && (value->vdata.text != NULL))
        free (value->vdata.text) ;

    value->type = Id3ValueUnknownType ;
    value->vdata.octets = NULL ;
    value->length = 0 ;

    return (0) ;

}
