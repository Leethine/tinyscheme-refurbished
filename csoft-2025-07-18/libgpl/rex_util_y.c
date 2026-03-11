/* $Id: rex_util_y.c,v 1.10 2023/05/24 12:27:41 alex Exp alex $ */
/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 2

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Substitute the type names.  */
#define YYSTYPE         RRSTYPE
/* Substitute the variable and function names.  */
#define yyparse         rrparse
#define yylex           rrlex
#define yyerror         rrerror
#define yydebug         rrdebug
#define yynerrs         rrnerrs

/* First part of user prologue.  */
#line 1 "rex_util_y.y"

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


#line 250 "rex_util_y.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef RRDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define RRDEBUG 1
#  else
#   define RRDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define RRDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined RRDEBUG */
#if RRDEBUG
extern int rrdebug;
#endif

/* Token type.  */
#ifndef RRTOKENTYPE
# define RRTOKENTYPE
  enum rrtokentype
  {
    ASCII_NUL = 258,
    ASTERISK = 259,
    CARET = 260,
    COLON = 261,
    COMMA = 262,
    CTYPE = 263,
    DIGIT = 264,
    DOLLAR_SIGN = 265,
    _ERROR = 266,
    HYPHEN = 267,
    LEFT_ANGLE = 268,
    LEFT_BRACE = 269,
    LEFT_BRACKET = 270,
    LEFT_PAREN = 271,
    PERIOD = 272,
    PLUS = 273,
    RIGHT_ANGLE = 274,
    RIGHT_BRACE = 275,
    RIGHT_BRACKET = 276,
    RIGHT_PAREN = 277,
    QUESTION_MARK = 278,
    SINGLE_CHARACTER = 279,
    STRING = 280,
    VERTICAL_BAR = 281
  };
#endif

/* Value type.  */
#if ! defined RRSTYPE && ! defined RRSTYPE_IS_DECLARED
typedef StateIndex RRSTYPE;
# define RRSTYPE_IS_TRIVIAL 1
# define RRSTYPE_IS_DECLARED 1
#endif



int rrparse (CompiledRE rtx);





#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined RRSTYPE_IS_TRIVIAL && RRSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   56

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  27
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  21
/* YYNRULES -- Number of rules.  */
#define YYNRULES  49
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  61

#define YYUNDEFTOK  2
#define YYMAXUTOK   281


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26
};

#if RRDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   231,   231,   256,   265,   267,   292,   294,   305,   315,
     325,   356,   366,   355,   390,   415,   440,   466,   465,   495,
     502,   503,   509,   516,   520,   515,   545,   546,   553,   554,
     561,   564,   575,   579,   587,   589,   595,   600,   599,   614,
     671,   677,   687,   697,   707,   717,   727,   737,   747,   757
};
#endif

#if RRDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ASCII_NUL", "ASTERISK", "CARET",
  "COLON", "COMMA", "CTYPE", "DIGIT", "DOLLAR_SIGN", "_ERROR", "HYPHEN",
  "LEFT_ANGLE", "LEFT_BRACE", "LEFT_BRACKET", "LEFT_PAREN", "PERIOD",
  "PLUS", "RIGHT_ANGLE", "RIGHT_BRACE", "RIGHT_BRACKET", "RIGHT_PAREN",
  "QUESTION_MARK", "SINGLE_CHARACTER", "STRING", "VERTICAL_BAR", "$accept",
  "complete_re", "regular_expression", "term", "factor", "@1", "@2", "$@3",
  "preamble", "preamble_body", "$@4", "@5", "complement", "number",
  "upper_bound", "digits", "character_classes", "character_class", "$@6",
  "character", "match_character", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281
};
# endif

#define YYPACT_NINF (-23)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      18,   -23,   -23,   -23,   -23,   -23,   -23,   -23,   -23,   -23,
     -23,   -23,   -23,   -23,     8,     6,   -23,    -3,   -23,    31,
      17,   -23,    18,   -23,    32,   -23,   -23,   -23,   -23,    21,
      13,   -23,   -23,   -23,    37,    38,   -23,   -23,    22,   -23,
      36,   -23,   -23,   -23,    18,    32,    29,   -23,   -23,   -23,
     -23,    25,   -17,   -23,   -23,    27,   -23,   -23,   -23,    33,
     -23
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       3,     8,    42,    43,    44,     9,    45,    17,    11,    46,
      47,    48,    49,    41,     0,     2,     4,     6,    19,    26,
      20,     1,     3,    14,    28,    15,    16,     7,    27,     0,
       0,    12,     5,    32,    30,    29,    39,    40,     0,    34,
      36,    22,    23,    21,     3,    28,     0,    33,    18,    35,
      37,     0,     0,    31,    10,     0,    24,    13,    38,     0,
      25
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -23,   -23,   -22,    39,   -23,   -23,   -23,   -23,   -23,   -23,
     -23,   -23,   -23,     9,   -23,   -23,   -23,    15,   -23,     0,
     -23
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    14,    15,    16,    17,    20,    44,    19,    31,    43,
      51,    59,    29,    34,    46,    35,    38,    39,    55,    40,
      18
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      32,    23,     1,     2,     3,    57,     4,     5,    21,    22,
       6,    24,     7,     8,     9,    25,    10,    11,    12,    41,
      26,    13,    52,     1,     2,     3,    42,     4,     5,    36,
      36,     6,    22,     7,     8,     9,    28,    10,    11,    12,
      30,    33,    13,    48,    45,    37,    37,    47,    50,    54,
      56,    37,    60,    49,    53,    58,    27
};

static const yytype_int8 yycheck[] =
{
      22,     4,     5,     6,     7,    22,     9,    10,     0,    26,
      13,    14,    15,    16,    17,    18,    19,    20,    21,     6,
      23,    24,    44,     5,     6,     7,    13,     9,    10,     8,
       8,    13,    26,    15,    16,    17,     5,    19,    20,    21,
      23,     9,    24,    21,     7,    24,    24,     9,    12,    20,
      25,    24,    19,    38,    45,    55,    17
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,     6,     7,     9,    10,    13,    15,    16,    17,
      19,    20,    21,    24,    28,    29,    30,    31,    47,    34,
      32,     0,    26,     4,    14,    18,    23,    30,     5,    39,
      23,    35,    29,     9,    40,    42,     8,    24,    43,    44,
      46,     6,    13,    36,    33,     7,    41,     9,    21,    44,
      12,    37,    29,    40,    20,    45,    25,    22,    46,    38,
      19
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    27,    28,    29,    29,    29,    30,    30,    31,    31,
      31,    32,    33,    31,    31,    31,    31,    34,    31,    31,
      35,    35,    36,    37,    38,    36,    39,    39,    40,    40,
      41,    41,    42,    42,    43,    43,    44,    45,    44,    44,
      46,    47,    47,    47,    47,    47,    47,    47,    47,    47
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     0,     1,     3,     1,     2,     1,     1,
       5,     0,     0,     6,     2,     2,     2,     0,     5,     1,
       0,     2,     1,     0,     0,     5,     0,     1,     0,     1,
       0,     2,     1,     2,     1,     2,     1,     0,     4,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (rtx, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if RRDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, rtx); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, CompiledRE rtx)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (rtx);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, CompiledRE rtx)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, rtx);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule, CompiledRE rtx)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , rtx);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, rtx); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !RRDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !RRDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, CompiledRE rtx)
{
  YYUSE (yyvaluep);
  YYUSE (rtx);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (CompiledRE rtx)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, rtx);
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 232 "rex_util_y.y"
          { display_production ("complete_re", "regular_expression") ;
            /* The initial state equals the first state in RE.  Add a final
               state and link the last state in RE to the final state. */
            rtx->startState = yyvsp[0] ;
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, yyvsp[0])].next1 = rtx->numStates ;
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
#line 1570 "rex_util_y.c"
    break;

  case 3:
#line 256 "rex_util_y.y"
          { display_production ("regular_expression", "<empty>") ;
            /* Add an EMPTY state. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Empty ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 1584 "rex_util_y.c"
    break;

  case 4:
#line 266 "rex_util_y.y"
          { display_production ("regular_expression", "term") ;  yyval = yyvsp[0] ; }
#line 1590 "rex_util_y.c"
    break;

  case 5:
#line 268 "rex_util_y.y"
          { display_production ("regular_expression", "regular_expression | regular_expression") ;
            /* Add an alternation state E1 and an empty state E2.  Link E1 to
               the first state in RE1 and the first state in RE2.  Link the
               last state in RE1 to E2; do the same for the last state in RE2.
               (The first state in "RE|RE" is E1.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Alternation ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = yyvsp[-2] ;
            rtx->stateList[rtx->numStates].next2 = yyvsp[0] ;
            yyval = rtx->numStates++ ;
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, yyvsp[-2])].next1 = rtx->numStates ;
            rtx->stateList[rexLastState (rtx, yyvsp[0])].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Empty ;
            rtx->stateList[rtx->numStates].u.m.match_char = ' ' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            rtx->numStates++ ;
          }
#line 1615 "rex_util_y.c"
    break;

  case 6:
#line 293 "rex_util_y.y"
          { display_production ("term", "factor") ;  yyval = yyvsp[0] ; }
#line 1621 "rex_util_y.c"
    break;

  case 7:
#line 295 "rex_util_y.y"
          { display_production ("term", "factor term") ;
            /* Link the last state in RE1 to the first state in RE2.  (The
               first state in "RE1 RE2" is the first state in RE1.) */
            rtx->stateList[rexLastState (rtx, yyvsp[-1])].next1 = yyvsp[0] ;
            yyval = yyvsp[-1] ;
          }
#line 1632 "rex_util_y.c"
    break;

  case 8:
#line 306 "rex_util_y.y"
          { display_production ("factor", "^") ;
            /* Add an anchor state and set its match character to "^". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Anchor ;
            rtx->stateList[rtx->numStates].u.m.match_char = '^' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 1646 "rex_util_y.c"
    break;

  case 9:
#line 316 "rex_util_y.y"
          { display_production ("factor", "$") ;
            /* Add an anchor state and set its match character to "$". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Anchor ;
            rtx->stateList[rtx->numStates].u.m.match_char = '$' ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 1660 "rex_util_y.c"
    break;

  case 10:
#line 326 "rex_util_y.y"
          { size_t  length ;
            display_production ("factor", "factor{[min][,[max]]}") ;
            /* Check for nested closures that might cause the matching
               algorithm to endlessly loop without consuming any input
               from the target string; e.g., "(a*){0,10}".  Actually,
               the maximum field puts a cap on the number of loops, but
               we'll disallow it anyway. */
            length = rexShortestPath (rtx, yyvsp[-4], true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                yyvsp[-4], length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor { m, n }") ;
                YYABORT ;
            }
            /* Add a closure state and set its minimum and maximum fields.
               Link the last state in RE to the closure state; link the closure
               state backwards to the first state in RE.  (The first state in
               "RE{min,max}" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, yyvsp[-4])].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            if (yyvsp[-2] < 0)  yyvsp[-2] = 0 ;
            rtx->stateList[rtx->numStates].u.c.minClosure = yyvsp[-2] ;
            if (yyvsp[-1] < 0)  yyvsp[-1] = yyvsp[-2] ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = yyvsp[-1] ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = yyvsp[-4] ;
            yyval = rtx->numStates++ ;
          }
#line 1694 "rex_util_y.c"
    break;

  case 11:
#line 356 "rex_util_y.y"
          {
            NEW_STATE (rtx) ;
            yyval = rtx->numStates++ ;
            rtx->stateList[yyval].type = LeftParen ;	/* P1 */
            rtx->stateList[yyval].u.p.subexpIndex = rtx->numGroups++ + 1 ;
            rtx->stateList[yyval].u.p.subexpState = -1 ;	/* Not known yet. */
            rtx->stateList[yyval].next1 = -1 ;		/* Not known yet. */
            rtx->stateList[yyval].next2 = -1 ;
          }
#line 1708 "rex_util_y.c"
    break;

  case 12:
#line 366 "rex_util_y.y"
          {
            yyval = yyvsp[-1] ;
          }
#line 1716 "rex_util_y.c"
    break;

  case 13:
#line 370 "rex_util_y.y"
          { display_production ("factor", "(regular_expression)") ;
		/* Add two parenthesis states, P1 and P2.  Link P1 to the first
		   state in RE; link the last state in RE to P2.  The group
		   index (left parenthesis count) was already stored in P1. */
            NEW_STATE (rtx) ;
            yyval = rtx->numStates++ ;			/* Use $$ as shorthand. */
            rtx->stateList[rexLastState (rtx, yyvsp[-1])].next1 = yyval ;
            rtx->stateList[yyval].type = RightParen ;	/* P2 */
            rtx->stateList[yyval].u.p.subexpIndex =
                rtx->stateList[yyvsp[-4]].u.p.subexpIndex ;	/* Index from P1. */
            rtx->stateList[yyval].u.p.subexpState = yyvsp[-4] ;
            rtx->stateList[yyval].next1 = -1 ;
            rtx->stateList[yyval].next2 = -1 ;
            rtx->stateList[yyvsp[-4]].u.p.subexpState = yyval ;	/* Back-fill P1. */
            rtx->stateList[yyvsp[-4]].next1 = yyvsp[-1] ;
            rtx->stateList[yyvsp[-4]].next2 = -1 ;
		/* NOTE that the result of this rule is P1, the "LeftParen"
		   state, not the "RightParen" state, so reassign $$ to $2. */
            yyval = yyvsp[-4] ;
          }
#line 1741 "rex_util_y.c"
    break;

  case 14:
#line 391 "rex_util_y.y"
          { size_t  length ;
            display_production ("factor", "factor*") ;
            /* Check for nested closures that might cause the matching
               algorithm to endlessly loop without consuming any input
               from the target string; e.g., "(a*)*". */
            length = rexShortestPath (rtx, yyvsp[-1], true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                yyvsp[-1], length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor*") ;
                YYABORT ;
            }
            /* Add a closure state.  Link the last state in RE to the closure
               state; link the closure state backwards to the first state in
               RE.  (The first state in "RE*" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, yyvsp[-1])].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            rtx->stateList[rtx->numStates].u.c.minClosure = 0 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = SIZE_MAX ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = yyvsp[-1] ;
            yyval = rtx->numStates++ ;
          }
#line 1770 "rex_util_y.c"
    break;

  case 15:
#line 416 "rex_util_y.y"
          { size_t  length ;
            display_production ("factor", "factor+") ;
            /* Check for nested closures that might cause the matching
               algorithm to endlessly loop without consuming any input
               from the target string; e.g., "(a*)+". */
            length = rexShortestPath (rtx, yyvsp[-1], true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                yyvsp[-1], length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor+") ;
                YYABORT ;
            }
            /* Add a closure state.  Link the last state in RE to the closure
               state; link the closure state backwards to the first state in
               RE.  (The first state in "RE+" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, yyvsp[-1])].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            rtx->stateList[rtx->numStates].u.c.minClosure = 1 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = SIZE_MAX ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = yyvsp[-1] ;
            yyval = rtx->numStates++ ;
          }
#line 1799 "rex_util_y.c"
    break;

  case 16:
#line 441 "rex_util_y.y"
          { size_t  length ;
            display_production ("factor", "factor?") ;
            /* Check for nested closures.  Even though nested closures are
               not a major threat in a zero-or-one closure, we check anyway,
               just to be consistent with the other forms of closure. */
            length = rexShortestPath (rtx, yyvsp[-1], true) ;
            LGI "(yyparse) Shortest path from state %"PRIdSSIZE_T" = %"PRIuSIZE_T"\n",
                yyvsp[-1], length) ;
            if (length == 0) {
                yyerror (rtx, "nested empty closure - factor?") ;
                YYABORT ;
            }
            /* Add a closure state.  Link the last state in RE to the closure
               state; link the closure state backwards to the first state in
               RE.  (The first state in "RE?" is the closure state.) */
            NEW_STATE (rtx) ;
            rtx->stateList[rexLastState (rtx, yyvsp[-1])].next1 = rtx->numStates ;
            rtx->stateList[rtx->numStates].type = Closure ;
            rtx->stateList[rtx->numStates].u.c.minClosure = 0 ;
            rtx->stateList[rtx->numStates].u.c.maxClosure = 1 ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = yyvsp[-1] ;
            yyval = rtx->numStates++ ;
          }
#line 1828 "rex_util_y.c"
    break;

  case 17:
#line 466 "rex_util_y.y"
          { CS_ZERO (&rtx->buildSet) ;
            rtx->lex.afterBracket = 1 ;
            rtx->lex.hyphenAsLiteral = false ;
          }
#line 1837 "rex_util_y.c"
    break;

  case 18:
#line 471 "rex_util_y.y"
          { display_production ("factor", "[character_classes]") ;
            rtx->lex.afterBracket = 0 ;
            if (yyvsp[-2])  CS_COMP (&rtx->buildSet) ;
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
            yyval = rtx->numStates++ ;
          }
#line 1866 "rex_util_y.c"
    break;

  case 19:
#line 496 "rex_util_y.y"
          { display_production ("factor", "match_character") ;  yyval = yyvsp[0] ; }
#line 1872 "rex_util_y.c"
    break;

  case 20:
#line 502 "rex_util_y.y"
          { display_production ("preamble", "<empty>") ;  yyval = -1 ; }
#line 1878 "rex_util_y.c"
    break;

  case 21:
#line 504 "rex_util_y.y"
          { display_production ("preamble", "?<preamble_body>") ;  yyval = yyvsp[0] ; }
#line 1884 "rex_util_y.c"
    break;

  case 22:
#line 510 "rex_util_y.y"
          { display_production ("preamble_body", ":") ;
            rtx->stateList[rtx->numStates-1].u.p.subexpIndex = 0 ;
            rtx->numGroups-- ;
            yyval = -1 ;
          }
#line 1894 "rex_util_y.c"
    break;

  case 23:
#line 516 "rex_util_y.y"
          { sdxErase (rtx->lex.sdx) ;
            rtx->lex.nameDelimiter = '>' ;
          }
#line 1902 "rex_util_y.c"
    break;

  case 24:
#line 520 "rex_util_y.y"
          { rtx->lex.nameDelimiter = '\0' ;
            if (yyvsp[0] < 0) {
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
            yyval = -1 ;
          }
#line 1926 "rex_util_y.c"
    break;

  case 26:
#line 545 "rex_util_y.y"
          { display_production ("complement", "<empty>") ;  yyval = 0 ; }
#line 1932 "rex_util_y.c"
    break;

  case 27:
#line 547 "rex_util_y.y"
          { display_production ("complement", "^") ;  yyval = -1 ; }
#line 1938 "rex_util_y.c"
    break;

  case 28:
#line 553 "rex_util_y.y"
          { display_production ("number", "<empty>") ;  yyval = -1 ; }
#line 1944 "rex_util_y.c"
    break;

  case 29:
#line 555 "rex_util_y.y"
          { display_production ("number", "digits") ;  yyval = yyvsp[0] ; }
#line 1950 "rex_util_y.c"
    break;

  case 30:
#line 561 "rex_util_y.y"
          { display_production ("upper_bound", "<empty>") ;
            yyval = -1 ;			/* Upper bound = lower bound. */
          }
#line 1958 "rex_util_y.c"
    break;

  case 31:
#line 565 "rex_util_y.y"
          { display_production ("lower_bound", "digits") ;
            if (yyvsp[0] < 0)
                yyval = SSIZE_MAX ;	/* Upper bound = infinity. */
            else
                yyval = yyvsp[0] ;		/* Normal upper bound. */
          }
#line 1969 "rex_util_y.c"
    break;

  case 32:
#line 576 "rex_util_y.y"
          { display_production ("digits", "DIGIT") ;
            yyval = yyvsp[0] - '0' ;
          }
#line 1977 "rex_util_y.c"
    break;

  case 33:
#line 580 "rex_util_y.y"
          { display_production ("digits", "digits DIGIT") ;
            yyval = (yyvsp[-1] * 10) + (yyvsp[0] - '0') ;
          }
#line 1985 "rex_util_y.c"
    break;

  case 34:
#line 588 "rex_util_y.y"
          { display_production ("character_classes", "character_class") ; }
#line 1991 "rex_util_y.c"
    break;

  case 35:
#line 590 "rex_util_y.y"
          { display_production ("character_classes", "character_classes character_class") ; }
#line 1997 "rex_util_y.c"
    break;

  case 36:
#line 596 "rex_util_y.y"
          { display_production ("character_class", "character") ;
            CS_SET (yyvsp[0], &rtx->buildSet) ;
          }
#line 2005 "rex_util_y.c"
    break;

  case 37:
#line 600 "rex_util_y.y"
          { rtx->lex.hyphenAsLiteral = true ; }
#line 2011 "rex_util_y.c"
    break;

  case 38:
#line 602 "rex_util_y.y"
          { size_t  i ;
            display_production ("character_class", "character-character") ;
            rtx->lex.hyphenAsLiteral = false ;
            if (yyvsp[-3] > yyvsp[0]) {
                LGI "(yyparse) Start character exceeds end character in character class range \"%c-%c\".\n",
                    (int) yyvsp[-3], (int) yyvsp[0]) ;
                yyerror (rtx, "inverted character class range") ;
                YYABORT ;
            }
            for (i = (size_t) yyvsp[-3] ;  i <= (size_t) yyvsp[0] ;  i++)
                CS_SET (i, &rtx->buildSet) ;
          }
#line 2028 "rex_util_y.c"
    break;

  case 39:
#line 615 "rex_util_y.y"
          { size_t  i ;
            display_production ("character_class", "[:<CTYPE>:]") ;
            for (i = 0 ;  i < CS_SETSIZE ;  i++) {
                switch (yyvsp[0]) {
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
#line 2085 "rex_util_y.c"
    break;

  case 40:
#line 672 "rex_util_y.y"
          { display_production ("character", "SINGLE_CHARACTER") ;  yyval = yyvsp[0] ; }
#line 2091 "rex_util_y.c"
    break;

  case 41:
#line 678 "rex_util_y.y"
          { display_production ("match_character", "SINGLE_CHARACTER") ;
            /* Add a one-character state for the character. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2105 "rex_util_y.c"
    break;

  case 42:
#line 688 "rex_util_y.y"
          { display_production ("match_character", "COLON") ;
            /* Add a one-character state for ":". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2119 "rex_util_y.c"
    break;

  case 43:
#line 698 "rex_util_y.y"
          { display_production ("match_character", "COMMA") ;
            /* Add a one-character state for ",". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2133 "rex_util_y.c"
    break;

  case 44:
#line 708 "rex_util_y.y"
          { display_production ("match_character", "DIGIT") ;
            /* Add a one-character state for the digit. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2147 "rex_util_y.c"
    break;

  case 45:
#line 718 "rex_util_y.y"
          { display_production ("match_character", "LEFT_ANGLE") ;
            /* Add a one-character state for "<". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2161 "rex_util_y.c"
    break;

  case 46:
#line 728 "rex_util_y.y"
          { display_production ("match_character", "PERIOD") ;
            /* Add a one-character state for any-character-matches. */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = REX_MATCH_CHAR_ANY_BUT_NL ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2175 "rex_util_y.c"
    break;

  case 47:
#line 738 "rex_util_y.y"
          { display_production ("match_character", "RIGHT_ANGLE") ;
            /* Add a one-character state for ">". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2189 "rex_util_y.c"
    break;

  case 48:
#line 748 "rex_util_y.y"
          { display_production ("match_character", "RIGHT_BRACE") ;
            /* Add a one-character state for "}". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2203 "rex_util_y.c"
    break;

  case 49:
#line 758 "rex_util_y.y"
          { display_production ("match_character", "RIGHT_BRACKET") ;
            /* Add a one-character state for "]". */
            NEW_STATE (rtx) ;
            rtx->stateList[rtx->numStates].type = Match ;
            rtx->stateList[rtx->numStates].u.m.match_char = INT_UCHAR (yyvsp[0]) ;
            rtx->stateList[rtx->numStates].next1 = -1 ;
            rtx->stateList[rtx->numStates].next2 = -1 ;
            yyval = rtx->numStates++ ;
          }
#line 2217 "rex_util_y.c"
    break;


#line 2221 "rex_util_y.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (rtx, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (rtx, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, rtx);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, rtx);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (rtx, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, rtx);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp, rtx);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 770 "rex_util_y.y"


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
