/* $Id: trex.c,v 1.6 2023/05/25 02:44:49 alex Exp alex $ */
/*******************************************************************************

Program:

    trex.c

    Test Regular EXpressions.


Author:    Alex Measday


Purpose:

    TREX tests various bits of code, primarily my REX_UTIL regular
    expression package.


    Invocation:

        % trex [-debug] [-Debug] [-error] [-help]
               [-all] [-benchmark] [-dump] [-environment <string>]
               [-find <find>] [-text <text>] [-replace <replacement>]
               [-henry] [-spencer] [-verify <file>] [-wildcard]
               [<find> [<text> [<replacement>]]]

    where:

        "-debug"
        "-Debug"
            turns debug on.  "-Debug" generates more detailed information
            for testing purposes; exactly what information depends on what
            I'm testing at the time and have manually edited into the code!
        "-error"
            enables error message output to standard error during test
            verification ("-verify").  Normally, such output is disabled
            during verification because some tests are designed to test
            error checking and the resulting error messages are annoying.
        "-help"
            prints out a help message.
        "-all"
        "-max <matches>"
            specify the maximum number of matches or replacements to be made
            in the target string.  The default maximum is 1.
        "-benchmark"
            times the compilation, matching, and replacement stages of
            the program.
        "-dump"
            dumps the compiled regular expression in a readable format to
            standard output.
        "-environment <string>"
            replaces environment variable references in <string> with the
            variables' values.  (Unrelated to regular expressions, I know!)
        "-wildcard"
            indicates that <find> is a wildcard expression that will be
            converted to a regular expression.
        "-find <find>"
        "-text <text>"
        "-replace <replacement>"
            are alternate means of specifying the positional arguments,
            <find>, <text>, and <replacement> below.  In particular, you
            can use these options to change the order of the arguments.
            The example I have in mind is tweaking the regular expression
            to get the desired matching behavior.  Putting the regular
            expression at the end of the line with
                        % trex -text <text> -find <find>
            or just
                        % trex -text <text> <find>
            makes it easier to edit the regular expression repeatedly while
            testing from the command line.  (I use TCSH; hitting cursor-up
            brings up the previous command with the cursor positioned at
            the end of the line.)
        "-henry"
            writes to standard output the internal copy of Henry Spencer's
            test suite.  This includes (i) the test suite modified for my
            regular expression syntax and (ii) the original test suite,
            unmodified.
        "-spencer"
        "-verify <file>"
            perform and verify matching and replacement with regular
            expressions.  If the command-line option is "-spencer",
            an internal copy of Henry Spencer's original test suite,
            modified as necessary for my regular expression syntax,
            is tested.  If the option is "-verify", the tests (in
            Henry Spencer's format) are read from an arbitrary file.
        "<find>"
            is the regular expression to be matched in <text>.
        "<text>"
            is the string in which to search for matches of the regular
            expression, <find>.  If this argument is not specified, the
            program simply checks to see if <find> is a valid regular
            expression (i.e., it compiles).
        "<replacement>"
            specifies what to replace the matched regular expression with
            in <text>.  If this argument is not specified, no substitution
            takes place.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "adx_util.h"			/* Array Descriptor utilities. */
#include  "bmw_util.h"			/* Benchmarking functions. */
#include  "fnm_util.h"			/* Filename utilities. */
#include  "meo_util.h"			/* Memory operations. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "rex_util.h"			/* Regular expression definitions. */
#include  "str_util.h"			/* String manipulation functions. */


#define  MAX_LENGTH  (2*1024)
#define  MAX_LINES  (2*1024)
#define  MAX_SUBEXP  10
#define  MAX_TRANSLATION  (2*1024)
#define  NUM_FIELDS  6
#define  FLD_TEST  0
#define  FLD_FIND  1
#define  FLD_TEXT  2
#define  FLD_STATUS  3
#define  FLD_REPLACE  4
#define  FLD_RESULT  5


/*******************************************************************************
    Private Function
*******************************************************************************/

#ifdef INTERNAL_TESTS
static  void  internalTests (
#    if PROTOTYPES
        void
#    endif
    ) ;
#endif

static  void  mystrenv (
#    if PROTOTYPES
        const  char  *string,
        ssize_t  length,
        char  *translation,
        size_t  maxLength
#    endif
    ) ;

static  void  printFields (
#    if PROTOTYPES
        char  *field[]
#    endif
    ) ;

static  bool  verify (
#    if PROTOTYPES
        size_t  line,
        const  char  *test,
        int  maxSubstitutions
#    endif
    ) ;


/*******************************************************************************
    Henry Spencer's Test File - an in-memory copy of Henry Spencer's test
    file, modified to use my regular expression matching and subtitution
    syntax.
*******************************************************************************/

static  const  char  *HenrySpencersTestFile[] = {

#    include  "rex_henry_spencer_tests.h"

    NULL

} ;


/*******************************************************************************
    RFC 822 Email Address Regular Expression - written by Cal Henderson.
*******************************************************************************/

static  const  char  *RFC822EmailAddressRegexp =
#    include  "trex-email-regexp-c.txt"
;

/*******************************************************************************
    TREX's Main Program.
*******************************************************************************/

int  main (

#    if PROTOTYPES
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

{    /* Local variables. */
    BmwClock  clock ;
    bool  all, benchmark, dump, help, substitute, wildcard ;
    char  *argument, *fileName, *find, *newText, *replace, *s, *text ;
    char  translation[MAX_TRANSLATION] ;
    const  char  *operation, *subop ;
    CompiledRE  pattern ;
    int  errflg, option ;
    MatchedRE  match ;
    OptContext  scan ;
    size_t  i ;

    const  char  *matchStart ;
    size_t  matchLength, maxMatches, numMatches ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{Debug}", "{help}",
        "{all}", "{benchmark}", "{dump}",
        "{email}", "{environment:}", "{find:}",
        "{henry}", "{internal}", "{max:}",
        "{replace:}", "{spencer}", "{substitute}",
        "{text:}", "{verify:}", "{wildcard}",
        NULL
    } ;




    aperror_print = 1 ;

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    all = false ;  benchmark = false ;  dump = false ;
    substitute = false ;  wildcard = false ;
    maxMatches = 0 ;
    fileName = NULL ;  match = NULL ;  pattern = NULL ;
    find = NULL ;  replace = NULL ;  text = NULL ;

    operation = NULL ;  subop = NULL ;

    help = false ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;

    while ((option = opt_get (scan, &argument)) && !errflg) {

        switch (option) {
        case 2:				/* "-Debug" */
            adx_util_debug = 1 ;
        case 1:				/* "-debug" */
            rex_util_debug = 1 ;
            break ;
        case 3:				/* "-help" */
            help = true ;
            break ;
        case 4:				/* "-all" */
            all = true ;
            break ;
        case 5:				/* "-benchmark" */
            benchmark = true ;
            break ;
        case 6:				/* "-dump" */
            dump = true ;
            break ;
        case 7:				/* "-email" */
            if (find != NULL) {
                errflg++ ;  break ;
            }
            find = (char *) RFC822EmailAddressRegexp ;
#ifdef NOT_HERE
            if (dump) {
                size_t  length = strlen (RFC822EmailAddressRegexp) ;
                i = 0 ;
                while (length > 0) {
                    printf ("/* %5"PRIuSIZE_T" */\t\"%.*s\"\n",
                            i, (length > 32) ? 32 : (int) length, &find[i]) ;
                    i += 32 ;  length -= (length > 32) ? 32 : length ;
                }
                exit (0) ;
            }
#endif
            break ;
        case 8:				/* "-environment <string>" */
            printf ("Before: \"%s\"\n", argument) ;
            mystrenv (argument, -1, translation, sizeof translation) ;
            printf (" After: \"%s\"\n", translation) ;
            exit (0) ;
            break ;
        case 9:				/* "-find <find>" */
            if (find == NULL)
                find = argument ;
            else
                errflg++ ;
            break ;
        case 10:			/* "-henry" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "henry" ;
            subop = "spencer" ;
            break ;
        case 11:			/* "-internal" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "internal" ;
            subop = "tests" ;
            break ;
        case 12:			/* "-max <matches>" */
            maxMatches = (size_t) strtoul (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 13:			/* "-replace <replacement>" */
            if (replace == NULL)
                replace = argument ;
            else
                errflg++ ;
            break ;
        case 14:			/* "-spencer" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            operation = "verify" ;
            subop = "spencer" ;
            break ;
        case 15:			/* "-substitute" */
            substitute = true ;
            break ;
        case 16:			/* "-text <text>" */
            if (text == NULL)
                text = argument ;
            else
                errflg++ ;
            break ;
        case 17:			/* "-verify <file>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            fileName = argument ;
            operation = "verify" ;
            subop = "file" ;
            break ;
        case 18:			/* "-wildcard" */
            wildcard = true ;
            break ;
        case NONOPT:			/* "<find> <text> <replace>" */
            if (find == NULL)
                find = argument ;
            else if (text == NULL)
                text = argument ;
            else if (replace == NULL)
                replace = argument ;
            else
                errflg++ ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (scan) ;


    if (operation == NULL)  operation = "find" ;
    if (strcmp (operation, "find") == 0) {
        if (find == NULL)  errflg++ ;
        if (text == NULL)
            subop = "compile" ;
        else if (replace == NULL)
            subop = "match" ;
        else if (substitute)
            subop = "substitute" ;
        else
            subop = "replace" ;
    } else if (strcmp (operation, "dump") == 0) {
        if (find == NULL)  errflg++ ;
    }

/*******************************************************************************
    Print help information, if necessary.
*******************************************************************************/

    if (errflg || help) {
        FILE  *helpout = (help) ? stdout : stderr ;
        fprintf (helpout, "Usage:  trex [-debug] [-Debug] [-help]\n") ;
        fprintf (helpout, "             [-all] [-benchmark] [-dump] [-max <matches>] [-wildcard]\n") ;
        fprintf (helpout, "             [-email] [-environment <string>] [-substitute]\n") ;
        fprintf (helpout, "             [-find <find>] [-text <text] [-replace <replacement>]\n") ;
        fprintf (helpout, "             [-henry] [-internal] [-spencer] [-verify <file>]\n") ;
        fprintf (helpout, "             [<find> [<text> [<replacement>]]]\n") ;
        if (!help) {
            fprintf (helpout, "Type \"trex -help\" for more detailed information.\n") ;
            exit (EINVAL) ;
        }
        fprintf (helpout, "\n") ;
        fprintf (helpout, "BASIC TESTS\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "Compile a regular expression, <find>; match it in <text>; and perform\n") ;
        fprintf (helpout, "the replacement specified by <replacement>.  The two trailing arguments\n") ;
        fprintf (helpout, "are optional and dropping the last or both result in the associated\n") ;
        fprintf (helpout, "operations not being performed:\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "    %% trex <find> [<text> [<replacement>]]\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "The 3 arguments can be specified in a different order using the\n") ;
        fprintf (helpout, "\"-find <find>\", \"-text <text>\", and \"-replace <replacement>\"\n") ;
        fprintf (helpout, "options.  This is useful when testing variations of one of the\n") ;
        fprintf (helpout, "arguments: specifying that argument last makes it easier to\n") ;
        fprintf (helpout, "recall the previous command and edit just that argument.\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "TEST SUITE\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "The program incorporates a copy of Henry Spencer's regular expression\n") ;
        fprintf (helpout, "test suite adapted to REX_UTIL's syntax.\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "Dump the internal copy of Henry Spencer's test suite to standard output.\n") ;
        fprintf (helpout, "The output includes (i) the test suite modified for REX_UTIL's RE and\n") ;
        fprintf (helpout, "replacement syntax and (ii) the original test suite, unmodified:\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "    %% trex -henry\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "Perform and verify matching and replacement with regular expressions\n") ;
        fprintf (helpout, "the internal copy of Henry Spencer's test suite.\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "    %% trex -spencer\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "Read a test suite (in Henry Spencer's format with REX_UTIL's syntax) from\n") ;
        fprintf (helpout, "an arbitrary file and perform and verify the RE matching and replacement:\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "    %% trex -verify <file>\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "REGULAR EXPRESSION CONSTRUCTS\n") ;
        fprintf (helpout, "\n") ;
#    define  REX_HELP_LINE(s)  fprintf (helpout, "%s\n", s) ;
#    include  "rex_util_help.h"		/* REX_UTIL help text. */
        exit (EINVAL) ;
    }

/*******************************************************************************
    Compile and match or replace the regular expression in the target text.
*******************************************************************************/

    if (strcmp (operation, "find") == 0) {

        if (wildcard) {
            printf ("Wildcard:\t\"%s\"\n", find) ;
            s = rexWild (find) ;
            if (s == NULL) {
                LGE "trex: Error converting wildcard \"%s\" to regular expression.\nrexWild: ",
                    find) ;
                exit (EINVAL) ;
            }
            find = s ;
        }

        printf ("RegExpr:\t\"%s\"\n", find) ;
#ifdef MEO_DUMP
        printf ("\n") ;
        meoDumpX (NULL, "    ", 0, find, strlen (find)) ;
        printf ("\n") ;
#endif

        bmwStart (&clock) ;

        if (rexCompile (find, &pattern)) {
            LGE "trex: Error compiling regular expression: \"%s\"\nrexCompile: ",
                find) ;
            exit (EINVAL) ;
        }

        bmwStop (&clock) ;
        if (benchmark)  printf ("Compile time:\t%g\n", bmwElapsed (&clock)) ;

        if (dump)  rexDump (stdout, pattern) ;

/*******************************************************************************
    If requested, match the regular expression zero or more times
    in the target text.
*******************************************************************************/

        if (strcmp (subop, "match") == 0) {

            printf ("Text:\t\t\"%s\"\n", text) ;
#ifdef MEO_DUMP
            printf ("\n") ;
            meoDumpX (NULL, "    ", 0, text, strlen (text)) ;
            printf ("\n") ;
#endif

/* Match the regular expression in the text. */

            matchStart = text ;  matchLength = 0 ;
            if (maxMatches == 0)  maxMatches = all ? strlen (text) : 1 ;
            rexCreate (NULL, NULL, &match) ;

            bmwStart (&clock) ;

            for (numMatches = 0 ;  numMatches < maxMatches ;  numMatches++) {

                if (!rexMatch (matchStart, -1, pattern, match,
                               &matchStart, &matchLength)) {
                    break ;		/* No match. */
                }

                if (!benchmark) {
                    printf ("Match #%"PRIuSIZE_T":\t\t\"%.*s\"\n",
                            numMatches + 1, (int) matchLength, matchStart) ;
                    for (i = 1 ;  i <= rexNumGroups (pattern) ;  i++) {
                        const  char  *groupStart ;
                        size_t  groupLength ;

                        rexGetGroup (match, i, &groupStart, &groupLength) ;
                        if (groupStart == NULL) {
                            LGI " Group[%"PRIuSIZE_T"]:\t<null>\n", i) ;
                        } else {
                            LGI " Group[%"PRIuSIZE_T"]:\t\"%.*s\"\n",
                                i, (int) groupLength, groupStart) ;
                        }
                    }
                }

                matchStart += matchLength ;

            }

            bmwStop (&clock) ;
            if (benchmark)  printf ("Match time:\t%g\n", bmwElapsed (&clock)) ;

            printf ("# of matches: %"PRIuSIZE_T"\n", numMatches) ;

        }

/*******************************************************************************
    If requested, perform match and replace on the text.
*******************************************************************************/

        else if (strcmp (subop, "replace") == 0) {

            printf ("\n") ;

            printf ("Text:\t\t\"%s\"\n", text) ;
#ifdef MEO_DUMP
            printf ("\n") ;
            meoDumpX (NULL, "    ", 0, text, strlen (text)) ;
            printf ("\n") ;
#endif

            printf ("Replace:\t\"%s\"\n", replace) ;
#ifdef MEO_DUMP
            printf ("\n") ;
            meoDumpX (NULL, "    ", 0, replace, strlen (replace)) ;
            printf ("\n") ;
#endif

            if (maxMatches == 0)  maxMatches = all ? strlen (text) : 1 ;

            bmwStart (&clock) ;

            if (rexReplace (text, -1, pattern, replace, maxMatches,
                            &newText, &numMatches)) {
                perror ("error replacing text") ;
                exit (errno) ;
            }

            bmwStop (&clock) ;
            if (benchmark)  printf ("Replace time:\t%g\n",
                                    bmwElapsed (&clock)) ;

            printf ("# of substitutions: %"PRIuSIZE_T"\n\n", numMatches) ;

            printf ("New Text:\t\"%s\"\n", newText) ;
#ifdef MEO_DUMP
            printf ("\n") ;
            meoDumpX (NULL, "    ", 0, newText, strlen (newText)) ;
#endif

            free (newText) ;

        }

/*******************************************************************************
    If requested, substitute the capture groups of a matched RE into a source
    string.  The RE is matched against the "-text" string on the command line
    and the values of the full match and of the capture groups are substituted
    into the source string, which may bear no relation to the text string.  In
    short:
        (1) rexReplace() matches the RE against the "-text" string and replaces
            part of the text string with all or part of the matched text.
        (2) rexSubstitute() matches the RE against the "-text" string and then
            substitutes the matched text and fields in the source string.
*******************************************************************************/

        else if (strcmp (subop, "substitute") == 0) {

            printf ("Text:\t\t\"%s\"\n", text) ;
#ifdef MEO_DUMP
            printf ("\n") ;
            meoDumpX (NULL, "    ", 0, text, strlen (text)) ;
            printf ("\n") ;
#endif

            printf ("Source:\t\t\"%s\"\n", replace) ;
#ifdef MEO_DUMP
            printf ("\n") ;
            meoDumpX (NULL, "    ", 0, replace, strlen (replace)) ;
            printf ("\n") ;
#endif

/* Match the regular expression in the text. */

            rexCreate (NULL, NULL, &match) ;

            if (!rexMatch (text, -1, pattern,
                           match, &matchStart, &matchLength)) {
                printf ("Match:\t<none>\n") ;
                exit (0) ;
            }

            printf ("Match:\t\t\"%.*s\"\n", (int) matchLength, matchStart) ;
            for (i = 1 ;  i <= rexNumGroups (pattern) ;  i++) {
                const  char  *groupStart ;
                size_t  groupLength ;

                rexGetGroup (match, i, &groupStart, &groupLength) ;
                if (groupStart == NULL)  continue ;
                LGI " Group[%"PRIuSIZE_T"]:\t\"%.*s\"\n",
                    i, (int) groupLength, groupStart) ;
            }

/* Substitute the matched fields in the source string. */

            if (rexSubstitute (match, replace, -1, &newText)) {
                LGE "(trex) Error substituting match.\nrexSubstitute: ") ;
                exit (errno) ;
            }

            printf ("New:\t\t\"%s\"\n", newText) ;

        }

    }

/*******************************************************************************
    Print to standard output the in-memory copy of Henry Spencer's test file,
    modified for use with my regular expression syntax.
*******************************************************************************/

    else if (strcmp (operation, "henry") == 0) {

        for (i = 0 ;  HenrySpencersTestFile[i] != NULL ;  i++) {
            printf ("%s\n", HenrySpencersTestFile[i]) ;
        }

    }

/*******************************************************************************
    Perform some internal tests; in other words, odds and ends that I think of
    that require hard-code C strings ... and anything else of which I think.
*******************************************************************************/

    else if (strcmp (operation, "internal") == 0) {

        i = 1 ;

        verify (i++,
                "abc\ndef\tHello, abc\ndef World!\ty\t-\t-\t# Test that matching crosses newlines.",
                1) ;

        verify (i++,
                "c(def)g\tabcdefghi\ty\t$u'-$1-$`\tHI-def-ab\t# Test of $` and $'.",
                1) ;

        verify (i++,
                "(1(2)(3))(4(5)(6))(7(8)(9))(10(11)(12)(13(14)(15)))\tabc-123456789101112131415-def\ty\t\\&-\\0-\\1-\\2-\\3-\\4-\\5-\\6-\\7-\\8-\\9-\\10-\\11-\\12-\\13-\\14-\\15-\\16-no-16\t123456789101112131415-123456789101112131415-123-2-3-456-5-6-789-8-9-101112131415-11-12-131415-14-15--no-16",
                1) ;

        verify (i++,
                "(1(2)(3))(4(5)(6))(7(8)(9))(10(11)(12)(13(14)(15)))\tabc-123456789101112131415-def\ty\t$&-$0-$1-$2-$3-$4-$5-$6-$7-$8-$9-$10-$11-$12-$13-$14-$15-$16-no-16\t123456789101112131415-123456789101112131415-123-2-3-456-5-6-789-8-9-101112131415-11-12-131415-14-15--no-16",
                1) ;

        verify (i++,
                "(1(2)(3))(4(5)(6))(7(8)(9))(10(11)(12)(13(14)(15)))\tabc-123456789101112131415-def\ty\t$&-$<0>-$<1>-$<2>-$<3>-$<4>-$<5>-$<6>-$<7>-$<8>-$<9>-$<10>-$<11>-$<12>-$<13>-$<14>-$<15>-$<16>-no-16\t123456789101112131415-123456789101112131415-123-2-3-456-5-6-789-8-9-101112131415-11-12-131415-14-15--no-16",
                1) ;

    }

/*******************************************************************************
    Perform and verify regular-expression matching and substitution tests.
    If the command-line option was "-verify <file>", read the tests from
    a file.  If the option was "-spencer", read the tests from the
    internal copy of Henry Spencer's test file, modified for use with
    my regular-expression matching and substitution syntax.
*******************************************************************************/

    else if (strcmp (operation, "verify") == 0) {

        char  *s, **table ;
        FILE  *file ;
        size_t  numFail, numLines, numPass ;
        static  char  buffer[MAX_LENGTH] ;

        if (strcmp (subop, "spencer") == 0) {	/* Use internal test file. */

            table = (char **) HenrySpencersTestFile ;
            for (numLines = 0 ;  table[numLines] != NULL ;  numLines++)
                /* Count # of lines in file. */ ;

        } else {				/* Use disk file. */

/* Open the input file. */

            if (fileName == NULL) {
                file = stdin ;
            } else {
                fileName = fnmBuild (FnmPath, fileName, NULL) ;
                if (fileName == NULL) {
                    perror ("Error building test file name.\nfnmBuild: ") ;
                    exit (errno) ;
                }
                file = fopen (fileName, "r") ;
                if (file == NULL) {
                    perror ("Error opening test file.\nfopen: ") ;
                    exit (errno) ;
                }
            }

            table = calloc (MAX_LINES, sizeof (char *)) ;
            if (table == NULL) {
                perror ("Error allocating test table.\ncalloc: ") ;
                exit (errno) ;
            }
            numLines = 0 ;

            while (!feof (file)) {
                if (fgets (buffer, sizeof buffer, file) == NULL)  break ;
                strTrimNL (buffer, -1) ;
                s = buffer + strspn (buffer, " \f") ;
                table[numLines] = strdup (s) ;
                if (table[numLines++] == NULL) {
                    perror ("Error duplicating test string.\nstrdup: ") ;
                    exit (errno) ;
                }
            }

            fclose (file) ;

        }

        aperror_print = 0 ;

        numFail = numPass = 0 ;

        for (i = 0 ;  i < numLines ;  i++) {
            s = table[i] ;
            if ((*s == '\0') || (*s == '#') || (*s == '\f'))  continue ;
            if (verify (i + 1, table[i], all ? -1 : 1))
                numPass++ ;
            else
                numFail++ ;
            if (strcmp (subop, "spencer") != 0) {
                free (table[i]) ;  table[i] = NULL ;
            }
        }

        printf ("\n# of tests passed: %"PRIuSIZE_T"  failed: %"PRIuSIZE_T"\n",
                numPass, numFail) ;

        if (strcmp (subop, "spencer") != 0)  free (table) ;

        aperror_print = 1 ;

    }


    if (pattern != NULL)  rexDestroy (pattern, match) ;

    exit (0) ;

}

/*******************************************************************************

    mystrenv() - translates environment variables ("$<name>") and home
    directory references ("~") embedded in a string.

*******************************************************************************/


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  rex_util_debug

static  void  mystrenv (

#    if PROTOTYPES
        const  char  *string,
        ssize_t  length,
        char  *translation,
        size_t  maxLength)
#    else
        string, length, translation, maxLength)

        char  *string ;
        ssize_t  length ;
        char  *translation ;
        size_t  maxLength ;
#    endif

{    /* Local variables. */
    char  follow, *name, *s ;
    size_t  i ;



    if (translation == NULL)  return ;
    if (string == NULL) {
        strcpy (translation, "") ;
        return ;
    }

    if (length < 0)  length = strlen (string) ;
    strncpym (translation, string, length, maxLength) ;

/* Scan through the string, replacing "~"s by the user's home directory and
   environment variables ("$<name>") by their values. */

    for (i = 0 ;  translation[i] != '\0' ;  ) {

        if ((translation[i] == '~') &&			/* "~" */
            ((i == 0) || (translation[i-1] == ':'))) {

            s = getenv ("HOME") ;			/* Get home directory. */
            if (s == NULL) {
                i++ ;					/* Insert "~". */
            } else {
                strRemove (1, i, translation) ;		/* Insert home directory. */
                strInsert (s, -1, i, translation, maxLength) ;
            }

        } else if ((translation[i] == '$') &&		/* "$<name>" */
                   ((i == 0) || !isalnum (INT_UCHAR (translation[i-1])))) {

            name = &translation[i] ;			/* Extract "<name>". */
            name += strspn (name, "$./:[") ;
            length = strcspn (name, "$./:[") ;
            follow = name[length] ;  name[length] = '\0' ;
            s = getenv (name) ;				/* Lookup "<name>". */
            name[length] = follow ;
							/* Replace "$<name>" ... */
            strRemove (name - &translation[i] + length, i, translation) ;
            if (s != NULL)				/* ... by "<value>". */
                strInsert (s, -1, i, translation, maxLength) ;

        } else {					/* Normal character. */

            i++ ;

        }

    }

    return ;

}

/*******************************************************************************
    verify() - runs a single verification test, returning true if the test
        passes and false if it fails.
*******************************************************************************/


static  bool  verify (

#    if PROTOTYPES
        size_t  line,
        const  char  *test,
        int  maxSubstitutions)
#    else
        testNumber, test, maxSubstitutions)

        size_t  line ;
        const  char  *test ;
        int  maxSubstitutions ;
#    endif

{    /* Local variables. */
    char  *field[NUM_FIELDS], *result, *s ;
    CompiledRE  pattern ;
    const  char  *start ;
    MatchedRE  match = NULL ;
    size_t  i, length, testNumber ;
    static  char  buffer[MAX_LENGTH] ;



    printf ("***** Test line %3"PRIuSIZE_T":\n", line) ;
    printf ("***** \"%s\"\n", test) ;

    strncpym (buffer, test, -1, sizeof buffer) ;

/* Parse the 6 fields on the input line. */

    field[FLD_TEST] = buffer ;
    for (i = 1 ;  i < NUM_FIELDS ;  i++) {
        field[i] = strchr (field[i-1], '\t') ;
        if (field[i] == NULL)  break ;
        *(field[i]++) = '\0' ;	/* Field[i] points to the separator between
				   field[i-1] and field[i].  Overwrite the
				   separator with NUL, thus NUL-terminating
				   field[i-1], and increment field[i]'s
				   pointer past the now-NUL character. */
    }

    if (i > 1) {		/* Follow through on the loop and terminate
				   the last field with NUL. */
        s = strchr (field[i-1], '\t') ;
        if (s != NULL)  *s = '\0' ;
    }

    testNumber = (size_t) strtol (field[FLD_TEST], NULL, 0) ;

    if (i < NUM_FIELDS) {
        printf ("***** Test #%3"PRIuSIZE_T": FAILED (insufficient fields)\n",
                testNumber) ;
        return (false) ;
    }

    printFields (field) ;

/* Compile the regular expression. */

    if (rexCompile (field[FLD_FIND], &pattern)) {
        if (*field[FLD_STATUS] == 'c') {
            printf ("***** Test #%3"PRIuSIZE_T": PASSED (did not compile)\n",
                    testNumber) ;
            return (true) ;
        } else {
            LGE "trex: Error compiling regular expression: \"%s\"\nrexCompile: ",
                field[FLD_FIND]) ;
            printf ("***** Test #%3"PRIuSIZE_T": FAILED (should compile)\n",
                    testNumber) ;
            return (false) ;
        }
    } else if (*field[FLD_STATUS] == 'c') {
        printf ("***** Test #%3"PRIuSIZE_T": FAILED (shouldn't compile)\n",
                testNumber) ;
        rexDestroy (pattern, NULL) ;
        return (false) ;
    }

/* Try to match the regular expression in the target string. */

    rexCreate (NULL, NULL, &match) ;

    if (rexMatch (field[FLD_TEXT], -1, pattern,
                  match, &start, &length)) {		/* Matched? */

        printf ("      Test #%3"PRIuSIZE_T": Matched %"PRIuSIZE_T": \"%.*s\"\n",
                testNumber, length, (int) length, start) ;

        if (*field[FLD_STATUS] != 'y') {
            printf ("***** Test #%3"PRIuSIZE_T": FAILED (shouldn't match)\n",
                    testNumber) ;
            rexDestroy (pattern, match) ;
            return (false) ;
        }

    } else {							/* No match. */

        rexDestroy (pattern, match) ;

        if (*field[FLD_STATUS] == 'n') {
            printf ("***** Test #%3"PRIuSIZE_T": PASSED (no match)\n",
                    testNumber) ;
            return (true) ;
        } else if (*field[FLD_STATUS] == 'y') {
            printf ("***** Test #%3"PRIuSIZE_T": FAILED (should match)\n",
                    testNumber) ;
            return (false) ;
        }

    }

/* Substitute the matched text in the fourth field's text and compare the
   result to the fifth field. */

    if (rexSubstitute (match, field[FLD_REPLACE], -1, &result)) {
        printf ("***** Test #%3"PRIuSIZE_T": FAILED (substitution system error)\n",
                testNumber) ;
        rexDestroy (pattern, match) ;
        return (false) ;
    } else {
        rexDestroy (pattern, match) ;
        if (strcmp (result, field[FLD_RESULT]) == 0) {
            printf ("***** Test #%3"PRIuSIZE_T": PASSED (correct substitution)\n",
                    testNumber) ;
            free (result) ;
            return (true) ;
        } else {
            printf ("***** Test #%3"PRIuSIZE_T": FAILED (incorrect substitution: \"%s\")\n",
                    testNumber, result) ;
            free (result) ;
            return (false) ;
        }
    }

}

/*******************************************************************************
    printFields() - prints the test fields from a line.
*******************************************************************************/


static  void  printFields (

#    if PROTOTYPES
        char  *field[])
#    else
        field)

        char  *field[] ;
#    endif

{

    printf ("    TEST#:\t%s\n", field[FLD_TEST]) ;
    printf ("    FIND:\t\"%s\"\n", field[FLD_FIND]) ;
    printf ("    TEXT:\t\"%s\"\n", field[FLD_TEXT]) ;
    printf ("    STATUS:\t\"%s\"\n", field[FLD_STATUS]) ;
    printf ("    REPLACE:\t\"%s\"\n", field[FLD_REPLACE]) ;
    printf ("    RESULT:\t\"%s\"\n", field[FLD_RESULT]) ;

    return ;

}
