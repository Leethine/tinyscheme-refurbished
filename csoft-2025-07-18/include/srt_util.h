/* $Id: srt_util.h,v 1.4 2021/04/21 01:49:35 alex Exp $ */
/*******************************************************************************

    srt_util.h

    SubRip Text (SRT) File Utilities.

*******************************************************************************/

#ifndef  SRT_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  SRT_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */


#define  SRT_LINE_WARNING  1		/* srtCheck() warning/error mask bits. */
#define  SRT_SEQUENCE_WARNING  2
#define  SRT_DURATION_ERROR  4
#define  SRT_OVERLAP_ERROR  8


/*******************************************************************************
    Subtitles (Client View) and Definitions.
*******************************************************************************/

					/* Subtitle and list handles. */
typedef  struct  _Subtitle  *Subtitle ;
typedef  struct  _SubtitleList  *SubtitleList ;


/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  srt_util_debug  OCD ("srt_util") ;

/*******************************************************************************
    Public functions for individual subtitles (sequence number, begin/end
    times, and text).
*******************************************************************************/

extern  errno_t  srtAddLine P_((Subtitle caption,
                                const char *text,
                                ssize_t length))
    OCD ("srt_util") ;

extern  errno_t  srtApplyMXB P_((Subtitle caption,
                                 double slope,
                                 double yIntercept))
    OCD ("srt_util") ;

extern  errno_t  srtApplyPLF P_((Subtitle caption,
                                 Subtitle captions[],
                                 double slopes[],
                                 double yIntercepts[]))
    OCD ("srt_util") ;

extern  errno_t  srtCalcMXB P_((Subtitle caption1,
                                double offset1,
                                Subtitle caption2,
                                double offset2,
                                double *slope,
                                double *yIntercept))
    OCD ("srt_util") ;

extern  errno_t  srtCalcPLF P_((size_t numCaptions,
                                Subtitle captions[],
                                double offsets[],
                                double slopes[],
                                double yIntercepts[]))
    OCD ("srt_util") ;

extern  int  srtCheck P_((Subtitle caption1,
                          Subtitle caption2,
                          size_t lineLimit))
    OCD ("srt_util") ;

extern  errno_t  srtCreate P_((ssize_t sequenceNum,
                               Subtitle *caption))
    OCD ("srt_util") ;

extern  errno_t  srtDestroy P_((Subtitle caption))
    OCD ("srt_util") ;

extern  Subtitle  srtDuplicate P_((Subtitle caption))
    OCD ("srt_util") ;

extern  errno_t  srtGet P_((Subtitle caption,
                            const char *which,
                            ...))
    OCD ("srt_util") ;
/* "SEQ#" */
/* "BEGIN" */
/* "END" */
/* "#LINES" */
/* "TEXT" */

extern  errno_t  srtGetTimes P_((Subtitle caption,
                                 struct timeval *beginTime,
                                 struct timeval *endTime))
    OCD ("srt_util") ;

extern  Subtitle  srtReadFile P_((FILE *file,
                                  TagUTF *tag))
    OCD ("srt_util") ;

extern  Subtitle  srtReadString P_((const char *string))
    OCD ("srt_util") ;

extern  errno_t  srtSet P_((Subtitle caption,
                            const char *which,
                            ...))
    OCD ("srt_util") ;

extern  errno_t  srtSetTimes P_((Subtitle caption,
                                 struct timeval beginTime,
                                 struct timeval endTime))
    OCD ("srt_util") ;

extern  struct  timeval  srtStringToTime P_((const char *string))
    OCD ("srt_util") ;

extern  char  *srtTimeToString P_((struct timeval time,
                                   char *string))
    OCD ("srt_util") ;

/*******************************************************************************
    Public functions for subtitle lists.
*******************************************************************************/

extern  errno_t  srtListAdd P_((SubtitleList list,
                                Subtitle caption,
                                ssize_t position))
    OCD ("srt_util") ;

extern  errno_t  srtListApplyMXB P_((SubtitleList list,
                                     double slope,
                                     double yIntercept))
    OCD ("srt_util") ;

extern  errno_t  srtListCreate P_((SubtitleList *list))
    OCD ("srt_util") ;

extern  errno_t  srtListDelete P_((SubtitleList list,
                                   ssize_t position))
    OCD ("srt_util") ;

extern  errno_t  srtListDestroy P_((SubtitleList list))
    OCD ("srt_util") ;

extern  ssize_t  srtListFind P_((SubtitleList list,
                                 ssize_t sequenceNum))
    OCD ("srt_util") ;

extern  Subtitle  srtListGet P_((SubtitleList list,
                                 ssize_t position))
    OCD ("srt_util") ;

extern  size_t  srtListLength P_((SubtitleList list))
    OCD ("srt_util") ;

extern  errno_t  srtListLoad P_((SubtitleList list,
                                 const char *fileName,
                                 ssize_t position))
    OCD ("srt_util") ;

extern  SubtitleList  srtListMerge P_((SubtitleList list1,
                                       SubtitleList list2))
    OCD ("srt_util") ;

extern  errno_t  srtListRenumber P_((SubtitleList list,
                                     ssize_t sequenceNum))
    OCD ("srt_util") ;

extern  errno_t  srtListSave P_((SubtitleList list,
                                 const char *fileName))
    OCD ("srt_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
