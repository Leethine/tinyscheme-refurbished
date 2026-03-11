/* $Id: iel_util.h,v 1.1 2021/03/24 05:15:02 alex Exp $ */

#ifndef  IEL_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  IEL_UTIL_H  yes


/*******************************************************************************

    iel_util.h

    Information and Error Logging.


    The IEL utilities are actually a set of macros that perform information
    and error logging.  For information logging, code invokes the following
    macro:

        LGI "<printf(3) format>", arg1, arg2, ...) ;

    NOTE that there is NO left parenthesis after LGI, but there IS a right
    parenthesis at the end of the statement.  Error logging is accomplished
    with:

        LGE "<printf(3) format>", arg1, arg2, ...) ;

    The implementation of the macros was inspired by (and basically copied
    from) Philip J. Maker's GNU Nana library (https://github.com/pjmaker/nana).
    The Nana library is an actual library with many more features than the
    simple logging macros that I borrowed would suggest.

    The differences between my macros and the Nana library's corresponding
    macros:

        (1) The prefixes.  I use I_ and E_ prefixes for information and
            error logging, respectively.  The corresponding prefixes in
            Nana are L_ (logging, "L.h") and I_ (invariants/assertions,
            "I.h").

        (2) Nana makes use of the GNU C Preprocessor's support for defining
            macros with a variable number of arguments.  Since I've built
            my code on platforms whose C Preprocessors don't support
            variable-length argument lists, the LGI and LGE definitions
            supply the left parenthesis and the developer is responsible
            for supplying the right parenthesis.


    INFORMATION LOGGING
    -------------------

    The LGI macro is defined as follows:

        if ((I_LEVEL == 2) ||				-- Always, or ...
            ((I_LEVEL == 1) && (I_DEFAULT_GUARD)))	-- conditionally ...
            I_DEFAULT_HANDLER (I_DEFAULT_PARAMS		-- log a message.

    where the implementation macros are:

        I_LEVEL - specifies when LGI logs a message.  If the level is 2,
            the message is always logged.  If the level is 1, the message
            is only logged if I_DEFAULT_GUARD evaluates to a true value.

        I_DEFAULT_GUARD - provides conditional logging of a message by LGI
            (if I_LEVEL is 2).  This can be any expression that evaluates
            to true (non-zero) or false (zero).

        I_DEFAULT_HANDLER - is the function LGI will call to log the message.

        I_DEFAULT_PARAMS - are leading arguments of the handler function that
            LGI inserts before the trailing, developer-supplied arguments.
            This macro is a comma-separated list of arguments that must have
            a final comma after the last argument.

    For example, in my LIBGPL library, each package has a global integer
    called "<package>_debug".  A program can enable logging from a package
    by setting its debug variable to a non-zero value.  So, with these
    macro definitions from this header file:

        #define  I_DEFAULT_HANDLER  printf
        #define  I_DEFAULT_PARAMS
        #define  I_LEVEL  1

    and these declarations in the regular expression package, "rex_util.c":

        int  rex_util_debug = 0 ;	-- Debug switch (0 = off, !0 = on).
        #undef  I_DEFAULT_GUARD
        #define  I_DEFAULT_GUARD  rex_util_debug

    the following statement in rexCompile():

        LGI "(rexCompile) CompiledRE %p created for \"%s\".\n",
            (void *) *pattern, expression) ;

    will be expanded to:

        if ((1 == 2) || ((1 == 1) && (rex_util_debug)))
            printf ("(rexCompile) CompiledRE %p created for \"%s\".\n",
                    (void *) *pattern, expression) ;


    ERROR LOGGING
    -------------

    The LGE error logging macro is implemented with similar macros, for
    example:

        #define  E_DEFAULT_GUARD  1
        #define  E_DEFAULT_HANDLER  fprintf
        #define  E_DEFAULT_PARAMS  stderr,
        #define  E_LEVEL  2

    With E_LEVEL equal to 2, error messages will always be logged.  Much
    like LGI, this LGE error logging statement in rexCompile():

        LGE "(rexCompile) Error allocating context node for \"%s\".\nmalloc: ",
            expression) ;

    expands to:

        if ((2 == 2) || ((2 == 1) && (1)))
            fprintf (stderr,
                     "(rexCompile) Error allocating context node for \"%s\".\nmalloc: ",
                     expression) ;

    As you see below, my software actually uses my aperror() function to
    print the error message and the ERRNO message, which is why there is
    the "\nmalloc: " at the end of the format string.


    ADAPTING TO OTHER LOGGING METHODS
    ---------------------------------

    LGI and LGE can be adapted to other logging methods by redefining their
    implementation macros.  On a C++ project at work, we used DebugMsg()
    and ErrorMsg() functions that logged messages to a program's log file
    object (which could have various destinations, e.g., a file, a network
    connection, etc.).  So, for LGI messages, the following definitions:

        #define  I_DEFAULT_GUARD  1
        #define  I_DEFAULT_HANDLER  DebugMsg
        #define  I_DEFAULT_PARAMS  DBG_APP1,

    render the LGI example above as:

        if ((1 == 2) || ((1 == 1) && (1)))
            DebugMsg (DBG_APP1,
                      "(rexCompile) CompiledRE %p created for \"%s\".\n",
                      (void *) *pattern, expression) ;

    I_DEFAULT_GUARD is set to 1 so that DebugMsg() is always called; the
    DBG_APP1 argument to the function is a bit set in a bit mask that is
    matched against the program's bit mask (specified in a command-line
    option) of what types of debug to log.  Now that I look at it, I
    could have just set I_LEVEL to 2 to ensure DebugMsg() was always
    called; I doubt it, but maybe I was just trying to emphasize that
    the real guard was tested inside the DebugMsg() function.

    LGE is implemented similarly, except that "ErrorMsg (DBG_ERROR, ...)"
    is called to log the error message.

*******************************************************************************/

/*******************************************************************************
    Information logging.
*******************************************************************************/

#ifndef I_DEFAULT_GUARD
#    define  I_DEFAULT_GUARD  1
#endif
#ifndef I_DEFAULT_HANDLER
#    define  I_DEFAULT_HANDLER  printf
#endif
#ifndef I_DEFAULT_PARAMS
#    define  I_DEFAULT_PARAMS
#endif
#ifndef I_LEVEL
#    define  I_LEVEL  1
#endif

#ifndef LGI
#    define  LGI							\
        if ((I_LEVEL == 2) || ((I_LEVEL == 1) && (I_DEFAULT_GUARD)))	\
            I_DEFAULT_HANDLER (I_DEFAULT_PARAMS
#endif

/*******************************************************************************
    Error logging.
*******************************************************************************/

#if !defined(USE_APERROR) || USE_APERROR
#    include  "aperror.h"		/* Error reporting function. */
#    ifndef E_DEFAULT_HANDLER
#        define  E_DEFAULT_HANDLER  aperror
#    endif
#    ifndef E_DEFAULT_PARAMS
#        define  E_DEFAULT_PARAMS
#    endif
#endif

#ifndef E_DEFAULT_GUARD
#    define  E_DEFAULT_GUARD  1
#endif
#ifndef E_DEFAULT_HANDLER
#    define  E_DEFAULT_HANDLER  fprintf
#endif
#ifndef E_DEFAULT_PARAMS
#    define  E_DEFAULT_PARAMS  stderr,
#endif
#ifndef E_LEVEL
#    define  E_LEVEL  2
#endif

#ifndef LGE
#    define  LGE							\
        if ((E_LEVEL == 2) || ((E_LEVEL == 1) && (E_DEFAULT_GUARD)))	\
            E_DEFAULT_HANDLER (E_DEFAULT_PARAMS
#endif


#endif				/* If this file was not INCLUDE'd previously. */
