/* $Id: rex_internals.h,v 1.15 2023/05/24 12:27:41 alex Exp alex $ */
/*******************************************************************************

  rex_internals.h

  Regular Expression Utility Internal Definitions.

*******************************************************************************/


#ifndef  REX_INTERNALS_H	/* Has the file been INCLUDE'd already? */
#define  REX_INTERNALS_H  yes


#include  "nnl_util.h"		/* Name-to-Number Lookup utilities. */
#include  "sdx_util.h"		/* String Descriptor utilities. */
#include  "rex_util.h"		/* Regular expression utility definitions. */


/*******************************************************************************
    The following section defines macro's for manipulating character-set bit
    masks.  The macro definitions are modelled after the file-descriptor bit
    mask macro's (FD_SET, FD_CLR, etc.) defined in "<sys/types.h>" and used
    by the select(2) system call.
*******************************************************************************/

#ifndef NBBY
#    define  NBBY  8				/* # of bits in a byte. */
#endif

#define	 CS_SETSIZE  256

typedef	 int  cs_mask ;
#define	 NCSBITS  (sizeof (cs_mask) * NBBY)	/* # of bits per cs_mask. */
#ifndef	howmany
#    ifdef sun386
#        define  howmany(x,y)  ((((u_int)(x))+(((u_int)(y))-1))/((u_int)(y)))
#    else
#        define  howmany(x,y)  (((x)+((y)-1))/(y))
#    endif
#endif
#define  NCSMASK  howmany (CS_SETSIZE, NCSBITS)	/* # of masks per cs_set. */

typedef	 struct  cs_set {
    cs_mask  css_bits[howmany(CS_SETSIZE, NCSBITS)] ;
}  cs_set ;

/* In the 7 macros below, arguments p, etc. are all pointers to cs_set's. */

#define	 CS_SET(n,p)	((p)->css_bits[((unsigned char)(n))/NCSBITS] |= (1 << (((unsigned char)(n)) % NCSBITS)))
#define	 CS_CLR(n,p)	((p)->css_bits[((unsigned char)(n))/NCSBITS] &= ~(1 << (((unsigned char)(n)) % NCSBITS)))
#define	 CS_ISSET(n,p)	((p)->css_bits[((unsigned char)(n))/NCSBITS] & (1 << (((unsigned char)(n)) % NCSBITS)))

/* Clear all bits in the bit map. */
#define	 CS_ZERO(p)	memset ((void *) (p), '\0', sizeof (cs_set))

/* Set all bits in the bit map. */
#define	 CS_FILL(p)	memset ((void *) (p), 0xFF, sizeof (cs_set))

/* Complement all bits in the bit map. */
#define	 CS_COMP(p)	{ int _i ;					\
			  for (_i = 0 ;  _i < NCSMASK ;  _i++)		\
			    (p)->css_bits[_i] = ~((p)->css_bits[_i]) ; }

/* Copy one bit map to another. */
#define	 CS_COPY(p1,p2)	\
		memcpy ((void *) (p2), (const void *) (p1), sizeof (cs_set))

/* Bit-wise OR of one bit map with another. */
#define	 CS_OR(p1,p2)	{ int _i ;					\
			  for (_i = 0 ;  _i < NCSMASK ;  _i++)		\
			    (p2)->css_bits[_i] |= (p1)->css_bits[_i] ; }

/*******************************************************************************

    StateIndex
    ----------

    In a YACC source file, a semantic action can return a value to its parent
    grammar rule by assigning the value to YACC pseudo-variable $$.  The parent
    grammar rule can then reference its constituents' values using $1, $2, etc.
    For example, here's the rule for "{[m][,[n]]}", which specifies how many
    times a subexpression ("factor") can be repeated:

        factor:    factor  LEFT_BRACE  number  upper_bound  RIGHT_BRACE
                      { ... semantic action for "<factor>:" can reference:
                                $1 (factor)
                                $3 (number)
                                $4 (upper_bound)
                        ... create a new state node, store the number and
                            upper bound, and then return the index of the
                            new state in $$ ...
                      }

    Most of the regular expression parser's actions return the index in the
    state list of a newly created state node.  Numeric rules return integers.
    In the example above, $1 returned by "factor" is a state index, $3 is the
    integer value of "number", and $4 is the integer value of "upper_bound".

    The data type chosen for these values is "ssize_t", which can accommodate
    both state indices and the integer values allowed in regular expressions.
    The data type is conveyed to YACC by #define'ing YYSTYPE.  Since the
    Makefile runs YACC's output through SED(1) to convert all instances
    of "YY" and "yy" to "RR" and "rr", respectively, RRSTYPE is defined
    directly here.

*******************************************************************************/

    typedef  ssize_t  StateIndex ;	/* Index 0..N-1 into array of state
					   nodes.  -1 is a NULL index. */

#if !defined(HAVE_PERCENT_Z_FORMAT) || HAVE_PERCENT_Z_FORMAT
#    define  PRI_STATEINDEX  "zd"
#else
#    define  PRI_STATEINDEX  PRIdSSIZE_T
#endif

#define  RRSTYPE  StateIndex

#define  PRI_RRSTYPE  PRI_STATEINDEX

/*******************************************************************************

    Regular Expression Graph
    ------------------------

    A compiled regular expression is represented by a context node that
    contains a pointer to an array of state nodes:

                                .---------.
        Compiled Pattern -----> | Context |
                                |   Node  |     .---------.
                                |    *--------->| State 0 |
                                `---------'     | State 1 |
                                                |   ...   |
                                                |   ...   |
                                                | State N |
                                                `---------'

    Each state node contains two links to other states.  These "pointers"
    are simply the indices of other states in the state array; a null
    link is indicated by an index of -1.

    State nodes are of several types: ALTERNATION, ANCHOR, CLOSURE, EMPTY,
    FINAL, LEFT_PAREN, MATCH, and RIGHT_PAREN.  An RE that matches a single
    character (e.g., "a" or "[A-Za-z]") is represented by a single MATCH
    node:

                             ----> MATCH ---->

    A MATCH node for a range of characters ("[A-Za-z]" for example) contains
    a pointer to a 256-bit bit map in which bits corresponding to in-range
    characters are set.

    The various forms of closure ("RE*", "RE+", "RE?", and "RE{m,n}")
    are represented by a CLOSURE node that is linked to the RE's graph:

               ------------------------------.
                                             V
                   .-->  RE's graph ----> CLOSURE ---->
                   |                         |
                   `-------------------------'

    The CLOSURE node contains the lower and upper bounds (M and N) for
    the closure.

    Alternating expressions (e.g., "abc|def") are formed by creating an
    ALTERNATION node with links to the two REs; the two REs are, in turn,
    linked to an EMPTY node:

                    .----------> RE1's graph -----------.
                    |                                   V
         ----> ALTERNATION                            EMPTY ----->
                    |                                   ^
                    `----------> RE2's graph -----------'

    The LEFT_PAREN and RIGHT_PAREN nodes are used for subexpression
    assignments (e.g., "(RE)$n"):

         ----> LEFT_PAREN ----> RE's graph ----> RIGHT_PAREN ---->

    The LEFT_PAREN node contains the index I (1..N) of the numbered capture
    group to be assigned the subexpression's text.  (Unnumbered groups are
    indicated by an index of 0.)  The RIGHT_PAREN node has a "hidden" link
    back to the matching LEFT_PAREN.

    Anchored regular expressions ("^RE" and "RE$") are represented by ANCHOR
    nodes:

                  ----> ANCHOR(^) ----> RE's graph ---->
                  ----> RE's graph ----> ANCHOR($) ---->

    A successful match of a target string by a regular expression occurs
    when the FINAL state of the RE's graph is reached:

                                ----> FINAL

    State nodes have the following fields:

        TYPE - the type of the state.
        MATCH_CHAR - is the character to be matched:
                -3 = any character;
                -2 = any character but newline;
                -1 = use the MATCH_CHARSET bit mask;
            0..255 = the character to be matched.
        MATCH_CHARSET - if MATCH_CHAR equals -1, MATCH_CHARSET points to a
            dynamically-allocated character set bit mask in which the bits
            corresponding to characters that can be matched are set.
        NEXT1 - is the index of the next state, in the case of concatenated
            RE's or alternating RE's.  (A null link is indicated by an index
            of -1.)
        NEXT2 - is the index of the next state, in the case of backtracking
            closure RE's (RE* and RE+) and alternating RE's.  (A null link is
            indicated by an index of -1.)
        VISITED - indicates whether or not the state has been visited during
            a traversal.  This field is used by functions LONGEST_PATH and
            SHORTEST_PATH during the parsing of an RE.

    For closure nodes, certain fields have the following meanings:

        TYPE - is "closure".
        minClosure - is the minimum number of occurrences for the RE subject
            to the closure.
        maxClosure - is the maximum number of occurrences for the RE subject
            to the closure.

    For parentheses nodes, certain fields have the following meanings:

        TYPE - is "left_paren" or "right_paren".
        subexpIndex - for the left parenthesis, is the index (1..N) of the
            numbered capture group to be assigned the value of the matched
            text.  An index of 0 indicates an unnumbered capture group,
            "(?:...)", which are used simply for grouping purposes and
            cannot be referenced in substitution text.
        subexpState - for the right parenthesis, is the index of the state
            for the corresponding left parenthesis.

    For anchor nodes, certain fields have the following meanings:

        TYPE - is "anchor".
        MATCH_CHAR - is '^' or '$', depending on what type of anchor it is.

*******************************************************************************/


    typedef  enum  StateType {
        Empty, Alternation, Anchor, Closure,
        Final, LeftParen, Match, RightParen
    }  StateType ;

    typedef  struct  StateNode {
        StateType  type ;		/* (See member descriptions above.) */
        StateIndex  next1 ;		/* Next state or first alternate. */
        StateIndex  next2 ;		/* Closure start or second alternate. */
        bool  visited ;			/* Used during RE compilation. */
        union {
            struct  closures {
                size_t  minClosure ;
                size_t  maxClosure ;
            }  c ;
            struct  matches {
#                define  REX_MATCH_CHAR_ANY  -3
#                define  REX_MATCH_CHAR_ANY_BUT_NL  -2
#                define  REX_MATCH_CHAR_CHARSET  -1
                int  match_char ;
                cs_set  *match_charset ;
            }  m ;
            struct  parens {
                size_t  subexpIndex ;
                StateIndex  subexpState ;
            }  p ;
        }  u ;
    }  StateNode ;

/*******************************************************************************

    CompiledREs contain the context of a regular expression pattern.  The
    fields in a context node have the following meanings:

      Lexical Information Passed Back and Forth Between Parser and Lexer
      ------------------------------------------------------------------

        scan - is a pointer used by the lexer to scan PATTERN during
            the parse of the regular expression.

        afterBracket - controls the interpretation of characters within
            a character class specification (i.e., "[...]").  A value
            of zero indicates that the parser is not in the midst of a
            bracket expression.  Once the left bracket is encountered,
            rrparse() sets this variable to 1; as each character in the
            bracket expression is scanned, afterBracket is incremented.

        hyphenAsLiteral - controls the interpretation of hyphens within
            a bracketed character class expression.

        nameDelimiter - specifies the right (end) delimiter of a group name
            in the regular expression and indicates to the lexer that it
            should be in name-scan mode.  Normally, this character is '\0',
            meaning the lexer should not be in name-scan mode.  When the
            parser encounters the left delimiter of a name, it sets this
            variable to the right delimiter.  The lexer will then scan
            the name until it finds the right delimiter or an unallowed
            character in names.

        sdx - is a string descriptor used by the lexer to build a
            dynamically allocated string of characters when instructed
            to do so by the parser.  When the string is complete, the
            lexer will return a STRING token to the parser, which can
            then access this field to get the string.

      Regular Expression-Proper Information
      -------------------------------------

        expression - is the text of the regular expression that was compiled.

        errorText - is a dynamically allocated copy of the error message
            reported to rrerror() by the parser when it encounters an error.
            This is usually not of great use.

        bolAnchor - indicates if the start state of the regular expression
            is a beginning-of-line anchor.  If this flag is TRUE, rexMatch()
            only needs to call rexSearch() once, at the beginning of the
            target string; it does NOT need to shift the starting point for
            rexSearch() through each character in the target string

        numGroups - is the number of parenthesized capture groups in the
            regular expression.  Application programs can retrieve this
            value by calling rexNumGroups().  NOTE that the count includes
            named groups since all left parenthesises are automatically
            assigned incrementing group indices; applications can look up
            a name's group index using the group name lookup table.

        maxGroupNames - is the maximum number of allocated entries in the
            compiled RE's group names lookup table.  The parser increments
            this in chunks as needed as the regular expression is parsed.

        numGroupNames - is the actual number of entries used in the group
            names lookup table, not including the list-terminating NULL
            entry.

        groupNameLUT - is a name/number lookup table that maps capture
            group names to their parenthesized group index.

        buildSet - is a 256-bit bit map in which each bit corresponds to
            a character (0-255).  The parser uses this field to build the
            set of characters matched by a bracketed character class
            definition; e.g., "[A-Z0-9]" will result in a bit map with
            36 bits set for the allowed capital letters and numbers.
            If the complement character follows the left bracket, the
            parser performs a bit-wise complement of the entire set;
            e.g., "[^A-Z0-9]" will result in 220 bits being set (all
            the characters except the 36 capital letters and numbers).

        firstSet - is a 256-bit bit map in which each bit corresponds to
            a character (0-255).  Upon completely parsing the regular
            expression and building the state graph, the parser explores
            the state graph to determine the set of characters that may
            appear as the first character in a target string matched by
            the regular expression.  For example, RE "abc" has only one
            "first character": 'a'; on the other hand, "[[:alnum:]_]"
            has 63 possible first characters: 26 lower-case letters,
            26 upper-case letters, 10 digits, and underscore.  This
            information is used by rexMatch() to skip calls to
            rexSearch() when the first character in a target string
            admits no possibility of a match.

        maxStates - is the maximum number of allocated state entries in the
            compiled RE's state list.  The parser increments this in chunks
            as needed as the regular expression is parsed.

        numStates - is the actual number of state entries in the compiled
            RE's state list.

        startState - is the index in the state list of the initial state of
            the regular expression's graph.

        stateList - points to a dynamically-allocated array of state nodes.

*******************************************************************************/

/* Used to communicate information between the parser and lexer. */

    typedef  struct  LexInfo {
        char  *scan ;			/* (See member descriptions above.) */
        StringDx  insert ;
        size_t  insertOffset ;
        int  afterBracket ;
        bool  hyphenAsLiteral ;
        char  nameDelimiter ;		/* '\0' or right delimiter. */
        StringDx  sdx ;
    }  LexInfo ;

    typedef  struct  _CompiledRE {
        char  *expression ;		/* (See member descriptions above.) */
        char  *errorText ;
        LexInfo  lex ;
        bool  bolAnchor ;
        size_t  numGroups ;
        size_t  maxGroupNames ;
        size_t  numGroupNames ;
        NNLMap  *groupNameLUT ;
        cs_set  buildSet ;
        cs_set  firstSet ;
        size_t  maxStates ;
        size_t  numStates ;
        StateIndex  startState ;
        StateNode  *stateList ;		/* Pointer to array of state nodes. */
    }  _CompiledRE ;

					/* Regular expression being parsed. */
    extern  CompiledRE  rtx  OCD ("rex_util") ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

/* REX_UTIL.C */

extern  errno_t  rexDumpClass P_((FILE *outfile,
                                  const char *leadIn,
                                  cs_set *charSet))
    OCD ("rex_util") ;

/* REX_UTIL_Y.Y */

extern  int  rrparse P_((CompiledRE rtx))
    OCD ("rex_util") ;

/* REX_INTERNALS.C */

extern  errno_t  rexAddGroupName P_((CompiledRE pattern,
                                     char *name,
                                     ssize_t length,
                                     size_t  group))
    OCD ("rex_util") ;

extern  errno_t  rexAllocateState P_((CompiledRE pattern))
    OCD ("rex_util") ;

extern  bool  rexFirstChar P_((CompiledRE pattern,
                               StateIndex state,
                               bool isStart))
    OCD ("rex_util") ;

extern  StateIndex  rexLastState P_((CompiledRE pattern,
                                     StateIndex state))
    OCD ("rex_util") ;

extern  size_t  rexLongestPath P_((CompiledRE pattern,
                                   StateIndex state,
                                   bool isStart))
    OCD ("rex_util") ;

extern  const  char  *rexNameOf P_((StateType type,
                                    bool upper))
    OCD ("rex_util") ;

extern  size_t  rexShortestPath P_((CompiledRE pattern,
                                    StateIndex state,
                                    bool isStart))
    OCD ("rex_util") ;


#endif				/* If this file was not INCLUDE'd previously. */
