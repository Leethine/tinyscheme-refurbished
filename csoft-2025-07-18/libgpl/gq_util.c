/* $Id: gq_util.c,v 1.1 2021/10/31 19:23:53 alex Exp alex $ */
/*******************************************************************************

File:

    gq_util.c

    Generalized Queue (GQ) Utilities


Author:    Alex Measday


Purpose:

    The GQ_UTIL package implements double-ended queues (deques).

        NOTE: I needed a queue in one of my programs and I was
        inspired to write this package by the work of Perl Master
        Kevin Esteb (https://github.com/klesteb/), whose C library
        implements and makes extensive use of deques.  One large
        project I worked on had what I would call a single "unifying"
        data structure/concept, around which higher-level functions
        and multiple programs were elegantly implemented.  Kevin's
        deques seemed to me to play that kind of role since he was
        able to use them in so many different ways in other parts of
        his library.

        (The only unifying concept I can think of in my library is
        debug/error output, built-in in rough form since the library's
        earliest days back in the late 1980s.  However, credit for that
        really belongs to Henry F. Ledgard's PROGRAMMING PROVERBS FOR
        FORTRAN PROGRAMMERS, 1975 edition I believe.)

        Why didn't I just use Kevin's deques?  I'm retired and need the
        intellectual stimulation.  If I were working, I would download
        and use his library, which provides many other, useful capabilities
        in addition to deques.

    In a normal queue, items are added at the rear of the queue and
    removed from the front, thus producing first-in, first-out (FIFO)
    behavior.  In a double-ended queue, an application can add and
    remove items at both ends, front and rear, of the queue.  As a
    result, a deque can also be used as a regular FIFO queue or as a
    last-in, first-out (LIFO) stack.  (And since the GQ_UTIL package
    allows items to be inserted in or retrieved/removed from the middle
    of its queues, a GQ_UTIL queue can also be used as a list!)

    The GQ_UTIL package does not have built-in support for:

      - input-restricted deques (addition at one end, removal at either end)
      - output-restricted deques (addition at either end, removal at one end)

    Applications requiring such deques are responsible for enforcing these
    constraints on the deques; i.e., don't perform operations that violate
    the constraints.

    TERMINOLOGY
    -----------

    A GQ_UTIL "generalized queue" is a sequence of items.

    In a GQ_UTIL queue with N items, the items are numbered 1..N,
    where item 1 is at the front of the queue and item N is at the
    rear.

    As opposed to item numbering, positions in the queue are 0..N,
    where position I falls in between items I and I+1.  When adding
    an item to a queue, positions have the following meanings (and
    note that this package allows items to be inserted in the middle
    of the queue):

        0         - Add the new item to the front of the queue,
                    before item 1.

        1..N-1    - Insert the new item in the middle of the queue.
                    Adding an item at position 1 inserts the new
                    item in between existing items 1 and 2.  Adding
                    an item at position 2 adds the item in between
                    items 2 and 3.  Adding an item at position N-1
                    inserts the item before item N, the item at the
                    rear of the queue, thus making the new item the
                    new penultimate item in the queue.

        N         - Add the new item after the existing item at the
                    rear of the queue.

    Some shortcut positions:

        -1        - Refers to the same position as N; this saves the
                    caller from first having to call gqCount() to
                    locate the end of the queue.

        >N        - Any number greater than N also refers to position N.

    Items nominally are removed from a queue by position, although the
    meanings of position and item number are effectively the same:

        0 or 1    - Remove the front item, item #1, from the queue.

        2..N-1    - Remove the specified item.

        N         - Remove the rear item, item #N, from the queue.

        -1 or >N  - Also remove the rear item from the queue.

    OPERATIONS
    ----------

    There are 3 operations on a queue (starred "functions" are macros that
    call the base function):

    (1) Add an item to the queue.

            gqAdd()		- adds/inserts an item at an arbitrary position.
          * gqAddFront()	- inserts an item at the beginning of the queue.
          * gqAddRear()		- adds an item to the end of the queue.
          * gqPush()		- stack-oriented shorthand for gqAddFront().

    (2) Examine an item in the queue.

            gqGet()		- gets the item at an arbitrary position.
          * gqGetFront()	- gets the item at the beginning of the queue.
          * gqGetRear()		- gets the item at the end of the queue.
          * gqTop()		- stack-oriented shorthand for gqGetFront().

    (3) Remove an item from the queue.

            gqRemove()		- removes the item at an arbitrary position.
          * gqRemoveFront()	- removes the item at the beginning.
          * gqRemoveRear()	- removes the item at the end.
          * gqPop()		- stack-oriented shorthand for gqRemoveFront().

    EXAMPLE
    -------

    The following example shows a GQ_UTIL queue being exercised as a
    double-ended queue, a regular queue, and a stack:

        #include  <stdio.h>			-- Standard I/O definitions.
        #include  <stdlib.h>			-- C Library definitions.
        #include  "gq_util.h"			-- General queue utilities.

        main (int argc, char *argv[])
        {
            GeneralQ  queue ;
            size_t  i ;

            gqCreate (5, &queue) ;		-- Create a 5-slot queue.

            ----

            printf ("Double-ended queue:\n") ;

            gqAddFront (queue, "A") ;		-- Add 6 string items.
            gqAddRear (queue, "B") ;		-- "AB"
            gqAddFront (queue, "C") ;		-- "CAB"
            gqAddRear (queue, "D") ;		-- "CABD"
            gqAddFront (queue, "E") ;		-- "ECABD"
            gqAddRear (queue, "F") ;		-- Too many items; drop "E".

            while (gqCount (queue) > 0) {	-- Print contents of queue.
                printf ("%s", (char *) gqRemoveFront (queue)) ;
            }
            printf ("\n") ;			-- Prints "CABDF".

            ----

            printf ("Regular FIFO queue:\n") ;

            gqAddRear (queue, "A") ;		-- Add 6 string items.
            gqAddRear (queue, "B") ;		-- "AB"
            gqAddRear (queue, "C") ;		-- "ABC"
            gqAddRear (queue, "D") ;		-- "ABCD"
            gqAddRear (queue, "E") ;		-- "ABCDE"
            gqAddRear (queue, "F") ;		-- Too many items; drop "A".

            while (gqCount (queue) > 0) {	-- Print contents of queue.
                printf ("%s", (char *) gqRemoveFront (queue)) ;
            }
            printf ("\n") ;			-- Prints "BCDEF".

            ----

            printf ("LIFO stack-like queue:\n") ;

            gqPush (queue, "A") ;		-- Add 6 string items.
            gqPush (queue, "B") ;		-- "BA"
            gqPush (queue, "C") ;		-- "CBA"
            gqPush (queue, "D") ;		-- "DCBA"
            gqPush (queue, "E") ;		-- "EDCBA"
            gqPush (queue, "F") ;		-- Too many items; drop "A".

            while (gqCount (queue) > 0) {	-- Print contents of queue.
                printf ("%s", (char *) gqPop (queue)) ;
            }
            printf ("\n") ;			-- Prints "FEDCB".

            gqDestroy (queue) ;			-- Delete the queue.

            exit (0) ;

        }


Public Procedures (* defined as macros):

    gqAdd() - adds an item to a queue.
    gqCount() - returns the number of items in a queue.
    gqCreate() - creates a queue.
    gqDestroy() - destroys a queue.
    gqErase() - empties a queue.
    gqGet() - retrieves an item from a queue.
    gqRemove() - removes an item from a queue.

  * gqAddFront() - adds an item at the front of a queue.
  * gqGetFront() - retrieves the item at the front of a queue.
  * gqRemoveFront() - removes the item at the front of a queue.

  * gqAddRear() - adds an item at the rear of a queue.
  * gqGetRear() - retrieves the item at the rear of a queue.
  * gqRemoveRear() - removes the item at the rear of a queue.

  * gqPush() - adds an item at the front of a "stack" queue.
  * gqPop() - removes the item at the front of a "stack" queue.
  * gqTop() - gets the item at the front of a "stack" queue.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  "gq_util.h"			/* Generalized Queue utilities. */


/*******************************************************************************
    Generalized Queue Data Structures.
*******************************************************************************/

typedef  struct  _GqItem {
    void  *value ;			/* User's value. */
    struct  _GqItem  *prev ;		/* Link to previous item. */
    struct  _GqItem  *next ;		/* Link to next item. */
}  _GqItem, *GqItem ;

typedef  struct  _GeneralQ {
    size_t  maxItems ;			/* Allowed # of items in queue. */
    size_t  numItems ;			/* # of items in queue. */
    GqItem  front ;			/* Item at front of queue. */
    GqItem  rear ;			/* Item at rear of queue. */
}  _GeneralQ ;


int  gq_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  gq_util_debug

/*!*****************************************************************************

Procedure:

    gqAdd ()

    Add an Item to a Generalized Queue.


Purpose:

    Function gqAdd() adds a user-supplied item to a general queue at a
    user-specified position.


    Invocation:

        status = gqAdd (queue, position, item) ;

    where

        <queue>		- I
            is the general queue handle returned by gqCreate().
        <position>	- I
            is the position in the queue after which to add the item.
            Items are numbered 1..N in a queue with N items.  If the
            specified position is:
                0       - the new item is inserted at the beginning of the
                          queue, thus becoming the new head of the queue.
                1..N-1  - the new item is inserted after the item at that
                          position.  For example, a position of 1 results
                          in the new item being inserted in between the
                          existing first and second items.
                N       - the new item is added at the end of the queue,
                          thus becoming the new rear of the queue.  (A
                          position greater than N has the same behavior.)
                -1      - is shorthand for N; the new item is added to
                          the very end of the queue.
        <item>		- I
            is the item to be added to the queue, cast as a (void *) pointer.
            NULL is an allowed value.
        <status>	- O
            returns the status of adding the item to the queue, zero if
            no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  gqAdd (

#    if PROTOTYPES
        GeneralQ  queue,
        ssize_t  position,
        void  *item)
#    else
        queue, position, item)

        GeneralQ  queue ;
        ssize_t  position ;
        void  *item ;
#    endif

{    /* Local variables. */
    GqItem  node, prev ;
    ssize_t  i ;



    if (queue == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(gqAdd) NULL queue: ") ;
        return (errno) ;
    }

/* Create an item node for the new item. */

    node = (GqItem) malloc (sizeof (_GqItem)) ;
    if (node == NULL) {
        LGE "(gqAdd) Error allocating node for queue %p, item %p.\nmalloc: ",
            (void *) queue, (void *) item) ;
        return (errno) ;
    }

    node->value = item ;
    node->prev = NULL ;
    node->next = NULL ;

/* If the size of the queue is constrained and the queue is full, remove
   the item at the front or rear of the queue, depending on the position
   at which the new item will be added. */

    if ((queue->maxItems > 0) && (queue->numItems == queue->maxItems)) {
        if ((position < 0) || (queue->numItems <= (size_t) position)) {
		/* Add new item at rear, so remove item at front. */
            gqRemove (queue, GQFRONT) ;
        } else {
		/* Add new item at front or middle, so remove item at rear. */
            gqRemove (queue, GQREAR) ;
        }
    }

/* Add the item to the queue. */

    if (queue->numItems == 0) {		/* Queue was empty. */
        queue->front = node ;
        queue->rear = node ;
    }

    else if (position == 0) {		/* Insert at front of queue. */
        (queue->front)->prev = node ;
        node->next = queue->front ;
        queue->front = node ;
    }
					/* Append to rear of queue. */
    else if ((position < 0) || (queue->numItems <= (size_t) position)) {
        (queue->rear)->next = node ;
        node->prev = queue->rear ;
        queue->rear = node ;
    }

    else {				/* Insert in the middle of queue. */

        prev = queue->front ;
        for (i = 1 ;  i < position ;  i++) {
            prev = prev->next ;
        }

        node->prev = prev ;
        node->next = prev->next ;
        prev->next = node ;		/* Link previous node. */
        (node->next)->prev = node ;	/* Link following node. */

    }

    queue->numItems++ ;

    LGI "(gqAdd) Queue %p, position %"PRIdSSIZE_T", item %p, node %p.\n",
        (void *) queue, position, (void *) item, (void *) node) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    gqCount ()

    Count the Number of Items in a Generalized Queue.


Purpose:

    Function gqCount() returns the number of items in a general queue.


    Invocation:

        count = gqCount (queue) ;

    where

        <queue>		- I
            is the general queue handle returned by gqCreate().
        <count>		- O
            returns the number of items in the queue.

*******************************************************************************/


size_t  gqCount (

#    if PROTOTYPES
        const  GeneralQ  queue)
#    else
        queue)

        GeneralQ  queue ;
#    endif

{

    if (queue == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(gqCount) NULL queue: ") ;
        return (0) ;
    }

    return (queue->numItems) ;

}

/*!*****************************************************************************

Procedure:

    gqCreate ()

    Create a Generalized Queue.


Purpose:

    Function gqCreate() creates and initializes an empty queue.

    The size of a queue can be constrained by specifying a maximum number
    of items in the queue.  If a new item is added to the rear of a full
    queue, the front item in the queue is removed.  If the new item is
    added at any other position in the queue, the rear item in the queue
    is removed.


    Invocation:

        status = gqCreate (maxItems, &queue) ;

    where

        <maxItems>	- I
            limits the number of items a queue can hold; a value of zero
            or less means there is no limit.  If a queue is limited, then
            adding an item to a full queue results in an existing item
            being removed.
        <queue>		- O
            returns a handle for the queue.
        <status>	- O
            returns the status of creating a queue, zero if no errors
            occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  gqCreate (

#    if PROTOTYPES
        ssize_t  maxItems,
        GeneralQ  *queue)
#    else
        maxItems, queue)

        ssize_t  maxItems ;
        GeneralQ  *queue ;
#    endif

{

    if (queue == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(gqCreate) NULL queue handle pointer.\n") ;
        return (errno) ;
    }

/* Create and initialize the queue. */

    *queue = (GeneralQ) malloc (sizeof (_GeneralQ)) ;
    if (*queue == NULL) {
        LGE "(gqCreate) Error allocating general queue.\nmalloc: ") ;
        return (errno) ;
    }

    (*queue)->maxItems = (maxItems < 0) ? 0 : (size_t) maxItems ;
    (*queue)->numItems = 0 ;
    (*queue)->front = NULL ;
    (*queue)->rear = NULL ;

    LGI "(gqCreate) Created general queue %p.\n", (void *) *queue) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    gqDestroy ()

    Destroy a Generalized Queue.


Purpose:

    Function gqDestroy() destroys a previously created general queue.


    Invocation:

        status = gqDestroy (queue) ;

    where

        <queue>		- I
            is the general queue handle returned by gqCreate().
        <status>	- O
            returns the status of destroying the queue, zero if
            there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  gqDestroy (

#    if PROTOTYPES
        GeneralQ  queue)
#    else
        queue)

        GeneralQ  queue ;
#    endif

{

    if (queue == NULL)  return (0) ;

    LGI "(gqDestroy) Destroying general queue %p ...\n", (void *) queue) ;

/* First, delete the item nodes in the queue. */

    while (queue->numItems > 0) {
        gqRemove (queue, GQFRONT) ;
    }

    queue->numItems = 0 ;
    queue->front = NULL ;
    queue->rear = NULL ;

/* Second, delete the queue header. */

    free (queue) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    gqErase ()

    Empty a Generalized Queue.


Purpose:

    Function gqErase() empties out a previously created general queue;
    i.e., it deletes all the items in a queue, but doesn't destroy the
    now-empty queue.


    Invocation:

        status = gqErase (queue) ;

    where

        <queue>		- I
            is the general queue handle returned by gqCreate().
        <status>	- O
            returns the status of deleting the items in the queue,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  gqErase (

#    if PROTOTYPES
        GeneralQ  queue)
#    else
        queue)

        GeneralQ  queue ;
#    endif

{    /* Local variables. */
    GqItem  items, node ;



    if (queue == NULL)  return (0) ;

    LGI "(gqErase) Emptying general queue %p ...\n", (void *) queue) ;

/* Save the item list and clear the queue header. */

    items = queue->front ;
    queue->front = queue->rear = NULL ;
    queue->numItems = 0 ;

/* Delete all the items. */

    while (items != NULL) {
        node = items ;
        items = items->next ;
        free (node) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    gqGet ()

    Retrieve an Item by Position from a Generalized Queue.


Purpose:

    Function gqGet() retrieves the I-th item in a queue.  The position, I,
    is nominally in the range 1..N, where N is the number of items in the
    queue.  Some added shortcut values give the following positions:

        -1      - get the item at the rear of the queue.
        0, 1    - get the item at the front of the queue.
        2..N-1  - get an item from the middle of the queue.
        N or >N - get the item at the rear of the queue.

    NOTE that the item is simply located in the queue and its value is
    returned to the caller; the item is NOT removed from the queue.


    Invocation:

        item = gqGet (queue, position) ;

    where

        <queue>		- I
            is the general queue handle returned by gqCreate().
        <position>	- I
            is the position in the queue of the item to retrieve.
            Items are numbered 1..N in a queue with N items.  If
            the specified position is:
                -1      - get the item at the rear of the queue.
                0, 1    - get the item at the front of the queue.
                2..N-1  - get an item from the middle of the queue.
                N or >N - get the item at the rear of the queue.
        <item>		- O
            returns the requested item, whose value is a (void *) pointer.
            NULL is returned in the event of an error; e.g., the queue
            is empty.  However, gqAdd() allows items to have NULL values.
            If a program has NULL entries in a queue, there is no way to
            distinguish a NULL error value from a NULL item value.

*******************************************************************************/


void  *gqGet (

#    if PROTOTYPES
        const  GeneralQ  queue,
        ssize_t  position)
#    else
        queue, position)

        GeneralQ  queue ;
        ssize_t  position ;
#    endif

{    /* Local variables. */
    GqItem  node ;
    ssize_t  i ;



    if (queue == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(gqGet) NULL queue: ") ;
        return (NULL) ;
    }

    if (queue->numItems == 0)  return (NULL) ;	/* Queue is empty. */

/* Locate the desired item in the queue. */

    if ((position < 0) || (queue->numItems <= (size_t) position))
        return ((queue->rear)-> value) ;	/* Rear of queue. */
    else if (position < 2)
        return ((queue->front)->value) ;	/* Front of queue. */

    node = queue->front ;			/* Middle of queue. */
    for (i = 1 ;  i < position ;  i++)
        node = node->next ;

    return (node->value) ;

}

/*!*****************************************************************************

Procedure:

    gqRemove ()

    Remove an Item by Position from a Generalized Queue.


Purpose:

    Function gqRemove() removes the I-th item in a queue.  The position, I,
    is nominally in the range 1..N, where N is the number of items in the
    queue.  Some added shortcut values give the following positions:

        -1      - remove the item at the rear of the queue.
        0, 1    - remove the item at the front of the queue.
        2..N-1  - remove an item from the middle of the queue.
        N or >N - remove the item at the rear of the queue.


    Invocation:

        item = gqRemove (queue, position) ;

    where

        <queue>		- I
            is the general queue handle returned by gqCreate().
        <position>	- I
            is the position in the queue of the item to remove.
            Items are numbered 1..N in a queue with N items.  If
            the specified position is:
                -1      - remove the item at the rear of the queue.
                0, 1    - remove the item at the front of the queue.
                2..N-1  - remove an item from the middle of the queue.
                N or >N - remove the item at the rear of the queue.
        <item>		- O
            returns the removed item, whose value is a (void *) pointer.
            NULL is returned in the event of an error; e.g., the queue
            is empty.  However, gqAdd() allows items to have NULL values.
            If a program has NULL entries in a queue, there is no way to
            distinguish a NULL error value from a NULL item value.

*******************************************************************************/


void  *gqRemove (

#    if PROTOTYPES
        const  GeneralQ  queue,
        ssize_t  position)
#    else
        queue, position)

        GeneralQ  queue ;
        ssize_t  position ;
#    endif

{    /* Local variables. */
    GqItem  node ;
    ssize_t  i ;
    void  *item ;



    if (queue == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(gqRemove) NULL queue: ") ;
        return (NULL) ;
    }

    if (queue->numItems == 0)  return (NULL) ;	/* Queue is empty. */

/* Remove the item from the queue. */

    if (queue->numItems == 1) {		/* Queue has one item. */
        node = queue->front ;
        queue->front = NULL ;
        queue->rear = NULL ;
    }
					/* Remove from front of queue. */
    else if ((position == 0) || (position == 1)) {
        node = queue->front ;
        queue->front = node->next ;
        (queue->front)->prev = NULL ;
    }
					/* Remove from rear of queue. */
    else if ((position < 0) || (queue->numItems <= (size_t) position)) {
        node = queue->rear ;
        queue->rear = node->prev ;
        (queue->rear)->next = NULL ;
    }

    else {				/* Remove from middle of queue. */

        node = queue->front ;		/* Find item node. */
        for (i = 1 ;  i < position ;  i++) {
            node = node->next ;
        }
					/* Unlink item node. */
        (node->prev)->next = node->next ;
        (node->next)->prev = node->prev ;

    }

    queue->numItems-- ;

/* Deallocate the item node and return the item to the caller. */

    item = node->value ;
    free (node) ;

    LGI "(gqRemove) Queue %p, position %"PRIdSSIZE_T", item %p, node %p.\n",
        (void *) queue, position, (void *) item, (void *) node) ;

    return (item) ;

}
