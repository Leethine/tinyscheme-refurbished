/* $Id: subtle.c,v 1.5 2021/04/21 12:00:06 alex Exp $ */
/*******************************************************************************

Program:

    subtle.c

    SubRip Text File Scaler (SUBrip tiTLEr).


Author:    Alex Measday


Purpose:

    SUBTLE adjusts the time tags in a SubRip Text (".srt") file in order
    to synchronize the subtitles with the video.  SUBTLE doesn't actually
    deal with the video, but instead requires the user to watch the video
    with subtitles, measure their asynchronicity, and invoke SUBTLE with
    those measurements.

    For example, I usually watch the video with VideoLAN's VLC media player.
    At the same time, I have the SRT file open in a text editor.  In two
    trial-and-error steps:

        (1) I pick one of the earliest subtitles for which the audio is
            clear.  (This is not necessarily the very first subtitle;
            e.g., the first few subtitles may be difficult-to-distinguish
            voices in a crowd.)  I position the video around where the
            subtitle occurs, playback the video, and listen for a
            person to speak the text of the subtitle.  When the person
            begins to speak those words, I pause the video player and
            note the time at which I paused.  I then compare that time
            to the subtitle's start time, the difference between the two
            being the time-shift required to synchronize that subtitle
            with the video's audio track.

        (2) I repeat the first step for one of the last subtitles in the
            video.

    If I'm lucky, the subtitles are already in sync with the video.
    If not, I now have the offset in seconds of the subtitles at the
    beginning of the video and the offset of the subtitles at the end
    of the video.  If a subtitle is displayed before the corresponding
    audio, the offset will be positive (e.g., 5 if the subtitle appears
    5 seconds before the audio).  If the subtitle is delayed, the offset
    will be negative (e.g., -5 if the subtitle appears 5 seconds after
    the audio.

    With the measurements in hand, I run SUBTLE with the beginning and
    ending offsets:

        % subtle -offset <begin>,<end> <SRT file>

    SUBTLE computes the equation of the line connecting the shifted times
    at the beginning and ending subtitles in the video.  (The X-axis is
    the uncorrected subtitle times in seconds; the Y-axis is the shifted
    times in seconds.)  SUBTLE then goes through the list of subtitles,
    substitutes each subtitle's start and stop times in the equation to
    get the shifted times, and, finally, regenerates the SRT file with
    the corrected times so that the subtitles are now synchronized with
    the audio track.

    If the offset is constant throughout the video, SUBTLE can be invoked
    with a single offset:

        % subtle -offset <seconds> <SRT file>

    Going back to the two-offset case, SUBTLE, by default, associates
    the beginning offset with the second subtitle in the file and the
    ending offset with the next-to-last subtitle.  My thinking is that
    sometimes the first subtitle is for information purposes only and
    has no correlation with the audio track.  Likewise for the last
    subtitle.

    More accurate synchronization is possible by, instead, associating
    the offsets with specific subtitles, identified by their sequence
    numbers:

        %subtle -offset <seq#>:<begin>,<seq#>:<end> <SRT file>

    The corrected SRT file is written to standard output unless the
    "-output <file>" option is specified on the command line.


    Invocation:

        % subtle [-debug] [-help]
                 [-apply] [-calculate] [-check]
                 [-fix] [-insert <position>]
                 [-line <slope>,<y-intercept>] [-merge]
                 [-offset [<seq#>:]<seconds>[,[<seq#>:]<seconds>]]
                 [-output <file>] [-renumber <start>]
                 <file1> [<file2>]

    where

        "-debug"
            turns debug on.
        "-help"
            displays help information.
        "-apply"
            is a useless option.  When the "-offset" operation is specified
            alone, SUBTLE (i) calculates the slope and Y-intercept of the line
            connecting the shifted times and (ii) applies the correction to
            all of the subtitles.  If the "-offset" operation is specified
            along with the "-calculate" option, SUBTLE calculates and displays
            the slope and Y-intercept, but the correction is NOT applied to
            the subtitles.  In other words, "-apply" is the default option
            for the "-offset" operation.
        "-calculate"
            displays the slope and Y-intercept calculated for the "-offset"
            operation.  The correction is NOT applied to the subtitles.
            If this option is specified on the command line, the "-offset"
            option must also be specified.
        "-check"
            checks the subtitles for (i) too many text lines, (ii) out-of-order
            sequence numbers, (iii) zero or negative display durations, and
            (iv) display times that overlap or even precede the immediately
            previous subtitles.
        "-fix"
            Not implemented yet.
        "-insert <position>"
            inserts the subtitles from one file into those of another file,
            at the specified position, and outputs the combined list to a
            third file.  This may be useful in its own right at some point,
            but, for now, I use it to test the coherence of positions in
            the SRT_UTIL library package.  Two SRT input files must be
            specified on the command line, either by two file names or by
            one file name and the "-stdin" option.  The combined list of
            subtitles is output to standard output or to the file named in
            a "-output" option.  The subtitles from the first SRT file are
            loaded into a list.  The subtitles from the second SRT file are
            loaded into the same list at position P.  For P in the range
            1..N, where N is the number of subtitles in the first file,
            subtitles P..N from the first file are displaced by the number
            of subtitles in the second file.  If P is 1, the second file's
            subtitles are prepended to the first file's subtitles.  If
            P < 1 (zero or negative) or P > N, the second file's subtitles
            are appended to to the first file's subtitles.
        "-line <slope>,<Y-intercept>"
            specifies a pre-computed equation of the line connecting shifted
            times.  As in the "-offset" operation, the correction is applied
            to the subtitles and the corrected SRT file is written out.
        "-merge"
            merges the subtitles from one file with those of another file
            and outputs the combined list to a third file.  Two SRT input
            files must be specified on the command line, either by two file
            names or by one file name and the "-stdin" option.  The merged
            list of subtitles is output to standard output or to the file
            named in a "-output" option.  Subtitles are pulled from each
            input file one by one: (i) the earliest subtitle is added to
            the merged list, (ii) the merge process advances to the next
            subtitle in the earliest subtitle's list, and (iii) the process
            repeats, comparing the new subtitle's start time with that of
            the not-yet-added subtitle from the other input list.  The
            sequence numbers in the merged list are renumbered before the
            list of subtitles is output.
        "-offset [<seq#>:]<seconds>[,[<seq#>:]<seconds>]"
            specifies the differences in seconds, respectively, between the
            start times of of two subtitles and the corresponding times in
            the audio track.  Ideally, the first subtitle should be early
            in the video and the second subtitle should be late in the video.
            A positive offset, N, means the subtitle appears N seconds before
            the audio, so the subtitle's start time must be adjusted (delayed)
            by adding N seconds to it.  A negative offset, -N, means the
            subtitle appears N seconds after the audio, so the subtitle's
            start time must be adjusted (hastened) by subtracting N seconds
            from it.  SUBTLE calculates the equation of the line connecting
            the shifted times of the two subtitles, applies the correction
            to all of the subtitles, and writes the corrected SRT file to
            standard output (or the "-output" file).  If "-calculate" is also
            specified on the command line, SUBTLE calculates and displays the
            equation, but does not apply the correction to the subtitles.
            By default, the first offset is associated with the second
            subtitle in the SRT file; the second offset is associated with
            the next-to-last subtitle in the file.  An offset can be linked
            to a specific subtitle by specifying the subtitle's sequence
            number, "<seq#>:".
        "-output <file>"
            specifies the name of a file to which output is to be written.
            By default, output is written to standard output.
        "-renumber <start>"
            renumbers the sequence numbers of all the subtitles.  The very
            first subtitle is assigned this option's start number.  The
            sequence number is incremented by 1 for each of the subsequent
            subtitles.  If this option is accompanied by a "-offset" or
            "-line" option, the subtitle times are adjusted before the
            sequence numbers are renumbered.
        "-stdin"
            causes the SRT subtitles to be read from standard input instead
            of a disk file, useful if SUBTLE is used in a command pipeline.
        "<file1>"
            specifies the SRT file to process.
        "<file2>"
            specifies a second SRT file to be used with the "-insert" and
            "-merge" operations.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  <sys/types.h>			/* System type definitions. */
#include  <sys/stat.h>			/* File status definitions. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "srt_util.h"			/* SubRip Text utilities. */
#include  "str_util.h"			/* String manipulation functions. */

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
    bool  help, renumber ;
    char  *argument, *fileName1, *fileName2, *outputFileName, *s ;
    const  char  *operation, *subop ;
    double  offsetBegin, offsetEnd, real, slope, yIntercept ;
    int  errflg, option, present ;
    OptContext  context ;
    ssize_t  position1, position2, renumberStart ;
    ssize_t  sequenceBegin, sequenceEnd ;
    Subtitle  caption1, caption2 ;
    SubtitleList  list ;

#define  PRESENT_SEQB  1
#define  PRESENT_SEQE  2
#define  PRESENT_OFFB  4
#define  PRESENT_OFFE  8
#define  PRESENT_OFF  (PRESENT_OFFB | PRESENT_OFFE)

    static  const  char  *optionList[] = {	/* Command line options. */
        "{debug}", "{help}",
        "{apply}", "{calculate}", "{check}",
        "{fix}", "{insert:}", "{line:}",
        "{merge}", "{offset:}", "{output:}",
        "{renumber:}", "{stdin}",
        NULL
    } ;



    aperror_print = 1 ;


/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    help = false ;  renumber = false ;
    fileName1 = fileName2 = outputFileName = NULL ;
    offsetBegin = offsetEnd = 0.0 ;
    operation = NULL ;  subop = NULL ;
    position1 = position2 = 0 ;
    present = 0 ;  renumberStart = 1 ;
    sequenceBegin = 1 ;  sequenceEnd = -1 ;

    opt_init (argc, argv, NULL, optionList, &context) ;
    errflg = 0 ;

    while ((option = opt_get (context, &argument)) && !errflg) {

        switch (option) {
        case 1:			/* "-debug" */
            srt_util_debug = 1 ;
            break ;
        case 2:			/* "-help" */
            help = true ;
            break ;
        case 3:			/* "-apply" */
            subop = "apply" ;
            break ;
        case 4:			/* "-calculate" */
            subop = "calculate" ;
            break ;
        case 5:			/* "-check" */
            operation = "check" ;
            subop = "check" ;
            break ;
        case 6:			/* "-fix" */
            operation = "check" ;
            subop = "fix" ;
            break ;
        case 7:			/* "-insert <position>" */
            position1 = (ssize_t) strtol (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            operation = "insert" ;
            break ;
        case 8:			/* "-line <slope>,<Y-intercept>" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            slope = strtod (argument, &s) ;
            if (*s == ',') {
                yIntercept = strtod (++s, &s) ;
                if (*s != '\0')  errflg++ ;
            } else {
                errflg++ ;
            }
            operation = "offset" ;
            subop = "line" ;
            break ;
        case 9:			/* "-merge" */
            operation = "merge" ;
            subop = NULL ;
            break ;
        case 10:		/* "-offset [<seq#>:]<seconds>[,[<seq#>:]<seconds>]" */
            if (operation != NULL) {	/* Operation already specified? */
               errflg++ ;  break ;
            }
            present = 0 ;
            real = strtod (argument, &s) ;
            if (*s == ':') {
                sequenceBegin = (ssize_t) real ;
                present |= PRESENT_SEQB ;
                argument = s + 1 ;
                real = strtod (argument, &s) ;
            }
            if (*s == ',') {
                offsetBegin = real ;
                present |= PRESENT_OFFB ;
                argument = s + 1 ;
                real = strtod (argument, &s) ;
            }
            if (*s == ':') {
                sequenceEnd = (ssize_t) real ;
                present |= PRESENT_SEQE ;
                argument = s + 1 ;
                real = strtod (argument, &s) ;
            }
            if (*s == '\0') {
                if (present & PRESENT_OFFB) {
                    offsetEnd = real ;
                    present |= PRESENT_OFFE ;
                } else {
                    offsetBegin = offsetEnd = real ;
                    present |= PRESENT_OFFB | PRESENT_OFFE ;
                }
            } else {
                errflg++ ;
            }
            operation = "offset" ;
            if (subop == NULL)	/* Not set before by "-line" or "-calc"? */
                subop = "apply" ;
            break ;
        case 11:			/* "-output <file>" */
            if (outputFileName == NULL)
                outputFileName = argument ;
            else
                errflg++ ;
            break ;
        case 12:			/* "-renumber <start>" */
            renumberStart = (ssize_t) strtol (argument, &s, 0) ;
            if (*s == '\0')
                renumber = true ;
            else
                errflg++ ;
            break ;
        case 13:			/* "-stdin" */
            if (fileName1 == NULL)
                fileName1 = "/dev/stdin" ;
            else if (fileName2 == NULL)
                fileName2 = "/dev/stdin" ;
            else
                errflg++ ;
            break ;
        case NONOPT:		/* "<file1> [<file2>]" */
            if (fileName1 == NULL)
                fileName1 = argument ;
            else if (fileName2 == NULL)
                fileName2 = argument ;
            else
                errflg++ ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (context) ;


    if ((fileName1 != NULL) && (strcmp (fileName1, "/dev/stdin") == 0))
        fileName1 = NULL ;
    if ((fileName2 != NULL) && (strcmp (fileName2, "/dev/stdin") == 0))
        fileName2 = NULL ;

    if (renumber && (operation == NULL))  operation = "renumber" ;

/*******************************************************************************
    Print help information, if necessary.
*******************************************************************************/

    if (operation == NULL) {
        errflg++ ;	/* No operation specified on the command line. */
    } else if ((strcmp (operation, "insert") == 0) &&
               (fileName1 == NULL) && (fileName2 == NULL)) {
        errflg++ ;	/* No inserting "one" standard input into "another". */
    } else if ((strcmp (operation, "merge") == 0) &&
               (fileName1 == NULL) && (fileName2 == NULL)) {
        errflg++ ;	/* No merging of "two" standard inputs. */
    }

    if (errflg || help) {
        fprintf (stderr, "Usage:  subtle [-debug] [-help]\n") ;
        fprintf (stderr, "               [-apply] [-calculate] [-check]\n") ;
        fprintf (stderr, "               [-fix] [-insert <position>]\n") ;
        fprintf (stderr, "               [-line <slope>,<Y-intercept>] [-merge]\n") ;
        fprintf (stderr, "               [-offset [<seq#>:]<seconds>[,[<seq#>:]<seconds>]]\n") ;
        fprintf (stderr, "               [-output <file>] [-renumber <start>] [-stdin]\n") ;
        fprintf (stderr, "               <file> [<mergeFile>]\n") ;
        if (!help) {
            fprintf (stderr, "\n") ;
            fprintf (stderr, "(Reminder: offsets shift subtitle times relative to the audio track,\n") ;
            fprintf (stderr, "so use positive offsets to delay subtitles appearing before the audio\n") ;
            fprintf (stderr, "and negative offsets to hurry up subtitles appearing after the audio.)\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "Type \"subtle -help\" for more detailed information.\n") ;
            exit (EINVAL) ;
        }
        fprintf (stderr, "\n") ;
        fprintf (stderr, "SUBTLE adjusts the time tags in a SubRip Text (SRT) file in order\n") ;
        fprintf (stderr, "to synchronize the subtitles with a video's audio track.  The user\n") ;
        fprintf (stderr, "must watch the video with subtitles, measure the time offsets from\n") ;
        fprintf (stderr, "the audio for two widely separated subtitles (typically one of the\n") ;
        fprintf (stderr, "first subtitles in the file and one of the last subtitles), and then\n") ;
        fprintf (stderr, "supply SUBTLE with those time offsets.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "A time offset is the number of seconds between a subtitle's start time\n") ;
        fprintf (stderr, "and the audio track's time.  The offset is added to the subtitle's time\n") ;
        fprintf (stderr, "to match the audio track's time:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    A positive offset, N, means the subtitle appears N seconds\n") ;
        fprintf (stderr, "    before the audio; the subtitle's appearance must be delayed\n") ;
        fprintf (stderr, "    by adding N seconds to its time.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    A negative offset, -N, means the subtitle appears N seconds\n") ;
        fprintf (stderr, "    after the audio; the subtitle's appearance must be hastened\n") ;
        fprintf (stderr, "    by subtracting N seconds from its time.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "(Fractional seconds are allowed; for example, 3.14 is an offset of\n") ;
        fprintf (stderr, "3 seconds and 14 centiseconds, otherwise known as 140 milliseconds.)\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "If the offset is constant throughout the video, this command will shift\n") ;
        fprintf (stderr, "the times of all the subtitles by the specified number of seconds:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %% subtle -offset <seconds> <file>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "If the offset is not constant, two offsets must be specified:\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %% subtle -offset <seconds>,<seconds> <file>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "By default, the two offsets belong to the second and the next-to-last\n") ;
        fprintf (stderr, "subtitles in the file.  SUBTLE calculates the equation of the line\n") ;
        fprintf (stderr, "connecting the two shifted times and then uses that equation to adjust\n") ;
        fprintf (stderr, "all of the times in the file.\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "A more accurate equation can be computed by associating the offsets with\n") ;
        fprintf (stderr, "specific subtitles, identified by a sequence number prefixed to each of\n") ;
        fprintf (stderr, "the subtitles' offsets.  (A sequence number is found at the beginning of\n") ;
        fprintf (stderr, "each subtitle in an SRT file.)\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "    %% subtle -offset <seq#>:<seconds>,<seq#>:<seconds> <file>\n") ;
        fprintf (stderr, "\n") ;
        fprintf (stderr, "The subtitles are read from a file or, in the presence of the \"-stdin\"\n") ;
        fprintf (stderr, "option, from standard input.  The corrected, hopefully synchronized\n") ;
        fprintf (stderr, "subtitles are written to standard output unless a \"-output\" file is\n") ;
        fprintf (stderr, "specified.\n") ;

        exit (0) ;
    }

/*******************************************************************************
    Perform "-offset" operations (suboperations "apply", "calculate",
    and "line").
*******************************************************************************/

    if (strcmp (operation, "offset") == 0) {

        srtListCreate (&list) ;
        if (srtListLoad (list, fileName1, -1))  exit (errno) ;

        if (strcmp (subop, "line") == 0) {

			/* Use slope and Y-intercept from the command line. */
            subop = "apply" ;

        } else {
			/* Calculate the slope and Y-intercept. */

            if (present & PRESENT_SEQB) {
                position1 = srtListFind (list, sequenceBegin) ;
            } else {
                position1 = 2 ;
            }
            caption1 = srtListGet (list, position1) ;
            if (caption1 == NULL) {
                SET_ERRNO (EINVAL) ;
                LGE "(subtle) No caption at beginning position %ld.\n",
                    (long) position1) ;
                exit (errno) ;
            }

            if (present & PRESENT_SEQE) {
                position2 = srtListFind (list, sequenceEnd) ;
            } else {
                position2 = -2 ;
            }
            caption2 = srtListGet (list, position2) ;
            if (caption2 == NULL) {
                SET_ERRNO (EINVAL) ;
                LGE "(subtle) No caption at ending position %ld.\n",
                    (long) position2) ;
                exit (errno) ;
            }

            if ((present & PRESENT_OFFB) && !(present & PRESENT_OFFE)) {
			/* For example: "-compute <bseconds>" */
                offsetEnd = offsetBegin ;
            } else if (!(present & PRESENT_OFFB) && (present & PRESENT_OFFE)) {
			/* For example: "-compute ,<eseconds>" */
                offsetBegin = offsetEnd ;
            } else if (!((present & PRESENT_OFF) == PRESENT_OFF)) {
			/* For example: "-compute <seqB>:,<seqE>:" */
                offsetBegin = offsetEnd = 0.0 ;
            }

            srtCalcMXB (caption1, offsetBegin, caption2, offsetEnd,
                        &slope, &yIntercept) ;

        }

        if (strcmp (subop, "calculate") == 0) {

            printf ("Beginning Position: %4ld\n", (long) position1) ;
            printf ("            Offset: %.3f\n", offsetBegin) ;
            printf ("   Ending Position: %4ld\n", (long) position2) ;
            printf ("            Offset: %.3f\n", offsetEnd) ;
            printf ("\n") ;
            printf ("             Slope: %.10g\n", slope) ;
            printf ("       Y-intercept: %.10g\n", yIntercept) ;

        } else if (strcmp (subop, "apply") == 0) {

            srtListApplyMXB (list, slope, yIntercept) ;

            if (renumber)  srtListRenumber (list, renumberStart) ;

            if (srtListSave (list, outputFileName)) {
                LGE "(subtle) Error saving subtitles to \"%s\"\nsrtListSave: ",
                    (outputFileName == NULL) ? "stdout" : outputFileName) ;
                exit (errno) ;
            }

        }

    }

/*******************************************************************************
    Perform "check" operation.
*******************************************************************************/

    else if (strcmp (operation, "check") == 0) {
        char  buffer[128] ;
        int  errorMask ;
        size_t  i ;
        ssize_t  sequenceNum ;

        srtListCreate (&list) ;
        if (srtListLoad (list, fileName1, -1))  exit (errno) ;

        caption1 = NULL ;

        for (i = 1 ;  i <= srtListLength (list) ;  i++) {

            caption2 = srtListGet (list, i) ;

            errorMask = srtCheck (caption1, caption2, 2) ;
            if (errorMask != 0) {
                strcpy (buffer, "") ;
                if (errorMask & SRT_LINE_WARNING)
                    strcat (buffer, "too many lines,") ;
                if (errorMask & SRT_SEQUENCE_WARNING)
                    strcat (buffer, "out of sequence,") ;
                if (errorMask & SRT_DURATION_ERROR)
                    strcat (buffer, "invalid duration,") ;
                if (errorMask & SRT_OVERLAP_ERROR)
                    strcat (buffer, "time overlap,") ;
                if (strlen (buffer) > 0)	/* Erase trailing comma. */
                    buffer[strlen (buffer) - 1] = '\0' ;
                srtGet (caption2, "SEQ#", &sequenceNum) ;
                printf ("Subtitle %4lu (Seq #%4ld):  %s\n",
                        (unsigned long) i, (long) sequenceNum, buffer) ;
            }

            caption1 = caption2 ;

        }

    }

/*******************************************************************************
    Perform "insert" operation.
*******************************************************************************/

    else if (strcmp (operation, "insert") == 0) {

/* Load the first file into a list.  Then load the second file into the
   same list at the position specified on the command line. */

        srtListCreate (&list) ;		/* First of two SRT input files. */
        if (srtListLoad (list, fileName1, -1))  exit (errno) ;

					/* Second of two SRT input files. */
        if (srtListLoad (list, fileName2, position1))  exit (errno) ;

/* Regenerate the sequence numbers and save the combined list to a file. */

        if (renumber)  srtListRenumber (list, renumberStart) ;

        if (srtListSave (list, outputFileName)) {
            LGE "(subtle) Error saving subtitles to \"%s\"\nsrtListSave: ",
                (outputFileName == NULL) ? "stdout" : outputFileName) ;
            exit (errno) ;
        }

    }

/*******************************************************************************
    Perform "merge" operation.
*******************************************************************************/

    else if (strcmp (operation, "merge") == 0) {
        SubtitleList  list1, list2, list3 ;

/* Read and merge the two input lists. */

        srtListCreate (&list1) ;	/* First of two SRT input files. */
        if (srtListLoad (list1, fileName1, -1))  exit (errno) ;

        srtListCreate (&list2) ;	/* Second of two SRT input files. */
        if (srtListLoad (list2, fileName2, -1))  exit (errno) ;

					/* Merge the two lists. */
        list3 = srtListMerge (list1, list2) ;
        if (list3 == NULL)  exit (errno) ;

/* Regenerate the sequence numbers and save the merged list to a file. */

        if (renumber)  srtListRenumber (list3, renumberStart) ;

        if (srtListSave (list3, outputFileName)) {
            LGE "(subtle) Error saving subtitles to \"%s\"\nsrtListSave: ",
                (outputFileName == NULL) ? "stdout" : outputFileName) ;
            exit (errno) ;
        }

    }

/*******************************************************************************
    Perform "renumber" operation.
*******************************************************************************/

    else if (strcmp (operation, "renumber") == 0) {

        srtListCreate (&list) ;
        if (srtListLoad (list, fileName1, -1))  exit (errno) ;

        srtListRenumber (list, renumberStart) ;

        if (srtListSave (list, outputFileName)) {
            LGE "(subtle) Error saving subtitles to \"%s\"\nsrtListSave: ",
                (outputFileName == NULL) ? "stdout" : outputFileName) ;
            exit (errno) ;
        }

    }


    exit (0) ;

}
