/* $Id: assert.h,v 1.1 2019/06/21 02:28:49 alex Exp $ */
/*******************************************************************************

    assert.h

    Standard C Assertions.

    NOTE: This is a very simple "assert.h" file for building PalmOS software
    with PRC-TOOLS.  (I needed it for compiling BSD-XDR's "xdr_rec.c".)  If
    the assertion fails, an error message is written to standard error, which
    is probably not very useful on a Palm Pilot.

    If C Preprocessor symbol NDEBUG is defined, the assertion is factored out
    of the source code with an empty assert() macro.  Because the standard
    requires that "assert.h" can be #include'd multiple times in a source
    file, with NDEBUG #define'd or #undef'ed before each inclusion, there
    is no #if preventing the header file from being processed more than once.
    [The assert() "function" is always defined as a macro.  More full-featured
    "assert.h" files will have guards around declarations of functions, etc.
    referenced in the macro, but not around the actual definition of the macro.]

    For my own convenience, the assertion is disabled if either NDEBUG is
    defined or if HAVE_ASSERT is defined with a value of 0.

*******************************************************************************/

#undef  assert

#undef  abort				/* "sys_socket.h" defines this away,
					   but it is in m68k-palmos's libc. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */

#if defined(NDEBUG) || (defined(HAVE_ASSERT) && !HAVE_ASSERT)

#    define  assert(ignore)  ((void) 0)

#else

#    define  assert(expression)						\
        ((expression)  ||						\
         (fprintf (stderr, "%s:%s:%s: Assertion \"%s\" failed.\n",	\
                   __FILE__, __LINE__, __func__, #expression),		\
          abort ()))

#endif	/* NDEBUG */
