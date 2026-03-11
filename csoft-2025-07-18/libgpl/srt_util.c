/* $Id: srt_util.c,v 1.7 2022/12/02 03:25:29 alex Exp alex $ */
/*******************************************************************************

File:

    srt_util.c

    SubRip Text (SRT) File Utilities


Author:    Alex Measday


Purpose:

    The SRT_UTIL package provides functions for reading, manipulating, and
    saving SubRip Text (SRT) subtitle files.

    An SRT file is a text file containing the subtitles for a video.  Each
    subtitle is in the following format:

        <sequence #>
        HH:MM:SS,LLL --> HH:MM:SS,LLL	-- Beginning and ending display times.
        <first line of subtitle>
        ...
        <last line of subtitle>
					-- Blank line signalling end of record.

    The sequence numbers generally run from 1 to N, where N is the number
    of subtitles in the file.  The beginning and ending display times
    specify how far along in the video the subtitle appears and disappears,
    respectively; the fields in the time are hours, minutes, seconds, and
    milliseconds, using the European convention of a comma for the decimal
    point.  Then comes each line of text in the subtitle, followed by a
    blank line marking the end of the record.  For example:

        172
        00:09:28,340 --> 00:09:30,880
        JOHN: George Harrison,
        the scouse of distinction.
        <blank line>

    Assume an existing SRT file is to be modified.  An application must
    (i) create an empty list of subtitles, (ii) read the subtitles from
    the file and add them to the list, (iii) modify the subtitles as
    desired, and (iv) save the list of modified subtitles to a file:

        SubtitleList  list ;

        srtListCreate (&list) ;				-- Empty list.
        srtListLoad (list, "<input file>", -1) ;	-- Read file.
        ... modify the subtitles in the list ...
        srtListSave (list, "<output file>", -1) ;	-- Write file.
        srtDestroyList (list) ;				-- All done!

    Individual subtitles in a list can be found by sequence number with
    srtListFind() or by position (1..N) with srtListGet().  When chopping
    up a list, the sequence numbers can get out of order; regenerating a
    normal sequence of numbers is done with srtListRenumber().

    In the case of inserting a new subtitle in an existing SRT file or
    adding it to a brand new SRT file, the following example (i) creates
    a new subtitle; (ii) sets its sequence number (1768), beginning and
    ending display times, and lines of text; and (iii) adds it to the
    end of a list like the one above:

        Subtitle  caption ;

        srtCreate (1768, &caption) ;
        srtSetTimes (caption,
                     srtStringToTime ("01:35:38,566"),
                     srtStringToTime ("01:35:40,899")) ;
        srtAddLine (caption, "There's nothing wrong with you", -1) ;
        srtAddLine (caption, "that can't be cured with a", -1) ;
        srtAddLine (caption, "little Prozac and a polo mallet.", -1) ;
        srtListAdd (list, caption, -1) ;

    Attributes of a subtitle can retrieved or modified by name with
    srtGet() and srtSet(), respectively.  A subtitle's display times
    can be accessed with those two functions or, more directly, with
    srtGetTimes() and srtSetTimes().

    SYNCHRONIZING SUBTITLES WITH AUDIO
    ----------------------------------

    I wrote my SUBTLE program and this function package for the purpose
    of synchronizing subtitles with a video's audio track.  The program
    and package assume that the subtitles (i) are already synchronized,
    (ii) have a constant offset from the audio throughout the video, or
    (iii) have an offset that varies linearly with time.  The SRT_UTIL
    package can calculate the equation of the line in the latter two
    cases (and the trivial first case as well).  The user must supply
    the offsets in seconds of two widely separated subtitles, ideally
    from the beginning and end of the video.  The offsets are more or
    less easily measured by watching the video and listening for what's
    said in the subtitle of interest to begin being said.  Pause the
    video immediately and compare the video's timestamp to the subtitle's
    beginning time.  Subtract the subtitle's time from the video's time.
    If the difference (offset) is positive, that means the subtitle
    appears too early and must be delayed the given number of seconds.
    If the difference is negative, that means the subtitle appears too
    late and must be moved up by the (absolute value) number of seconds.

    The following code fragment calculates the equation of the line
    connecting the synchronized (shifted) times of two subtitles:

        Subtitle  caption1, caption2 ;
        double  offset1, offset2, slope, yIntercept ;

        caption1 = srtListGet (list, 2) ;	-- Second subtitle.
        offset1 = 3.75 ;			-- Observed offset.

        caption2 = srtListGet (list, -1) ;	-- Next-to-last subtitle.
        offset1 = 15.0 ;			-- Observed offset.

        srtCalcMXB (caption1, offset 1, caption2, offset2,
                    &slope, yIntercept) ;

    The synchronized beginning and ending times of any subtitle in the
    list can be computed by substituting the unsynchronized times into
    the equation:

              sync-time = (slope * unsync-time) + y-intercept

    In the case of a constant offset throughout the video, srtCalcMXB()
    can be skipped altogether--the slope is simply 1 and the y-intercept
    is the offset.

        NOTE because I get confused myself: srtCalcMXB() works with
        a graph that maps unsynchronized times (along the x-axis) to
        synchronized times (y-axis).  If the subtitles are perfectly
        aligned with the audio, the modeled line would be a 45-degree
        diagonal through the origin (y = 1x + 0 = x).  If the offset
        is constant throughout the video, there would again be a
        45-degree diagonal positioned above or below the origin by
        the offset.

        Confusion sets in for me when I offhandedly visualize the
        graph, as the image that immediately comes to mind is a
        graph of the offsets versus the uncorrected times.  Thus
        I picture a constant offset as a horizontal line parallel
        to the x-axis instead of the 45-degree line whose equation
        srtCalcMXB() actually returns.  Each approach works -- I
        just happened to choose the synchronized times approach
        (which, incidentally, saves one floating-point addition
        when applying the equation to a raw time!).

    All of the subtitles in the list can be corrected with a single call:

        srtListApplyMXB (list, slope, yIntercept) ;


Public Procedures (subtitles):

    srtAddLine() - adds a line of text to a subtitle.
    srtApplyMXB() - adjusts the begin and end display times of a subtitle.
    srtCalcMXB() - find the line connecting the shifted times of two subtitles.
    srtCheck() - checks two adjacent captions for various errors.
    srtCreate() - creates a single subtitle.
    srtDestroy() - destroys a subtitle.
    srtDuplicate() - duplicates a subtitle.
    srtGet() - gets the value of a subtitle attribute.
    srtGetTimes() - gets a subtitle's beginning and ending display times.
    srtReadFile() - reads a subtitle from a disk file.
    srtReadString() - reads a subtitle from a string.
    srtSet() - sets the value of a subtitle attribute.
    srtSetTimes() - sets a subtitle's beginning and ending display times.
    srtStringToTime() - converts a string to a subtitle time.
    srtTimeToString() - converts a subtitle time to a string.

Public Procedures (collections):

    srtListAdd() - adds/inserts a subtitle to/in a list.
    srtListApplyMXB() - adjusts the times of all the subtitles in a list.
    srtListCreate() - creates an empty list of subtitles.
    srtListDelete() - deletes a subtitle from a list.
    srtListDestroy() - destroys a list of subtitles.
    srtListFind() - finds a subtitle by sequence number.
    srtListGet() - retrieves a subtitle by position.
    srtListLength() - returns the number of subtitles in a list.
    srtListLoad() - loads subtitles from a disk file.
    srtListMerge() - merges two lists into a new list.
    srtListRenumber() - renumbers the subtitles in a list.
    srtListSave() - saves subtitles to a disk file.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#if HAVE_STDARG_H
#    include  <stdarg.h>		/* Variable-length argument lists. */
#else
#    include  <varargs.h>		/* Variable-length argument lists. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "fnm_util.h"			/* Filename utilities. */
#include  "meo_util.h"			/* Memory operations. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */
#include  "srt_util.h"			/* SubRip Text utilities. */


/*******************************************************************************
    SubRip Text Data Structures.
*******************************************************************************/

#define  MAX_SUBTITLE_LINES  16L

typedef  struct  _Subtitle {
    ssize_t  sequenceNum ;		/* Sequence # of subtitle. */
    struct  timeval  beginTime ;	/* Begin and end display times. */
    struct  timeval  endTime ;
    size_t  numLines ;			/* # of lines of text to display. */
    char  *text[MAX_SUBTITLE_LINES] ;	/* The text. */
}  _Subtitle ;

typedef  struct  _SubtitleList {
    size_t  maxSubtitles ;		/* Maximum # currently allocated. */
    size_t  numSubtitles ;		/* Actual # currently used. */
    Subtitle  *subtitles ;		/* Array of subtitles. */
}  _SubtitleList ;


int  srt_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  srt_util_debug

/*!*****************************************************************************

Procedure:

    srtAddLine ()

    Add a Line to a Subtitle's Text.


Purpose:

    Function srtAddLine() adds a new line of text to a subtitle.  A copy
    of the text is added to the subtitle, so the caller can reuse its
    storage of the text.


    Invocation:

        status = srtAddLine (caption, text, length) ;

    where

        <caption>	- I
            is the subtitle handle returned by srt_Create().
        <text>		- I
            is the line of text to add to the subtitle.  A dynamically
            allocated copy of the line is made; it will be deallocated
            by srtDestroy() or by certain calls to srtGet().  If this
            argument is NULL, a blank line (zero-length string) is added.
        <length>	- I
            is the length of the string to be added.  If this argument is
            less than zero, srtAddLine() will determine the length of the
            string itself.
        <status>	- O
            returns the status of adding the line of text to the subtitle,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtAddLine (

#    if PROTOTYPES
        Subtitle  caption,
        const  char  *text,
        ssize_t  length)
#    else
        caption, text, length)

        Subtitle  caption ;
        const  char  *text ;
        ssize_t  length ;
#    endif

{

    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtAddLine) NULL subtitle.\n") ;
        return (errno) ;
    }

    if (caption->numLines >= MAX_SUBTITLE_LINES) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtAddLine) %ld lines exceeds maximum.\n",
            caption->numLines + 1) ;
        return (errno) ;
    }

/* Add a copy of the line to the text body of the subtitle. */

    if (text == NULL) {
        text = "" ;  length = 0 ;
    } else if (length < 0) {
        length = (ssize_t) strlen (text) ;
    }

    caption->text[caption->numLines] = strndup (text, (size_t) length) ;
    if (caption->text[caption->numLines] == NULL) {
        LGE "(srtAddLine) Error duplicating %ld-character text.\nstrndup: ",
            (long) length) ;
        return (errno) ;
    }

    caption->numLines++ ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtApplyMXB ()

    Adjust the Begin and End Display Times of a Subtitle.


Purpose:

    Function srtApplyMXB() adjusts the times of a subtitle to synchronize
    the subtitle with the audio track, using the slope and y-intercept
    computed by a prior call to srtCalcMXB().

    A steep slope may produce negative times, which srtApplyMXB() clips
    to 00:00:00.000.  This anomaly is unlikely to happen and it suggests
    that a more widely separated pair of subtitles should be passed to
    srtCalcMXB().


    Invocation:

        status = srtApplyMXB (caption, slope, yIntercept) ;

    where

        <caption>	- I
            is a subtitle handle returned by srtCreate().
        <slope>		- I
            is the slope of the line connecting the shifted times of two
            subtitles; see srtCalcMXB().
        <yIntercept>		- I
            is the y-intercept of the line connecting the shifted times
            of the two subtitles; see srtCalcMXB().
        <status>	- O
            returns the status of adjusting the subtitle's times,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtApplyMXB (

#    if PROTOTYPES
        Subtitle  caption,
        double  slope,
        double  yIntercept)
#    else
        caption, slope, yIntercept)

        Subtitle  caption ;
        double  slope ;
        double  yIntercept ;
#    endif

{    /* Local variables. */
    double  adjtime ;



    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtApplyMXB) NULL subtitle.\n") ;
        return (errno) ;
    }

    adjtime = (slope * tvFloat (caption->beginTime)) + yIntercept ;
    if (adjtime < 0.0)  adjtime = 0.0 ;
    caption->beginTime = tvCreateF (adjtime) ;

    adjtime = (slope * tvFloat (caption->endTime)) + yIntercept ;
    if (adjtime < 0.0)  adjtime = 0.0 ;
    caption->endTime = tvCreateF (adjtime) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtCalcMXB ()

    Find the Equation of the Line Between the Shifted Times of Two Subtitles.


Purpose:

    Function srtCalcMXB() calculates the equation, y = mx + b, for the line
    connecting the shifted start times of two subtitles.  The equation can
    be used to interpolate or extrapolate the shifted times for all of the
    other subtitles.

    A graph of the line has (i) start times in seconds along the X-axis and
    (ii) the shifted times (time plus offset) in seconds along the Y-axis.
    So, a subtitle's original beginning/ending times can be substituted into
    the equation to get the corrected times that will, hopefully, synchronize
    the subtitle with the video.

    I usually pick one subtitle near the beginning of the video and a second
    subtitle towards the end of the video.  I then watch the video and, for
    each subtitle, note the difference between when the caption is displayed
    and when someone in the video says what the caption says they say.  (Or,
    more simply, watch the video to hear when the person begins speaking the
    subtitle text.  Then compare the video time to the subtitle time in the
    SRT file.)


    Invocation:

        status = srtCalcMXB (caption1, offset1, caption2, offset2,
                             &slope, &yIntercept) ;

    where

        <caption1>	- I
            is the handle for the first subtitle.
        <offset1>	- I
            is the time offset in seconds between the first subtitle's
            start time and the audio track's time.  The offset is added to
            the subtitle's time to match the audio track's time.  A positive
            offset, N, means the subtitle appears N seconds before the audio;
            the subtitle's appearance must be delayed by adding N seconds
            to its time.  A negative offset, -N, means the subtitle appears
            N seconds after the audio; the subtitle's appearance must be
            hastened by subtracting N seconds from its time.  The offset
            is a floating-point number, so fractions of seconds can be
            specified; e.g., 3.75 for 3 and 3/4 seconds.
        <caption2>	- I
            is the handle for the second subtitle.
        <offset2>	- I
            is the time offset in seconds between the second subtitle's
            start time and the audio track's time.  The offset is added to
            the subtitle's time to match the audio track's time.  A positive
            offset, N, means the subtitle appears N seconds before the audio;
            the subtitle's appearance must be delayed by adding N seconds
            to its time.  A negative offset, -N, means the subtitle appears
            N seconds after the audio; the subtitle's appearance must be
            hastened by subtracting N seconds from its time.  The offset
            is a floating-point number, so fractions of seconds can be
            specified; e.g., 3.14 for 3 seconds and 140 milliseconds.
        <slope>		- O
            returns the slope of the line connecting the two offset times.
        <yIntercept>	- O
            returns the y-intercept of the line.
        <status>	- O
            returns the status of calculating the equation of the line,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtCalcMXB (

#    if PROTOTYPES
        Subtitle  caption1,
        double  offset1,
        Subtitle  caption2,
        double  offset2,
        double  *slope,
        double  *yIntercept)
#    else
        caption1, offset1, caption2, offset2, slope, yIntercept)

        Subtitle  caption1 ;
        double  offset1 ;
        Subtitle  caption2 ;
        double  offset2 ;
        double  *slope ;
        double  *yIntercept ;
#    endif

{    /* Local variables. */
    char  buffer[16] ;
    double  localB, localM, time1, time2 ;



    if ((caption1 == NULL) || (caption2 == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtCalcMXB) NULL subtitle(s).\n") ;
        return (errno) ;
    }

    if (tvCompare (caption1->beginTime, caption2->beginTime) == 0) {
        srtTimeToString (caption1->beginTime, buffer) ;
        SET_ERRNO (EINVAL) ;
        LGE "(srtCalcMXB) Subtitles #%ld and #%ld have the same start time: %s\n",
            (long) caption1->sequenceNum,
            (long) caption2->sequenceNum, buffer) ;
        return (errno) ;
    }

    if (slope == NULL)  slope = &localM ;
    if (yIntercept == NULL)  yIntercept = &localB ;

/* Determine the equation of the line connecting the two offsets. */

    time1 = tvFloat (caption1->beginTime) ;
    time2 = tvFloat (caption2->beginTime) ;

    *slope = ((time2 + offset2) - (time1 + offset1)) / (time2 - time1) ;
    *yIntercept = (time1 + offset1) - (*slope * time1) ;

    srtTimeToString (caption1->beginTime, buffer) ;
    LGI "(srtCalcMXB) Subtitle #%ld: %s  Offset: %g\n",
        (long) caption1->sequenceNum, buffer, offset1) ;
    srtTimeToString (caption2->beginTime, buffer) ;
    LGI "(srtCalcMXB) Subtitle #%ld: %s  Offset: %g  Slope = %g, Y-intercept = %g\n",
        (long) caption2->sequenceNum, buffer, offset2, *slope, *yIntercept) ;


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtCheck ()

    Check a Subtitle for Various Errors and Warnings.


Purpose:

    Function srtCheck() checks a subtitle with respect to the previous
    subtitle for various error and warning conditions.  A bit mask
    specifying the conditions found is returned to the caller.  Zero
    or more bits may be set in the mask and have the following meanings:

        SRT_LINE_WARNING - indicates that the subtitle has more than
            two lines of text.

        SRT_SEQUENCE_WARNING - indicates that the subtitle's sequence
            number is less than or equal to the previous subtitle's
            sequence number.

        SRT_DURATION_ERROR - indicates that the subtitle's ending time is
            less than or equal to its beginning time; i.e., the duration
            is either (i) a meaningless, negative number of seconds or
            (ii) zero seconds -- a veritable blip on the screen.

        SRT_OVERLAP_ERROR - indicates that there is some overlap in
            the display of the preceding and current subtitles; i.e.,
            the subtitle's beginning time is less than the previous
            subtitle's ending time (and may even be less than the
            previous subtitle's beginning time as well).

    Two subtitles are passed into this function: the preceding subtitle, if
    any, and the subtitle being checked.  When checking the first subtitle
    in a file, NULL should be passed in for the preceding subtitle.


    Invocation:

        result = srtCheck (caption1, caption2, lineLimit) ;

    where

        <caption1>	- I
            is the handle for the preceding subtitle.  This argument should
            be NULL when checking the first subtitle in the file.
        <caption2>	- I
            is the handle for the subtitle being checked.
        <lineLimit>	- I
            is the limit on the number of lines of text in the subtitle.
            If the subtitle has more lines of text than the limit, the
            SRT_LINE_WARNING is signalled.  If this argument is zero,
            a default limit of 2 is used.
        <result>	- O
            returns a mask with the bits set for the warning and error
            conditions detected.  A return value of zero indicates that
            the subtitle passes muster.

*******************************************************************************/


int  srtCheck (

#    if PROTOTYPES
        Subtitle  caption1,
        Subtitle  caption2,
        size_t  lineLimit)
#    else
        caption1, caption2, lineLimit)

        Subtitle  caption1 ;
        Subtitle  caption2 ;
        size_t  lineLimit ;
#    endif

{    /* Local variables. */
    int  result ;



    if (caption2 == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtCheck) NULL subtitle.\n") ;
        return (0) ;
    }

/* Check for the warning and error conditions. */

    result = 0 ;

    if (lineLimit == 0)  lineLimit = 2 ;
    if (caption2->numLines > lineLimit)
        result |= SRT_LINE_WARNING ;

    if (tvCompare (caption2->endTime, caption2->beginTime) <= 0)
        result |= SRT_DURATION_ERROR ;

    if (caption1 != NULL) {

        if (caption2->sequenceNum <= caption1->sequenceNum)
            result |= SRT_SEQUENCE_WARNING ;

        if (tvCompare (caption2->beginTime, caption1->endTime) < 0)
            result |= SRT_OVERLAP_ERROR ;

    }


    return (result) ;

}

/*!*****************************************************************************

Procedure:

    srtCreate ()

    Create a Subtitle.


Purpose:

    Function srtCreate() creates a new, empty subtitle.


    Invocation:

        status = srtCreate (sequenceNum, &caption) ;

    where

        <sequenceNum>	- I
            is the sequence number for the subtitle.  Zero can be specified
            if the sequence number is not yet known.
        <caption>	- O
            returns a handle for the new subtitle.  This handle is used for
            accessing the subtitle in subsequent SRT_UTIL calls.
        <status>	- O
            returns the status of creating the subtitle, zero if no errors
            occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  srtCreate (

#    if PROTOTYPES
        ssize_t  sequenceNum,
        Subtitle  *caption)
#    else
        sequenceNum, caption)

        ssize_t  sequenceNum ;
        Subtitle  *caption ;
#    endif

{

    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtCreate) NULL subtitle handle pointer.\n") ;
        return (errno) ;
    }

/* Create and initialize the subtitle. */

    *caption = (Subtitle) malloc (sizeof (_Subtitle)) ;
    if (*caption == NULL) {
        LGE "(srtCreate) Error allocating subtitle.\nmalloc: ") ;
        return (errno) ;
    }

    (*caption)->sequenceNum = sequenceNum ;
    (*caption)->beginTime = tvCreate (0, 0) ;
    (*caption)->endTime = tvCreate (0, 0) ;
    (*caption)->numLines = 0 ;

    LGI "(srtCreate) Created subtitle %p for sequence #%ld.\n",
        (void *) *caption, (long) (*caption)->sequenceNum) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtDestroy ()

    Destroy a Subtitle.


Purpose:

    Function srtDestroy() destroys a previously created subtitle.


    Invocation:

        status = srtDestroy (caption) ;

    where

        <caption>	- I
            is the subtitle handle returned by srtCreate().
        <status>	- O
            returns the status of destroying the subtitle,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtDestroy (

#    if PROTOTYPES
        Subtitle  caption)
#    else
        caption)

        Subtitle  caption ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (caption == NULL)  return (0) ;

    LGI "(srtDestroy) Destroying subtitle %p ...\n", (void *) caption) ;

    i = caption->numLines ;
    while (i-- > 0) {
        if (caption->text[i] != NULL) {
            free (caption->text[i]) ;
            caption->text[i] = NULL ;
        }
    }
    caption->numLines = 0 ;

    free (caption) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtDuplicate ()

    Duplicate a Subtitle.


Purpose:

    Function srtDuplicate() creates a dynamically allocated copy of an
    existing subtitle.  The name was chosen to avoid confusion between
    an srtdup() and strdup(3).


    Invocation:

        duplicate = srtDuplicate (caption) ;

    where

        <caption>	- I
            is the subtitle to be copied.
        <duplicate>	- O
            returns a copy of the input subtitle; NULL is returned in the
            event of an error.  When the duplicate is no longer needed,
            call srtDestroy() to delete it.

*******************************************************************************/


Subtitle  srtDuplicate (

#    if PROTOTYPES
        Subtitle  caption)
#    else
        caption)

        Subtitle  caption ;
#    endif

{    /* Local variables. */
    size_t  i ;
    Subtitle  duplicate ;



    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtDuplicate) NULL subtitle.\n") ;
        return (NULL) ;
    }

    if (srtCreate (caption->sequenceNum, &duplicate)) {
        LGE "(srtDuplicate) Error creating duplicate subtitle.\nsrtCreate: ") ;
        return (NULL) ;
    }

    duplicate->beginTime = caption->beginTime ;
    duplicate->endTime = caption->endTime ;

    for (i = 0 ;  i < caption->numLines ;  i++) {
        if (srtAddLine (duplicate, caption->text[i], -1)) {
            LGE "(srtDuplicate) Error adding text line.\nsrtAddLine: ") ;
            PUSH_ERRNO ;  srtDestroy (duplicate) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

    return (duplicate) ;

}

/*!*****************************************************************************

Procedure:

    srtGet ()

    Get the Value of a Subtitle Attribute.


Purpose:

    Function srtGet() gets the value of one of a subtitle's attributes.
    The "BEGIN" and "END" tags are for completeness' sake; use
    srtGetTimes() instead.  The attributes are retrieved as follows:

        Sequence # -
            ssize_t  sequenceNum ;
            srtGet (caption, "SEQ#", &sequenceNum) ;

        Beginning Display Time -
            struct  timeval  beginTime ;
            srtGet (caption, "BEGIN", &beginTime) ;

        Ending Display Time -
            struct  timeval  endTime ;
            srtGet (caption, "END", &endTime) ;

        # of Lines of Text -
            size_t  numLines ;
            srtGet (caption, "#LINES", &numLines) ;

        Lines of Text -
            char  **text ;
            srtGet (caption, "TEXT", &text) ;
            printf ("1st line: \"%s\"\n", text[0]) ;
            printf ("2nd line: \"%s\"\n", text[1]) ;
            ...


    Invocation:

        status = srtGet (caption, which, ...) ;

    where

        <caption>	- I
            is the subtitle handle returned by srtCreate().
        <which>		- I
            is the name of the desired attribute.  Valid attribute names:
                "SEQ#" - sequence number
                "BEGIN" - beginning display time
                "END" - ending display time
                "#LINES" - number of lines of text
                "TEXT" - lines of text
        <...>		- O
            is a correctly typed pointer to a variable in which the
            attribute's value will be stored.  See the attributes
            and data types in the "Purpose:" section above.
        <status>	- O
            returns the status of getting the subtitle attribute,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtGet (

#    if PROTOTYPES
        Subtitle  caption,
        const  char  *which,
        ...)
#    else
        caption, which, va_alist)

        Subtitle  caption ;
        char  *which ;
        va_dcl
#    endif

{    /* Local variables. */
    va_list  ap ;



    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtGet) NULL subtitle.\n") ;
        return (errno) ;
    }

    if (which == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtGet) NULL attribute name.\n") ;
        return (errno) ;
    }

#if HAVE_STDARG_H
    va_start (ap, which) ;
#else
    va_start (ap) ;
#endif

    if (strcasecmp (which, "SEQ#") == 0) {
        ssize_t  *value = va_arg (ap, ssize_t *) ;

        if (value == NULL) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtGet) NULL value pointer for \"%s\".\n", which) ;
        }
        *value = caption->sequenceNum ;
    }

    else if (strcasecmp (which, "BEGIN") == 0) {
        struct  timeval  *value = va_arg (ap, struct timeval *) ;

        if (value == NULL) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtGet) NULL value pointer for \"%s\".\n", which) ;
        }
        *value = caption->beginTime ;
    }

    else if (strcasecmp (which, "END") == 0) {
        struct  timeval  *value = va_arg (ap, struct timeval *) ;

        if (value == NULL) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtGet) NULL value pointer for \"%s\".\n", which) ;
        }
        *value = caption->endTime ;
    }

    else if (strcasecmp (which, "#LINES") == 0) {
        size_t  *value = va_arg (ap, size_t *) ;

        if (value == NULL) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtGet) NULL value pointer for \"%s\".\n", which) ;
        }
        *value = caption->numLines ;
    }

    else if (strcasecmp (which, "TEXT") == 0) {
        char  ***value = va_arg (ap, char ***) ;

        if (value == NULL) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtGet) NULL value pointer for \"%s\".\n", which) ;
        }
        *value = caption->text ;
    }

    else {
        va_end (ap) ;
        SET_ERRNO (EINVAL) ;
        LGE "(srtGet) Invalid attribute name: \"%s\"\n", which) ;
        return (errno) ;
    }

    va_end (ap) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtGetTimes ()

    Get a Subtitle's Beginning and Ending Display Times.


Purpose:

    Function srtGetTimes() gets the beginning and ending display times
    of a subtitle.


    Invocation:

        status = srtGetTimes (caption, &beginTime, &endTime) ;

    where

        <caption>	- I
            is the subtitle handle returned by srtCreate().
        <beginTime>	- O
            returns the subtitle's beginning display time.
        <endTime>	- O
            returns the subtitle's ending display time.
        <status>	- O
            returns the status of getting the times, zero
            if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtGetTimes (

#    if PROTOTYPES
        Subtitle  caption,
        struct  timeval  *beginTime,
        struct  timeval  *endTime)
#    else
        caption, beginTime, endTime)

        Subtitle  caption ;
        struct  timeval  *beginTime ;
        struct  timeval  *endTime ;
#    endif

{

    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtGetTimes) NULL subtitle.\n") ;
        return (errno) ;
    }

    if ((beginTime == NULL) && (endTime == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtGetTimes) NULL time addresses.\n") ;
        return (errno) ;
    }

    if (beginTime != NULL)  *beginTime = caption->beginTime ;
    if (endTime != NULL)  *endTime = caption->endTime ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtReadFile ()

    Read a Subtitle from a Disk File.


Purpose:

    Function srtReadFile() reads the next subtitle (sequence number, times,
    and text) from an open disk file.  A handle for a dynamically allocated
    subtitle is returned to the calling program, which is then responsible
    for eventually calling srtDestroy() to deallocate the subtitle when it
    is no longer needed.

    Special cases of reaching the end of the file while reading the next
    subtitle:

        EOF before the start of the "next" subtitle - This is a normal
            condition and is signaled to the caller by the return of
            a read-only, empty subtitle with a sequence number of -1.
            The caller should NOT srtDestroy() this read-only, constant
            subtitle.

        EOF in the middle of reading a subtitle - This is an error and
            NULL is returned instead of an incomplete subtitle.

        EOF while reading the text of the subtitle - This is a normal
            condition possibly encountered when reading the last subtitle
            in the file.  A handle for the fully-populated, dynamically
            allocated subtitle is returned to the caller.  The caller
            SHOULD srtDestroy() this subtitle when it is no longer needed.


    Invocation:

        caption = srtReadFile (file) ;

    where

        <file>		- I
            is the "FILE *" handle for an open file.
        <caption>	- O
            returns a handle for the new subtitle; NULL is returned in
            the event of an error.  This handle is used for accessing
            the subtitle in subsequent SRT_UTIL calls.  The caller is
            responsible for invoking srtDestroy() to deallocate the
            subtitle when it's no longer needed.  See the special cases
            of end-of-file listed above for details on (i) EOF before a
            subtitle, (ii) EOF in the middle of reading a subtitle, and
            (iii) EOF terminating the lines of text in the subtitle.

*******************************************************************************/


Subtitle  srtReadFile (

#    if PROTOTYPES
        FILE  *file,
        TagUTF  *tag)
#    else
        file, tag)

        FILE  *file ;
        TagUTF  *tag ;
#    endif

{    /* Local variables. */
    char  *s ;
    Subtitle  caption ;
#define  MAX_INPUT_LINE  2048
    static  char  buffer[MAX_INPUT_LINE] ;

	/* Read-only subtitle indicating end-of-file (sequence # = -1). */
    static  const  _Subtitle  CaptionEOF = { -1, { 0, 0 }, {0, 0 }, 0 } ;



    if (file == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtReadFile) NULL file handle.\n") ;
        return (NULL) ;
    }


/*******************************************************************************
    Skip any leading blank lines.
*******************************************************************************/

    do {

        memset (buffer, 0xAA, sizeof buffer) ;	/* To debug UTF-16 and UTF-32. */

        if (fgets (buffer, sizeof buffer, file) == NULL) {
            if (feof (file))  return ((Subtitle) &CaptionEOF) ;
            if (ferror (file)) {
                SET_ERRNO (EIO) ;
                LGE "(srtReadFile) Error skipping blank lines in file.\nfgets: ") ;
                return (NULL) ;
            }
        }

/* Fgets(3) is thrown for a loop by the 0x00 bytes in UTF-16 and UTF-32 text.
   And, yes, I actually encountered a hopefully rare, UTF-16LE SRT file.
   I need to read chunks of such files in binary, convert the text to UTF-8
   (while making sure the chunk doesn't end with the first unit of a
   two-unit UTF-16 character), search for the different line terminators,
   and return a UTF-8 line of text to the caller.  I may get around to
   doing this someday (or using the ICU library), but, for now, I'll stick
   with UTF-8 and use a text editor to re-encode UTF-16 and UTF-32 files.  */

#define  UTF_8_ONLY  1
#ifdef UTF_8_ONLY

        if (*tag == UTF_NULL_TAG) {
            *tag = utfbom (buffer, (ssize_t) strlen (buffer)) ;
            if (BOM_IS_PRESENT (*tag)) {
                if (IS_UTF_16 (*tag) || IS_UTF_32 (*tag)) {
#ifndef EILSEQ
#    define  EILSEQ  EINVAL
#endif
                    SET_ERRNO (EILSEQ) ;
                    LGE "(srtReadFile) UTF-%d encoding not supported.\n",
                        IS_UTF_16 (*tag) ? 16 : 32) ;
                    return (NULL) ;
                }
                strRemove (UTF_8_BOM_BYTES, 0, buffer) ;
            } else {
                *tag = UTF_8_NULL_TAG ;		/* Assume UTF-8. */
            }
        }

#else

/* Debug output to see what was read.  Presetting all the bytes in the
   buffer to 0xAA with the memset(3) above helps delineate the actual
   bytes that were read. */

printf ("fgets --> strlen() = %lu\n", (unsigned long) strlen (buffer)) ;
meoDumpX (stdout, "    ", 0, buffer, 128) ;

        if (*tag == UTF_NULL_TAG) {
/* strlen() doesn't work for UTF-16 and UTF-32. */
            ssize_t  length = (ssize_t) strlen (buffer) ;
            static  char  utf8[MAX_INPUT_LINE] ;

            *tag = utfbom (buffer, (ssize_t) length) ;

            if (BOM_IS_PRESENT (*tag)) {
                if (IS_UTF_8 (*tag)) {
                    strRemove (UTF_8_BOM_BYTES, 0, buffer) ;
                } else if (IS_UTF_16 (*tag)) {
                    length = utf16to8 (buffer, length, *tag, false,
                                       sizeof utf8, utf8) ;
                    strncpym (buffer, utf8, (size_t) length, sizeof buffer) ;
                } else if (IS_UTF_32 (*tag)) {
                    length = utf32to8 (buffer, length, *tag, false,
                                       sizeof utf8, utf8) ;
                    strncpym (buffer, utf8, (size_t) length, sizeof buffer) ;
                } else {
#ifndef EILSEQ
#    define  EILSEQ  EINVAL
#endif
                    SET_ERRNO (EILSEQ) ;
                    LGE "(srtReadFile) Unrecognized UTF encoding.\n") ;
                    return (NULL) ;
                }
            } else {
                *tag = UTF_8_NULL_TAG ;		/* Assume UTF-8. */
            }

        }
#endif

    } while (strTrim (buffer, -1) == 0) ;


    if (srtCreate (0, &caption)) {
        LGE "(srtReadFile) Error creating subtitle structure.\nsrtCreate: ") ;
        return (NULL) ;
    }


/*******************************************************************************
    The first non-blank line has the sequence number.
*******************************************************************************/

    caption->sequenceNum = (ssize_t) strtol ((const char *) buffer, &s, 10) ;
    if (*s != '\0') {
        LGE "(srtReadFile) Invalid sequence #:; \"%s\"\n", buffer) ;
        PUSH_ERRNO ;  srtDestroy (caption) ;  POP_ERRNO ;
        return (NULL) ;
    }
    LGI "(srtReadFile)     Subtitle #: %ld\n", (long) caption->sequenceNum) ;


/*******************************************************************************
    The next line has the beginning and end times of the subtitle.
*******************************************************************************/

    if (fgets (buffer, sizeof buffer, file) == NULL) {
        if (feof (file)) {
            SET_ERRNO (EINVAL) ;
            LGE "(srtReadFile) Unexpected end-of-file.\nfgets: ") ;
            PUSH_ERRNO ;  srtDestroy (caption) ;  POP_ERRNO ;
            return (NULL) ;
        }
        if (ferror (file)) {
            SET_ERRNO (EIO) ;
            LGE "(srtReadFile) Error reading time range from file.\nfgets: ") ;
            PUSH_ERRNO ;  srtDestroy (caption) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

    if (strTrim (buffer, -1) == 0) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtReadFile) Unexpected blank line; expected begin/end times.\n") ;
        PUSH_ERRNO ;  srtDestroy (caption) ;  POP_ERRNO ;
        return (NULL) ;
    }

    s = buffer + strspn (buffer, " \t") ;  s += strcspn (s, " \t") ;
    s += strspn (s, " \t") ;  s+= strspn (s, "->") ;	/* Skip "-->". */

    caption->beginTime = srtStringToTime (buffer) ;
    caption->endTime = srtStringToTime (s) ;

    LGI "(srtReadFile) Subtitle Times: %ld.%03ld --> %ld.%03ld\n",
        (long) caption->beginTime.tv_sec,
        (long) caption->beginTime.tv_usec / 1000,
        (long) caption->endTime.tv_sec,
        (long) caption->endTime.tv_usec / 1000) ;


/*******************************************************************************
    Read the lines of text for the subtitle.  The end of the subtitle
    is signaled by a blank line or EOF.
*******************************************************************************/

    while (true) {

        if (fgets (buffer, sizeof buffer, file) == NULL) {
            if (feof (file))  return (caption) ;
            if (ferror (file)) {
                SET_ERRNO (EIO) ;
                LGE "(srtReadFile) Error reading text lines in file.\nfgets: ") ;
                PUSH_ERRNO ;  srtDestroy (caption) ;  POP_ERRNO ;
                return (NULL) ;
            }
        }

        if (strTrim (buffer, -1) == 0)  return (caption) ;	/* Blank line? */

        if (srtAddLine (caption, (const char *) buffer, -1)) {
            LGE "(srtReadFile) Error adding text line to subtitle.\nsrtAddLine: ") ;
            PUSH_ERRNO ;  srtDestroy (caption) ;  POP_ERRNO ;
            return (NULL) ;
        }

        LGI "(srtReadFile)  Subtitle Text: [%lu] \"%s\"\n",
            (unsigned long) caption->numLines,
            caption->text[caption->numLines - 1]) ;

    }


    LGI "(srtReadFile) Subtitle handle = %p\n", (void *) caption) ;

    return (caption) ;

}

/*!*****************************************************************************

Procedure:

    srtSet ()

    Set the Value of a Subtitle Attribute.


Purpose:

    Function srtSet() sets the value of one of a subtitle's attributes.
    The "BEGIN" and "END" tags are for completeness' sake; use
    srtSetTimes() instead.  The attributes are set as follows:

        Sequence # -
            ssize_t  sequenceNum = 1234 ;
            srtSet (caption, "SEQ#", sequenceNum) ;

        Beginning Display Time -
            struct  timeval  beginTime = {123, 500000 } ;
            srtSet (caption, "BEGIN", &beginTime) ;

        Ending Display Time -
            struct  timeval  endTime = {124, 750000 } ;
            srtSet (caption, "END", &endTime) ;

        # of Lines of Text -
            size_t  numLines = 2 ;
            srtSet (caption, "#LINES", numLines) ;

        Lines of Text -
            char  *text[2] ;
            text[0] = strdup ("JOHN: George Harrison,") ;
            text[1] = strdup ("the scouse of distinction.") ;
            srtSet (caption, "TEXT", text) ;

    NOTE the special handling of the following attributes:

        "#LINES"
            sets the number of lines of text in the subtitle.  If the new
            number of lines is less than the current number of lines, the
            excess lines are deallocated and set to NULL.

        "TEXT"
            replaces N lines of text in the subtitle, where N is the
            current number of lines in the subtitle.  The existing lines
            are deallocated and replaced by the new, caller-supplied
            lines of text.  These strings of text are assumed to have
            been dynamically allocated by the caller.  Depending on the
            circumstances, the caller may need to set "#LINES" before
            setting "TEXT".

    All of which suggests that, if all the lines of text are being replaced,
    it is much simpler to call srtSet() to set the number of lines to zero
    and then call srtAddLine() for each of the new lines.


    Invocation:

        status = srtSet (caption, which, ...) ;

    where

        <caption>		- I
            is the subtitle handle returned by srtCreate().
        <which>			- I
            specifies the attribute being set.  Valid attribute names:
                "SEQ#" - sequence number
                "BEGIN" - beginning display time
                "END" - ending display time
                "#LINES" - number of lines of text
                "TEXT" - lines of text
        <...>			- I
            is the correctly typed value to be assigned to the attribute.
            See the attributes and data types in the "Purpose:" section
            above.
        <status>		- O
            returns the status of setting the subtitle attribute,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtSet (

#    if PROTOTYPES
        Subtitle  caption,
        const  char  *which,
        ...)
#    else
        caption, which, va_alist)

        Subtitle  caption ;
        char  *which ;
        va_dcl
#    endif

{    /* Local variables. */
    char  **text ;
    size_t  i ;
    va_list  ap ;



    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtSet) NULL subtitle.\n") ;
        return (errno) ;
    }

    if (which == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtSet) NULL attribute name.\n") ;
        return (errno) ;
    }

#if HAVE_STDARG_H
    va_start (ap, which) ;
#else
    va_start (ap) ;
#endif

/*******************************************************************************
    Assign the subtitle a new sequence number.
*******************************************************************************/

    if (strcasecmp (which, "SEQ#") == 0) {

        caption->sequenceNum = va_arg (ap, ssize_t) ;

    }

/*******************************************************************************
    Specify new beginning or ending display times for the subtitle.
*******************************************************************************/

    else if (strcasecmp (which, "BEGIN") == 0) {

        caption->beginTime = va_arg (ap, struct timeval) ;

    } else if (strcasecmp (which, "END") == 0) {

        caption->endTime = va_arg (ap, struct timeval) ;

    }

/*******************************************************************************
    Change the number of lines of text in the subtitle.  If the new number
    of lines is less than the current number of lines, the lines of text
    after the new number of lines are deleted.  (For example, suppose the
    subtitle currently has 5 lines of text.  If this function is called
    to set "#LINES" to 3, then lines 4 and 5 of the text are deleted.)
*******************************************************************************/

    else if (strcasecmp (which, "#LINES") == 0) {
        size_t  numLines = va_arg (ap, size_t) ;

        if (numLines > MAX_SUBTITLE_LINES) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtGet) #LINES of %lu exceeds maximum of %lu\n",
                (unsigned long) numLines, (unsigned long) MAX_SUBTITLE_LINES) ;
            return (errno) ;
        }

        text = caption->text ;

        for (i = caption->numLines ;  i < numLines ;  i++) {
            text[i] = NULL ;
        }

        for (i = numLines ;  i < caption->numLines ;  i++) {
            if (text[i] != NULL)  free (text[i]) ;
            text[i] = NULL ;
        }

        caption->numLines = numLines ;

    }

/*******************************************************************************
    Replace all the lines of text in the subtitle.  The existing lines of text
    are deleted and the new lines of text are transferred to the subtitle.
    The new lines (character strings) are assumed to have been dynamically
    allocated by the caller and the subtitle takes ownership of them.  The
    "TEXT" operation uses the subtitle's current line count, so the caller
    may need to set "#LINES" before setting "TEXT" if the number of lines
    is different.
*******************************************************************************/

    else if (strcasecmp (which, "TEXT") == 0) {

        text = va_arg (ap, char **) ;
        if (text == NULL) {
            va_end (ap) ;
            SET_ERRNO (EINVAL) ;
            LGE "(srtSet) NULL array pointer for \"%s\".\n", which) ;
            return (errno) ;
        }

        for (i = 0 ;  i < caption->numLines ;  i++) {
            if (caption->text[i] != NULL)  free (caption->text[i]) ;
            caption->text[i] = text[i] ;
        }

    }

/*******************************************************************************
    An invalid attribute name was passed in.
*******************************************************************************/

    else {
        va_end (ap) ;
        SET_ERRNO (EINVAL) ;
        LGE "(srtSet) Invalid attribute name: \"%s\"\n", which) ;
        return (errno) ;
    }


    va_end (ap) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtSetTimes ()

    Set a Subtitle's Beginning and Ending Display Times.


Purpose:

    Function srtSetTimes() sets the beginning and ending display times
    of a subtitle.

        NOTE: If only one of the times is to be set, call srtGetTimes()
        to get the existing times, change whichever time to its new
        value, and then call srtSetTimes() with the new time and the
        other existing time.


    Invocation:

        status = srtSetTimes (caption, beginTime, endTime) ;

    where

        <caption>	- I
            is the subtitle handle returned by srtCreate().
        <beginTime>	- I
            is the new beginning display time.
        <endTime>	- I
            is the new ending display time.
        <status>	- O
            returns the status of setting the times, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtSetTimes (

#    if PROTOTYPES
        Subtitle  caption,
        struct  timeval  beginTime,
        struct  timeval  endTime)
#    else
        caption, beginTime, endTime)

        Subtitle  caption ;
        struct  timeval  beginTime ;
        struct  timeval  endTime ;
#    endif

{

    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtSetTimes) NULL subtitle.\n") ;
        return (errno) ;
    }

    caption->beginTime = beginTime ;
    caption->endTime = endTime ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtStringToTime ()

    Convert a String to a Subtitle Time.


Purpose:

    Function srtStringToTime() converts the ASCII representation of
    a subtitle beginning or ending time ("HH:MM:SS,mmm") to a binary
    "timeval" structure


    Invocation:

        time = srtStringToTime (string) ;

    where

        <string>	- I
            is the ASCII representation of a time formatted as "HH:MM:SS,mmm".
        <time>		- O
            returns a binary UNIX "timeval" structure containing the specified
            time.

*******************************************************************************/


struct  timeval  srtStringToTime (

#    if PROTOTYPES
        const  char  *string)
#    else
        string)

        char  *string ;
#    endif

{    /* Local variables. */
    int  numItems ;
    long  hours, milliseconds, minutes, seconds ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtStringToTime) NULL string.\n") ;
        return (tvCreate (0, 0)) ;
    }

    numItems = sscanf (string, "%02ld:%02ld:%02ld%*[,.]%03ld",
                       &hours, &minutes, &seconds, &milliseconds) ;
    if ((numItems != 4) || (hours < 0) ||
        (minutes < 0) || (minutes > 59) || (seconds < 0) || (seconds > 59)) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtStringToTime) String conversion failed; \"%s\"\n", string) ;
        return (tvCreate (0, 0)) ;
    }

    return (tvCreate ((((hours * 60) + minutes) * 60) + seconds,
                      milliseconds * 1000)) ;

}

/*!*****************************************************************************

Procedure:

    srtTimeToString ()

    Convert a Subtitle Time to a String.


Purpose:

    Function srtTimeToString() converts a subtitle beginning or ending
    time to its ASCII representation: "HH:MM:SS,mmm".


    Invocation:

        result = srtTimeToString (time, string) ;

    where

        <time>		- I
            is the subtitle time to be converted.
        <string>	- I
            is a caller-supplied buffer to which the formatted time will
            be written.  It should be at least 13 characters in length
            (12 characters for "HH:MM:SS,mmm" plus the NUL character).
        <result>	- O
            simply returns the STRING argument.

*******************************************************************************/


char  *srtTimeToString (

#    if PROTOTYPES
        struct  timeval  time,
        char  *string)
#    else
        time, string)

        struct  timeval  time ;
        char  *string ;
#    endif

{    /* Local variables. */
    long  hours, milliseconds, minutes, seconds ;



    if (string == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtTimeToString) NULL string.\n") ;
        return (NULL) ;
    }

/* Round to the nearest millisecond.  A carry or borrow may propagate to the
   number of seconds; e.g., 24.999600 seconds, where 999,600 microseconds will
   round to 1,000 milliseconds, carry the 1, and the nearest millisecond will
   be 25.000 seconds. */

    if (time.tv_usec < 0) {
        milliseconds = (time.tv_usec - 500) / 1000 ;
    } else {
        milliseconds = (time.tv_usec + 500) / 1000 ;
    }

    seconds = time.tv_sec ;
    if (milliseconds < -999) {
        seconds-- ;  milliseconds += 1000 ;	/* Borrow a second. */
    } else if (milliseconds > 999) {
        seconds++ ;  milliseconds -= 1000 ;	/* Carry a second. */
    }

    minutes = seconds / 60 ;
    seconds -= minutes * 60 ;
    hours = minutes / 60 ;
    minutes -= hours * 60 ;

    sprintf (string, "%02ld:%02ld:%02ld,%03ld",
             hours, minutes, seconds, milliseconds) ;

    return (string) ;

}

/*!*****************************************************************************

Procedure:

    srtListAdd ()

    Add a Subtitle to a List of Subtitles.


Purpose:

    Function srtListAdd() adds/inserts a subtitle to/in a list of subtitles.


    Invocation:

        status = srtListAdd (list, caption, position) ;

    where

        <list>		- I
            is the list to which the subtitle is to be added.
        <caption>	- I
            is the subtitle to be added to the list.
        <position>	- I
            is the position (1..N) in the list at which the subtitle will be
            inserted, where N is the number of subtitles currently in the list.
            For position P in the range 1..N, the existing subtitles P..N are
            displaced by 1 to positions P+1..N+1 and the new subtitle becomes
            the subtitle at position P.  A negative position (e.g., -1), zero,
            or any position greater than N causes the subtitle to be added to
            the end of the list.
        <status>	- O
            returns the status of adding the subtitle to the list,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListAdd (

#    if PROTOTYPES
        SubtitleList  list,
        Subtitle  caption,
        ssize_t  position)
#    else
        list, caption, position)

        SubtitleList  list ;
        Subtitle  caption ;
        ssize_t  position ;
#    endif

{

    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListAdd) NULL subtitle list.\n") ;
        return (errno) ;
    }

    if (caption == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListAdd) NULL subtitle.\n") ;
        return (errno) ;
    }

/*******************************************************************************
    If the currently allocated array of subtitles is full, expand it.
*******************************************************************************/

    if (list->numSubtitles >= list->maxSubtitles) {

        size_t  maxSubtitles = list->maxSubtitles ;
        Subtitle  *array ;
#define  MAX_SUBTITLE_BLOCK  256

        if (maxSubtitles == 0) {
            maxSubtitles = MAX_SUBTITLE_BLOCK ;
            array = calloc (maxSubtitles, sizeof (Subtitle)) ;
        } else {
            maxSubtitles += MAX_SUBTITLE_BLOCK ;
            array = realloc (list->subtitles,
                             maxSubtitles * sizeof (Subtitle)) ;
        }
        if (array == NULL) {
            LGE "(srtListAdd) Error allocating %lu bytes of memory for list %p.\ncalloc/realloc: ",
                (unsigned long) (maxSubtitles * sizeof (Subtitle)),
                (void *) list) ;
            return (errno) ;
        }

        list->maxSubtitles = maxSubtitles ;
        list->subtitles = array ;

    }

/* Add the subtitle to the list. */

    if ((position <= 0) || (position > (ssize_t) list->numSubtitles)) {
        list->subtitles[list->numSubtitles++] = caption ;
    } else {
        size_t  numToMove = list->numSubtitles++ - position + 1 ;
        memmove (&list->subtitles[position],
                 &list->subtitles[position-1],
                 numToMove * sizeof (Subtitle)) ;
        list->subtitles[position-1] = caption ;
    }


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtListApplyMXB ()

    Adjust the Times of All the Subtitles in a List.


Purpose:

    Function srtListApplyMXB() adjusts the beginning and ending display
    times of all the subtitles in a list in order to synchronize the
    subtitles with the audio track.  The adjustments are made using the
    slope and y-intercept computed by a prior call to srtCalcMXB().


    Invocation:

        status = srtListApplyMXB (list, slope, yIntercept) ;

    where

        <list>	- I
            is the subtitle list handle returned by srtListCreate().
        <slope>		- I
            is the slope of the line connecting the shifted times of two
            subtitles; see srtCalcMXB().
        <yIntercept>		- I
            is the y-intercept of the line connecting the shifted times
            of the two subtitles; see srtCalcMXB().
        <status>	- O
            returns the status of adjusting the subtitles' times,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListApplyMXB (

#    if PROTOTYPES
        SubtitleList  list,
        double  slope,
        double  yIntercept)
#    else
        list, slope, yIntercept)

        SubtitleList  list ;
        double  slope ;
        double  yIntercept ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListApplyMXB) NULL subtitle list.\n") ;
        return (errno) ;
    }

    for (i = 0 ;  i < list->numSubtitles ;  i++) {
        srtApplyMXB (list->subtitles[i], slope, yIntercept) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtListCreate ()

    Create a List of Subtitles.


Purpose:

    Function srtListCreate() creates an empty list of subtitles.
    Individual subtitles can be added to the list using srtListAdd();
    multiple subtitles can be read from a disk file and added to an
    existing list using srtListLoad().


    Invocation:

        status = srtListCreate (&list) ;

    where

        <list>		- O
            returns a handle for the new subtitle list.  This handle
            is used for accessing the list in subsequent srtListXyz()
            function calls.
        <status>	- O
            returns the status of creating the subtitle list,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListCreate (

#    if PROTOTYPES
        SubtitleList  *list)
#    else
        file)

        SubtitleList  *list ;
#    endif

{

    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListCreate) NULL subtitle list return pointer.\n") ;
        return (errno) ;
    }

/* Create and initialize the subtitle list. */

    *list = (SubtitleList) malloc (sizeof (_SubtitleList)) ;
    if (*list == NULL) {
        LGE "(srtListCreate) Error allocating subtitle list.\nmalloc: ") ;
        return (errno) ;
    }

    (*list)->maxSubtitles = 0 ;
    (*list)->numSubtitles = 0 ;
    (*list)->subtitles = NULL ;

    LGI "(srtListCreate) Created subtitle list %p.\n", (void *) *list) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtListDestroy ()

    Destroy a Subtitle File.


Purpose:

    Function srtListDestroy() destroys a previously created subtitle list.


    Invocation:

        status = srtListDestroy (list) ;

    where

        <list>	- I
            is the subtitle list handle returned by srtListCreate().
        <status>	- O
            returns the status of destroying the subtitle list,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListDestroy (

#    if PROTOTYPES
        SubtitleList  list)
#    else
        list)

        SubtitleList  list ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (list == NULL)  return (0) ;

    LGI "(srtListDestroy) Destroying subtitle list %p ...\n", (void *) list) ;

/* First, delete the subtitles. */

    for (i = 0 ;  i < list->numSubtitles ;  i++)
        srtDestroy (list->subtitles[i]) ;
    list->numSubtitles = 0 ;

/* Second, delete the array that held the subtitles. */

    if (list->subtitles != NULL)  free (list->subtitles) ;
    list->subtitles = NULL ;

/* Finally, delete the subtitle list structure itself. */

    free (list) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtListDelete ()

    Delete a Subtitle from a List.


Purpose:

    Function srtListDelete() deletes a subtitle from a subtitle list.


    Invocation:

        status = srtListDelete (list, position) ;

    where

        <list>		- I
            is the subtitle list handle returned by srtListCreate().
        <position>	- I
            is the position (1..N) in the list of the subtitle to be
            deleted.  A negative position (e.g., -1) is shorthand for
            position N, the last subtitle in the list.
        <status>	- O
            returns the status of deleting the subtitle, zero if no
            errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListDelete (

#    if PROTOTYPES
        SubtitleList  list,
        ssize_t  position)
#    else
        list, position)

        SubtitleList  list ;
        ssize_t  position ;
#    endif

{    /* Local variables. */
    size_t  numToMove ;



    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListDelete) NULL list.\n") ;
        return (errno) ;
    }

    if (position < 0)  position = list->numSubtitles ;

    if ((position == 0) || (position > (ssize_t) list->numSubtitles)) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListDelete) Invalid position %ld for %lu-element list %p.\n",
            (long) position, (unsigned long) list->numSubtitles,
            (void *) list) ;
        return (errno) ;
    }

/* Erase the subtitle and ... */

    srtDestroy (list->subtitles[position-1]) ;

/* ... shift the following subtitles down one position. */

    numToMove = list->numSubtitles++ - position ;
    if (numToMove > 0)  memmove (&list->subtitles[position-1],
                                 &list->subtitles[position],
                                 numToMove * sizeof (Subtitle)) ;
    list->subtitles[list->numSubtitles--] = NULL ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtListFind ()

    Find a Subtitle by Sequence Number in a List.


Purpose:

    Function srtListFind() finds the first subtitle in a list with a
    matching sequence number and returns the position of the subtitle
    in the list.


    Invocation:

        position = srtListFind (list, sequenceNum) ;

    where

        <list>		- I
            is the subtitle list handle returned by srtListCreate().
        <sequenceNum>	- I
            is the subtitle sequence number.
        <position>	- O
            returns the position in the list of the first subtitle with
            the specified sequence number.  A position of zero is returned
            if the list is NULL or no subtitle is found with the desired
            sequence number.

*******************************************************************************/


ssize_t  srtListFind (

#    if PROTOTYPES
        SubtitleList  list,
        ssize_t  sequenceNum)
#    else
        list, sequenceNum)

        SubtitleList  list ;
        ssize_t  sequenceNum ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListFind) NULL list.\n") ;
        return (0) ;
    }

    for (i = 0 ;  i < list->numSubtitles ;  i++) {
        if (list->subtitles[i]->sequenceNum == sequenceNum)
            return ((ssize_t) (i + 1)) ;		/* Found. */
    }

    return (0) ;					/* Not found. */

}

/*!*****************************************************************************

Procedure:

    srtListGet ()

    Retrieve a Subtitle by Position from a List.


Purpose:

    Function srtListGet() retrieves a subtitle by position (1..N) from a list.


    Invocation:

        caption = srtListGet (list, position) ;

    where

        <list>		- I
            is the subtitle list handle returned by srtListCreate().
        <position>	- I
            is the position (1..N) in the list of the desired subtitle.
            Zero is shorthand for position N, the last subtitle in the
            list.  Negative numbers have the following meaning:
                *    -1 indexes position N-1, the next to last subtitle;
                *    -2 indexes position N-2;
                *    -3 indexes position N-3;
                 ...
                *    -(N-1) indexes position 1.
            Negative numbers from -N on down are invalid positions.
        <caption>	- O
            returns the requested subtitle.  NULL is returned if the list
            is NULL.  NULL is also returned if, given a list length of N,
            (i) position > N or (ii) position <= -N.

*******************************************************************************/


Subtitle  srtListGet (

#    if PROTOTYPES
        SubtitleList  list,
        ssize_t  position)
#    else
        list, position)

        SubtitleList  list ;
        ssize_t  position ;
#    endif

{

    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListGet) NULL list.\n") ;
        return (NULL) ;
    }

    if (position == 0)  position = list->numSubtitles ;

    if ((list->numSubtitles == 0) ||
        (position <= -((ssize_t) list->numSubtitles)) ||
        ((ssize_t) list->numSubtitles < position)) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListGet) Invalid position %ld for %lu-element list %p.\n",
            (long) position, (unsigned long) list->numSubtitles,
            (void *) list) ;
        return (NULL) ;
    }

    if (position < 0)  position += list->numSubtitles ;

    return (list->subtitles[position - 1]) ;

}

/*!*****************************************************************************

Procedure:

    srtListLength ()

    Get the Number of Subtitles in a List.


Purpose:

    Function srtListLength() returns the number of subtitles in a subtitle list.


    Invocation:

        numCaptions = srtListLength (list) ;

    where

        <list>	- I
            is the subtitle list handle returned by srtListCreate().
        <count>	- O
            returns the number of subtitles in the list.

*******************************************************************************/


size_t  srtListLength (

#    if PROTOTYPES
        SubtitleList  list)
#    else
        list)

        SubtitleList  list ;
#    endif

{

    return ((list == NULL) ? 0 : list->numSubtitles) ;

}

/*!*****************************************************************************

Procedure:

    srtListLoad ()

    Load Subtitles from a Disk File.


Purpose:

    Function srtListLoad() reads the subtitles from a SubRip Text (".srt")
    disk file and inserts or adds the subtitles at a specified position in
    a subtitle list.


    Invocation:

        status = srtListLoad (list, fileName, position) ;

    where

        <list>		- I
            is the list to which the subtitles in the file will be added.
        <fileName>	- I
            is the name of the subtitle file to read.
        <position>	- I
            is the position (1..N) in the list at which the subtitles from
            the file will be inserted, where N is the number of subtitles
            currently in the list.  For position P in the range 1..N, the
            existing subtitles P..N are displaced by the number of subtitles
            read from the file.  A negative position (e.g., -1), zero, or
            any position greater than N result in the subtitles being added
            to the end of the list.
        <status>	- O
            returns the status of loading the subtitles from the file,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListLoad (

#    if PROTOTYPES
        SubtitleList  list,
        const  char  *fileName,
        ssize_t  position)
#    else
        list, fileName, position)

        SubtitleList  list ;
        char  *fileName ;
        ssize_t  position ;
#    endif

{    /* Local variables. */
    char  *pathname ;
    FILE  *file ;
    Subtitle  caption ;
    TagUTF  tag ;



    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListLoad) NULL subtitle list.\n") ;
        return (errno) ;
    }

    if ((position <= 0) || (position > (ssize_t) list->numSubtitles))
        position = list->numSubtitles + 1 ;

/* Open the input file. */

    if (fileName == NULL) {
        file = stdin ;
    } else {
        pathname = fnmBuild (FnmPath, fileName, NULL) ;
        if (pathname == NULL) {
            LGE "(srtListLoad) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
            return (errno) ;
        }
        file = fopen (fileName, "r") ;
        if (file == NULL) {
            LGE "(srtListLoad) Error opening \"%s\".\nfopen: ", pathname) ;
            PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
            return (errno) ;
        }
        free (pathname) ;
    }

/* Read each subtitle from the file and add them to the list. */

    tag = UTF_NULL_TAG ;	/* Let srtReadFile() detect BOM. */

    caption = srtReadFile (file, &tag) ;
    while (caption != NULL) {
        if (caption->sequenceNum < 0) {		/* Normal end-of-file! */
            return (0) ;
        }
        if (srtListAdd (list, caption, position++)) {
            LGE "(srtListLoad) Error adding subtitle #%ld to list %p.\nsrtListAdd: ",
                (long) caption->sequenceNum, (void *) list) ;
            PUSH_ERRNO ;  srtDestroy (caption) ;  fclose (file) ;  POP_ERRNO ;
            return (errno) ;
        }
        caption = srtReadFile (file, &tag) ;
    }


/* CAPTION equals NULL, so there must have been an error in srtReadFile(). */

    LGE "(srtListLoad) Error reading subtitle from file.\nsrtReadFile: ") ;

    return (errno) ;

}

/*!*****************************************************************************

Procedure:

    srtListMerge ()

    Merge Two Subtitle Lists into a New List.


Purpose:

    Function srtListMerge() combines the subtitles from two lists into
    a new list of subtitles sorted by start time.  If two subtitles from
    the two lists have the same start time, the one from the first list
    is added first.  The sequence numbers of the subtitles added to the
    merged list are kept intact, so the caller may wish to follow up
    srtListMerge() with an srtRenumber() to regenerate fresh sequence
    numbers.

    Why did I write this?  I was looking for English subtitles for
    an Indian movie in which the characters effortlessly switch back
    and forth between Hindi and English, sometimes every couple of
    sentences.  The English SRT files I examined almost all had
    English subtitles ... for the English dialog!  I finally found
    a file with English subtitles for the Hindi dialog only (or so
    I thought).  I wanted English subtitles for the entire movie, so
    I figured I could merge an SRT file of English English subtitles
    with an SRT file of Hindi English subtitles.  This function was
    the result of my cunning plan.  Unfortunately, when I later
    looked down at the end of the Hindi file, I discovered that
    subtitles were few and far between.  It seems the transcriber
    started out with good intentions, but must have gotten bored
    part way through the film and only noted down subtitles every
    couple of minutes, despite some dense Hindi/English conversations
    as the film reached its conclusion.


    Invocation:

        list3 = srtListMerge (list1, list2) ;

    where

        <list1>	- I
            is the first subtitle list.
        <list2>	- I
            is the second subtitle list.
        <list3>	- O
            returns a new list of the merged subtitles from the two input
            lists; NULL is returned in the event of an error.  The list
            consists of dynamically allocated copies of the subtitles in
            the input lists, so each of the three lists can be modified
            or deleted without affecting the other two.

*******************************************************************************/


SubtitleList  srtListMerge (

#    if PROTOTYPES
        SubtitleList  list1,
        SubtitleList  list2)
#    else
        list1, list2)

        SubtitleList  list1 ;
        SubtitleList  list2 ;
#    endif

{    /* Local variables. */
    size_t  i, j, length1, length2 ;
    struct  timeval  begin1, begin2 ;
    Subtitle  srt1, srt2, srt3 ;
    SubtitleList  list3 ;



    if ((list1 == NULL) || (list2 == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListMerge) NULL input list(s).\n") ;
        return (NULL) ;
    }

/* Create the empty, merged output list. */

    if (srtListCreate (&list3)) {
        LGE "(srtListMerge) Error creating merged output list.\nsrtListCreate: ") ;
        return (NULL) ;
    }

/*******************************************************************************
    Merge the subtitles from the two input lists into a third list with the
    subtitles sorted by their beginning display time.  In each iteration of
    the loop below, the next subtitle is retrieved from one of the lists and
    its beginning time is compared to the beginning time of the current
    subtitle from the other list.  The earliest subtitle is added to the
    merged list and, on the next loop iteration, the next subtitle will be
    retrieved from the earliest subtitle's list.
*******************************************************************************/

    begin1 = begin2 = tvCreate (0, 0) ;
    length1 = srtListLength (list1) ;
    length2 = srtListLength (list2) ;
    srt1 = srt2 = srt3 = NULL ;

    i = j = 1 ;

    while ((i <= length1) || (j <= length2)) {

/* If the subtitle from one input list was added to the output list in
   the previous iteration (indicated by the subtitle handle being reset
   to NULL), then get the next subtitle from the list.  The subtitle
   from the other input list is still under consideration, so it is not
   necessary to get it again. */

        if ((i <= length1) && (srt1 == NULL)) {
            srt1 = srtListGet (list1, i) ;
            begin1 = srt1->beginTime ;
        }
        if ((j <= length2) && (srt2 == NULL)) {
            if (srt2 == NULL)  srt2 = srtListGet (list2, j) ;
            begin2 = srt2->beginTime ;
        }

/* If one list is exhausted, continue adding the remaining subtitles
   from the other list. */

        if (i > length1) {
            srt3 = srtDuplicate (srt2) ;
            srt2 = NULL ;  j++ ;
        } else if (j > length2) {
            srt3 = srtDuplicate (srt1) ;
            srt1 = NULL ;  i++ ;
        }

/* Otherwise, compare the beginning times of the subtitles from the two
   input lists and add the earliest subtitle to the merged output list. */

        else if (tvCompare (begin1, begin2) <= 0) {
            srt3 = srtDuplicate (srt1) ;
            srt1 = NULL ;  i++ ;
        } else {
            srt3 = srtDuplicate (srt2) ;
            srt2 = NULL ;  j++ ;
        }

        if (srt3 == NULL) {
            LGE "(srtListMerge) Error duplicating subtitle.\nsrtDuplicate: ") ;
            PUSH_ERRNO ;  srtDestroy (srt3) ;  POP_ERRNO ;
            return (NULL) ;
        }

        if (srtListAdd (list3, srt3, -1)) {
            LGE "(srtListMerge) Error adding subtitle to merged list.\nsrtListAdd: ") ;
            PUSH_ERRNO ;  srtDestroy (srt3) ;  POP_ERRNO ;
            return (NULL) ;
        }

    }


    return (list3) ;

}

/*!*****************************************************************************

Procedure:

    srtListRenumber ()

    Renumber the Sequence Numbers of the Subtitles in a List.


Purpose:

    Function srtListRenumber() renumbers the subtitles in a list with an
    incrementing sequence number, e.g., 1, 2, 3, ..., N.


    Invocation:

        status = srtListRenumber (list, sequenceNum) ;

    where

        <list>		- I
            is the subtitle list handle returned by srtListCreate().
        <sequenceNum>	- I
            is the beginning sequence number, applied to the first subtitle
            in the list.
        <status>	- O
            returns the status of renumbering the subtitles, zero if there
            were no errors and ERRNO if the list is NULL.

*******************************************************************************/


errno_t  srtListRenumber (

#    if PROTOTYPES
        SubtitleList  list,
        ssize_t  sequenceNum)
#    else
        list, sequenceNum)

        SubtitleList  list ;
        ssize_t  sequenceNum ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListRenumber) NULL list.\n") ;
        return (errno) ;
    }

    for (i = 0 ;  i < list->numSubtitles ;  i++) {
        list->subtitles[i]->sequenceNum = sequenceNum++ ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    srtListSave ()

    Save Subtitles to a Disk File.


Purpose:

    Function srtListSave() saves a list of subtitles to a SubRip Text (".srt")
    disk file.


    Invocation:

        status = srtListSave (list, fileName) ;

    where

        <list>		- I
            is the list to be saved to disk.
        <fileName>	- I
            is the name of the subtitle file.
        <status>	- O
            returns the status of saving the subtitles to the file,
            zero if no errors occurred and ERRNO otherwise.

*******************************************************************************/


errno_t  srtListSave (

#    if PROTOTYPES
        SubtitleList  list,
        const  char  *fileName)
#    else
        list, fileName)

        SubtitleList  list ;
        char  *fileName ;
#    endif

{    /* Local variables. */
    char  begins[16], ends[16], *pathname ;
    FILE  *file ;
    size_t  i, j ;
    Subtitle  caption ;



    if (list == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(srtListSave) NULL subtitle list.\n") ;
        return (errno) ;
    }

/* Create/open the output file. */

    if (fileName == NULL) {
        file = stdout ;
    } else {
        pathname = fnmBuild (FnmPath, fileName, NULL) ;
        if (pathname == NULL) {
            LGE "(srtListSave) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
            return (errno) ;
        }
        file = fopen (fileName, "w") ;
        if (file == NULL) {
            LGE "(srtListSave) Error creating/opening \"%s\".\nfopen: ",
                pathname) ;
            PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
            return (errno) ;
        }
        free (pathname) ;
    }

/* Format and write out each subtitle to the file. */

    for (i = 0 ;  i < list->numSubtitles ;  i++) {
        caption = list->subtitles[i] ;
        fprintf (file, "%ld\n", (long) caption->sequenceNum) ;
        srtTimeToString (caption->beginTime, begins) ;
        srtTimeToString (caption->endTime, ends) ;
        fprintf (file, "%s --> %s\n", begins, ends) ;
        for (j = 0 ;  j < caption->numLines ;  j++) {
            fprintf (file, "%s\n", caption->text[j]) ;
        }
        fprintf (file, "\n") ;
    }

    if (fileName != NULL)  fclose (file) ;	/* Don't close stdout. */

    return (0) ;

}
