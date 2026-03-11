/* $Id: nnl_util.c,v 1.3 2021/09/20 02:14:02 alex Exp $ */
/*******************************************************************************

File:

    nnl_util.c

    Name-to-Number Lookup Utilities


Author:    Alex Measday


Purpose:

    The NNL_UTIL package provides several functions for looking up entries in
    name/number tables, returning (i) the number corresponding to a name or
    (ii) the name corresponding to a number.

    A name/number table is simply an array of number/name entries whose last
    entry has a NULL name (NNLMap, defined in "nnl_util.h", is a structure
    containing a long number and a character string):

        #include  "nnl_util.h"		-- Name-to-Number Lookup utilities.

        static  const  NNLTable  exampleLUT = {
            {  1, "On" },
            { -1, "Off" },
            {  2, "Red" },
            { -2, "Green" },
            {  3, "Show" },
            { -3, "Hide" },
            { 45, NULL }
        } ;

    (The number in the last NULL entry can be any number, e.g., a default
    value or an error indicator.)


Procedures:

    nnlCount() - returns the number of entries in a name/number table.
    nnlNameOf() - looks up a name by number in a name/number table.
    nnlNumberOf() - looks up a number by name in a name/number table.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */

/*!*****************************************************************************

Procedure:

    nnlCount ()

    Count the Number of Name/Number Entries in a Table.


Purpose:

    Function nnlCount() returns the number of name/number entries in a table,
    excluding the final NULL name entry.


    Invocation:

        count = nnlCount (table) ;

    where

        <table>	- I
            is a name/number table.
        <count>	- O
            returns the number of entries in the table, excluding the final
            NULL name entry.  0 is returned if a NULL table is passed in.

*******************************************************************************/


size_t  nnlCount (

#    if PROTOTYPES
        const  NNLTable  table)
#    else
        table)

        NNLTable  table ;
#    endif

{    /* Local variable. */
    size_t  i ;



    if (table == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(nnlCount) NULL table: ") ;
        return (0) ;
    }

    for (i = 0 ;  table[i].name != NULL ;  i++)
        ;

    return (i) ;

}

/*!*****************************************************************************

Procedure:

    nnlNameOf ()

    Lookup a Name by Number.


Purpose:

    Function nnlNameOf() looks up a number in a name/number table and
    returns its corresponding name.


    Invocation:

        name = nnlNameOf (table, number) ;

    where

        <table>		- I
            is the name/number table.
        <number>	- I
            is the number to lookup.
        <name>		- O
            returns the name; NULL is returned for a NULL table or
            an invalid number.

*******************************************************************************/


const  char  *nnlNameOf (

#    if PROTOTYPES
        const  NNLTable  table,
        long  number)
#    else
        table, number)

        NNLTable  table ;
        long  number ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (table == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(nnlNameOf) NULL table: ") ;
        return (NULL) ;
    }

    for (i = 0 ;  table[i].name != NULL ;  i++) {
        if (table[i].number == number)  break ;
    }

    return (table[i].name) ;

}

/*!*****************************************************************************

Procedure:

    nnlNumberOf ()

    Lookup a Number by Name.


Purpose:

    Function nnlNumberOf() looks up a name in a name/number table and
    returns its corresponding name.


    Invocation:

        number = nnlNumberOf (table, name, length, ignoreCase, partialMatch) ;

    where

        <table>		- I
            is the name/number table.
        <name>	- I
            is the name to lookup.
        <length>	- I
            is the length of the name.  If this argument is less than zero,
            the name is assumed to be NUL-terminated and strlen(3) is used
            to determine its length.
        <ignoreCase>	- I
            specifies whether the string comparisons of the target name and
            the entries in the table should ignore alphabetic case (true)
            or not (false).
        <partialMatch>	- I
            specifies whether or not partial matches are allowed.  If this
            argument is true (i.e., partial matches are allowed), a target
            name can be any abbreviation of an entry name in the table.
            For example, target names of "g", "gr", "gre", "gree", and
            "green" will all match "green" in the table.  Care should be
            taken if multiple entries have common prefixes.  For example,
            a target name of "gr" will partially match "green" or "grinch",
            whichever entry appears first in the table.
        <number>	- O
            returns the number corresponding to the name.  0 is returned for
            a NULL table.  If the target name is not found in the table, the
            number from the terminating NULL entry in the table is returned.

*******************************************************************************/


long  nnlNumberOf (

#    if PROTOTYPES
        const  NNLTable  table,
        const  char  *name,
        ssize_t  length,
        bool  ignoreCase,
        bool  partialMatch)
#    else
        table, name, length, ignoreCase, partialMatch)

        NNLTable  table ;
        char  *name ;
        ssize_t  length ;
        bool  ignoreCase ;
        bool  partialMatch) ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (table == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(nnlNumberOf) NULL table: ") ;
        return (0) ;
    } else if (name == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(nnlNumberOf) NULL name: ") ;
        return (0) ;
    }

    if (length < 0)  length = (ssize_t) strlen (name) ;

    for (i = 0 ;  table[i].name != NULL ;  i++) {
				/* Input name too long? */
        if (length > (ssize_t) strlen (table[i].name))  continue ;
				/* Input name too short for full match? */
        if (!partialMatch && (table[i].name[length] != '\0'))  continue ;
				/* All set for full/partial match. */
        if (ignoreCase) {
            if (strncasecmp (table[i].name, name, length) == 0)  break ;
        } else {
            if (strncmp (table[i].name, name, length) == 0)  break ;
        }
    }

    return (table[i].number) ;

}
