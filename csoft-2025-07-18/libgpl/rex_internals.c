/* $Id: rex_internals.c,v 1.8 2023/05/24 12:27:41 alex Exp alex $ */
/*******************************************************************************

File:

    rex_internals.c

    Regular Expression Parser Internal Utilities.


Author:    Alex Measday


Purpose:

    The "rex_internals.h" header file defines the data structures used to
    build a compiled regular expression.  This source file, "rex_internals.c",
    contains "private" functions used to manipulate these structures during
    the parsing of a regular expression specification string.

    These "private" functions were originally defined as static functions
    in the YACC grammar file, "rex_util_y.y", so the functions truly were
    private to the parser.  I've moved the functions to this file to simplify
    the grammar file ... somewhat.  As a result, the "private" functions have
    to be public so that the YACC parser can reference them.  The parser is
    the only software that should be calling these functions.


"Private" Procedures

    rexAddGroupName() - adds a capture group name to the compiled RE.
    rexAllocateState() - expands the state list to store more states.
    rexFirstChar() - determines the set of characters that may appear
        at the beginning of a string matched by a regular expression
        or subexpression.
    rexLastState() - locates the last state in a regular expression or
        subexpression.
    rexLongestPath() - determines the number of states in the longest path
        through a regular expression or subexpression.
    rexNameOf() - returns the human-readable name of a state type.
    rexShortestPath() - determines the number of states in the shortest path
        through a regular expression or subexpression.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Standard character functions. */
#ifndef isascii
#    define  isascii(c)  ((unsigned char) (c) <= 0177)
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* Standard C string functions. */
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "rex_util.h"			/* Regular expression definitions. */
#include  "rex_internals.h"		/* Internal definitions. */

#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  rex_util_debug

/*!*****************************************************************************

Procedure:

    rexAddGroupName ()

    Add a Capture Group Name/Index Mapping to a Compiled RE.


Purpose:

    Function rexAddGroupName() adds a capture group name and index mapping
    to the lookup table in the compiled regular expression structure.  The
    parser adds group names/indices to the lookup table as it encounters
    named capture groups in the regular expression.


    Invocation:

        status = rexAddGroupName (pattern, name, length, group) ;

    where

        <pattern>	- I
            is the being-compiled regular expression object.
        <name>		- I
            is the group name being added to the lookup table.
            A dynamically allocated copy of the input name is made;
            rexDestroy will free all of the names in the LUT when
            it destroys the compiled RE.
        <length>	- I
            is the length of the group name; if -1 is specified, the
            name is assumed to be NUL-terminated and its length will
            be determined by this function.
        <group>		- I
            is the index of the numbered group to which the name is bound.
            All capture groups are assigned an incrementing number as the
            parser encounters the beginning (left parenthesis) of each
            capture group.
        <status>	- O
            returns zero if the group name/index mapping is successfully
            added to the lookup table.  ERRNO is returned if (re)allocation
            of the lookup table fails or duplication of the input name fails.

*******************************************************************************/

#define  GROUP_NAME_LUT_INCREMENT  16

errno_t  rexAddGroupName (

#    if PROTOTYPES
        CompiledRE  pattern,
        char  *name,
        ssize_t  length,
        size_t  group)
#    else
        pattern, name, length, group)

        CompiledRE  pattern ;
        char  *name,
        ssize_t  length,
        size_t  group)
#    endif

{    /* Local variables. */
    NNLMap  *lut ;
    size_t  i, size ;



    if ((pattern->maxGroupNames == 0) ||	/* Leave room for EOT marker. */
        (pattern->numGroupNames >= (pattern->maxGroupNames - 1))) {
        size = (pattern->maxGroupNames + GROUP_NAME_LUT_INCREMENT)
               * sizeof (NNLMap) ;
        if (pattern->groupNameLUT == NULL)
            lut = malloc (size) ;
        else
            lut = realloc (pattern->groupNameLUT, size) ;
        if (lut == NULL) {
            LGE "(rexAddGroupName) Error (re)allocating the group name LUT.\nrealloc: ") ;
            return (errno) ;
        }
        pattern->groupNameLUT = lut ;
        pattern->maxGroupNames += GROUP_NAME_LUT_INCREMENT ;
    }

    i = pattern->numGroupNames ;
    pattern->groupNameLUT[i].number = (long) group ;
    if (length < 0)  length = strlen (name) ;
    pattern->groupNameLUT[i].name = strndup (name, (size_t) length) ;
    if (pattern->groupNameLUT[i].name == NULL) {
        LGE "(rexAddGroupName) Error duplicating group name \"%*s\".\nstrndup: ",
            (int) length, name) ;
        return (errno) ;
    }

    i++ ;
    pattern->groupNameLUT[i].number = -1 ;
    pattern->groupNameLUT[i].name = NULL ;

    pattern->numGroupNames++ ;

    LGI "(rexAddGroupName) Added name/index, \"%*s\"/%"PRIuSIZE_T", to group name LUT.\n",
        (int) length, name, group) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexAllocateState ()

    Allocate an Entry for a New State.


Purpose:

    Function rexAllocateState() simply checks if there is enough room
    to add a new state to the state list in a being-compiled RE object.
    If there is enough room, nothing is done; the calling routine is
    responsible for incrementing the RE object's "numStates" counter.
    If there is not enough room, the size of the state list is increased.
    Zero is returned if the calling routine can now add a state; ERRNO
    is returned if rexAllocateState() could not resize the state list.


    Invocation:

        status = rexAllocateState (pattern) ;

    where

        <pattern>	- I
            is the being-compiled regular expression object.
        <status>	- O
            returns zero if there is now enough room in the state list
            to add a new state.  ERRNO is returned if (re)allocation
            of the state list to hold another state failed.

*******************************************************************************/

#define  SLIST_INCREMENT  32

errno_t  rexAllocateState (

#    if PROTOTYPES
        CompiledRE  pattern)
#    else
        pattern)

        CompiledRE  pattern ;
#    endif

{    /* Local variables. */
    StateNode  *s ;
    size_t  size ;



    if (pattern->numStates >= pattern->maxStates) {
        size = (pattern->maxStates + SLIST_INCREMENT) * sizeof (StateNode) ;
        if (pattern->stateList == NULL)
            s = malloc (size) ;
        else
            s = realloc (pattern->stateList, size) ;
        if (s == NULL) {
            LGE "(rexAllocateState) Error (re)allocating the state list.\nrealloc: ") ;
            return (errno) ;
        }
        pattern->stateList = s ;
        pattern->maxStates += SLIST_INCREMENT ;
    }

/* Initialize the newly allocated state in case the parser forgets to set
   the "next" links.  Originally, the "next" links in the "Final" state
   were not set because they're not needed.  VALGRIND reported reads from
   uninitialized, malloc(3)'ed memory in rexDump() when dumping the final
   state; rexDump() reads the "next" links of every state node before
   performing the state-type-specific processing, which, in the "Final"
   state, doesn't involve the links at all.  The parser now explicitly
   sets the links to -1 when creating the "Final" state after the
   completion of the regular expression, but I'll initialize them here
   for good measure.  (The other state types already set both links
   appropriately.)  Note that the newly allocated state is at index
   NUMSTATES since the existing states are indexed by 0..NUMSTATES-1;
   the caller -- the parser -- is responsible for filling in the new
   state and incrementing NUMSTATES. */

    pattern->stateList[pattern->numStates].type = Empty ;
    pattern->stateList[pattern->numStates].next1 = -1 ;
    pattern->stateList[pattern->numStates].next2 = -1 ;

    LGI "(rexAllocateState) State list: %p  Max states: %"PRIuSIZE_T"  Num states: %"PRIuSIZE_T"\n",
        (void *) pattern->stateList, pattern->maxStates, pattern->numStates) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexFirstChar ()

    Assemble the Set of First Characters Matched by a Regular Expression.


Purpose:

    Function rexFirstChar() computes the set of characters that may appear at
    the beginning of a string that could be matched by a regular expression.
    This is an optimization for the RE matching algorithm: assuming an
    unanchored (no '^') RE, an unoptimized rexMatch() would attempt to match
    the RE at each character position in the string until a match is made or
    the end of the string is reached.  The optimized rexMatch() can quickly
    look up the character at the current position in the string.  If the
    character is in the RE's first-character set, rexMatch() will attempt
    to match the RE beginning at that position.  If the current character
    is not in the RE's first-character set, rexMatch() can skip applying
    the RE to the current position in the string and, instead, advance to
    the next position.

    The first-character set is represented by a 256-bit bitmap where the
    bits map to the 256 distinct values of an 8-bit character.  If bit N
    is set (1), the character with value N is one of the first characters.
    If bit N is clear (0), that character is not in the set of first
    characters.  An empty set is a bitmap with all bits zero; an
    any-character set is a bitmap with all bits one.  Since there is only
    one first-character set for a whole regular expression, the rexFirstChar()
    calls operate directly on the bitmap stored in the compiled RE.

    Function rexFirstChar() is called on the start state after the full
    regular expression is completely parsed and compiled.  In this top-level
    call, the set of first characters is initialized to the empty set and
    the "visited" flags of all the states are cleared.  The graph is then
    recursively traversed to build the union of possible first characters
    matched by the regular expression.  In pseudocode:

			-- Top-level call to rexFirstChar().
        RE's first-set = rexFirstChar (start-state) ;

    where rexFirstChar() looks somewhat as follows:

        function  rexFirstChar (state) returns character-set {
            if (state.visited)  return (empty-set) ;
            state.visited = true ;
            if (state.type == MATCH) {
			-- Add characters matched by this state to the set.
                return (state.match-set) ;
            } else if (state.type == FINAL) {
			-- Final state only reached if RE can match 0-length
			-- strings, ""; e.g., because of Kleene closures.
			-- Since RE matches "" without consuming current
			-- character, current character can be any character.
			-- So, add all characters to first-character set.
                return (set-of-all-characters) ;
            } else {
                first-set = empty-state ;
                for each of the state's outward branches {
                    first-set = first-set UNION rexFirstChar (branch-state) ;
                }
                return (first-set) ;
            }
        }

    Note that rexFirstChar() does not recurse beyond the first MATCH or FINAL
    state encountered along a thread through the RE graph.  Both state types
    define sets of first characters, albeit in different ways.

    Don't confuse an RE's first-character set with individual MATCH states'
    match-character sets.  A MATCH state matches/consumes a single character
    in the string.  The state's match-character set specifies the characters
    and character classes matched by the MATCH state.  If a particular MATCH
    state is the first MATCH state encountered along a thread through the RE
    graph, the characters in the match-character set ARE added to the RE's
    first-character set.

    For example, rexFirstChar() will reach 3 MATCH states because of
    alternation and a star closure in RE "(A|B)*[a-z[:digit:]]":

        (1) 'A' (match-character set)
        (2) 'B' (match-character set)
        (3) any of "abcdefghijklmnopqrstuvwxyz0123456789" (match-character set)

    The first-character set for the entire RE is the union of the match sets:
    "ABabcdefghijklmnopqrstuvwxyz0123456789"

    Some other examples:  The set of first characters for RE "abc" is simply
    'a'; the set for RE "[A-Za-z][A-Za-z0-9]*" contains 'A' through 'Z' and
    'a' through 'z'.  The set of first characters for RE "a*" is the set of
    all characters, since the RE could still match a zero-length string if
    the target string begins with any character other than 'a'.  rexFirstChar()
    returns a function value of true if the computed set contains all
    characters and false if the computed set does not contain all characters.


    Invocation:

        all = rexFirstChar (pattern, state, isStart) ;

    where

        <pattern>	- I
            is the compiled regular expression.
        <state>		- I
            is the state at which to begin determining the set of first
            characters.  The top-level call to rexFirstChar() should
            specify the start state of the entire regular expression.
            Recursive calls to rexFirstChar() will be made until all
            of the leading (first-character) MATCH states have been
            visited and their characters added to the first-character
            set.  (Or until the FINAL state is reached because of empty
            closures.)
        <isStart>	- I
            should be true in the top-level call to rexFirstChar() and false
            in recursive calls.  In the top-level call, when this argument
            is true, rexFirstChar() will initialize the first-character set
            to the empty set and clear all of the "visited" flags for the
            states in the RE's graph.  As the states are visited, they are
            flagged as "visited" so that recursive calls, with this argument
            false, do not get into infinite loops.
        <all>		- O
            returns true if the first-character set now includes all
            characters and false otherwise.  True is only returned when
            a MATCH state is a dot-match ('.' matches any character) or
            if the RE's FINAL state is reached (a zero-length string
            can precede any character).

*******************************************************************************/


bool  rexFirstChar (

#    if PROTOTYPES
        CompiledRE  pattern,
        StateIndex  state,
        bool  isStart)
#    else
        pattern, state, isStart)

        CompiledRE  pattern ;
        StateIndex  state ;
        bool  isStart ;
#    endif

{    /* Local variables. */
    size_t  i ;



/* When starting to compute the set of first characters for a given regular
   expression, (i) zero out all the character bits in the first-character
   set and (ii) clear the visited flags in all the states. */

    LGI "(rexFirstChar) State = %"PRI_STATEINDEX"\n", state) ;
    if (isStart) {
        CS_ZERO (&pattern->firstSet) ;
        for (i = 0 ;  i < pattern->numStates ;  i++) {
            pattern->stateList[i].visited = false ;
        }
    }
    if ((state < 0) || (pattern->stateList[state].visited))  return (false) ;

/* Mark the state as visited during the traversal.  The VISITED flags keep
   the program from looping endlessly on cycles in the RE's graph (e.g.,
   closure states). */

    pattern->stateList[state].visited = true ;

/* Compute the set of first characters, starting with this state and proceeding
   to the end of the RE's graph. */

    switch (pattern->stateList[state].type) {
    case Anchor:
    case Empty:
    case LeftParen:
    case RightParen:
        return (rexFirstChar (pattern, pattern->stateList[state].next1,
                false)) ;
    case Alternation:
    case Closure:
        return (rexFirstChar (pattern, pattern->stateList[state].next1, false)
                ||
                rexFirstChar (pattern, pattern->stateList[state].next2, false)
               ) ;
    case Final:
        CS_FILL (&pattern->firstSet) ;
        return (true) ;
    case Match:
        switch (pattern->stateList[state].u.m.match_char) {
        case REX_MATCH_CHAR_ANY:
            CS_FILL (&pattern->firstSet) ;
            return (true) ;
        case REX_MATCH_CHAR_ANY_BUT_NL:
            CS_FILL (&pattern->firstSet) ;
            CS_CLR ('\r', &pattern->firstSet) ;
            CS_CLR ('\n', &pattern->firstSet) ;
            return (true) ;
        case REX_MATCH_CHAR_CHARSET:		/* Match multiple characters. */
            CS_COPY (pattern->stateList[state].u.m.match_charset,
                     &pattern->firstSet) ;
            return (false) ;
        default:				/* Match a single character. */
            CS_SET (pattern->stateList[state].u.m.match_char,
                    &pattern->firstSet) ;
            return (false) ;
        }
    default:
        return (false) ;
    }

}

/*!*****************************************************************************

Procedure:

    rexLastState ()

    Locate the Last State in a Regular (Sub)Expression.


Purpose:

    Function rexLastState() locates the last state in the graph of a regular
    expression or subexpression.  Passed the index of the first state in the
    graph, rexLastState() simply traverses the NEXT1 links until a null link
    (index = -1) is found.

        NOTE that this technique assumes that the NEXT1 links lead to the
        last state in the graph; yyparse() adheres to this convention and
        uses NEXT2 links only for circling back in closure (RE* and RE+)
        expressions.

    This function is used both to add a final state to a complete regular
    expression and to add intermediate "final" states to subexpressions.
    For example, in the alternation part of RE "(a|b)*", the last state
    of the 'a' branch and the last state of the 'b' branch are both
    linked to a third, empty state which terminates the alternation.
    (Every subexpression has a single first state and a single last state.
    In the "a|b" subexpression, the first state is the alternation state
    with two links to the match states for 'a' and 'b', respectively.
    The two match states are linked to an empty "final" state for the
    subexpression.)


    Invocation:

        last = rexLastState (pattern, first) ;

    where

        <pattern>	- I
            is the (partially) compiled regular expression.
        <first>		- I
            is the beginning state of the regular expression's or
            subexpression's graph.
        <last>		- O
            returns the end state of the graph.  -1 is returned if
            an invalid first state is specified or if the graph is
            incorrectly structured.

*******************************************************************************/


StateIndex  rexLastState (

#    if PROTOTYPES
        CompiledRE  pattern,
        StateIndex  state)	/* The first state in the RE graph. */
#    else
        pattern, state)

        CompiledRE  pattern ;
        StateIndex  state ;	/* The first state in the RE graph. */
#    endif

{    /* Local Variables. */
    size_t  i ;



    if ((state < 0) || ((StateIndex) pattern->numStates <= state))
        return (-1) ;

/* Just in case, use the "visited" flags to avoid infinite loops. */

    for (i = 0 ;  i < pattern->numStates ;  i++) {
        pattern->stateList[i].visited = false ;
    }

/* Follow the chain of states until the last state is found. */

    while (pattern->stateList[state].next1 >= 0) {
        state = pattern->stateList[state].next1 ;
        if (pattern->stateList[state].visited)  return (-1) ;
        pattern->stateList[state].visited = true ;
    }

    return (state) ;

}

/*!*****************************************************************************

Procedure:

    rexLongestPath ()

    Compute the Longest Path Through a Regular Expression.


Purpose:

    Function rexLongestPath() computes the longest path through a regular
    expression.  The longest path is an estimate of the maximum number of
    states that would be visited during an attempt to match a target string.

        NOTE:  rexLongestPath() was originally used to estimate the size
        of the heap-allocated call stack for the non-recursive version of
        rexSearch().  A poor estimate as it turns out and a dynamically
        growing call stack is now used instead.

    The top-level call to rexLongestPath() should specify the start state
    of the full regular expression and the is-start argument should be true.
    Recursive calls by rexLongestPath() itself will specify beginning states
    of subexpressions and the is-start argument will be false.  For example,
    given RE "(a|b)*", rexLongestPath() will (in simplified form):

      - start with the '(' state and clear all of the states' "visited" flags,
      - proceed to the '|' state,
      - recursively compute the lengths through the 'a' and 'b' paths
        of the alternation and choose the path with the longest length,
      - etc., etc.


    Invocation:

        length = rexLongestPath (pattern, state, isStart) ;

    where

        <pattern>	- I
            is the (partially) compiled regular expression.
        <state>		- I
            is the state at which to begin computing the longest path.
            A top-level call to rexLongestPath() will specify the start
            state of the entire regular expression.  The top-level call
            will recursively call rexLongestPath() with current location
            states to compute the lengths of subexpressions within the
            regular expression.
        <isStart>	- I
            should be true in the top-level call to rexLongestPath() and
            false in recursive calls.  In the top-level call, when this
            argument is true, rexLongestPath() will clear all of the
            "visited" flags for the states in RE's graph.  As the states
            are visited, they are flagged as "visited" so that recursive
            calls, with this argument false, do not get into infinite loops.
        <length>	- O
            returns the length of the longest path through the regular
            expression.

*******************************************************************************/


size_t  rexLongestPath (

#    if PROTOTYPES
        CompiledRE  pattern,
        StateIndex  state,
        bool  isStart)
#    else
        pattern, state, isStart)

        CompiledRE  pattern ;
        StateIndex  state ;
        bool  isStart ;
#    endif

{    /* Local variables. */
    size_t  i, j ;



/* When starting to compute the longest path through a given regular
   expression, clear the visited flags in all the states. */

    LGI "(rexLongestPath) State = %"PRI_STATEINDEX"\n", state) ;
    if (isStart) {		/* To start, mark each state as not visited. */
        for (i = 0 ;  i < pattern->numStates ;  i++) {
            pattern->stateList[i].visited = false ;
        }
    }
    if ((state < 0) || (pattern->stateList[state].visited))  return (0) ;

/* Mark the state as visited during the traversal.  The VISITED flags keep
   the program from looping endlessly on cycles in the RE's graph (e.g.,
   closure states). */

    pattern->stateList[state].visited = true ;

/* Compute the longest path from this state to the end of the RE's graph. */

    switch (pattern->stateList[state].type) {
    case Empty:
        return (rexLongestPath (pattern, pattern->stateList[state].next1, false)) ;
    case Anchor:
        return (rexLongestPath (pattern, pattern->stateList[state].next1, false)) ;
    case Alternation:
        i = 1 + rexLongestPath (pattern, pattern->stateList[state].next1, false) ;
        j = 1 + rexLongestPath (pattern, pattern->stateList[state].next2, false) ;
        return ((i > j) ? i : j) ;
    case Closure:
        return (rexLongestPath (pattern, pattern->stateList[state].next2, false)
                + 2 +
                rexLongestPath (pattern, pattern->stateList[state].next1, false)) ;
    case Final:
        return (0) ;
    case Match:
        return (1 + rexLongestPath (pattern, pattern->stateList[state].next1, false)) ;
    case LeftParen:
        return (1 + rexLongestPath (pattern, pattern->stateList[state].next1, false)) ;
    case RightParen:
        return (1 + rexLongestPath (pattern, pattern->stateList[state].next1, false)) ;
    default:
        return (0) ;
    }

}

/*!*****************************************************************************

Procedure:

    rexNameOf ()

    Get the Human-Readable Name of a State Type.


Purpose:

    Function rexNameOf() returns the human-readable name of the type of a
    state node in a compiled RE.


    Invocation:

        name = rexNameOf (type, upper) ;

    where

        <type>		- I
            is the state type being looked up.
        <upper>		- I
            specifies if the type name should be returned in all upper-case
            (true) or capitalized (false).  The latter is how the enumerations
            are actually spelled.  The upper-case names are abbreviated to
            8 characters or less; for example, "LeftParen" becomes "LPAREN".
        <name>		- O
            returns zero if there is now enough room in the state list
            to add a new state.  ERRNO is returned if (re)allocation
            of the state list to hold another state failed.

*******************************************************************************/


const  char  *rexNameOf (

#    if PROTOTYPES
        StateType  type,
        bool  upper)
#    else
        type, upper)

        StateType  type ;
        bool  upper ;
#    endif

{    /* Local variables. */
    const  char  *name ;

    static  const  NNLTable  stateNameLUT  OCD ("rex_util") = {
        { (long) Empty, "Empty" },
        { (long) Alternation, "Alternation" },
        { (long) Anchor, "Anchor" },
        { (long) Closure, "Closure" },
        { (long) Final, "Final" },
        { (long) LeftParen, "LeftParen" },
        { (long) Match, "Match" },
        { (long) RightParen, "RightParen" },
        { -1, NULL }
    } ;

    static  const  NNLTable  stateNameUpperLUT  OCD ("rex_util") = {
        { (long) Empty, "EMPTY" },
        { (long) Alternation, "ALTER" },
        { (long) Anchor, "ANCHOR" },
        { (long) Closure, "CLOSURE" },
        { (long) Final, "FINAL" },
        { (long) LeftParen, "LPAREN" },
        { (long) Match, "MATCH" },
        { (long) RightParen, "RPAREN" },
        { -1, NULL }
    } ;



    name = nnlNameOf ((upper ? stateNameUpperLUT : stateNameLUT), (long) type) ;

    return ((name == NULL) ? "<none>" : name) ;

}

/*!*****************************************************************************

Procedure:

    rexShortestPath ()

    Compute the Shortest Path Through a Regular Expression.


Purpose:

    Function rexShortestPath() computes the shortest path through a regular
    expression.  The shortest path equals the minimum length of a target
    string that would be matched by the regular expression.

    The top-level call to rexShortestPath() should specify the start state
    of the full regular expression and the is-start argument should be true.
    Recursive calls by rexShortestPath() itself will specify beginning states
    of subexpressions and the is-start argument will be false.  For example,
    given RE "(a|b)*", rexShortestPath() will (in simplified form):

      - start with the '(' state and clear all of the states' "visited" flags,
      - proceed to the '|' state,
      - recursively compute the lengths through the 'a' and 'b' paths
        of the alternation and choose the path with the shortest length,
      - etc., etc.


    Invocation:

        length = rexShortestPath (pattern, state, isStart) ;

    where

        <pattern>	- I
            is the (partially) compiled regular expression.
        <state>		- I
            is the state at which to begin computing the shortest path.
            A top-level call to rexShortestPath() will specify the start
            state of the entire regular expression.  The top-level call
            will recursively call rexShortestPath() with current location
            states to compute the lengths of subexpressions within the
            regular expression.
        <isStart>	- I
            should be true in the top-level call to rexShortestPath() and
            false in recursive calls.  In the top-level call, when this
            argument is true, rexShortestPath() will clear all of the
            "visited" flags for the states in RE's graph.  As the states
            are visited, they are flagged as "visited" so that recursive
            calls, with this argument false, do not get into infinite loops.
        <length>	- O
            returns the length of the shortest path through the regular
            expression.

*******************************************************************************/


size_t  rexShortestPath (

#    if PROTOTYPES
        CompiledRE  pattern,
        StateIndex  state,
        bool  isStart)
#    else
        pattern, state, isStart)

        CompiledRE  pattern ;
        StateIndex  state ;
        bool  isStart ;
#    endif

{    /* Local variables. */
    size_t  i, j ;



/* When starting to compute the shortest path through a given regular
   expression, clear the visited flags in all the states. */

    if (state < 0) {
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX" => 0\n", state) ;
        return (0) ;
    }
    if (isStart) {		/* To start, mark each state as not visited. */
        for (i = 0 ;  i < pattern->numStates ;  i++) {
            pattern->stateList[i].visited = false ;
        }
    }
    if (pattern->stateList[state].visited) {
        LGI "(rexShortestPath) State %"PRI_STATEINDEX" vIsItEd => 0\n", state) ;
        return (0) ;
    }
    LGI "(rexShortestPath) State = %"PRI_STATEINDEX"\n", state) ;

/* Mark the state as visited during the traversal.  The VISITED flags keep
   the program from looping endlessly on cycles in the RE's graph (e.g.,
   closure states). */

    pattern->stateList[state].visited = true ;

/* Compute the shortest path from this state to the end of the RE's graph. */

    switch (pattern->stateList[state].type) {
    case Empty:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: Empty\n", state) ;
        return (rexShortestPath (pattern, pattern->stateList[state].next1, false)) ;
    case Anchor:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: Anchor\n", state) ;
        return (rexShortestPath (pattern, pattern->stateList[state].next1, false)) ;
    case Alternation:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: Alternation\n", state) ;
        i = rexShortestPath (pattern, pattern->stateList[state].next1, false) ;
        j = rexShortestPath (pattern, pattern->stateList[state].next2, false) ;
        return ((i < j) ? i : j) ;
    case Closure:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: Closure\n", state) ;
        i = rexShortestPath (pattern, pattern->stateList[state].next1, false) ;
        j = rexShortestPath (pattern, pattern->stateList[state].next2, false) ;
        return ((j * pattern->stateList[state].u.c.minClosure)  +  i) ;
    case Final:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: Final\n", state) ;
        return (0) ;
    case Match:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: Match\n", state) ;
        return (1 +
                rexShortestPath (pattern, pattern->stateList[state].next1, false)) ;
    case LeftParen:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: LeftParen\n", state) ;
        return (rexShortestPath (pattern, pattern->stateList[state].next1, false)) ;
    case RightParen:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: RightParen\n", state) ;
        return (rexShortestPath (pattern, pattern->stateList[state].next1, false)) ;
    default:
        LGI "(rexShortestPath) State = %"PRI_STATEINDEX"  Type: <default>\n", state) ;
        return (0) ;
    }

}
