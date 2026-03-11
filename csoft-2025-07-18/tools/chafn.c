/* $Id: chafn.c,v 1.13 2025/01/19 18:53:22 alex Exp alex $ */
/*******************************************************************************

Program:

    chafn.c

    Change File Names.


Author:    Alex Measday


Purpose:

    CHAFN renames files using regular-expression-based pattern substitutions
    applied to the file names.

    Below are (i) IMPORTANT command-line options to be aware of and
    (ii) example invocations of CHAFN for different renaming tasks.


    VERY IMPORTANT Command-Line Options
    ===================================

        "-test"
        "-debug"

            If either of these options is present, CHAFN runs through the
            motions of renaming files, but does not actually rename them.
            This is useful for checking what files will be changed and how
            by your command line before running the program to actually
            change the file names.

            WARNING: the "-test" or "-debug" option MUST be specified BEFORE
            any file names.  While scanning the command-line arguments, CHAFN
            renames files as it encounters them, so test/debug mode must be
            enabled before CHAFN gets to the files.

                RIGHT:  % chafn -test ...
                WRONG:  % chafn ... -test

        "-glob"

            If this option is specified--again, like "-test", before any
            file names--CHAFN will treat each file name on the command line
            as a possibly wildcard file specification.  A directory scan for
            matching file names is performed internally and matched files
            are renamed.  If "-glob" is not specified, CHAFN treats each
            file name as the name of a single file.

            This option is generally useful for command shells that do NOT
            perform "globbing" before invoking CHAFN.  Assume a directory
            contains, among other files:

                  "09 - Dig A Pony [Rooftop Concert].mp3"

            In a command shell without globbing,

                WRONG:  % chafn ... *.mp3

            will only match a file whose name is literally '*.mp3'.

                RIGHT:  % chafn -glob ... *.mp3

            will match all the MP3 files, including the example above.

            UNIX-like shells that perform "globbing" pass the matched file
            names as individual command-line arguments to the program.  In
            this case, it's probably a good idea to NOT "-glob".

                RIGHT:  %chafn ... *.mp3

            will pass the names of all the MP3 files to CHAFN, including
            "09 - Dig A Pony [Rooftop Concert].mp3", which CHAFN will
            correctly consider the literal name of a single file.  However:

                WRONG:  %chafn -glob ... *.mp3

            will miss the "Dig A Pony" file because the shell passes
            "09 - Dig A Pony [Rooftop Concert].mp3" to CHAFN and CHAFN's
            internal globbing will treat the "regular expression" square
            brackets in the file name as part of a wildcard file
            specification.  The following is not necessary, but:

                RIGHT:  %chafn -glob ... '*.mp3'

            will work, with CHAFN's internal globbing expanding '*.mp3'.
            (And this is what I had to do before I added the "-glob" option!)


    Regular-Expression Matching and Substitution
    ============================================

    CHAFN's default mode of operation is to perform a regular-expression
    match and replace on each file name.  Therefore, the first two non-option
    arguments are the RE pattern and replacement strings.  Subsequent
    non-option arguments are the file names to be changed.  By default,
    only a single match/replace is performed on a file name; the number
    can be increased with the "-all" or "-max" options:

        % chafn [-all] [-max <substitutions>]
                <pattern> <replacement>
                <file(s)>

    Reminder: the "-test" option, specified before the files, is your friend
    if you don't know what you're doing!

    The syntax for regular expressions and substitution strings is as used
    in the REX_UTIL package and can also be viewed with the "chafn -help"
    command.

    NOTE that backslash marks ("\") in REs may need to be doubled up
    depending on shell quoting conventions.  For example, "\d" in an RE
    matches a decimal digit and must be entered in Unix shells as "\\d".

    The RE match/replace is applied to the entire file name as a string,
    including the extension and version number; e.g., "file.ext.123".

    Do NOT combine this mode of operation with the other file name
    modification options.


    Other File Name Modifications
    =============================

    In contrast to the RE match/replace mode of operation, the other
    file name modification options treat ALL non-option arguments as
    file names to be changed.  If CHAFN detects that a regular expression
    has already been defined when encountering one of these options, it
    will signal an error.  (Some of the other options utilize internal
    RE patterns/replacements and the presence of more than one of these
    options will also result in an error.)

    Case Conversion
    ---------------

    Capitalization is fairly simple and easily fooled by adjacent
    non-alphabetic characters, so double-check the changes after
    they're made.

        % chafn [-capitalize] [-lower] [-upper]
                [-max <substitutions>]
                <file(s)>

    Translate Selected Characters to Other Characters
    -------------------------------------------------

    The translation string is a sequence of paired characters in which
    the first in a pair is the character to be translated and the second
    is the replacement character.  For example, "-xlat abcdef" will
    convert every 'a' to 'b', every 'c' to 'd', and every 'e' to 'f'.

        % chafn -xlat <translation>
                <file(s)>

    Convert Square/Curly Brackets to Parentheses
    ---------------------------------------------

    A shortcut for "-xlat '[(]){(})'".

        % chafn -brackets
                <file(s)>

    Smart Characters Converted to Normal ASCII
    ------------------------------------------

    For converting characters such as smart/curly single and double
    quotes to their straight ASCII equivalents.

        % chafn -smart
                <file(s)>

    Insert Prefix/Append Suffix
    ---------------------------

        % chafn [-prefix <text>] [-suffix <text>]
                <file(s)>

    Append/Replace File Version Number, ".%03d"
    -------------------------------------------

    VAX/VMS files had/have version numbers appended to a file name
    after a semicolon, e.g., "info.txt;1", "info.txt;2", etc.  The
    oldest version is version 1 and succeeding versions are increasingly
    more recent.  A version-less file name, e.g., "info.txt", refers to
    the most recent version.  Inspired by VAX/VMS, I long ago implemented
    a similar scheme under Unix (and Cygwin on Windows) in which the
    latest version of a file has no version number and earlier versions
    have 3-digit versions beginning with 001; e.g., "info.txt.001",
    "info.txt.002", ..., "info.txt".  The shell script I use to
    invoke my text editor automatically makes a backup version of
    the file.

    CHAFN can append or replace a file's version number on both VMS
    and non-VMS systems.  The version number is incremented after each
    file name to which it is applied; the default increment is 1.

        % chafn [-increment <n>] -version <number>
                <file(s)>


    Audio File Name Modification
    ============================

    I frequently work with MP3 files and I have a personal preference for
    audio file names:

        "<track> - <title>.mp3"
        "<track> - <title> - <artist>.mp3"

    Audio files obtained from other people generally follow similar
    but not always identical conventions, so CHAFN has some name
    modification options to convert these to my obviously correct
    convention.

    File names that have only a space between the track and song title
    (e.g., "12 Get Back.mp3") are converted to my convention with the
    "-yphen" option (e.g., "12 - Get Back.mp3").

    File names that have a tight hyphen between the track and song title
    (e.g., "12-Get Back.mp3") are converted to my convention with the
    "-Yphen" option (e.g., "12 - Get Back.mp3").

    File names with ". " between the track and song title (e.g.,
    "12. Get Back.mp3") are converted to my convention with the
    "-dot" option (e.g., "12 - Get Back.mp3").

    If I have file names of the form, "<track> - <artist> - <title>.mp3",
    I usually swap the artist and title with the "-swap .mp3" option.
    I don't use this option much anymore since, if the audio files have
    ID3 tags, TAG311 can rename the files using the information in the tags.

    CHAFN can prepend a track number (e.g., "12 - ") to an audio file name.
    The minimum field width is 2, so track numbers 1-9 have a leading zero;
    for example, "-track 7" produces "07 - " in the file name.  The track
    number is incremented after each file name to which it is applied;
    the default increment is 1.

        % chafn [-increment <n>] -track <number>
                <file(s)>

    If the file name already has a track number, you will get two track
    numbers, e.g., "34 - 12 - Hello, Goodbye.mp3".  The earlier track
    number, " - 12", can be deleted with an RE match/replace:

        % chafn ' - \\d\\d' "" <file(s)>


    Invocation
    ==========

        % chafn [-debug] [-test] [-help]
                [-change] [-full] [-glob]
                [-all] [-max <substitutions>]
                [-brackets] [-xlat <translation>]
                [-dot] [-hyphens] [-smart] [-spaces] [-underscores]
                [-capitalize] [-lower] [-upper]
                [-prefix <text>] [-suffix <text>]
                [-swap <extension>] [-yphen] [-Yphen]
                [-increment <number>] [-track <number>] [-version <number>]
                [<pattern> <replacement>] <file(s)>

    where

        "-debug"
        "-test"
            puts the program in debug/test mode.  In either of these modes,
            the program does NOT rename the files.  This is useful if you
            want to check beforehand that your invocation works the way
            you expect.  In "-test" mode, the program generates the normal
            output.  In "-debug" mode, the program generates additional
            debug messages.
        "-help"
            displays help information.
        "-change"
            begins the new specification of a file name change.  This option
            allows you to specify a file name change for one set of files and
            then a different file name change for another set of files.  The
            option is followed by a name change specification (e.g., pattern
            and replacement text or other options) and the files to which the
            name change is to be applied.  (There is an implied "-change" at
            the beginning of the command line.)
        "-full"
            specifies if full path names should be printed.  After each
            rename operation, the old and new file names are printed to
            the user's terminal.  By default, only the file name, extension,
            and version are printed.  With the "-full" option, the full path
            names of the old and new files are printed.
        "-glob"
            causes file name arguments to be interpreted internally as
            possible wildcard file names.  See the description above at
            the beginning of this file's prologue.
        "-all"
        "-max <substitutions>"
            specify the maximum number of substitutions to perform.
            The default maximum is 1 for the usual pattern/replacement
            substitutions.  The "-all" option causes global replacement.
            For case conversions, the default is to apply the conversion
            globally to the file name; this can be altered by specifying
            the "-max" option *after* the case-conversion option.
        "-brackets"
            converts square and curly brackets ("[]" and "{}") to parentheses.
            I occasionally work with MP3 files that have square brackets in
            their names and I like to convert them to parentheses.  This
            translation is done BEFORE the regular expression, if any, is
            applied.  (This option conflicts with the "-xlat" option, so
            CHAFN signals an error if both are used in the same command.)
        "-capitalize"
        "-lower"
        "-upper"
            causes alphabetic characters in file names to be capitalized,
            converted to lower-case, or converted to upper-case, respectively.
            CHAFN uses the C Library's character types functions/macros
            (in <ctype.h>), so these conversions probably are limited to
            ASCII characters or a small superset of ASCII.  (Capitalization
            employs a simple regular expression, so be sure to check the
            results for incorrect capitalization.)
        "-dot"
            changes a period followed by a space to " - ".  For example,
            "10. One After 09.mp3" would be renamed as "10 - One After 09.mp3".
        "-hyphens"
            replaces all hyphens ("-") in the file name with spaces.
        "-increment <number>"
            is the increment added to track or version numbers when "-track"
            or "-version" are applied to multiple files.  The default increment
            is 1.
        "-prefix <text>"
            inserts the text at the beginning of each file name.  (This is just
            shorthand for the pattern, "^", and the replacement text, "<text>".)
        "-smart"
            converts "smart" UTF-8 code sequences and Microsoft characters
            into their normal ASCII equivalents.  For example, left and
            right curly quotes are converted to straight ASCII quotes.
            This conversion is done BEFORE the regular expression, if any,
            is applied.
        "-spaces"
            replaces all spaces in the file name with hyphens ("-").
        "-suffix <text>"
            appends the text to each file name.  (This is just shorthand for
            the pattern, "$", and the replacement text, "<text>".)
        "-swap <extension>"
            swaps the second and third hyphen-separated fields in a file name.
            The file extension may or may not include the period (e.g., both
            ".mp3" and "mp3" are accepted).  I frequently use this for music
            file names of the form:
                "<track> - <artist> - <title>.mp3"
            The result is:
                "<track> - <title> - <artist>.mp3"
        "-track <number>"
            inserts a track number, separated by a hyphen, at the beginning of
            each file name.  A leading zero is added for numbers less than ten.
            Examples: "01 - ", "02 - ", ..., "10 - ", "11 - ", etc.  When
            this option is applied to multiple files, the track number is
            incremented by 1 after each file (as in the example above).  An
            increment other than 1 can be specified with the "-increment"
            option.
        "-underscores"
            replaces all underscores ("_") in the file name with spaces.
        "-version <number>"
            replaces the 3-digit version number at the end of a file name with
            a new version number.  If the file name has no version number, the
            new version number is appended to the file name.  If multiple file
            names are being processed, the version number is incremented after
            each file: ".023", ".024", ".025", etc.  An increment other than
            the default, 1, can be specified with the "-increment" option.
            Under OpenVMS, version numbers are separated from the file name
            by a semi-colon instead of a period; e.g., ";023".
        "-xlat <translation>"
            translates selected characters to other characters.  The argument
            is a string of character pairs, the first character in a pair
            being a character to be translated and the second being the
            replacement character.  For example, "-xlat abcdef" converts
            any 'a' in a file name to 'b', any 'c' to 'd', and any 'e' to 'f'.
            (This option conflicts with the "-brackets" option, so CHAFN
            signals an error if both are used in the same command.)
        "-yphen"
            replaces the first space in a file name by " - " (space, 'yphen,
            space).  Useful for MP3 files named "<track> <title>.mp3".
        "-Yphen"
            replaces the first hyphen in a file name by " - " (space, 'Yphen,
            space).  Useful for MP3 files named "<track>-<title>.mp3".
        "<pattern>"
            is a regular expression used to match the text to be replaced.
        "<replacement>"
            is the replacement text.
        "<file(s)>"
            specifies the file(s) to be renamed.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  <sys/types.h>			/* System type definitions. */
#include  <sys/stat.h>			/* File status definitions. */
#include  "drs_util.h"			/* Directory scanning utilities. */
#include  "fnm_util.h"			/* Filename utilities. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "rex_util.h"			/* Regular expression definitions. */
#include  "sdx_util.h"			/* String Descriptor utilities. */
#include  "str_util.h"			/* String manipulation functions. */


static  int  debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  debug

/*******************************************************************************
    ToDo - a structure containing the changes to be applied to a file name.
        I originally passed the changes as individual arguments to
        renameFiles(), but that began to be unwieldy.

    <desmart>
        specifies whether (true) or not (false) Microsoft and UTF-8
        "smart" characters are to be converted to their normal ASCII
        equivalents.  For example, left and right curly quotes are
        converted to straight ASCII quotes.
    <translation>
        is a table of character-to-character translations to be made
        on a file name.  If this argument is NULL, no translations
        are performed.
    <pattern>
        is the compiled regular expression specifying the text to be
        matched in the file names.  If this argument is NULL, the
        regular expression search/replace is not applied to the file
        names.
    <replacement>		- I
        is the text to replace the matched patterns in the file names.
    <maxSubstitutions>
        specifies the maximum number (0..N) of substitutions that are
        to be made in the source string.  A value of -1 causes global
        substitutions in the source string.  Substitutions are not
        recursive; the search for the next occurrence of the search
        string in the source string begins following the end of the
        last match.
    <increment>
        is the increment added to the track and/or version numbers
        that follow.  A value of zero results in no increment and
        you take your chances with a negative increment.  NOTE that
        the increment is used for both the track number and the
        version number, so you probably want to avoid simultaneously
        changing both.
    <track>
        on input, <track> specifies an initial track number if a track
        number is to be prepended to the file name.  If the file
        specification matches multiple files, the initial track number
        will be prepended to the first file name and incremented for
        subsequent file names.  The incremented track number is returned
        to the calling program.  An input track number of -1 indicates
        that no track number is to be prepended to the file name(s).
    <version>
        on input, <version> specifies an initial version number for a
        file name.  If the file specification matches multiple files,
        the initial version number will be attached to the first file
        name and incremented for subsequent file names.  The incremented
        version number is returned to the calling program.

*******************************************************************************/

typedef  struct  ToDo {
    bool  desmart ;
    char  *translation ;
    CompiledRE  pattern ;
    char  *replacement ;
    int  maxSubstitutions ;	/* -1 = global substitution. */
    int  increment ;		/* Used for both <tracK> and <version>. */
    int  track ;		/* Is incremented by renameFiles() when used. */
    int  version ;		/* Is incremented by renameFiles() when used. */
}  ToDo ;

/*******************************************************************************
    Private Functions
*******************************************************************************/

static  char  *desmartFileName (
#    if PROTOTYPES
        char  *fileName,
        ssize_t  length
#    endif
    ) ;

static  errno_t  renameFiles (
#    if PROTOTYPES
        ToDo  *todo,
        bool  testOnly,
        bool  glob,
        const  char  *fileSpec,
        bool  fullPath
#    endif
    ) ;

/*******************************************************************************
    Main Program.
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
    bool  changesMade, full, glob, help, patrep, testOnly ;
    char  *argument, buffer[128] ;
    int  errflg, option ;
    OptContext  context ;
    ToDo  todo ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{test}", "{help}",
        "{all}", "{brackets}", "{capitalize}", "{change}",
        "{dot}", "{full}", "{glob}", "{hyphens}",
        "{increment:}", "{lower}", "{max:}", "{prefix:}",
        "{smart}", "{spaces}", "{suffix:}", "{swap:}",
        "{track:}", "{underscores}", "{upper}",
        "{version:}", "{xlat:}", "{yphen}", "{Yphen}",
        NULL
    } ;




    aperror_print = 1 ;


/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    todo.desmart = false ;
    todo.translation = NULL ;
    todo.pattern = NULL ;
    todo.replacement = NULL ;
    todo.maxSubstitutions = 1 ;
    todo.increment = 1 ;
    todo.track = -1 ;
    todo.version = -1 ;

    changesMade = false ;  full = false ;  glob = false ;
    help = false ;  testOnly = false ;
    patrep = true ;	/* Expecting pattern/replacment on command line. */

    opt_init (argc, argv, NULL, optionList, &context) ;
    errflg = 0 ;

    while ((option = opt_get (context, &argument)) && !errflg) {

        switch (option) {
        case 1:			/* "-debug" */
            drs_util_debug = 1 ;
            rex_util_debug = 1 ;
        case 2:			/* "-test" */
            debug = 1 ;
            testOnly = true ;
            break ;
        case 3:			/* "-help" */
            help = true ;
            break ;
        case 4:			/* "-all" */
            todo.maxSubstitutions = -1 ;
            break ;
        case 5:			/* "-brackets" */
            if (todo.translation == NULL) {
                todo.translation = xltable1 (NULL, "[(]){(})") ;
            } else {
                errflg++ ;
                LGE "chafn: -brackets conflicts with preceding -brackets or -xlat\n") ;
            }
            patrep = false ;
            break ;
        case 6:			/* "-capitalize" */
            if ((todo.pattern != NULL) ||
                rexCompile ("(['.]|[[:alpha:]])([[:alpha:]]*)",
                            &todo.pattern)) {
                errflg++ ;
            }
            todo.replacement = "\\u1\\l2" ;
            todo.maxSubstitutions = -1 ;
            patrep = false ;
            break ;
        case 7:			/* "-change" */
            todo.desmart = false ;
            todo.pattern = NULL ;
            todo.replacement = NULL ;
            patrep = true ;
            break ;
        case 8:			/* "-dot" */
            if ((todo.pattern != NULL) || rexCompile ("\\. ", &todo.pattern)) {
                errflg++ ;
            }
            todo.replacement = " - " ;
            patrep = false ;
            break ;
        case 9:			/* "-full" */
            full = true ;
            break ;
        case 10:		/* "-glob" */
            glob = true ;
            break ;
        case 11:		/* "-hyphens" */
            if ((todo.pattern != NULL) || rexCompile ("-", &todo.pattern))
                errflg++ ;
            todo.replacement = " " ;
            todo.maxSubstitutions = -1 ;
            patrep = false ;
            break ;
        case 12:		/* "-increment <number>" */
            todo.increment = atoi (argument) ;
            break ;
        case 13:		/* "-lower" */
            if ((todo.pattern != NULL) || rexCompile ("^.*$", &todo.pattern))
                errflg++ ;
            todo.replacement = "\\l&" ;
            patrep = false ;
            break ;
        case 14:		/* "-max <substitutions>" */
            todo.maxSubstitutions = atoi (argument) ;
            break ;
        case 15:		/* "-prefix <text>" */
            if ((todo.pattern != NULL) || rexCompile ("^", &todo.pattern))
                errflg++ ;
            todo.replacement = argument ;
            patrep = false ;
            break ;
        case 16:		/* "-smart" */
            todo.desmart = true ;
            patrep = false ;
            break ;
        case 17:		/* "-spaces" */
            if ((todo.pattern != NULL) || rexCompile (" ", &todo.pattern))
                errflg++ ;
            todo.replacement = "-" ;
            todo.maxSubstitutions = -1 ;
            patrep = false ;
            break ;
        case 18:		/* "-suffix <text>" */
            if ((todo.pattern != NULL) || rexCompile ("$", &todo.pattern))
                errflg++ ;
            todo.replacement = argument ;
            patrep = false ;
            break ;
        case 19:		/* "-swap <extension>" */
            strlcpy (buffer, " - ([^-]*) - (.*)(",
                     sizeof buffer) ;
            if (argument[0] != '.')
                strlcat (buffer, ".", sizeof buffer) ;
            strlcat (buffer, argument, sizeof buffer) ;
            strlcat (buffer, ")", sizeof buffer) ;
            if ((todo.pattern != NULL) || rexCompile (buffer, &todo.pattern))
                errflg++ ;
            todo.replacement = " - $2 - $1$3" ;
            patrep = false ;
            break ;
        case 20:		/* "-track <number>" */
            if ((todo.pattern != NULL) || rexCompile ("^", &todo.pattern))
                errflg++ ;
            todo.replacement = "99 - " ;		/* Dummy non-NULL value. */
            todo.track = atoi (argument) ;
            patrep = false ;
            break ;
        case 21:		/* "-underscores" */
            if ((todo.pattern != NULL) || rexCompile ("_", &todo.pattern))
                errflg++ ;
            todo.replacement = " " ;
            todo.maxSubstitutions = -1 ;
            patrep = false ;
            break ;
        case 22:		/* "-upper" */
            if ((todo.pattern != NULL) || rexCompile ("^.*$", &todo.pattern))
                errflg++ ;
            todo.replacement = "\\u&" ;
            patrep = false ;
            break ;
        case 23:		/* "-version <number>" */
            if ((todo.pattern != NULL) || rexCompile ("$", &todo.pattern))
                errflg++ ;
            todo.replacement = "" ;			/* Dummy non-NULL value. */
            todo.version = atoi (argument) ;
            patrep = false ;
            break ;
        case 24:		/* "-xlat <translation>" */
            if (todo.translation == NULL) {
                todo.translation = xltable1 (NULL, argument) ;
            } else {
                errflg++ ;
                LGE "chafn: -xlat conflicts with preceding -brackets or -xlat\n") ;
            }
            patrep = false ;
            break ;
        case 25:		/* "-yphen" */
            if ((todo.pattern != NULL) || rexCompile (" ", &todo.pattern))
                errflg++ ;
            todo.replacement = " - " ;
            patrep = false ;
            break ;
        case 26:		/* "-Yphen" */
            if ((todo.pattern != NULL) || rexCompile ("\\-", &todo.pattern))
                errflg++ ;
            todo.replacement = " - " ;
            patrep = false ;
            break ;
        case NONOPT:		/* "<pattern> <replacement> <file(s)>" */
            if (patrep && (todo.pattern == NULL)) {
                strConvert (argument) ;
                if (rexCompile (argument, &todo.pattern))  errflg++ ;
            } else if (patrep && (todo.replacement == NULL)) {
                todo.replacement = argument ;
            } else if ((patrep || !patrep) &&
                       (strlen (argument) > 0)) {
                if (renameFiles (&todo, testOnly, glob, argument, full)) {
                    errflg++ ;
                } else {	/* Whether or not files actually renamed. */
                    changesMade = true ;
                }
            } else {
                errflg++ ;
            }
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (context) ;

/*******************************************************************************
    Help text.
*******************************************************************************/

    if (errflg || help || !changesMade) {
        FILE  *helpout = (help) ? stdout : stderr ;
        fprintf (helpout, "Usage:  chafn [-debug] [-test] [-help]\n") ;
        fprintf (helpout, "              [-change] [-full] [-glob]\n") ;
        fprintf (helpout, "              [-all] [-max <substitutions>]\n") ;
        fprintf (helpout, "              [-brackets] [-xlat <translation>]\n") ;
        fprintf (helpout, "              [-dot] [-hyphens] [-smart] [-spaces] [-underscores]\n") ;
        fprintf (helpout, "              [-capitalize] [-lower] [-upper]\n") ;
        fprintf (helpout, "              [-prefix <text>] [-suffix <text>]\n") ;
        fprintf (helpout, "              [-swap <extension>] [-yphen] [-Yphen]\n") ;
        fprintf (helpout, "              [-increment <number>] [-track <number>] [-version <number>]\n") ;
        fprintf (helpout, "              [<pattern> <replacement>] <file(s)>\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "NOTE: Verify the changes being made, without actually changing the\n") ;
        fprintf (helpout, "      file names, using \"-test\" or \"-debug\" BEFORE the file names.\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "NOTE: Modification options MUST be specified BEFORE the file names.\n") ;
        fprintf (helpout, "\n") ;
        fprintf (helpout, "NOTE: Be careful specifying multiple modification options.  They\n") ;
        fprintf (helpout, "      might not interact well.  Use \"-test\" to verify the changes.\n") ;
        if (!help) {
            fprintf (helpout, "\n") ;
            fprintf (helpout, "Type \"chafn -help\" for more detailed information and RegEx syntax.\n") ;
            exit (EINVAL) ;
        }
        fprintf (helpout, "\n") ;
#    define  TEXT_H_LINE(s)  fprintf (helpout, "%s\n", s) ;
#    include  "chafn_help.h"			/* CHAFN help text. */
        fprintf (helpout, "\n") ;
        fprintf (helpout, "REGULAR EXPRESSION CONSTRUCTS\n") ;
        fprintf (helpout, "\n") ;
#    define  REX_HELP_LINE(s)  fprintf (helpout, "%s\n", s) ;
#    include  "rex_util_help.h"			/* REX_UTIL help text. */
    }


    exit (0) ;

}

/*******************************************************************************
    tryStrDesmart() - helper function passed to sdxRetry() in desmartFileName().
*******************************************************************************/

static  errno_t  tryStrDesmart (
#    if PROTOTYPES
        StringDx  descriptor,		/* File name to be de"smart"ed. */
        void *userData)			/* Not used. */
#    else
        descriptor, userData)

        StringDx  descriptor ;
        void *userData ;
#    endif
{
    if (strDesmart (sdxStringZ (descriptor), sdxLength (descriptor),
                    sdxMaxLength (descriptor))) {
        if (errno != ERANGE) {
            LGE "(tryStrDesmart) Error de\"smart\"ting: \"%s\"\nstrDesmart: ",
                sdxStringZ (descriptor)) ;
        }
        PUSH_ERRNO ;  sdxSetLength (descriptor, -1) ;  POP_ERRNO ;
        return (errno) ;
    }
	/* Since the string may have been modified and its length changed
	   by strDesmart(), update the descriptor's length. */
    sdxSetLength (descriptor, -1) ;
    LGI "(tryStrDesmart) \"%s\"\n", sdxStringZ (descriptor)) ;
    return (0) ;
}


/*******************************************************************************
    desmartFileName() - itself.
*******************************************************************************/

char  *desmartFileName (

#    if PROTOTYPES
        char  *fileName,
        ssize_t  length)
#    else
        fileName, length)

        char  *fileName ;
        ssize_t  length ;
#    endif

{    /* Local variables. */
    StringDx  fdx ;



    if (fileName == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(desmartFileName) NULL input file name: ") ;
        return (NULL) ;
    }

/* Create an expanding string for the de"smart"ed file name. */

    if (sdxCreate (fileName, length, SdxVolatile, &fdx)) {
        LGE "(desmartFileName) Error creating string for file name.\nsdxCreate: ") ;
        return (NULL) ;
    }

/* Replace the smart characters in the file name. */

    if (sdxRetry (fdx, 128, tryStrDesmart, NULL)) {
        LGE "(desmartFileName) Error replacing smart characters: \"%s\"\nsdxRetry: ") ;
        PUSH_ERRNO ;  sdxDestroy (fdx) ;  POP_ERRNO ;
        return (NULL) ;
    }

/* Grab the translation string and return it to the caller. */

    sdxOwn (fdx) ;
    fileName = sdxStringZ (fdx) ;
    sdxDestroy (fdx) ;

    return (fileName) ;

}

/*!*****************************************************************************

Procedure:

    renameFiles ()


Purpose:

    Function renameFiles() renames the specified set of files.  If
    the "desmart" flag is true, smart characters in a file name are
    converted to their ASCII equivalents before the regular expression
    search and replace is applied to the file name.

    Since CHAFN is just a tool, renameFiles() doesn't properly free up
    dynamically allocated memory in the event of an error.


    Invocation:

        status = renameFiles (&todo, testOnly, glob, fileSpec, fullPath) ;

    where

        <todo>		- I/O
            is the address of a structure containing the changes to be
            applied to each file name; see the ToDo structure definition
            for the meaning of the fields.  If track and/or version numbers
            are to be changed, renameFiles() also increments those fields
            in the ToDo structure.
        <testOnly>		- I
            specifies if this call is for testing purposes only.  If so,
            renameFiles() goes through the motions of renaming the files,
            including generating the usual verification output, but does
            NOT actually rename the files.  This is useful to test if the
            renaming is as desired before actually renaming the files.
        <glob>			- I
            specifies if the file specification is to be interpreted as a
            wildcard specification or not.  If this argument is false, the
            file specification is treated as the name of a single file.
            If this argument is true, the file specification is treated
            as a possibly wildcard specification.  A directory scan is
            performed and any files whose names are matched by the file
            specification are renamed.  If the file specification has no
            wildcard characters, then zero files or one file in the
            directory will be matched.  If the file specification does
            have wildcard characters, then zero or one or more files can
            be matched.
        <fileSpec>		- I
            is the (possibly wildcard) specification of the file(s) to be
            renamed.
        <fullPath>		- I
            specifies if the full path names of the existing file and the
            renamed file should be printed after each rename operation.
            If this argument is false, only the actual file name,
            extension, and version are printed.
        <status>		- O
            returns the status of renaming the file(s), zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  renameFiles (

#    if PROTOTYPES
        ToDo  *todo,
        bool  testOnly,
        bool  glob,
        const  char  *fileSpec,
        bool  fullPath)
#    else
        todo, testOnly, glob, fileSpec, fullPath)

        ToDo  *todo ;
        bool  testOnly;
        bool  glob ;
        char  *fileSpec ;
        bool  fullPath ;
#    endif

{    /* Local variables. */
    bool  equal ;
    char  *fileName = NULL, *newName, number[32] ;
    char  *oldName, *printNew, *printOld, *s ;
    DirectoryScan  scan ;
    size_t  numSubstitutions ;



/*******************************************************************************
    For each file matched by the old file specification, rename the file using
    the template provided by the new file specification.
*******************************************************************************/

    if (glob) {
        if (drsCreate (fileSpec, &scan))  return (errno) ;
        oldName = (char *) drsFirst (scan) ;
    } else {
        oldName = (char *) fileSpec ;
    }

    while (oldName != NULL) {

/* Dynamically allocate a copy of the file name (excluding the directory).
   If requested, convert "smart" characters in the name to "dumb" characters. */

        if (todo->desmart) {
            s = fnmBuild (FnmFile, oldName, NULL) ;
            if (s == NULL)  return (errno) ;
            fileName = desmartFileName (s, -1) ;
            PUSH_ERRNO ;  free (s) ;  POP_ERRNO ;
            if (fileName == NULL)  return (errno) ;
        } else {
            fileName = fnmBuild (FnmFile, oldName, NULL) ;
            if (fileName == NULL)  return (errno) ;
        }

/* If requested, translate characters in the file name. */

        if (todo->translation != NULL) {
            strxlt (fileName, todo->translation) ;
        }

/* If a music track number is to be prepended to the file name,
   construct the replacement text.  (The pattern passed in should
   be the compiled regular expression, "^".)  The track number is
   then incremented in case the file specification matches multiple
   files. */

        if (todo->track >= 0) {
            sprintf (number, "%02d - ", todo->track) ;
            todo->replacement = number ;
            todo->track += todo->increment ;
        }

/* If a regular expression pattern was specified, then apply the RE to the
   file name, producing a new, dynamically allocated name. */

        if (todo->pattern == NULL) {
            newName = fileName ;
        } else {
            if (rexReplace (fileName, -1, todo->pattern,
                            todo->replacement, todo->maxSubstitutions,
                            &newName, &numSubstitutions)) {
                PUSH_ERRNO ;  free (fileName) ;  POP_ERRNO ;
                return (errno) ;
            }
            free (fileName) ;
        }
        fileName = NULL ;

/* If the version number is to be replaced, then do so.  The version
   number passed into this function is then incremented in case the
   file specification matches multiple files. */

#ifdef VMS
#    define  VERSION_SEPARATOR  ";"
#else
#    define  VERSION_SEPARATOR  "."
#endif

        if (todo->version >= 0) {
            sprintf (number, "%s%03d", VERSION_SEPARATOR, todo->version) ;
            fileName = fnmBuild (FnmFile, number, newName, NULL) ;
            PUSH_ERRNO ;  free (newName) ;  POP_ERRNO ;
            if (fileName == NULL)  return (errno) ;
            newName = fileName ;  fileName = NULL ;
            todo->version += todo->increment ;
        }

/* If the new name is different than the old name, then rename the file. */

        fileName = fnmBuild (FnmFile, oldName, NULL) ;
        if (fileName == NULL) {
            PUSH_ERRNO ;  free (newName) ;  POP_ERRNO ;
            return (errno) ;
        }

        equal = (strcmp (newName, fileName) == 0) ;
        free (fileName) ;  fileName = NULL ;

        if (!equal) {

            s = fnmBuild (FnmDirectory, oldName, NULL) ;
            if (s == NULL) {
                PUSH_ERRNO ;  free (newName) ;  POP_ERRNO ;
                return (errno) ;
            }
            fileName = newName ;
            newName = fnmBuild (FnmPath, fileName, s, NULL) ;
            PUSH_ERRNO ;  free (s) ;  free (fileName) ;  POP_ERRNO ;
            if (newName == NULL)  return (errno) ;

            printOld = fnmBuild (FnmFile, oldName, NULL) ;
            if (printOld == NULL)  return (errno) ;
            printNew = fnmBuild (FnmFile, newName, NULL) ;
            if (printNew == NULL)  return (errno) ;

            if (fullPath)
                printf ("RENAME %s\n    TO %s\n", oldName, newName) ;
            else
                printf ("RENAME %s\n    TO %s\n", printOld, printNew) ;

            free (printOld) ;
            free (printNew) ;

            if (!testOnly && rename (oldName, newName)) {
                LGE "(renameFiles) Error renaming \"%s\" to \"%s\"\nrename: ",
                    oldName, newName) ;
                PUSH_ERRNO ;  free (newName) ;  POP_ERRNO ;
                return (errno) ;
            }

        }

        free (newName) ;

        if (glob)
            oldName = (char *) drsNext (scan) ;
        else
            oldName = NULL ;

    }

    if (glob)  drsDestroy (scan) ;

    return (0) ;

}
