/* $Id: cobl_util.c,v 1.1 2024/02/20 19:27:20 alex Exp $ */
/*******************************************************************************

File:

    cobl_util.c

    Collectable Object List Utilities.


Author:    Alex Measday

Purpose:

        NOTE: I wrote this and some other utilities back in 1996
        in anticipation of a project I would be assigned to at a
        remote site.  However, I was instead moved back to company
        headquarters for a different project and I put this "on the
        shelf".  Almost 30 years later, in 2023, I was fixing up my
        MicroEmacs variant, whose &-string functions stored their
        results in fixed-size, static buffers.  I realized that
        COBL_UTIL would allow me to store the results in dynamically
        allocated strings.  I couldn't figure out the "before" and
        "after" points in expression processing from the code alone,
        so I settled for simply ensuring the &-string functions
        didn't overflow their result buffers and I left COBL_UTIL
        on the shelf.  Early in 2024, I felt more in the mood for
        diving into the debugger to understand MicroEmacs, so I
        pulled COBL_UTIL out of the mothballs, added the user-data
        parameter, and changed the debug logging to the LGI/LGE
        convention I had adopted in the intervening years.

    The COBL_UTIL package implements a "poor man's" form of garbage
    collection; i.e., it provides a simple means of keeping track of
    allocated objects and of "collecting" or destroying those objects.

    Keeping track of allocated objects is done by maintaining a list
    of "collectable" objects.  Associated with each object in a list
    is an object destruction function that is called when the object
    is collected.  Collectable object lists can be homogeneous (all
    objects in the list are of the same type) or heterogenous (objects
    in the list can be of different types).

    The basic outline for using COBL_UTIL is as follows:

        #include  "cobl_util.h"			-- Collectable object lists.
        ColObjList  listOfObjects ;
        SomeType  *object ;
        					-- Create empty list.
        coblCreate (NULL, NULL, &listOfObjects) ;

        ... Perform processing, allocating new objects as needed and
        ... registering each of them with:
        ...     coblAdd (listOfObjects, (void *) object, NULL) ;

        coblCollect (listOfObjects) ;		-- Delete/destroy all objects.

        coblDestroy (listOfObjects) ;		-- Destroy list.

    The penultimate step is redundant in that coblDestroy() will collect
    the objects in its list anyway, but you may wish to perform multiple
    allocate-collect cycles before finally destroying the list itself.

    When an object is collected, its associated destroy function is called
    to deallocate the object.  Destroy functions can be specified on a
    per-list basis (typically a homogeneous list) :

        extern  errno_t  destroySomeType (void *object, void *userData) ;
        ...
        coblCreate (destroySomeType, NULL, &listOfObjects) ;

    and/or a per-object basis (a heterogeneous list):

        coblAdd (listOfObjects, (void *) object, destroySomeType) ;

    As you might guess, the COBL_UTIL package knows nothing about
    homogeneous and heterogeneous lists.  The destroy function specified
    when a list is created is simply a default that can be superseded on
    a per-object basis in the coblAdd() call.  If no destroy function is
    specified at either level, the C library's free(3) function is called.

    The order in which objects are collected is the reverse of the order
    in which they were registered; in other words, last-registered,
    first-destroyed.

    If necessary, you can remove the registration of a collectable object
    WITHOUT destroying it:

        coblDelete (listOfObjects, (void *) object) ;

    As just mentioned, the object list is in reverse chronological order:
    from the most recently registered object back; i.e., a stack.  Groups
    of objects (similar to stack frames) can be delineated with "markers",
    where a marker is an object with a NULL object pointer whose identity
    is qualified by it user data value.  With the exception of not
    attempting to free(3) them, COBL_UTIL handles markers the same as
    any other object.  However, the coblCollectTo() function will only
    collect objects back to and including a specified marker, a feature
    that might prove useful in pushing and popping groups of objects;
    e.g., in recursive algorithms:

        void  recurvsiveFunction (... arguments ...) {
            if (<done>)  return ;
					-- Add marker for this instance.
            coblAdd (listOfObjects, NULL, NULL, NULL) ;
            ... processing, adding objects as necessary ...
            recursiveFunction (... arguments ...) ;
            ... more processing ...
					-- Pop objects from this instance.
            coblCollectTo (listOfObjects, NULL) ;
        }

    (And which, I hope, is easier than simply creating/destroying separate
    lists for each group.)


Public Procedures:

    coblAdd() - registers an object as collectable.
    coblCollect() - destroys all members of a list of collectable objects.
    coblCollectTo() - destroys members of a list of collectable objects
        back to a marker.
    coblCount() - returns the number of collectable objects in a list.
    coblCreate() - creates an empty list of collectable objects.
    coblDelete() - removes an object from a list of collectable objects.
    coblDestroy() - destroys a list of collectable objects.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  "cobl_util.h"			/* Collectable object lists. */


/*******************************************************************************
    Collectable Object - is a descriptor for a collectable object.
*******************************************************************************/

typedef  struct  _CollectableObject {
    void  *object ;			/* Pointer to the object. */
    ColObjDestroyFunc  destroy ;	/* Object destruction function. */
    void  *userData ;			/* Context for destructor. */
    struct  _CollectableObject  *next ;
}  _CollectableObject, *CollectableObject ;


/*******************************************************************************
    Collectable Object List - is the header for a list of collectable objects.
*******************************************************************************/

typedef  struct  _ColObjList {
    ColObjDestroyFunc  destroy ;	/* Default object destruction function. */
    void  *userData ;			/* Default context for destructor. */
    size_t  numObjects ;
    CollectableObject  first ;
}  _ColObjList ;


int  cobl_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  cobl_util_debug

/*******************************************************************************

Procedure:

    coblAdd ()

    Register an Object for Collection.


Purpose:

    Function coblAdd() registers an object for collection by adding it to
    a list of collectable objects.


    Invocation:

        status = coblAdd (list, object, destroy, userData) ;

    where

        <list>		- I
            is the list handle returned by coblCreate().
        <object>	- I
            is a (VOID *) pointer to the object being registered.
        <destroy>	- I
            is the function to call to destroy the object when it is
            collected.  If this argument is NULL, the default destroy
            function defined for the list (see coblCreate()) is called.
            The destroy function should have the following calling sequence:
                errno_t  destroyFunc (void *object, void *userData) ;
            The returned ERRNO value is ignored by the COBL_UTIL package.
        <userData>	- I
            is a caller-supplied (VOID *) value that will be passed to the
            destroy function when it is invoked.
        <status>	- O
            returns the status of registering the object, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  coblAdd (

#    if PROTOTYPES
        ColObjList  list,
        void  *object,
        ColObjDestroyFunc  destroy,
        void  *userData)
#    else
        list, object, destroy, userData)

        ColObjList  list ;
        void  *object ;
        ColObjDestroyFunc  destroy ;
        void  *userData ;
#    endif

{    /* Local variables. */
    CollectableObject  cob ;




    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(coblAdd) NULL list handle: ") ;
        return (errno) ;
    }

/* Create a descriptor for the collectable object. */

    cob = (CollectableObject) malloc (sizeof (_CollectableObject)) ;
    if (cob == NULL) {
        LGE "(coblAdd) Error creating object descriptor.\nmalloc: ") ;
        return (errno) ;
    }
    cob->object = object ;
    cob->destroy = destroy ;
    cob->userData = userData ;

/* Insert the object descriptor at the front of the list. */

    cob->next = list->first ;
    list->first = cob ;

    list->numObjects-- ;

    LGI "(coblAdd) List: %p  Object: %p\n", (void *) list, (void *) object) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    coblCollect ()

    Destroy All Collectable Objects in a List.


Purpose:

    Function coblCollect() "collects" (i.e., destroys) all of the objects
    in a list of collectable objects.  After collection, the list is empty.

    User-defined, per-object or per-list destroy functions are called to
    destroy objects; the C Library free(3) function is called when neither
    function is defined for an individual object.  Marker objects, if any,
    are also passed to user-defined destroy functions, so these functions
    should recognize NULL object pointers as markers.


    Invocation:

        status = coblCollect (list) ;

    where

        <list>		- I
            is the list handle returned by coblCreate().
        <status>	- O
            returns the status of collecting the objects in the list,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  coblCollect (

#    if PROTOTYPES
        ColObjList  list)
#    else
        list)

        ColObjList  list ;
#    endif

{    /* Local variables. */
    CollectableObject  cob ;
    void  *userData ;




    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(coblCollect) NULL list handle: ") ;
        return (errno) ;
    }

/* Destroy each object in the list. */

    while (list->first != NULL) {
        cob = list->first ;
        LGI "(coblCollect) List: %p  Object: %p\n",
            (void *) list, (void *) cob->object) ;
        userData = cob->userData ;
        if (userData == NULL)  userData = list->userData ;
        if (cob->destroy != NULL)
            cob->destroy (cob->object, userData) ;
        else if (list->destroy != NULL)
            list->destroy (cob->object, userData) ;
        else if (cob->object != NULL)
            free (cob->object) ;
        coblDelete (list, cob->object) ;
    }

    return (0) ;

}

/*******************************************************************************

Procedure:

    coblCollectTo ()

    Destroy Collectable Objects Back to a Marker in a List.


Purpose:

    Function coblCollectTo() "collects" (i.e., destroys) the most recent
    objects in a list back to a specified marker object, including the
    marker object.  If there is a user-defined destroy function for the
    marker object or list, that function is called for the marker object
    too and the destroy function should recognize the NULL object pointer
    as a marker.


    Invocation:

        status = coblCollectTo (list, marker) ;

    where

        <list>		- I
            is the list handle returned by coblCreate().
        <marker>	- I
            is the (VOID *) user data specified when the marker was added
            to the list.
        <status>	- O
            returns the status of collecting the objects in the list back
            to the marker, zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  coblCollectTo (

#    if PROTOTYPES
        ColObjList  list,
        void  *marker)
#    else
        list, marker)

        ColObjList  list ;
        void  *marker ;
#    endif

{    /* Local variables. */
    CollectableObject  cob ;
    void  *object, *userData ;




    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(coblCollectTo) NULL list handle: ") ;
        return (errno) ;
    }

/* Destroy the most recent objects in the list one-by-one until the
   marker object is reached. */

    while (list->first != NULL) {
        cob = list->first ;
        object = cob->object ;
        if (object == NULL) {
            LGI "(coblCollectTo) List: %p  Marker: %p\n",
                (void *) list, (void *) cob->userData) ;
        } else {
            LGI "(coblCollectTo) List: %p  Object: %p\n",
                (void *) list, (void *) cob->object) ;
        }
        userData = cob->userData ;
        if (userData == NULL) {		/* Substitute list-level user data? */
            if (object != NULL)		/* But allow marker with NULL data. */
                userData = list->userData ;
        }
        if (cob->destroy != NULL)
            cob->destroy (cob->object, userData) ;
        else if (list->destroy != NULL)
            list->destroy (cob->object, userData) ;
        else if (cob->object != NULL)
            free (cob->object) ;
        userData = cob->userData ;	/* Restore original value. */
        coblDelete (list, cob->object) ;
        if ((object == NULL) && (userData == marker))  break ;
    }

    return (0) ;

}

/*******************************************************************************

Procedure:

    coblCount ()

    Count the Number of Collectable Objects in a List.


Purpose:

    Function coblCount() returns the number of objects (including markers)
    in a collectable object list.


    Invocation:

        numObjects = coblCount (list) ;

    where

        <list>		- I
            is the list handle returned by coblCreate().
        <numObjects>	- O
            returns the number of objects (including markers) in the list.

*******************************************************************************/


size_t  coblCount (

#    if PROTOTYPES
        ColObjList  list)
#    else
        list)

        ColObjList  list ;
#    endif

{

    return ((list == NULL) ? 0 : list->numObjects) ;

}

/*******************************************************************************

Procedure:

    coblCreate ()

    Create an Empty List of Collectable Objects.


Purpose:

    Function coblCreate() creates an empty list of collectable objects.


    Invocation:

        status = coblCreate (destroy, userData, &list) ;

    where

        <destroy>	- I
            is the default function to call to destroy an object being
            collected from the list; object-specific function specified
            in coblAdd() calls override this default for those objects
            only.  The destroy function should have the following calling
            sequence:
                errno_t  destroyFunc (void *object, void *userData) ;
            The returned ERRNO value is ignored by the COBL_UTIL package.
        <userData>	- I
            is a default caller-supplied (VOID *) value that will be passed
            to the destroy function when it is invoked.  This value can be
            overridden on a per-object basis by specifying a superseding
            value in an object's coblAdd() registration.
        <list>		- O
            returns a handle for the new list.  This handle is used in
            calls to the other COBL_UTIL functions.
        <status>	- O
            returns the status of creating the list, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  coblCreate (

#    if PROTOTYPES
        ColObjDestroyFunc  destroy,
        void  *userData,
        ColObjList  *list)
#    else
        destroy, list)

        ColObjDestroyFunc  destroy ;
        void  *userData ;
        ColObjList  *list ;
#    endif

{

/* Allocate a list header structure. */

    *list = (ColObjList) malloc (sizeof (_ColObjList)) ;
    if (*list == NULL) {
        LGE "(coblCreate) Error creating collectable object list.\nmalloc: ") ;
        return (errno) ;
    }

/* Initialize the list to empty. */

    (*list)->destroy = destroy ;
    (*list)->userData = userData ;
    (*list)->numObjects = 0 ;
    (*list)->first = NULL ;

    LGI "(coblCreate) List: %p\n", (void *) (*list)) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    coblDelete ()

    Remove the Registration of a Collectable Object.


Purpose:

    Function coblDelete() removes the registration of a collectable object
    by deleting the object from its list.  The object is NOT destroyed;
    the caller is responsible for doing so.


    Invocation:

        status = coblDelete (list, object) ;

    where

        <list>		- I
            is the list handle returned by coblCreate().
        <object>	- I
            is a (VOID *) pointer to the object being unregistered.
        <status>	- O
            returns the status of unregistering the object, zero if
            there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  coblDelete (

#    if PROTOTYPES
        ColObjList  list,
        void  *object)
#    else
        list, object)

        ColObjList  list ;
        void  *object ;
#    endif

{    /* Local variables. */
    CollectableObject  cob, prev ;




    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(coblDelete) NULL list handle: ") ;
        return (errno) ;
    }

/* Locate the object in the list. */

    prev = NULL ;
    for (cob = list->first ;  cob != NULL ;  prev = cob, cob = cob->next) {
        if (cob->object == object)  break ;
    }

    if (cob == NULL) {
        SET_ERRNO (ENOENT) ;
        LGE "(coblDelete) Object %p not found in list %p.\n",
            (void *) object, (void *) list) ;
        return (errno) ;
    }

/* Unlink the object from the list. */

    if (prev == NULL)
        list->first = cob->next ;
    else
        prev->next = cob->next ;

    list->numObjects-- ;

    free (cob) ;

    LGI "(coblDelete) List: %p  Object: %p\n", (void *) list, (void *) object) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    coblDestroy ()

    Destroy a List of Collectable Objects.


Purpose:

    Function coblDestroy() collects the remaining objects in a list of
    collectable objects and then destroys the list.


    Invocation:

        status = coblDestroy (list) ;

    where

        <list>		- I
            is the list handle returned by coblCreate().
        <status>	- O
            returns the status of destroying the list, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  coblDestroy (

#    if PROTOTYPES
        ColObjList  list)
#    else
        list)

        ColObjList  list ;
#    endif

{

    LGI "(coblDestroy) List: %p\n", (void *) list) ;

    if (list == NULL)  return (0) ;

/* Collect the remaining objects in the list. */

    coblCollect (list) ;

/* Deallocate the list header. */

    free ((char *) list) ;

    return (0) ;

}

#ifdef  TEST

/*******************************************************************************

    Program to test the COBL_UTIL() functions.

    Under UNIX,
        compile and link as follows:
            % cc -g -DTEST cobl_util.c -I<... includes ...> <libraries ...>
        and run with the following command line:
            % a.out <wildcard_file_spec>

    Under VxWorks,
        compile and link as follows:
            % cc -g -c -DTEST -DVXWORKS cobl_util.c -I<... includes ...> \
                       -o test_cobl.o
            % ld -r test_cobl.o <libraries ...> -o test_cobl.vx.o
        load as follows:
            -> ld <test_cobl.vx.o
        and run with the following command line:
            -> test_cobl.vx.o "<wildcard_file_spec>"

*******************************************************************************/


int  objDestroy (object)
    void  *object ;
{
    printf ("(objDestroy) %p\n", (void *) object) ;
    return (0) ;
}


#ifdef VXWORKS

test_cobl ()

#else

main (argc, argv)
    int  argc ;
    char  *argv[] ;

#endif

{    /* Local variables. */
    ColObjList  list ;
    int  i ;



    cobl_util_debug = 1 ;

    if (coblCreate (NULL, NULL, &list))  exit (errno) ;

    for (i = 0 ;  i < 32 ;  i++)
        if (coblAdd (list, (void *) i, objDestroy, NULL))  break ;

    coblCollect (list) ;

    coblDestroy (list) ;

}
#endif
