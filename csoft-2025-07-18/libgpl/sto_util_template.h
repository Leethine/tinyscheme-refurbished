/* $Id: sto_util_template.h,v 1.1 2021/04/10 04:52:41 alex Exp $ */
/*******************************************************************************

    Length-Limited String to Number Conversion Functions Template.

        NOTE that the source file "#include"ing this template is
        responsible for including the header files necessary for
        the template to compile.  As of now:

            <stddef.h> - ptrdiff_t data type
            <stdlib.h> - strto...() functions
            <string.h> - strndup(3)
            "pragmatics.h" - LGE macro
            <stdio.h> - depending on the LGE macro

        If the application has a different error reporting mechanism,
        the two LGE statements can be replaced or deleted, obviating
        the need for "pragmatics.h".

*******************************************************************************/


#ifndef STRINGIFY
#    define  _STRINGIFY(a)  #a
#    define  STRINGIFY(a)  _STRINGIFY(a)
#endif


/*******************************************************************************

    STRNTO_FUNC - is the name of the strnto...() function being defined.
    STRNTO_TYPE - is the data type of this function's return value.
    STRTO_FUNC - is the name of the corresponding strto...() function
        for this data type.

    These three macros must be defined before including this template.
    Furthermore, these macros are "#undef"ed at the end of the template.

    If an anto...() function is being implemented, STRNTO_ANTO must be
    defined (value ignored).  It also is "#undef"ed at the end.

    The STRTO_NUM_ARGS macro below is equally important, but it defaults
    to 3 arguments, which the integer functions take: input string, end
    pointer, and base.

    For example, implementing strntoull(), which converts a string to an
    unsigned long long (an integer type), is done as follows:

        #define  STRNTO_FUNC  strntoull
        #define  STRNTO_TYPE  unsigned long long
        #define  STRTO_FUNC  strtoull
        #define  STRTO_NUM_ARGS  3
        #include  "sto_util_template.h"

    Implementing strntold(), which converts a string to a long double
    (a floating-point type), is done this way:

        #define  STRNTO_FUNC  strntold
        #define  STRNTO_TYPE  long double
        #define  STRTO_FUNC  strtold
        #define  STRTO_NUM_ARGS  2
        #include  "sto_util_template.h"

    Implementing antoi(), the length-limited counterpart to atoi(3),
    requires that STRNTO_ANTO be defined:

        #define  STRNTO_ANTO
        #define  STRNTO_FUNC  antoi
        #define  STRNTO_TYPE  int
        #define  STRTO_FUNC  strtol
        #define  STRTO_NUM_ARGS  3
        #include  "sto_util_template.h"

*******************************************************************************/

#if !defined(STRNTO_FUNC) || !defined(STRNTO_TYPE) || !defined(STRTO_FUNC)
#    error
#endif


/*******************************************************************************
    STRTO_NUM_ARGS - specifies how many arguments this data type's
        strto...() function takes.  This template handles only the
        following two possibilities:
            #args = 2:    Floating point functions whose arguments
                          are the input string and the end pointer.
            #args = 3:    Integer functions whose arguments are the
                          input string, the base, and the end pointer.
*******************************************************************************/

#if !defined(STRTO_NUM_ARGS)
#    define  STRTO_NUM_ARGS  3
#endif
#if (STRTO_NUM_ARGS != 2) && (STRTO_NUM_ARGS != 3)
#    error
#endif

/*******************************************************************************
    The Template.  A single function template was able to be used by
    "#if"ing in or out the base argument depending on STRTO_NUM_ARGS.
    (And the end pointer argument based on STRNTO_ANTO.)
*******************************************************************************/

STRNTO_TYPE  STRNTO_FUNC (

#  ifdef STRNTO_ANTO

#    if PROTOTYPES
        const  char  *substring,
        size_t  length)
#    else
        substring, length)

        const  char  *substring ;
        size_t  length ;
#    endif

#  else		/* !STRNTO_ANTO */

#    if PROTOTYPES
        const  char  *substring,
        char  **endptr,
#     if STRTO_NUM_ARGS == 3
        int  base,
#     endif
        size_t  length)
#    else	/* !PROTOTYPES */
        substring,
        endptr,
#     if STRTO_NUM_ARGS == 3
        base,
#     endif
        length)

        const  char  *substring ;
        char  **endptr ;
#     if STRTO_NUM_ARGS == 3
        int  base ;
#     endif
        size_t  length ;
#    endif	/* !PROTOTYPES */

#  endif	/* !STRNTO_ANTO */

{    /* Local variables. */
    char  *s ;
    STRNTO_TYPE  number ;
#ifdef STRNTO_ANTO
    char  *endp ;
    char  **endptr = &endp ;
#  if STRTO_NUM_ARGS == 3
    int  base = 10 ;
#  endif
#endif


    if (endptr != NULL)  *endptr = NULL ;

    if (substring == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(" STRINGIFY (STRNTO_FUNC) ") NULL input string: ") ;
        return (0) ;
    }

    s = strndup (substring, length) ;
    if (s == NULL) {
        LGE "(" STRINGIFY (STRNTO_FUNC) ") Error duplicating number \"%*s\"\nstrndup: ",
            (int) length, substring) ;
        return (0) ;
    }

#if STRTO_NUM_ARGS == 3
    number = (STRNTO_TYPE) STRTO_FUNC (s, endptr, base) ;
#else
    number = (STRNTO_TYPE) STRTO_FUNC (s, endptr) ;
#endif

    if (endptr != NULL) {
        ptrdiff_t  consumed = *endptr - s ;
        *endptr = (char *) substring + consumed ;
    }

    free (s) ;

    return (number) ;

}

/*******************************************************************************
    Undefine the important macros so that the programmer is forced to
    explicitly redefine them before including this template again.
*******************************************************************************/

#undef  STRNTO_ANTO
#undef  STRNTO_FUNC
#undef  STRNTO_TYPE
#undef  STRTO_FUNC
#undef  STRTO_NUM_ARGS
