/* $Id: id3_util.h,v 1.16 2024/05/28 11:32:48 alex Exp $ */
/*******************************************************************************

    id3_util.h

    ID3 Tag Utilities.

*******************************************************************************/

#ifndef  ID3_UTIL_H		/* Has the file been INCLUDE'd already? */
#define  ID3_UTIL_H  yes

#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
extern  "C"  {
#endif


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <stdio.h>			/* Standard I/O definitions. */

#ifdef READ_THIS
    /***********************************************************************
        The "id3_internals.h" file is '#include'd below, after the
        data type definitions and before the function prototypes.
    ***********************************************************************/
#endif


/*******************************************************************************
    ID3 Tag (Client View) and Definitions.
*******************************************************************************/

typedef  struct  _Id3Tag  *Id3Tag ;	/* Tag handle. */

/* ID3 version specification is represented using the least significant 3 bytes
   of a long integer.  According to the informal ID3v2 standard, the first field
   has no name (e.g., v2), the second field is the major version number (e.g.,
   v2.3), and the third field is the revision number (e.g., v2.3.0). */

#define  ID3V_VERSION(version)  (((version) >> 16) & 0x0FFL)
#define  ID3V_MAJOR(version)  (((version) >> 8) & 0x0FFL)
#define  ID3V_REVISION(version)  ((version) & 0x0FFL)

typedef  long  Id3Version ;

#define  ID3V(version,major,revision)	\
	((((version) & 0x0FFL) << 16) |	\
	(((major) & 0x0FFL) << 8) |	\
	((revision) & 0x0FFL))

#define  ID3V_V1	ID3V (1, 0, 0)
#define  ID3V_V1_1	ID3V (1, 1, 0)
#define  ID3V_V2	ID3V (2, 0, 0)
#define  ID3V_V2_2	ID3V (2, 2, 0)
#define  ID3V_V2_3	ID3V (2, 3, 0)
#define  ID3V_V2_4	ID3V (2, 4, 0)

#define  ID3_V1_TRAILER_SIZE  128
#define  ID3_V2_HEADER_SIZE  10
#define  ID3_V2_FRAME_HEADER_SIZE  10

typedef  enum  Id3TextEncoding {
    ID3_ISO_8859_1 = 0,
    ID3_UTF16_WITH_BOM = 1,
    ID3_UTF16BE_NO_BOM = 2,		/* ID3v2.4 ... */
    ID3_UTF8 = 3
}  Id3TextEncoding ;

/*******************************************************************************

    ID3 Frame Types

    The names for the frame types are borrowed from Florian Heidenreich's
    "Mp3tag" program, "The universal tag editor." https://www.mp3tag.de/en/
    I used an awk(1) script to generate this enumeration listing from his
    "Tag Field Mappings Table".  https://docs.mp3tag.de/mapping/

    I highly recommend Mp3tag.  I like to use my "tag311" program to add or
    change ID3v1 tags, followed immediately by using Mp3tag to add/change
    the ID3v2 tags.  I realize the first step is pointless since Mp3tag
    already handles ID3v1 tags, but it lets me believe that tag311 is kinda,
    sorta useful for something!

*******************************************************************************/

				/* Synonym for Mp3tag's "WWW" (below). */
#define  ID3_WXXX_GENERIC  ID3_WWW

typedef  enum  Id3FrameType {
    ID3_NULL_FRAME_TYPE = 0,	/* Always the first entry; see id3IndexOf(). */
    ID3_ATTACHEDPIC,		/* Not in Mp3tag mapping; added by me. */
    ID3_ENCRYPTREG,		/* Not in Mp3tag mapping; added by me. */
    ID3_GENERALOBJ,		/* Not in Mp3tag mapping; added by me. */
    ID3_GROUPIDREG,		/* Not in Mp3tag mapping; added by me. */
    ID3_PASSTHROUGH,		/* Not in Mp3tag mapping; added by me. */
    ID3_PLAYCOUNT,		/* Not in Mp3tag mapping; added by me. */
    ID3_PRIVATE,		/* Not in Mp3tag mapping; added by me. */
    ID3_SYNCEDLYRICS,		/* Not in Mp3tag mapping; added by me. */
    ID3_TEXT_GENERIC,		/* Not in Mp3tag mapping; added by me. */
    ID3_TXXX_GENERIC,		/* Not in Mp3tag mapping; added by me. */
    ID3_WURL_GENERIC,		/* Not in Mp3tag mapping; added by me. */
    ID3_ACOUSTID_ID,
    ID3_ACOUSTID_FINGERPRINT,
    ID3_ALBUM,
    ID3_ALBUMSORT,
    ID3_ALBUMARTIST,
    ID3_ALBUMARTISTSORT,
    ID3_ARTIST,
    ID3_ARTISTSORT,
    ID3_BARCODE,
    ID3_BPM,
    ID3_CATALOGNUMBER,
    ID3_COMMENT,
    ID3_COMPILATION,
    ID3_COMPOSER,
    ID3_COMPOSERSORT,
    ID3_CONDUCTOR,
    ID3_CONTENTGROUP,
    ID3_COPYRIGHT,
    ID3_DATE,
    ID3_DESCRIPTION,
    ID3_DISCNUMBER,
    ID3_ENCODEDBY,
    ID3_ENCODERSETTINGS,
    ID3_ENCODINGTIME,
    ID3_FILEOWNER,
    ID3_FILETYPE,
    ID3_GENRE,
    ID3_GROUPING,
    ID3_INITIALKEY,
    ID3_INVOLVEDPEOPLE,
    ID3_ISRC,
    ID3_LANGUAGE,
    ID3_LENGTH,
    ID3_LYRICIST,
    ID3_MEDIATYPE,
    ID3_MIXARTIST,
    ID3_MOOD,
    ID3_MOVEMENTNAME,
    ID3_MOVEMENT,
    ID3_MOVEMENTTOTAL,
    ID3_MUSICBRAINZ_ALBUMARTISTID,
    ID3_MUSICBRAINZ_ALBUMID,
    ID3_MUSICBRAINZ_ALBUMRELEASECOUNTRY,
    ID3_MUSICBRAINZ_ALBUMSTATUS,
    ID3_MUSICBRAINZ_ALBUMTYPE,
    ID3_MUSICBRAINZ_ARTISTID,
    ID3_MUSICBRAINZ_DISCID,
    ID3_MUSICBRAINZ_ORIGINALALBUMID,
    ID3_MUSICBRAINZ_ORIGINALARTISTID,
    ID3_MUSICBRAINZ_RELEASEGROUPID,
    ID3_MUSICBRAINZ_RELEASETRACKID,
    ID3_MUSICBRAINZ_TRACKID,
    ID3_MUSICBRAINZ_TRMID,
    ID3_MUSICBRAINZ_WORKID,
    ID3_MUSICIANCREDITS,
    ID3_NARRATOR,
    ID3_NETRADIOOWNER,
    ID3_NETRADIOSTATION,
    ID3_ORIGALBUM,
    ID3_ORIGARTIST,
    ID3_ORIGFILENAME,
    ID3_ORIGLYRICIST,
    ID3_ORIGYEAR,
    ID3_PODCAST,
    ID3_PODCASTCATEGORY,
    ID3_PODCASTDESC,
    ID3_PODCASTID,
    ID3_PODCASTKEYWORDS,
    ID3_PODCASTURL,
    ID3_POPULARIMETER,
    ID3_PUBLISHER,
    ID3_RELEASETIME,
    ID3_SETSUBTITLE,
    ID3_SUBTITLE,
    ID3_TAGGINGTIME,
    ID3_TITLE,
    ID3_TITLESORT,
    ID3_TRACK,
    ID3_UNIQUEFILEID,
    ID3_UNSYNCEDLYRICS,
    ID3_WWW,
    ID3_WWWARTIST,
    ID3_WWWAUDIOFILE,
    ID3_WWWAUDIOSOURCE,
    ID3_WWWCOMMERCIALINFO,
    ID3_WWWCOPYRIGHT,
    ID3_WWWPAYMENT,
    ID3_WWWPUBLISHER,
    ID3_WWWRADIOPAGE,
    ID3_YEAR,
    ID3_MAX_FRAME_TYPES		/* Total # types, including the null type. */
}  Id3FrameType ;

/*******************************************************************************
    Attached Picture Types
*******************************************************************************/

typedef  enum  Id3PictureType {
    ID3_PIC_OTHER = 0,
    ID3_PIC_FILE_ICON ,
    ID3_PIC_OTHER_ICON ,
    ID3_PIC_COVER_FRONT ,
    ID3_PIC_COVER_BACK ,
    ID3_PIC_LEAFLET_PAGE ,
    ID3_PIC_MEDIA ,
    ID3_PIC_LEAD_ARTIST ,
    ID3_PIC_ARTIST ,
    ID3_PIC_CONDUCTOR ,
    ID3_PIC_BAND_ORCHESTRA ,
    ID3_PIC_COMPOSER ,
    ID3_PIC_LYRICIST ,
    ID3_PIC_RECORDING_LOCATION ,
    ID3_PIC_DURING_RECORDING ,
    ID3_PIC_DURING_PERFORMANCE ,
    ID3_PIC_SCREEN_CAPTURE ,
    ID3_PIC_BRIGHT_COLOURED_FISH ,	/* For real! */
    ID3_PIC_ILLUSTRATION ,
    ID3_PIC_ARTIST_LOGOTYPE ,
    ID3_PIC_PUBLISHER_LOGOTYPE
}  Id3PictureType ;

/*******************************************************************************
    Miscellaneous declarations.
*******************************************************************************/

					/* Global debug switch (1/0 = yes/no). */
extern  int  id3_util_debug  OCD ("id3_util") ;


/*******************************************************************************
    ID3 utilities internal definitions.
*******************************************************************************/

#include  "id3_internals.h"		/* Internal definitions. */


/*******************************************************************************
    Public functions (general, in "id3_util.c").
*******************************************************************************/

extern  errno_t  id3CfgGet P_((Id3Tag tag,
                               const char *options,
                               ...))
    OCD ("id3_util") ;

extern  errno_t  id3CfgSet P_((Id3Tag tag,
                               const char *options,
                               ...))
    OCD ("id3_util") ;

extern  uint32_t  id3CRC32 P_((const void *buffer,
                               size_t length,
                               uint32_t previousCRC32))
    OCD ("id3_util") ;

extern  unsigned  long  id3DecodeSize P_((const char *buffer))
    OCD ("id3_util") ;

extern  unsigned  long  id3DecodeSSI P_((const char *buffer,
                                         size_t length))
    OCD ("id3_util") ;

extern  size_t  id3DeUnsync P_((char *buffer,
                                size_t length))
    OCD ("id3_util") ;

extern  errno_t  id3Dump P_((FILE *file,
                             const char *indentation,
                             bool brief,
                             bool numberFrames,
                             Id3Tag tag))
    OCD ("id3_util") ;

extern  errno_t  id3DumpGenres P_((FILE *file,
                                   const char *indentation))
    OCD ("id3_util") ;

extern  errno_t  id3DumpLUT P_((FILE *file,
                                const char *indentation))
    OCD ("id3_util") ;

extern  errno_t  id3DumpPicTypes P_((FILE *file,
                                     const char *indentation))
    OCD ("id3_util") ;

extern  void  id3EncodeSize P_((unsigned long value,
                                char *buffer))
    OCD ("id3_util") ;

extern  void  id3EncodeSSI P_((unsigned long value,
                               size_t length,
                               char *buffer))
    OCD ("id3_util") ;

extern  Id3TextEncoding  id3Encoding P_((const Id3Tag tag))
    OCD ("id3_util") ;

extern  bool  id3IsEmpty P_((const Id3Tag tag))
    OCD ("id3_util") ;

extern  bool  id3IsFrame P_((const char *buffer,
                             size_t length))
    OCD ("id3_util") ;

extern  bool  id3IsTag P_((const char *buffer,
                           size_t length))
    OCD ("id3_util") ;

extern  errno_t  id3ReadHeaders P_((const char *fileName,
                                    char **v1Header,
                                    char **v2Header))
    OCD ("id3_util") ;

extern  errno_t  id3ReadHeadersF P_((FILE *file,
                                     char **v1Header,
                                     char **v2Header))
    OCD ("id3_util") ;

extern  errno_t  id3ReadTags P_((const char *fileName,
                                 Id3Tag *v1Tag,
                                 Id3Tag *v2Tag))
    OCD ("id3_util") ;

extern  errno_t  id3ReadTagsF P_((FILE *file,
                                  Id3Tag *v1Tag,
                                  Id3Tag *v2Tag))
    OCD ("id3_util") ;

extern  errno_t  id3ReadV1 P_((FILE *file,
                               Id3Tag *tag))
    OCD ("id3_util") ;

extern  errno_t  id3ReadV2 P_((FILE *file,
                               Id3Tag *tag))
    OCD ("id3_util") ;

extern  errno_t  id3Strip P_((const char *fileName,
                              Id3Version version))
    OCD ("id3_util") ;

extern  errno_t  id3StripF P_((FILE *file,
                               Id3Version version))
    OCD ("id3_util") ;

extern  char  *id3Unsync P_((const char *buffer,
                             size_t length,
                             size_t *numUnsync))
    OCD ("id3_util") ;

extern  errno_t  id3WriteTags P_((const char *fileName,
                                  Id3Tag v1Tag,
                                  Id3Tag v2Tag,
                                  bool mergeTags,
                                  bool sortFrames,
                                  const char *extension))
    OCD ("id3_util") ;

/*******************************************************************************
    Public functions (get fields, in "id3_util.c").
*******************************************************************************/

extern  const  char  *id3GetAlbum P_((Id3Tag tag))
    OCD ("id3_util") ;

extern  const  char  *id3GetArtist P_((Id3Tag tag))
    OCD ("id3_util") ;

extern  const  char  *id3GetComment P_((Id3Tag tag,
                                        const char *language,
                                        const char *description))
    OCD ("id3_util") ;

extern  const  char  *id3GetCover P_((Id3Tag tag,
                                      Id3PictureType pictureType,
                                      const char *description,
                                      size_t *length,
                                      const char **mimeType))
    OCD ("id3_util") ;

extern  const  char  *id3GetGenre P_((Id3Tag tag))
    OCD ("id3_util") ;

extern  const  char  *id3GetList  P_((Id3Tag tag,
                                      Id3FrameType type))
    OCD ("id3_util") ;

extern  const  char  *id3GetObject P_((Id3Tag tag,
                                       const char *description,
                                       size_t *length,
                                       const char **mimeType,
                                       const char **fileName))
    OCD ("id3_util") ;

extern  size_t  id3GetPlayCount P_((Id3Tag tag,
                                    const char *email,
                                    unsigned int *rating))
    OCD ("id3_util") ;

extern  const  char  *id3GetPrivate P_((Id3Tag tag,
                                        const char *ownerID,
                                        size_t *length))
    OCD ("id3_util") ;

extern  const  char  *id3GetRegistration P_((Id3Tag tag,
                                             Id3FrameType type,
                                             unsigned int symbol,
                                             const char **data,
                                             size_t *length))
    OCD ("id3_util") ;

extern  const  char  *id3GetSong P_((Id3Tag tag))
    OCD ("id3_util") ;

extern  const  char  *id3GetText P_((Id3Tag tag,
                                     Id3FrameType type,
                                     const char *qualifier))
    OCD ("id3_util") ;

extern  long  id3GetTrack P_((Id3Tag tag,
                              long *total))
    OCD ("id3_util") ;

extern  const  char  *id3GetUFID P_((Id3Tag tag,
                                     const char *ownerID,
                                     size_t *length))
    OCD ("id3_util") ;

extern  const  char  *id3GetUnsyncLyrics P_((Id3Tag tag,
                                             const char *language,
                                             const char *description))
    OCD ("id3_util") ;

extern  const  char  *id3GetURL P_((Id3Tag tag,
                                    Id3FrameType type,
                                    const char *description))
    OCD ("id3_util") ;

extern  Id3Version  id3GetVersion P_((Id3Tag tag))
    OCD ("id3_util") ;

extern  long  id3GetYear P_((Id3Tag tag))
    OCD ("id3_util") ;

/*******************************************************************************
    Public functions (set fields, in "id3_util.c").
*******************************************************************************/

extern  errno_t  id3SetAlbum P_((Id3Tag tag,
                                 const char *album))
    OCD ("id3_util") ;

extern  errno_t  id3SetArtist P_((Id3Tag tag,
                                  const char *artist))
    OCD ("id3_util") ;

extern  errno_t  id3SetComment P_((Id3Tag tag,
                                   bool delete,
                                   const char *language,
                                   const char *description,
                                   const char *comment))
    OCD ("id3_util") ;

extern  errno_t  id3SetCover P_((Id3Tag tag,
                                 char *cover,
                                 size_t length,
                                 bool makeCopy,
                                 const char *mimeType,
                                 Id3PictureType type,
                                 const char *description))
    OCD ("id3_util") ;

extern  errno_t  id3SetGenre P_((Id3Tag tag,
                                 const char *genre))
    OCD ("id3_util") ;

extern  errno_t  id3SetList P_((Id3Tag tag,
                                Id3FrameType type,
                                const char *listSpec))
    OCD ("id3_util") ;

extern  errno_t  id3SetObject P_((Id3Tag tag,
                                 char *object,
                                 size_t length,
                                 bool makeCopy,
                                 const char *mimeType,
                                 const char *fileName,
                                 const char *description))
    OCD ("id3_util") ;

extern  errno_t  id3SetPlayCount P_((Id3Tag tag,
                                     bool delete,
                                     const char *email,
                                     unsigned int rating,
                                     size_t counter))
    OCD ("id3_util") ;

extern  errno_t  id3SetPrivate P_((Id3Tag tag,
                                   bool delete,
                                   const char *ownerID,
                                   char *data,
                                   size_t length,
                                   bool makeCopy))
    OCD ("id3_util") ;

extern  errno_t  id3SetRegistration P_((Id3Tag tag,
                                        Id3FrameType type,
                                        bool delete,
                                        unsigned int symbol,
                                        const char *ownerID,
                                        char *data,
                                        size_t length,
                                        bool makeCopy))
    OCD ("id3_util") ;

extern  errno_t  id3SetSong P_((Id3Tag tag,
                                const char *song))
    OCD ("id3_util") ;

extern  errno_t  id3SetText P_((Id3Tag tag,
                                Id3FrameType type,
                                const char *text,
                                const char *qualifier))
    OCD ("id3_util") ;

extern  errno_t  id3SetTrack P_((Id3Tag tag,
                                 long track,
                                 long total))
    OCD ("id3_util") ;

extern  errno_t  id3SetUFID P_((Id3Tag tag,
                                bool delete,
                                const char *ownerID,
                                char *fileID,
                                size_t length,
                                bool makeCopy))
    OCD ("id3_util") ;

extern  errno_t  id3SetUnsyncLyrics P_((Id3Tag tag,
                                        bool delete,
                                        const char *language,
                                        const char *description,
                                        const char *lyrics))
    OCD ("id3_util") ;

extern  errno_t  id3SetURL P_((Id3Tag tag,
                               Id3FrameType type,
                               const char *url,
                               const char *qualifier))
    OCD ("id3_util") ;

extern  errno_t  id3SetVersion P_((Id3Tag tag,
                                   Id3Version version))
    OCD ("id3_util") ;

extern  errno_t  id3SetYear P_((Id3Tag tag,
                                long year))
    OCD ("id3_util") ;

					/* Get flags from ID3v2 tag. */
extern  int  id3Flags P_((Id3Tag tag))
    OCD ("id3_util") ;
					/* Get size of ID3v2 tag. */
extern  ssize_t  id3Size P_((Id3Tag tag))
    OCD ("id3_util") ;

/*******************************************************************************
    Public functions (lookup, in "id3_util.c").
*******************************************************************************/

extern  const  char  *id3FromGenre P_((long number))
    OCD ("id3_util") ;

extern  long  id3ToGenre P_((const char *name))
    OCD ("id3_util") ;

extern  const  char  *id3FromPictureType P_((Id3PictureType type))
    OCD ("id3_util") ;

extern  Id3PictureType  id3ToPictureType P_((const char *name))
    OCD ("id3_util") ;

/*******************************************************************************
    Public functions (tag operations, in "id3_tag.c").
*******************************************************************************/

Id3Tag  id3TagCopy P_((Id3Tag oldTag))
    OCD ("id3_util") ;

errno_t  id3TagCreate P_((Id3Version version,
                          Id3Tag *tag))
    OCD ("id3_util") ;

errno_t  id3TagDestroy P_((Id3Tag tag))
    OCD ("id3_util") ;

errno_t  id3TagDump P_((FILE *file,
                        const char *indentation,
                        Id3Tag tag))
    OCD ("id3_util") ;

errno_t  id3TagErase P_((Id3Tag tag))
    OCD ("id3_util") ;

Id3Tag  id3TagMerge P_((Id3Tag primaryTag,
                        Id3Tag overlayTag))
    OCD ("id3_util") ;

errno_t  id3TagSortFrames P_((Id3Tag tag))
    OCD ("id3_util") ;


#ifdef __cplusplus		/* If this is a C++ compiler, use C linkage */
}
#endif

#endif				/* If this file was not INCLUDE'd previously. */
