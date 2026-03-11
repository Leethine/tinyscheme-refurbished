/* $Id: sto_util.c,v 1.2 2021/09/14 16:06:07 alex Exp $ */
/*******************************************************************************

File:

    sto_util.c

    Length-Limited String to Number Conversion Functions


Author:    Alex Measday


Purpose:

    The STO_UTIL package provides functions for converting length-limited
    substrings to numbers.

    There is a standard set of NUL-terminated string to number conversion
    functions; e.g., strtol() for long integers and strtod() for double
    floating-point numbers:

        long  integer = strtol (thisString, NULL, 0) ;
        double  real = strtod (thatString, NULL) ;

    Unfortunately, there aren't any standard, corresponding functions for
    length-limited substrings, such as:

        const  char  *csvString = "abc,def,123,ghi,456.789" ;
        char  *endp ;
        unsigned  long  number = strntoul (&csvString[8], &endp, 10, 5) ;

    This example scans at most 5 characters beginning with the 8th character
    in the string ("123,g").  In this case, strntoul() will return a value
    of 123; the end pointer will return &csvString[11], the address of the
    comma immediately following the number.

    On the internet, I found a variety of signatures for the strnto...()
    functions, some including character-set information.  My personal
    preference is to order arguments by type: (i) input, (ii) input/output,
    and (iii) output.  In particular, the string length argument should be
    placed right after the input string and the base (an input parameter)
    should come before the end pointer (an output argument).  If I'd had my
    druthers:

        result = strtol (string, base, &endp) ;		-- Not the real order.
        result = strntol (string, length, base, &endp) ;

    However, I didn't have my druthers and I decided to add the string length
    after the existing arguments.  If the functions are ever standardized, I
    figure that that's what will happen.

    My strnto...() functions depend on the existence of the strto...()
    functions.  A NUL-terminated duplicate of the substring is made with
    strndup(3) and the duplicate is then passed to the corresponding
    strto...() function.  With all the strndup(3)s, my functions are
    obviously not the speediest ones in the world.  One implementation
    I saw used alloca(3), an idea I like except for the problems with
    alloca(3).  Another implementation recreated the whole kit and
    caboodle of the strto...() functions, modified to check if the
    scan has reached the end of the substring.

    After writing the strntol() function and then modifying a copy of it in
    a matter of seconds for strntoul(), I realized that the functions would
    all look the same.  I created a template file, "sto_util_template.h",
    which uses C Preprocessor macros for the function names and data types.
    Consequently, for each function, the "code" in this file consists of
    "#define"ing the macros and then "#include"ing the template.

    The template handles two types of functions calls, those for integer
    conversions and those for floating-point conversions, distinguished
    by the number of arguments passed to their strto...() functions.  For
    example, all the integer strto...()s have 3 arguments:

        result = strto...(string, &endp, base) ;

    The floating-point functions drop the base and have 2 arguments:

        result = strto...(string, &endp) ;

    The strnto...() functions, of course, have the extra length argument
    at the end.

    Shortly afterwards, I added the anto...() functions, which are simply
    the strnto...() functions without the endpoint and base arguments.


Public Procedures (String-to functions):

    strntol() - string to long.
    strntoul() - string to unsigned long.

    strntoll() - string to long long.
    strntoull() - string to unsigned long long.

    strntof() - string to float.
    strntod() - string to double.
    strntold() - string to long double.

Public Procedures (Abbreviated ASCII-to functions):

    antof() - ASCII to double.
    antoi() - ASCII to integer.
    antol() - ASCII to long.
    antoll() - ASCII to long long

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stddef.h>			/* Standard C definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "sto_util.h"			/* String-to-number functions. */


int  sto_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  sto_util_debug

/*******************************************************************************
    antof(), antoi(), antol(), and antoll()
*******************************************************************************/

#if defined(HAVE_ANTOF) && !HAVE_ANTOF
#    define  STRNTO_ANTO  1
#    define  STRNTO_FUNC  antof
#    define  STRNTO_TYPE  double
#    define  STRTO_FUNC  strtod
#    define  STRTO_NUM_ARGS  2
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_ANTOI) && !HAVE_ANTOI
#    define  STRNTO_ANTO  1
#    define  STRNTO_FUNC  antoi
#    define  STRNTO_TYPE  int
#    define  STRTO_FUNC  strtol
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_ANTOL) && !HAVE_ANTOL
#    define  STRNTO_ANTO  1
#    define  STRNTO_FUNC  antol
#    define  STRNTO_TYPE  long
#    define  STRTO_FUNC  strtol
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_ANTOLL) && !HAVE_ANTOLL
#    define  STRNTO_ANTO  1
#    define  STRNTO_FUNC  antoll
#    define  STRNTO_TYPE  long long
#    define  STRTO_FUNC  strtoll
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif

/*******************************************************************************
    strntol() and strntoul()
*******************************************************************************/

#if defined(HAVE_STRNTOL) && !HAVE_STRNTOL
#    define  STRNTO_FUNC  strntol
#    define  STRNTO_TYPE  long
#    define  STRTO_FUNC  strtol
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_STRNTOUL) && !HAVE_STRNTOUL
#    define  STRNTO_FUNC  strntoul
#    define  STRNTO_TYPE  unsigned long
#    define  STRTO_FUNC  strtoul
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif


/*******************************************************************************
    strntoll() and strntoull()
*******************************************************************************/

#if defined(HAVE_STRNTOLL) && !HAVE_STRNTOLL
#    define  STRNTO_FUNC  strntoll
#    define  STRNTO_TYPE  long long
#    define  STRTO_FUNC  strtoll
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_STRNTOULL) && !HAVE_STRNTOULL
#    define  STRNTO_FUNC  strntoull
#    define  STRNTO_TYPE  unsigned long long
#    define  STRTO_FUNC  strtoull
#    define  STRTO_NUM_ARGS  3
#    include  "sto_util_template.h"
#endif

/*******************************************************************************
    strntof(), strntod(), and strntold
*******************************************************************************/

#if defined(HAVE_STRNTOF) && !HAVE_STRNTOF
#    define  STRNTO_FUNC  strntof
#    define  STRNTO_TYPE  float
#    define  STRTO_FUNC  strtof
#    define  STRTO_NUM_ARGS  2
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_STRNTOD) && !HAVE_STRNTOD
#    define  STRNTO_FUNC  strntod
#    define  STRNTO_TYPE  double
#    define  STRTO_FUNC  strtod
#    define  STRTO_NUM_ARGS  2
#    include  "sto_util_template.h"
#endif

#if defined(HAVE_STRNTOLD) && !HAVE_STRNTOLD
    /* Check that the platform has C Library's strtold(3); PalmOS doesn't. */
#    if !defined(HAVE_STRTOLD) || HAVE_STRTOLD
#        define  STRNTO_FUNC  strntold
#        define  STRNTO_TYPE  long double
#        define  STRTO_FUNC  strtold
#        define  STRTO_NUM_ARGS  2
#        include  "sto_util_template.h"
#    endif
#endif
