%{
/* $Id: rex_util_y.y,v 1.24 2023/05/24 12:27:41 alex Exp alex $ */
/*******************************************************************************

Procedure:

    rrparse ()


Author:    Alex Measday


Purpose:

    Function rrparse() is a Bison-generated routine that parses a regular
    expression (RE) and builds a non-deterministic finite state machine
    that will recognize the RE.

    The grammar for regular expressions is derived from the grammar presented
    by Robert Sedgewick in his book, ALGORITHMS (Chapter 21: Parsing).  This
    was done back in 1989 or 1990, so I used YACC to build the parser.  An
    interpreter I was writing at the time included a YACC parser for an
    interpreted language and this YACC parser for run-time compilation and
    application of regular expressions in the language.  I let the language
    parser use the standard yy/YY prefixes, but I had the LIBGPL Makefile
    run a sed(1) command to replace all yy/YY prefixes in the generated
    regular expression parser with rr/RR prefixes.

    I continued to use YACC (or Bison in YACC-compatibility mode) for
    the regular expression parser for the next 30 years.  I never had
    any problems with it, but there's always been two nagging concerns:
    (i) that YACC's standalone internals might get messed up or in an
    unstable state if an error occurs and (ii) how that might affect
    the parsing of the next regular expression.

    One important change that was needed to rrparse() and rrlex() was
    to make them pure reentrant functions, in Bison's terminology and
    now in its capabilities.


    Invocation:

        status = rrparse (compiledRE) ;

    where

        <compiledRE)	- I/O
            is the context for the regular expression being compiled.
            The context includes the regular expression string, which
            the lexer will scan.  The finite state machine built by
            the parser is added to the context.
        <status>	- O
            returns the status of parsing the input, zero if no errors occurred
            and a non-zero value otherwise.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <ctype.h>			/* Character classification macros. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* Standard C string functions. */
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */
#include  "sto_util.h"			/* String-to-number functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */
#include  "rex_util.h"			/* Regular expression definitions. */
#include  "rex_internals.h"		/* Internal definitions. */

#define  NEW_STATE(rtx) \
  { if (rexAllocateState (rtx)) { \
        yyerror (rtx, "error increasing size of state list") ; \
        YYABORT ; \
    } \
  }

#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  rex_util_debug

#define  display_production(left, right) \
    LGI "%s ::== %s\n", left, right) ;

#define  display_token(name, text)	\
    { if (isprint (INT_UCHAR ((text)))) { LGI "-- Token %s = \"%c\"\n", name, text) ; } else { LGI "-- Token %s = \".\" (0x%02X)\n", name, INT_UCHAR ((text))) ; } }

#define  unput(rtx, c)			\
	(((rtx->lex.scan == rtx->expression) || (c == '\0'))	\
	? -1 : (rtx->lex.scan--, 0))

/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

/* Character classification names.  The ordering of these names is important
   to YYPARSE; see the "character_class => CTYPE" rule before making any
   changes. */

    static  char  *class_name[] = {
        "alnum",
        "alpha",
        "ascii",
        "blank",
        "cntrl",
        "digit",
        "graph",
        "lower",
        "print",
        "punct",
        "space",
        "upper",
        "xdigit",
        "word",
        NULL
    } ;

/*******************************************************************************
    Private Functions
*******************************************************************************/

static  int  yyerror (
#    if PROTOTYPES
        CompiledRE  rtx,
        const  char  *s
#    endif
    )  OCD ("rex_util") ;

static  int  yylex (
#    if PROTOTYPES
        YYSTYPE  *lvalp,
        CompiledRE  rtx
#    endif
    )  OCD ("rex_util") ;

static  int  rrinput (
#    if PROTOTYPES
        CompiledRE  rtx
#    endif
    )  OCD ("rex_util") ;

static  errno_t  rrInsert (
#    if PROTOTYPES
        CompiledRE  rtx,
        bool  erase,
        const  char  *text,
        ssize_t  length
#    endif
    )  OCD ("rex_util") ;

#ifdef NOT_USED_YET

static  void  rrunput (
#    if PROTOTYPES
        CompiledRE  rtx,
        int  c
#    endif
    )  OCD ("rex_util") ;

static  int  rexTokenByName (
#    if PROTOTYPES
        const  char  *name
#    endif
    )  OCD ("rex_util") ;

static  const  char  *rexTokenByNumber (
#    if PROTOTYPES
        int  number
#    endif
    )  OCD ("rex_util") ;

#endif	/* NOT_USED_YET */

%}

/*******************************************************************************
    Bison Declarations
*******************************************************************************/

%define  api.prefix  {rr}
%define  api.pure  full
%define  api.value.type  {StateIndex}

%lex-param  {CompiledRE rtx}
%parse-param  {CompiledRE rtx}

/* Token definitions. */

%token  ASCII_NUL
%token  ASTERISK
%token  CARET
%token  COLON
%token  COMMA
%token  CTYPE
%token  DIGIT
%token  DOLLAR_SIGN
%token  _ERROR
%token  HYPHEN
%token  LEFT_ANGLE
%token  LEFT_BRACE
%token  LEFT_BRACKET
%token  LEFT_PAREN
%token  PERIOD
%token  PLUS
%token  RIGHT_ANGLE
%token  RIGHT_BRACE
%token  RIGHT_BRACKET
%token  RIGHT_PAREN
%token  QUESTION_MARK
%token  SINGLE_CHARACTER
%token  STRING
%token  VERTICAL_BAR

/* Operator precedence. */

%left  VERTICAL_BAR
%right  PLUS  ASTERISK  QUESTION_MARK

/* Start symbol of the regular expression grammar. */

%start  complete_re


%%

/*******************************************************************************
  Regular Expression Grammar
*******************************************************************************/


complete_re :
        regular_expression
          { display_production ("complete_re", "regular_expression") ;
            /* The initial state equals the first state in RE.  Add a final
               state and link the last state in RE to the final state. */
            rtx->startState = $1 ;
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, $1)].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Final ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            rtx->numStates++ ;
            /* Compute the set of first characters that may appear at the
               beginning of a target string matched by this RE.  This set
               allows rexMatch() to avoid calling rexSearch() when the first
               character of the target string is not in the set of first
               characters. */
            CS_ZERO (&rtx->firstSet) ;
            rexFirstChar (rtx, rtx->startState, true) ;
          }
    ;


regular_expression :
        /* empty */
          { display_production ("regular_expression", "<empty>") ;
            /* Add an EMPTY state. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Empty ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   term
          { display_production ("regular_expression", "term") ;  $$ = $1 ; }
    |   regular_expression  VERTICAL_BAR  regular_expression
          { display_production ("regular_expression", "regular_expression | regular_expression") ;
            /* Add an alternation state E1 and an empty state E2.  Link E1 to
               the first state in RE1 and the first state in RE2.  Link the
               last state in RE1 to E2; do the same for the last state in RE2.
               (The first state in "RE|RE" is E1.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Alternation ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = $1 ;
            rtx->stateList[rtx->numStates].next2 = $3 ;
            $$ = rtx->numStates++ ;
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, $1)].next1 = rtx->numStates ;
            rtx->stateList[rexLastState (rtx, $3)].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Empty ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            rtx->numStates++ ;
          }
    ;


term :
        factor
          { display_production ("term", "factor") ;  $$ = $1 ; }
    |   factor  term
          { display_production ("term", "factor term") ;
            /* Link the last state in RE1 to the first state in RE2.  (The
               first state in "RE1 RE2" is the first state in RE1.) */
            rtx->stateList[rexLastState (rtx, $1)].next1 = $2 ;
            $$ = $1 ;
          }
    ;


factor :
        CARET
          { display_production ("factor", "^") ;
            /* Add an anchor state and set its match character to "^". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Anchor ;
            rtx->stateList[rtx->numStates].u.m.match_char = '^' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   DOLLAR_SIGN
          { display_production ("factor", "$") ;
            /* Add an anchor state and set its match character to "$". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Anchor ;
            rtx->stateList[rtx->numStates].u.m.match_char = '$' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   factor  LEFT_BRACE  number  upper_bound  RIGHT_BRACE
          { size_t  length ;
            display_production ("factor", "factor{[min][,[max]]}") ;
            /* Check for nested closures that might cause the matching
               algorithm to endlessly loop without consuming any input
               from the target string; e.g., "(a*){0,10}".  Actually,
               the maximum field puts a cap on the number of loops, but
               we'll disallow it anyway. */
            length = rexShortestPath (rtx, $1, true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                $1, length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor { m, n }") ;
                YYABORT ;
            }
            /* Add a closure state and set its minimum and maximum fields.
               Link the last state in RE to the closure state; link the closure
               state backwards to the first state in RE.  (The first state in
               "RE{min,max}" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, $1)].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            if ($3 < 0)  $3 = 0 ;
            rtx->stateList[rtx->numStates].u.c.minClosure = $3 ;
            if ($4 < 0)  $4 = $3 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = $4 ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = $1 ;
            $$ = rtx->numStates++ ;
          }
    |   LEFT_PAREN
          {
            NEW_STATE (rtx) ;
            $$ = rtx->numStates++ ;
            rtx->stateList[$$].type = LeftParen ;	/* P1 */
            rtx->stateList[$$].u.p.subexpIndex = rtx->numGroups++ + 1 ;
            rtx->stateList[$$].u.p.subexpState = -1 ;	/* Not known yet. */
            rtx->stateList[$$].next1 = -1 ;		/* Not known yet. */
            rtx->stateList[$$].next2 = -1 ;
          }
        preamble
          {
            $$ = $2 ;
          }
        regular_expression  RIGHT_PAREN
          { display_production ("factor", "(regular_expression)") ;
		/* Add two parenthesis states, P1 and P2.  Link P1 to the first
		   state in RE; link the last state in RE to P2.  The group
		   index (left parenthesis count) was already stored in P1. */
            NEW_STATE (rtx) ;
            $$ = rtx->numStates++ ;			/* Use $$ as shorthand. */
            rtx->stateList[rexLastState (rtx, $5)].next1 = $$ ;
            rtx->stateList[$$].type = RightParen ;	/* P2 */
            rtx->stateList[$$].u.p.subexpIndex =
                rtx->stateList[$2].u.p.subexpIndex ;	/* Index from P1. */
            rtx->stateList[$$].u.p.subexpState = $2 ;
            rtx->stateList[$$].next1 = -1 ;
            rtx->stateList[$$].next2 = -1 ;
            rtx->stateList[$2].u.p.subexpState = $$ ;	/* Back-fill P1. */
            rtx->stateList[$2].next1 = $5 ;
            rtx->stateList[$2].next2 = -1 ;
		/* NOTE that the result of this rule is P1, the "LeftParen"
		   state, not the "RightParen" state, so reassign $$ to $2. */
            $$ = $2 ;
          }
    |   factor  ASTERISK
          { size_t  length ;
            display_production ("factor", "factor*") ;
            /* Check for nested closures that might cause the matching
               algorithm to endlessly loop without consuming any input
               from the target string; e.g., "(a*)*". */
            length = rexShortestPath (rtx, $1, true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                $1, length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor*") ;
                YYABORT ;
            }
            /* Add a closure state.  Link the last state in RE to the closure
               state; link the closure state backwards to the first state in
               RE.  (The first state in "RE*" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, $1)].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            rtx->stateList[rtx->numStates].u.c.minClosure = 0 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = SIZE_MAX ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = $1 ;
            $$ = rtx->numStates++ ;
          }
    |   factor  PLUS
          { size_t  length ;
            display_production ("factor", "factor+") ;
            /* Check for nested closures that might cause the matching
               algorithm to endlessly loop without consuming any input
               from the target string; e.g., "(a*)+". */
            length = rexShortestPath (rtx, $1, true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                $1, length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor+") ;
                YYABORT ;
            }
            /* Add a closure state.  Link the last state in RE to the closure
               state; link the closure state backwards to the first state in
               RE.  (The first state in "RE+" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, $1)].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            rtx->stateList[rtx->numStates].u.c.minClosure = 1 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = SIZE_MAX ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = $1 ;
            $$ = rtx->numStates++ ;
          }
    |   factor  QUESTION_MARK
          { size_t  length ;
            display_production ("factor", "factor?") ;
            /* Check for nested closures.  Even though nested closures are
               not a major threat in a zero-or-one closure, we check anyway,
               just to be consistent with the other forms of closure. */
            length = rexShortestPath (rtx, $1, true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                $1, length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor?") ;
                YYABORT ;
            }
            /* Add a closure state.  Link the last state in RE to the closure
               state; link the closure state backwards to the first state in
               RE.  (The first state in "RE?" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, $1)].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            rtx->stateList[rtx->numStates].u.c.minClosure = 0 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = 1 ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = $1 ;
            $$ = rtx->numStates++ ;
          }
    |   LEFT_BRACKET
          { CS_ZERO (&rtx->buildSet) ;
            rtx->lex.afterBracket = 1 ;
            rtx->lex.hyphenAsLiteral = false ;
          }
        complement  character_classes  RIGHT_BRACKET
          { display_production ("factor", "[character_classes]") ;
            rtx->lex.afterBracket = 0 ;
            if ($3)  CS_COMP (&rtx->buildSet) ;
            LGI "Character Class:\n") ;
            if (I_DEFAULT_GUARD) {
                rexDumpClass (stdout, "    Matches: ", &rtx->buildSet) ;
            }
            /* Add a multi-character state for the character class. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char =
                REX_MATCH_CHAR_CHARSET ;
            rtx->stateList[rtx->numStates].u.m.match_charset =
                (cs_set *) malloc (sizeof (cs_set)) ;
            if (rtx->stateList[rtx->numStates].u.m.match_charset == NULL) {
                yyerror (rtx, "error allocating character class set") ;
                YYABORT ;
            }
            CS_COPY (&rtx->buildSet,
                     rtx->stateList[rtx->numStates].u.m.match_charset) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   match_character
          { display_production ("factor", "match_character") ;  $$ = $1 ; }
    ;


preamble :
        /* empty */
          { display_production ("preamble", "<empty>") ;  $$ = -1 ; }
    |   QUESTION_MARK  preamble_body
          { display_production ("preamble", "?<preamble_body>") ;  $$ = $2 ; }
    ;


preamble_body :
        COLON		/* Remove numbering from non-capture groups. */
          { display_production ("preamble_body", ":") ;
            rtx->stateList[rtx->numStates-1].u.p.subexpIndex = 0 ;
            rtx->numGroups-- ;
            $$ = -1 ;
          }
    |   LEFT_ANGLE
          { sdxErase (rtx->lex.sdx) ;
            rtx->lex.nameDelimiter = '>' ;
          }
        STRING
          { rtx->lex.nameDelimiter = '\0' ;
            if ($3 < 0) {
                yyerror (rtx, "empty or invalid group name") ;
                YYABORT ;
            }
			/* Note that the number of states was incremented
			   when a state was added for the beginning of the
			   group (LEFT_PAREN).  Consequently, that state has
			   the array index in the state list of the number
			   of states minus one.  (In other words, the number
			   of states is 1..N-based while the state list is
			   0..N-1-based.) */
            if (rexAddGroupName (rtx, sdxStringZ (rtx->lex.sdx), -1,
                        rtx->stateList[rtx->numStates-1].u.p.subexpIndex)) {
                yyerror (rtx, "error adding group name") ;
                YYABORT ;
            }
            $$ = -1 ;
          }
        RIGHT_ANGLE
    ;


complement :
        /* empty */
          { display_production ("complement", "<empty>") ;  $$ = 0 ; }
    |   CARET
          { display_production ("complement", "^") ;  $$ = -1 ; }
    ;


number :
        /* empty */
          { display_production ("number", "<empty>") ;  $$ = -1 ; }
    |   digits
          { display_production ("number", "digits") ;  $$ = $1 ; }
    ;


upper_bound :
        /* empty */
          { display_production ("upper_bound", "<empty>") ;
            $$ = -1 ;			/* Upper bound = lower bound. */
          }
    |   COMMA  number
          { display_production ("lower_bound", "digits") ;
            if ($2 < 0)
                $$ = SSIZE_MAX ;	/* Upper bound = infinity. */
            else
                $$ = $2 ;		/* Normal upper bound. */
          }
    ;


digits :
        DIGIT
          { display_production ("digits", "DIGIT") ;
            $$ = $1 - '0' ;
          }
    |   digits  DIGIT
          { display_production ("digits", "digits DIGIT") ;
            $$ = ($1 * 10) + ($2 - '0') ;
          }
    ;


character_classes :
        character_class
          { display_production ("character_classes", "character_class") ; }
    |   character_classes  character_class
          { display_production ("character_classes", "character_classes character_class") ; }
    ;


character_class :
        character
          { display_production ("character_class", "character") ;
            CS_SET ($1, &rtx->buildSet) ;
          }
    |   character  HYPHEN
          { rtx->lex.hyphenAsLiteral = true ; }
        character
          { size_t  i ;
            display_production ("character_class", "character-character") ;
            rtx->lex.hyphenAsLiteral = false ;
            if ($1 > $4) {
                LGI "(yyparse) Start character exceeds end character in character class range \"%c-%c\".\n",
                    (int) $1, (int) $4) ;
                yyerror (rtx, "inverted character class range") ;
                YYABORT ;
            }
            for (i = (size_t) $1 ;  i <= (size_t) $4 ;  i++)
                CS_SET (i, &rtx->buildSet) ;
          }
    |   CTYPE
          { size_t  i ;
            display_production ("character_class", "[:<CTYPE>:]") ;
            for (i = 0 ;  i < CS_SETSIZE ;  i++) {
                switch ($1) {
                case  0:
                    if (isalnum (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  1:
                    if (isalpha (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  2:
                    if (isascii (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  3:
                    if (isblank (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  4:
                    if (iscntrl (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  5:
                    if (isdigit (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  6:
                    if (isgraph (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  7:
                    if (islower (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  8:
                    if (isprint (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case  9:
                    if (ispunct (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case 10:
                    if (isspace (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case 11:
                    if (isupper (INT_UCHAR (i)))  CS_SET (i, &rtx->buildSet) ;
                    break ;
                case 12:
                    if (isxdigit (INT_UCHAR (i))) CS_SET (i, &rtx->buildSet) ;
                    break ;
                case 13:			/* [[:word:]] == [A-Za-z0-9_] */
                    if (isalnum (INT_UCHAR (i)) || (i == '_'))
                        CS_SET (i, &rtx->buildSet) ;
                    break ;
                default:
                    break ;
                }
            }
          }
    ;


character :
        SINGLE_CHARACTER
          { display_production ("character", "SINGLE_CHARACTER") ;  $$ = $1 ; }
    ;


match_character :
        SINGLE_CHARACTER
          { display_production ("match_character", "SINGLE_CHARACTER") ;
            /* Add a one-character state for the character. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   COLON
          { display_production ("match_character", "COLON") ;
            /* Add a one-character state for ":". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   COMMA
          { display_production ("match_character", "COMMA") ;
            /* Add a one-character state for ",". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   DIGIT
          { display_production ("match_character", "DIGIT") ;
            /* Add a one-character state for the digit. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   LEFT_ANGLE
          { display_production ("match_character", "LEFT_ANGLE") ;
            /* Add a one-character state for "<". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   PERIOD
          { display_production ("match_character", "PERIOD") ;
            /* Add a one-character state for any-character-matches. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = REX_MATCH_CHAR_ANY_BUT_NL ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   RIGHT_ANGLE
          { display_production ("match_character", "RIGHT_ANGLE") ;
            /* Add a one-character state for ">". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   RIGHT_BRACE
          { display_production ("match_character", "RIGHT_BRACE") ;
            /* Add a one-character state for "}". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    |   RIGHT_BRACKET
          { display_production ("match_character", "RIGHT_BRACKET") ;
            /* Add a one-character state for "]". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR ($1) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            $$ = rtx->numStates++ ;
          }
    ;


%%

/*!*****************************************************************************
    Function YYERROR is invoked automatically by YYPARSE when an error is
    detected.  YYERROR simply prints out the error message passed to it by
    YYPARSE.
*******************************************************************************/

static  int  yyerror (

#    if PROTOTYPES
        CompiledRE  rtx,
        const  char  *s)
#    else
        rtx, s)

        CompiledRE  rtx ;
        const  char  *s ;
#    endif

{

/* Make a dynamic copy of the error text.  It was not clear (to me) where
   the Bison-generated code was storing the text, so I don't know the
   lifetime of the Bison-supplied error text.  It does build the messages
   on the fly, so the text is in a changeable memory buffer somewhere. */

    if (rtx->errorText != NULL)  free (rtx->errorText) ;
    rtx->errorText = strdup (s) ;
    if (rtx->errorText == NULL) {
        LGE "(rrerror) Error duplicating message, \"%s\"\nstrdup: ", s) ;
        return (errno) ;
    }

    SET_ERRNO (EINVAL) ;
    LGE "(rrerror) %s\n", rtx->errorText) ;
    return (errno) ;

}

/*!*****************************************************************************
    Function RRLEX returns the next token of input.  This function is normally
    generated by F/LEX, but, for this parser, it's simple enough to do by hand.
*******************************************************************************/


static  int  yylex (

#    if PROTOTYPES
        YYSTYPE  *lvalp,
        CompiledRE  rtx)
#    else
        lvalp, rtx)

        YYSTYPE  *lvalp ;
        CompiledRE  rtx ;
#    endif

{    /* Local variables. */
    char  buffer[12], *endn ;
    size_t  length ;
    UniCodePoint  codePoint ;
    unsigned  char  c ;
    unsigned  int  i ;




/* At the beginning of input, reset the lexical flags. */

    if (rtx->lex.scan == rtx->expression) {	/* Reset lexical flags. */
        rtx->lex.afterBracket = 0 ;
        rtx->lex.hyphenAsLiteral = false ;
    }

/* Get the next character. */

    c = (unsigned char) rrinput (rtx) ;  *lvalp = c ;

/* At the end of the string, return an end marker. */

    if (c == '\0') {
        display_token ("END_MARKER", '0') ;  return (0) ;
    }

/* If a capture group name is expected, then extract it from the input. */

    if (rtx->lex.nameDelimiter) {

        unput (rtx, c) ;	/* Back up to first character of name. */

#ifndef GROUP_NAME_CHAR_SET
#    define  GROUP_NAME_CHAR_SET	\
	"-._ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
#endif

        length = strspn (rtx->lex.scan, GROUP_NAME_CHAR_SET) ;
        if ((length > 0) && (rtx->lex.scan[length] == rtx->lex.nameDelimiter)) {
            if (sdxAddS (rtx->lex.sdx, rtx->lex.scan, length)) {
                LGE "(yylex) Error duplicating group name \"%*s\".\nsdxAddS: ",
                    (int) length, rtx->lex.scan) ;
                display_token ("_ERROR", c) ;  return (_ERROR) ;
            }
            *lvalp = 0 ;	/* Group name stored in rtx->lex.sdx. */
            rtx->lex.scan += length ;
        } else {
            *lvalp = -1 ;	/* Empty or invalid group name. */
            SET_ERRNO (EINVAL) ;
            LGE "(yylex) Empty or invalid group name \"%*s\": ",
                (int) length, rtx->lex.scan) ;
        }

        display_token ("STRING", c) ;
        return (STRING) ;

    }

/*******************************************************************************

    Within a bracket expression ("[...]"), most characters are interpreted
    literally.  The parser and lexer communicate the progress of the parsing
    of the bracket expression using the lexer's AFTER-BRACKET field in the
    compiled RE structure.  This field has the following values under the
    following conditions:

        0 - Indicates that YYPARSE is NOT in the middle of parsing
            a bracket expression.

        1 - Indicates that YYPARSE has encountered the left bracket
            that begins a bracket expression.  YYLEX begins special
            handling of the characters in the bracket expression.

       -1 - Indicates that the complement '^' character appeared
            immediately after the left bracket.  YYLEX changes
            AFTER-BRACKET from 1 (which is how it knew the caret
            was immediately after the left bracket) to -1.

     -1,1 - Indicates that YYLEX is about to input the character
            which immediately follows a "[" with no caret or "[^"
            with the caret.  If that next character is a right
            bracket, "[]" or "[^]", then YYLEX treats the right
            bracket as a literal character to be incorporated
            into the bracket expression.  (There still needs to
            be a second right bracket at the end of the bracket
            expression; e.g., "[]...]" and "[^]...]".)

       >1 - Indicates that YYPARSE and YYLEX are past the initial
            "[" or "[^" and a literal right bracket, "[]" or "[^]".
            YYLEX treats the succeeding input characters normally.
            If a second left bracket, "[...[...]", is encountered,
            it is treated as a literal character to be matched,
            except ...  If the left bracket is followed by a colon,
            a character class specification, "[...[:<ctype>:]...]",
            is expected; e.g., "[...[:alnum:]...]" is equivalent
            to specifying "A-Za-z0-9" in the bracket expression.

            If a hyphen, "-", appears immediately before the right
            bracket that terminates the bracket expression, the
            character is treated as a literal hyphen and not as a
            range-of-characters operator.  For example, "[A-Z-]+"
            will match "AFTER-BRACKET".

            The full bracket expression is terminated by a right
            bracket, "[...]".  YYPARSE resets AFTER-BRACKET to 0.

    When the left bracket is encountered, YYPARSE sets AFTER_BRACKET to 1.
    A "^" immediately after the left bracket is returned as the character
    class complement indicator and AFTER_BRACKET is set to -1.  A right
    bracket or hyphen immediately after the left bracket (AFTER_BRACKET
    == 1) or the complement character (AFTER_BRACKET == -1) is returned
    as a literal character.  A hyphen following a hyphen ("[a--]") or
    preceding the right bracket ("[a-]") is returned as a literal character.

    A character class in a bracket expression is specified with a special,
    embedded bracket expression of a sort, "[:<class>:]", where the character
    class name appears between the colons (":"); e.g., "[^0-6[:alpha:]789]"
    matches any character but the ASCII alphabetic characters and the digits
    0 through 9.  When "[:" is seen within a bracket expression, YYPARSE
    instructs YYLEX to extract the following class name, look it up in
    the class name table, and return its index to YYPARSE.  An invalid
    name and a missing trailing delimiter (":]") are errors.

    Literal single-character "\<c>" constructs are handled by the non-bracket
    expression SWITCH statement further down below.

*******************************************************************************/

    if (rtx->lex.afterBracket) {
        switch (c) {
        case '^':
            if (rtx->lex.afterBracket == 1) {
                rtx->lex.afterBracket = -1 ;
                display_token ("CARET", c) ;  return (CARET) ;
            }
            if (rtx->lex.afterBracket == -1)  rtx->lex.afterBracket = 1 ;
            rtx->lex.afterBracket++ ;
            display_token ("SINGLE_CHARACTER", c) ;  return (SINGLE_CHARACTER) ;
        case ']':
            if (rtx->lex.afterBracket++ > 1) {
                display_token ("RIGHT_BRACKET", c) ;  return (RIGHT_BRACKET) ;
            }
            rtx->lex.afterBracket = 2 ;
            display_token ("SINGLE_CHARACTER", c) ;  return (SINGLE_CHARACTER) ;
        case '-':				/* Is "-" followed by a "]"? */
            c = (unsigned char) rrinput (rtx) ;  unput (rtx, c) ;
            if ((rtx->lex.afterBracket++ > 1) &&
                (!rtx->lex.hyphenAsLiteral) && (c != ']')) {
                display_token ("HYPHEN", c) ;  return (HYPHEN) ;
            }
            rtx->lex.afterBracket = 2 ;
            display_token ("SINGLE_CHARACTER", '-') ;
            return (SINGLE_CHARACTER) ;
        case '[':
            if (rtx->lex.afterBracket == -1)  rtx->lex.afterBracket = 1 ;
            rtx->lex.afterBracket++ ;
            c = (unsigned char) rrinput (rtx) ;
            if (c != ':') {
                unput (rtx, c) ;  c = '[' ;
                display_token ("SINGLE_CHARACTER", c) ;
                return (SINGLE_CHARACTER) ;
            }
            rtx->lex.afterBracket = 2 ;
				/* Extract class name from "[:<class>:]". */
            for (i = 0 ;
                 (c = (unsigned char) rrinput (rtx)) && (c != ':') ;
                ) {
                if (i < (sizeof buffer - 1))  buffer[i++] = c ;
            }
            buffer[i] = '\0' ;  strlwr (buffer) ;
            if (c != ':') {	/* Check for trailing ":]". */
                SET_ERRNO (EINVAL) ;
                LGE "(yylex) Missing or truncated \":]\": [:%s%c\n", buffer, c) ;
                *lvalp = c ;
                display_token ("_ERROR", c) ;  return (_ERROR) ;
            }
            c = (unsigned char) rrinput (rtx) ;
            if (c != ']') {	/* Check for trailing ":]". */
                SET_ERRNO (EINVAL) ;
                LGE "(yylex) Missing or truncated \":]\": [:%s:%c\n", buffer, c) ;
                *lvalp = c ;
                display_token ("_ERROR", c) ;  return (_ERROR) ;
            }
				/* Lookup name in class name table. */
            for (i = 0 ;  class_name[i] != NULL ;  i++) {
                if (strcmp (buffer, class_name[i]) == 0)  break ;
            }
            if (class_name[i] == NULL) {	/* Invalid class name? */
                SET_ERRNO (EINVAL) ;
                LGE "(yylex) Invalid character class: [:%s:]\n", buffer) ;
                *lvalp = ':' ;
                display_token ("_ERROR", ':') ;  return (_ERROR) ;
            }
            *lvalp = i ;  return (CTYPE) ;
        case '\\':
            if (rtx->lex.afterBracket == -1)  rtx->lex.afterBracket = 1 ;
            rtx->lex.afterBracket++ ;  rtx->lex.afterBracket++ ;
            break ;
        default:
            if (rtx->lex.afterBracket == -1)  rtx->lex.afterBracket = 1 ;
            rtx->lex.afterBracket++ ;
            display_token ("SINGLE_CHARACTER", c) ;
            return (SINGLE_CHARACTER) ;
        }
    }


/* Outside of a bracket expression, characters receive the standard regular
   expression interpretation. */

    switch (c) {
    case '*':
        display_token ("ASTERISK", c) ;  return (ASTERISK) ;
    case '^':
        display_token ("CARET", c) ;  return (CARET) ;
    case ':':
        display_token ("COLON", c) ;  return (COLON) ;
    case ',':
        display_token ("COMMA", c) ;  return (COMMA) ;
    case '$':
        display_token ("DOLLAR_SIGN", '$') ;  return (DOLLAR_SIGN) ;
    case '<':
        display_token ("LEFT_ANGLE", c) ;  return (LEFT_ANGLE) ;
    case '{':
        display_token ("LEFT_BRACE", c) ;  return (LEFT_BRACE) ;
    case '[':
        display_token ("LEFT_BRACKET", c) ;  return (LEFT_BRACKET) ;
    case '(':
        display_token ("LEFT_PAREN", c) ;  return (LEFT_PAREN) ;
    case '.':
        display_token ("PERIOD", c) ;  return (PERIOD) ;
    case '+':
        display_token ("PLUS", c) ;  return (PLUS) ;
    case '?':
        display_token ("QUESTION_MARK", c) ;  return (QUESTION_MARK) ;
    case '>':
        display_token ("RIGHT_ANGLE", c) ;  return (RIGHT_ANGLE) ;
    case '}':
        display_token ("RIGHT_BRACE", c) ;  return (RIGHT_BRACE) ;
    case ']':
        display_token ("RIGHT_BRACKET", c) ;  return (RIGHT_BRACKET) ;
    case ')':
        display_token ("RIGHT_PAREN", c) ;  return (RIGHT_PAREN) ;
    case '|':
        display_token ("VERTICAL_BAR", c) ;  return (VERTICAL_BAR) ;
    case '\\':
        c = rrinput (rtx) ;  *lvalp = c ;
        if (c == '\0') {
            rtx->errorText = strdup ("Regular expression ends with a single backslash, \"\\\".") ;
            display_token ("_ERROR", 'u') ;  return (_ERROR) ;
        }
        switch (c) {
        case 'a':  *lvalp = '\a' ;  break ;
        case 'b':  *lvalp = '\b' ;  break ;
        case 'f':  *lvalp = '\f' ;  break ;
        case 'n':  *lvalp = '\n' ;  break ;
        case 'r':  *lvalp = '\n' ;  break ;
        case 't':  *lvalp = '\t' ;  break ;
        case 'v':  *lvalp = '\v' ;  break ;
        case 'd':
        case 'D':
            if (rtx->lex.afterBracket) {
                rrInsert (rtx, true, "[:digit:]", -1) ;
            } else if (c == 'd') {
                rrInsert (rtx, true, "[[:digit:]]", -1) ;
            } else {
                rrInsert (rtx, true, "[^[:digit:]]", -1) ;
            }
            return (rrlex (lvalp, rtx)) ;	/* Return left bracket. */
        case 'x':
            c = (unsigned char) strntoul (rtx->lex.scan, &endn, 16, 2) ;
            if ((endn - rtx->lex.scan) != 2) {
                SET_ERRNO (EINVAL) ;
                LGE "(rrlex) Invalid \\xHH character specification: \"\\x%.2s\"\n",
                    rtx->lex.scan) ;
                *lvalp = 'x' ;
                display_token ("_ERROR", 'x') ;  return (_ERROR) ;
            }
            *lvalp = c ;
            rrinput (rtx) ;  rrinput (rtx) ;	/* Skip over hex digits. */
            break ;
        case 'u':
        case 'U':
            if (rtx->lex.afterBracket) {
                SET_ERRNO (EINVAL) ;
                LGE "(rrlex) \\u/\\U specifications not allowed in bracket expressions.\n") ;
                LGE "(rrlex) \\u/\\U converted to multi-character UTF-8; try alternation instead.\n") ;
                *lvalp = c ;
                display_token ("_ERROR", c) ;  return (_ERROR) ;
            }
#ifdef WHAT_IF
    What if "\uXXXX" is in lex.insert instead of lex.scan?
#endif
            if (*rtx->lex.scan == '{') {
                rrinput (rtx) ;			/* Skip left delimiter. */
                codePoint = (UniCodePoint) strtol (rtx->lex.scan, &endn, 16) ;
                length = endn - rtx->lex.scan ;
                if (*endn != '}') {
                    SET_ERRNO (EINVAL) ;
                    LGE "(rrlex) Invalid code point specification: \"\\%c{%.*s%c\"\n",
                        c, (int) length, rtx->lex.scan, *endn) ;
                    *lvalp = c ;
                    display_token ("_ERROR", c) ;  return (_ERROR) ;
                }
                length++ ;			/* To skip over '}' too. */
            } else {
                length = (c == 'u') ? 4 : 6 ;	/* \uXXXX or \UXXXXXX */
                codePoint = (UniCodePoint) strntol (rtx->lex.scan, &endn,
                                                    16, length) ;
                if ((endn - rtx->lex.scan) != length) {
                    SET_ERRNO (EINVAL) ;
                    LGE "(rrlex) Invalid code point specification: \"\\%c%.*s\"\n",
                        c, (int) length, rtx->lex.scan) ;
                    *lvalp = c ;
                    display_token ("_ERROR", c) ;  return (_ERROR) ;
                }
            }
            for (i = 0 ;  i < length ;  i++) {	/* Skip over hex digits. */
                rrinput (rtx) ;
            }
            if (utf8put (codePoint, buffer, &length)) {
                LGE "(rrlex) Error converting code point to UTF-8.\nutf8put: \n") ;
                *lvalp = 'u' ;
                display_token ("_ERROR", 'u') ;  return (_ERROR) ;
            }
            rrInsert (rtx, true, buffer, length) ;
            c = rrinput (rtx) ;		/* Get first byte of UTF-8 string. */
            *lvalp = c ;
            display_token ("SINGLE_CHARACTER", c) ;
            return (SINGLE_CHARACTER) ;
        default:   break ;
        }
        display_token ("SINGLE_CHARACTER", c) ;
        return (SINGLE_CHARACTER) ;
    default:
        if (isdigit (c)) {
            display_token ("DIGIT", c) ;  return (DIGIT) ;
        } else {
            display_token ("SINGLE_CHARACTER", c) ;  return (SINGLE_CHARACTER) ;
        }
    }

}

/*******************************************************************************

    rrinput ()

*******************************************************************************/


static  int  rrinput (

#    if PROTOTYPES
        CompiledRE  rtx)
#    else
        rtx)

        CompiledRE  rtx ;
#    endif

{    /* Local variables. */
    char  *insertScan ;
    int  c ;


/*******************************************************************************
    - What happens if we drop down to the expression text and unput()
      is called?
*******************************************************************************/

/* If the insert string has text and is not at the NUL terminator yet,
   then return the next character.  Note that the lexer may have added
   text to the insert string between calls to rrinput(), so it is
   necessary to get the insert string afresh each time (and to keep
   track of the lexer's position with an offset instead of a pointer). */

    insertScan = sdxStringZ (rtx->lex.insert) ;
    if (insertScan != NULL) {
        c = INT_UCHAR (insertScan[rtx->lex.insertOffset]) ;
        if (c != '\0') {
            rtx->lex.insertOffset++ ;
            return (c) ;
        }
        sdxErase (rtx->lex.insert) ;	/* End of insert string; ... */
        rtx->lex.insertOffset = 0 ;	/* ... drop down to expression text. */
    }

/* If the insert string has no text remaining, then return the next
   character in the expression text.  Don't advance beyond the NUL
   terminator. */

    c = INT_UCHAR (*rtx->lex.scan) ;
    if (c != '\0')  rtx->lex.scan++ ;
    return (c) ;

}

/*******************************************************************************

    rrInsert ()

*******************************************************************************/


static  errno_t  rrInsert (

#    if PROTOTYPES
        CompiledRE  rtx,
        bool  erase,
        const  char  *text,
        ssize_t  length)
#    else
        rtx, erase, text, length)

        CompiledRE  rtx ;
        bool  erase ;
        const  char  *text ;
        ssize_t  length ;
#    endif

{

    if (erase)  sdxErase (rtx->lex.insert) ;

    if (length < 0)  length = strlen (text) ;

    if (sdxAddS (rtx->lex.insert, text, length)) {
        LGE "(rrInsert) Error inserting \"%.*s\".\nsdxAddS: ",
            (int) length, text) ;
        return (errno) ;
    }

    LGI "(rrInsert) Inserted: \"%.*s\"\n", (int) length, text) ;

    return (0) ;

}

#ifdef NOT_USED_YET
/*******************************************************************************

    Parser Token Name/Number Lookup Table

    In this grammar file read by the parser-generator (Bison) are
    "%token <name>" statements which define the special, terminal
    tokens in the grammar.  Bison converts these token definitions
    into C Preprocessor definitions: "#define <name> <number>",
    where the numbers appear to begin at 258 and are incremented
    for each token.

    This library's Makefile scans the grammar file for the "%token"
    statements and automatically generates a "{ <name>, "<name>" },"
    line for each token.  These are suitable as entries in a
    name/number lookup table (see the NNL_UTIL package).  Remember
    that the name in "%token <name>" becomes a C Preprocessor macro,
    so, for example, "%token LEFT_BRACKET" will produce:
                     { LEFT_BRACKET, "LEFT_BRACKET" },
    (There is a trailing comma since the entry will be followed by
    more entries.)

    The lookup table is declared at or near the bottom of the grammar
    file because it must be initialized AFTER Bison has generated the
    "#define" token definitions.

*******************************************************************************/

static  const  NNLTable  tokenLUT  OCD ("rex_util")  = {
#    include  "rex_tokens.h"	/* Automatically generated by Makefile. */
    { -1, NULL }
} ;


/*******************************************************************************
    Function rexTokenByName() looks up a token name and returns its number.
*******************************************************************************/

static  int  rexTokenByName (

#    if PROTOTYPES
        const  char  *name)
#    else
        name)

        const  char  *name ;
#    endif

{

    return ((int) nnlNumberOf (tokenLUT, name, false, false)) ;

}


/*******************************************************************************
    Function rexTokenByNumber() looks up a token number and returns its name.
*******************************************************************************/

static  const  char  *rexTokenByNumber (

#    if PROTOTYPES
        int  number)
#    else
        number)

        int  number ;
#    endif

{

    return (nnlNameOf (tokenLUT, number)) ;

}
#endif	/* NOT_USED_YET */
