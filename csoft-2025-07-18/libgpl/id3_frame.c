/* $Id: id3_frame.c,v 1.9 2024/05/28 11:35:01 alex Exp alex $ */
/*******************************************************************************

File:

    id3_frame.c

    ID3 Tag Utilities - Frames


Author:    Alex Measday


Purpose:

    This file is an internal part of the ID3_UTIL package and provides
    functions for the manipulation of ID3 frames.  An ID3 tag is composed
    of one or more ID3 frames.  Each frame, in turn, contains one or more
    data values; e.g., character strings in the case of text frames.  This
    file, as its name suggests, implements the functions for dealing with
    ID3 frames.

    The list of values in a tag is represented by an array of value handles.
    Note that a call to adxGet() to retrieve a value handle returns the
    address of the element containing the value handle.  In other words,
    adxGet() returns a pointer (Id3Value *) to the handle and not the
    handle directly.


Public Procedures:

    id3FrameAddStringV() - adds a string value to an ID3 frame.
    id3FrameAddValue() - adds a value to an ID3 frame.
    id3FrameCopy() - duplicates an ID3 frame.
    id3FrameCreate() - creates an empty ID3 frame.
    id3FrameDataCopy() - copies type-specific frame data.
    id3FrameDataErase() - erases type-specific frame data.
    id3FrameDataInit() - initializes type-specific frame data.
    id3FrameDecode() - decodes an ID3 frame.
    id3FrameDestroy() - destroys an ID3 frame.
    id3FrameDump() - formats and dumps an ID3 frame for debug purposes.
    id3FrameDumpLUT() - dumps the frame information lookup table.
    id3FrameEncode() - encodes an ID3v2 frame into its file format.
    id3FrameErase() - erases the contents of an ID3 frame.
    id3FrameGetValue() - gets the I-th value in an ID3 frame.
    id3FrameInfoByID() - looks up frame information by ID.
    id3FrameInfoByName() - looks up frame information by type name.
    id3FrameInfoByType() - looks up frame information by type.
    id3FrameParsePair() - parses the next pair of values in a string.
    id3FramePVLfromS() - constructs a paired-values list from a string.
    id3FramePVLtoS() - formats a paired-values list as a string.
    id3FrameTypeName() - returns text name for type.
    id3FrameUDDescOf() - looks up the description for a user-defined type.
    id3FrameUDTypeOf() - looks up a user-defined type.
    id3FrameUDUpgrade() - upgrades a frame to a predefined, user-defined type.
    id3FrameValueCount() - returns the number of values in an ID3 frame.

Private Procedures:

    id3FrameDecodeComment() - decodes an ID3v2 comment frame.
    id3FrameDecodeGenObj() - decodes an ID3v2 general object frame.
    id3FrameDecodePassthrough() - passes through an ID3v2 frame as-is.
    id3FrameDecodePicture() - decodes an ID3v2 picture frame.
    id3FrameDecodePlayCount() - decodes an ID3v2 PCNT/POPM frame.
    id3FrameDecodePrivate() - decodes an ID3v2 private frame.
    id3FrameDecodePVList() - decodes an ID3v2 paired-values list frame.
    id3FrameDecodeRegistration() - decodes ENCR/GRID frames.
    id3FrameDecodeString() - decodes a string from an ID3v2 frame.
    id3FrameDecodeText() - decodes an ID3v2 text frame.
    id3FrameDecodeUFID() - decodes an ID3v2 UFID frame.
    id3FrameDecodeURL() - decodes an ID3v2 URL link frame.
    id3FrameDecodeUSLT() - decodes an ID3v2 unsynced lyrics frame.
    id3FrameDecodeVUInt() - decodes an ID3v2 variable-length integer.

    id3FrameEncodeComment() - encodes an ID3v2 comment frame body.
    id3FrameEncodeGenObj() - encodes an ID3v2 general object frame.
    id3FrameEncodePicture() - encodes an ID3v2 picture frame body.
    id3FrameEncodePlayCount() - encodes an ID3v2 PCNT/POPM frame.
    id3FrameEncodePrivate() - encodes an ID3v2 private frame.
    id3FrameEncodePVList() - encodes an ID3v2 paired-values list frame.
    id3FrameEncodeRegistration() - encodes ENCR/GRID frames.
    id3FrameEncodeString() - encodes an ID3v2 string.
    id3FrameEncodeText() - encodes an ID3v2 text frame body.
    id3FrameEncodeUFID() - encodes an ID3v2 UFID frame.
    id3FrameEncodeURL() - encodes an ID3v2 URL link frame.
    id3FrameEncodeUSLT() - encodes an ID3v2 unsynced lyrics frame.
    id3FrameEncodeVUInt() - encodes an ID3v2 variable-length integer.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "adx_util.h"			/* Array Descriptor utilities. */
#include  "get_util.h"			/* "Get Next" functions. */
#include  "meo_util.h"			/* Memory operations. */
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */
#include  "sdx_util.h"			/* String Descriptor utilities. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */
#include  "id3_util.h"			/* ID3 tag utilities. */
#include  "id3_internals.h"		/* Internal definitions. */


	/* The default/only wide-character text encoding used by the
	   id3FrameEncode...() functions.  The one common Unicode encoding
	   between ID3v2.3 and ID3v2.4 is UTF-16 with a BOM indicating the
	   byte ordering.  So, always use this encoding. */

#define  DEFAULT_WCS_ENCODING  ID3_UTF16_WITH_BOM


#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  id3_util_debug


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  id3FrameDecodeComment (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodeGenObj (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodePassthrough (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body,
        size_t  length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodePicture (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodePlayCount (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodePrivate (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodePVList (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodeRegistration (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  char  *id3FrameDecodeString (
#    if PROTOTYPES
        Id3TextEncoding  encoding,
        const  char  *buffer,
        size_t  length,
        size_t  *skip
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodeText (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodeUFID (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodeURL (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameDecodeUSLT (
#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body
#    endif
    )  OCD ("id3_util") ;

static  size_t  id3FrameDecodeVUInt (
#    if PROTOTYPES
        const  char  *buffer,
        size_t  length,
        size_t  *skip
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeComment (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeGenObj (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodePicture (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodePlayCount (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodePrivate (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodePVList (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeRegistration (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeString (
#    if PROTOTYPES
        Id3TextEncoding encoding,
        const  char  *string,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeText (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeUFID (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeURL (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeUSLT (
#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

static  errno_t  id3FrameEncodeVUInt (
#    if PROTOTYPES
        size_t  integer,
        MemoryDx  bdx,
        size_t  *length
#    endif
    )  OCD ("id3_util") ;

/*******************************************************************************

    ID3V2 Frame Information Lookup Table.

    This information was generated by an awk(1) script from Mp3tag's "Tag
    Field Mappings Table" ( https://docs.mp3tag.de/mapping/ ).  Mp3tag is
    Florian Heidenreich's "universal tag editor", a full-featured, Windows
    GUI program which I both regularly use and highly recommend.
    ( https://www.mp3tag.de/en/ )

    The second entry in the table below, added by me, is for generic,
    user-defined, text information frames ("TXXX").  Mp3tag also defines
    specific TXXX frames which are qualified in the table by
    "TXXX:<description>"; e.g., see some of the MusicBrainz tags.

*******************************************************************************/

Id3FrameLUTEntry  id3FrameLUT[] = {
    { ID3_NULL_FRAME_TYPE, "NULL", NULL, NULL, Id3MarshalNull },
    { ID3_ATTACHEDPIC, "ATTACHEDPIC", "APIC", "APIC", Id3MarshalPicture },
    { ID3_ENCRYPTREG, "ENCRYPTREG", "ENCR", "ENCR", Id3MarshalRegistration },
    { ID3_GENERALOBJ, "GENERALOBJ", "GEOB", "GEOB", Id3MarshalGenObj },
    { ID3_GROUPIDREG, "GROUPIDREG", "GRID", "GRID", Id3MarshalRegistration },
    { ID3_PASSTHROUGH, "PASSTHROUGH", "PTRU", "PTRU", Id3MarshalPassthrough },
    { ID3_PLAYCOUNT, "PLAYCOUNT", "PCNT", "PCNT", Id3MarshalPlayCount },
    { ID3_PRIVATE, "PRIVATE", "PRIV", "PRIV", Id3MarshalPrivate },
    { ID3_SYNCEDLYRICS, "SYNCEDLYRICS", "SYLT", "SYLT", Id3MarshalPassthrough },
    { ID3_TEXT_GENERIC, "TEXT_GENERIC", "Tgen", "Tgen", Id3MarshalText },
    { ID3_TXXX_GENERIC, "TXXX", "TXXX", "TXXX", Id3MarshalUText },
    { ID3_WURL_GENERIC, "WURL_GENERIC", "Wgen", "Wgen", Id3MarshalURL },
    { ID3_ACOUSTID_ID, "ACOUSTID_ID", "TXXX:Acoustid Id", "TXXX:Acoustid Id", Id3MarshalUText },
    { ID3_ACOUSTID_FINGERPRINT, "ACOUSTID_FINGERPRINT", "TXXX:Acoustid Fingerprint", "TXXX:Acoustid Fingerprint", Id3MarshalUText },
    { ID3_ALBUM, "ALBUM", "TALB", "TALB", Id3MarshalText },
    { ID3_ALBUMSORT, "ALBUMSORT", "TSOA", "TSOA", Id3MarshalText },
    { ID3_ALBUMARTIST, "ALBUMARTIST", "TPE2", "TPE2", Id3MarshalText },
    { ID3_ALBUMARTISTSORT, "ALBUMARTISTSORT", "TSO2", "TSO2", Id3MarshalText },
    { ID3_ARTIST, "ARTIST", "TPE1", "TPE1", Id3MarshalText },
    { ID3_ARTISTSORT, "ARTISTSORT", "TSOP", "TSOP", Id3MarshalText },
    { ID3_BARCODE, "BARCODE", "TXXX:BARCODE", "TXXX:BARCODE", Id3MarshalUText },
    { ID3_BPM, "BPM", "TBPM", "TBPM", Id3MarshalText },
    { ID3_CATALOGNUMBER, "CATALOGNUMBER", "TXXX:CATALOGNUMBER", "TXXX:CATALOGNUMBER", Id3MarshalUText },
    { ID3_COMMENT, "COMMENT", "COMM", "COMM", Id3MarshalComment },
    { ID3_COMPILATION, "COMPILATION", "TCMP", "TCMP", Id3MarshalText },
    { ID3_COMPOSER, "COMPOSER", "TCOM", "TCOM", Id3MarshalText },
    { ID3_COMPOSERSORT, "COMPOSERSORT", "TSOC", "TSOC", Id3MarshalText },
    { ID3_CONDUCTOR, "CONDUCTOR", "TPE3", "TPE3", Id3MarshalText },
    { ID3_CONTENTGROUP, "CONTENTGROUP", "TIT1", "TIT1", Id3MarshalText },
    { ID3_COPYRIGHT, "COPYRIGHT", "TCOP", "TCOP", Id3MarshalText },
    { ID3_DATE, "DATE", "TDAT", "TXXX:DATE", Id3MarshalUText },
    { ID3_DESCRIPTION, "DESCRIPTION", NULL, NULL, Id3MarshalText },
    { ID3_DISCNUMBER, "DISCNUMBER", "TPOS", "TPOS", Id3MarshalText },
    { ID3_ENCODEDBY, "ENCODEDBY", "TENC", "TENC", Id3MarshalText },
    { ID3_ENCODERSETTINGS, "ENCODERSETTINGS", "TSSE", "TSSE", Id3MarshalText },
    { ID3_ENCODINGTIME, "ENCODINGTIME", NULL, "TDEN", Id3MarshalText },
    { ID3_FILEOWNER, "FILEOWNER", "TOWN", "TOWN", Id3MarshalText },
    { ID3_FILETYPE, "FILETYPE", "TFLT", "TFLT", Id3MarshalText },
    { ID3_GENRE, "GENRE", "TCON", "TCON", Id3MarshalText },
    { ID3_GROUPING, "GROUPING", "GRP1", "GRP1", Id3MarshalText },
    { ID3_INITIALKEY, "INITIALKEY", "TKEY", "TKEY", Id3MarshalText },
    { ID3_INVOLVEDPEOPLE, "INVOLVEDPEOPLE", "IPLS", "TIPL", Id3MarshalPVList },
    { ID3_ISRC, "ISRC", "TSRC", "TSRC", Id3MarshalText },
    { ID3_LANGUAGE, "LANGUAGE", "TLAN", "TLAN", Id3MarshalText },
    { ID3_LENGTH, "LENGTH", "TLEN", "TLEN", Id3MarshalText },
    { ID3_LYRICIST, "LYRICIST", "TEXT", "TEXT", Id3MarshalText },
    { ID3_MEDIATYPE, "MEDIATYPE", "TMED", "TMED", Id3MarshalText },
    { ID3_MIXARTIST, "MIXARTIST", "TPE4", "TPE4", Id3MarshalText },
    { ID3_MOOD, "MOOD", NULL, "TMOO", Id3MarshalText },
    { ID3_MOVEMENTNAME, "MOVEMENTNAME", "MVNM", "MVNM", Id3MarshalText },
    { ID3_MOVEMENT, "MOVEMENT", "MVIN", "MVIN", Id3MarshalText },
    { ID3_MOVEMENTTOTAL, "MOVEMENTTOTAL", "MVIN", "MVIN", Id3MarshalText },
    { ID3_MUSICBRAINZ_ALBUMARTISTID, "MUSICBRAINZ_ALBUMARTISTID", "TXXX:MusicBrainz Album Artist Id", "TXXX:MusicBrainz Album Artist Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ALBUMID, "MUSICBRAINZ_ALBUMID", "TXXX:MusicBrainz Album Id", "TXXX:MusicBrainz Album Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ALBUMRELEASECOUNTRY, "MUSICBRAINZ_ALBUMRELEASECOUNTRY", "TXXX:MusicBrainz Album Release Country", "TXXX:MusicBrainz Album Release Country", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ALBUMSTATUS, "MUSICBRAINZ_ALBUMSTATUS", "TXXX:MusicBrainz Album Status", "TXXX:MusicBrainz Album Status", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ALBUMTYPE, "MUSICBRAINZ_ALBUMTYPE", "TXXX:MusicBrainz Album Type", "TXXX:MusicBrainz Album Type", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ARTISTID, "MUSICBRAINZ_ARTISTID", "TXXX:MusicBrainz Artist Id", "TXXX:MusicBrainz Artist Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_DISCID, "MUSICBRAINZ_DISCID", "TXXX:MusicBrainz Disc Id", "TXXX:MusicBrainz Disc Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ORIGINALALBUMID, "MUSICBRAINZ_ORIGINALALBUMID", "TXXX:MusicBrainz Original Album Id", "TXXX:MusicBrainz Original Album Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_ORIGINALARTISTID, "MUSICBRAINZ_ORIGINALARTISTID", "TXXX:MusicBrainz Original Artist Id", "TXXX:MusicBrainz Original Artist Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_RELEASEGROUPID, "MUSICBRAINZ_RELEASEGROUPID", "TXXX:MusicBrainz Release Group Id", "TXXX:MusicBrainz Release Group Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_RELEASETRACKID, "MUSICBRAINZ_RELEASETRACKID", "TXXX:MusicBrainz Release Track Id", "TXXX:MusicBrainz Release Track Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_TRACKID, "MUSICBRAINZ_TRACKID", "UFID:http://musicbrainz.org", "UFID:http://musicbrainz.org", Id3MarshalUFID },
    { ID3_MUSICBRAINZ_TRMID, "MUSICBRAINZ_TRMID", "TXXX:MusicBrainz TRM Id", "TXXX:MusicBrainz TRM Id", Id3MarshalUText },
    { ID3_MUSICBRAINZ_WORKID, "MUSICBRAINZ_WORKID", "TXXX:MusicBrainz Work Id", "TXXX:MusicBrainz Work Id", Id3MarshalUText },
    { ID3_MUSICIANCREDITS, "MUSICIANCREDITS", NULL, "TMCL", Id3MarshalPVList },
    { ID3_NARRATOR, "NARRATOR", NULL, NULL, Id3MarshalText },
    { ID3_NETRADIOOWNER, "NETRADIOOWNER", "TRSO", "TRSO", Id3MarshalText },
    { ID3_NETRADIOSTATION, "NETRADIOSTATION", "TRSN", "TRSN", Id3MarshalText },
    { ID3_ORIGALBUM, "ORIGALBUM", "TOAL", "TOAL", Id3MarshalText },
    { ID3_ORIGARTIST, "ORIGARTIST", "TOPE", "TOPE", Id3MarshalText },
    { ID3_ORIGFILENAME, "ORIGFILENAME", "TOFN", "TOFN", Id3MarshalText },
    { ID3_ORIGLYRICIST, "ORIGLYRICIST", "TOLY", "TOLY", Id3MarshalText },
    { ID3_ORIGYEAR, "ORIGYEAR", "TORY", "TDOR", Id3MarshalText },
    { ID3_PODCAST, "PODCAST", "PCST", "PCST", Id3MarshalPassthrough },
    { ID3_PODCASTCATEGORY, "PODCASTCATEGORY", "TCAT", "TCAT", Id3MarshalText },
    { ID3_PODCASTDESC, "PODCASTDESC", "TDES", "TDES", Id3MarshalText },
    { ID3_PODCASTID, "PODCASTID", "TGID", "TGID", Id3MarshalText },
    { ID3_PODCASTKEYWORDS, "PODCASTKEYWORDS", "TKWD", "TKWD", Id3MarshalText },
    { ID3_PODCASTURL, "PODCASTURL", "WFED", "WFED", Id3MarshalPassthrough },
    { ID3_POPULARIMETER, "POPULARIMETER", "POPM", "POPM", Id3MarshalPlayCount },
    { ID3_PUBLISHER, "PUBLISHER", "TPUB", "TPUB", Id3MarshalText },
    { ID3_RELEASETIME, "RELEASETIME", "TDRL", "TDRL", Id3MarshalText },
    { ID3_SETSUBTITLE, "SETSUBTITLE", NULL, "TSST", Id3MarshalText },
    { ID3_SUBTITLE, "SUBTITLE", "TIT3", "TIT3", Id3MarshalText },
    { ID3_TAGGINGTIME, "TAGGINGTIME", NULL, "TDTG", Id3MarshalText },
    { ID3_TITLE, "TITLE", "TIT2", "TIT2", Id3MarshalText },
    { ID3_TITLESORT, "TITLESORT", "TSOT", "TSOT", Id3MarshalText },
    { ID3_TRACK, "TRACK", "TRCK", "TRCK", Id3MarshalText },
    { ID3_UNIQUEFILEID, "UNIQUEFILEID", "UFID", "UFID", Id3MarshalUFID },
    { ID3_UNSYNCEDLYRICS, "UNSYNCEDLYRICS", "USLT", "USLT", Id3MarshalUnsyncLyrics },
    { ID3_WWW, "WWW", "WXXX", "WXXX", Id3MarshalUURL },
    { ID3_WWWARTIST, "WWWARTIST", "WOAR", "WOAR", Id3MarshalURL },
    { ID3_WWWAUDIOFILE, "WWWAUDIOFILE", "WOAF", "WOAF", Id3MarshalURL },
    { ID3_WWWAUDIOSOURCE, "WWWAUDIOSOURCE", "WOAS", "WOAS", Id3MarshalURL },
    { ID3_WWWCOMMERCIALINFO, "WWWCOMMERCIALINFO", "WCOM", "WCOM", Id3MarshalURL },
    { ID3_WWWCOPYRIGHT, "WWWCOPYRIGHT", "WCOP", "WCOP", Id3MarshalURL },
    { ID3_WWWPAYMENT, "WWWPAYMENT", "WPAY", "WPAY", Id3MarshalURL },
    { ID3_WWWPUBLISHER, "WWWPUBLISHER", "WPUB", "WPUB", Id3MarshalURL },
    { ID3_WWWRADIOPAGE, "WWWRADIOPAGE", "WORS", "WORS", Id3MarshalURL },
    { ID3_YEAR, "YEAR", "TYER", "TDRC", Id3MarshalText },
    { ID3_NULL_FRAME_TYPE, NULL, NULL, NULL }	/* Indicates end of list. */
} ;

/*!*****************************************************************************

Procedure:

    id3FrameAddStringV ()

    Add a String Value to an ID3 Frame.


Purpose:

    The id3FrameAddStringV() function adds a string value to an ID3 frame's
    list of data values.


    Invocation:

        status = id3FrameAddStringV (frame, string, length) ;

    where

        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame.
        <string>	- I
            is the character string to be added to the frame's list of
            values.  A copy of the string is made and stored in the new
            data value; the copy will be freed when the value is destroyed.
        <length>	- I
            is the length of the string.  If this argument is less than
            zero, strlen(3) will be called to determine the length of
            the string.
        <status>	- O
            returns the status of duplicating and adding the string to
            the frame's list of values, zero if there were no errors
            and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameAddStringV (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *string,
        ssize_t  length)
#    else
        frame, value, length)

        Id3Frame  frame ;
        char  *string ;
        ssize_t  length ;
#    endif

{    /* Local variables. */
    Id3Value  value ;



    if ((frame == NULL) || (string == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameAddStringV) NULL frame handle or string: ") ;
        return (errno) ;
    }

    if (length < 0)  length = (ssize_t) strlen (string) ;

/* Create an ID3 value and store a copy of the string in it. */

    if (id3ValueCreate (&value)) {
        LGE "(id3FrameAddStringV) Error creating value object for \"%.*s\".\nid3ValueCreate: ",
            (int) length, string) ;
        return (errno) ;
    }

    value->type = Id3ValueText ;

    value->vdata.text = strndup (string, length) ;
    if (value->vdata.text == NULL) {
        LGE "(id3FrameAddStringV) Error duplicating string: \"%.*s\"\nstrndup: ",
            (int) length, string) ;
        PUSH_ERRNO ;  id3ValueDestroy (value) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Add the new value to the frame's list of data values. */

    if (id3FrameAddValue (frame, value)) {
        LGE "(id3FrameAddStringV) Error adding value %p to %s frame %p.\nid3FrameAddValue: ",
            (void *) value, frame->info->name, (void *) frame) ;
        PUSH_ERRNO ;  id3ValueDestroy (value) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameAddValue ()

    Add a Data Value to an ID3 Frame.


Purpose:

    The id3FrameAddValue() function adds a value to an ID3 frame's list of
    data values.  (The value becomes the property of the frame and the value
    will be destroyed when the frame is destroyed or erased.)


    Invocation:

        status = id3FrameAddValue (frame, value) ;

    where

        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame.
        <value>		- I
            is the handle, returned by id3ValueCreate(), of the data value.
        <status>	- O
            returns the status of adding the value to the frame's list of
            values, zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameAddValue (

#    if PROTOTYPES
        Id3Frame  frame,
        Id3Value  value)
#    else
        frame, value)

        Id3Frame  frame ;
        Id3Value  value ;
#    endif

{

    if ((frame == NULL) || (value == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameAddValue) NULL frame or value handle: ") ;
        return (errno) ;
    }

    if (adxAdd (frame->values, &value, 1, -1)) {
        LGE "(id3FrameAddValue) Error adding value %p to frame %p's array of values.\nadxAdd: ",
            (void *) value, (void *) frame) ;
        return (errno) ;
    }

    if (value->type == Id3ValueText) {
        LGI "(id3FrameAddValue) Added value %p (%s) to %s frame %p.\n",
            (void *) value, value->vdata.text,
            id3FrameTypeName (frame->type),
            (void *) frame) ;
    } else {
        LGI "(id3FrameAddValue) Added value %p to %s frame %p.\n",
            (void *) value, id3FrameTypeName (frame->type), (void *) frame) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameCopy ()

    Duplicate an ID3 Frame.


Purpose:

    The id3FrameCopy() function creates a full copy of an existing,
    frame field's value.


    Invocation:

        newFrame = id3FrameCopy (oldFrame) ;

    where

        <oldFrame>	- I
            is the existing field value to copy.
        <newFrame>	- O
            returns a handle for the new value; NULL is returned upon error.
            After the value is no longer needed, id3FrameDestroy() should
            be called to dispose of the value.

*******************************************************************************/


Id3Frame  id3FrameCopy (

#    if PROTOTYPES
        Id3Frame  oldFrame)
#    else
        oldFrame)

        Id3Frame  oldFrame ;
#    endif

{    /* Local variables. */
    Id3Frame  newFrame ;
    Id3Value  newValue, *oldValue ;
    size_t  i, count ;



    if (oldFrame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameCopy) NULL old frame handle: ") ;
        return (NULL) ;
    }

/* Allocate a new frame structure. */

    if (id3FrameCreate (oldFrame->type, &newFrame)) {
        LGE "(id3FrameCopy) Error creating new frame.\nid3FrameCreate: ") ;
        return (NULL) ;
    }

/* Copy the general frame fields. */

    newFrame->delete = oldFrame->delete ;
    newFrame->size = oldFrame->size ;
    newFrame->flags = oldFrame->flags ;
    newFrame->groupID = oldFrame->groupID ;
    newFrame->encoding = oldFrame->encoding ;
    newFrame->delete = oldFrame->delete ;

    if (oldFrame->frameID != NULL) {
        newFrame->frameID = strdup (oldFrame->frameID) ;
        if (newFrame->frameID == NULL) {
            LGE "(id3FrameCopy) Error duplicating frame ID \"%s\".\nstrdup: ",
                oldFrame->frameID) ;
            PUSH_ERRNO ;  id3FrameDestroy (newFrame) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

/* Copy the frame-type-specific fields. */

    if (id3FrameDataCopy (oldFrame, newFrame)) {
        LGE "(id3FrameCopy) Error copying frame-type-specific fields.\nid3FrameDataCopy: ") ;
        return (NULL) ;
    }

/* Make copies of each of the old frame's field values and add them to
   the new frame.  Remember that the array of values is an array of
   handles-for/pointers-to value structures and that adxGet() returns
   the address of a handle; hence there is an extra level of indirection. */

    count = adxCount (oldFrame->values) ;
    for (i = 0 ;  i < count ;  i++) {
        oldValue = adxGet (oldFrame->values, i) ;
        newValue = id3ValueCopy (*oldValue) ;
        if (newValue == NULL) {
            LGE "(id3FrameCopy) Error duplicating field value %p.\nid3ValueCopy: ",
                (void *) *oldValue) ;
            PUSH_ERRNO ;  id3FrameDestroy (newFrame) ;  POP_ERRNO ;
            return (NULL) ;
        }
        if (id3FrameAddValue (newFrame, newValue)) {
            LGE "(id3FrameCopy) Error adding new value %p to new frame %p.\nid3FrameAddValue: ",
                (void *) newValue, (void *) newFrame) ;
            PUSH_ERRNO ;  id3FrameDestroy (newFrame) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

    return (newFrame) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameCreate ()

    Create an Empty ID3 Frame.


Purpose:

    The id3FrameCreate() function creates an empty ID3 frame.  Empty,
    except for the frame's initially empty array of values; i.e., the
    array descriptor is allocated but the array has zero elements.


    Invocation:

        status = id3FrameCreate (type, &frame) ;

    where

        <type>		- I
            is the nominal frame type (defined in "id3_util.h").  The type
            can be manually changed after creation.
        <frame>		- O
            returns a handle for the new frame.  This handle is used
            in calls to the other ID3_UTIL functions.  After the frame
            is no longer needed, id3FrameDestroy() should be called to
            dispose of the frame.
        <status>	- O
            returns the status of creating the frame, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameCreate (

#    if PROTOTYPES
        Id3FrameType  type,
        Id3Frame  *frame)
#    else
        type, frame)

        Id3FrameType  type ;
        Id3Frame  *frame ;
#    endif

{

    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameCreate) NULL frame return: ") ;
        return (errno) ;
    }

/* Allocate a new frame structure. */

    *frame = (_Id3Frame *) malloc (sizeof (_Id3Frame)) ;
    if (*frame == NULL) {
        LGE "(id3FrameCreate) Error allocating frame structure.\nmalloc: ") ;
        return (errno) ;
    }

    (*frame)->type = type ;
    (*frame)->info = id3FrameInfoByType (type) ;
    if ((*frame)->info == NULL)  (*frame)->info = &id3FrameLUT[0] ;
    (*frame)->parent = NULL ;
    (*frame)->delete = false ;
    (*frame)->size = 0 ;
    (*frame)->flags = 0 ;
    (*frame)->groupID = 0 ;
    (*frame)->encoding = ID3_ISO_8859_1 ;
    (*frame)->frameID = NULL ;
    (*frame)->marshalType = (*frame)->info->marshalType ;

/* Initialize the frame-type-specific fields in the frame. */

    memset (&(*frame)->fdata, 0, sizeof (*frame)->fdata) ;

    id3FrameDataInit (*frame) ;

/* Allocate an empty array of values. */

    if (adxCreate (0, sizeof (Id3Value), NULL, 0, AdxDynamic,
                   &(*frame)->values)) {
        LGE "(id3FrameCopy) Error allocating array of values.\nadxCreate: ") ;
        PUSH_ERRNO ;  id3FrameDestroy (*frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    LGI "(id3FrameCreate) Created %s frame %p.\n",
        id3FrameTypeName ((*frame)->type), (void *) (*frame)) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDataCopy ()

    Copy Type-Specific Frame Data.


Purpose:

    The id3FrameDataCopy() function duplicates the type-specific, dynamically
    allocated fields in one frame and stores the duplicates in another frame.
    These fields are found in the "fdata" union part of ID3 frame structures.
    The existing type-specific fields in the target frame are overwritten;
    if these fields have actual data, then the caller should erase the target
    frame before performing the copy.


    Invocation:

        status = id3FrameDataCopy (source, target) ;

    where

        <source>	- I
            is the handle, returned by id3FrameCreate(), of the source frame.
        <target>	- I/O
            is the handle, returned by id3FrameCreate(), of the target frame.
        <status>	- O
            returns the status of copying the "fdata" fields from the source
            to the target, zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDataCopy (

#    if PROTOTYPES
        Id3Frame  source,
        Id3Frame  target)
#    else
        source, target)

        Id3Frame  source ;
        Id3Frame  target ;
#    endif

{

    if ((source == NULL) || (target == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDataCopy) NULL source or target: ") ;
        return (errno) ;
    }

    if (source->type != target->type) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDataCopy) Source/target frame types do not agree (%s, %s): ",
            source->info->name, target->info->name) ;
        return (errno) ;
    }

    if (source->info != target->info) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDataCopy) Source/target frame LUTs do not agree (%s, %s): ",
            source->info->name, target->info->name) ;
        return (errno) ;
    }

    if (target->info->marshalType == Id3MarshalComment) {

        strncpy (target->fdata.comm.language, source->fdata.comm.language, 4) ;

        if ((source->fdata.comm.description != NULL) &&
            strdup2 (source->fdata.comm.description,
                     &target->fdata.comm.description)) {
            LGE "(id3FrameDataCopy) Error duplicating COMM description.\nstrdup2: ") ;
            return (errno) ;
        }

        if ((source->fdata.comm.text != NULL) &&
            strdup2 (source->fdata.comm.text, &target->fdata.comm.text)) {
            LGE "(id3FrameDataCopy) Error duplicating COMM text.\nstrdup2: ") ;
            return (errno) ;
        }

    } else if (target->info->marshalType == Id3MarshalGenObj) {

        char  *object ;
        size_t  length ;

        if ((source->fdata.geob.mimeType != NULL) &&
            strdup2 (source->fdata.geob.mimeType,
                     &target->fdata.geob.mimeType)) {
            LGE "(id3FrameDataCopy) Error duplicating GEOB MIME type: \"%s\"\nstrdup2: ",
                source->fdata.geob.mimeType) ;
            return (errno) ;
        }

        if ((source->fdata.geob.fileName != NULL) &&
            strdup2 (source->fdata.geob.fileName,
                     &target->fdata.geob.fileName)) {
            LGE "(id3FrameDataCopy) Error duplicating GEOB file name: \"%s\"\nstrdup2: ",
                source->fdata.geob.fileName) ;
            return (errno) ;
        }

        if ((source->fdata.geob.description != NULL) &&
            strdup2 (source->fdata.geob.description,
                     &target->fdata.geob.description)) {
            LGE "(id3FrameDataCopy) Error duplicating APIC description: \"%s\"\nstrdup2: ",
                source->fdata.geob.description) ;
            return (errno) ;
        }

        object = source->fdata.geob.object ;
        length = source->fdata.geob.length ;

        if (object == NULL)
            target->fdata.geob.object = memdup ("", 1) ;
        else if (length == 0)
            target->fdata.geob.object = memdup (object, 1) ;
        else
            target->fdata.geob.object = memdup (object, length) ;
        if (target->fdata.geob.object == NULL) {
            LGE "(id3FrameDataCopy) Error duplicating %"PRIuSIZE_T"-byte GEOB object.\nmemdup: ",
                length) ;
            return (errno) ;
        }

        target->fdata.geob.length = length ;

    } else if (target->info->marshalType == Id3MarshalPassthrough) {

        if ((source->fdata.ptru.buffer != NULL) &&
            memdup2 (source->fdata.ptru.buffer, source->fdata.ptru.length,
                     (void **) &target->fdata.ptru.buffer)) {
            LGE "(id3FrameDataCopy) Error duplicating %"PRIuSIZE_T"-byte passthrough frame.\nmemdup2: ",
                source->fdata.ptru.length) ;
            return (errno) ;
        }

        target->fdata.ptru.length = source->fdata.ptru.length ;

    } else if (target->info->marshalType == Id3MarshalPicture) {

        char  *image ;
        size_t  length ;

        if ((source->fdata.apic.mimeType != NULL) &&
            strdup2 (source->fdata.apic.mimeType,
                     &target->fdata.apic.mimeType)) {
            LGE "(id3FrameDataCopy) Error duplicating APIC MIME type: \"%s\"\nstrdup2: ",
                source->fdata.apic.mimeType) ;
            return (errno) ;
        }

        target->fdata.apic.type = source->fdata.apic.type ;

        if ((source->fdata.apic.description != NULL) &&
            strdup2 (source->fdata.apic.description,
                     &target->fdata.apic.description)) {
            LGE "(id3FrameDataCopy) Error duplicating APIC description: \"%s\"\nstrdup2: ",
                source->fdata.apic.description) ;
            return (errno) ;
        }

        image = source->fdata.apic.image ;
        length = source->fdata.apic.length ;

        if (image == NULL)
            target->fdata.apic.image = memdup ("", 1) ;
        else if (length == 0)
            target->fdata.apic.image = memdup (image, 1) ;
        else
            target->fdata.apic.image = memdup (image, length) ;
        if (target->fdata.apic.image == NULL) {
            LGE "(id3FrameDataCopy) Error duplicating %"PRIuSIZE_T"-byte APIC image.\nmemdup: ",
                length) ;
            return (errno) ;
        }

        target->fdata.apic.length = length ;

    } else if (target->info->marshalType == Id3MarshalPlayCount) {

        if ((source->fdata.play.email != NULL) &&
            strdup2 (source->fdata.play.email, &target->fdata.play.email)) {
            LGE "(id3FrameDataCopy) Error duplicating POPM email: \"%s\"\nstrdup2: ",
                source->fdata.play.email) ;
            return (errno) ;
        }

        target->fdata.play.rating = source->fdata.play.rating ;
        target->fdata.play.counter = source->fdata.play.counter ;

    } else if (target->info->marshalType == Id3MarshalPrivate) {

        if ((source->fdata.priv.ownerID != NULL) &&
            strdup2 (source->fdata.priv.ownerID, &target->fdata.priv.ownerID)) {
            LGE "(id3FrameDataCopy) Error duplicating PRIV owner ID: \"%s\"\nstrdup2: ",
                source->fdata.priv.ownerID) ;
            return (errno) ;
        }

        if ((source->fdata.priv.data != NULL) &&
            memdup2 (source->fdata.priv.data, source->fdata.priv.length,
                     &target->fdata.priv.data)) {
            LGE "(id3FrameDataCopy) Error duplicating %"PRIuSIZE_T"-byte PRIV data.\nmemdup2: ",
                source->fdata.priv.length) ;
            return (errno) ;
        }

        target->fdata.priv.length = source->fdata.priv.length ;

    } else if (target->info->marshalType == Id3MarshalRegistration) {

        if ((source->fdata.regi.ownerID != NULL) &&
            strdup2 (source->fdata.regi.ownerID, &target->fdata.regi.ownerID)) {
            LGE "(id3FrameDataCopy) Error duplicating %s owner ID: \"%s\"\nstrdup2: ",
                target->info->v2_3FrameID, source->fdata.regi.ownerID) ;
            return (errno) ;
        }

        target->fdata.regi.symbol = source->fdata.regi.symbol ;

        if ((source->fdata.regi.data != NULL) &&
            memdup2 (source->fdata.regi.data, source->fdata.regi.length,
                     &target->fdata.regi.data)) {
            LGE "(id3FrameDataCopy) Error duplicating %"PRIuSIZE_T"-byte %s data field.\nmemdup2: ",
                source->fdata.regi.length, target->info->v2_3FrameID) ;
            return (errno) ;
        }

        target->fdata.regi.length = source->fdata.regi.length ;

    } else if ((target->info->marshalType == Id3MarshalText) ||
               (target->info->marshalType == Id3MarshalPVList)) {

        if ((source->fdata.text.text != NULL) &&
            strdup2 (source->fdata.text.text, &target->fdata.text.text)) {
            LGE "(id3FrameDataCopy) Error duplicating TEXT text: \"%s\"\nstrdup2: ",
                source->fdata.text.text) ;
            return (errno) ;
        }

    } else if (target->info->marshalType == Id3MarshalUFID) {

        if ((source->fdata.ufid.ownerID != NULL) &&
            strdup2 (source->fdata.ufid.ownerID, &target->fdata.ufid.ownerID)) {
            LGE "(id3FrameDataCopy) Error duplicating UFID owner ID: \"%s\"\nstrdup2: ",
                source->fdata.ufid.ownerID) ;
            return (errno) ;
        }

        if ((source->fdata.ufid.fileID != NULL) &&
            memdup2 (source->fdata.ufid.fileID, source->fdata.ufid.length,
                     &target->fdata.ufid.fileID)) {
            LGE "(id3FrameDataCopy) Error duplicating %"PRIuSIZE_T"-byte UFID file ID.\nmemdup2: ",
                source->fdata.ufid.length) ;
            return (errno) ;
        }

        target->fdata.ufid.length = source->fdata.ufid.length ;

    } else if (target->info->marshalType == Id3MarshalUnsyncLyrics) {

        strncpy (target->fdata.uslt.language, source->fdata.uslt.language, 4) ;

        if ((source->fdata.uslt.description != NULL) &&
            strdup2 (source->fdata.uslt.description,
                     &target->fdata.uslt.description)) {
            LGE "(id3FrameDataCopy) Error duplicating USLT description.\nstrdup2: ") ;
            return (errno) ;
        }

        if ((source->fdata.uslt.lyrics != NULL) &&
            strdup2 (source->fdata.uslt.lyrics, &target->fdata.uslt.lyrics)) {
            LGE "(id3FrameDataCopy) Error duplicating USLT lyrics.\nstrdup2: ") ;
            return (errno) ;
        }

    } else if (target->info->marshalType == Id3MarshalURL) {

        if ((source->fdata.wurl.url != NULL) &&
            strdup2 (source->fdata.wurl.url, &target->fdata.wurl.url)) {
            LGE "(id3FrameDataCopy) Error duplicating URL: \"%s\"\nstrdup2: ",
                source->fdata.wurl.url) ;
            return (errno) ;
        }

    } else if (target->info->marshalType == Id3MarshalUText) {

        if ((source->fdata.text.description != NULL) &&
            strdup2 (source->fdata.text.description,
                     &target->fdata.text.description)) {
            LGE "(id3FrameDataCopy) Error duplicating TXXX description: \"%s\"\nstrdup2: ",
                source->fdata.text.description) ;
            return (errno) ;
        }

        if ((source->fdata.text.text != NULL) &&
            strdup2 (source->fdata.text.text, &target->fdata.text.text)) {
            LGE "(id3FrameDataCopy) Error duplicating TXXX text: \"%s\"\nstrdup2: ",
                source->fdata.text.text) ;
            return (errno) ;
        }

    } else if (target->info->marshalType == Id3MarshalUURL) {

        if ((source->fdata.wurl.description != NULL) &&
            strdup2 (source->fdata.wurl.description,
                     &target->fdata.wurl.description)) {
            LGE "(id3FrameDataCopy) Error duplicating WXXX description: \"%s\"\nstrdup2: ",
                source->fdata.wurl.description) ;
            return (errno) ;
        }

        if ((source->fdata.wurl.url != NULL) &&
            strdup2 (source->fdata.wurl.url, &target->fdata.wurl.url)) {
            LGE "(id3FrameDataCopy) Error duplicating WXXX URL: \"%s\"\nstrdup2: ",
                source->fdata.wurl.url) ;
             return (errno) ;
        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDataErase ()

    Erase Type-Specific Frame Data.


Purpose:

    The id3FrameDataErase() function frees the type-specific, dynamically
    allocated fields in the "fdata" union part of an ID3 frame structure.


    Invocation:

        status = id3FrameDataErase (frame) ;

    where

        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame
            to initialize.
        <status>	- O
            returns the status of initializing the "fdata" fields,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDataErase (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{

    if (frame == NULL)  return (0) ;

/* Erase the frame-type-specific fields in the frame. */

    switch (frame->info->marshalType) {
    case Id3MarshalComment:
        if (frame->fdata.comm.description != NULL)
            free (frame->fdata.comm.description) ;
        if (frame->fdata.comm.text != NULL)
            free (frame->fdata.comm.text) ;
        break ;
    case Id3MarshalGenObj:
        if (frame->fdata.geob.mimeType != NULL)
            free (frame->fdata.geob.mimeType) ;
        if (frame->fdata.geob.fileName != NULL)
            free (frame->fdata.geob.fileName) ;
        if (frame->fdata.geob.description != NULL)
            free (frame->fdata.geob.description) ;
        if (frame->fdata.geob.object != NULL)
            free (frame->fdata.geob.object) ;
        break ;
    case Id3MarshalPassthrough:
        if (frame->fdata.ptru.buffer != NULL)
            free (frame->fdata.ptru.buffer) ;
        break ;
    case Id3MarshalPicture:
        if (frame->fdata.apic.mimeType != NULL)
            free (frame->fdata.apic.mimeType) ;
        if (frame->fdata.apic.description != NULL)
            free (frame->fdata.apic.description) ;
        if (frame->fdata.apic.image != NULL)
            free (frame->fdata.apic.image) ;
        break ;
    case Id3MarshalPlayCount:
        if (frame->fdata.play.email != NULL)
            free (frame->fdata.play.email) ;
        break ;
    case Id3MarshalPrivate:
        if (frame->fdata.priv.ownerID != NULL)
            free (frame->fdata.priv.ownerID) ;
        if (frame->fdata.priv.data != NULL)
            free (frame->fdata.priv.data) ;
        break ;
    case Id3MarshalRegistration:
        if (frame->fdata.regi.ownerID != NULL)
            free (frame->fdata.regi.ownerID) ;
        if (frame->fdata.regi.data != NULL)
            free (frame->fdata.regi.data) ;
        break ;
    case Id3MarshalText:
    case Id3MarshalUText:
        if (frame->fdata.text.description != NULL)
            free (frame->fdata.text.description) ;
        if (frame->fdata.text.text != NULL)
            free (frame->fdata.text.text) ;
        break ;
    case Id3MarshalUFID:
        if (frame->fdata.ufid.ownerID != NULL)
            free (frame->fdata.ufid.ownerID) ;
        if (frame->fdata.ufid.fileID != NULL)
            free (frame->fdata.ufid.fileID) ;
        break ;
    case Id3MarshalUnsyncLyrics:
        if (frame->fdata.uslt.description != NULL)
            free (frame->fdata.uslt.description) ;
        if (frame->fdata.uslt.lyrics != NULL)
            free (frame->fdata.uslt.lyrics) ;
        break ;
    case Id3MarshalURL:
    case Id3MarshalUURL:
        if (frame->fdata.wurl.description != NULL)
            free (frame->fdata.wurl.description) ;
        if (frame->fdata.wurl.url != NULL)
            free (frame->fdata.wurl.url) ;
        break ;
    default:
        break ;
    }

/* And then reinitialize the fields. */

    return (id3FrameDataInit (frame)) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDataInit ()

    Initialize Type-Specific Frame Data.


Purpose:

    The id3FrameDataInit() function initializes the type-specific,
    dynamically allocated fields in the "fdata" union part of an
    ID3 frame structure; i.e., these fields are set to NULL.


    Invocation:

        status = id3FrameDataInit (frame) ;

    where

        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame
            to initialize.
        <status>	- O
            returns the status of initializing the "fdata" fields,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDataInit (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    const  char  *description ;



    if (frame == NULL)  return (0) ;

/* Initialize frame-type-specific fields in the frame. */

    switch (frame->info->marshalType) {
    case Id3MarshalComment:
        strncpy (frame->fdata.comm.language, "eng", 4) ;
        frame->fdata.comm.description = NULL ;
        frame->fdata.comm.text = NULL ;
        break ;
    case Id3MarshalGenObj:
        frame->fdata.geob.mimeType = NULL ;
        frame->fdata.geob.fileName = NULL ;
        frame->fdata.geob.description = NULL ;
        frame->fdata.geob.object = NULL ;
        frame->fdata.geob.length = 0 ;
        break ;
    case Id3MarshalPassthrough:
        frame->fdata.ptru.buffer = NULL ;
        frame->fdata.ptru.length = 0 ;
        break ;
    case Id3MarshalPicture:
        frame->fdata.apic.mimeType = NULL ;
        frame->fdata.apic.type = 0 ;
        frame->fdata.apic.description = NULL ;
        frame->fdata.apic.image = NULL ;
        frame->fdata.apic.length = 0 ;
        break ;
    case Id3MarshalPlayCount:
        frame->fdata.play.email = NULL ;
        frame->fdata.play.rating = 0 ;
        frame->fdata.play.omit = false ;
        frame->fdata.play.counter = 0 ;
        break ;
    case Id3MarshalPrivate:
        frame->fdata.priv.ownerID = NULL ;
        frame->fdata.priv.data = NULL ;
        break ;
    case Id3MarshalRegistration:
        frame->fdata.regi.ownerID = NULL ;
        frame->fdata.regi.data = NULL ;
        break ;
    case Id3MarshalText:
        frame->fdata.text.description = NULL ;
        frame->fdata.text.text = NULL ;
        break ;
    case Id3MarshalUFID:
        frame->fdata.ufid.ownerID = NULL ;
        frame->fdata.ufid.fileID = NULL ;
        break ;
    case Id3MarshalUnsyncLyrics:
        strncpy (frame->fdata.uslt.language, "eng", 4) ;
        frame->fdata.uslt.description = NULL ;
        frame->fdata.uslt.lyrics = NULL ;
        break ;
    case Id3MarshalURL:
        frame->fdata.wurl.description = NULL ;
        frame->fdata.wurl.url = NULL ;
        break ;
    case Id3MarshalUText:
        description = id3FrameUDDescOf (frame->type) ;
        if (description == NULL) {
            frame->fdata.text.description = NULL ;
        } else {
            frame->fdata.text.description = strdup (description) ;
            if (frame->fdata.text.description == NULL) {
                LGE "(id3FrameDataInit) Error duplicating UText description: \"%s\"\nstrdup: ",
                    description) ;
                return (errno) ;
            }
        }
        frame->fdata.text.text = NULL ;
        break ;
    case Id3MarshalUURL:
        description = id3FrameUDDescOf (frame->type) ;
        if (description == NULL) {
            frame->fdata.wurl.description = NULL ;
        } else {
            frame->fdata.wurl.description = strdup (description) ;
            if (frame->fdata.wurl.description == NULL) {
                LGE "(id3FrameDataInit) Error duplicating UUrl description: \"%s\"\nstrdup: ",
                    description) ;
                return (errno) ;
            }
        }
        frame->fdata.wurl.url = NULL ;
        break ;
    default:
        break ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecode ()

    Decode an ID3v2.3/ID3v2.4 Frame from its File Format.


Purpose:

    Function id3FrameDecode() decodes an ID3v2.3 or ID3v2.4 frame in
    an ID3v2 tag.


    Invocation:

        status = id3FrameDecode (tag, buffer, length, &frame, &skip) ;

    where

        <tag>		- I
            is the handle for the frame's parent tag.
        <buffer>	- I
            is the address of a buffer containing the ID3v2 frame which
            will be decoded.
        <length>	- I
            is the length in bytes of the buffer.
        <frame>		- O
            returns a handle for the new ID3 frame; the caller is responsible
            for calling id3FrameDestroy() after the tag is no longer needed.
        <skip>		- O
            returns the offset in bytes of the next frame in the buffer
            after this frame.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDecode (

#    if PROTOTYPES
        Id3Tag  tag,
        char  *buffer,
        size_t  length,
        Id3Frame  *frame,
        size_t  *skip)
#    else
        tag, buffer, length, frame, skip)

        Id3Tag  tag ;
        char  *buffer ;
        size_t  length ;
        Id3Frame  *frame ;
        size_t  *skip ;
#    endif

{    /* Local variables. */
    char  *body ;
    size_t  frameLength, frameSize ;



    if ((tag == NULL) || (frame == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecode) NULL tag handle or frame return: ") ;
        return (errno) ;
    }

/* Allocate a new frame structure. */

    if (id3FrameCreate (ID3_NULL_FRAME_TYPE, frame)) {
        LGE "(id3FrameDecode) Error creating new frame.\nid3FrameDecode: ") ;
        return (errno) ;
    }

    (*frame)->frameID = strndup (buffer, 4) ;
    if ((*frame)->frameID == NULL) {
        LGE "(id3FrameDecode) Error duplicating frame ID: \"%.4s\".\nstrndup: ",
            buffer) ;
        PUSH_ERRNO ;  id3FrameDestroy (*frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    (*frame)->info = id3FrameInfoByID (buffer) ;
    if ((*frame)->info == NULL) {		/* Unrecognized frame ID? */
        if (buffer[0] == 'T') {			/* Assume text frame? */
            (*frame)->info = id3FrameInfoByType (ID3_TEXT_GENERIC) ;
        } else if (buffer[0] == 'W') {		/* Assumer URL frame? */
            (*frame)->info = id3FrameInfoByType (ID3_WURL_GENERIC) ;
        } else {				/* Pass frame through as is. */
            (*frame)->info = id3FrameInfoByType (ID3_PASSTHROUGH) ;
        }
    }
    (*frame)->type = (*frame)->info->type ;

    if (id3GetVersion (tag) < ID3V_V2_4) {	/* ID3v2.3: $xx xx xx xx */
        frameSize = buffer[4] & 0x0FF ;
        frameSize <<= 8 ;
        frameSize |= buffer[5] & 0x0FF ;
        frameSize <<= 8 ;
        frameSize |= buffer[6] & 0x0FF ;
        frameSize <<= 8 ;
        frameSize |= buffer[7] & 0x0FF ;
    } else {					/* ID3v2.4: 4 * %0xxxxxxx */
        frameSize = (size_t) id3DecodeSSI (&buffer[4], 4) ;
    }

    (*frame)->size = frameSize ;

    frameLength = ID3_V2_FRAME_HEADER_SIZE + (*frame)->size ;
    if (skip != NULL)  *skip = frameLength ;

    (*frame)->flags = buffer[8] & 0x0FF ;
    (*frame)->flags <<= 8 ;
    (*frame)->flags |= buffer[9] & 0x0FF ;

    LGI "Frame: \"%.4s\"  (%d) %s  Size: %"PRIuSIZE_T"  Flags: 0x%04lX\n",
        buffer, (int) (*frame)->type, (*frame)->info->name,
        (*frame)->size, (*frame)->flags) ;

/* If the frame is compressed or encrypted, neither of which is supported,
   then simply pass the frame through as-is and do not attempt to decode it. */

    if (((id3GetVersion (tag) == ID3V_V2_3) &&
         ((*frame)->flags & ID3_FRAME_V2_3_COMPRESSED)) ||
        ((id3GetVersion (tag) == ID3V_V2_4) &&
         ((*frame)->flags & ID3_FRAME_V2_4_COMPRESSED))) {
		/* Pass through frame as-is. */
        (*frame)->type = ID3_PASSTHROUGH ;
        (*frame)->info = id3FrameInfoByType (ID3_PASSTHROUGH) ;
        id3FrameDataInit (*frame) ;
        SET_ERRNO (ENOTSUP) ;
        LGE "(id3FrameDecode) Compression not supported; pass through %.4s frame (0x%04lX) as-is.\n",
            buffer, (*frame)->flags) ;
    }

    if (((id3GetVersion (tag) == ID3V_V2_3) &&
         ((*frame)->flags & ID3_FRAME_V2_3_ENCRYPTED)) ||
        ((id3GetVersion (tag) == ID3V_V2_4) &&
         ((*frame)->flags & ID3_FRAME_V2_4_ENCRYPTED))) {
		/* Pass through frame as-is. */
        (*frame)->type = ID3_PASSTHROUGH ;
        (*frame)->info = id3FrameInfoByType (ID3_PASSTHROUGH) ;
        id3FrameDataInit (*frame) ;
        SET_ERRNO (ENOTSUP) ;
        LGE "(id3FrameDecode) Encryption not supported; pass through %.4s frame (0x%04lX) as-is.\n",
            buffer, (*frame)->flags) ;
    }

    if (frameLength > length) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecode) Frame length %"PRIuSIZE_T" exceeds buffer length %"PRIuSIZE_T": ",
            frameLength, length) ;
        PUSH_ERRNO ;  id3FrameDestroy (*frame) ;  *frame = NULL;  POP_ERRNO ;
        return (errno) ;
    }

/* In the case of an ID3v2.4 unsynchronized frame, de-unsynchronize it to get
   the original data back. */

    body = buffer + ID3_V2_FRAME_HEADER_SIZE ;

    if ((id3GetVersion (tag) == ID3V_V2_4) &&
        ((*frame)->flags & ID3_V2_UNSYNCHRONIZED_FRAME)) {
        size_t  numSync = id3DeUnsync (body, (*frame)->size) ;
        LGI "(id3FrameDecode) De-unsynchronized, %"PRIuSIZE_T" bytes less.\n",
            numSync) ;
        (*frame)->size -= numSync ;
        frameLength -= numSync ;
    }

/* If this is not a pass-through frame, then check for decode the optional
   group ID and data length appended to the frame header. */

    if (((*frame)->info->marshalType != Id3MarshalPrivate) &&
        (((id3GetVersion (tag) == ID3V_V2_3) &&
         ((*frame)->flags & ID3_FRAME_V2_3_GROUPED)) ||
        ((id3GetVersion (tag) == ID3V_V2_4) &&
         ((*frame)->flags & ID3_FRAME_V2_4_GROUPED)))) {
        (*frame)->groupID = *body++ ;
        SET_ERRNO (0) ;
        LGE "(id3FrameDecode) No error - frame header with group ID 0x%02X encountered and decoded!\n",
            (unsigned int) (*frame)->groupID) ;
    }

    if (((*frame)->info->marshalType != Id3MarshalPrivate) &&
        (id3GetVersion (tag) == ID3V_V2_4) &&
        ((*frame)->flags & ID3_FRAME_V2_4_DATA_LENGTH)) {
        body += 4 ;		/* Skip it; don't need it; clear flag. */
        (*frame)->flags &= ~ID3_FRAME_V2_4_DATA_LENGTH ;
        SET_ERRNO (0) ;
        LGE "(id3FrameDecode) No error - frame header's appended data length dropped!\n") ;
    }

/* Decode the type-specific frame body. */

#ifdef DEBUG_PURPOSES
    if ((*frame)->type != ID3_ATTACHEDPIC)
        meoDumpX (NULL, "    ", 0, buffer, frameLength) ;
#endif

    if ((*frame)->info->marshalType == Id3MarshalComment) {

        if (id3FrameDecodeComment (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding comment frame.\nid3FrameDecodeComment: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalGenObj) {

        if (id3FrameDecodeGenObj (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding general object frame.\nid3FrameDecodeGenObj: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalPassthrough) {

        if (id3FrameDecodePassthrough (*frame, buffer, frameLength)) {
            LGE "(id3FrameDecode) Error decoding passthrough frame.\nid3FrameDecodePassthrough: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalPicture) {

        if (id3FrameDecodePicture (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding picture frame!\nid3FrameDecodePicture: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalPlayCount) {

        if (id3FrameDecodePlayCount (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding PCNT/POPM frame.\nid3FrameDecodePlayCount: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalPrivate) {

        if (id3FrameDecodePrivate (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding private frame.\nid3FrameDecodePrivate: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalPVList) {

        if (id3FrameDecodePVList (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding list frame!\nid3FrameDecodePVList: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalRegistration) {

        if (id3FrameDecodeRegistration (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding %s frame.\nid3FrameDecodeRegistration: ",
                (*frame)->info->v2_3FrameID) ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if (((*frame)->info->marshalType == Id3MarshalText) ||
               ((*frame)->info->marshalType == Id3MarshalUText)) {

        if (id3FrameDecodeText (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding text frame.\nid3FrameDecodeText: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalUFID) {

        if (id3FrameDecodeUFID (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding UFID frame.\nid3FrameDecodeUFID: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if ((*frame)->info->marshalType == Id3MarshalUnsyncLyrics) {

        if (id3FrameDecodeUSLT (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding USLT frame.\nid3FrameDecodeUSLT: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else if (((*frame)->info->marshalType == Id3MarshalURL) ||
               ((*frame)->info->marshalType == Id3MarshalUURL)) {

        if (id3FrameDecodeURL (*frame, body)) {
            LGE "(id3FrameDecode) Error decoding URL link frame.\nid3FrameDecodeURL: ") ;
            PUSH_ERRNO ;  id3FrameDestroy (*frame) ;
            *frame = NULL ;  POP_ERRNO ;
            return (errno) ;
        }

    } else {

        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecode) Unrecognized frame ID: \"%s\"\n",
            (*frame)->frameID) ;
        PUSH_ERRNO ;  id3FrameDestroy (*frame) ;  *frame = NULL ;  POP_ERRNO ;
        return (errno) ;

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDestroy ()

    Destroy an ID3 Frame.


Purpose:

    The id3FrameDestroy() function frees the dynamically allocated contents
    in an ID3 frame and then the frame structure itself.


    Invocation:

        status = id3FrameDestroy (frame) ;

    where

        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame
            to destroy.
        <status>	- O
            returns the status of destroying the frame, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDestroy (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{

    if (frame == NULL)  return (0) ;

    LGI "(id3FrameDestroy) Destroying %s frame %p ...\n",
        id3FrameTypeName (frame->type), (void *) frame) ;

    id3FrameErase (frame) ;

    adxDestroy (frame->values) ;

    free (frame) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDump ()

    Dump an ID3 Frame.


Purpose:

    The id3FrameDump() function formats and prints an ID3 frame to a file
    for debug purposes.


    Invocation:

        status = id3FrameDump (file, indentation, frame) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame to dump.
        <status>	- O
            returns the status of dumping the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDump (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation,
        Id3Frame  frame)
#    else
        file, indentation, frame)

        FILE  *file ;
        char  *indentation ;
        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    static  char  valueIndent[128] ;
    char  frameIDPrint[12] ;
    Id3Value  *value ;
    size_t  i, count ;



    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    if (frame == NULL) {
        fprintf (file, "%s{ Dump of Frame 0: <nil> }\n", indentation) ;
        return (0) ;
    }

    strcpy (frameIDPrint, "") ;
    if ((frame->type == ID3_TEXT_GENERIC) ||
        (frame->type == ID3_WURL_GENERIC)) {
        if (frame->frameID == NULL)
            strcpy (frameIDPrint, " (<null>)") ;
        else
            sprintf (frameIDPrint, " (%s)", frame->frameID) ;
    }

    count = adxCount (frame->values) ;

    if (count == 0) {
        fprintf (file, "%s{ Dump of %s%s Frame %p%s:\n",
                 indentation,
                 id3FrameTypeName (frame->type),
                 frameIDPrint,
                 (void *) frame,
                 frame->delete ? " (Overlay Delete)" : "") ;
    } else {
        fprintf (file, "%s{ Dump of %s%s Frame %p%s (%"PRIuSIZE_T" values):\n",
                 indentation,
                 id3FrameTypeName (frame->type),
                 frameIDPrint,
                 (void *) frame,
                 frame->delete ? " (Overlay Delete)" : "",
                 count) ;
    }

    strncpym (valueIndent, indentation,		/* Add 4 spaces to indent. */
              sizeof valueIndent, sizeof valueIndent) ;
    strncatm (valueIndent, "    ",
              sizeof valueIndent, sizeof valueIndent) ;

    if (frame->info->marshalType == Id3MarshalPicture) {
        fprintf (file, "%sMIME: \"%s\"  Type: %s  Length: %"PRIuSIZE_T"  Description: \"%s\"\n",
                 valueIndent,
                 frame->fdata.apic.mimeType,
                 id3FromPictureType (frame->fdata.apic.type),
                 frame->fdata.apic.length,
                 frame->fdata.apic.description) ;
    } else if (frame->info->marshalType == Id3MarshalComment) {
        fprintf (file, "%sLanguage: \"%s\"  Description: \"%s\"  Comment: \"%s\"\n",
                 valueIndent,
                 frame->fdata.comm.language,
                 frame->fdata.comm.description,
                 frame->fdata.comm.text) ;
    } else if (frame->info->marshalType == Id3MarshalGenObj) {
        fprintf (file, "%sMIME: \"%s\"  File: \"%s\"  Length: %"PRIuSIZE_T"  Description: \"%s\"\n",
                 valueIndent,
                 frame->fdata.geob.mimeType,
                 frame->fdata.geob.fileName,
                 frame->fdata.geob.length,
                 frame->fdata.geob.description) ;
    } else if (frame->info->marshalType == Id3MarshalPassthrough) {
        fprintf (file, "%sPassthrough: \"%s\"  Full Length: %"PRIuSIZE_T"\n",
                 valueIndent,
                 frame->frameID,
                 frame->fdata.ptru.length) ;
        meoDumpX (file, valueIndent, 0,
                  frame->fdata.ptru.buffer, frame->fdata.ptru.length) ;
    } else if (frame->info->marshalType == Id3MarshalPlayCount) {
        if (frame->type != ID3_PLAYCOUNT) {
            fprintf (file, "%sEmail: \"%s\"  Rating: %u\n",
                     valueIndent,
                     (frame->fdata.play.email == NULL)
                         ? "(nil)" : frame->fdata.play.email,
                     frame->fdata.play.rating) ;
        }
        fprintf (file, "%sCounter: %zu\n",
                 valueIndent,
                 frame->fdata.play.counter) ;
    } else if (frame->info->marshalType == Id3MarshalPrivate) {
        fprintf (file, "%sOwner: \"%s\"  Length: %"PRIuSIZE_T"\n",
                 valueIndent,
                 frame->fdata.priv.ownerID,
                 frame->fdata.priv.length) ;
        meoDumpX (file, valueIndent, 0,
                  frame->fdata.priv.data, frame->fdata.priv.length) ;
    } else if (frame->info->marshalType == Id3MarshalPVList) {
        fprintf (file, "%sRole: \"%s\"\n",
                 valueIndent,
                 frame->fdata.text.text) ;
    } else if (frame->info->marshalType == Id3MarshalRegistration) {
        fprintf (file, "%sOwner: \"%s\"  Symbol: 0x%02X  Length: %"PRIuSIZE_T"\n",
                 valueIndent,
                 frame->fdata.regi.ownerID,
                 frame->fdata.regi.symbol,
                 frame->fdata.regi.length) ;
        meoDumpX (file, valueIndent, 0,
                  frame->fdata.regi.data, frame->fdata.regi.length) ;
    } else if (frame->info->marshalType == Id3MarshalText) {
        fprintf (file, "%sText: \"%s\"\n",
                 valueIndent,
                 frame->fdata.text.text) ;
    } else if (frame->info->marshalType == Id3MarshalUFID) {
        fprintf (file, "%sOwner: \"%s\"  Length: %"PRIuSIZE_T"\n",
                 valueIndent,
                 frame->fdata.ufid.ownerID,
                 frame->fdata.ufid.length) ;
        meoDumpX (file, valueIndent, 0,
                  frame->fdata.ufid.fileID, frame->fdata.ufid.length) ;
    } else if (frame->info->marshalType == Id3MarshalUnsyncLyrics) {
        fprintf (file, "%sLanguage: \"%s\"  Description: \"%s\"  Lyrics:\n\"%s\"\n",
                 valueIndent,
                 frame->fdata.uslt.language,
                 frame->fdata.uslt.description,
                 frame->fdata.uslt.lyrics) ;
    } else if (frame->info->marshalType == Id3MarshalURL) {
        fprintf (file, "%sURL: \"%s\"\n",
                 valueIndent,
                 frame->fdata.wurl.url) ;
    } else if (frame->info->marshalType == Id3MarshalUText) {
        fprintf (file, "%sDescription: \"%s\"\n",
                 valueIndent,
                 frame->fdata.text.description) ;
        fprintf (file, "%sText: \"%s\"\n",
                 valueIndent,
                 frame->fdata.text.text) ;
    } else if (frame->info->marshalType == Id3MarshalUURL) {
        fprintf (file, "%sDescription: \"%s\"\n",
                 valueIndent,
                 frame->fdata.wurl.description) ;
        fprintf (file, "%sURL: \"%s\"\n",
                 valueIndent,
                 frame->fdata.wurl.url) ;
    }

    for (i = 0 ;  i < count ;  i++) {
        value = adxGet (frame->values, i) ;
        id3ValueDump (file, valueIndent, *value) ;
    }

    fprintf (file, "%s}\n", indentation) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDumpLUT ()

    Dump the Frame Information Lookup Table.


Purpose:

    The id3FrameDumpLUT() function formats and prints the frame information
    lookup table.  The only fields printed are the ID3v2.3 frame ID, the
    ID3v2.4 frame ID, and the frame type name.  In the case of the TXXX
    and WXXX, user-defined frames, the description field is appended,
    enclosed in parentheses.  For example:

        ...
        COMM  COMM  COMMENT
        TCMP  TCMP  COMPILATION
        ...
        TXXX  TXXX  MUSICBRAINZ_ALBUMID (MusicBrainz Album Id)
        ...
        TYER  TDRC  YEAR


    Invocation:

        status = id3FrameDumpLUT (file, indentation) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <status>	- O
            returns the status of dumping the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameDumpLUT (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation)
#    else
        file, indentation)

        FILE  *file ;
        char  *indentation ;
#    endif

{    /* Local variables. */
    const  char  *colon, *description, *v2_3ID, *v2_4ID ;
    size_t  i ;



    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    for (i = 1 ;  id3FrameLUT[i].type != ID3_NULL_FRAME_TYPE ;  i++) {

        v2_3ID = id3FrameLUT[i].v2_3FrameID ;
        v2_4ID = id3FrameLUT[i].v2_4FrameID ;

        colon = description = NULL ;

        if (id3FrameLUT[i].marshalType == Id3MarshalUText) {

            if (v2_3ID != NULL)
                colon = strchr (v2_3ID, ':') ;
            if ((colon == NULL) && (v2_4ID != NULL))
                colon = strchr (v2_4ID, ':') ;
            if (colon != NULL)  description = colon + 1 ;
            v2_3ID = v2_4ID = "TXXX" ;

        } else if (id3FrameLUT[i].marshalType == Id3MarshalUURL) {

            if (v2_3ID != NULL)
                colon = strchr (v2_3ID, ':') ;
            if ((colon == NULL) && (v2_4ID != NULL))
                colon = strchr (v2_4ID, ':') ;
            if (colon != NULL)  description = colon + 1 ;
                v2_3ID = v2_4ID = "WXXX" ;

        }

        if (v2_3ID == NULL)  v2_3ID = "    " ;
        if (v2_4ID == NULL)  v2_4ID = "    " ;

        if (description == NULL) {
            fprintf (file, "%s%.4s  %.4s  %s\n",
                     indentation,
                     v2_3ID,
                     v2_4ID,
                     id3FrameLUT[i].name) ;
        } else {
            fprintf (file, "%s%.4s  %.4s  %s  (%s)\n",
                     indentation,
                     v2_3ID,
                     v2_4ID,
                     id3FrameLUT[i].name,
                     description) ;
        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncode ()

    Encode an ID3v2 Frame into its File Format.


Purpose:

    Function id3FrameEncode() encodes an ID3v2 frame into a byte buffer.


    Invocation:

        status = id3FrameEncode (tag, frame, bdx, &length) ;

    where

        <tag>		- I
            is the handle for the frame's parent tag.
        <frame>		- I
            is the handle for the ID3 frame to be encoded.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The encoded frame will be
            appended to the current contents of the buffer.
        <length>	- O
            returns the length in bytes of the encoded frame, including
            the frame header.
        <status>	- O
            returns the status of encoding the frame, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameEncode (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        tag, frame, bdx, length)

        Id3Tag  tag ;
        Id3Frame  frame ;
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    char  *body, *buf, *header ;
    const  char  *prefix ;
    size_t  bodyLength = 0, headerOffset, len, numSync ;



    if ((frame == NULL) || (tag == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncode) NULL frame, parent tag, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    *length = 0 ;

    if (frame->delete) {
        LGI "(id3FrameEncode) Skipping overlay-delete %s frame %p.\n",
            id3FrameTypeName (frame->type),
            (void *) frame) ;
        return (0) ;
    }

/* Initialize the 10-byte frame header.  The frame size will be encoded and
   filled in later after the full length of the frame is known. */

    headerOffset = mdxLength (bdx) ;	/* Offset past buffer's prior contents. */

    if (mdxAddS (bdx, false, NULL, ID3_V2_FRAME_HEADER_SIZE)) {
        LGE "(id3FrameEncode) Error adding ID3v2 frame header for frame %p.\nmdxAddS: ",
            (void *) frame) ;
        return (errno) ;
    }

    header = mdxRegionP (bdx, headerOffset) ;	/* Location of new header. */

    memset (header, 0, ID3_V2_FRAME_HEADER_SIZE) ;

    if ((frame->type == ID3_TEXT_GENERIC) && (frame->frameID != NULL))
        prefix = frame->frameID ;
    else if ((frame->type == ID3_WURL_GENERIC) && (frame->frameID != NULL))
        prefix = frame->frameID ;
    else if ((id3GetVersion (frame->parent) < ID3V_V2_4) &&
             (frame->info->v2_3FrameID != NULL))
        prefix = frame->info->v2_3FrameID ;
    else if (frame->info->v2_4FrameID != NULL)
        prefix = frame->info->v2_4FrameID ;
    else
        prefix = "ABCD" ;

    strncpy (&header[0], prefix, 4) ;

    header[8] = 0 ;			/* Flags. */
    header[9] = 0 ;

/* If the frame is flagged with a group ID, add the group ID to the header. */

    if ((frame->info->marshalType != Id3MarshalPrivate) &&
        (((id3GetVersion (tag) == ID3V_V2_3) &&
         (frame->flags & ID3_FRAME_V2_3_GROUPED)) ||
        ((id3GetVersion (tag) == ID3V_V2_4) &&
         (frame->flags & ID3_FRAME_V2_4_GROUPED)))) {
        if (mdxAddB (bdx, frame->groupID)) {
            LGE "(id3FrameEncode) Error adding group ID 0x%02X for frame %p.\nmdxAddB: ",
                (unsigned int) frame->groupID, (void *) frame) ;
            return (errno) ;
        }
        if (id3GetVersion (tag) == ID3V_V2_3) {
            header[9] |= ID3_FRAME_V2_3_GROUPED & 0x0FF ;
        } else {
            header[9] |= ID3_FRAME_V2_4_GROUPED & 0x0FF ;
        }
    }

/* Encode the data fields in the frame. */

    if (frame->info->marshalType == Id3MarshalComment) {

        if (id3FrameEncodeComment (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding comment frame.\nid3FrameEncodeComment: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalGenObj) {

        if (id3FrameEncodeGenObj (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding general object frame.\nid3FrameEncodePicture: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalPassthrough) {

		/* Copy stored frame header and body as-is to buffer. */

        memcpy (header, frame->fdata.ptru.buffer, ID3_V2_FRAME_HEADER_SIZE) ;

        bodyLength = frame->fdata.ptru.length - ID3_V2_FRAME_HEADER_SIZE ;

        if (mdxAddS (bdx, false,
                     frame->fdata.ptru.buffer + ID3_V2_FRAME_HEADER_SIZE,
                     bodyLength)) {
            LGE "(id3FrameEncode) Error adding %"PRIuSIZE_T"-byte passthrough body for frame %p.\nmdxAddS: ",
                bodyLength, (void *) frame) ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalPicture) {

        if (id3FrameEncodePicture (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding picture frame!\nid3FrameEncodePicture: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalPlayCount) {

        if (id3FrameEncodePlayCount (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding PCNT/POPM frame.\nid3FrameEncodePlayCount: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalPrivate) {

        if (id3FrameEncodePrivate (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding private frame.\nid3FrameEncodePrivate: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalPVList) {

        if (id3FrameEncodePVList (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding list frame!\nid3FrameEncodePVList: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalRegistration) {

        if (id3FrameEncodeRegistration (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding %s frame.\nid3FrameEncodeRegistration: ",
                frame->info->v2_3FrameID) ;
            return (errno) ;
        }

    } else if ((frame->info->marshalType == Id3MarshalText) ||
               (frame->info->marshalType == Id3MarshalUText)) {

        if (id3FrameEncodeText (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding text frame.\nid3FrameEncodeText: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalUFID) {

        if (id3FrameEncodeUFID (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding UFID frame.\nid3FrameEncodeUFID: ") ;
            return (errno) ;
        }

    } else if (frame->info->marshalType == Id3MarshalUnsyncLyrics) {

        if (id3FrameEncodeUSLT (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding USLT frame.\nid3FrameEncodeUSLT: ") ;
            return (errno) ;
        }

    } else if ((frame->info->marshalType == Id3MarshalURL) ||
               (frame->info->marshalType == Id3MarshalUURL)) {

        if (id3FrameEncodeURL (frame, bdx, &bodyLength)) {
            LGE "(id3FrameEncode) Error encoding URL link frame.\nid3FrameEncodeURL: ") ;
            return (errno) ;
        }

    } else {

        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncode) Invalid marshaling type %d, frame ID \"%s\".\n",
            (int) frame->info->marshalType, frame->frameID) ;
        return (errno) ;

    }



/* If this is an ID3v2.4 tag and frames are to be unsynchronized, then do so.
   If the unsynchronization changes the contents of the frame (i.e., there
   were spurious MP3 frame sync codes that needed to be encoded), then flag
   the frame as unsynchronized and increment the unsynchronized frame count
   in the tag.  (In ID3v2.4 tags, the tag-level unsynchronization flag is
   not set if no frames required unsynchonization.)

   On the off chance that the last frame in a tag ends with a 0xFF byte and
   that the byte after the tag completes a false sync code, the ID3 standard
   recommends adding explicit padding or a footer to avoid such a false sync
   code.  It's simpler, however, for id3Unsync() to just add implicit padding
   (of one byte) to the frame, which will be removed when the frame is
   de-unsynchronized.  (The extra 0x00 byte is added to any unsynchronized
   frame ending in 0xFF, not just the last frame in the tag.) */

    frame->flags &= ~ID3_V2_UNSYNCHRONIZED_FRAME ;		/* Clear bit. */

    if ((id3GetVersion (tag) == ID3V_V2_4) && tag->unsync) {

        body = (char *) mdxRegionP (bdx, headerOffset) +
               ID3_V2_FRAME_HEADER_SIZE ;
        len = mdxLength (bdx) - headerOffset - ID3_V2_FRAME_HEADER_SIZE ;
        buf = id3Unsync (body, len, &numSync) ;
        if (buf == NULL) {
            LGE "(id3FrameEncode) Error unsynchronizing frame %p.\nid3Unsync: ",
                (void *) frame) ;
            return (errno) ;
        }

        LGI "(id3FrameEncode) Unsynchronized, %"PRIuSIZE_T" bytes more.\n",
            numSync) ;

        if (numSync > 0) {
            mdxSetLength (bdx, headerOffset + ID3_V2_HEADER_SIZE) ;
            if (mdxAddS (bdx, false, buf, len + numSync)) {
                LGE "(id3FrameEncode) Error storing %"PRIuSIZE_T"-byte unsynchronizing body for frame %p.\nmdxAddS: ",
                    len + numSync, (void *) frame) ;
                PUSH_ERRNO ;  free (buf) ;  POP_ERRNO ;
                return (errno) ;
            }
            free (buf) ;
            frame->flags |= ID3_V2_UNSYNCHRONIZED_FRAME ;	/* Set bit. */
            bodyLength = len + numSync ;
            tag->numUnsyncFrames++ ;
        }

    }

/* Now that it's known, encode and store the frame size in the header. */

    header = mdxRegionP (bdx, headerOffset) ;

					/* Frame size excludes header. */
    if (id3GetVersion (tag) < ID3V_V2_4) {	/* ID3v2.3: $xx xx xx xx */
        header[4] = (bodyLength >> 24) & 0x0FF ;
        header[5] = (bodyLength >> 16) & 0x0FF ;
        header[6] = (bodyLength >> 8) & 0x0FF ;
        header[7] = bodyLength & 0x0FF ;
    } else {					/* ID3v2.4: 4 * %0xxxxxxx */
        id3EncodeSSI (bodyLength, 4, &header[4]) ;
    }

    header[8] = (frame->flags & 0x0FF00) >> 8 ;	/* Flags. */
    header[9] = (frame->flags & 0x000FF) ;

					/* Return full header+body length. */
    *length = ID3_V2_FRAME_HEADER_SIZE + bodyLength ;

    LGI "(id3FrameEncode) %s frame %p encoded: %"PRIuSIZE_T" bytes.\n",
        id3FrameTypeName (frame->type), (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameErase ()

    Erase the Contents of an ID3 Frame.


Purpose:

    The id3FrameErase() function frees the dynamically allocated contents
    in a frame and reinitializes the frame to its empty state, leaving the
    now-empty array of values intact.


    Invocation:

        status = id3FrameErase (frame) ;

    where

        <frame>		- I
            is the handle, returned by id3FrameCreate(), of the frame to erase.
        <status>	- O
            returns the status of erasing the frame , zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FrameErase (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    Id3Value  *value ;
    size_t  i ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameErase) NULL frame handle: ") ;
        return (errno) ;
    }

    LGI "(id3FrameErase) Erasing %s frame %p ...\n",
        id3FrameTypeName (frame->type), (void *) frame) ;

/* Delete frame-type-specific fields in the frame. */

    id3FrameDataErase (frame) ;

/* Delete the field values, if any, in the frame.  Remember that
   the array of values is an array of handles-for/pointers-to value
   structures and that adxGet() returns the address of a handle;
   hence there is an extra level of indirection.  NOTE that the
   existing array descriptor for the values is left in place,
   albeit now with zero elements. */

    i = adxCount (frame->values) ;
    while (i-- > 0) {
        value = adxGet (frame->values, i) ;
        id3ValueDestroy (*value) ;
        adxDelete (frame->values, 1, i) ;
    }

/* Reinitialize the rest of the frame. */

    frame->type = ID3_NULL_FRAME_TYPE ;
    frame->parent = NULL ;

    if (frame->frameID != NULL)  free (frame->frameID) ;
    frame->frameID = NULL ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameGetValue ()

    Get the I-th Value in an ID3 Frame.


Purpose:

    The id3FrameGetValue() function returns the I-th value in an ID3 frame.
    Index I is in the range 0..N-1, where N is the number of values in the
    frame.  Normally there is one value in a frame, but some text frame
    types allow multiple values within a single frame.


    Invocation:

        value = id3FrameGetValue (frame, index) ;

    where

        <frame>		- I
            is the frame handle returned by id3FrameCreate().
        <index>		- I
            is the index 0..N-1 of the desired value, where N is the number
            of values in the frame.
        <value>		- O
            returns the value handle; NULL is returned if the index is
            out of range.

*******************************************************************************/


Id3Value  id3FrameGetValue (

#    if PROTOTYPES
        Id3Frame  frame,
        size_t  which)
#    else
        frame, which)

        Id3Frame  frame ;
        size_t  which ;
#    endif

{

    if ((frame == NULL) || (frame->values == NULL)) {
        return (NULL) ;
    } else {
        Id3Value  *value = adxGet (frame->values, (ssize_t) which) ;
        return ((value == NULL) ? NULL : *value) ;
    }

}

/*!*****************************************************************************

Procedure:

    id3FrameInfoByID ()

    Lookup Frame Information by ID.


Purpose:

    Function id3FrameInfoByID() looks up frame information using a frame ID.
    Frame IDs are the 4-character ASCII identifiers at the beginning of every
    frame; e.g., "TALB", "TTRCK", "COMM", etc.


    Invocation:

        entry = id3FrameInfoByID (frameID) ;

    where

        <frameID>	- I
            is the 4-character identifier from the frame.  The function
            only examines the first 4 characters of the string, so the
            frame header can be passed in directly regardless of whether
            or not a NUL character is in the 5th byte.
        <entry>		- O
            returns a pointer to the frame type's entry in the frame
            information table; NULL is returned if no matching frame
            type is found.

*******************************************************************************/


Id3FrameLUTEntry  *id3FrameInfoByID (

#    if PROTOTYPES
        const  char  *frameID)
#    else
        frameID)

        char  *frameID ;
#    endif

{    /* Local variables. */
    const  char  *s ;
    size_t  i ;



    for (i = 1 ;  id3FrameLUT[i].type != ID3_NULL_FRAME_TYPE ;  i++) {
        s = id3FrameLUT[i].v2_3FrameID ;	/* Matching v2.3 ID? */
        if ((s != NULL) && (strncmp (frameID, s, 4) == 0))
            return (&id3FrameLUT[i]) ;
        s = id3FrameLUT[i].v2_4FrameID ;	/* Matching v2.4 ID? */
        if ((s != NULL) && (strncmp (frameID, s, 4) == 0))
            return (&id3FrameLUT[i]) ;
    }

    return (NULL) ;				/* Not found. */

}

/*!*****************************************************************************

Procedure:

    id3FrameInfoByName ()

    Lookup Frame Information by Frame Type Name.


Purpose:

    Function id3FrameInfoByName() looks up frame information using the frame
    type name; i.e., the <name> part of the enumerated ID3_<name> frame types.
    The name matching is case-insensitive.


    Invocation:

        entry = id3FrameInfoByName (typeName) ;

    where

        <typeName>	- I
            is the frame type name.  The frame types are enumerated in
            "id3_util.h" as ID3_<name>.  For example, ID3_ARTIST is
            the enumerated type for the artist's name and "ARTIST" is
            the type name (case-insensitive).
        <entry>		- O
            returns a pointer to the frame type's entry in the frame
            information table; NULL is returned if no matching frame
            type is found.

*******************************************************************************/


Id3FrameLUTEntry  *id3FrameInfoByName (

#    if PROTOTYPES
        const  char  *typeName)
#    else
        typeName)

        char  *typeName ;
#    endif

{    /* Local variables. */
    size_t  i ;



    for (i = 1 ;  id3FrameLUT[i].type != ID3_NULL_FRAME_TYPE ;  i++) {
        if (strcasecmp (typeName, id3FrameLUT[i].name) == 0)
            return (&id3FrameLUT[i]) ;		/* Found. */
    }

    return (NULL) ;				/* Not found. */

}

/*!*****************************************************************************

Procedure:

    id3FrameInfoByType ()

    Lookup Frame Information by Frame Type.


Purpose:

    Function id3FrameInfoByType() looks up frame information using one of
    the frame types defined in "id3_util.h"; e.g., ID3_ALBUM, ID3_TRACK,
    ID3_COMMENT, etc.


    Invocation:

        entry = id3FrameInfoByType (frameType) ;

    where

        <frameType>	- I
            is the frame type (defined in "id3_util.h").
        <entry>		- O
            returns a pointer to the frame type's entry in the frame
            information table; NULL is returned if no matching frame
            type is found.

*******************************************************************************/


Id3FrameLUTEntry  *id3FrameInfoByType (

#    if PROTOTYPES
        Id3FrameType  frameType)
#    else
        frameType)

        Id3FrameType  frameType ;
#    endif

{    /* Local variables. */
    size_t  i ;



    for (i = 1 ;  id3FrameLUT[i].type != ID3_NULL_FRAME_TYPE ;  i++) {
        if (id3FrameLUT[i].type == frameType)		/* Found? */
            return (&id3FrameLUT[i]) ;
    }

    return (NULL) ;					/* Not found. */

}

/*!*****************************************************************************

Procedure:

    id3FrameParsePair ()

    Parse the Next Pair of Values in a String.


Purpose:

    The id3FrameParsePair() function parses and returns the next pair
    of values in a list specification string according to the following
    syntax:

        "<value1>:<value2>;"


    Invocation:

        eol = id3FrameParsePair (listSpec, &value1, &length1,
                                 &value2, &length2, &next) ;

    where

        <listSpec>	- I
            is the list specification string in the format described above.
        <value1>	- O
            returns a pointer to the first value in the list specification
            string.
        <length1>	- O
            returns the length of the first value.
        <value2>	- O
            returns a pointer to the second value in the list specification
            string.
        <length2>	- O
            returns the length of the second value.
        <next>		- O
            returns a pointer to the character after the semicolon in the
            list specification string.  This can be used in the next call
            to id3FrameParsePair() to get the next pair of values.
        <eol>		- O
            returns false if a pair of values was found and true if the
            end of the list specification was reached.

*******************************************************************************/


bool  id3FrameParsePair (

#    if PROTOTYPES
        const  char  *listSpec,
        char  **value1,
        size_t  *length1,
        char  **value2,
        size_t  *length2,
        char  **next)
#    else
        listSpec, value1, length1, value2, length2, next)

        char  *listSpec ;
        char  **value1 ;
        size_t  *length1 ;
        char  **value2 ;
        size_t  *length2 ;
        char  **next ;
#    endif

{    /* Local variables. */
    size_t  offsetC, offsetS ;



    if (listSpec == NULL) {
        *value1 = NULL ;  length1 = 0 ;
        *value2 = NULL ;  length2 = 0 ;
        *next = NULL ;
        return (true) ;
    }

    offsetC = strcspn (listSpec, ":") ;
    offsetS = strcspn (listSpec, ";") ;

    if (offsetS < offsetC)  offsetC = offsetS ;

    *value1 = (char *) listSpec ;
    *length1 = offsetC ;

    if (listSpec[offsetC] == ':') {
        *value2 = (char *) &listSpec[offsetC+1] ;
        *length2 = offsetS - offsetC - 1 ;
    } else {
        *value2 = NULL ;
        *length2 = 0 ;
    }

    if (listSpec[offsetS] == ';') {
        *next = (char *) &listSpec[offsetS+1] ;
        if (**next == '\0')  *next = NULL ;
    } else if (listSpec[offsetS] != '\0') {
        *next = (char *) &listSpec[offsetS] ;
    } else {
        *next = NULL ;
    }

    return ((*length1 == 0) && (*length2 == 0)) ;

}

/*!*****************************************************************************

Procedure:

    id3FramePVLfromS ()

    Constructs a Paired-Values List From a String.


Purpose:

    The id3FramePVLfromS() function parses a list specification string
    and adds the parsed value pairs to a paired-values list frame.

    Paired-values lists are used for ID3 "involved people list" frames
    and the specification string looks as follows:

        "<role1>:<person1>;<role2>:<person2>;...<roleN>:<personN>"

    where <role> is loosely defined depending on the particular frame type.
    For example, <role> is an apt name for ID3v2.3's "involved people list"
    frame (IPLS), but refers to musical instruments in ID3v2.4's "musician
    credits list" frame (TMCL).


    Invocation:

        status = id3FramePVLfromS (frame, listSpec, replace) ;

    where

        <frame>		- I
            is the frame handle returned by id3FrameCreate().
        <listSpec>	- I
            is the list specification string in the format described above.
        <replace>	- I
            indicates whether or not the existing elements in the list
            should be replaced.  If this argument is true, the paired
            values currently in the list are deleted before the new
            values are added.  If this argument is false, the new
            values are simply added to the existing list.
        <status>	- O
            returns the status of parsing the specification string and
            adding new pairs to the frame's paired-values list, zero if
            there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3FramePVLfromS (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *listSpec,
        bool  replace)
#    else
        frame, listSpec, replace)

        Id3Frame  frame ;
        char  *listSpec ;
        bool  replace ;
#    endif

{    /* Local variables. */
    char  *begin, *field1, *field2, *next ;
    size_t  length1, length2, space ;



    if ((frame == NULL) || (listSpec == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FramePVLfromS) NULL frame handle or list specification: ") ;
        return (errno) ;
    }

    if (frame->info->marshalType != Id3MarshalPVList) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FramePVLfromS) %s frame %p is not a paired-values list.\n",
            frame->info->name, (void *) frame) ;
        return (errno) ;
    }

/* Parse and add each pair of values in the list specification string
  to the frame's paired-values list. */

    begin = (char *) listSpec ;

    while (!id3FrameParsePair (begin, &field1, &length1,
                               &field2, &length2, &next)) {

        begin = next ;

        if (((field1 == NULL) || (length1 == 0)) &&
            ((field2 == NULL) || (length2 == 0))) {
            continue ;			/* Empty pair, go to next pair. */
        }

        if (field1 == NULL)  field1 = "" ;
        space = strspn (field1, " \t") ;	/* Trim leading blanks. */
        field1 += space ; length1 -= space ;

        if (field2 == NULL)  field2 = "" ;
        space = strspn (field2, " \t") ;	/* Trim leading blanks. */
        field2 += space ; length2 -= space ;

        LGI "(id3FramePVLfromS) Adding pair \"%.*s\" : \"%.*s\" to %s frame %p.\n",
            (int) length1, field1, (int) length2, field2,
            frame->info->name, (void *) frame) ;

        if (frame->fdata.text.text == NULL) {
            frame->fdata.text.text = strndup (field1, length1) ;
            if (frame->fdata.text.text == NULL) {
                LGE "(id3FramePVLfromS) Error duplicating \"%.*s\" for %s frame %p.\nstrndup: ",
                    (int) length1, field1, frame->info->name, (void *) frame) ;
                return (errno) ;
            }
        } else {
            if (id3FrameAddStringV (frame, field1, length1)) {
                LGE "(id3FramePVLfromS) Error adding \"%.*s\" to %s frame %p.\nid3FrameAddStringV: ",
                    (int) length1, field1, frame->info->name, (void *) frame) ;
                return (errno) ;
            }
        }

        if (id3FrameAddStringV (frame, field2, length2)) {
            LGE "(id3FramePVLfromS) Error adding \"%.*s\" to %s frame %p.\nid3FrameAddStringV: ",
                (int) length2, field2, frame->info->name, (void *) frame) ;
            return (errno) ;
        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FramePVLtoS ()

    Format a Paired-Values List as a String.


Purpose:

    The id3FramePVLtoS() function creates a list specification string
    for a paired-values list.  This is a single string containing the
    entire list in a format that can be parsed by the id3FramePVLfromS()
    function.

    Paired-values lists are used for ID3 "involved people list" frames
    and the specification string looks as follows:

        "<role1>:<person1>;<role2>:<person2>;...<roleN>:<personN>"


    Invocation:

        listSpec = id3FramePVLtoS (frame) ;

    where

        <frame>		- I
            is the frame handle returned by id3FrameCreate().
        <listSpec>	- O
            returns the specification string as a dynamically allocated
            string; the caller is responsible for free(3)ing the string
            after it is no longer neede.  NULL is returned in the event
            of an error.

*******************************************************************************/


char  *id3FramePVLtoS (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    char  *listSpec ;
    Id3Value  *value ;
    size_t  count, i ;
    StringDx  sdx ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FramePVLtoS) NULL frame handle: ") ;
        return (NULL) ;
    }

    if (frame->info->marshalType != Id3MarshalPVList) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FramePVLtoS) %s frame %p is not a paired-values list.\n",
            frame->info->name, (void *) frame) ;
        return (NULL) ;
    }

/* Create a descriptor for a dynamically growing specification string
   and initialize it with the first role value. */

    if (sdxCreate (frame->fdata.text.text, -1, SdxDynamic, &sdx)) {
        LGE "(id3FramePVLtoS) Error creating specification string descriptor.\nsdxCreate: ") ;
        return (NULL) ;
    }

/* Add the frame's remaining string values to the specification string.
   Since the initial role field comes from the text string above, an
   even-indexed value (such as the 0-th value) is the person playing
   the preceding field's role and the succeeding odd-indexed value is
   the role for the next person, ultimately giving a sequence of
   "<role>:<person>;"s. */

    count = adxCount (frame->values) ;
    for (i = 0 ;  i < count ;  i++) {
        value = adxGet (frame->values, i) ;
        if (sdxAddC (sdx, (i % 2) ? ';' : ':')) {
            LGE "(id3FramePVLtoS) Error adding field separator to specification string.\nsdxAddC: ") ;
            PUSH_ERRNO ;  sdxDestroy (sdx) ;  POP_ERRNO ;
            return (NULL) ;
        }
        if (sdxAddS (sdx, (*value)->vdata.text, -1)) {
            LGE "(id3FramePVLtoS) Error adding field \"%s\" to specification string.\nsdxAddS: ") ;
            PUSH_ERRNO ;  sdxDestroy (sdx) ;  POP_ERRNO ;
            return (NULL) ;
        }
    }

/* Return the dynamically allocated specification string to the caller. */

    sdxOwn (sdx) ;
    listSpec = sdxStringZ (sdx) ;
    sdxDestroy (sdx) ;

    return (listSpec) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameTypeName ()

    Get the Text Name for an ID3 Frame Type.


Purpose:

    The id3FrameTypeName() function returns the name of an ID3 frame type;
    e.g., "ARTIST" for type ID3_ARTIST.


    Invocation:

        name = id3FrameTypeName (frameType) ;

    where

        <frameType>	- I
            is the frame type (defined in "id3_util.h").
        <name>		- O
            returns the name string for the frame type.  If an invalid type
            is given, a string, "(<number>)", is returned.  The name string
            is stored internally and it should not be modified or deleted
            by the calling program.

*******************************************************************************/


const  char  *id3FrameTypeName (

#    if PROTOTYPES
        Id3FrameType  frameType)
#    else
        frameType)

        Id3FrameType  frameType ;
#    endif

{    /* Local variables. */
    Id3FrameLUTEntry  *entry ;



    entry = id3FrameInfoByType (frameType) ;
    if (entry == NULL) {
        static  char  number[16] ;
        sprintf (number, "(%ld)", (long) frameType) ;
        return (number) ;
    } else {
        return (entry->name) ;
    }

}

/*!*****************************************************************************

Procedure:

    id3FrameUDDescOf ()

    Lookup the Description for a User-Defined ID3 Frame Type.


Purpose:

    The id3FrameUDDescOf() function looks up a predefined, user-defined
    frame type in the frame information table and, if found, returns the
    corresponding description.

    Terms:

        User-Defined Frame Types - have frame IDs of "TXXX" or "WXXX".

        Non-Predefined UD Types - are ID3_TXXX_GENERIC and ID3_WXXX_GENERIC.

        Predefined UD Types - have types distinct from the generic types,
            but are still decoded/encoded as "TXXX" and "WXXX" frames.
            In the frame information table, the predefined frame IDs
            appear as "TXXX:<description>" or "WXXX:<description>".

    If the caller-specified frame type is a predefined UD type, the
    <description> string from the frame information table is returned.


    Invocation:

        description = id3FrameUDDescOf (frameType) ;

    where

        <frameType>	- I
            is the enumerated frame type under consideration.  Frame
            types are defined in "id3_util.h".
        <description>	- O
            returns the description string for the frame type; NULL is
            returned if the frame type has no description string (or is
            not a predefined UD type).

*******************************************************************************/


const  char  *id3FrameUDDescOf (

#    if PROTOTYPES
        Id3FrameType  frameType)
#    else
        frameType)

        Id3FrameType  frameType ;
#    endif

{    /* Local variables. */
    const  char  *frameID ;
    Id3FrameLUTEntry  *info ;



    info = id3FrameInfoByType (frameType) ;
    if (info == NULL)  return (NULL) ;

    frameID = info->v2_3FrameID ;
    if (frameID == NULL)  frameID = info->v2_4FrameID ;

    if ((frameID != NULL) && (frameID[4] == ':'))
        return (&frameID[5]) ;		/* "T/WXXX:<description>" */
    else
        return (NULL) ;			/* No description. */

}

/*!*****************************************************************************

Procedure:

    id3FrameUDTypeOf ()

    Lookup a User-Defined ID3 Frame Type.


Purpose:

    The id3FrameUDTypeOf() function looks up a user-defined frame
    ID/description and returns the more specific frame type, if found.
    The user-defined IDs are "TXXX" for user-defined text frames and
    "WXXX" for user-defined URLs.  The Mp3tag editor provides separately
    named types for qualified TXXX frames, which are annotated with
    "TXXX:<description>".

    For example, the MusicBrainz disc ID frame, of type MUSICBRAINZ_DISCID,
    has an annotated frame ID of "TXXX:MusicBrainz Disc Id".  When a TXXX
    frame with a description string of "MusicBrainz Disc Id" is decoded,
    id3FrameDecodeUserText() can look up the description and change the
    the frame type from ID3_TXXX_GENERIC to ID3_MUSICBRAINZ_DISCID.

    (Mp3tag currently doesn't define any qualified types for the user-defined,
    WXXX URL frames, but just in case...)


    Invocation:

        frameType = id3FrameUDTypeOf (frameID, description) ;

    where

        <frameID>	- I
            is the generic, user-defined frame ID: "TXXX" or "WXXX".
        <description>	- I
            is the UTF-8 description string found in the frame.
        <frameType>	- I
            returns the qualified the frame type (defined in "id3_util.h").
            ID3_NULL_FRAME_TYPE (0) is returned if no qualified frame type
            matching the input arguments is found.

*******************************************************************************/


Id3FrameType  id3FrameUDTypeOf (

#    if PROTOTYPES
        const  char  *frameID,
        const  char  *description)
#    else
        frameID, description)

        char  *frameID ;
        char  *description ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if ((description == NULL) || (strlen (description) == 0))
        return (ID3_NULL_FRAME_TYPE) ;

    if (frameID == NULL)  frameID = "TXXX" ;

    for (i = 1 ;  id3FrameLUT[i].type != ID3_NULL_FRAME_TYPE ;  i++) {
        if ((id3FrameLUT[i].v2_3FrameID != NULL) &&
            (strncmp (id3FrameLUT[i].v2_3FrameID, frameID, 4) == 0) &&
            (id3FrameLUT[i].v2_3FrameID[4] == ':')) {
            if (strcasecmp (&id3FrameLUT[i].v2_3FrameID[5], description) == 0)
                return (id3FrameLUT[i].type) ;			/* Found! */
        }
        if ((id3FrameLUT[i].v2_4FrameID != NULL) &&
            (strncmp (id3FrameLUT[i].v2_4FrameID, frameID, 4) == 0) &&
            (id3FrameLUT[i].v2_4FrameID[4] == ':')) {
            if (strcasecmp (&id3FrameLUT[i].v2_4FrameID[5], description) == 0)
                return (id3FrameLUT[i].type) ;			/* Found! */
        }
    }

    return (ID3_NULL_FRAME_TYPE) ;				/* Not found. */

}

/*!*****************************************************************************

Procedure:

    id3FrameUDUpgrade ()

    Upgrade a User-Defined ID3 Frame Type to Its Predefined Type.


Purpose:

    The id3FrameUDUpgrade() function upgrades generic, user-defined types to
    their description-qualifed, predefined types.  The generic types are the
    TXXX (text) and WXXX (URL) frame types.  The predefined, user-defined
    types are ones with a certain description.  For example, the
    ID3_MUSICBRAINZ_DISCID frame type is a TXXX frame with a description
    of "MusicBrainz Disc Id".

    This function takes the description field in the frame and searches
    the frame information table for an entry of the form:

               "TXXX:<description>" or "WXXX:<description>"

    depending on the generic frame type.  If a matching predefined, frame
    type is found, then:

        (1) The frame's generic type is changed to the predefined type.

        (2) The frame's information table link is updated to point to
            the new type's entry.

    This function successfully completes with no changes if (i) the frame's
    type is not ID3_TXXX_GENERIC or ID3_WXXX_GENERIC (ID3_WWW) or (ii) no
    matching predefined type is found.


    Invocation:

        updated = id3FrameUDUpgrade (frame) ;

    where

        <frame>		- I
            is the frame handle returned by id3FrameCreate().
        <updated>	- O
            returns true if the frame's type was updated and false if not.
            (The latter is also the case if the frame's type is not a TXXX
            or WXXX generic type.)

*******************************************************************************/


bool  id3FrameUDUpgrade (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{    /* Local variables. */
    Id3FrameLUTEntry  *entry ;
    Id3FrameType  udType ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameUDUpgrade) NULL frame handle: ") ;
        return (false) ;
    }

    if (frame->type == ID3_TXXX_GENERIC) {
        udType = id3FrameUDTypeOf ("TXXX", frame->fdata.text.description) ;
        if (udType == ID3_NULL_FRAME_TYPE)  return (false) ;
    } else if (frame->type == ID3_WXXX_GENERIC) {
        udType = id3FrameUDTypeOf ("WXXX", frame->fdata.wurl.description) ;
        if (udType == ID3_NULL_FRAME_TYPE)  return (false) ;
    } else {
        return (false) ;
    }

    frame->type = udType ;		/* Update frame's type and LUT entry. */
    entry = id3FrameInfoByType (udType) ;
    if (entry != NULL)  frame->info = entry ;

    return (true) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameValueCount ()

    Get the Number of Values in an ID3 Frame.


Purpose:

    The id3FrameValueCount() function returns the number of values in
    an ID3 frame.  This is normally 1, but some text frame types allow
    multiple values within a single frame.


    Invocation:

        numValues = id3FrameValueCount (frame) ;

    where

        <frame>		- I
            is the frame handle returned by id3FrameCreate().
        <numValues>	- O
            returns the number of values in the frame.

*******************************************************************************/


size_t  id3FrameValueCount (

#    if PROTOTYPES
        Id3Frame  frame)
#    else
        frame)

        Id3Frame  frame ;
#    endif

{

    if (frame == NULL)
        return (0) ;
    else
        return (adxCount (frame->values)) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeComment ()

    Decode an ID3v2.3/ID3v2.4 Comment Frame from its File Format.


Purpose:

    Function id3FrameDecodeComment() decodes an ID3v2.3 or ID3v2.4
    comment frame in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodeComment (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeComment (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeComment) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

/*******************************************************************************
    Decode the fields preceding the comment itself.
*******************************************************************************/

/* Get the text encoding indicator. */

    frame->encoding = (Id3TextEncoding) body[0] ;

    body++ ;				/* Advance to start of value(s). */
    remaining = frame->size - 1 ;

/* Get the ISO-8859-1, 3-character language code. */

    if (remaining < 3) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) Body is too small for language code: ") ;
        return (errno) ;
    }

    strncpym (frame->fdata.comm.language, body,
              3, sizeof frame->fdata.comm.language) ;

    body += 3 ;  remaining -= 3 ;

/* Get the short description of the comment. */

    if (remaining < 1) {		/* Single-byte NUL at least? */
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) Body is too small for description: ") ;
        return (errno) ;
    }

    if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
        ((frame->encoding == ID3_UTF16_WITH_BOM) ||
         (frame->encoding == ID3_UTF16BE_NO_BOM))) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) Body is too small for description: ") ;
        return (errno) ;
    }

    frame->fdata.comm.description =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.comm.description == NULL) {
        LGE "(id3FrameDecodeComment) Error decoding description.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/*******************************************************************************
    Decode the comment itself.
*******************************************************************************/

    frame->fdata.comm.text =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.comm.text == NULL) {
        LGE "(id3FrameDecodeComment) Error decoding comment.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeGenObj ()

    Decode an ID3v2.3/ID3v2.4 General Encapsulated Object Frame.


Purpose:

    Function id3FrameDecodeGenObj() decodes an ID3v2.3 or ID3v2.4
    general encapsulated object (GEOB) frame in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodeGenObj (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeGenObj (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeGenObj) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeGenObj) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeGenObj) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

/* Get the text encoding indicator. */

    frame->encoding = (Id3TextEncoding) body[0] ;

    body++ ;  remaining = frame->size - 1 ;

/* Get the object's NUL-terminated, ISO-8859-1 MIME type. */

    skip = strnlen (body, remaining) + 1 ;
    if (remaining <= skip) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeGenObj) Body is too small for MIME type: ") ;
        return (errno) ;
    }

    frame->fdata.geob.mimeType = strndup (body, skip - 1) ;
    if (frame->fdata.geob.mimeType == NULL) {
        LGE "(id3FrameDecodeGenObj) Error duplicating MIME type \"%s\".\nstrdup: ",
            body) ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Get the object's file name. */

    if (remaining < 1) {		/* Single-byte NUL at least? */
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) Body is too small for description: ") ;
        return (errno) ;
    }

    if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
        ((frame->encoding == ID3_UTF16_WITH_BOM) ||
         (frame->encoding == ID3_UTF16BE_NO_BOM))) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeGenObj) Body is too small for description: ") ;
        return (errno) ;
    }

    frame->fdata.geob.fileName =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.geob.fileName == NULL) {
        LGE "(id3FrameDecodeGenObj) Error decoding file name.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Get the description of the object. */

    if (remaining < 1) {		/* Single-byte NUL at least? */
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) Body is too small for description: ") ;
        return (errno) ;
    }

    if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
        ((frame->encoding == ID3_UTF16_WITH_BOM) ||
         (frame->encoding == ID3_UTF16BE_NO_BOM))) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeGenObj) Body is too small for description: ") ;
        return (errno) ;
    }

    frame->fdata.geob.description =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.geob.description == NULL) {
        LGE "(id3FrameDecodeGenObj) Error decoding description.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Finally, get the object. */

    frame->fdata.geob.length = remaining ;
    frame->fdata.geob.object = memdup (body, remaining) ;
    if (frame->fdata.geob.object == NULL) {
        LGE "(id3FrameDecodeGenObj) Error duplicating %"PRIuSIZE_T"-byte object.\nmemdup: ",
            remaining) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodePassthrough ()

    Pass Through an ID3v2 Frame As-Is.


Purpose:

    Function id3FrameDecodePassthrough() "decodes" an ID3v2 frame by passing
    it through as-is; i.e., the raw input frame will be passed through to
    output without decoding or encoding.  This mode is intended for
    unrecognized or unimplemented frame types.

        NOTE: This function is called with the *full* ID3v2 frame
        *including* the frame header.  This is in contrast to the
        other frame-decode functions which are only passed the
        frame body *following* the header.


    Invocation:

        status = id3FrameDecodePassthrough (frame, buffer, length) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <buffer>	- I
            is the buffer containing the full ID3v2 frame, beginning with
            the frame header.
        <length>	- I
            is the length in bytes of the full ID3v2 frame, including the
            frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodePassthrough (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *buffer,
        size_t  length)
#    else
        frame, buffer, length)

        Id3Frame  frame ;
        char  *buffer ;
        size_t  length ;
#    endif

{

    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePassthrough) NULL frame handle: ") ;
        return (errno) ;
    }

    if (buffer == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePassthrough) NULL buffer: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodePassthrough) %.4s frame %p ...\n",
        buffer, (void *) frame) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

/* Store a copy of the complete buffer (including header) in the frame
   structure. */

    frame->fdata.ptru.buffer = memdup (buffer, length) ;
    if (frame->fdata.ptru.buffer == NULL) {
        LGE "(id3FrameDecodePassthrough) Error duplicating %"PRIuSIZE_T"-byte buffer.\nmemdup: ",
            length) ;
        return (errno) ;
    }
    frame->fdata.ptru.length = length ;

    frame->frameID = strndup (buffer, 4) ;
    if (frame->frameID == NULL) {
        LGE "(id3FrameDecodePassthrough) Error duplicating frame ID \"%.4s\".\nstndup: ",
            buffer) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodePicture ()

    Decode an ID3v2.3/ID3v2.4 Picture Frame from its File Format.


Purpose:

    Function id3FrameDecodePicture() decodes an ID3v2.3 or ID3v2.4
    attached picture frame in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodePicture (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodePicture (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePicture) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePicture) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodePicture) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

/* Get the text encoding indicator. */

    frame->encoding = (Id3TextEncoding) body[0] ;

    body++ ;  remaining = frame->size - 1 ;

/* Get the image's NUL-terminated, ISO-8859-1 MIME type. */

    skip = strnlen (body, remaining) + 1 ;
    if (remaining <= skip) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePicture) Body is too small for MIME type: ") ;
        return (errno) ;
    }

    frame->fdata.apic.mimeType = strndup (body, skip - 1) ;
    if (frame->fdata.apic.mimeType == NULL) {
        LGE "(id3FrameDecodePicture) Error duplicating MIME type \"%.*s\".\nstrndup: ",
            (int) skip - 1, body) ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Get the 1-byte picture type. */

    if (remaining < 1) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePicture) Body is too small for picture type: ") ;
        return (errno) ;
    }

    frame->fdata.apic.type = (long) body[0] ;

    body++ ;  remaining-- ;

/* Get the short description of the image. */

    if (remaining < 1) {		/* Single-byte NUL at least? */
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeComment) Body is too small for description: ") ;
        return (errno) ;
    }

    if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
        ((frame->encoding == ID3_UTF16_WITH_BOM) ||
         (frame->encoding == ID3_UTF16BE_NO_BOM))) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePicture) Body is too small for description: ") ;
        return (errno) ;
    }

    frame->fdata.apic.description =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.apic.description == NULL) {
        LGE "(id3FrameDecodePicture) Error decoding description.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Finally, get the image. */

    frame->fdata.apic.length = remaining ;
    frame->fdata.apic.image = memdup (body, remaining) ;
    if (frame->fdata.apic.image == NULL) {
        LGE "(id3FrameDecodePicture) Error duplicating %"PRIuSIZE_T"-byte image.\nmemdup: ",
            remaining) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodePlayCount ()

    Decode an ID3v2.3/ID3v2.4 PCNT/POPM Frame from its File Format.


Purpose:

    Function id3FrameDecodePlayCount() decodes an ID3v2.3/ID3v2.4
    Play Counter (PCNT) or Popularimeter (POPM) frame in an ID3v2 tag.
    (A PCNT frame has a counter; a POPM frame has an email address,
    a rating number, and a counter.)


    Invocation:

        status = id3FrameDecodePlayCount (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodePlayCount (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePlayCount) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePlayCount) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodePlayCount) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

    remaining = frame->size ;

/* If this a Popularimeter frame, then get the email address and rating. */

    if (frame->type != ID3_PLAYCOUNT) {

/* POPM email address. */

        skip = strnlen (body, remaining) + 1 ;
        if (remaining <= skip) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodePlayCount) Body is too small for POPM email: ") ;
            return (errno) ;
        }

        frame->fdata.play.email = strndup (body, skip - 1) ;
        if (frame->fdata.play.email == NULL) {
            LGE "(id3FrameDecodePlayCount) Error duplicating POPM email \"%.*s\".\nstrndup: ",
                (int) skip - 1, body) ;
            return (errno) ;
        }

        body += skip ;  remaining -= skip ;

/* POPM rating. */

        if (remaining < 1) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodePlayCount) Body is too small for POPM rating: ") ;
            return (errno) ;
        }

        frame->fdata.play.rating = (unsigned char) body[0] ;

        body++ ;  remaining-- ;

    }

/* Get the variable-length play counter, which is mandatory for the PCNT
   play counter frame and optional for the POPM popularimeter frame. */

    if (remaining == 0) {
        if (frame->type == ID3_PLAYCOUNT) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodePlayCount) Body is too small for PCNT counter: ") ;
            return (errno) ;
        } else {
            frame->fdata.play.omit = true ;
        }
    } else {
        frame->fdata.play.counter = id3FrameDecodeVUInt (body, remaining, &skip) ;
        frame->fdata.play.omit = false ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodePrivate ()

    Decode an ID3v2.3/ID3v2.4 Private Frame from its File Format.


Purpose:

    Function id3FrameDecodePrivate() decodes an ID3v2.3 or ID3v2.4 Private
    frame in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodePrivate (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodePrivate (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePrivate) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePrivate) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodePrivate) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

    remaining = frame->size ;

/* Get the owner ID. */

    skip = strnlen (body, remaining) + 1 ;
    if (remaining <= skip) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodePrivate) Body is too small for owner ID: ") ;
        return (errno) ;
    }

    frame->fdata.priv.ownerID = strndup (body, skip - 1) ;
    if (frame->fdata.priv.ownerID == NULL) {
        LGE "(id3FrameDecodePrivate) Error duplicating owner ID \"%.*s\".\nstrndup: ",
            (int) skip - 1, body) ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Get the private data. */

    frame->fdata.priv.length = remaining ;
    frame->fdata.priv.data = memdup (body, remaining) ;
    if (frame->fdata.priv.data == NULL) {
        LGE "(id3FrameDecodePrivate) Error duplicating %"PRIuSIZE_T"-byte data.\nmemdup: ",
            remaining) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodePVList ()

    Decode an ID3v2.3/ID3v2.4 Paired-Values List from its File Format.


Purpose:

    Function id3FrameDecodePVList() decodes an ID3v2.3 or ID3v2.4
    paired-values list frame.  In ID3v2.4, these frames are simply
    multi-value, text information frames.  ID3v2.3 doesn't allow
    more than one value in a text information frame, so the one
    instance of this type of frame -- the ID3v2.3 "involved people
    list" (IPLS) frame -- is just not counted as a text information
    frame despite having the exact same format as the ID3v2.4 frames.


    Invocation:

        status = id3FrameDecodePVList (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodePVList (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    errno_t  status ;



/* Since this is just a multi-value text information frame, decode it as such.
   Function id3FrameDecodeText() doesn't enforce ID3v2.3's single-value limit
   on text information frames; if that function is ever changed to enforce
   the restriction, then either it should make an exception for list frames
   or this function, id3FrameDecodePVList(), will have to decode ID3v2.3 list
   frames itself. */

    status = id3FrameDecodeText (frame, body) ;
    if (status) {
        LGE "(id3FrameDecodePVList) Error decoding list frame.\nid3FrameDecodeText: ") ;
        return (status) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeRegistration ()

    Decode ID3v2.3/ID3v2.4 ENCR/GRID Frames from their File Formats.


Purpose:

    Function id3FrameDecodeRegistration() decodes ID3v2.3/ID3v2.4 Encryption
    Method (ENCR) and Group Identification (GRID) Registration Frames in an
    ID3v2 tag.


    Invocation:

        status = id3FrameDecodeRegistration (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeRegistration (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeRegistration) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeRegistration) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeRegistration) %s frame %p ...\n",
        frame->info->v2_3FrameID, (void *) frame) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

    remaining = frame->size ;

/* Get the owner ID. */

    skip = strnlen (body, remaining) + 1 ;
    if (remaining <= skip) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeRegistration) %s body is too small for owner ID: ",
            frame->info->v2_3FrameID) ;
        return (errno) ;
    }

    frame->fdata.regi.ownerID = strndup (body, skip - 1) ;
    if (frame->fdata.regi.ownerID == NULL) {
        LGE "(id3FrameDecodeRegistration) Error duplicating %s owner ID \"%.*s\".\nstrndup: ",
            frame->info->v2_3FrameID, (int) skip - 1, body) ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Get the method/group symbol. */

    frame->fdata.regi.symbol = (unsigned int) *body++ ;
    remaining-- ;

/* Get the data field. */

    frame->fdata.regi.length = remaining ;
    frame->fdata.regi.data = memdup (body, remaining) ;
    if (frame->fdata.regi.data == NULL) {
        LGE "(id3FrameDecodeRegistration) Error duplicating %"PRIuSIZE_T"-byte %s data.\nmemdup: ",
            remaining, frame->info->v2_3FrameID) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeString ()

    Decode a String from an ID3v2.3/ID3v2.4 Frame in its File Format.


Purpose:

    Function id3FrameDecodeString() decodes a string in an ID3v2 frame,
    returning a dynamically allocated copy of the string.  In the frame,
    the string may be encoded in different ways depending on the the ID3
    version number and the encoding byte embedded in the frame.  This
    function converts all of these encodings to UTF-8 for internal storage.


    Invocation:

        string = id3FrameDecodeString (encoding, buffer, length, &skip) ;

    where

        <encoding>	- I
            is the text encoding indicator from the frame.
        <buffer>	- I
            is the buffer containing the encoded string.
        <length>	- I
            is the length in bytes of the buffer.
        <skip>		- O
            returns the offset in bytes of the next field in the buffer
            after the string.
        <string>	- O
            returns a dynamically allocated, UTF-8 copy of the string;
            the caller is responsible for free(3)ing the string after
            it is no longer needed.  NULL is returned upon error.

*******************************************************************************/


char  *id3FrameDecodeString (

#    if PROTOTYPES
        Id3TextEncoding  encoding,
        const  char  *buffer,
        size_t  length,
        size_t  *skip)
#    else
        encoding, buffer, length, skip)

        Id3TextEncoding  encoding ;
        const  char  *buffer ;
        size_t  length ;
        size_t  *skip ;
#    endif

{    /* Local variables. */
    bool  nulTerminated ;
    char  *result ;
    size_t  byteLength, estimate, temp, unitLength ;
    TagUTF  bomOrder ;



    if (skip == NULL)  skip = &temp ;

/* If the string in the frame is 8-bit based (ISO-8859-1 or UTF-8),
   simply duplicate the string as is. */

    if ((encoding == ID3_ISO_8859_1) || (encoding == ID3_UTF8)) {

        result = strndup (buffer, length) ;
        if (result == NULL) {
            LGE "(id3FrameDecodeString) Error duplicating \"%.*s\".\nstrndup: ",
                length, buffer) ;
            return (NULL) ;
        }

        *skip = strnlen (buffer, length) ;
        if (*skip < length)  (*skip)++ ;		/* NUL, if present. */

    }

/* Otherwise it is UTF-16, with or without a byte-order marker.  Convert it
   to UTF-8 for internal storage.  A factor of 2 is used for convenience's
   sake in computing the maximum length of the UTF-8 string.  The actual
   factor is 1.5: (i) the largest single-unit, two-byte Unicode
   (non-)character, U+FFFF, is converted to a 3-byte UTF-8 sequence; and
   (ii) the remaining two-unit, 4-byte UTF-16 characters map to 3- or 4-byte
   UTF-8 sequences. */

    else {

        unitLength = utf16nlen (buffer, length / UTF_16_UNIT_BYTES, &bomOrder) ;

        byteLength = unitLength * UTF_16_UNIT_BYTES ;
        if (BOM_IS_PRESENT (bomOrder))  byteLength += UTF_16_UNIT_BYTES ;

        nulTerminated = (byteLength < length) ;

        estimate = (byteLength * 2) + 1 ;
        LGI "(id3FrameDecodeString) UTF-16 Length: %"PRIuSIZE_T"  UTF-8 Estimate: %"PRIuSIZE_T"\n",
            unitLength, estimate) ;

        result = malloc (estimate) ;
        if (result == NULL) {
            LGE "(id3FrameDecodeString) Error allocating %"PRIuSIZE_T"-byte UTF-8 string.\nmalloc: ",
                estimate) ;
            return (NULL) ;
        }

        utf16to8 (buffer, byteLength, bomOrder, false, estimate, result) ;

        *skip = byteLength ;				/* BOM and text. */
        if (nulTerminated)
            *skip += UTF_16_UNIT_BYTES ;		/* UTF-16 NUL. */

    }

    LGI "(id3FrameDecodeString) \"%s\"\n", result) ;

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeText ()

    Decode an ID3v2.3/ID3v2.4 Text Frame from its File Format.


Purpose:

    Function id3FrameDecodeText() decodes an ID3v2.3 or ID3v2.4 text frame,
    both predefined ("Txyz") and user-defined ("TXXX"), in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodeText (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeText (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    Id3Value  value ;
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeText) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeText) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeText) Frame %p %s ... %"PRIuSIZE_T"-byte body\n",
        (void *) frame, frame->info->name, frame->size) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

    remaining = frame->size ;

/* Get the text encoding indicator. */

    frame->encoding = (Id3TextEncoding) body[0] ;

    body++ ;  remaining-- ;

/* If this is a user-defined text frame type, get the short description
   of the text value. */

    if (frame->info->marshalType == Id3MarshalUText) {

        if (remaining < 1) {		/* Single-byte NUL at least? */
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodeText) Body is too small for description: ") ;
            return (errno) ;
        }

        if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
            ((frame->encoding == ID3_UTF16_WITH_BOM) ||
             (frame->encoding == ID3_UTF16BE_NO_BOM))) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodeText) Body is too small for description: ") ;
            return (errno) ;
        }

        frame->fdata.text.description =
            id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

        if (frame->fdata.text.description == NULL) {
            LGE "(id3FrameDecodeText) Error decoding description.\nid3FrameDecodeString: ") ;
            return (errno) ;
        }

        body += skip ;  remaining -= skip ;

/* If this is a qualified, user-defined frame type ("TXXX:<description>"),
   then substitute the qualified type for the generic frame type. */

        if (frame->type == ID3_TXXX_GENERIC)  id3FrameUDUpgrade (frame) ;

    }

/* Get the text value. */

    frame->fdata.text.text =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.text.text == NULL) {
        LGE "(id3FrameDecodeText) Error decoding text.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* ID3v2.4 text information frames can have multiple, NUL-separated values.
   Decode the second through N-th strings, if any, and add them as value
   objects to the frame. */

    while (remaining > 0) {

        LGI "(id3FrameDecodeText) Value pre-skip %"PRIuSIZE_T", body %p, remaining %"PRIuSIZE_T"\n",
            skip, (void *) body, remaining) ;

        if (id3ValueCreate (&value)) {
            LGE "(id3FrameDecodeText) Error creating value object.\nid3ValueCreate: ") ;
            return (errno) ;
        }

        value->vdata.text =
            id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;
        if (value->vdata.text == NULL) {
            LGE "(id3FrameDecodeText) Error decoding value text.\nid3FrameDecodeString: ") ;
            PUSH_ERRNO ;  id3ValueDestroy (value) ;  POP_ERRNO ;
            return (errno) ;
        }
        value->type = Id3ValueText ;

        if (id3FrameAddValue (frame, value)) {
            LGE "(id3FrameDecodeText) Error adding value %p to %s frame %p.\nid3FrameAddValue: ",
                (void *) value, frame->info->name, (void *) frame) ;
            PUSH_ERRNO ;  id3ValueDestroy (value) ;  POP_ERRNO ;
            return (errno) ;
        }

        body += skip ;  remaining -= skip ;

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeUFID ()

    Decode an ID3v2.3/ID3v2.4 UFID Frame from its File Format.


Purpose:

    Function id3FrameDecodeUFID() decodes an ID3v2.3 or ID3v2.4 Unique
    File Identifier frame in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodeUFID (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeUFID (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUFID) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUFID) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeUFID) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

    remaining = frame->size ;

/* Get the owner ID. */

    skip = strnlen (body, remaining) + 1 ;
    if (remaining <= skip) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUFID) Body is too small for owner ID: ") ;
        return (errno) ;
    }

    frame->fdata.ufid.ownerID = strndup (body, skip - 1) ;
    if (frame->fdata.ufid.ownerID == NULL) {
        LGE "(id3FrameDecodeUFID) Error duplicating owner ID \"%.*s\".\nstrndup: ",
            (int) skip - 1, body) ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/* Get the file ID, which is arbitrary binary data up to 64 bytes in length. */

    if (remaining > MAX_FILEID_LENGTH)  remaining = MAX_FILEID_LENGTH ;

    frame->fdata.ufid.length = remaining ;
    frame->fdata.ufid.fileID = memdup (body, remaining) ;
    if (frame->fdata.ufid.fileID == NULL) {
        LGE "(id3FrameDecodeUFID) Error duplicating %"PRIuSIZE_T"-byte file ID.\nmemdup: ",
            remaining) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeURL ()

    Decode an ID3v2.3/ID3v2.4 URL Link Frame from its File Format.


Purpose:

    Function id3FrameDecodeURL() decodes an ID3v2.3 or ID3v2.4 URL link frame,
    both predefined ("Wxyz") and user-defined ("WXXX"), in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodeURL (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeURL (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeURL) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeURL) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeURL) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

    remaining = frame->size ;

/* If this is a user-defined URL frame type, get the text encoding byte ... */

    if (frame->info->marshalType == Id3MarshalUURL) {

        frame->encoding = (Id3TextEncoding) body[0] ;

        body++ ;  remaining-- ;

/* ... and the description string. */

        if (remaining < 1) {		/* Single-byte NUL at least? */
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodeURL) Body is too small for description: ") ;
            return (errno) ;
        }

        if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
            ((frame->encoding == ID3_UTF16_WITH_BOM) ||
             (frame->encoding == ID3_UTF16BE_NO_BOM))) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3FrameDecodeURL) Body is too small for description: ") ;
            return (errno) ;
        }

        frame->fdata.wurl.description =
            id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

        if (frame->fdata.wurl.description == NULL) {
            LGE "(id3FrameDecodeURL) Error decoding description.\nid3FrameDecodeString: ") ;
            return (errno) ;
        }

        body += skip ;  remaining -= skip ;

/* If this is a qualified, user-defined frame type ("WXXX:<description>"),
   then substitute the qualified type for the generic frame type. */

        if (frame->type == ID3_WXXX_GENERIC)  id3FrameUDUpgrade (frame) ;

    }

/* Get the URL.  The URL in the raw frame is an ISO-8859-1 string which may
   or may not be NUL-terminated, so limit the length of the URL to the end
   of the frame. */

    frame->fdata.wurl.url = strndup (body, remaining) ;
    if (frame->fdata.wurl.url == NULL) {
        LGE "(id3FrameDecodeURL) Error duplicating URL: \"%.*s\"\nstrndup: ",
            (int) remaining, body) ;
        return (errno) ;
    }

    body += remaining ;  remaining = 0 ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeUSLT ()

    Decode an ID3v2.3/ID3v2.4 Unsynchronized Lyrics Frame from its File Format.


Purpose:

    Function id3FrameDecodeUSLT() decodes an ID3v2.3 or ID3v2.4 unsynchronized
    lyrics (USLT) frame in an ID3v2 tag.


    Invocation:

        status = id3FrameDecodeUSLT (frame, body) ;

    where

        <frame>		- I/O
            is a handle for the ID3v2 frame, with the frame type, frame
            size, and frame flags already set.  The value(s) in the frame
            will be filled in as the frame is decoded.
        <body>		- I
            is the body of the frame following the frame header.
        <status>	- O
            returns the status of decoding the frame, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameDecodeUSLT (

#    if PROTOTYPES
        Id3Frame  frame,
        const  char  *body)
#    else
        frame, body)

        Id3Frame  frame ;
        char  *body ;
#    endif

{    /* Local variables. */
    size_t  remaining, skip ;



    if (frame == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUSLT) NULL frame handle: ") ;
        return (errno) ;
    }

    if (body == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUSLT) NULL body: ") ;
        return (errno) ;
    }

    LGI "(id3FrameDecodeUSLT) Frame %p %s ...\n",
        (void *) frame, frame->info->name) ;

/* Initialize the dynamic fields in the frame structure. */

    id3FrameDataInit (frame) ;

/*******************************************************************************
    Decode the fields preceding the lyrics.
*******************************************************************************/

/* Get the text encoding indicator. */

    frame->encoding = (Id3TextEncoding) body[0] ;

    body++ ;				/* Advance to start of value(s). */
    remaining = frame->size - 1 ;

/* Get the ISO-8859-1, 3-character language code. */

    if (remaining < 3) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUSLT) Body is too small for language code: ") ;
        return (errno) ;
    }

    strncpym (frame->fdata.uslt.language, body,
              3, sizeof frame->fdata.uslt.language) ;

    body += 3 ;  remaining -= 3 ;

/* Get the description of the lyrics. */

    if (remaining < 1) {		/* Single-byte NUL at least? */
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUSLT) Body is too small for description: ") ;
        return (errno) ;
    }

    if ((remaining < 2) &&		/* Two-byte UTF-16 NUL at least? */
        ((frame->encoding == ID3_UTF16_WITH_BOM) ||
         (frame->encoding == ID3_UTF16BE_NO_BOM))) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameDecodeUSLT) Body is too small for description: ") ;
        return (errno) ;
    }

    frame->fdata.uslt.description =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.uslt.description == NULL) {
        LGE "(id3FrameDecodeUSLT) Error decoding description.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    body += skip ;  remaining -= skip ;

/*******************************************************************************
    Decode the lyrics themselves.
*******************************************************************************/

    frame->fdata.uslt.lyrics =
        id3FrameDecodeString (frame->encoding, body, remaining, &skip) ;

    if (frame->fdata.uslt.lyrics == NULL) {
        LGE "(id3FrameDecodeUSLT) Error decoding lyrics.\nid3FrameDecodeString: ") ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameDecodeVUInt ()

    Decode a Variable-Length Integer from its File Format in an ID3v2 Frame.


Purpose:

    Function id3FrameDecodeVUInt() decodes a variable-length unsigned
    integer at the *end* of an ID3v2 frame.  A variable-length integer's
    width in bytes is only enough to hold its value.  For example, values
    from 0..255 require only 1 byte, from 256..65,535 only 2 bytes, etc.
    (The ID3v2 specifications require a minimum of 4 bytes, but this
    function doesn't enforce this; the corresponding encode function does!)

        NOTE: Variable-length integers are used in Play Counter (PCNT)
        and Popularimeter (POPM) frames.  The end of a counter is
        signaled by the end of its frame, which this function knows
        based on the LENGTH argument.

        NOTE: If the width of the value in the frame is wider than the
        host's size_t representation, this function will happily keep
        shifting out the most significant bytes of the value.  It's a
        play counter and it's not really worth the effort to implement
        multi-precision host integers.  (The representation is not a
        problem, but incrementing such a counter is.)


    Invocation:

        integer = id3FrameDecodeVUInt (buffer, length, &skip) ;

    where

        <buffer>	- I
            is the buffer containing the variable-length integer.
        <length>	- I
            is the length in bytes of the buffer.
        <skip>		- O
            returns the offset in bytes of the next field in the buffer
            after the integer.  Since decoding the integer consumes the
            rest of the frame, the skip value will always be identical
            to LENGTH.
        <integer>	- O
            returns the decoded unsigned integer.  0 is returned if the
            buffer length is zero.

*******************************************************************************/


size_t  id3FrameDecodeVUInt (

#    if PROTOTYPES
        const  char  *buffer,
        size_t  length,
        size_t  *skip)
#    else
        buffer, length, skip)

        const  char  *buffer ;
        size_t  length ;
        size_t  *skip ;
#    endif

{    /* Local variables. */
    size_t  temp ;
    size_t  result ;



    if (skip == NULL)  skip = &temp ;
    *skip = 0 ;

    if (length == 0)			/* End of buffer, no integer? */
        return (0) ;

    result = 0 ;
    while (length-- > 0) {
        result = (result << 8) + (*buffer++ & 0x0FF) ;
        (*skip)++ ;
    }


    LGI "(id3FrameDecodeVUInt) %zu\n", result) ;

    return (result) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeComment ()

    Encode an ID3v2 Comment Frame Body into its File Format.


Purpose:

    Function id3FrameEncodeComment() encodes the body of an ID3v2 frame into
    a byte buffer.


    Invocation:

        status = id3FrameEncodeComment (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeComment (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    char  *buffer ;
    size_t  partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeComment) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the text encoding byte to the buffer. */

    if (mdxAddB (bdx, id3Encoding (frame->parent))) {
        LGE "(id3FrameEncodeComment) Error adding text encoding byte to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        return (errno) ;
    }

    (*length)++ ;

/* Add the 3-byte language code. */

    if (mdxAddS (bdx, false, NULL, 3)) {
        LGE "(id3FrameEncodeComment) Error adding 3-byte language code to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    buffer = mdxRegionP (bdx, priorLength + *length) ;
    strncpy (buffer, frame->fdata.comm.language, 3) ;

    *length += 3 ;

/* Encode the description into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.comm.description,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeComment) Error encoding description string \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.comm.description, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

/* Encode the comment itself into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.comm.text,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeComment) Error encoding text string \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.comm.text, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

    LGI "(id3FrameEncodeComment) Frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeGenObj ()

    Encode an ID3v2 General Encapsulated Object Frame Body.


Purpose:

    Function id3FrameEncodeGenObj() encodes the body of an ID3v2 frame into
    a byte buffer.


    Invocation:

        status = id3FrameEncodeGenObj (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeGenObj (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeGenObj) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the text encoding byte to the buffer. */

    if (mdxAddB (bdx, id3Encoding (frame->parent))) {
        LGE "(id3FrameEncodeGenObj) Error adding text encoding byte to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        return (errno) ;
    }

    (*length)++ ;

/* Add the MIME type (an ISO-8859-1 string with a NUL terminator). */

    if (mdxAddS (bdx, false, frame->fdata.geob.mimeType,
                 strlen (frame->fdata.geob.mimeType) + 1)) {
        LGE "(id3FrameEncodeGenObj) Error adding MIME type \"%s\" to frame %p.\nmdxAddS: ",
            frame->fdata.geob.mimeType, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += strlen (frame->fdata.geob.mimeType) + 1 ;

/* Encode the file name into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.geob.fileName,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeGenObj) Error encoding fileName \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.geob.fileName, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

/* Encode the description into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.geob.description,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeGenObj) Error encoding description \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.geob.description, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

/* Add the object data to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.geob.object,
                 frame->fdata.geob.length)) {
        LGE "(id3FrameEncodeGenObj) Error adding %"PRIuSIZE_T"-byte object to frame %p.\nmdxAddS: ",
            frame->fdata.geob.length, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += frame->fdata.geob.length ;

    LGI "(id3FrameEncodeGenObj) Frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodePicture ()

    Encode an ID3v2 Attached Picture Frame Body into its File Format.


Purpose:

    Function id3FrameEncodePicture() encodes the body of an ID3v2 frame into
    a byte buffer.


    Invocation:

        status = id3FrameEncodePicture (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodePicture (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodePicture) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the text encoding byte to the buffer. */

    if (mdxAddB (bdx, id3Encoding (frame->parent))) {
        LGE "(id3FrameEncodePicture) Error adding text encoding byte to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        return (errno) ;
    }

    (*length)++ ;

/* Add the MIME type (an ISO-8859-1 string with a NUL terminator). */

    if (mdxAddS (bdx, false, frame->fdata.apic.mimeType,
                 strlen (frame->fdata.apic.mimeType) + 1)) {
        LGE "(id3FrameEncodePicture) Error adding MIME type \"%s\" to frame %p.\nmdxAddS: ",
            frame->fdata.apic.mimeType, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += strlen (frame->fdata.apic.mimeType) + 1 ;

/* Add the picture type. */

    if (mdxAddB (bdx, (char) frame->fdata.apic.type)) {
        LGE "(id3FrameEncodePicture) Error adding picture type %ld to frame %p.\nmdxAddB: ",
            frame->fdata.apic.type, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    (*length)++ ;

/* Encode the description into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.apic.description,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodePicture) Error encoding description \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.apic.description, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

/* Add the image data to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.apic.image,
                 frame->fdata.apic.length)) {
        LGE "(id3FrameEncodePicture) Error adding %"PRIuSIZE_T"-byte image to frame %p.\nmdxAddS: ",
            frame->fdata.apic.length, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += frame->fdata.apic.length ;

    LGI "(id3FrameEncodePicture) Frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodePlayCount ()

    Encode an ID3v2 PCNT/POPM Frame Body into its File Format.


Purpose:

    Function id3FrameEncodePlayCount() encodes the body of an ID3v2
    Play Counter (PCNT) or Popularimeter (POPM) frame into a byte buffer.


    Invocation:

        status = id3FrameEncodePlayCount (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodePlayCount (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodePlayCount) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* If this a Popularimeter frame, then encode the email address and rating. */

    if (frame->type != ID3_PLAYCOUNT) {

/* POPM email address (an ISO-8859-1 string with a NUL terminator). */

        if (mdxAddS (bdx, false, frame->fdata.play.email,
                     strlen (frame->fdata.play.email) + 1)) {
            LGE "(id3FrameEncodePlayCount) Error adding email \"%s\" to POPM frame %p.\nmdxAddS: ",
                frame->fdata.play.email, (void *) frame) ;
            PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
            return (errno) ;
        }

        *length += strlen (frame->fdata.play.email) + 1 ;

/* POPM rating. */

        if (mdxAddB (bdx, (char) frame->fdata.play.rating)) {
            LGE "(id3FrameEncodePlayCount) Error adding rating %u to POPM frame %p.\nmdxAddB: ",
                frame->fdata.play.rating, (void *) frame) ;
            PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
            return (errno) ;
        }

        (*length)++ ;

    }

/* Encode the play counter.  (This is an optional field in POPM frames
   and mandatory in PCNT frames.) */

    if ((frame->type == ID3_PLAYCOUNT) || !frame->fdata.play.omit) {

        if (id3FrameEncodeVUInt (frame->fdata.play.counter, bdx, &partLength)) {
            LGE "(id3FrameEncodePlayCount) Error encoding counter in frame %p.\nid3FrameEncodeVUInt: ",
                (void *) frame) ;
            PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
            return (errno) ;
        }

        *length += partLength ;

    }

    LGI "(id3FrameEncodePlayCount) Frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodePrivate ()

    Encode an ID3v2 Private Frame Body into its File Format.


Purpose:

    Function id3FrameEncodePrivate() encodes the body of an ID3v2 Private
    frame into a byte buffer.


    Invocation:

        status = id3FrameEncodePrivate (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodePrivate (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodePrivate) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the ISO-8859-1 owner ID (with a NUL terminator) to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.priv.ownerID,
                 strlen (frame->fdata.priv.ownerID) + 1)) {
        LGE "(id3FrameEncodePrivate) Error adding owner ID \"%s\" to frame %p.\nmdxAddS: ",
            frame->fdata.priv.ownerID, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += strlen (frame->fdata.priv.ownerID) + 1 ;

/* Add the private data to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.priv.data,
                 frame->fdata.priv.length)) {
        LGE "(id3FrameEncodePrivate) Error adding %"PRIuSIZE_T"-byte data to frame %p.\nmdxAddS: ",
            frame->fdata.priv.length, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += frame->fdata.priv.length ;

    LGI "(id3FrameEncodePrivate) %s frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        frame->info->name, (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodePVList ()

    Encode an ID3v2 Paired-Values List Body into its File Format.


Purpose:

    Function id3FrameEncodePVList() encodes an ID3v2 paired-values list
    frame into a byte buffer.  In ID3v2.4, these frames are simply
    multi-value, text information frames.  ID3v2.3 doesn't allow more
    than one value in a text information frame, so the one instance of
    this type of frame -- the ID3v2.3 "involved people list" (IPLS)
    frame -- is just not counted as a text information frame despite
    having the exact same format as the ID3v2.4 frames.


    Invocation:

        status = id3FrameEncodePVList (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodePVList (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    errno_t  status ;



/* Since this is just a multi-value text information frame, encode it as such.
   Function id3FrameEncodeText() enforces ID3v2.3's single-value limit on text
   information frames but makes an exception for list frames. */

    status = id3FrameEncodeText (frame, bdx, length) ;
    if (status) {
        LGE "(id3FrameEncodePVList) Error encoding list frame.\nid3FrameEncodeText: ") ;
        return (status) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeRegistration ()

    Encode an ID3v2 ENCR/GRID Frame Body into its File Format.


Purpose:

    Function id3FrameEncodeRegistration() encodes the body of an
    ID3v2.3/ID3v2.4 Encryption Method (ENCR) or Group Identification (GRID)
    Registration Frame into a byte buffer.


    Invocation:

        status = id3FrameEncodeRegistration (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeRegistration (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeRegistration) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the ISO-8859-1 owner ID (with a NUL terminator) to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.regi.ownerID,
                 strlen (frame->fdata.regi.ownerID) + 1)) {
        LGE "(id3FrameEncodeRegistration) Error adding owner ID \"%s\" to %s frame %p.\nmdxAddS: ",
            frame->fdata.regi.ownerID, frame->info->v2_3FrameID,
            (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += strlen (frame->fdata.regi.ownerID) + 1 ;

/* Add the method/group symbol to the buffer. */

    if (mdxAddB (bdx, (char) frame->fdata.regi.symbol)) {
        LGE "(id3FrameEncodeRegistration) Error adding symbol 0x%02X to %s frame %p.\nmdxAddB: ",
            frame->fdata.regi.symbol, frame->info->v2_3FrameID,
            (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += 1 ;

/* Add the data field to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.regi.data,
                 frame->fdata.regi.length)) {
        LGE "(id3FrameEncodeRegistration) Error adding %"PRIuSIZE_T"-byte data to %s frame %p.\nmdxAddS: ",
            frame->fdata.regi.length, frame->info->v2_3FrameID,
            (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += frame->fdata.regi.length ;

    LGI "(id3FrameEncodeRegistration) %s frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        frame->info->v2_3FrameID, (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeString ()

    Encode an ID3v2 String into its File Format.


Purpose:

    Function id3FrameEncodeString() encodes a UTF-8 string into the body
    of an ID3v2 frame.  The input string is converted to the specified
    text encoding.


    Invocation:

        status = id3FrameEncodeString (encoding, string, bdx, &length) ;

    where

        <encoding>	- I
            specifies the text encoding to use; see "id3_internals.h".
        <string>	- I
            is the UTF-8 string to be encoded.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the string will be encoded.  The encoded string will be
            appended to the prior contents of the buffer.
        <length>	- O
            returns the length in bytes of the encoded string.
        <status>	- O
            returns the status of encoding the string, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeString (

#    if PROTOTYPES
        Id3TextEncoding  encoding,
        const  char  *string,
        MemoryDx  bdx,
        size_t  *length)
#    else
        encoding, string, bdx, length)

        Id3TextEncoding  encoding ;
        char  *string ;
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    char  *buffer ;
    size_t  estimate, priorLength ;
    TagUTF  bomOrder ;



    if ((string == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeString) NULL string, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

/*******************************************************************************
    If the encoding type is 8-bit based (ISO-8859-1 or UTF-8), then copy
    the string as-is into the buffer.
*******************************************************************************/

    if ((encoding == ID3_ISO_8859_1) || (encoding == ID3_UTF8)) {

        *length = strlen (string) + 1 ;

        if (mdxAddS (bdx, false, string, *length)) {
            LGE "(id3FrameEncodeString) Error adding %"PRIuSIZE_T"-byte+1 string: \"%s\"\nmdxAddS: ",
                *length - 1, string) ;
            return (errno) ;
        }

        LGI "(id3FrameEncodeString) %"PRIuSIZE_T"-byte+1 %s string: \"%s\"\n",
            *length - 1,
            (encoding == ID3_ISO_8859_1) ? "ISO-8859-1" : "UTF-8",
            string) ;

        return (0) ;

    }

/*******************************************************************************
    The encoding type is UTF-16, with or without a byte-order marker.
*******************************************************************************/

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Allocate memory in the buffer for the converted string.  The estimated
   maximum length of the converted string is based on the fact that the
   maximum multiplier for UTF-8-to-UTF-16 lengths is 2: (i) a 1-byte UTF-8
   character becomes a 2-byte UTF-16 character and (ii) 2-, 3-, and 4-byte
   UTF-8 characters map to at most two UTF-16 units (4 bytes). */

    estimate = (2 * UTF_16_UNIT_BYTES) +	/* UTF-16 BOM */
               (strlen (string) * UTF_16_UNIT_BYTES) +
               UTF_16_UNIT_BYTES ;		/* UTF-16 NUL terminator */

    LGI "(id3FrameEncodeString) UTF-8 Length: %"PRIuSIZE_T"  UTF-16 Estimate: %"PRIuSIZE_T"\n",
        strlen (string), estimate) ;

    if (mdxAddS (bdx, false, NULL, estimate)) {
        LGE "(id3FrameEncodeString) Error allocating %"PRIuSIZE_T"-byte buffer for \"%s\".\nmdxAddS: ",
            estimate, string) ;
        return (errno) ;
    }

/* Convert the UTF-8 string to UTF-16 with the converted string written to
   the output buffer. */

    if (encoding == ID3_UTF16BE_NO_BOM)
        bomOrder = UTF_16_NULL_TAG ;		/* Big endian, no BOM. */
    else
        bomOrder = UTF_16_TAG_LE ;		/* Little endian, with BOM. */

    buffer = mdxRegionP (bdx, priorLength) ;

    if (utf8to16 (string, -1, true, bomOrder, estimate, buffer) < 0) {
        LGE "(id3FrameEncodeString) Error converting \"%s\" to UTF-16.\nutf8to16: ",
            string) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Trim the buffer back to the actual length of the converted string:
   1-unit BOM + N-unit string + 1-unit NUL. */

    if (encoding == ID3_UTF16BE_NO_BOM)
        *length = (utf16len (buffer, NULL) + 1) * UTF_16_UNIT_BYTES ;
    else
        *length = (1 + utf16len (buffer, NULL) + 1) * UTF_16_UNIT_BYTES ;

    mdxSetLength (bdx, priorLength + *length) ;

    LGI "(id3FrameEncodeString) %"PRIuSIZE_T"-byte UTF-16 string: \"%s\" (UTF-8)\n",
        *length, string) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeText ()

    Encode an ID3v2 Text Frame Body into its File Format.


Purpose:

    Function id3FrameEncodeText() encodes the body of an ID3v2 text frame
    into a byte buffer.  This function handles predefined ("Txyz") frames,
    unsupported (ID3_TEXT_GENERIC) frames, and user-defined ("TXXX") frames.


    Invocation:

        status = id3FrameEncodeText (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeText (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    Id3Value  *value ;
    size_t  count, i, partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeText) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

/*******************************************************************************

    Basic Layout of Text Frame Types
    --------------------------------

    ("n-byte NUL terminator" means 1-/2-byte UTF-8/UTF-16 NUL characters.)

    Predefined Text Frame Types ("Txyz") -
        First byte:	Text Encoding
        Second byte:	Encoded string followed by n-byte NUL terminator.
        ... Additional NUL-terminated value object strings if any ...

    User-Defined Text Frame Types ("TXXX") -
        First byte:	Text Encoding
        Second byte:	Encoded description string with n-byte NUL terminator.
        Subsequent:	Encoded text string with n-byte NUL terminator.
        ... Additional NUL-terminated value object strings if any ...

*******************************************************************************/

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the text encoding byte to the buffer. */

    if (mdxAddB (bdx, id3Encoding (frame->parent))) {
        LGE "(id3FrameEncodeText) Error adding text encoding byte to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        return (errno) ;
    }

    (*length)++ ;

/* If this is a user-defined text frame type, encode the short description
   into the buffer. */

    if (frame->info->marshalType == Id3MarshalUText) {

        if (id3FrameEncodeString (id3Encoding (frame->parent),
                                  frame->fdata.text.description,
                                  bdx, &partLength)) {
            LGE "(id3FrameEncodeText) Error encoding description string \"%s\" for frame %p.\nid3FrameEncodeString: ",
                frame->fdata.text.description, frame) ;
            PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
            return (errno) ;
        }

        *length += partLength ;

    }

/* Encode the text itself into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.text.text,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeText) Error encoding text string \"%s\" for frame %p.\nid3FrameEncodeString: ",
            (void *) frame->fdata.text.text, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

/* If this is an ID3v2.4 tag or a paired-values list frame (v2.3 or v2.4),
   then encode additional string values, if any. */

    if ((id3GetVersion (frame->parent) >= ID3V_V2_4) ||
        (frame->info->marshalType == Id3MarshalPVList)) {

        count = adxCount (frame->values) ;

        for (i = 0 ;  i < count ;  i++) {
            value = adxGet (frame->values, i) ;
            if ((*value)->type != Id3ValueText)  break ;
            if (id3FrameEncodeString (id3Encoding (frame->parent),
                                      (*value)->vdata.text,
                                      bdx, &partLength)) {
                LGE "(id3FrameEncodeText) Error encoding text value[%"PRIuSIZE_T"] string \"%s\" for frame %p.\nid3FrameEncodeString: ",
                    i, (*value)->vdata.text, (void *) frame) ;
                PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
                return (errno) ;
            }
            *length += partLength ;
        }

    }

    LGI "(id3FrameEncodeText) %s frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        frame->info->name, (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeUFID ()

    Encode an ID3v2 UFID Frame Body into its File Format.


Purpose:

    Function id3FrameEncodeUFID() encodes the body of an ID3v2 UFID frame
    into a byte buffer.


    Invocation:

        status = id3FrameEncodeUFID (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeUFID (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeUFID) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the ISO-8859-1 owner ID (with a NUL terminator) to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.ufid.ownerID,
                 strlen (frame->fdata.ufid.ownerID) + 1)) {
        LGE "(id3FrameEncodeUFID) Error adding owner ID \"%s\" to frame %p.\nmdxAddS: ",
            frame->fdata.ufid.ownerID, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += strlen (frame->fdata.ufid.ownerID) + 1 ;

/* Add the file ID (up to 64 bytes of binary data) to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.ufid.fileID,
                 frame->fdata.ufid.length)) {
        LGE "(id3FrameEncodeUFID) Error adding %"PRIuSIZE_T"-byte file ID to frame %p.\nmdxAddS: ",
            frame->fdata.ufid.length, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += frame->fdata.ufid.length ;

    LGI "(id3FrameEncodeUFID) %s frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        frame->info->name, (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeURL ()

    Encode an ID3v2 URL Link Frame Body into its File Format.


Purpose:

    Function id3FrameEncodeURL() encodes the body of an ID3v2 URL link frame
    into a byte buffer.  This function handles predefined ("Wxyz") frames,
    unsupported (ID3_WURL_GENERIC) frames, and user-defined ("WXXX") frames.


    Invocation:

        status = id3FrameEncodeURL (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeURL (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    size_t  partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeURL) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

/*******************************************************************************

    Basic Layout of URL Link Frame Types
    ------------------------------------

    ("n-byte NUL terminator" means 1-/2-byte UTF-8/UTF-16 NUL characters.)

    Predefined URL Link Frame Types ("Wxyz") -
        First byte:	ISO-8859-1 URL with terminator ('\0').

    User-Defined URL Link Frame Types ("WXXX") -
        First byte:	Text Encoding
        Second byte:	Encoded description string with n-byte NUL terminator.
        Subsequent:	ISO-8859-1 URL with terminator ('\0').

    The NUL terminator on the URL is not required since the frame size
    can specify the end of the string, but I add it in anyway (and the
    frame size is adjusted to include the NUL character).

*******************************************************************************/

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* If this is a user-defined text frame type, encode the short description
   into the buffer. */

    if (frame->info->marshalType == Id3MarshalUURL) {

/* Add the text encoding byte to the buffer ... */

        if (mdxAddB (bdx, id3Encoding (frame->parent))) {
            LGE "(id3FrameEncodeURL) Error adding description encoding byte to frame %p.\nmdxAddB: ",
                (void *) frame) ;
            return (errno) ;
        }

        (*length)++ ;

/* ... and the description. */

        if (id3FrameEncodeString (id3Encoding (frame->parent),
                                  frame->fdata.wurl.description,
                                  bdx, &partLength)) {
            LGE "(id3FrameEncodeURL) Error encoding description string \"%s\" for frame %p.\nid3FrameEncodeString: ",
                frame->fdata.wurl.description, frame) ;
            PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
            return (errno) ;
        }

        *length += partLength ;

    }

/* Add the ISO-8859-1 URL string (with a NUL terminator) to the buffer. */

    if (mdxAddS (bdx, false, frame->fdata.wurl.url,
                 strlen (frame->fdata.wurl.url) + 1)) {
        LGE "(id3FrameEncodeURL) Error adding URL \"%s\" to frame %p.\nmdxAddS: ",
            frame->fdata.wurl.url, (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += strlen (frame->fdata.wurl.url) + 1 ;

    LGI "(id3FrameEncodeURL) %s frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        frame->info->name, (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeUSLT ()

    Encode an ID3v2 Unsynchronized Lyrics Frame Body into its File Format.


Purpose:

    Function id3FrameEncodeUSLT() encodes the body of an ID3v2 unsynchronized
    lyrics (USLT) frame into a byte buffer.


    Invocation:

        status = id3FrameEncodeUSLT (frame, bdx, &length) ;

    where

        <frame>		- I
            is the handle of the ID3 frame.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the frame will be encoded.  The frame header must be the
            last item in the buffer; the encoded body will be appended to
            the header.
        <length>	- O
            returns the length in bytes of the encoded frame body.
        <status>	- O
            returns the status of encoding the body, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeUSLT (

#    if PROTOTYPES
        Id3Frame  frame,
        MemoryDx  bdx,
        size_t  *length)
#    else
        frame, bdx, length)

        Id3Frame  frame,
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    char  *buffer ;
    size_t  partLength, priorLength ;



    if ((frame == NULL) || (bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeUSLT) NULL frame, buffer descriptor, or return length: ") ;
        return (errno) ;
    }

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

/* Add the text encoding byte to the buffer. */

    if (mdxAddB (bdx, id3Encoding (frame->parent))) {
        LGE "(id3FrameEncodeUSLT) Error adding text encoding byte to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        return (errno) ;
    }

    (*length)++ ;

/* Add the 3-byte language code. */

    if (mdxAddS (bdx, false, NULL, 3)) {
        LGE "(id3FrameEncodeUSLT) Error adding 3-byte language code to frame %p.\nmdxAddB: ",
            (void *) frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    buffer = mdxRegionP (bdx, priorLength + *length) ;
    strncpy (buffer, frame->fdata.uslt.language, 3) ;

    *length += 3 ;

/* Encode the description into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.uslt.description,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeUSLT) Error encoding description string \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.uslt.description, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

/* Encode the lyrics themselves into the buffer. */

    if (id3FrameEncodeString (id3Encoding (frame->parent),
                              frame->fdata.uslt.lyrics,
                              bdx, &partLength)) {
        LGE "(id3FrameEncodeUSLT) Error encoding lyrics \"%s\" for frame %p.\nid3FrameEncodeString: ",
            frame->fdata.uslt.lyrics, frame) ;
        PUSH_ERRNO ;  mdxSetLength (bdx, priorLength) ;  POP_ERRNO ;
        return (errno) ;
    }

    *length += partLength ;

    LGI "(id3FrameEncodeUSLT) Frame %p body encoded: %"PRIuSIZE_T" bytes.\n",
        (void *) frame, *length) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3FrameEncodeVUInt ()

    Encode an ID3v2 Variable-Length Integer into its File Format.


Purpose:

    Function id3FrameEncodeVUInt() encodes a variable-length integer into
    the body of an ID3v2 frame.  The host representation of the value is
    as a size_t integer.  The ID3 representation is big-endian with leading
    zero bytes omitted and a minimum length of 32 bits.  For example, on a
    64-bit machine, 0x0000000000012345 would be encoded as 0x00012345
    (minimum width of 4 bytes).  0x0000001234567890 would be encoded as
    0x01234567890 (leading 0x000000 omitted).


    Invocation:

        status = id3FrameEncodeVUInt (integer, bdx, &length) ;

    where

        <integer>	- I
            is the unsigned integer to be encoded.
        <bdx>		- I/O
            is a caller-supplied, MDX_UTIL descriptor for the buffer into
            which the integer will be encoded.  The encoded integer will
            be appended to the prior contents of the buffer.
        <length>	- O
            returns the length in bytes of the encoded integer.
        <status>	- O
            returns the status of encoding the integer, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


static  errno_t  id3FrameEncodeVUInt (

#    if PROTOTYPES
        size_t  integer,
        MemoryDx  bdx,
        size_t  *length)
#    else
        integer, bdx, length)

        size_t  integer ;
        MemoryDx  bdx ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    char  *buffer ;
    size_t  i, mask, numBytes, priorLength, value ;



    if ((bdx == NULL) || (length == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3FrameEncodeVUInt) NULL buffer descriptor or return length: ") ;
        return (errno) ;
    }

/* Determine the number of bytes needed to represent the integer. */

    mask = 0x0FF ;
    for (i = 1 ;  i < sizeof (size_t) ;  i++) {
        mask <<= 8 ;			/* Shift 0xFF to MSB. */
    }

    numBytes = sizeof (size_t) ;
    while ((numBytes > 0) && ((integer & mask) == 0)) {
        numBytes-- ;
        mask >>= 8 ;
    }

    if (numBytes < MIN_PLAYCOUNT_BYTE_WIDTH)
        numBytes = MIN_PLAYCOUNT_BYTE_WIDTH ;

/* Encode the integer byte-by-byte into the buffer. */

    priorLength = mdxLength (bdx) ;
    *length = 0 ;

    if (mdxAddS (bdx, false, NULL, numBytes)) {
        LGE "(id3FrameEncodeVUInt) Error allocating %"PRIuSIZE_T" extra bytes.\nmdxAddS: ",
            numBytes) ;
        return (errno) ;
    }

    buffer = mdxRegionP (bdx, priorLength) ;

    value = integer ;
    for (i = numBytes ;  i-- > 0 ;  value >>= 8) {
        buffer[i] = (char) (value & 0x0FF) ;
    }

    (*length) += numBytes ;

    LGI "(id3FrameEncodeVUInt) Encoded %"PRIuSIZE_T" in %"PRIuSIZE_T" bytes.\n",
        integer, numBytes) ;

    return (0) ;

}
