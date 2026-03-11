/* $Id: rex_util.c,v 1.43 2023/05/24 12:27:41 alex Exp alex $ */
/*******************************************************************************

File:

    rex_util.c

    Regular Expression Matching Utilities


Author:    Alex Measday


Files:

    rex_util.h		Definitions for applications.
    rex_util.c		Caller API for compilation, matching, and substitution.
    rex_internals.h	Internal definitions of compiled RE structures.
    rex_internals.c	Various internally used functions.
    rex_util_y.y	Parser grammar and lexer function.
    rex_util_y.c	Pre-generated parser for use on non-Linux platforms.


Purpose:


        NOTE: I originally wrote these utilities back in late 1989 or
        early 1990.  (Parts of the original file prologue are preserved
        below for my own information.)  The original syntax for regular
        expression matching and replacement was influenced by my
        experience with LEX(1) and YACC(1) and by examination of other
        implementations.

        30 years later, a few years of good experience with the Sigil
        EPUB editor's regular expression find and replace features
        motivated me to finally upgrade this package.  First of all,
        I made the parser fully reentrant (thanks to GNU's Bison) and
        the matching and replacement functions fully reentrant.

        The original search function to match a regular expression against
        text, rexSearch(), was intentionally written in two versions:
        (i) a fully recursive function implemented as a model for (ii) an
        iterative function that emulated the recursive function with a
        manually allocated and managed stack.  For 30 years, I've always
        used the iterative version, although I don't remember if I ever
        compared the performance of the two versions.  The code for the
        iterative version is understandable after some study, but it is
        unwieldy.  In the midst of making these new changes, I switched
        to the recursive version.  I eliminated tail recursion (which
        had been eliminated in the original iterative version from Day 1),
        making rexSearch() a hybrid function with both recursion and
        iteration.

        The following changes were made to the regular expression
        syntax:

          - Subexpressions (capture groups) are now numbered by
            the count of left parentheses.

          - Capture groups can also be given names in addition
            to their automatically assigned numbers.

          - Non-capture groups are supported.

        and to the substitution syntax:

          - Numbered groups can be referenced in the substitution
            string with "\M", "$M", "\<M>", or "$<M>", where M is
            a possibly multi-digit group index, 1..N.

          - Named groups can be referenced as "\<name>" or "$<name>".

          - "\`" or "$`" refer to the text preceding the match;
            "\'" or "$'" refer to the text after the match.

        Capture groups, capturing groups, back references, back-references,
        and backreferences ... what's in a name?  (I am considering adding
        back-references to the package.  Or maybe not.)


    PATTTERN MATCHING
    =================

    The REX utilities provide a simple, regular expression (RE) matching
    and substitution capability to applications and support the following
    regular expression constructs:

        Basic Regular Expression Symbols:

            c       matches character c.

            \c      matches character c.  Escaping c in this way allows
                    matching a literal character c when c is an RE special
                    symbol.  For example, "(" is a special symbol indicating
                    the start of an RE group and doesn't actually match any
                    character itself.  However, "\(" has nothing to do with
                    groups and matches a literal left parenthesis in the
                    target text.

            .       matches any character.

            ^       anchors the left edge of the match at the beginning of
                    the target string. If an RE is not anchored, rexMatch()
                    and rexReplace() will scan the entire text string looking
                    for a match. For example, regular expression "abc" would
                    match those 3 characters in both "abcxyz" and "xyzabc".
                    Anchored regular expression "^abc", on the other hand,
                    will only match the first 3 characters in "abcxyz";
                    no match will be made in "xyzabc".

            $       anchors the right edge of the match at the end of the
                    target string.  For example, regular expression "abc$"
                    matches those 3 characters in "xyzabc", but not in
                    "abcxyz".

            A regular expression anchored at both ends ("^RE$") must consume
            the entire target string in order for the match to succeed.  Aside
            from appearing at the very beginning and at the very end of the
            complete RE, these anchors can also be used with the same meanings
            in alternation expressions; for example, "^abc|^def$" is equivalent
            to matching "^abc$" or "^def$".

        Quantified Regular Expressions:

            RE*     matches zero or more instances of the RE.

            RE+     matches one or more instances of the RE.

            RE?     matches zero or one instance of the RE.

            RE{[m][,[n]]}
                    matches M through N instances of the RE.  If not
                    specified, M defaults to 0.  Depending on whether or
                    not the comma is present, N defaults to M ("RE{m}")
                    or a very large number ("RE{m,}").  "RE*" is equivalent
                    to "RE{0,}".  "RE+" is equivalent to "RE{1,}".  "RE?" is
                    equivalent to "RE{0,1}".

        Concatenation and Alternation:

            RE1RE2  matches RE1 followed immediately by RE2 (no intervening
                    spaces in the RE pattern or in the target string).

            RE1|RE2 matches RE1 or RE2.

        Grouping:

            (RE)    allows grouping of RE's.  These subexpressions also
                    function as capture groups, the indices of which are
                    automatically assigned according to the order of the
                    left parentheses.  The indices begin with 1 and there
                    is no limit on the number of groups, memory permitting.

            (?<name>RE)
                    assigns a name, enclosed in angle brackets, to a
                    capture group.  In replacement and substitution,
                    the capture group can be referenced by index or name.

            (?:RE)  specifies a non-capture group.  The group has no index
                    and cannot be referenced in substitution strings.

            For example, "((RE1)(?:RE2)(?<name>RE3))" has 3 visible
            capture groups:

                group 1 (outer parentheses) - captures RE1, RE2, and RE3.
                group 2 (first inner parentheses) - captures RE1.
                group 3 (third inner parentheses) - captures RE3.

            Group 3 can be referenced in a substitution pattern by name
            ("$<name>") or by number ("$3").

        Special Character Sequences:

            \a \b \f \n \r \t \v
                    match the standard C control characters: alert, backspace,
                    form feed, new line, carriage return, horizontal tab, and
                    vertical tab, respectively.

            \d      matches a decimal digit, '0' to '9'.

            \D      matches anything but a decimal digit, '0' to '9'.

            \xHH    matches the character with the given 2-digit,
                    hexadecimal value.

            \uHHHH  inserts and matches the 1-to-4 characters of the UTF-8
                    representation of Unicode code point U+HHHH.  This is
                    for code points in the Basic Multilingual plane, U+0000
                    through U+FFFF.  The code point must be specified as
                    4 hexadecimal digits.

            \UHHHHHH
                    inserts and matches the 1-to-4 characters of the UTF-8
                    representation of Unicode code point U+HHHHHH.  This is
                    for code points in any of the 17 Unicode planes; the code
                    points range from U+000000 to U+10FFFF.  6 hexadecimal
                    digits must be specified.

            \u{HHH...}
                    specifies a Unicode code point using a variable number
                    of hexadecimal digits and inserts and matches its UTF-8
                    representation.

            NOTE that the Unicode code point specifiers are replaced by
            the 1- to 4-character UTF-8 representation of the code point.
            Consequently, the Unicode specifiers should NOT be used in the
            square-bracketed character classes (immediately below).  As an
            example, the violin emoji, U+1F3BB, a single code point, is
            represented in UTF-8 by a sequence of 4 bytes: 0xF0, 0x9F,
            0x8E, and 0xBB.  If the emoji is specified between square
            brackets, "[\U01F3BB]", it will be replaced internally by
            the 4 UTF-8 "characters": "[\xF0\x9F\x8E\xBB]".  The latter
            will match any single one of the UTF-8 "characters", but not
            the sequence of 4 bytes.  Alternation can be used to simulate
            the desired behavior: "[...whatever...]|\U01F3BB".

        Character Classes (Outer Square Brackets):

            [...]   matches any character between the square brackets.

            [^...]  matches any character from the complement of the
                    characters between the square brackets.

        Character Classes (Within the Brackets):

            c-c     matches any character in a range of characters.  Any
                    number of these ranges may be specified between the
                    square brackets or between the complementing caret
                    and the right square bracket.  For example, "[123a-z45]"
                    matches the numbers '1', '2', '3', '4', and '5' and the
                    lower-case English alphabet (letters 'a' through 'z').

            c       matches the specified character; e.g., "[aeiou]"
                    matches a vowel.

            [:class:]		-- must appear inside [...] or [^...]
                    matches an ASCII character in the specified class,
                    where the possible classes are:
                        [:alnum:]    [:digit:]    [:space:]
                        [:alpha:]    [:graph:]    [:upper:]
                        [:ascii:]    [:lower:]    [:xdigit:]
                        [:blank:]    [:print:]    [:word:]
                        [:cntrl:]    [:punct:]
                    NOTE that the character class, "[:class:]", MUST appear
                    between the generic character class square brackets or
                    between the complementing caret and the right square
                    bracket.  For example, "[^A-Z[:lower:]@#![:digit:]]"
                    will not match the upper-case English alphabet, the
                    lower-case English alphabet, decimal digits, or the
                    punctuation marks '@', '#', and '!'.

    PATTERN-DIRECTED TEXT SUBSTITUTION
    ==================================

    The REX_UTIL package's replacement and substitution functions support
    the following special character sequences in the replacement text:

        \M  $M
        \<M>  $<M>  insert subexpression M (in the range 1..N) matched by
                    the RE.  M may be multiple decimal digits, so use the
                    angle bracket form if digits follow the inserted group.

        \<name>
        $<name>     insert the named group if it was matched by the RE.

        \&  $&
        \0  $0      insert the entire text matched by the RE.

        \`  $`      insert the text preceding the match.

        \'  $'      insert the text after the match.

        \l...
        $l...       insert the numbered or named group matched by the RE,
                    converted to lower case.  For example, "\lM", "$l<M>",
                    "\l<name>", "$l&", and "\l`".

        \u...
        $u...       insert the numbered or named group matched by the RE,
                    converted to upper case.  For example, "$uM", "\u<M>",
                    "$u<name>", "\u&", and "$u'".

        \c          Insert character 'c' (e.g., "\$" gives "$").

    Here are two examples of how to swap two 3-letter sequences, "def"
    and "ghi".  Swapping "defghi" yields "ghidef".  The first example
    replaces every match of "defghi" in the input string with "ghidef".
    The result is the entire input string with each match replaced.

    (In my use of the terms, replacement is different than substitution,
    as will be seen in the second example.  Replacement is the normal
    idea of substitution in a text editor: find each match of the regular
    expression in the text and replace it, leaving the rest of the text
    unchanged.)

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  <stdlib.h>		-- Standard C Library definitions.
        #include  "sto_util.h"		-- String-to-number functions.
        #include  "rex_util.h"		-- Regular expression definitions.

        main (int argc, char *argv[])
        {
            char  *result ;
            CompiledRE  pattern ;

					-- Compile RE.
            rexCompile ("(?<first>def)(?<second>ghi)", &pattern) ;

					-- Global match and replace.
            rexReplace ("abcdefghiabcdefghijkl", -1, pattern,
                        "$<second>$<first>", -1, &result, NULL) ;

					-- Prints "abcghidefabcghidefjkl".
            printf ("Result = "\"%s\"\n", result) ;
            free (result) ;

            rexDestroy (pattern, NULL) ;

            exit (0) ;

        }

    Substitution is different from replacement.  Replacement replaces the
    matched text in the full input string.  Substitution does not target
    the input string; instead, it substitutes matched capture groups in
    a source string that has no relation to the input string.  However,
    replacement makes use of substitution.  A loop in rexReplace() calls
    rexMatch() to find the next match in the input string and then
    rexSubstitute() to replace that match.

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  <stdlib.h>		-- Standard C Library definitions.
        #include  "sto_util.h"		-- String-to-number functions.
        #include  "rex_util.h"		-- Regular expression definitions.

        main (int argc, char *argv[])
        {
            char  *result ;
            CompiledRE  pattern ;
            MatchedRE  match ;

					-- Compile RE; create match object.
            rexCreate ("(?<first>def)(?<second>ghi)", &pattern, &match) ;

					-- Match "defghi".
            rexMatch ("abcdefghijkl", -1, pattern, match, NULL, NULL) ;

					-- Substitution swaps "def" and "ghi".
            rexSubstitute (match, "$<second>$<first>", -1, &result) ;

					-- Prints "ghidef", not input string.
            printf ("Result = \"%s\"\n", result) ;
            free (result) ;

            rexDestroy (pattern, match) ;

            exit (0) ;

        }


Historical Information from Original Prologue:

        (NOTEs added in 2021)

        The main impetus for writing these utilities was that our
        development had been under SunOS, but we were now porting
        our system to HP/UX and real-time operating system VxWorks,
        neither of which supported the same RE functions as SunOS.
        Looking at an archived copy of the 1988 SunOS man pages,
        SunOS's C library included two RE packages.  REGEX(3) was
        a bare-bones RE implementation that only allowed one
        compiled RE at a time and had no replacement function.
        REGEXP(3), also missing a replacement function, allowed
        multiple compiled REs, but the use of global variables
        during matching meant that only one RE match could be
        active at a time.  Not to mention the second sentence
        in REGEXP(3)'s man page: "The interface to this file is
        unpleasantly complex."  I forget which set of functions
        I used before writing my own; probably REGEX(3).

        30 years later, I felt like updating these utilities to handle
        more contemporary RE features and especially to convert the
        parser into a reentrant parser.  The former came out of some
        years of experience using the Sigil EPUB editor's RE search
        and replacement capabilities.  With respect to the latter,
        my original parser was almost reentrant -- good enough for
        horseshoes!  Global variable rtx was used to build and pass
        the compiled RE back to rexCompile().  Also, static file-scope
        variables were used to pass information back and forth between
        the parser and the lexer.

        I had no (apparent!) problems in programs that created multiple
        compiled REs on the fly.  The GNU Bison documentation, in response
        to a frequently asked question about resetting a parser, says
        there is no need -- the problems people have with calling the
        parser repeatedly are caused by FLEX(1)-generated lexers not
        being reset in between parser calls.  My parser had and has a
        hand-written lexer which simply advances a pointer along an
        application-supplied character string; the pointer is always
        reset by rexCompile() before calling the parser.

    These utilities provide a simple, regular expression (RE) matching
    capability.  These utilities are similar to the REGCMP(3) utilities
    (from the System V Programmers Workbench), but REGCMP(3) isn't
    available on all systems.

    ...

    The YACC grammar used to parse the regular expressions is based on the
    grammar found in Chapter 21 ("Parsing") of Robert Sedgewick's book,
    ALGORITHMS.  The graph representation of a regular expression is based
    on the representation described in Chapter 20 ("Pattern Matching").
    The actual matching of a regular expression to a string is performed
    using a fairly straightforward, recursive search of the RE graph - I
    never quite got a handle on Sedgewick's use of double-ended queues!
    HP/UX's REGEXP(5) documentation was used as the authoritative source
    on syntax and semantics of regular expressions.  The idea of generating
    the set of first characters in a regular expression was "stolen" from
    GNU's REGEX functions (they called it a "fastmap").  Henry Spencer's
    test set of regular expressions was used to check out some of the
    capabilities of the REX utilities.

        (NOTE that the following performance tests were done in 1990.)

    The REX utilities are slower than the UNIX regular expression utilities.
    The REGEXP(3) routines are very fast, but they only support a very basic
    set of regular expression constructs.  The REGCMP(3) routines aren't
    quite as fast, but they support a more powerful set of regular expression
    constructs.  The REX utilities come in third, but their performance is
    not bad.  100,000 matches of "aaabbbcdef" by RE "a*b*cdef" on a Sparc1
    workstation took 12 seconds for the REX utilities (that's about 8300
    matches per second) and only 2 seconds for the REGEXP(3) utilities.
    Tests of the REGCMP(3) utilities on an HP workstation showed that they
    are about 4 times as fast as the REX utilities.

    I obtained a copy of UNIX V8-compatible REGEXP(3) routines (not to
    be confused with the REGEXP(3) routines mentioned above) from UUNET.
    These functions were written by Henry Spencer, a noted UNIX guru at
    the University of Toronto.  The REX utilities were generally faster
    than these REGEXP(3) routines, but used more memory; the REGEXP(3)
    routines used a flat character string representation for REs inspired
    by Kernighan and Plauger's approach in their book, SOFTWARE TOOLS.

        (NOTE: My parser used and uses a flat state node array.
        The only dynamically allocated memory in the state node
        is a conditionally allocated, 32-byte, character-set bit
        map for states that match multiple characters; i.e.,
        bracketed character classes.  I have added a field for a
        dynamic string descriptor with the thought of eventually
        merging consecutive SINGLE_CHARACTER states into a single
        STRING state.  Thank you to Henry Spencer for the idea.)

    I used the test set supplied with Henry Spencer's REGEXP(3) code
    to work the kinks out of the REX utilities.  The major change that
    resulted from this exercise was the check for nested, empty closure
    expressions while parsing an RE.  Expressions such as "(a*)*" are
    now rejected, since they would cause endless cycling during a
    pattern match; innocuous nestings like "(a*)?" are also rejected.
    Null expressions (e.g., "()" and "(abc|)def") are now allowed.
    Also, "^" and "$" are always treated as beginning- and end-of-line
    anchors, unless they're escaped by a forward slash.  Consequently,
    regular expressions such as "^abc|^def$" are now possible.


Public Procedures:

    rexCompile() - compiles a regular expression.
    rexCreate() - creates a compiled RE and/or a matched RE structure.
    rexDestroy() - deletes a compiled RE and/or a matched RE structure.
    rexDump() - dumps a compiled regular expression pattern to a file.
    rexGetGroup() - gets a specified capture group from a matched RE structure.
    rexMatch() - attempts to match a subject string against a compiled RE.
    rexMatchReset() - resets a matched RE structure.
    rexNumGroups() - returns the number of capture groups in a compiled RE.
    rexReplace() - search for and replace text matched by a compiled RE.
    rexSubstitute() - substitutes matched text for group references in a string.
    rexWild() - converts a CSH(1)-style wildcard expression to an RE.

Semi-Private Procedures (function prototype in "rex_internals.h"):

    rexDumpClass() - dumps a character class to a file.

Private Procedures:

    rexSearch() - performs a graph search of the RE in order to match the RE
        to the target string.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Standard character functions. */
#ifndef isascii
#    define  isascii(c)  (INT_UCHAR (c) <= 0177)
#endif
#if HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* Standard C string functions. */
#if defined(REX_UTIL_RECURSIVE_SEARCH) && !REX_UTIL_RECURSIVE_SEARCH
#    include  "adx_util.h"		/* Array Descriptor utilities. */
#endif
#if defined(REPORT_ERROR_WITH_MEO_UTIL) && REPORT_ERROR_WITH_MEO_UTIL
#    include  "meo_util.h"		/* Memory operations. */
#endif
#include  "sdx_util.h"			/* String Descriptor utilities. */
#include  "sto_util.h"			/* String-to-number functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "rex_util.h"			/* Regular expression definitions. */
#include  "rex_internals.h"		/* Internal definitions. */


/*******************************************************************************
    Regular Expression Data Structures (in addition to those defined in
    "rex_internals.h").
*******************************************************************************/

typedef  struct  CaptureGroup {
    const  char  *startp ;		/* Pointer to beginning of captured
					   text; NULL means no match yet. */
    const  char  *endp ;		/* Pointer to the very next character
					   following the last character in the
					   captured text; NULL means no match. */
}  CaptureGroup ;

typedef  struct  _MatchedRE {
    CompiledRE  pattern ;		/* Regular expression used for match. */
    int  depth ;			/* Depth of rexSearch() recursion. */
    const  char  *target ;		/* Target text which was searched. */
    bool  matches ;			/* RE matched in target text? */
    CaptureGroup  full ;		/* Full extent of complete match. */
    size_t  maxGroups ;			/* Allocated size of group array. */
    size_t  numGroups ;			/* # of capture groups. */
    CaptureGroup  *group ;		/* Array of capture groups. */
}  _MatchedRE ;

typedef  struct  _AncillarySearchInfo {
    size_t  count ;			/* Closure count. */
}  _AncillarySearchInfo, *AncillarySearchInfo ;

/* When REX_UTIL is compiled for a non-recursive rexSearch(), implement
   the recursive call stack manually in heap memory.  Stored on the stack
   are the local variables of concern.  The "return address" for a call
   is determined based on the state index (saved on the stack), the dynamic
   popped-from-stack and not-popped-from-stack condition, and, in the case
   of a closure state, which steps in its multi-stage processing have been
   completed (saved on the stack). */

#if defined(REX_UTIL_RECURSIVE_SEARCH) && !REX_UTIL_RECURSIVE_SEARCH
    typedef  struct  StackEntry {
        StateIndex  state ;		/* State to return to. */
        size_t  offset ;		/* Offset in text at time of call. */
        size_t  count ;			/* Count for closures. */
        int  step ;			/* Step (0,1,2) in closure processing. */
    }  StackEntry ;
#endif


/*******************************************************************************
    Global variables.
*******************************************************************************/

int  rex_util_debug = 0 ;		/* Debug switch (0 = off, !0 = on). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  rex_util_debug


/*******************************************************************************
    Private Functions
*******************************************************************************/

static  bool  rexSearch (
#    if PROTOTYPES
        CompiledRE  pattern,
        StateIndex  state,
        const  char  *target,
        ssize_t  length,
        size_t  first,
        size_t  *nextChar,
        MatchedRE  match,
        AncillarySearchInfo  ansi
#    endif
    )  OCD ("rex_util") ;

/*!*****************************************************************************

Procedure:

    rexCompile ()


Purpose:

    Function rexCompile() compiles a regular expression pattern into a form
    suitable for matching by rexMatch().

        NOTE:  rexMatch() will return TRUE if the regular expression is
        matched anywhere in the target string.  If you want rexMatch()
        to return TRUE only if the entire target string is matched,
        anchor the match at the beginning and end of the target string
        by putting the '^' and '$' markers at the beginning and end of
        the regular expression you pass into rexCompile().


    Invocation:

        status = rexCompile (expression, &pattern) ;

    where

        <expression>	- I
            is the regular expression to be compiled.
        <pattern>	- O
            returns a handle for the compiled pattern.  This handle
            should be passed into subsequent calls to rexMatch().
        <status>	- O
            returns the status of compiling the regular expression, zero if
            no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  rexCompile (

#    if PROTOTYPES
        const  char  *expression,
        CompiledRE  *pattern)
#    else
        expression, pattern)

        char  *expression ;
        CompiledRE  *pattern ;
#    endif

{

/* Create and initialize a context structure for the regular expression. */

    *pattern = (CompiledRE) malloc (sizeof (_CompiledRE)) ;
    if (*pattern == NULL) {
        LGE "(rexCompile) Error allocating context node for \"%s\".\nmalloc: ",
            expression) ;
        return (errno) ;
    }

    (*pattern)->expression = strdup (expression) ;
    if ((*pattern)->expression == NULL) {
        LGE "(rexCompile) Error duplicating pattern string \"%s\".\nstrdup: ",
            expression) ;
        free (*pattern) ;
        return (errno) ;
    }

    (*pattern)->errorText = NULL ;
    (*pattern)->lex.scan = (*pattern)->expression ;
    (*pattern)->lex.insert = NULL ;
    (*pattern)->lex.insertOffset = 0 ;
    (*pattern)->lex.afterBracket = 0 ;
    (*pattern)->lex.hyphenAsLiteral = false ;
    (*pattern)->lex.nameDelimiter = '\0' ;
    (*pattern)->lex.sdx = NULL ;
    (*pattern)->bolAnchor = false ;
    (*pattern)->numGroups = 0 ;
    (*pattern)->maxGroupNames = 0 ;
    (*pattern)->numGroupNames = 0 ;
    (*pattern)->groupNameLUT = NULL ;
    CS_ZERO (&(*pattern)->buildSet) ;
    CS_ZERO (&(*pattern)->firstSet) ;
    (*pattern)->maxStates = 0 ;
    (*pattern)->numStates = 0 ;
    (*pattern)->startState = -1 ;
    (*pattern)->stateList = NULL ;

    if (sdxCreate (NULL, 0, SdxDynamic, &(*pattern)->lex.insert)) {
        LGE "(rexCompile) Error creating insert string descriptor for lexer.\nsdxCreate: ") ;
        PUSH_ERRNO ;  rexDestroy (*pattern, NULL) ;  POP_ERRNO ;
        return (errno) ;
    }

    if (sdxCreate (NULL, 0, SdxDynamic, &(*pattern)->lex.sdx)) {
        LGE "(rexCompile) Error creating match string descriptor for lexer.\nsdxCreate: ") ;
        PUSH_ERRNO ;  rexDestroy (*pattern, NULL) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Compile the regular expression. */

    if (rrparse (*pattern)) {
        PUSH_ERRNO ;  fflush (stdout) ;  POP_ERRNO ;
        if (errno <= 0)  SET_ERRNO (EINVAL) ;
#if defined(REPORT_ERROR_WITH_MEO_UTIL) && REPORT_ERROR_WITH_MEO_UTIL
        LGE "(rexCompile) Error compiling regular expression:\nrrparse: ") ;
        meoDumpT (stderr, "    ", NULL,
                  (*pattern)->expression, strlen ((*pattern)->expression)) ;
#else
        LGE "(rexCompile) Error compiling regular expression \"%s\"\nrrparse: ",
            expression) ;
#endif
        if (((*pattern)->lex.scan != NULL) && (*(*pattern)->lex.scan != '\0')) {
#if defined(REPORT_REMAINDER_WITH_MEO_UTIL) && REPORT_REMAINDER_WITH_MEO_UTIL
            ptrdiff_t  diff = (*pattern)->lex.scan - (*pattern)->expression ;
            size_t  before = (size_t) diff ;
            size_t  after = strlen ((*pattern)->lex.scan) ;
            LGE "(rexCompile) Parsed before error:\n") ;
            meoDumpT (stderr, "    ", NULL,
                      (*pattern)->expression, (size_t) before) ;
            LGE "(rexCompile) Remainder of RE:\n") ;
            meoDumpT (stderr, "    ", (void *) before,
                      (*pattern)->lex.scan, after) ;
#else
            LGE "(rexCompile) Remainder of RE: \"%s\"\n",
                (*pattern)->lex.scan) ;
#endif
        }
        if (((*pattern)->errorText != NULL) && (*(*pattern)->errorText != '\0')) {
            LGE "(rexCompile) Error text: \"%s\"\n", (*pattern)->errorText) ;
        }
        PUSH_ERRNO ;
        if (rex_util_debug)  rexDump (stdout, *pattern) ;
        rexDestroy (*pattern, NULL) ;  *pattern = NULL ;
        POP_ERRNO ;
        return (errno) ;
    }

    (*pattern)->bolAnchor =
        ((*pattern)->stateList[(*pattern)->startState].type == Anchor) &&
        ((*pattern)->stateList[(*pattern)->startState].u.m.match_char == '^') ;

    sdxDestroy ((*pattern)->lex.insert) ;
    (*pattern)->lex.insert = NULL ;

    sdxDestroy ((*pattern)->lex.sdx) ;
    (*pattern)->lex.sdx = NULL ;

    LGI "(rexCompile) CompiledRE %p created for \"%s\".\n",
        (void *) *pattern, expression) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexCreate ()


Purpose:

    Function rexCreate() creates a compiled regular expression object and/or
    a matched regular expression object.

        NOTE: When both a compiled RE object and a match structure are
        being created and an error occurs, neither entity is returned
        to the calling program.  If the RE compilation fails, no attempt
        is made to create the match structure.  If the RE compilation is
        successful and creation of the match structure fails, the compiled
        RE object is destroyed.


    Invocation:

        status = rexCreate (expression, &pattern, &match) ;

    where

        <expression>	- I
            is the regular expression to be compiled.
        <pattern>	- O
            returns a handle for the compiled pattern.  This handle
            should be passed into subsequent calls to rexMatch().
            If the pass-by-reference address for this argument is NULL,
            a compiled regular expression will not be created.
        <match>		- O
            returns a handle for the match structure.  This handle should
            be passed into subsequent calls to rexMatch() or rexReplace().
            If the pass-by-reference address for this argument is NULL,
            a matched regular expression object will not be created.
        <status>	- O
            returns the status of compiling the regular expression or creating
            the match structure, zero if no errors occurred and ERRNO otherwise.
            If the regular expression was successfully compiled and creating a
            match structure ended in error, the compiled RE is destroyed.

*******************************************************************************/


errno_t  rexCreate (

#    if PROTOTYPES
        const  char  *expression,
        CompiledRE  *pattern,
        MatchedRE  *match)
#    else
        expression, pattern, match)

        char  *expression ;
        CompiledRE  *pattern ;
        MatchedRE  *match ;
#    endif

{

    if (pattern != NULL)  *pattern = NULL ;
    if (match != NULL)  *match = NULL ;

/* Compile the regular expression into a pattern object. */

    if ((pattern != NULL) && rexCompile (expression, pattern)) {
        LGE "(rexCreate) Error compiling \"%s\".\nrexCompile: ", expression) ;
        return (errno) ;
    }

/* Create and initialize the matched RE structure. */

    if (match == NULL)  return (0) ;

    *match = (MatchedRE) malloc (sizeof (_MatchedRE)) ;
    if (*match == NULL) {
        LGE "(rexCreate) Error allocating matched RE structure.\nmalloc: ") ;
        PUSH_ERRNO ;
        if (*pattern != NULL) {
            rexDestroy (*pattern, NULL) ;  *pattern = NULL ;
        }
        POP_ERRNO ;
        return (errno) ;
    }
    (*match)->pattern = (pattern == NULL) ? NULL : *pattern ;
    (*match)->depth = 0 ;
    (*match)->target = NULL ;
    (*match)->matches = false ;
    (*match)->full.startp = NULL ;
    (*match)->full.endp = NULL ;
    (*match)->maxGroups = 0 ;
    (*match)->numGroups = 0 ;
    (*match)->group = NULL ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexDestroy ()


Purpose:

    Function rexDestroy() frees up the memory occupied by a compiled regular
    expression and/or a matched regular expression.  The function should be
    called after a program has finished using the compiled RE and or the
    matched RE.


    Invocation:

        status = rexDestroy (pattern, match) ;

    where

        <pattern>	- I
            is the handle for the compiled pattern returned by rexCompile().
            NULL can be specified if the caller is only deleting a match.
        <match>		- I
            is the handle for the regular expression match returned
            by rexMatch().  NULL can be specified if the caller is only
            deleting a compiler regular expression.
        <status>	- O
            returns the status of deleting either of the input arguments,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  rexDestroy (

#    if PROTOTYPES
        CompiledRE  pattern,
        MatchedRE  match)
#    else
        pattern, match)

        CompiledRE  pattern ;
        MatchedRE  match ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (pattern != NULL) {
        if (pattern->expression != NULL)  free (pattern->expression) ;
        if (pattern->errorText != NULL)  free (pattern->errorText) ;
        if (pattern->lex.insert != NULL)  sdxDestroy (pattern->lex.insert) ;
        if (pattern->lex.sdx != NULL)  sdxDestroy (pattern->lex.sdx) ;
        if (pattern->groupNameLUT != NULL) {
            for (i = 0 ;  i < pattern->numGroupNames ;  i++) {
                if (pattern->groupNameLUT[i].name != NULL)
                    free ((char *) pattern->groupNameLUT[i].name) ;
            }
            free (pattern->groupNameLUT) ;
        }
        if (pattern->stateList != NULL) {
            for (i = 0 ;  i < pattern->numStates ;  i++) {
                if ((pattern->stateList[i].type == Match) &&
                    (pattern->stateList[i].u.m.match_char ==
                     REX_MATCH_CHAR_CHARSET) &&
                    (pattern->stateList[i].u.m.match_charset != NULL))
                    free (pattern->stateList[i].u.m.match_charset) ;
            }
            free (pattern->stateList) ;
        }
        free (pattern) ;
    }

    if (match != NULL) {
        if (match->group != NULL)  free (match->group) ;
        free (match) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexDump ()


Purpose:

    Function rexDump() dumps the contents of a compiled regular expression.


    Invocation:

        status = rexDump (outfile, pattern) ;

    where

        <outfile>	- I
            is the UNIX file descriptor (FILE *) for the output file.
        <pattern>	- I
            is the handle for the compiled pattern returned by rexCompile().
        <status>	- O
            returns zero.

*******************************************************************************/


errno_t  rexDump (

#    if PROTOTYPES
        FILE  *outfile,
        CompiledRE  pattern)
#    else
        outfile, pattern)

        FILE  *outfile ;
        CompiledRE  pattern ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (pattern == NULL)  return (0) ;

/* Dump the context node. */

    fprintf (outfile, "Regular Expression: \"%s\"\n", pattern->expression) ;
    fprintf (outfile, "              Scan: \"%s\"\n",
             (pattern->lex.scan == NULL) ? "<null>" : pattern->lex.scan) ;
    fprintf (outfile, "        BOL Anchor: %s\n",
             (pattern->bolAnchor ? "yes" : "no")) ;

    fprintf (outfile, "\n  # Capture Groups: %"PRIuSIZE_T"\n",
             pattern->numGroups) ;
    for (i = 0 ;  i < pattern->numGroupNames ;  i++) {
        fprintf (outfile, "       Group Index: %ld\tName: \"%s\"\n",
                 pattern->groupNameLUT[i].number,
                 pattern->groupNameLUT[i].name) ;
    }
    rexDumpClass (outfile, "             First: ", &pattern->firstSet) ;

    fprintf (outfile, "\n  Maximum # states: %"PRIuSIZE_T"\n",
             pattern->maxStates) ;
    fprintf (outfile, "   Actual # states: %"PRIuSIZE_T"\n",
             pattern->numStates) ;
    fprintf (outfile, "       Start state: %"PRI_STATEINDEX"\n\n",
             pattern->startState) ;


/* Dump the list of states. */

    if (pattern->stateList != NULL) {

        StateNode  *slist = pattern->stateList ;

        for (i = 0 ;  i < pattern->numStates ;  i++) {

            StateIndex  next1, next2, paren ;
            StateType  type, type1, type2, typep ;
            const  char  *name, *name1, *name2, *namep ;

            type = slist[i].type ;
            name = rexNameOf (type, true) ;

            next1 = slist[i].next1 ;
            type1 = (next1 >= 0) ? slist[next1].type : -1 ;
            name1 = (next1 >= 0) ? rexNameOf (type1, true) : "<>" ;

            next2 = slist[i].next2 ;
            type2 = (next2 >= 0) ? slist[next2].type : -1 ;
            name2 = (next2 >= 0) ? rexNameOf (type2, true) : "<>" ;

            if ((slist[i].type == LeftParen) || (slist[i].type == RightParen))
                paren = slist[i].u.p.subexpState ;
            else
                paren = -1 ;
            typep = (paren >= 0) ? slist[paren].type : -1 ;
            namep = (paren >= 0) ? rexNameOf (typep, true) : "<>" ;

            switch (slist[i].type) {
            case Empty:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%s\n",
                         i, name, next1, name1, next2, name2) ;
                break ;
            case Anchor:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%-8s  Anchor: '%c'\n",
                         i, name, next1, name1, next2, name2,
                         slist[i].u.m.match_char) ;
                break ;
            case Alternation:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%s\n",
                         i, name, next1, name1, next2, name2) ;
                break ;
            case Closure:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%-8s  Min/Max: %"PRIuSIZE_T" %"PRIuSIZE_T"\n",
                         i, name, next1, name1, next2, name2,
                         slist[i].u.c.minClosure, slist[i].u.c.maxClosure) ;
                break ;
            case Final:
                fprintf (outfile, "%3"PRIuSIZE_T": %s\n", i, name) ;
                break ;
            case LeftParen:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%-8s  Group/Paren = %"PRIuSIZE_T" %"PRI_STATEINDEX":%s\n",
                         i, name, next1, name1, next2, name2,
                         slist[i].u.p.subexpIndex,
                         slist[i].u.p.subexpState, namep) ;
                break ;
            case Match:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%-8s  Matches '%c' (0x%02X)\n",
                         i, name, next1, name1, next2, name2,
                         slist[i].u.m.match_char,
                         INT_UCHAR (slist[i].u.m.match_char)) ;
                if (slist[i].u.m.match_char < 0)
                    rexDumpClass (outfile, "        Matches: ",
                                  slist[i].u.m.match_charset) ;
                break ;
            case RightParen:
                fprintf (outfile, "%3"PRIuSIZE_T": %-8s  --> %3"PRI_STATEINDEX":%-8s  %3"PRI_STATEINDEX":%-8s  Group/Paren = %"PRIuSIZE_T" %"PRI_STATEINDEX":%s\n",
                         i, name, next1, name1, next2, name2,
                         slist[i].u.p.subexpIndex,
                         slist[i].u.p.subexpState, namep) ;
                break ;
            default:
                fprintf (outfile, "%3"PRIuSIZE_T": %s\n", i, name) ;
                break ;
            }

        }
    }


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexDumpClass ()


Purpose:

    Function rexDumpClass() dumps a character class.


    Invocation:

        status = rexDumpClass (outfile, leadIn, &charSet) ;

    where

        <outfile>	- I
            is the UNIX file descriptor (FILE *) for the output file.
        <leadIn>	- I
            is the lead-in text for an output dump line.  For each group
            of contiguous characters in the set, the lead-in text is
            output, followed by the starting and ending characters in
            the range of contiguous characters.
        <charSet>	- I
            is a pointer to the character set structure that is to be dumped.
        <status>	- O
            returns zero.

*******************************************************************************/


errno_t  rexDumpClass (

#    if PROTOTYPES
        FILE  *outfile,
        const  char  *leadIn,
        cs_set  *charSet)
#    else
        outfile, leadIn, charSet)

        FILE  *outfile ;
        char  *leadIn ;
        cs_set  *charSet ;
#    endif

{    /* Local variables. */
    bool  inRange ;
    int  i ;



    if (charSet == NULL)  return (0) ;

    inRange = false ;
    for (i = 1 ;  i < CS_SETSIZE ;  i++) {
        if (CS_ISSET (i, charSet)) {
            if (!inRange) {
                if (leadIn != NULL)  fprintf (outfile, "%s", leadIn) ;
                if ((i < CS_SETSIZE - 1) && CS_ISSET (i+1, charSet)) {
                    if (isprint (i))
                        fprintf (outfile, "'%c' - ", i) ;
                    else
                        fprintf (outfile, "'\\%3.3o' - ", i) ;
                }
                inRange = true ;
            }
            if (i < CS_SETSIZE - 1)  continue ;
        }
        if (inRange) {
            if (isprint (i-1))
                fprintf (outfile, "'%c'\n", i - 1) ;
            else
                fprintf (outfile, "'\\%3.3o'\n", i - 1) ;
            inRange = false ;
        }
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexGetGroup ()


Purpose:

    Function rexGetGroup gets a specified capture group (parenthesized
    subexpression) from a matched regular expression.


    Invocation:

        status = rexGetGroup (match, group, &groupStart, &groupLength) ;

    where

        <match>		- I
            is a matched RE structure previously populated by a call to
            rexMatch(), so any capture groups in the matched text are
            fully defined.
        <group>		- I
            is the number (1..9) of the desired capture group.  Group 0
            returns the full matched text.
        <groupStart>	- O
            returns a pointer to the beginning of the group's text.  NULL
            is returned if the group was not included in the match (perhaps
            because of alternation).  The function's return value in the
            latter case is still zero for success.  This argument can be
            NULL if the caller does not need the group start returned.
        <groupLength>	- O
            returns the length of the group's text.  This argument can be
            NULL if the caller does not need the group length returned.
        <status>	- O
            returns the status of getting the group, zero on success
            and ERRNO otherwise.

*******************************************************************************/


errno_t  rexGetGroup (

#    if PROTOTYPES
        MatchedRE  match,
        size_t  group,
        const  char  **groupStart,
        size_t  *groupLength)
#    else
        match, group, groupStart, groupLength)

        MatchedRE  match ;
        size_t  group ;
        const  char  **groupStart ;
        size_t  *groupLength ;
#    endif

{    /* Local variables. */
    CaptureGroup  *capture ;



    if (groupStart != NULL)  *groupStart = NULL ;	/* In case of error. */
    if (groupLength != NULL)  *groupLength = 0 ;

    if (match == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexGetGroup) NULL match.\n") ;
        return (errno) ;
    } else if (!match->matches) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexGetGroup) Regular expression not matched.\n") ;
        return (errno) ;
    } else if (group > match->numGroups) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexGetGroup) Invalid group #%"PRIuSIZE_T".\n", group) ;
        return (errno) ;
    }

    capture = (group == 0) ? &match->full : &match->group[group-1] ;

    if ((capture->startp == NULL) || (capture->endp == NULL)) {
        if (groupStart != NULL)  *groupStart = NULL ;
        if (groupLength != NULL)  *groupLength = 0 ;
    } else {
        if (groupStart != NULL)  *groupStart = capture->startp ;
        if (groupLength != NULL)
            *groupLength = capture->endp - capture->startp ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexMatch ()


Purpose:

    Function rexMatch() tries to match a target string against a compiled
    regular expression.

        NOTE:  rexMatch() returns TRUE if the regular expression is matched
        anywhere in the target string.  If you want rexMatch() to return TRUE
        only if the entire target string is matched, anchor the match at the
        beginning and end of the target string by putting the '^' and '$'
        markers at the beginning and end of the regular expression you pass
        into rexCompile().


    Invocation:

        matched = rexMatch (target, length, pattern, match,
                            &matchStart, &matchLength) ;

    where

        <target>	- I
            is the string which will be matched against the regular expression.
        <length>	- I
            is the length of the target string.  If this argument is less than
            zero, the text is assumed to be NUL-terminated and strlen(3) is
            used to determine its length.
        <pattern>	- I
            is the handle for the compiled pattern returned by rexCompile().
        <match>		- I/O
            is a handle for a match structure returned by rexCreate().  The
            structure returns more information about the match, especially
            the fragments of text captured by the parenthesized subexpressions
            in the full regular expression.  If necessary, rexMatch() will
            expand the structure to include the number of subexpressions in
            the regular expression.
        <matchStart>	- O
            returns a pointer to the start of the substring in TARGET that was
            matched by the regular expression; a pointer to TARGET is returned
            if the regular expression was not matched.  This argument can be
            NULL if you're not interested in this information.
        <matchLength>	- O
            returns the length of the substring in TARGET that was matched
            by the regular expression; zero is returned if the regular
            expression was not matched (or if the matched substring was of
            length 0).  This argument can be NULL if you're not interested
            in this information.
        <matched>	- O
            returns true if the target string matches the compiled RE;
            false is returned if the target was not matched.

*******************************************************************************/


bool  rexMatch (

#    if PROTOTYPES
        const  char  *target,
        ssize_t  length,
        CompiledRE  pattern,
        MatchedRE  match,
        const  char  **matchStart,
        size_t  *matchLength)
#    else
        target, length, pattern, match, matchStart, matchLength)

        const  char  *target ;
        ssize_t  length ;
        CompiledRE  pattern ;
        MatchedRE  match ;
        const  char  **matchStart ;
        size_t  *matchLength ;
#    endif

{    /* Local variables. */
    bool  allocated, matched ;
    size_t  first, next ;




    if ((pattern == NULL) || (target == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexMatch) NULL pattern\n") ;
        return (false) ;
    }

    if (length < 0)  length = strlen (target) ;

/* If no matched RE structure was supplied, then create a temporary one. */

    if (match == NULL) {
        if (rexCreate (NULL, NULL, &match)) {
            LGE "(rexMatch) Error creating match structure.\nrexCreate: \n") ;
            return (false) ;
        }
        allocated = true ;
    } else {
        allocated = false ;
    }

/* At each character position in the target string, try matching the regular
   expression.  If the character at the starting character position is not
   in the first set of characters for the regular expression, then skip
   that character position. */

#ifdef QUESTION
    *************  What about zero-length target
    *************  and RE that matches zero-length string?
#endif
    LGI "(rexMatch) First = %"PRIuSIZE_T"  Length = %"PRIdSSIZE_T"  next = %"PRIuSIZE_T"\n", (size_t) 0, length, (size_t) 0) ;
    for (matched = false, first = next = 0 ;
         first <= (size_t) length ;
         first++) {
#ifdef NOT_NOW
        if (!CS_ISSET (target[first], &pattern->firstSet)) {
    LGI "(rexMatch) First = %"PRIuSIZE_T"  Length = %"PRIdSSIZE_T"  next = %"PRIuSIZE_T"  (CONTINUE)\n", first, length, next) ;
            continue ;					/* No match possible. */
        }
#endif
        if (rexMatchReset (match, pattern)) {
            LGE "(rexMatch) Error initializing match structure.\nrexMatchReset: \n") ;
            if (allocated) {
                PUSH_ERRNO ;  rexDestroy (NULL, match) ;  POP_ERRNO ;
            }
            return (false) ;
        }
        match->target = target ;
        LGI "(rexMatch) Scanning \"%s\" for \"%s\"\n",
            &target[first], pattern->expression) ;
        matched = rexSearch (pattern, pattern->startState, target,
                             length, first, &next, match, NULL) ;
        if (matched)  break ;			/* Successful match? */
        if (pattern->bolAnchor)  break ;	/* Only match at beginning-of-line? */
    LGI "(rexMatch) First = %"PRIuSIZE_T"  Length = %"PRIdSSIZE_T"  next = %"PRIuSIZE_T"\n", first, length, next) ;
    }

    if (matched) {
        match->matches = true ;
        match->full.startp = target + first ;
        match->full.endp = target + next ;
        if (matchStart != NULL)  *matchStart = match->full.startp ;
        if (matchLength != NULL)
            *matchLength = match->full.endp - match->full.startp ;
        if (I_DEFAULT_GUARD) {
            size_t  i ;
            for (i = 0 ;  i <= rexNumGroups (pattern) ;  i++) {
                const  char  *groupStart ;
                size_t  groupLength ;
                rexGetGroup (match, i, &groupStart, &groupLength) ;
                if (groupStart == NULL)  continue ;
                LGI "(rexMatch) Group[%"PRIuSIZE_T"] = \"%.*s\"\n",
                    i, (int) groupLength, groupStart) ;
            }
        }
    } else {
        match->matches = false ;
        match->full.startp = NULL ;  match->full.endp = NULL ;
        if (matchStart != NULL)  *matchStart = NULL ;
        if (matchLength != NULL)  *matchLength = 0 ;
    }


    if (allocated)  rexDestroy (NULL, match) ;

    return (matched) ;

}

/*!*****************************************************************************

Procedure:

    rexMatchReset ()


Purpose:

    Function rexMatchReset() resets the search fields in a matched RE
    structure.  It's intended for use by rexMatch().  As that function
    steps through a target string, trying to match an unanchored regular
    expression at each character position, it needs to reset the matched
    RE structure before calling rexSearch() for the next position.


    Invocation:

        status = rexMatchReset (match, pattern) ;

    where

        <match>		- I
            is a matched RE structure created by rexCreate() and possibly
            used in previous calls to rexSearch().
        <pattern>	- I
            is the handle for the compiled pattern returned by rexCompile()
            or rexCreate().  If this argument is NULL, rexMatchReset() will
            use the existing pattern in the match structure.
        <status>	- O
            returns the status of resetting the matched RE structure,
            zero on success and ERRNO otherwise.

*******************************************************************************/


errno_t  rexMatchReset (

#    if PROTOTYPES
        MatchedRE  match,
        CompiledRE  pattern)
#    else
        match, pattern)

        MatchedRE  match ;
        CompiledRE  pattern ;
#    endif

{    /* Local variables. */
    size_t  i, numGroups ;



    if (match == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexMatchReset) NULL match.\n") ;
        return (errno) ;
    }

    if (pattern != NULL)  match->pattern = pattern ;

    match->depth = 0 ;
    match->matches = false ;
    match->full.startp = NULL ;
    match->full.endp = NULL ;

/* If the compiled RE has changed (i.e., the number of groups is different),
   then free the existing capture group array, if any. */

    numGroups = (match->pattern == NULL) ? 0 : rexNumGroups (match->pattern) ;

    if (numGroups == 0) {
        if ((match->maxGroups > 0) && (match->group != NULL))
            free (match->group) ;
        match->group = NULL ;
        match->maxGroups = 0 ;
        match->numGroups = 0 ;
    }

/* If a new capture group array has to be allocated (i.e., the number of
   groups is greater than the maximum currently allocated), then do so. */

    if (numGroups > match->maxGroups) {
        if ((match->maxGroups > 0) && (match->group != NULL))
            free (match->group) ;
        match->maxGroups = match->numGroups ;
        match->group = calloc (numGroups, sizeof (CaptureGroup)) ;
        if (match->group == NULL) {
            LGE "(rexMatch) Error allocating %"PRIuSIZE_T"-element group array.\ncalloc: ",
                numGroups) ;
            return (false) ;
        }
        match->maxGroups = match->numGroups = numGroups ;
    }

/* Initialize each of the capture groups to "not found". */

    for (i = 0 ;  i < match->numGroups ;  i++) {
        match->group[i].startp = match->group[i].endp = NULL ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexNumGroups ()


Purpose:

    Function rexNumGroups returns the number of capture groups (parenthesized
    subexpressions) defined for a compiled regular expression.


    Invocation:

        numGroups = rexNumGroups (pattern) ;

    where

        <pattern>	- I
            is the handle for the compiled pattern returned by rexCompile().
        <numGroups>	- O
            returns the number of capture groups defined in a regular
            expression.  0 is returned if there are no groups or in case
            of an error.

*******************************************************************************/


size_t  rexNumGroups (

#    if PROTOTYPES
        CompiledRE  pattern)
#    else
        pattern)

        CompiledRE  pattern ;
#    endif

{
    if (pattern == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexNumGroups) NULL pattern.\n") ;
        return (0) ;
    } else {
        return (pattern->numGroups) ;
    }
}

/*!*****************************************************************************

Procedure:

    rexReplace ()


Purpose:

    Function rexReplace() performs a search-and-replace operation on a
    string of text.  The search string (specified by a previously-compiled
    regular expression) is located in the source string and replaced by the
    substitution text.  This process may be repeated a specified number of
    times.  rexReplace() is intended to perform EX(1)-style subsitutions

                          s/<RE>/<replacement>/[g]

    on a line of text.  For example:

        s/abc/def/	(Replaces the first occurrence of "abc" with "def".)
        s/abc/xyz/g	(Replaces all occurrences of "abc" with "xyz".)

    In line with the LEX(1)-style origins of the REX_UTIL package,
    rexReplace() uses a notation for its replacement text different
    than that used by other regular expression packages.  The following
    character sequences in replacement text cause the specified actions:

        $1 - $9    Insert subexpression (1..9) matched by RE.
        $&         Insert entire text matched by RE.
        $l1 - $l9  Insert subexpression (1..9) matched by RE,
                   converted to lower case.
        $l&        Insert entire text matched by RE, converted
                   to lower case.
        $u1 - $u9  Insert subexpression (1..9) matched by RE,
                   converted to upper case.
        $u&        Insert entire text matched by RE, converted
                   to upper case.
        \c         Insert character 'c' (e.g., "\$" gives "$").

    For example, applying the following substitution command:

        s/(abc)(def)/$2$1 is $& split in half and reversed/

    to source string "abcdef" produces this result:

        "defabc is abcdef split in half and reversed"

    rexReplace() is probably not very fast - I'm just pleased it works!


        NOTE: If rexReplace() returns successfully -- no errors -- the
        result argument ALWAYS returns a dynamically allocated string,
        which the caller is responsible for FREE(3)ing.  The result
        may be a zero-length string, "", but it is still dynamically
        allocated.  If rexReplace() returns with an error status, the
        result string is always NULL and should not be used.


    Invocation:

        status = rexReplace (source, length, pattern, replacement,
                             maxSubstitutions, &result, &numSubstitutions) ;

    where

        <source>		- I
            is the text string to which the search-and-replace operation
            is to be applied.
        <length>	- I
            is the length of the source string.  If this argument is less than
            zero, the text is assumed to be NUL-terminated and strlen(3) is
            used to determine its length.
        <pattern>		- I
            is the handle for the compiled pattern returned by rexCompile().
        <replacement>		- I
            is a string specifying the substitutions to be made in the
            source string when the search string is matched.  See the
            description of special character sequences above.
        <maxSubstitutions>	- I
            specifies the maximum number (0..N) of substitutions that are
            to be made in the source string.  A value of -1 causes global
            substitutions in the source string.  Substitutions are not
            recursive; the search for the next occurrence of the search
            string in the source string begins following the end of the
            last match.
        <result>		- O
            returns the result of substituting the replacement text in the
            source string.  RESULT returns a pointer to a dynamically allocated
            string of the necessary length.  The calling program is responsible
            for FREE(3)ing the result string.
        <numSubstitutions>	- O
            returns the actual number of substitutions (0..N) made in the
            source string.  This argument may be NULL if you're not interested
            in the return value.
        <status>		- O
            returns the status of performing the search-and-replace operation,
            zero if no errors occurred (whether or not any substitutions were
            made) and ERRNO otherwise (e.g., unable to MALLOC(3) space for the
            RESULT string).

*******************************************************************************/


errno_t  rexReplace (

#    if PROTOTYPES
        const  char  *source,
        ssize_t  length,
        CompiledRE  pattern,
        const  char  *replacement,
        ssize_t  maxSubstitutions,	/* -1 = throughout source string. */
        char  **result,
        size_t  *numSubstitutions)
#    else
        source, length, pattern, replacement, maxSubstitutions,
        result, numSubstitutions)

        char  *source ;
        ssize_t  length ;
        CompiledRE  pattern ;
        char  *replacement ;
        ssize_t  maxSubstitutions ;	/* -1 = throughout source string. */
        char  **result ;
        size_t  *numSubstitutions ;
#    endif

{    /* Local variables. */
    char  *matchStart, *substitution ;
    const  char  *groupStart ;
    MatchedRE  match ;
    size_t  i, groupLength, matchLength, matchOffset, numSubs ;
    StringDx  rdx ;




    if ((source == NULL) || (pattern == NULL) ||
        (replacement == NULL) || (result == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexReplace) NULL source, pattern, replacement, or result.\n") ;
        return (errno) ;
    }

    if (length < 0)  length = strlen (source) ;

    *result = NULL ;
    if (numSubstitutions == NULL)  numSubstitutions = &numSubs ;

    LGI "(rexReplace) #Fields = %"PRIuSIZE_T"  Max Substitutions = %"PRIdSSIZE_T"\n",
        rexNumGroups (pattern), maxSubstitutions) ;


/* Create a matched RE structure for the regular expression's capture groups.
   The fields will be filled in by each rexMatch() call. */

    if (rexCreate (NULL, NULL, &match)) {
        LGE "(rexReplace) Error creating matched RE struture.\nrexCreate: ") ;
        return (errno) ;
    }


/* Create a string descriptor for a dynamically growing result string.
   The string is created with an initial, zero-length string, "", in
   case the input source is a zero-length string.  Not matching the
   regular expression and not making any substitutions are NOT errors.
   In the single case I encountered, the old rexReplace() returned
   successfully with the result string a NULL pointer; the calling
   program crashed when using the NULL result in a strcmp(3) call. */

    if (sdxCreate ("", 0, SdxVolatile, &rdx)) {
        LGE "(rexReplace) Error creating result string descriptor.\nsdxCreate: ") ;
        PUSH_ERRNO ;  rexDestroy (NULL, match) ;  POP_ERRNO ;
        return (errno) ;
    }


/* For the requested number of substitutions or until no more substitutions
   can be made, match the regular expression in the current result string
   (initially the source string) and substitute the replacement text for the
   matched text.  Substitutions are not recursive: after each substitution,
   the attempt at matching the regular expression begins following the
   substituted text. */

    if (maxSubstitutions <= 0)  maxSubstitutions = (ssize_t) strlen (source) ;
    matchLength = 0 ;  matchOffset = 0 ;

    for (*numSubstitutions = 0 ;
         *numSubstitutions < (size_t) maxSubstitutions ;
         (*numSubstitutions)++) {


/* Try matching the regular expression with the text following the last match. */

        LGI "(rexReplace) Iteration = %"PRIuSIZE_T"  Result = \"%s\"  Source = \"%s\"\n",
            *numSubstitutions,
            (sdxStringZ (rdx) == NULL) ? "" : sdxStringZ (rdx),
            source) ;

        if (!rexMatch (source, length, pattern, match,
                       (const char **) &matchStart, &matchLength)) {
            break ;		/* No match. */
        }
        matchOffset = matchStart - source ;

        LGI "(rexReplace) Match = \"%.*s\"  Length = %"PRIuSIZE_T"\n",
            (int) matchLength, matchStart, matchLength) ;
        for (i = 0 ;  i < match->numGroups ;  i++) {
            rexGetGroup (match, i+1, &groupStart, &groupLength) ;
            if (groupStart == NULL) {
                LGI "(rexReplace)     Field[%"PRIuSIZE_T"] = NULL\n", i) ;
            } else {
                LGI "(rexReplace)     Field[%"PRIuSIZE_T"] = \"%.*s\"  Length = %"PRIuSIZE_T"\n",
                    i, (int) groupLength, groupStart, groupLength) ;
            }
        }


/* Add the pre-match text to the result string; i.e., the text between the
   end of the last match and the beginning of this match.  Since the SOURCE
   variable points to the first character after the end of the last match,
   the length of the pre-match text is simply the offset of this match. */

        if (sdxAddS (rdx, source, matchOffset)) {
            LGE "(rexReplace) Error adding pre-match text to result.\nsdxAddS: ") ;
            PUSH_ERRNO ;  rexDestroy (NULL, match) ;
            sdxDestroy (rdx) ;  POP_ERRNO ;
            return (errno) ;
        }


/* Next, substitute the replacement text for the matched text in the result
   string. */

        if (rexSubstitute (match, replacement, -1, &substitution)) {
            LGE "(rexReplace) Error substituting replacement for match.\nrexSubstitute: ") ;
            PUSH_ERRNO ;  rexDestroy (NULL, match) ;
            sdxDestroy (rdx) ;  POP_ERRNO ;
            return (errno) ;
        }


        if (sdxAddS (rdx, substitution, -1)) {
            LGE "(rexReplace) Error adding substituted text to result.\nsdxAddS: ") ;
            PUSH_ERRNO ;  rexDestroy (NULL, match) ;
            sdxDestroy (rdx) ;  free (substitution) ;  POP_ERRNO ;
            return (errno) ;
        }

        free (substitution) ;

/* NOTE that changes are made in the substitution string returned by
   rexSubstitute(), so SOURCE can simply advance through the input
   source string.  Here, advance SOURCE to the first character after
   the matched text in the source so that, at the top of the loop,
   rexMatch() will beginning scanning there for the next match. */

        source += matchOffset + matchLength ;


    }     /* For each substitution. */


    if (*source != '\0') {	/* Trailing unmatched text? */
        if (sdxAddS (rdx, source, -1)) {
            LGE "(rexReplace) Error adding post-matchs text to result.\nsdxAddS: ") ;
            PUSH_ERRNO ;  rexDestroy (NULL, match) ;
            sdxDestroy (rdx) ;  POP_ERRNO ;
            return (errno) ;
        }
    }


    *result = sdxStringZ (rdx) ;
    sdxOwn (rdx) ;  sdxDestroy (rdx) ;


    LGI "(rexReplace) # of Substitutions Made: %"PRIuSIZE_T"\n", *numSubstitutions) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexSubstitute ()


Purpose:

    Function rexSubstitute() substitutes capture groups from a previously
    matched regular expression for group references in a string.  The
    group references can include the fully matched text as well as RE
    subexpressions.  The resulting string is returned to the caller in
    a dynamically allocated string that the caller is responsible for
    freeing when it is no longer needed.

    I wrote the REX_UTIL package back around 1990.  The package included
    rexMatch() to match a regular expression against a string and
    rexReplace() to perform text-editor-like matches and replacements.
    As part of my testing, I downloaded Henry Spencer's famous regex(3)
    package and test suite, modified the test suite to use REX_UTIL's
    syntax, and ran the suite using the REX_UTIL package.

    In 2021, I looked again at Henry's test suite and realized that what
    I had remembered as the match replacement string in each test was
    actually an arbitrary string containing references to the match's
    subexpressions.  In other words, each test matched a regular
    expression against string A, let's call it, and then, if necessary,
    substituted the matched text and subexpressions from string A into
    an altogether different string, string B.  This seemed odd to me
    because the replacement string in my rexReplace() function is used
    to replace only the matched text in the original string, which I
    thought would be the usually desired functionality.

    Fortunately, I had an epiphany.  The regex(3) package calls reg_exec()
    to match an RE against string A and then calls reg_sub() to substitute
    the matched groups from string A into string B.  String B could very
    well be my replacement string which, after substitution, replaces the
    matched text in the original string.  My original rexReplace() did
    something similar in that it calls rexMatch() to match an RE against
    a string and then internally performs the substitutions specified by
    the replacement string in a copy of the original input string.  So,
    I've moved the substitution logic into a separate function, this
    function.

        NOTE that rexReplace() is a bit more complicated than
        a call to rex_exec() and to reg_sub().  The application
        can specify how many replacements are to be made in a
        string, perhaps only the first match in the string or
        perhaps all of the matches in the string.  So there is
        a big loop for each of the attempted matches.  At the
        top of the loop is a call to rexMatch() to search for
        a match.  If found, the guts of the loop performed the
        substitutions, now done in rexSubstitute().  However,
        rexReplace() must also keep track of pre- and post-match
        text; also, for multiple match/replacements, the function
        must be sure to begin the search for a new match after the
        end of the last match.

    The following character sequences in the source string cause the
    specified actions:

        \M  $M
        \<M>  $<M>   insert subexpression M (in the range 1..N) matched by
                     the RE.  M may be multiple decimal digits, so use the
                     angle bracket form if digits follow the inserted group.
        \<name>
        $<name>      insert the named group if it was matched by the RE.
        \&  $&
        \0  $0       insert the entire text matched by the RE.
        \`  $`       insert the text preceding the match.
        \'  $'       insert the text after the match.
        \l...
        $l...        insert the numbered or named group matched by the RE,
                     converted to lower case.  For example, "\lM", "$l<M>",
                     "\l<name>", "$l&", and "\l`".
        $u...        insert the numbered or named group matched by the RE,
                     converted to upper case.  For example, "$uM", "\u<M>",
                     "$u<name>", "\u&", and "$u'".
        \c           Insert character 'c' (e.g., "\$" gives "$").


    Invocation:

        status = rexSubstitute (match, source, length, &result) ;

    where

        <match>		- I
            is a matched RE structure previously populated by a call to
            rexMatch(), so any capture groups in the matched text are
            fully defined.
        <source>	- I
            is a text string with embedded group references (see above).
        <length>	- I
            is the length of the text to add.  If this argument is -1,
            the text is assumed to be NUL-terminated and strlen(3) is
            used to determine its length.
        <result>	- O
            returns the result of substituting the matched capture groups
            in a copy of the source string.  RESULT returns a pointer to
            a dynamically-allocated string of the necessary length.  The
            calling program is responsible for FREE(2)ing the result string.
        <status>		- O
            returns the status of performing the substitution operation,
            zero if no errors occurred (whether or not any substitutions
            were made) and ERRNO otherwise (e.g., unable to MALLOC(3)
            space for the RESULT string).

*******************************************************************************/


errno_t  rexSubstitute (

#    if PROTOTYPES
        MatchedRE  match,
        const  char  *source,
        ssize_t  length,
        char  **result)
#    else
        match, source, length, result)

        MatchedRE  match ;
        const  char  *source ;
        ssize_t  length ;
        char  **result ;
#    endif

{    /* Local variables. */
    char  *endn, groupLeader, *r ;
    const  char  *bog, *eog, *eos, *groupStart, *s ;
    int  changeCase ;			/* -1=lower, 0=same, +1=upper. */
    long  lutNumber ;
    size_t  index, groupLength, preGroupLength ;
    StringDx  rdx ;




    if ((match == NULL) || (source == NULL) || (result == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(rexSubstitute) NULL match, source, or result.\n") ;
        return (errno) ;
    }

/* Determine the length of the source string.  Then create a pointer to the
   character after the last character in the string; i.e., "&source[length]".
   This end-of-string pointer will be used to tell when the scan and copy
   loop below has reached the end of the source string.  (Since the source
   may or may not be NUL-terminated, the code can't merely check for NUL.) */

    if (length < 0)  length = strlen (source) ;
    eos = &source[length] ;

/* Create a string descriptor for a dynamically growing result string. */

    if (sdxCreate (NULL, 0, SdxDynamic, &rdx)) {
        LGE "(rexSubstitute) Error creating result string descriptor.\nsdxCreate: ") ;
        return (errno) ;
    }

/* Copy the source string to the result, substituting each reference to a
   group with the capture group text from the matched RE. */

    for (s = source ;  s != eos ;  s++) {

        switch (*s) {

        case '\\':
        case '$':
            groupLeader = *s ;		/* Save '\' or '$'. */
            if (++s == eos)  s-- ;	/* Treat as literal at end-of-line. */
            switch (*s) {
            case 'l':			/* Convert to lower case? */
                s++ ;  changeCase = -1 ;  break;
            case 'u':			/* Convert to upper case? */
                s++ ;  changeCase = 1 ;  break;
            default:
                changeCase = 0 ;  break ;
            }
					/* Insert capture group. */
            if (isdigit (INT_UCHAR (*s))) {
                index = (size_t) strntoul (s, &endn, 10, eos - s) ;
                s = --endn ;		/* Skip past the number, but be
					   mindful of s++ in the for loop. */
                rexGetGroup (match, index, &groupStart, &groupLength) ;
                if ((groupStart == NULL) || (groupLength == 0))  break ;
                preGroupLength = sdxLength (rdx) ;
                sdxAddS (rdx, groupStart, groupLength) ;
                r = sdxStringZ (rdx) + preGroupLength ;
                if (changeCase < 0)
                    strnlwr (r, groupLength) ;
                else if (changeCase > 0)
                    strnupr (r, groupLength) ;
            }
					/* Insert named capture group. */
            else if (*s == '<') {
                bog = s + 1 ;		/* Beginning and end of group name. */
                eog = strnchr (bog, '>', eos - bog) ;
                if ((eog == NULL) ||	/* No closing angle bracket? */
                    (bog == eog)) {	/* Empty group name? */
                    sdxAddC (rdx, *s) ;	/* Add literal '<'. */
                    break ;
                }
                if (isdigit (INT_UCHAR (*bog))) {
                    index = (size_t) strntoul (bog, &endn, 10, eog - bog) ;
                    if (endn != eog) {	/* # didn't fill entire group name? */
					/* Pick an out-of-range index. */
                        index = match->pattern->numGroups + 1 ;
					/* Back up for the '<'. */
                        endn = (char *) s - 1 ;
                    }
                } else if (match->pattern->numGroupNames > 0) {
                    lutNumber = nnlNumberOf (match->pattern->groupNameLUT,
                                             bog, eog - bog, false, false) ;
                    if (lutNumber < 0) {	/* Invalid group name? */
					/* Pick an out-of-range index. */
                        index = match->pattern->numGroups + 1 ;
					/* Back up for the '<'. */
                        endn = (char *) s - 1 ;
                    } else {			/* Valid group name. */
					/* NOTE that LUT group # is direct
					   index, 0..N-1, into group array,
					   but rexGetGroup() expects 1..N. */
                        index = lutNumber + 1 ;
                        endn = (char *) eog ;
                    }
                } else {		/* Group name when no group names. */
					/* Pick an out-of-range index. */
                    index = match->pattern->numGroups + 1 ;
					/* Back up for the '<'. */
                    endn = (char *) s - 1 ;
                }
                s = endn ;		/* Skip past the closing angle bracket,
					   but be mindful of s++ in for loop. */
                rexGetGroup (match, index, &groupStart, &groupLength) ;
                if ((groupStart == NULL) || (groupLength == 0))  break ;
                preGroupLength = sdxLength (rdx) ;
                sdxAddS (rdx, groupStart, groupLength) ;
                r = sdxStringZ (rdx) + preGroupLength ;
                if (changeCase < 0)
                    strnlwr (r, groupLength) ;
                else if (changeCase > 0)
                    strnupr (r, groupLength) ;
            }
            					/* Insert entire match. */
            else if (*s == '&') {
                rexGetGroup (match, 0, &groupStart, &groupLength) ;
                if ((groupStart == NULL) || (groupLength == 0))  break ;
                preGroupLength = sdxLength (rdx) ;
                sdxAddS (rdx, groupStart, groupLength) ;
                r = sdxStringZ (rdx) + preGroupLength ;
                if (changeCase < 0)
                    strnlwr (r, groupLength) ;
                else if (changeCase > 0)
                    strnupr (r, groupLength) ;
            }

            else if (*s == '`') {
                groupStart = match->target ;
                groupLength = match->full.startp - match->target ;
                preGroupLength = sdxLength (rdx) ;
                sdxAddS (rdx, groupStart, groupLength) ;
                r = sdxStringZ (rdx) + preGroupLength ;
                if (changeCase < 0)
                    strnlwr (r, groupLength) ;
                else if (changeCase > 0)
                    strnupr (r, groupLength) ;
            }

            else if (*s == '\'') {
                groupStart = match->full.endp ;
                groupLength = strlen (groupStart) ;
                preGroupLength = sdxLength (rdx) ;
                sdxAddS (rdx, groupStart, groupLength) ;
                r = sdxStringZ (rdx) + preGroupLength ;
                if (changeCase < 0)
                    strnlwr (r, groupLength) ;
                else if (changeCase > 0)
                    strnupr (r, groupLength) ;
            }

            else if (groupLeader == '\\') {
                if (changeCase)  s-- ;	/* Back up to pick up "l" or "u". */
                sdxAddC (rdx, *s) ;
            }

            else if (groupLeader == '$') {
                sdxAddC (rdx, '$') ;
                if (changeCase)
                    s -= 2 ;		/* Back up to pick up "l" or "u". */
                else if (s != eos)
                    s-- ;		/* Back up to pick up string terminator. */
            }

            break ;

        default:
            sdxAddC (rdx, *s) ;		/* Arbitrary character. */
            break ;
        }

    }     /* Substitute capture groups in result string. */


    *result = sdxStringZ (rdx) ;
    if (*result == NULL) {
        *result = strdup ("") ;
        if (*result == NULL) {
            LGE "(rexSubstitute) Zero-length result; error duplicating \"\".\nstrdup: ") ;
            PUSH_ERRNO ;  sdxDestroy (rdx) ;  POP_ERRNO ;
            return (errno) ;
        }
    }
    sdxOwn (rdx) ;  sdxDestroy (rdx) ;


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    rexWild ()


Purpose:

    Function rexWild() converts a UNIX CSH(1)-style wildcard expression to
    the corresponding regular expression (RE).  The RE conversion handles
    the following cases:

        "^", RE's start-of-string symbol, is prepended to the RE to anchor the
            pattern match at the beginning of the target string.

        "*" in the wildcard expression is converted to ".*" in the RE.  "." is
            a special RE symbol that matches any character and "*" indicates
            zero or more occurrences of the preceding character, so ".*" matches
            zero or more characters in the target string.

        "?" in the wildcard expression is converted to "." in the RE.  "." is
            a special RE symbol that matches any single character.

        "." in the wildcard expression is converted to "\." (escaped dot) in
            the RE to disambiguate it from RE's special symbol.

        All other characters are passed through as is (meaning they must occur
            in the target file name).  NOTE that a shell character list/range
            enclosed in square brackets ("[...]") is passed through as is; the
            shell character set specification is compatible with the regular
            expression character set specification.

        "$", RE's end-of-string symbol, is appended to the RE so that execution
            of the RE must consume the entire target string.

    For example, the following wildcard expression:

                             "*.c.0*"

    (which matches strings like "fsearch.c.007") will be converted into:

                           "^.*\.c\.0.*$"


    Invocation:

        regularExpression = rexWild (wildcardExpression) ;

    where

        <wildcardExpression>	- I
            is the wildcard expression to be converted to a regular expression.
        <regularExpression>	- O
            returns the result of substituting the replacement text in the
            source string.  RESULT returns a pointer to a dynamically allocated
            string of the necessary length.  The calling program is responsible
            for FREE(3)ing the result string.
        <regularExpression>	- O
            returns the converted regular expression in a dynamically allocated
            string of the necessary length.  The calling program is responsible
            for FREE(3)ing this string after it is no longer needed.

*******************************************************************************/


char  *rexWild (

#    if PROTOTYPES
        const  char  *wildcard)
#    else
        wildcard)

        char  *wildcard ;
#    endif

{    /* Local variables. */
    char  buffer[2], *r, *s ;
    StringDx  rdx ;




    if (wildcard == NULL)  wildcard = "*" ;

    if (sdxCreate ("", 0, SdxVolatile, &rdx)) {
        LGE "(rexWild) Error creating result string descriptor.\nsdxCreate: ") ;
        return (NULL) ;
    }

/* Convert the wildcard to a regular expression. */

    if (sdxAddS (rdx, "^", -1)) {	/* Anchor match at start-of-line. */
        LGE "(rexWild) Error adding \"^\" to regular expression string.\nsdxAddS: ") ;
        PUSH_ERRNO ;  sdxDestroy (rdx) ;  POP_ERRNO ;
        return (NULL) ;
    }

    for (s = (char *) wildcard ;  *s != '\0' ;  r++, s++) {
        if (*s == '*')
            r = ".*" ;			/* Match 0 or more of any character. */
        else if (*s == '?')
            r = "." ;			/* Match any single character. */
        else if (*s == '.')
            r = "\\." ;			/* Match a period. */
        else if (*s == '+')
            r = "\\+" ;			/* Escape +-closure. */
        else if (*s == '(')
            r = "\\(" ;			/* Escape parentheses. */
        else if (*s == ')')
            r = "\\)" ;			/* Escape parentheses. */
        else if (*s == '{')
            r = "\\{" ;			/* Escape braces. */
        else if (*s == '}')
            r = "\\}" ;			/* Escape braces. */
        else if (*s == '$')
            r = "\\$" ;			/* Escape dollar sign. */
        else {
            buffer[0] = *s ;  buffer[1] = '\0' ;
            r = buffer ;		/* Match this character exactly. */
        }
        if (sdxAddS (rdx, r, -1)) {	/* Add to result. */
            LGE "(rexWild) Error adding \"%s\" to regular expression string.\nsdxAddS: ",
                r) ;
            PUSH_ERRNO ;  sdxDestroy (rdx) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

    if (sdxAddS (rdx, "$", -1)) {	/* Match must extend to end-of-line. */
        LGE "(rexWild) Error adding \"$\" to regular expression string.\nsdxAddS: ") ;
        PUSH_ERRNO ;  sdxDestroy (rdx) ;  POP_ERRNO ;
        return (NULL) ;
    }

    r = sdxStringZ (rdx) ;
    sdxOwn (rdx) ;  sdxDestroy (rdx) ;

    LGI "(rexWild) Wildcard: =>%s<=  RE: =>%s<=\n", wildcard, r) ;

    return (r) ;

}

/*!*****************************************************************************

Procedure:

    rexSearch ()


Purpose:

    Function rexSearch() tries to match a target string against a compiled
    regular expression by searching the graph that represents the regular
    expression.

    Two implementations of rexSearch() are provided below.  The recursive
    form of rexSearch() is the default.  If REX_UTIL.C is compiled with the
    "-DREX_UTIL_RECURSIVE_SEARCH=0" C Preprocessor flag, the iterative form
    is used instead.

    In the iterative form of rexSearch(), a stack is used to simulate the
    recursion.  Both the recursive and iterative forms optimize tail
    recursion by looping instead of doing a recursive/"recursive" call.

        NOTE: I have not measured, and thus cannot compare, the performance
        and memory consumption of the recursive and iterative versions of
        rexSearch().  In other words, I don't know which version is faster
        and which version uses less memory.  I can only say that the
        recursive version will (usually) use multiple entries on the actual
        call stack, whereas the iterative version uses a single entry on the
        actual call stack and manages a multiple entry, pseudo-call-stack
        on the heap.

        NOTE: The following paragraph about a now obsolete field in the
        CompiledRE structure explains the need for the ancillary search
        information argument to rexSearch().

            numClosures - is the number of quantified closures in the
                regular expression.  When attempting to match a regular
                expression with closures against a text string, rexSearch()
                counts the number of instances of each closure and compares
                the counts to the minimum and maximum limits of the closures.
                To keep the CompiledRE itself reentrant, an application must
                allocate and initialize a separate array of counts before
                calling rexSearch().  (There is another changing field in
                the state nodes: visited.  However, this field is only used
                by "private" functions in "rex_internals.c" which need to
                traverse the state graph while the RE is being compiled.
                After the RE is successfully compiled, the "visited" fields
                are no longer needed.)

        The number of closures is no longer used; rexSearch() allocates an
        array of counts for all of the states in the CompiledRE.  The counts
        for non-closure states are wasted space, but they save the necessity
        of implementing a mapping of states 0..N-1 to counts 0..M-1.

        It occurs to me that I should have just put the counts array in the
        match structure instead of adding an extra argument to rexSearch().
        Maybe I'll do that ... but that "ansi" argument name sounds so nice!


    Invocation:

        matched = rexSearch (pattern, state, target, length, first,
                             &next, match, ansi) ;

    where

        <pattern>	- I
            is the handle for the compiled pattern returned by rexCompile().
        <state>		- I
            is the start state of the regular expression that will be used
            to match the target string.  This argument is only used by the
            recursive version of rexSearch().
        <target>	- I
            is the string which will be matched against the regular expression.
        <length>	- I
            is the length of the target string.  If this argument is less than
            zero, the text is assumed to be NUL-terminated and strlen(3) is
            used to determine its length.
        <first>		- I
            is the offset of the character in TARGET with which rexSearch()
            begins its search for a match.
        <next>		- O
            returns, if the match was successful, the offset of the next
            character following the last character matched by the regular
            expression.
        <match>		- O
            is the handle for a match structure returned by rexCreate()
            with at least the number of capture groups/subexpressions
            specified in the regular expression.  The offsets and lengths
            of the capture groups encountered in this search are returned
            to the caller via this match structure; they can be retrieved
            with rexGetGroup().
        <ansi>		- I/O
            is a pointer to ancillary search information that rexSearch()
            uses and modifies during the search process.  This information
            used to be stored in the compiled pattern graph, but that meant
            that simultaneous searches using the same compiled RE could not
            be done.  The other REX_UTIL functions that call rexSearch()
            must pass in NULL for this argument.  With a NULL argument,
            rexSearch() will (i) know that it is the top-level invocation
            of itself, (ii) must allocate and initialize the ancillary
            search information, and (iii) will pass that information on
            to recursive calls of rexSearch().  When control returns back
            up to the top-level rexSearch() for the final time, this data
            will be deallocated before returning to the caller.
        <matched>	- O
            returns true if the target string matches the compiled RE and
            false if it is not matched.

*******************************************************************************/


static  bool  rexSearch (

#    if PROTOTYPES
        CompiledRE  pattern,
        StateIndex  state,
        const  char  *target,
        ssize_t  length,
        size_t  first,
        size_t  *next,
        MatchedRE  match,
        AncillarySearchInfo  ansi)
#    else
        pattern, state, target, length, first, next, match, ansi)

        CompiledRE  pattern ;
        StateIndex  state ;
        char  *target ;
        ssize_t  length ;
        size_t  first ;
        size_t  *next ;
        MatchedRE  match ;
        AncillarySearchInfo  ansi ;
#    endif

{    /* Local variables. */
    bool  allocated, matched = false ;
    size_t  count = 0, i ;
    StateNode  *slist ;

#if defined(REX_UTIL_RECURSIVE_SEARCH) && !REX_UTIL_RECURSIVE_SEARCH
    ArrayDx  stack ;
    bool  poppedFromStack ;
    int  step ;
    ssize_t  sp ;
    StackEntry  entry, *top ;
#endif



#define  INDENT  (match->depth * 2)

    LGI "%*sCALL {{\n", INDENT, "") ;

    match->depth++ ;

    if (length < 0)  length = strlen (target) ;


/*******************************************************************************
    If this is the top-level instance of rexSearch() (indicated by the
    ANSI argument being NULL), allocate and initialize the storage for
    the ancillary search information.  This information will be passed
    to recursive calls of rexSearch(); when control returns for the final
    time to this top-level instance of rexSearch(), the ancillary search
    information will be freed.
*******************************************************************************/

    allocated = (ansi == NULL) ;
    if (allocated) {
        ansi = calloc (pattern->numStates, sizeof (_AncillarySearchInfo)) ;
        if (ansi == NULL) {
            LGE "(rexSearch) Error allocating %"PRIuSIZE_T"-element information array.\ncalloc: ",
                pattern->numStates) ;
            return (false) ;
        }
        /* Closure counts already initialized by calloc(3). */
    }

#if !defined(REX_UTIL_RECURSIVE_SEARCH) || REX_UTIL_RECURSIVE_SEARCH


/*******************************************************************************

    Recursive Search Implementation
    -------------------------------

*******************************************************************************/


/*******************************************************************************
    Traverse the graph representation of the regular expressions, attempting
    to match matching character nodes to characters in the target string.
*******************************************************************************/

    *next = first ;
    slist = pattern->stateList ;
    if (state < 0)  LGI "%*sState = %"PRIdSSIZE_T"\n", INDENT, "", state) ;


    while (state >= 0) {

        int  match_char = slist[state].u.m.match_char ;

        if (slist[state].type == Closure) {
            LGI "%*sState = %"PRIdSSIZE_T"  %s  First = %"PRIuSIZE_T"  Count = %"PRIuSIZE_T"  \"%s\"\n",
                INDENT, "", state, rexNameOf (slist[state].type, true),
                first, ansi[state].count, target + first) ;
        } else {
            LGI "%*sState = %"PRIdSSIZE_T"  %s  First = %"PRIuSIZE_T"  \"%s\"\n",
                INDENT, "", state, rexNameOf (slist[state].type, true),
                first, target + first) ;
        }

        switch (slist[state].type) {

        case Empty:			/* Extend search on NEXT1 branch. */
            state = slist[state].next1 ;
            break ;

        case Anchor:			/* If anchored correctly, extend search on NEXT1 branch. */
            if (((match_char == '^') && (first == 0)) ||
                ((match_char == '$') && (first >= (size_t) length))) {
                state = slist[state].next1 ;
            } else {
                matched = false ;
                goto onReturn ;
            }
            break ;

        case Alternation:		/* Try one branch, then the other. */
            if (rexSearch (pattern, slist[state].next1, target, length,
                           first, next, match, ansi)) {
                matched = true ;
                goto onReturn ;
            } else {
                state = slist[state].next2 ;
            }
            break ;

        case Closure:
            count = ansi[state].count ;		/* Save current count from state. */
            if (ansi[state].count < slist[state].u.c.maxClosure) {
                ansi[state].count++ ;		/* Attempt closure on NEXT2 branch. */
                if (rexSearch (pattern, slist[state].next2, target, length,
                               first, next, match, ansi)) {
                    matched = true ;
                    goto onReturn ;
                }
                ansi[state].count = count ;	/* Restore state's count. */
                if (count < slist[state].u.c.minClosure) {
                    matched = false ;	/* Insufficient # of repetitions - terminate this branch of search. */
                    goto onReturn ;
                }
            } else {
                ansi[state].count = 0 ;		/* Maximum # of repetitions - extend search on NEXT1 branch. */
            }
		/* NEXT2 branch failed or maximum # of repetitions reached - extend search on NEXT1 branch. */
            ansi[state].count = 0 ;
            matched = rexSearch (pattern, slist[state].next1, target, length,
                                 first, next, match, ansi) ;
            if (!matched)
                ansi[state].count = count ;	/* Restore state's count and return failure. */
            goto onReturn ;

        case Final:
            *next = first ;		/* Success! */
            matched = true ;
            goto onReturn ;

        case Match:
            while (slist[state].type == Match) {
                if (first >= (size_t) length) {
                    matched = false ;	/* End of target string - return. */
                    goto onReturn ;
                }
                match_char = slist[state].u.m.match_char ;

                if (match_char == REX_MATCH_CHAR_ANY) {
                    *next = ++first ;	/* Character matches - follow NEXT1 branch. */
                    state = slist[state].next1 ;
                } else if (match_char == REX_MATCH_CHAR_ANY_BUT_NL) {
#ifdef QUESTION
    ********** Must check to ensure character is not an end-of-line character.
#endif
                    *next = ++first ;	/* Character matches - follow NEXT1 branch. */
                    state = slist[state].next1 ;
                } else if ((match_char == REX_MATCH_CHAR_CHARSET) &&
                           (CS_ISSET (target[first],
                                      slist[state].u.m.match_charset))) {
                    *next = ++first ;	/* Character matches - follow NEXT1 branch. */
                    state = slist[state].next1 ;
                } else if (match_char == (unsigned char) target[first]) {
                    *next = ++first ;	/* Character matches - follow NEXT1 branch. */
                    state = slist[state].next1 ;
                } else {
                    matched = false ;	/* Character doesn't match - return. */
                    goto onReturn ;
                }
            }
            break ;	/* Continue searching from non-MATCH state. */

        case LeftParen:
            i = slist[state].u.p.subexpIndex ;	/* Extend search on NEXT1 branch. */
            if (rexSearch (pattern, slist[state].next1, target, length,
                           first, next, match, ansi)) {
/*******************************************************************************
    A matched group's substring was originally specifed by its offset/length;
    a value of -1 in either meant that the group was not matched or that the
    match is not yet complete, respectively.  I originally saved the offset of
    the substring BEFORE the call to rexSearch() above; the recursive call(s)
    to rexSearch() would eventually encounter the RightParen and set the group
    length.

    However, when I reworked the REX_UTIL package, the group capture did not
    work in some cases and I couldn't figure out why.  Setting the group's
    offset AFTER the call to rexSearch(), per Henry Spencer's code, caused
    20 more failures in his test suite.  I finally switched to using start/end
    pointers like Henry Spencer did, setting the start pointer after the call
    to rexSearch(), and my code magically worked.  I'm still puzzled.
*******************************************************************************/
                if ((i > 0) &&				/* Numbered capture group? */
                    (match->group[i-1].startp == NULL)) {
							/* Save start of text. */
                    match->group[i-1].startp = target + first ;
                }
                matched = true ;
                goto onReturn ;
            } else {
                matched = false ;		/* Search failed. */
                goto onReturn ;
            }

        case RightParen:
            i = slist[state].u.p.subexpIndex ;	/* Extend search on NEXT1 branch. */
            if (rexSearch (pattern, slist[state].next1, target, length, first,
                           next, match, ansi)) {
                if ((i > 0) &&				/* Numbered capture group? */
                    (match->group[i-1].endp == NULL)) {	/* Save end of text. */
                    match->group[i-1].endp = target + first ;
                }
                matched = true ;
                goto onReturn ;
            } else {
                matched = false ;		/* Search failed. */
                goto onReturn ;
            }

        default:
            break ;

        }

    }


    /* Recursive search failed.  Proceed to the clean-up and return code
       below, AFTER THE NON-RECURSIVE VERSION OF REXSEARCH() IMMEDIATELY
       BELOW.  (The clean-up and return code is common to both versions
       of the function.) */

#else	/* NON-RECURSIVE IMPLEMENTATION */


/*******************************************************************************

    Non-Recursive Search Implementation
    -----------------------------------

    Simulate recursion using a manually managed call stack on the heap.

    For state types which involve processing before and after one or more
    "recursive" calls, this code structure:

        if (!poppedFromStack) {
            ... step 1 in processing this state ...
        } else if (poppedFromStack) {
            ... step 2..N in processing this state ..
        }

    is used rather than the seemingly simpler:

        if (poppedFromStack) {
            ... step 2..N in processing this state ...
        } else {
            ... step 1 in processing this state ..
        }

    to make the popped-from-stack condition more explicit and to more
    closely mirror the chronological structure of the recursive code
    for the state type.

*******************************************************************************/


/* Define preprocessor macros for pushing/popping entries to/from the
   heap-allocated call stack. */

/* Push local variables on call stack; result is true if push fails (e.g.,
   memory allocation error) and false if push succeeds. */

#    define  push(s, o, c, p) \
        (entry.state = s, entry.offset = o, entry.count = c, entry.step = p, \
         (adxAdd (stack, &entry, 1, -1) ? true : (sp++, false)))

/* Pop local variables from call stack; result is true if pop fails
   (i.e., stack is empty) and false if pop succeeds. */

#define  pop(s, o, c, p) \
    (count = adxCount (stack), \
     (count > 0) ? (top = (StackEntry *) adxGet (stack, count - 1), \
                    s = top->state, o = top->offset, \
                    c = top->count, p = top->step, \
                    adxDelete (stack, 1, -1), \
                    sp--, false) \
                 : true)


/* Allocate a stack for simulating the recursive graph search.  The maximum
   number of entries allocated for the stack is equal to the length of the
   target string plus the length of the longest path in the RE graph.  Match
   nodes and parentheses nodes do not consume stack space; only alternation
   and closure nodes do.  Consequently, the size of the run-time stack (i.e.,
   the number of alternation/closure nodes traversed) will be proportional
   to the number of characters in the target string and the sizing of the
   stack should be sufficient.  Keep your fingers crossed! */

    if (adxCreate (0, sizeof (StackEntry), NULL, 0, AdxDynamic, &stack)) {
        LGE "(rexSearch) Error allocating non-recursive call stack.\nadxCreate: ") ;
        PUSH_ERRNO ;  free (ansi) ;  POP_ERRNO ;
        return (false) ;
    }


/* Traverse the graph representation of the regular expressions, attempting to
   match matching character nodes to characters in the target string. */

    *next = first ;
    state = pattern->startState ;
    step = 0 ;  poppedFromStack = false ;  sp = 0 ;
    slist = pattern->stateList ;

    while (state >= 0) {

        int  match_char = slist[state].u.m.match_char ;

        LGI "(rexSearch)%*sState = %"PRI_STATEINDEX"  %s  First = %"PRIuSIZE_T"  Count = %"PRIuSIZE_T"  Step = %d  SP = %"PRIdSSIZE_T"%s\n",
            (int) (sp+1)*2, " ", state,
            rexNameOf (slist[state].type, true), first,
            (poppedFromStack ? count : ansi[state].count),
            step, sp, (poppedFromStack ? " *" : "")) ;

        switch (slist[state].type) {

        case Empty:
            state = slist[state].next1 ;		/* Extend search on NEXT1 branch. */
            break ;

        case Anchor:
            if (((match_char == '^') && (first == 0)) ||
                ((match_char == '$') && (target[first] == '\0')))
                state = slist[state].next1 ;		/* Extend search on NEXT1 path. */
            else
                state = -1 ;
            break ;

        case Alternation:
            if (poppedFromStack) {
                if (matched) {
                    state = -1 ;			/* No need to search further. */
                } else {
                    state = slist[state].next2 ;	/* Extend search on NEXT2 path. */
                }
            } else {
                LGI "(rexSearch) %*sPUSH (%"PRIdSSIZE_T"++) state = %"PRI_STATEINDEX"  offset = %"PRIuSIZE_T"  count = %"PRIuSIZE_T"  step = %d  [|]\n",
                    INDENT, "", sp, state, first, (size_t) 0, 1) ;
                if (push (state, first, 0, 1)) {
                    LGE "(rexSearch) Stack overflow on RE \"%s\", target \"%s\"\n",
                        pattern->expression, target) ;
                    goto onError ;
                }
                match->depth++ ;
                state = slist[state].next1 ;		/* Extend search on NEXT1 path. */
            }
            break ;

        case Closure:
            if (!poppedFromStack) {

			/* First visit to state (i.e., extending search
			   from previous state) or looping on closure
			   (via NEXT2 link). */

                count = ansi[state].count ;		/* Save current count from state. */
                if (ansi[state].count < slist[state].u.c.maxClosure) {
                    ansi[state].count++ ;  step = 2 ;
                    i = slist[state].next2 ;		/* Attempt closure on NEXT2 path. */
                } else {
                    ansi[state].count = 0 ;  step = 1 ;
                    i = slist[state].next1 ;		/* Maximum # of repetitions - extend search on NEXT1 path. */
                }
                LGI "(rexSearch) %*sPUSH (%"PRIdSSIZE_T"++) state = %"PRI_STATEINDEX"  offset = %"PRIuSIZE_T"  count = %"PRIuSIZE_T"  step = %d  [Cnot]\n",
                    INDENT, "", sp, state, first, count, step) ;
                if (push (state, first, count, step)) {
                    LGE "(rexSearch) Stack overflow on RE \"%s\", target \"%s\"\n",
                        pattern->expression, target) ;
                    goto onError ;
                }
                match->depth++ ;
                state = i ;

            } else if (poppedFromStack) {

			/* Return from search on state (NEXT1) following
			   closure; the search may have been successful
			   (matched) or not. */

                ansi[state].count = count ;		/* Restore state's count. */
                if (count < slist[state].u.c.minClosure) {
                    state = -1 ;			/* Insufficient # of repetitions - terminate this branch of search. */
                } else if (count < slist[state].u.c.maxClosure) {
                    if (!matched && (step == 2)) {
                        LGI "(rexSearch) %*sPUSH (%"PRIdSSIZE_T"++) state = %"PRI_STATEINDEX"  offset = %"PRIuSIZE_T"  count = %"PRIuSIZE_T"  step = %d  [Cp=2]\n",
                            INDENT, "", sp, state, first, count, 1) ;
                        if (push (state, first, count, 1)) {
                            LGE "(rexSearch) Stack overflow on RE \"%s\", target \"%s\"\n",
                                pattern->expression, target) ;
                            goto onError ;
                        }
                        match->depth++ ;
                        ansi[state].count = 0 ;
                        state = slist[state].next1 ;	/* NEXT2 branch failed - extend search on NEXT1 path. */
                    } else {
                        state = -1 ;			/* Matched or NEXT1 branch failed too. */
                    }
                } else {
                    state = -1 ;			/* Maximum # of repetitions - search on NEXT1 path failed. */
                }

            }
            break ;

        case Final:
            *next = first ;			/* Success! */
            matched = true ;
            state = -1 ;
            break ;

        case Match:
            match_char = slist[state].u.m.match_char ;
            if (first >= (size_t) length) {
                state = -1 ;				/* End of target string - return. */
            } else if (match_char == REX_MATCH_CHAR_ANY) {
                first++ ;
                state = slist[state].next1 ;		/* Character matches - follow NEXT1 branch. */
            } else if (match_char == REX_MATCH_CHAR_ANY_BUT_NL) {
                first++ ;
                state = slist[state].next1 ;		/* Character matches - follow NEXT1 branch. */
            } else if ((match_char == REX_MATCH_CHAR_CHARSET) &&
                       (CS_ISSET (target[first], slist[state].u.m.match_charset))) {
                first++ ;
                state = slist[state].next1 ;		/* Character matches - follow NEXT1 branch. */
            } else if (match_char == (unsigned char) target[first]) {
                first++ ;
                state = slist[state].next1 ;		/* Character matches - follow NEXT1 branch. */
            } else {
                state = -1 ;				/* Character doesn't match - return. */
            }
            break ;

        case LeftParen:
            if (!poppedFromStack) {
                LGI "(rexSearch) %*sPUSH (%"PRIdSSIZE_T"++) state = %"PRI_STATEINDEX"  offset = %"PRIuSIZE_T"  count = %"PRIuSIZE_T"  step = %d  [|]\n",
                    INDENT, "", sp, state, first, (size_t) 0, 1) ;
                if (push (state, first, 0, 1)) {
                    LGE "(rexSearch) Stack overflow on RE \"%s\", target \"%s\"\n",
                        pattern->expression, target) ;
                    goto onError ;
                }
                match->depth++ ;
                state = slist[state].next1 ;		/* Extend search on NEXT1 path. */
            } else if (poppedFromStack && matched) {
                i = slist[state].u.p.subexpIndex ;
                if ((i > 0) &&				/* Numbered capture group? */
                    (match->group[i-1].startp == NULL)) {	/* Save end of text. */
                    match->group[i-1].startp = target + first ;
                }
                state = -1 ;
            } else if (poppedFromStack && !matched) {
                state = -1 ;
            }
            break ;

        case RightParen:
            if (!poppedFromStack) {
                LGI "(rexSearch) %*sPUSH (%"PRIdSSIZE_T"++) state = %"PRI_STATEINDEX"  offset = %"PRIuSIZE_T"  count = %"PRIuSIZE_T"  step = %d  [|]\n",
                    INDENT, "", sp, state, first, (size_t) 0, 1) ;
                if (push (state, first, 0, 1)) {
                    LGE "(rexSearch) Stack overflow on RE \"%s\", target \"%s\"\n",
                        pattern->expression, target) ;
                    goto onError ;
                }
                match->depth++ ;
                state = slist[state].next1 ;		/* Extend search on NEXT1 path. */
            } else if (poppedFromStack && matched) {
                i = slist[state].u.p.subexpIndex ;
                if ((i > 0) &&				/* Numbered capture group? */
                    (match->group[i-1].endp == NULL)) {	/* Save end of text. */
                    match->group[i-1].endp = target + first ;
                }
                state = -1 ;
            } else if (poppedFromStack && !matched) {
                state = -1 ;
            }
            break ;

        default:
            break ;

        }

        poppedFromStack = false ;
        while (state < 0) {			/* "Return" from recursive calls. */
            LGI "(rexSearch) %*sPOP (--%"PRIdSSIZE_T") loop state = %"PRI_STATEINDEX"\n",
                INDENT, "", sp, state) ;
            if (pop (state, first, count, step))  break ;
            match->depth-- ;
            LGI "(rexSearch) %*sPOP (%"PRIdSSIZE_T") state = %"PRI_STATEINDEX"  offset = %"PRIuSIZE_T"  count = %"PRIuSIZE_T"  step = %d  [|]\n",
                INDENT, "", sp, state, first, count, step) ;
            poppedFromStack = true ;
        }

    }


/* The non-recursive search has completed successfully: without error, though
   with or without a match.  Go to the clean-up and return code. */

    goto onReturn ;

/*******************************************************************************
    On error or completion of the search attempt, free the ancillary search
    information.  This is common to both the recursive and non-recursive
    versions of rexSearch().
*******************************************************************************/

onError:

    matched = false ;		/* Not found or error. */

#endif  /* REX_UTIL_RECURSIVE_SEARCH */

onReturn:

    if (allocated) {		/* Top-level call to rexSearch()? */
        free (ansi) ;
    }

#if defined(REX_UTIL_RECURSIVE_SEARCH) && !REX_UTIL_RECURSIVE_SEARCH
    adxDestroy (stack) ;
#endif

    match->depth-- ;
    LGI "%*sRETURN }}  Matched = %s  First = %"PRIuSIZE_T"  Next = %"PRIuSIZE_T"  \"%s\"\n",
        INDENT, "", matched ? "true" : "false", first, *next, target + *next) ;

    return (matched) ;

}
