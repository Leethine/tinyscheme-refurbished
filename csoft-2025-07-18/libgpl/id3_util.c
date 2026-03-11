/* $Id: id3_util.c,v 1.21 2024/05/28 11:35:01 alex Exp $ */
/*******************************************************************************

File:

    id3_util.c

    ID3 Tag Utilities.


Author:    Alex Measday


Purpose:

    An ID3 tag contains identifying information about an MP3 audio track.
    Such information may include the track's song title, the artist's name,
    the album name, the publication/recording date, etc.

    The obsolete, but still frequently seen, version 1 (ID3v1) tag is a
    128-byte block with a fixed set of these fields.  The ID3v1 tag is
    stored at the end of the audio file.

    The newer version 2 (ID3v2) tags are stored at the beginning of the
    file.  ID3v2.3 tags are the most commonly seen version.  ID3v2.4 tags
    provide some additional frame types and additional methods for encoding
    Unicode characters, but don't differ significantly from ID3v2.3 tags.

    An ID3v2 "tag" consists of:

        - 10-byte tag header ('ID3', version, flags, size)
        - One or more ID3 frames for each unit of ID information

    Separate frame types are defined for the song title, artist's name, etc.
    An ID3v2 "frame" more generally consists of:

        - 10-byte frame header (4-character ID, size, flags)
        - One or more data values that, together, comprise the frame's
          unit of ID information

    ID3v2 data "values" (my term) include (i) ancillary data supporting
    a frame's key value or values and (ii) the key value or values in the
    frame's unit of ID information.  I think an example will make this
    clearer.  ID3 "attached picture" frames have a 4-character ID of 'APIC'
    and the key value in their unit of information is an image.  Each APIC
    frame has a number of supporting data values and the key value:

        Explicit data values:
            - Text encoding byte (indicating the format of Unicode strings)
            - MIME type (plain ASCII string specifying the image type,
              e.g., "image/png" and "image/jpeg")
            - Picture type (number specifying the ID3 picture type,
              e.g., front cover, back cover, artist photo)
            - Description (Unicode string per text encoding byte which
              describes this particular image)
            - Image (raw image data to be interpreted per the MIME type)
        Implicit data value:
            - Image data length (derived from the frame size)

    There may be multiple APIC frames with the same picture type, but they
    must have different descriptions.  For example, the tag for a Beatles
    song may have 4 "artist/performer" (0x08) pictures, one for each band
    member, but the descriptions in each of the 4 APIC frames must be
    different (perhaps "John", "Paul", "George", and "Ringo"!).

    An example of a frame with multiple, key data values is the ID3v2.4
    "Musician credits list" frame, the 'TMCL' frame:

        - Text encoding byte (indicating the format of Unicode strings)
        - Instrument string #1 (Unicode string for first musician's instrument)
        - Musician name #1 (Unicode string for first musician's name)
        ...
        - Instrument string #N (Unicode string for last musician's instrument)
        - Musician name #N (Unicode string for last musician's name)

    The contents of the strings are not standardized.  An instrument string
    could be a comma-separated list of instruments coupled with a single
    musician's name.  Or a single instrument coupled with a comma-separated
    list of musicians who played that instrument on the recording.

        Regarding the ID3v2.4 TMCL example, I should note an important
        difference between ID3v2.4 and ID3v2.3 frames.  ID3v2.4 allows
        a variable number of NUL-terminated strings at the end of a
        "text information" frame such as the TMCL frame.  ID3v2.3
        doesn't.  However, ID3v2.3's comparable 'IPLS' ("involved
        people list") frame, which looks like but is not a text
        information frame, does support a sequence of NUL-terminated
        strings in the frame.  Things I learned when I dug around
        while writing this paragraph!

        Now I need to go back and add support for the IPLS frame to
        ID3_UTIL.  The convention for the display and entry format
        of IPLS information among other programs seems to be as
        shown in this example:

            Guitar:John;Bass:Paul;Guitar:George;Drums:Ringo

        That's the display and entry format; the actual on-disk
        format is a sequence of NUL-terminated strings (ISO-8859-1
        encoding for the example's sake):

            Guitar\0John\0Bass\0Paul\0Guitar\0George\0Drums\0Ringo\0

    The ID3_UTIL package internally represents all ID3 tags (ID3v1, ID3v2.3,
    and ID3v2.4) as generalized ID3v2 tags using 3 object types consistent
    with the description above:

        Id3Tag - has the tag header information and a list of frames.

        Id3Frame - has the frame header information and a list of values.

        Id3Value - is an atomic bit of information; e.g., a string.

    The ID3 standard speaks of tags and frames, but not of data values at a
    third level in the hierarchy.  They seemed like an obvious abstraction
    to me and I began implementing ID3v2 frames with separate value objects
    for each data item (both supporting and key values) in a frame.  Accessing
    these items proved awkward and tedious, so I quickly switched to an
    "fdata" union of type-specific, frame data structures.  A frame type's
    key value and supporting data are stored in its structure and are easily
    accessed.  Additional key values allowed in ID3v2.4 and IPLS frames are
    stored in a list of value objects.  These additional values are all
    strings, but the Id3Value code still retains support for numbers and
    binary blobs as well, in line with my original intentions for these
    objects.

    SOURCE FILES
    ============

    There are two header files:

        id3_util.h
        id3_internals.h

    When I added ID3v2 support to ID3_UTIL, I created the internals header
    with the thought that only the tag, frame, and value C files would need
    to #include it.  However, as I developed the ID3v2 support in tandem
    with the TAG311 program, I couldn't make up my mind which parts of the
    tag, frame, and value functions/data needed to be accessible to an
    application.  For example, suppose an application wanted to create
    a frame whose type wasn't supported by the ID3_UTIL code (and thus
    was treated as an opaque, pass-through type by said code).  Such an
    application might need to work directly with the internal fields in
    tags and frames.  Consequently, "id3_util.h" itself #include's
    "id3_internals.h" and you only need to include the former.

    The resulting implementation accomplishes neither the originally
    desired hiding of implementation details nor the ability for an
    application to sensibly support unsupported frame types.  For the
    latter purpose, the application would also need to modify or
    supplement the internal frame type-marshaling mapping table used
    by id3FrameDecode() and id3FrameEncode() and I haven't provided
    that capability.

    There are 4 C files:

        id3_util.c
            - id3ReadTags()
            - id3WriteTags()
            - id3GetXyz() field functions
            - id3SetXyz() field functions
            - Other functions

        id3_tag.c
            - id3TagXyz() functions

        id3_frame.c
            - id3FrameXyz() functions

        id3_value.c
            - id3ValueXyz() functions


Public Procedures (in "id3_util.c"):

    id3CRC32() - computes the CRC-32 for a buffer of data.
    id3DecodeSSI() - decodes a synchsafe integer.
    id3DeUnsync() - de-unsynchronizes a data buffer.
    id3Dump() - dumps the contents of an ID3 tag.
    id3DumpGenres() - lists the ID3v1 music genres.
    id3DumpLUT() - lists the entries in the frame information table.
    id3DumpPicTypes() - lists the ID3v2 picture types.
    id3EncodeSSI() - encodes a synchsafe integer.
    id3IsEmpty() - checks if an ID3 tag is empty.
    id3IsFrame() - checks if a buffer begins contains an ID3 frame.
    id3IsTag() - checks if a buffer contains an ID3 tag.
    id3ReadHeaders() - reads the ID3 tag headers from a named file.
    id3ReadHeadersF() - reads the ID3 tag headers from an open file.
    id3ReadTags() - reads the ID3 tags from a named file.
    id3ReadTagsF() - reads the ID3 tags from an open file.
    id3ReadV1() - reads the ID3v1 tag from an open file.
    id3ReadV2() - reads the ID3v2 tag from an open file.
    id3Strip() - strips the ID3 tag from a named file.
    id3StripF() - strips the ID3 tag from an open file.
    id3Unsync() - unsynchronizes a data buffer.
    id3WriteTags() - writes ID3v1 and ID3v2 tags to a file.

    id3GetAlbum() - gets an ID3 tag's album field.
    id3GetArtist() - gets an ID3 tag's artist field.
    id3GetComment() - gets an ID3 tag's comment field.
    id3GetCover() - gets an ID3 tag's cover picture.
    id3GetGenre() - gets an ID3 tag's genre field.
    id3GetList() - gets an "involved people list" from an ID3 tag.
    id3GetObject() - gets an ID3 tag's general object.
    id3GetPlayCount() - gets a popularimeter/play-count field from an ID3 tag.
    id3GetPrivate() - gets a private data field from an ID3 tag.
    id3GetRegistration() - gets an ENCR/GRID registration field from an ID3 tag.
    id3GetSong() - gets an ID3 tag's song title.
    id3GetText() - gets a text information field from an ID3 tag.
    id3GetTrack() - gets sets an ID3 tag's track field.
    id3GetUFID() - gets a unique file ID from an ID3 tag.
    id3GetUnsyncLyrics() - gets unsynchronized lyrics from an ID3 tag.
    id3GetURL() - gets a URL link from an ID3 tag.
    id3GetVersion() - gets an ID3 tag's version.
    id3GetYear() - gets an ID3 tag's year field.

    id3SetAlbum() - sets an ID3 tag's album field.
    id3SetArtist() - sets an ID3 tag's artist field.
    id3SetComment() - sets an ID3 tag's comment field.
    id3SetCover() - sets an ID3 tag's cover picture.
    id3SetGenre() - sets an ID3 tag's genre field.
    id3SetList() - sets an "involved people list" in an ID3 tag.
    id3SetObject() - sets a general object in an ID3 tag.
    id3SetPlayCount() - sets a popularimeter/play-count field in an ID3 tag.
    id3SetPrivate() - sets a private data field in an ID3 tag.
    id3SetRegistration() - sets an ENCR/GRID registration field in an ID3 tag.
    id3SetSong() - sets an ID3 tag's song title.
    id3SetText() - sets a text information field in an ID3 tag.
    id3SetTrack() - sets an ID3 tag's track field.
    id3SetUFID() - sets a unique file ID in an ID3 tag.
    id3SetUnsyncLyrics() - sets unsynchronized lyrics in an ID3 tag.
    id3SetURL() - sets a URL in an ID3 tag.
    id3SetVersion() - sets an ID3 tag's version.
    id3SetYear() - sets an ID3 tag's year field.

    id3Flags() - gets an ID3v2 tag's header flags.
    id3Size() - gets an ID3v2 tag's size.

    id3FromGenre() - translates a genre number to its name.
    id3ToGenre() - translates a genre name to its number.

    id3FromPictureType() - translates a picture type to its name.
    id3ToPictureType() - translates a picture-type name to its number.

Public Procedures (in "id3_tag.c"):

    id3TagAddFrame() - adds/replaces an ID3 frame to/in an ID3 tag.
    id3TagCreate() - creates an empty ID3 tag.
    id3TagDecodeV1() - decodes an ID3v1 tag.
    id3TagDecodeV2() - decodes an ID3v2 tag.
    id3TagDeleteFrame() - deletes an ID3 frame from an ID3 tag.
    id3TagDeletePicture() - deletes an APIC frame from an ID3 tag.
    id3TagDestroy() - destroys an ID3 tag.
    id3TagDump() - formats and dumps an ID3 tag for debug purposes.
    id3TagEncodeV1() - encodes an ID3v1 tag into its file format.
    id3TagEncodeV2() - encodes an ID3v2 tag into its file format.
    id3TagErase() - erases the contents of an ID3 tag.
    id3TagFindFrame() - finds a specified frame in an ID3 tag.
    id3TagFrameCount() - gets the number of frames in an ID3 tag.
    id3TagGetFrame() - gets an indexed frame from an ID3 tag.
    id3TagMerge() - merges the contents of two ID3 tags into a third tag.
    id3TagSortFrames() - sorts the frames in an ID3 tag.

Public Procedures (in "id3_frame.c"):

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

Public Procedures (in "id3_value.c"):

    id3ValueCopy() - duplicates an ID3 value.
    id3ValueCreate() - creates an empty ID3 value.
    id3ValueDestroy() - destroys an ID3 value.
    id3ValueDump() - dumps an ID3 value.
    id3ValueErase() - erases the contents of an ID3 value.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */
#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#if !STDC_HEADERS
    extern  int  unlink () ;
#    define  remove(path)  unlink (path)
#endif
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#if !defined(HAVE_FTRUNCATE) || HAVE_FTRUNCATE
#    if defined(vaxc)
	/* DEC-C has ftruncate() in "unistd.h"; need to figure out VAX-C. */
#        undef  HAVE_FTRUNCATE
#        define  HAVE_FTRUNCATE  0
#    elif defined(_WIN32)
#        include  <io.h>		/* Low-level I/O definitions. */
#        define  ftruncate  _chsize
#    else
#        include  <unistd.h>		/* UNIX I/O definitions. */
#    endif
#endif
#include  "adx_util.h"			/* Array Descriptor utilities. */
#include  "fnm_util.h"			/* Filename utilities. */
#include  "mdx_util.h"			/* Memory Descriptor utilities. */
#include  "meo_util.h"			/* Memory operations. */
#include  "nnl_util.h"			/* Name-to-Number Lookup utilities. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "utf_util.h"			/* Unicode UTF utilities. */
#include  "id3_util.h"			/* ID3 tag utilities. */
#include  "id3_internals.h"		/* Internal definitions. */


int  id3_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  id3_util_debug

/*******************************************************************************
    ID3V1 Genre Lookup Table.
*******************************************************************************/

static  const  NNLTable  genreLUT  OCD ("id3_util")  = {
    { 0, "Blues" },
    { 1, "Classic Rock" },
    { 2, "Country" },
    { 3, "Dance" },
    { 4, "Disco" },
    { 5, "Funk" },
    { 6, "Grunge" },
    { 7, "Hip-Hop" },
    { 8, "Jazz" },
    { 9, "Metal" },
    { 10, "New Age" },
    { 11, "Oldies" },
    { 12, "Other" },
    { 13, "Pop" },
    { 14, "R&B" },
    { 15, "Rap" },
    { 16, "Reggae" },
    { 17, "Rock" },
    { 18, "Techno" },
    { 19, "Industrial" },
    { 20, "Alternative" },
    { 21, "Ska" },
    { 22, "Death Metal" },
    { 23, "Pranks" },
    { 24, "Soundtrack" },
    { 25, "Euro-Techno" },
    { 26, "Ambient" },
    { 27, "Trip-Hop" },
    { 28, "Vocal" },
    { 29, "Jazz+Funk" },
    { 30, "Fusion" },
    { 31, "Trance" },
    { 32, "Classical" },
    { 33, "Instrumental" },
    { 34, "Acid" },
    { 35, "House" },
    { 36, "Game" },
    { 37, "Sound Clip" },
    { 38, "Gospel" },
    { 39, "Noise" },
    { 40, "AlternRock" },
    { 40, "Alt. Rock" },	/* Alternative name. */
    { 41, "Bass" },
    { 42, "Soul" },
    { 43, "Punk" },
    { 44, "Space" },
    { 45, "Meditative" },
    { 46, "Instrumental Pop" },
    { 47, "Instrumental Rock" },
    { 48, "Ethnic" },
    { 49, "Gothic" },
    { 50, "Darkwave" },
    { 51, "Techno-Industrial" },
    { 52, "Electronic" },
    { 53, "Pop-Folk" },
    { 54, "Eurodance" },
    { 55, "Dream" },
    { 56, "Southern Rock" },
    { 57, "Comedy" },
    { 58, "Cult" },
    { 59, "Gangsta" },
    { 59, "Gangsta Rap" },	/* Alternative  name. */
    { 60, "Top 40" },
    { 61, "Christian Rap" },
    { 62, "Pop/Funk" },
    { 63, "Jungle" },
    { 64, "Native American" },
    { 65, "Cabaret" },
    { 66, "New Wave" },
    { 67, "Psychedelic" },	/* Misspelled as "Psychadelic" in standard. */
    { 68, "Rave" },
    { 69, "Showtunes" },
    { 70, "Trailer" },
    { 71, "Lo-Fi" },
    { 72, "Tribal" },
    { 73, "Acid Punk" },
    { 74, "Acid Jazz" },
    { 75, "Polka" },
    { 76, "Retro" },
    { 77, "Musical" },
    { 78, "Rock & Roll" },
    { 79, "Hard Rock" },
				/* End "standard" ID3v1 genres,
				   Begin Nullsoft Winamp extensions. */
    { 80, "Folk" },
    { 81, "Folk-Rock" },
    { 82, "National Folk" },
    { 83, "Swing" },
    { 84, "Fast Fusion" },
    { 84, "Fast-Fusion" },	/* Alternative name. */
    { 85, "Bebop" },		/* (Misspelled in standard as "Bebob". */
    { 86, "Latin" },
    { 87, "Revival" },
    { 88, "Celtic" },
    { 89, "Bluegrass" },
    { 90, "Avantgarde" },
    { 91, "Gothic Rock" },
    { 92, "Progressive Rock" },
    { 93, "Psychedelic Rock" },
    { 94, "Symphonic Rock" },
    { 95, "Slow Rock" },
    { 96, "Big Band" },
    { 97, "Chorus" },
    { 98, "Easy Listening" },
    { 99, "Acoustic" },
    { 100, "Humour" },
    { 101, "Speech" },
    { 102, "Chanson" },
    { 103, "Opera" },
    { 104, "Chamber Music" },
    { 105, "Sonata" },
    { 106, "Symphony" },
    { 107, "Booty Bass" },
    { 108, "Primus" },
    { 109, "Porn Groove" },
    { 110, "Satire" },
    { 111, "Slow Jam" },
    { 112, "Club" },
    { 113, "Tango" },
    { 114, "Samba" },
    { 115, "Folklore" },
    { 116, "Ballad" },
    { 117, "Power Ballad" },
    { 118, "Rhythmic Soul" },
    { 119, "Freestyle" },
    { 120, "Duet" },
    { 121, "Punk Rock" },
    { 122, "Drum Solo" },
    { 123, "A Cappella" },	/* Use this. */
    { 123, "Acappella" },	/* Spelling varies between ID3v2 standards. */
    { 124, "Euro-House" },
    { 125, "Dance Hall" },
    { 126, "Goa" },
    { 127, "Drum & Bass" },
    { 128, "Club-House" },
    { 129, "Hardcore" },
    { 130, "Terror" },
    { 131, "Indie" },
    { 132, "BritPop" },
    { 133, "Negerpunk" },	/* Apparently a bigoted joke entry.  Seriously. */
    { 134, "Polsk Punk" },
    { 135, "Beat" },
    { 136, "Christian Gangsta Rap" },
    { 137, "Heavy Metal" },
    { 138, "Black Metal" },
    { 139, "Crossover" },
    { 140, "Contemporary Christian" },
    { 141, "Christian Rock" },
    { 142, "Merengue" },
    { 143, "Salsa" },
    { 144, "Thrash Metal" },
    { 145, "Anime" },
    { 146, "JPop" },
    { 147, "Synthpop" },
    { 148, "Abstract" },
    { 149, "Art Rock" },
    { 150, "Baroque" },
    { 151, "Bhangra" },
    { 152, "Big Beat" },
    { 153, "Breakbeat" },
    { 154, "Chillout" },
    { 155, "Downtempo" },
    { 156, "Dub" },
    { 157, "EBM" },
    { 158, "Eclectic" },
    { 159, "Electro" },
    { 160, "Electroclash" },
    { 161, "Emo" },
    { 162, "Experimental" },
    { 163, "Garage" },
    { 164, "Global" },
    { 165, "IDM" },
    { 166, "Illbient" },
    { 167, "Industro-Goth" },
    { 168, "Jam Band" },
    { 169, "Krautrock" },
    { 170, "Leftfield" },
    { 171, "Lounge" },
    { 172, "Math Rock" },
    { 173, "New Romantic" },
    { 174, "Nu-Breakz" },
    { 175, "Post-Punk" },
    { 176, "Post-Rock" },
    { 177, "Psytrance" },
    { 178, "Shoegaze" },
    { 179, "Space Rock" },
    { 180, "Trop Rock" },
    { 181, "World Music" },
    { 182, "Neoclassical" },
    { 183, "Audiobook" },
    { 184, "Audio Theatre" },
    { 185, "Neue Deutsche Welle" },
    { 186, "Podcast" },
    { 187, "Indie Rock" },
    { 188, "G-Funk" },
    { 189, "Dubstep" },
    { 190, "Garage Rock" },
    { 191, "Psybient" },
    { 255, "None" },
    { -1, NULL }
} ;

/*******************************************************************************
    ID3V2 Picture Type Lookup Table.
*******************************************************************************/

static  const  NNLTable  pictureTypeLUT  OCD ("id3_util")  = {
    { ID3_PIC_OTHER, "Other" },
    { ID3_PIC_FILE_ICON, "32x32 pixels 'file icon' (PNG only)" },
    { ID3_PIC_OTHER_ICON, "Other file icon" },
    { ID3_PIC_COVER_FRONT, "Cover (front)" },
    { ID3_PIC_COVER_BACK, "Cover (back)" },
    { ID3_PIC_LEAFLET_PAGE, "Leaflet page" },
    { ID3_PIC_MEDIA, "Media (e.g. label side of CD)" },
    { ID3_PIC_LEAD_ARTIST, "Lead artist/lead performer/soloist" },
    { ID3_PIC_ARTIST, "Artist/performer" },
    { ID3_PIC_CONDUCTOR, "Conductor" },
    { ID3_PIC_BAND_ORCHESTRA, "Band/Orchestra" },
    { ID3_PIC_COMPOSER, "Composer" },
    { ID3_PIC_LYRICIST, "Lyricist/text writer" },
    { ID3_PIC_RECORDING_LOCATION, "Recording Location" },
    { ID3_PIC_DURING_RECORDING, "During recording" },
    { ID3_PIC_DURING_PERFORMANCE, "During performance" },
    { ID3_PIC_SCREEN_CAPTURE, "Movie/video screen capture" },
    { ID3_PIC_BRIGHT_COLOURED_FISH, "A bright coloured fish" },
    { ID3_PIC_ILLUSTRATION, "Illustration" },
    { ID3_PIC_ARTIST_LOGOTYPE, "Band/artist logotype" },
    { ID3_PIC_PUBLISHER_LOGOTYPE, "Publisher/Studio logotype" },
    { -1, NULL }
} ;

/*!*****************************************************************************

Procedure:

    id3CRC32 ()

    Compute the CRC-32 for a Buffer of Data.


Purpose:

    Function id3CRC32() computes the 32-bit, CRC-32 cyclic redundancy check
    for a buffer of data.

    The code is cut and pasted (with cosmetic changes to fit my coding style)
    from Stephan Brumme's "Fast CRC32" web page:

        https://create.stephan-brumme.com/crc32/

    (I'm using his "Bitwise, Improved Branch-free" algorithm, which is
    certainly fast enough for not-especially-large ID3 tags.)  His chosen
    polynomial is found at the cited Wikipedia web page:

        https://en.wikipedia.org/wiki/Cyclic_redundancy_check#Polynomial_representations_of_cyclic_redundancy_checks

    It's the CRC-32 "Reversed" polynomial.

********************************************************************************

    Stephan Brumme's code License:

        This software is provided 'as-is', without any express or implied
        warranty. In no event will the authors be held liable for any damages
        arising from the use of this software.

        Permission is granted to anyone to use this software for any purpose,
        including commercial applications, and to alter it and redistribute it
        freely, subject to the following restrictions:

        1. The origin of this software must not be misrepresented; you must not
           claim that you wrote the original software. If you use this software
           in a product, an acknowledgment in the product documentation would be
           appreciated but is not required.
        2. Altered source versions must be plainly marked as such, and must not
           be misrepresented as being the original software.
        3. This notice may not be removed or altered from any source
           distribution.zlib License

********************************************************************************

    Invocation:

        crc = id3CRC32 (buffer, length, previousCRC32) ;

    where

        <buffer>	- I
            is the buffer of data.
        <length>	- I
            is the number of bytes in the buffer.
        <previousCRC32>	- I
            is the previous CRC when computing the new CRC for appended data.
            Per Stephan Brumme's web page, this implementation is a "rolling
            algorithm.  That means, if you already have some chunk of data and
            its CRC, then you can append new data and compute the updated CRC
            but using your original CRC as a seed and just scanning through
            the appended data."
        <crc>		- O
            returns the computed CRC-32.

*******************************************************************************/


uint32_t  id3CRC32 (

#    if PROTOTYPES
        const  void  *buffer,
        size_t  length,
        uint32_t  previousCRC32)
#    else
        buffer, length, previousCRC32)

        void  *buffer ;
        size_t  length ;
        uint32_t  previousCRC32 ;
#    endif

{    /* Local variables. */
    uint32_t  crc = ~previousCRC32 ;	/* Same as previousCrc32 ^ 0xFFFFFFFF */
    unsigned  char*  current = (unsigned char*) buffer ;
#define  ID3_CRC32_POLYNOMIAL  0xEDB88320

    while (length--) {
        crc ^= *current++ ;
        for (unsigned int j = 0 ;  j < 8 ;  j++) {
            crc = (crc >> 1) ^ (-(crc & 1) & ID3_CRC32_POLYNOMIAL) ;
        }
    }

    return (~crc) ;

}

/*!*****************************************************************************

Procedure:

    id3DecodeSSI ()

    Decode an ID3 Synchsafe Integer.


Purpose:

    Function id3DecodeSSI() decodes an ID3 "synchsafe" integer from
    the 7 least-significant bits of N consecutive bytes.  Synchsafe
    integers are used for tag and ID3v2.4 frame sizes (28 bits spread
    over 4 bytes) and tag CRCs (32 bits spread over 5 bytes).


    Invocation:

        value = id3DecodeSSI (buffer, length) ;

    where

        <buffer>	- I
            is a buffer containing the encoded integer.
        <length>	- I
            is the number of bytes in the buffer, 4 or 5.
        <value>		- O
            returns the decoded integer.

*******************************************************************************/


unsigned  long  id3DecodeSSI (

#    if PROTOTYPES
        const  char  *buffer,
        size_t  length)
#    else
        buffer, length)

        char  *buffer ;
        size_t  length ;
#    endif

{    /* Local variables. */
    unsigned  long  value ;
    size_t  i ;



    if ((buffer == NULL) || (length == 0))  return (0) ;

    value = 0 ;

    for (i = 0 ;  i < length ;  i++) {
        value <<= 7 ;
        value |= (buffer[i] & 0x07FUL) ;
    }

    return (value) ;

}

/*!*****************************************************************************

Procedure:

    id3DeUnsync ()

    De-Unsynchronize a Data Buffer.


Purpose:

    Function id3DeUnsync() converts a buffer of ID3-unsynchronized data
    to its non-unsychronized form.

    ID3 data may contain false MP3 sync codes.  To eliminate these, the
    original data can be "unsynchronized" by inserting a 0x00 byte between
    two adjacent bytes in these cases:

        0xFFFx  =>  0xFF <0x00> 0xFx		(12 consecutive 1 bits)
        0xFFEx  =>  0xFF <0x00> 0xEx		(11 consecutive 1 bits)
        0xFF00  =>  0xFF <0x00> 0x00		(Special case)

    To recover the original data, id3DeUnsync() performs the reverse
    operation, shortening each 2-byte 0xFF00 sequence to one byte, 0xFF.


    Invocation:

        numSync = id3DeUnsync (buffer, length) ;

    where

        <buffer>	- I
            is the buffer of unsynchronized data.
        <length>	- I
            is the length in bytes of the buffer.
        <numSync>	- O
            returns the number of (0xFF,0x00) pairs (indicating false
            sync codes) converted to 0xFF; i.e., the new length of the
            buffer is <length> - <numSync>.

*******************************************************************************/


size_t  id3DeUnsync (

#    if PROTOTYPES
        char  *buffer,
        size_t  length)
#    else
        buffer, length)

        char  *buffer ;
        size_t  length ;
#    endif

{    /* Local variables */
    char  *s, *t ;
    size_t  numSync ;



    if ((buffer == NULL) || (length < 2))  return (0) ;

    numSync = 0 ;
    s = t = buffer ;

    while (length-- > 0) {		/* Enough to check 2 bytes? */
        if (((s[0] & 0x0FF) == 0x0FF) && (length > 0) && (s[1] == 0)) {
            *t++ = *s++ ;  s++ ;	/* Keep 0xFF and skip 0x00. */
            numSync++ ;  length-- ;
        } else {
            *t++ = *s++ ;
        }
    }

    return (numSync) ;

}

/*!*****************************************************************************

Procedure:

    id3Dump ()

    Dump the Contents of an ID3 Tag.


Purpose:

    Function id3Dump() dumps the contents of an ID3 tag to a file
    in a human-readable format.


    Invocation:

        status = id3Dump (file, indentation, brief, numberFrames, tag) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.
            NULL can be specified for ""; i.e., no indentation.
        <brief>		- I
            controls the output format, brief (true) or long (false).
        <numberFrames>		- I
            controls the numbering 1..N of ID3v2 frames: number each frame
            (true) or don't display numbers (false).
        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <status>	- O
            returns the status of generating the dump, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3Dump (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation,
        bool  brief,
        bool  numberFrames,
        Id3Tag  tag)
#    else
        file, indentation, brief, numberFrames, tag)

        FILE  *file ;
        char  *indentation ;
        bool  brief ;
        bool  numberFrames ;
        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    const  char  *comment, *frameID, *genre ;
    Id3Frame  *frame ;
    Id3FrameLUTEntry  *info ;
    Id3Version  version ;
    size_t  count, i, length, MXT ;




    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3Dump) NULL tag handle: ") ;
        return (errno) ;
    }

    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    version = id3GetVersion (tag) ;

/* Dump an ID3V1 tag. */

    if (version < ID3V_V2) {

        comment = id3GetComment (tag, NULL, NULL) ;
        genre = id3GetGenre (tag) ;

        if (brief) {
            fprintf (file, "%s%2ld\t%-30s\t(%s)\n",
                    indentation,
                    id3GetTrack (tag, NULL),
                    id3GetSong (tag),
                    (comment == NULL) ? "" : comment) ;
            fprintf (file, "%s%-30s\t%s\t%ld\t(%s)\n",
                    indentation,
                    id3GetArtist (tag),
                    id3GetAlbum (tag),
                    id3GetYear (tag),
                    (genre == NULL) ? "Unknown" : genre) ;
        } else {
            fprintf (file, "%sSong:\t\t%s\n", indentation, id3GetSong (tag)) ;
            fprintf (file, "%sArtist:\t\t%s\n", indentation, id3GetArtist (tag)) ;
            fprintf (file, "%sAlbum:\t\t%s\n", indentation, id3GetAlbum (tag)) ;
            fprintf (file, "%sYear:\t\t%ld\n", indentation, id3GetYear (tag)) ;
            fprintf (file, "%sComment:\t%s\n", indentation, (comment == NULL) ? "(None)" : comment) ;
            fprintf (file, "%sTrack:\t\t%ld\n", indentation, (long) id3GetTrack (tag, NULL)) ;
            fprintf (file, "%sGenre:\t\t%s\n", indentation, (genre == NULL) ? "Unknown" : genre) ;
        }

    }

/* Dump an ID3V2 tag. */

    else if (version >= ID3V_V2) {

        count = adxCount (tag->frames) ;

        fprintf (file, "ID3v%ld.%ld:  size %"PRIuSIZE_T"  flags 0x%02X  %"PRIuSIZE_T" frames\n",
                 ID3V_VERSION (version), ID3V_MAJOR (version),
                 tag->v2Size, tag->v2Flags, count) ;

        for (i = 0 ;  i < count ;  i++) {
            if (numberFrames) {
                fprintf (file, "%2"PRIuSIZE_T" ", i + 1) ;
		/* Need extra tab to align in "<nn> <name>:\t<value>". */
                MXT = 4 ;
            } else {
		/* Need extra tab to align in "<name>:\t<value>". */
                MXT = 7 ;
            }
            frame = adxGet (tag->frames, i) ;
            frameID = (*frame)->frameID ;
            info = (*frame)->info ;
            if (info->marshalType == Id3MarshalComment) {
                if (((*frame)->fdata.comm.description == NULL) ||
                    (strlen ((*frame)->fdata.comm.description) == 0)) {
                    fprintf (file, "%s%s (%.3s):\t%s\n",
                             indentation,
                             info->name,
                             (*frame)->fdata.comm.language,
                             (*frame)->fdata.comm.text) ;
                } else {
                    fprintf (file, "%s%s (%.3s):\t%s  (%s)\n",
                             indentation,
                             info->name,
                             (*frame)->fdata.comm.language,
                             (*frame)->fdata.comm.text,
                             (*frame)->fdata.comm.description) ;
                }
            } else if (info->marshalType == Id3MarshalGenObj) {
                if (((*frame)->fdata.geob.description == NULL) ||
                    (strlen ((*frame)->fdata.geob.description) == 0)) {
                    fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\t(%s) (%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.geob.length,
                             (*frame)->fdata.geob.mimeType,
                             (*frame)->fdata.geob.fileName) ;
                } else {
                    fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\t(%s) (%s) (%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.geob.length,
                             (*frame)->fdata.geob.mimeType,
                             (*frame)->fdata.geob.fileName,
                             (*frame)->fdata.geob.description) ;
                }
            } else if (info->marshalType == Id3MarshalPicture) {
                if (((*frame)->fdata.apic.description == NULL) ||
                    (strlen ((*frame)->fdata.apic.description) == 0)) {
                    fprintf (file, "%s%s:%s%s\t%"PRIuSIZE_T" bytes\t(%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             id3FromPictureType ((*frame)->fdata.apic.type),
                             (*frame)->fdata.apic.length,
                             (*frame)->fdata.apic.mimeType) ;
                } else {
                    fprintf (file, "%s%s:%s%s\t%"PRIuSIZE_T" bytes\t(%s) (%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             id3FromPictureType ((*frame)->fdata.apic.type),
                             (*frame)->fdata.apic.length,
                             (*frame)->fdata.apic.mimeType,
                             (*frame)->fdata.apic.description) ;
                }
            } else if (info->marshalType == Id3MarshalPlayCount) {
                if (info->type == ID3_PLAYCOUNT) {
                    fprintf (file, "%s%s:%s%zu\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.play.counter) ;
                } else if ((*frame)->fdata.play.omit) {
                    fprintf (file, "%s%s:%s<omit> (%s,%u)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.play.email,
                             (*frame)->fdata.play.rating) ;
                } else {
                    fprintf (file, "%s%s:%s%zu (%s,%u)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.play.counter,
                             (*frame)->fdata.play.email,
                             (*frame)->fdata.play.rating) ;
                }
            } else if (info->marshalType == Id3MarshalPrivate) {
                if (((*frame)->fdata.priv.ownerID == NULL) ||
                    (strlen ((*frame)->fdata.priv.ownerID) == 0)) {
                    fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\t()\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.priv.length) ;
                } else {
                    fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\t(%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.priv.length,
                             (*frame)->fdata.priv.ownerID) ;
                }
            } else if (info->marshalType == Id3MarshalPVList) {
                char  *listSpec = id3FramePVLtoS (*frame) ;
                fprintf (file, "%s%s:%s%s\n",
                         indentation,
                         info->name,
                         (strlen (info->name) < MXT) ? "\t\t" : "\t",
                         listSpec) ;
            } else if (info->marshalType == Id3MarshalPrivate) {
                if (((*frame)->fdata.regi.ownerID == NULL) ||
                    (strlen ((*frame)->fdata.regi.ownerID) == 0)) {
                    fprintf (file, "%s%s:%sSymbol 0x%02X, %"PRIuSIZE_T" bytes\t()\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (unsigned int) (*frame)->fdata.regi.symbol,
                             (*frame)->fdata.regi.length) ;
                } else {
                    fprintf (file, "%s%s:%sSymbol 0x%02X, %"PRIuSIZE_T" bytes\t(%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (unsigned int) (*frame)->fdata.regi.symbol,
                             (*frame)->fdata.regi.length,
                             (*frame)->fdata.regi.ownerID) ;
                }
            } else if (info->marshalType == Id3MarshalText) {
                if ((*frame)->type == ID3_TEXT_GENERIC) {
                    length = strlen (info->name) + 1 ;
                    if (frameID != NULL)  length += strlen (frameID) ;
                    fprintf (file, "%s%s/%s:%s%s\n",
                             indentation,
                             info->name,
                             (frameID == NULL) ? "" : frameID,
                             (length < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.text.text) ;
                } else {
                    fprintf (file, "%s%s:%s%s\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.text.text) ;
                }
            } else if (info->marshalType == Id3MarshalUFID) {
                if (((*frame)->fdata.ufid.ownerID == NULL) ||
                    (strlen ((*frame)->fdata.ufid.ownerID) == 0)) {
                    fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\t()\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.ufid.length) ;
                } else {
                    fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\t(%s)\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.ufid.length,
                             (*frame)->fdata.ufid.ownerID) ;
                }
            } else if (info->marshalType == Id3MarshalUnsyncLyrics) {
                if (((*frame)->fdata.uslt.description == NULL) ||
                    (strlen ((*frame)->fdata.uslt.description) == 0)) {
                    fprintf (file, "%s%s (%.3s):\t%zu characters\n",
                             indentation,
                             info->name,
                             (*frame)->fdata.uslt.language,
                             strlen ((*frame)->fdata.uslt.lyrics)) ;
                } else {
                    fprintf (file, "%s%s (%.3s):\t%zu characters  (%s)\n",
                             indentation,
                             info->name,
                             (*frame)->fdata.uslt.language,
                             strlen ((*frame)->fdata.uslt.lyrics),
                             (*frame)->fdata.uslt.description) ;
                }
            } else if (info->marshalType == Id3MarshalURL) {
                if ((*frame)->type == ID3_WURL_GENERIC) {
                    length = strlen (info->name) + 1 ;
                    if (frameID != NULL)  length += strlen (frameID) ;
                    fprintf (file, "%s%s/%s:%s%s\n",
                             indentation,
                             info->name,
                             (frameID == NULL) ? "" : frameID,
                             (length < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.wurl.url) ;
                } else {
                    fprintf (file, "%s%s:%s%s\n",
                             indentation,
                             info->name,
                             (strlen (info->name) < MXT) ? "\t\t" : "\t",
                             (*frame)->fdata.wurl.url) ;
                }
            } else if (info->marshalType == Id3MarshalUText) {
                length = strlen (info->name) + strlen (" ()") +
                         strlen ((*frame)->fdata.text.description) ;
                fprintf (file, "%s%s (%s):%s%s\n",
                         indentation,
                         info->name,
                         (*frame)->fdata.text.description,
                         (length < MXT) ? "\t\t" : "\t",
                         (*frame)->fdata.text.text) ;
            } else if (info->marshalType == Id3MarshalUURL) {
                length = strlen (info->name) + strlen (" ()") +
                         strlen ((*frame)->fdata.wurl.description) ;
                fprintf (file, "%s%s (%s):%s%s\n",
                         indentation,
                         info->name,
                         (*frame)->fdata.wurl.description,
                         (length < MXT) ? "\t\t" : "\t",
                         (*frame)->fdata.wurl.url) ;
            } else if (info->marshalType == Id3MarshalPassthrough) {
                fprintf (file, "%s%s:%s%s  %"PRIuSIZE_T" bytes\n",
                         indentation,
                         info->name,
                         (strlen (info->name) < MXT) ? "\t\t" : "\t",
                         (*frame)->frameID,
                         (*frame)->size) ;
            } else {
                fprintf (file, "%s%s:%s%"PRIuSIZE_T" bytes\n",
                         indentation,
                         info->name,
                         (strlen (info->name) < MXT) ? "\t\t" : "\t",
                         (*frame)->size) ;
            }
        }

    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3DumpGenres ()

    List the ID3v1 Music Genres.


Purpose:

    The id3DumpGenres() function generates a listing of the ID3v1 music
    numbers and genres.


    Invocation:

        status = id3DumpGenres (file, indentation) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <status>	- O
            returns the status of listing the genres, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3DumpGenres (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation)
#    else
        file, indentation)

        FILE  *file ;
        char  *indentation ;
#    endif

{    /* Local variables. */
    const  char  *s ;
    ssize_t  genre ;



    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    for (genre = 0 ;  ;  genre++) {
        s = id3FromGenre (genre) ;
        if (s == NULL)  break ;
        fprintf (file, "%s%3d: %s\n", indentation, (int) genre, s) ;
    }

    fprintf (file, "%s%3d: %s\n", indentation, 255, id3FromGenre (255)) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3DumpLUT ()

    List the Entries in the Frame Information Lookup Table.


Purpose:

    The id3DumpLUT() function formats and prints the frame information
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

        status = id3DumpLUT (file, indentation) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <status>	- O
            returns the status of listing the entries, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3DumpLUT (

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

    id3DumpPicTypes ()

    List the ID3v2 Picture Types.


Purpose:

    The id3DumpPicTypes() function generates a listing of the ID3v2
    picture types defined for APIC frames.


    Invocation:

        status = id3DumpPicTypes (file, indentation) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <status>	- O
            returns the status of listing the types, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3DumpPicTypes (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation)
#    else
        file, indentation)

        FILE  *file ;
        char  *indentation ;
#    endif

{    /* Local variables. */
    size_t  i ;



    if (file == NULL)  file = stdout ;
    if (indentation == NULL)  indentation = "" ;

    for (i = 0 ;  pictureTypeLUT[i].number >= 0 ;  i++) {
        fprintf (file, "%s%2ld: %s\n",
                 indentation,
                 pictureTypeLUT[i].number,
                 pictureTypeLUT[i].name) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3EncodeSSI ()

    Encode an ID3 Synchsafe Integer.


Purpose:

    Function id3EncodeSSI() encodes an ID3 "synchsafe" integer into
    the 7 least-significant bits of N consecutive bytes.  Synchsafe
    integers are used for tag and ID3v2.4 frame sizes (28 bits spread
    over 4 bytes) and tag CRCs (32 bits spread over 5 bytes).


    Invocation:

        id3EncodeSSI (value, length, buffer) ;

    where

        <value>		- I
            is the value to be encoded.
        <length>	- I
            is the number of bytes in the buffer, 4 or 5.
        <buffer>	- O
            is the buffer into which the encode value is stored.

*******************************************************************************/


void  id3EncodeSSI (

#    if PROTOTYPES
        unsigned  long  value,
        size_t  length,
        char  *buffer)
#    else
        value, length, buffer)

        unsigned  long  value ;
        size_t  length ;
        char  *buffer ;
#    endif

{

    if (buffer == NULL) return ;

    while (length-- > 0) {
        buffer[length] = value & 0x07FUL ;
        value >>= 7 ;
    }

    return ;

}

/*!*****************************************************************************

Procedure:

    id3Encoding ()

    Get an ID3 Tag's Preferred Text Encoding.


Purpose:

    Function id3Encoding() returns an ID3 tag's preferred text encoding;
    i.e., the preferred method for encoding wide-character strings in
    the tag's frames.


    Invocation:

        encoding = id3Encoding (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <encoding>	- O
            returns the ID3 tag's preferred text encoding method;
            see the Id3TextEncoding type definition in "id3_util.h".

*******************************************************************************/


Id3TextEncoding  id3Encoding (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL)
        return (ID3_UTF16_WITH_BOM) ;	/* Common to ID3v2.3 and ID3v2.4. */
    else
        return (tag->encoding) ;

}

/*!*****************************************************************************

Procedure:

    id3IsEmpty ()

    Check If an ID3 Tag is Empty.


Purpose:

    Function id3IsEmpty() returns true if all the fields in an ID3 tag are
    cleared and false otherwise.


    Invocation:

        isEmpty = id3IsEmpty (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <isEmpty>	- O
            returns true if all the fields in the ID3 tag are cleared and
            false otherwise.

*******************************************************************************/


bool  id3IsEmpty (

#    if PROTOTYPES
        const  Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL)  return (true) ;

    return ((tag->song == NULL) &&
            (tag->artist == NULL) &&
            (tag->album == NULL) &&
            (tag->comment == NULL) &&
            (tag->year <= 0) &&
            (tag->track <= 0) &&
            (tag->genre <= 0)) ;

}

/*!*****************************************************************************

Procedure:

    id3IsFrame()

    Check If a Buffer Begins With a Valid Frame ID.


Purpose:

    Function id3IsFrame() checks if a buffer has a valid ID3v2 frame ID
    (the 4-character ID at the beginning of the header).  Or at least
    looks like a valid frame ID (only upper-case letters and digits).


    Invocation:

        isFrame = id3IsFrame (buffer, length) ;

    where

        <buffer>	- I
            is the address of a buffer possibly holding an ID3v2 frame.
        <length>	- I
            is the length in bytes of the buffer.
        <isTag>		- O
            returns true if the buffer contains an ID3v2 frame ID and
            false if not.

*******************************************************************************/


bool  id3IsFrame (

#    if PROTOTYPES
        const  char  *buffer,
        size_t  length)
#    else
        buffer, length)

        char  *buffer ;
        size_t  length ;
#    endif

{    /* Local variables. */
    size_t  frameLength, i ;



    if ((buffer == NULL) || (length < ID3_V2_HEADER_SIZE))  return (false) ;

/* Check that the first character is an upper-case letter. */

    if (!isascii (INT_UCHAR (buffer[0])) ||
        !isalpha (INT_UCHAR (buffer[0])) ||
        !isupper (INT_UCHAR (buffer[0]))) {
        return (false) ;
    }

/* Check that the remaining 3 characters are either upper-case letters or
   numeric digits. */

    for (i = 1 ;  i < 4 ;  i++) {
        if (!isascii (INT_UCHAR (buffer[i])))  return (false) ;
        if (!isalnum (INT_UCHAR (buffer[i])))  return (false) ;
        if (isdigit (INT_UCHAR (buffer[i])))  continue ;
        if (!isupper (INT_UCHAR (buffer[i])))  return (false) ;
    }

/* Check that encoded frame length is less than/equal to buffer length. */

    frameLength = ID3_V2_HEADER_SIZE + (size_t) id3DecodeSSI (&buffer[4], 4) ;
    if (frameLength > length)  return (false) ;

    return (true) ;			/* Looks like an ID3v2 frame ID. */

}

/*!*****************************************************************************

Procedure:

    id3IsTag ()

    Check If a Buffer Contains an ID3 Tag.


Purpose:

    Function id3IsTag() checks if a buffer contains an ID3 tag.


    Invocation:

        isTag = id3IsTag (buffer, length) ;

    where

        <buffer>	- I
            is the address of a buffer possibly holding an ID3 tag.
        <length>	- I
            is the length in bytes of the buffer.
        <isTag>		- O
            returns true if the buffer contains an ID3 tag and false if not.

*******************************************************************************/


bool  id3IsTag (

#    if PROTOTYPES
        const  char  *buffer,
        size_t  length)
#    else
        buffer, length)

        char  *buffer ;
        size_t  length ;
#    endif

{    /* Local variables. */
    Id3Version  version ;



    if (buffer == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3IsTag) NULL buffer: ") ;
        return (false) ;
    }

/* Check for prefix string: "TAG" in ID3v1 tags and "ID3" in ID3v2 tags. */

    if (length < 3)  return (false) ;

    if ((buffer[0] == 'T') && (buffer[1] == 'A') && (buffer[2] == 'G'))
        version = ID3V_V1 ;
    else if ((buffer[0] == 'I') && (buffer[1] == 'D') && (buffer[2] == '3'))
        version = ID3V_V2 ;
    else
        return (false) ;

/* Check the buffer size. */

    if (ID3V_VERSION (version) == 1)
        return (length >= ID3_V1_TRAILER_SIZE) ;

    if ((ID3V_VERSION (version) == 2) && (length < ID3_V2_HEADER_SIZE))
        return (false) ;

/* ID3v2 tag - verify encoded version in buffer. */

    version = ID3V (2, buffer[3], buffer[4]) ;

    return (ID3V_VERSION (version) == 2) ;

}

/*!*****************************************************************************

Procedure:

    id3ReadHeaders ()

    Read the ID3 Tag Headers from a Named File.


Purpose:

    The id3ReadHeaders() function reads the ID3v1 and/or ID3v2 tag headers,
    if present, from a specified file.

            NOTE: The ID3v2 header and the ID3v1 tag are stored
            internally in id3ReadHeadersF().  They should be used
            or duplicated before calling id3ReadHeadersF() again,
            either directly or, as in this case, indirectly.


    Invocation:

        status = id3ReadHeaders (fileName, &v1Tag, &v2Header) ;

    where

        <fileName>	- I
            is the name of the file from which the ID3 tag headers
            will be read.  Environment variables may be embedded
            in the file name.
        <v1Tag>		- O
            returns the address of the raw, 128-byte ID3v1 tag; NULL is
            returned if the file has no ID3v1 tag.  (There is no ID3v1
            "header", so the full tag is returned.)  The tag is stored
            internally and should be used or duplicated before calling
            this function again.  If this argument is NULL (i.e., the
            argument, not the returned address), the ID3v2 tag header
            is not read.
       <v2Header>	- O
            returns the address of the 10-byte ID3v2 tag header; NULL is
            returned if the file has no ID3v2 tag.  The header is stored
            internally and it should be used or duplicated before calling
            this function again.  If this argument is NULL (i.e., the
            argument, not the returned address), the ID3v2 tag header
            is not read.
        <status>	- O
            returns the status of getting the header/tag from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ReadHeaders (

#    if PROTOTYPES
        const  char  *fileName,
        char  **v1Tag,
        char  **v2Header)
#    else
        fileName, v1Tag, v2Header)

        char  *fileName ;
        char  **v1Tag ;
        char  **v2Header ;
#    endif

{    /* Local variables. */
    char  *pathname ;
    FILE  *file ;



/* Open the file. */

    pathname = fnmBuild (FnmPath, fileName, NULL) ;
    if (pathname == NULL) {
        LGE "(id3ReadHeaders) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
        return (errno) ;
    }

    LGI "(id3ReadHeaders) File: \"%s\"\n", pathname) ;

    file = fopen (pathname, "rb") ;
    if (file == NULL) {
        LGE "(id3ReadHeaders) Error opening \"%s\".\n fopen: ", pathname) ;
        PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Get the ID3 header/tag from the file. */

    if (id3ReadHeadersF (file, v2Header, v1Tag)) {
        LGE "(id3ReadHeaders) Error getting ID3 header/tag from \"%s\".\nid3ReadHeadersF: ", pathname) ;
        PUSH_ERRNO ;  fclose (file) ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

    fclose (file) ;

    free (pathname) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ReadHeadersF ()

    Read the ID3 Tag Headers from an Open File.


Purpose:

    The id3ReadHeadersF() function reads the ID3v1 and/or ID3v2 tag headers,
    if present, from an open file.


    Invocation:

        status = id3ReadHeadersF (file, &v1Tag, &v2Header) ;

    where

        <file>		- I
            is the Unix FILE* handle for the previously opened file.
        <v1Tag>		- O
            returns the address of the raw, 128-byte ID3v1 tag; NULL is
            returned if the file has no ID3v1 tag.  (There is no ID3v1
            "header", so the full tag is returned.)  The tag is stored
            internally and should be used or duplicated before calling
            this function again.  If this argument is NULL (i.e., the
            argument, not the returned address), the ID3v2 tag header
            is not read.
       <v2Header>	- O
            returns the address of the 10-byte ID3v2 tag header; NULL is
            returned if the file has no ID3v2 tag.  The header is stored
            internally and it should be used or duplicated before calling
            this function again.  If this argument is NULL (i.e., the
            argument, not the returned address), the ID3v2 tag header
            is not read.
        <status>	- O
            returns the status of getting the header/tag from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ReadHeadersF (

#    if PROTOTYPES
        FILE  *file,
        char  **v1Tag,
        char  **v2Header)
#    else
        file, v1Tag, v2Header)

        FILE  *file ;
        char  **v1Tag ;
        char  **v2Header ;
#    endif

{    /* Local variables. */
    static  char  v1Buffer[ID3_V1_TRAILER_SIZE],
                  v2Buffer[ID3_V2_HEADER_SIZE] ;



/*******************************************************************************
    Read the ID3v2 header.
*******************************************************************************/

    if (v2Header != NULL) {

/* Position to and read the first 10 bytes in the file. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#        warning  id3ReadHeadersF: No fseek(3).
#else
        if (fseek (file, 0, SEEK_SET)) {
            LGE "(id3ReadHeadersF) Error positioning to ID3v2 tag.\nfseek: ") ;
            return (errno) ;
        }
#endif

        if (fread (v2Buffer, ID3_V2_HEADER_SIZE, 1, file) != 1) {
            LGE "(id3ReadHeadersF) Error reading ID3v2 header.\nfread: ") ;
            return (errno) ;
        }

        if (strncmp (v2Buffer, "ID3", 3) == 0)		/* ID3v2 tag present? */
            *v2Header = v2Buffer ;
        else
            *v2Header = NULL ;

    }

/*******************************************************************************
    Read the ID3v1 tag.
*******************************************************************************/

    if (v1Tag == NULL)  return (0) ;

/* Position to and read the last 128 bytes in the file. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  id3ReadHeadersF: No fseek(3).
#else
    if (fseek (file, -ID3_V1_TRAILER_SIZE, SEEK_END)) {
        LGE "(id3ReadHeadersF) Error positioning to ID3v1 tag.\nfseek: ") ;
        return (errno) ;
    }
#endif

    if (fread (v1Buffer, ID3_V1_TRAILER_SIZE, 1, file) != 1) {
        LGE "(id3ReadHeadersF) Error reading ID3v1 tag.\nfread: ") ;
        return (errno) ;
    }

    if (strncmp (v1Buffer, "TAG", 3) == 0)		/* ID3v1 tag present? */
        *v1Tag = v1Buffer ;
    else
        *v1Tag = NULL ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ReadTags ()

    Read the ID3 Tags from a Named File.


Purpose:

    The id3ReadTags() function reads the ID3v1 and/or ID3v2 tags, if present,
    from a specified file.


    Invocation:

        status = id3ReadTags (fileName, &v1Tag, &v2Tag) ;

    where

        <fileName>	- I
            is the name of the file from which the ID3 tags will be read.
            Environment variables may be embedded in the file name.
        <v1Tag>		- O
            returns a handle for the ID3v1 tag.  The caller is responsible
            for id3TagDestroy()ing the tag after it is no longer needed.
            If the file has no iD3v1 tag, a NULL handle is returned.  If
            this argument is NULL, the ID3v1 tag is not read from the file.
        <v2Tag>		- O
            returns a handle for the ID3v2 tag.  The caller is responsible
            for id3TagDestroy()ing the tag after it is no longer needed.
            If the file has no iD3v2 tag, a NULL handle is returned.  If
            this argument is NULL, the ID3v2 tag is not read from the file.
        <status>	- O
            returns the status of getting the tags from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ReadTags (

#    if PROTOTYPES
        const  char  *fileName,
        Id3Tag  *v1Tag,
        Id3Tag  *v2Tag)
#    else
        fileName, v1Tag, v2Tag)

        char  *fileName ;
        Id3Tag  *v1Tag ;
        Id3Tag  *v2Tag ;
#    endif

{    /* Local variables. */
    char  *pathname ;
    FILE  *file ;



/* Open the file. */

    pathname = fnmBuild (FnmPath, fileName, NULL) ;
    if (pathname == NULL) {
        LGE "(id3ReadTags) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
        return (errno) ;
    }

    LGI "(id3ReadTags) File: \"%s\"\n", pathname) ;

    file = fopen (pathname, "rb") ;
    if (file == NULL) {
        LGE "(id3ReadTags) Error opening \"%s\".\nfopen: ", pathname) ;
        PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Get the ID3 header/tag from the file. */

    if (id3ReadTagsF (file, v1Tag, v2Tag)) {
        LGE "(id3ReadTags) Error getting ID3 header/tag from \"%s\".\nid3ReadTagsF: ", pathname) ;
        PUSH_ERRNO ;  fclose (file) ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

    fclose (file) ;

    free (pathname) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ReadTagsF ()

    Read the ID3v1 and ID3v2 Tags from a File.


Purpose:

    The id3ReadTagsF() function retrieves and decodes the ID3v1 and ID3v2 tags,
    if present, from a file.


    Invocation:

        status = id3ReadTagsF (fileName, &v1Tag, &v2Tag) ;

    where

        <file>		- I
            is the Unix FILE* handle for the previously opened file.
        <v1Tag>		- O
            returns a handle for the ID3v1 tag.  The caller is responsible
            for id3TagDestroy()ing the tag after it is no longer needed.
            If the file has no iD3v1 tag, a NULL handle is returned.  If
            this argument is NULL, the ID3v1 tag is not read from the file.
        <v2Tag>		- O
            returns a handle for the ID3v2 tag.  The caller is responsible
            for id3TagDestroy()ing the tag after it is no longer needed.
            If the file has no iD3v2 tag, a NULL handle is returned.  If
            this argument is NULL, the ID3v2 tag is not read from the file.
        <status>	- O
            returns the status of getting the tags from the file, zero if
            there were no errors and ERRNO otherwise.  (Not finding a tag
            in a file is not an error and simply results in the return of
            a NULL tag handle in the corresponding arguments.)

*******************************************************************************/


errno_t  id3ReadTagsF (

#    if PROTOTYPES
        FILE  *file,
        Id3Tag  *v1Tag,
        Id3Tag  *v2Tag)
#    else
        file, v1Tag, v2Tag)

        FILE  *file ;
        Id3Tag  *v1Tag ;
        Id3Tag  *v2Tag ;
#    endif

{

/* Read the ID3v2 tag at the beginning of the file. */

    if ((v2Tag != NULL) && id3ReadV2 (file, v2Tag)) {
        LGE "(id3ReadTagsF) Error reading ID3v2 tag.\nid3ReadV2: ") ;
        *v2Tag = NULL ;
        return (errno) ;
    }

/* Read the ID3v1 tag at the end of the file. */

    if ((v1Tag != NULL) && id3ReadV1 (file, v1Tag)) {
        LGE "(id3ReadTagsF) Error reading ID3v1 tag.\nid3ReadV1") ;
        PUSH_ERRNO ;
        if (v2Tag != NULL) {
            id3TagDestroy (*v2Tag) ;  *v2Tag = NULL ;
        }
        POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ReadV1 ()

    Read the ID3v1 Tag from an Open File.


Purpose:

    The id3ReadV1() function retrieves the ID3v1 tag, if any, from an open file.


    Invocation:

        status = id3ReadV1 (file, &tag) ;

    where

        <file>		- I
            is the Unix FILE* handle for the previously opened file.
        <tag>		- O
            returns a handle for the ID3v1 tag.  This handle is used in calls
            to the other ID3_UTIL functions and, after it is no longer needed,
            it should be id3TagDestroy()ed.  If the file has no ID3v1 tag,
            a NULL handle and an error status of success (0) is returned.
        <status>	- O
            returns the status of getting the tag from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ReadV1 (

#    if PROTOTYPES
        FILE  *file,
        Id3Tag  *tag)
#    else
        file, tag)

        FILE  *file ;
        Id3Tag  *tag ;
#    endif

{    /* Local variables. */
    char  buffer[ID3_V1_TRAILER_SIZE] ;



    *tag = NULL ;

/* Position to and read the last 128 bytes in the file. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  id3ReadV1: No fseek(3).
#else
    if (fseek (file, -ID3_V1_TRAILER_SIZE, SEEK_END)) {
        LGE "(id3ReadV1) Error positioning to ID3v1 tag.\nfseek: ") ;
        return (errno) ;
    }
#endif

    if (fread (buffer, ID3_V1_TRAILER_SIZE, 1, file) != 1) {
        LGE "(id3ReadF) Error reading ID3v1 tag.\nfread: ") ;
        return (errno) ;
    }

    if (I_DEFAULT_GUARD) {
        LGI "(id3ReadV1) Input buffer at %p:\n", (void *) buffer) ;
        meoDumpX (NULL, "    ", 0, buffer, ID3_V1_TRAILER_SIZE) ;
    }


/* Check that the trailer read from the file is a valid ID3 tag. */

    if (!id3IsTag (buffer, ID3_V1_TRAILER_SIZE)) {
        LGI "(id3ReadV1) No ID3v1 tag in file.\n") ;
        return (0) ;			/* Successful with NULL handle. */
    }

/* Decode the contents of the buffer. */

    if (id3TagDecodeV1 (buffer, tag)) {
        LGE "(id3ReadV1) Error decoding ID3v1 tag.\nid3TagDecodeV1: ") ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3ReadV2 ()

    Read the ID3v2 Tag from an Open File.


Purpose:

    The id3ReadV2() function retrieves the ID3v2 tag, if any, from an open file.


    Invocation:

        status = id3ReadV2 (file, &tag) ;

    where

        <file>		- I
            is the Unix FILE* handle for the previously opened file.
        <tag>		- O
            returns a handle for the ID3v2 tag.  This handle is used in calls
            to the other ID3_UTIL functions and, after it is no longer needed,
            it should be id3TagDestroy()ed.  If the file has no ID3v2 tag,
            a NULL handle and an error status of success (0) is returned.
        <status>	- O
            returns the status of getting the tag from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3ReadV2 (

#    if PROTOTYPES
        FILE  *file,
        Id3Tag  *tag)
#    else
        file, tag)

        FILE  *file ;
        Id3Tag  *tag ;
#    endif

{    /* Local variables. */
    char  *buffer, header[ID3_V2_HEADER_SIZE] ;
    size_t  length ;



    *tag = NULL ;

/* Position to and read the first 10 bytes in the file. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  id3ReadV2: No fseek(3).
#else
    if (fseek (file, 0, SEEK_SET)) {
        LGE "(id3ReadV2) Error positioning to ID3v2 tag.\nfseek: ") ;
        return (errno) ;
    }
#endif

    if (fread (header, ID3_V2_HEADER_SIZE, 1, file) != 1) {
        LGE "(id3ReadV2) Error reading ID3v2 header.\nfread: ") ;
        return (errno) ;
    }

/* Check that the header read from the file is for a valid ID3 tag.  If not,
   simply return a NULL tag. */

    if (!id3IsTag (header, ID3_V2_HEADER_SIZE)) {
        LGI "(id3ReadV2) No ID3v2 tag in file.\n") ;
        return (0) ;			/* Successful with NULL handle. */
    }

/* Read the full ID3v2 tag into memory. */

    length = ID3_V2_HEADER_SIZE + id3DecodeSSI (&header[6], 4) ;

    buffer = malloc (length) ;
    if (buffer == NULL) {
        LGE "(id3ReadV2) Error allocating %"PRIuSIZE_T"-byte buffer for ID3v2 tag.\nmalloc: ",
            length) ;
        return (errno) ;
    }

    memcpy (buffer, header, ID3_V2_HEADER_SIZE) ;

    if (length > ID3_V2_HEADER_SIZE) {
        if (fread (&buffer[ID3_V2_HEADER_SIZE],
                   length - ID3_V2_HEADER_SIZE, 1, file) != 1) {
            LGE "(id3ReadV2) Error reading %"PRIuSIZE_T"-byte body of ID3v2 tag.\nmfread: ",
                length - ID3_V2_HEADER_SIZE) ;
            PUSH_ERRNO ;  free (buffer) ;  POP_ERRNO ;
            return (errno) ;
        }
    }

/* Decode the contents of the buffer. */

    if (id3TagDecodeV2 (buffer, length, tag)) {
        LGE "(id3ReadV2) Error decoding ID3v2 tag.\nid3TagDecodeV2: ") ;
        PUSH_ERRNO ;  free (buffer) ;  POP_ERRNO ;
        return (errno) ;
    }

    (*tag)->v2Buffer = buffer ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3Strip ()

    Strip the ID3 Tag from a Named File.


Purpose:

    The id3Strip() function strips the ID3 tag from a specified file.


    Invocation:

        status = id3Strip (fileName, version) ;

    where

        <fileName>	- I
            is the name of the file to which the ID3 tag will be written.
            Environment variables may be embedded in the file name.
        <version>	- I
            specifies which ID3 tag (v1 or v2) to strip from the file.
        <status>	- O
            returns the status of stripping the tag from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3Strip (

#    if PROTOTYPES
        const  char  *fileName,
        Id3Version  version)
#    else
        fileName, version)

        char  *fileName ;
        Id3Version  version ;
#    endif

{    /* Local variables. */
    char  *pathname ;
    FILE  *file ;



/* Open the file for reading and writing. */

    pathname = fnmBuild (FnmPath, fileName, NULL) ;
    if (pathname == NULL) {
        LGE "(id3Strip) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
        return (errno) ;
    }

    file = fopen (pathname, "rb+") ;
    if (file == NULL) {
        LGE "(id3Strip) Error opening %s.\nfopen: ", pathname) ;
        PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Strip the ID3 tag from the file. */

    if (id3StripF (file, version)) {
        LGE "(id3Strip) Error stripping ID3 tag from %s.\nid3StripF: ", pathname) ;
        PUSH_ERRNO ;  fclose (file) ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

    fclose (file) ;

    free (pathname) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3StripF ()

    Strip the ID3 Tag from an Open File.


Purpose:

    The id3StripF() function strips the ID3 tag from an open file.


    Invocation:

        status = id3StripF (file, version) ;

    where

        <file>		- I
            is the Unix FILE* handle for the previously opened file.
            The file must be opened for reading and writing ("rb+").
        <version>	- I
            specifies which ID3 tag (v1 or v2) to strip from the file.
        <status>	- O
            returns the status of stripping the tag from the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3StripF (

#    if PROTOTYPES
        FILE  *file,
        Id3Version  version)
#    else
        file, version)

        FILE  *file ;
        Id3Version  version ;
#    endif

{    /* Local variables. */
#if defined(HAVE_FTRUNCATE) && !HAVE_FTRUNCATE
#    warning  id3StripF: No ftruncate(3).
#else
#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  id3StripF: No fseek(3).
#else
    char  *v1tag, *v2header ;
    size_t  tagLength, truncatedLength ;



/* Determine if the file has an ID3 tag. */

    if (id3ReadHeadersF (file, &v1tag, &v2header)) {
        LGE "(id3StripF) Error checking for ID3 tags.\nid3ReadHeadersF: ") ;
        return (errno) ;
    }

    if (((ID3V_VERSION (version) == 1) && (v1tag == NULL)) ||
        ((ID3V_VERSION (version) == 2) && (v2header == NULL))) {
        SET_ERRNO (EEXIST) ;
        LGI "(id3StripF) No ID3v%ld tag.\n", ID3V_VERSION (version)) ;
        return (0) ;
    }

    if (ID3V_VERSION (version) == 1)
        tagLength = ID3_V1_TRAILER_SIZE ;
    else
        tagLength = ID3_V2_HEADER_SIZE + id3DecodeSSI (&v2header[6], 4) ;


/*******************************************************************************
    ID3v2 tags are located at the beginning of the file.  To remove the tag,
    the subsequent data in the file must be moved down to the beginning of
    the file.
*******************************************************************************/

    if (ID3V_VERSION (version) == 2) {

        char  *buffer ;
        size_t  here, numBytesRead, there ;

#define  ID3_CLEAN_BLOCK_SIZE  (128*1024UL)
        buffer = malloc (ID3_CLEAN_BLOCK_SIZE) ;
        if (buffer == NULL) {
            LGE "(id3StripF) Error allocating %lu-byte transfer buffer.\nmalloc: ",
                ID3_CLEAN_BLOCK_SIZE) ;
            return (errno) ;
        }

/* Move the data in the file down to the beginning of the file. */

        there = 0 ;	/* Shift first block down to beginning of file. */

        for ( ; ; ) {

            here = there + tagLength ;

            if (fseek (file, (long) here, SEEK_SET))  break ;

            numBytesRead = fread (buffer, 1, ID3_CLEAN_BLOCK_SIZE, file) ;
            if (numBytesRead < 1)  break ;

            LGI "(id3StripF) Moving %"PRIuSIZE_T" bytes from %"PRIuSIZE_T" to %"PRIuSIZE_T".\n",
                numBytesRead, here, there) ;

            if (fseek (file, (long) there, SEEK_SET))  break ;

            if (fwrite (buffer, numBytesRead, 1, file) != 1) {
                LGE "(id3StripF) Error writing %"PRIuSIZE_T" bytes.\nfwrite: ",
                    numBytesRead) ;
                PUSH_ERRNO ;  free (buffer) ;  POP_ERRNO ;
                return (errno) ;
            }
			/* Shift next block down to follow previous block. */
            there += numBytesRead ;

        }

        free (buffer) ;

    }


/*******************************************************************************
    Truncate the empty space at the end of the file (if the data was moved down
    overwriting the ID3v2 tag) or the 128-byte ID3v1 tag (if that tag is being
    stripped).
*******************************************************************************/

    if (fseek (file, 0, SEEK_END)) {
        LGE "(id3StripF) Error positioning to end of file.\nfseek: ") ;
        return (errno) ;
    }

    truncatedLength = (size_t) ftell (file) - tagLength ;

    if (ftruncate (fileno (file), (long) truncatedLength)) {
        LGE "(id3StripF) Error truncating file to %"PRIuSIZE_T" bytes.\nftruncate: ",
            truncatedLength) ;
        return (errno) ;
    }

    LGI "(id3StripF) Stripped ID3v%ld tag.\n", ID3V_VERSION (version)) ;

#endif
#endif


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3Unsync ()

    Unsynchronize a Data Buffer.


Purpose:

    Function id3Unsync() "unsynchronizes" a buffer of ID3 data.  The new,
    unsynchronized data is stored in a larger, dynamically allocated buffer
    which is returned to the caller.

    In an MP3 audio stream, each audio frame begins with a sync code of
    11 consecutive 1 bits, specifically a byte with all 8 bits set and
    a second byte whose 3 most significant bits are all set.  ID3 data,
    as opposed to the audio data, may contain false MP3 sync codes.
    To eliminate these, the original ID3 data can be "unsynchronized"
    by inserting a 0x00 byte between two adjacent bytes in these cases:

        0xFFFx  =>  0xFF <0x00> 0xFx		(12 consecutive 1 bits)
        0xFFEx  =>  0xFF <0x00> 0xEx		(11 consecutive 1 bits)
        0xFF00  =>  0xFF <0x00> 0x00		(Special case)

    Attention must be paid to a non-unsynchronized data buffer ending
    in a 0xFF byte.  This can happen at the end of an ID3v2.3 tag or
    at the end of the last frame in an ID3v2.4 tag.  The two versions'
    ID3 standards recommend adding padding after the 0xFF byte or,
    for ID3v2.4 tags only, adding an ID3 footer.  Adding a 0x00 byte
    at the end of the last ID3v2.4 frame is allowed, but frowned upon,
    by the v2.4 standard.

    The ID3-recommended solutions seem excessive and id3Unsync()
    instead uses the frowned-upon approach, including the trailing
    unsynchronization 0x00-byte as part of the encoded data (i.e.,
    the ID3v2.3 tag or the ID3v2.4 last frame).  Doing so possibly
    wastes a byte if the character following the tag or frame is
    not in the 0xE0..0xFF range, but it simplifies the handling
    of this case and confines that handling to id3Unsync().
    (De-unsynchronization code in ID3_UTIL and disparate other,
    non-ID3_UTIL-based applications will see the 0xFF00 in the
    ID3_UTIL-generated unsynchronized data and correctly drop
    the zero-byte.)


    Invocation:

        newBuffer = id3Unsync (buffer, length, &numSync) ;

    where

        <buffer>	- I
            is the buffer of ID3 data.
        <length>	- I
            is the length in bytes of the buffer.
        <numSync>	- O
            returns the number of false sync codes converted to a (0xFF,0x00)
            pair; i.e., the length of the new buffer is <length> + <numSync>.
        <newBuffer>	- O
            returns the dynamically allocated buffer containing the
            unsyncrhonized data; NULL is returned in the event of an
            error.  The caller is responsible for free(3)ing the new
            buffer after it is no longer needed.

*******************************************************************************/


char  *id3Unsync (

#    if PROTOTYPES
        const  char  *buffer,
        size_t  length,
        size_t  *numSync)
#    else
        buffer, length, numSync)

        char  *buffer ;
        size_t  length ;
        size_t  *numSync ;
#    endif

{    /* Local variables */
    char  *newBuffer ;
    unsigned  char  current, last ;
    MemoryDx  mdx ;
    size_t  count, origLength ;



    if (numSync != NULL)  *numSync = 0 ;

    if (buffer == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3Unsync) NULL buffer: ") ;
        return (NULL) ;
    }

    if (mdxCreate (NULL, 0, MdxDynamic, &mdx)) {
        LGE "(id3Unsync) Error creating memory descriptor.\nmdxCreate: ") ;
        return (NULL) ;
    }

    origLength = length ;

    for (count = 0, last = 0x00 ;  length > 0 ;  buffer++, length--) {
        current = (unsigned char) *buffer & 0x0FF ;
        if ((last == 0x0FF) &&
            ((current >= 0x0E0) || (current == 0x00))) {
            if (mdxAddB (mdx, 0)) {
                LGE "(id3Unsync) Error inserting 0x00-byte.\nmdxAddB: ") ;
                PUSH_ERRNO ;  mdxDestroy (mdx) ;  POP_ERRNO ;
                return (NULL) ;
            }
            count++ ;
        }
        if (mdxAddB (mdx, (char) current)) {
            LGE "(id3Unsync) Error copying byte.\nmdxAddB: ") ;
            PUSH_ERRNO ;  mdxDestroy (mdx) ;  POP_ERRNO ;
            return (NULL) ;
        }
        last = current ;
    }

    if (last == 0x0FF) {
        if (mdxAddB (mdx, 0)) {
            LGE "(id3Unsync) Error inserting final 0x00-byte.\nmdxAddB: ") ;
            PUSH_ERRNO ;  mdxDestroy (mdx) ;  POP_ERRNO ;
            return (NULL) ;
        }
        count++ ;
    }

    mdxOwn (mdx) ;
    newBuffer = mdxRegionP (mdx, 0) ;
    mdxDestroy (mdx) ;

    if (numSync != NULL)  *numSync = count ;

    LGI "(id3UnSync) Length: %"PRIuSIZE_T"  #Sync: %"PRIuSIZE_T"\n",
        origLength, count) ;

    return (newBuffer) ;

}

/*!*****************************************************************************

Procedure:

    id3WriteTags ()

    Write ID3 Tags to a Named File.


Purpose:

    The id3WriteTags() function writes ID3v1 and/or ID3v2 tags to a specified
    file.  Existing tags in the file may be merged into the supplied tags
    before they are replaced.

    Merging Tags
    ------------

    The merging of tags is a bit complicated.  (The steps below are affected
    by the merge-tags argument, the presence and absence of the existing tags,
    and the presence and absence of the caller-supplied tags.  The code takes
    into account the different combinations and ensures the desired, resulting
    tags are written to the file.)

    First:

      - The existing version 2 and version 1 tags, if present, are read
        from the file.

    Next, merge the version 2 tag:

      - If there are both an existing v2 tag and a caller-supplied v2 tag,
        then merge them into a new v2 tag.  The tags are merged in the sense
        that the caller-supplied frames update the frames in the existing tag.
        (Or it can be viewed as existing tag frames filling in missing frames
        in the caller-supplied tag.)

      - If there isn't an existing v2 tag, use the caller-supplied tag as-is.

    And then merge the version 1 tag:

      - If there are both an existing v1 tag and a caller-supplied v1 tag,
        then merge them (as with the v2 tags) into a new v1 tag.

      - If there isn't an existing v1 tag, then merge the caller-supplied tag
        with the merged v2 tag.

    Temporary File for ID3v2 Tags
    -----------------------------

    A temporary file is used for writing ID3v2 tags:

        (1) Create an empty temporary file.
        (2) Write the tag to the temporary file.
        (3) Copy the post-ID3v2-tag contents in the audio file
            to the temporary file.
        (4) Close both files.
        (5) Delete the audio file.
        (6) Rename the temporary file to the audio file name.

    Given a full, audio-file pathname of:

        <audio_file_path>/<audio_file_name>.<ext>

    the default pathname for the temporary file will be:

        <audio_file_path>/<audio_file_name>.t31

    In other words, the temporary file is created in the same directory
    as the audio file, with the same name except for ".t31" replacing
    the audio file's extension (e.g., ".mp3").

    An alternate directory, file name, and/or extension can be used by
    specifying the <extension> function argument.  See "fnm_util.c" for
    more details, but a couple of examples might suffice.  For a file
    extension other than ".t31":

        extension =>  ".xyz"
               gives  <audio_file_path>/<audio_file_name>.t31

    To create the temporary file in a different directory:

        extension =>  "/tmp/"
               gives  /tmp/<audio_file_name>.t31

    To use a different extension in a different directory:

        extension =>  "/tmp/.xyz"
               gives  /tmp/<audio_file_name>.xyz

    (If the temporary file is created in a different directory, the
    rename step listed above will result in the temporary file being
    moved back to the original audio file's directory.)


    Invocation:

        status = id3WriteTags (fileName, v1Tag, v2Tag,
                               mergeTags, sortFrames, extension) ;

    where

        <fileName>	- I
            is the name of the file to which the ID3 tags will be written.
            Environment variables may be embedded in the file name.
        <v1Tag>		- I
            is the handle for the ID3 tag to be encoded and stored as
            an ID3v1 tag in the file.  This argument can be the same
            tag as the ID3v2 tag; fields too long for the ID3v1 tag
            are truncated when encoded into the file-format ID3v2 tag.
            If this argument is NULL, a new ID3v1 tag is not written
            to the file and the existing ID3v1 tag, if present, is
            left as is.
        <v2Tag>		- I
            is the handle for the ID3 tag to be encoded and stored as
            an ID3v2 tag in the file.  If this argument is NULL, a new
            ID3v2 tag is not written to the file and the existing ID3v2
            tag, if present, is left as is.
        <mergeTags>	- I
            specifies whether or not the caller-supplied tags are merged
            with the existing tags in the file.  See the details above.
        <sortFrames>	- I
            specifies whether or not to sort the frames in the final ID3v2
            tag before it is written to the file.  See id3TagSortFrames()
            in "id3_tag.c" for the details about the sort order.
        <extension>	- I
            specifies an alternate extension and/or other pathname
            components for the ID3v2 temporary file; see the note
            above.  If NULL, the extension defaults to ".t31"; i.e.,
            the temporary file will have the same, full pathname as
            the input file except with a ".t31" extension.
        <status>	- O
            returns the status of writing the tag(s) to the file,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3WriteTags (

#    if PROTOTYPES
        const  char  *fileName,
        Id3Tag  v1Tag,
        Id3Tag  v2Tag,
        bool  mergeTags,
        bool  sortFrames,
        const  char  *extension)
#    else
        fileName, v1Tag, v2Tag, mergeTags, sortFrames, extension)

        char  *fileName ;
        Id3Tag  v1Tag ;
        Id3Tag  v2Tag ;
        bool  mergeTags ;
        bool  sortFrames ;
        char  *extension ;
#    endif

{    /* Local variables. */
    char  *copyBuffer, *newPathname, *pathname, *v2buffer ;
    errno_t  status ;
    FILE  *file, *newFile ;
    Id3Tag  newV1, newV2, oldV1, oldV2 ;
    size_t  length, numBytesRead ;
    static  char  v1buffer[ID3_V1_TRAILER_SIZE] ;



/* Initialize items to be cleaned up upon return. */

    status = 0 ;

    pathname = newPathname = NULL ;
    file = newFile = NULL ;
    newV1 = newV2 = NULL ;
    oldV1 = oldV2 = NULL ;
    v2buffer = NULL ;
    copyBuffer = NULL ;

/* Open the file. */

    pathname = fnmBuild (FnmPath, fileName, NULL) ;
    if (pathname == NULL) {
        LGE "(id3WriteTags) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
        status = errno ;
        goto CleanUp ;
    }

    LGI "(id3WriteTags) File: \"%s\"\n", pathname) ;

    file = fopen (pathname, "r+b") ;
    if (file == NULL) {
        LGE "(id3WriteTags) Error opening \"%s\".\nfopen: ", pathname) ;
        status = errno ;
        goto CleanUp ;
    }

/* Read the existing ID3v1/v2 tags from the file. */

    if (id3ReadTagsF (file, &oldV1, &oldV2)) {
        LGE "(id3WriteTags) Error reading existing ID3 tags from \"%s\".\nid3ReadTagsF: ", pathname) ;
        status = errno ;
        goto CleanUp ;
    }

/*******************************************************************************
    Merge the contents of the existing tags with the new tags supplied
    by the caller.
*******************************************************************************/

    newV1 = newV2 = NULL ;

    if (mergeTags) {

        if ((v2Tag != NULL) && (oldV2 != NULL)) {
            newV2 = id3TagMerge (oldV2, v2Tag) ;
            if (newV2 == NULL) {
                LGE "(id3WriteTags) Error merging Id3v2 tags for \"%s\".\nid3TagMerge: ",
                    pathname) ;
                status = errno ;
                goto CleanUp ;
            }
        }

        if (v1Tag != NULL) {
            if (oldV1 == NULL) {
                newV1 = id3TagMerge ((newV2 == NULL) ? v2Tag : newV2, v1Tag) ;
            } else {
                newV1 = id3TagMerge (oldV1, v1Tag) ;
            }
            if (newV1 == NULL) {
                LGE "(id3WriteTags) Error merging Id3v1 tags for \"%s\".\nid3TagMerge: ",
                    pathname) ;
                status = errno ;
                goto CleanUp ;
            }
        }

    }

/*******************************************************************************
    Encode the ID3v1 tag into its file format and write it to the file.
*******************************************************************************/

    if (v1Tag != NULL) {

        if (id3TagEncodeV1 (mergeTags ? newV1 : v1Tag, v1buffer)) {
            LGE "(id3WriteTags) Error encoding ID3v1 tag for \"%s\".\nid3TagEncodeV1: ",
                pathname) ;
            status = errno ;
            goto CleanUp ;
        }

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#        warning  id3WriteTags: No fseek(3).
#else
        if (fseek (file,
                   (oldV1 == NULL) ? 0 : -ID3_V1_TRAILER_SIZE, SEEK_END)) {
            LGE "(id3WriteTags) Error positioning to ID3v1 tag in \"%s\".\nfseek: ",
                pathname) ;
            status = errno ;
            goto CleanUp ;
        }
#endif

        if (fwrite (v1buffer, ID3_V1_TRAILER_SIZE, 1, file) != 1) {
            LGE "(id3WriteTags) Error writing ID3v1 tag to \"%s\".\nfwrite: ",
                pathname) ;
            status = errno ;
            goto CleanUp ;
        }

        fflush (file) ;

    }

/*******************************************************************************
    Encode the ID3v2 tag into its file format and write it to a temporary file.
*******************************************************************************/

    if (v2Tag == NULL) {	/* Only write V1 tag to file? */
        status = 0 ;		/* Then clean up and return immediately. */
        goto CleanUp ;
    }

    if (newV2 == NULL) {	/* Copy caller-supplied tag for sorting? */
        newV2 = id3TagCopy (v2Tag) ;
        if (newV2 == NULL) {
            LGE "(id3WriteTags) Error copying Id3v2 tag for \"%s\".\nid3TagCopy: ",
                pathname) ;
            status = errno ;
            goto CleanUp ;
        }
    }

    if (sortFrames && id3TagSortFrames (newV2)) {
        LGE "(id3WriteTags) Error sorting Id3v2 frames for \"%s\".\nid3TagSortFrames: ",
            pathname) ;
         status = errno ;
         goto CleanUp ;
    }

    if (id3TagEncodeV2 (newV2, &v2buffer, &length)) {
        LGE "(id3WriteTags) Error encoding ID3v2 tag for \"%s\".\nid3TagEncodeV2: ",
            pathname) ;
        status = errno ;
        goto CleanUp ;
    }

    newPathname = fnmBuild (FnmPath,
                            (extension == NULL) ? ".t31" : extension,
                            pathname, NULL) ;
    if (newPathname == NULL) {
        LGE "(id3WriteTags) Error parsing \"%s\",\"%s\".\nfnmBuild: ",
            (extension == NULL) ? ".t31" : extension, pathname) ;
        status = errno ;
        goto CleanUp ;
    }

    newFile = fopen (newPathname, "wb") ;
    if (newFile == NULL) {
        LGE "(id3WriteTags) Error opening \"%s\".\nfopen: ", newPathname) ;
        status = errno ;
        goto CleanUp ;
    }

    if (fwrite (v2buffer, length, 1, newFile) != 1) {
        LGE "(id3WriteTgas) Error writing ID3v2 tag to \"%s\".\nfwrite: ",
            newPathname) ;
        status = errno ;
        goto CleanUp ;
    }

/*******************************************************************************
    Append the post-tag contents of the original audio file to the ID3v2 tag
    in the temporary file.
*******************************************************************************/

/* Position past the ID3v2 tag in the audio file. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  id3WriteTags: No fseek(3).
#else
    if (fseek (file,
               (oldV2 == NULL) ? 0 : (ID3_V2_HEADER_SIZE + oldV2->v2Size),
               SEEK_SET)) {
        LGE "(id3WriteTags) Error positioning past ID3v2 tag in \"%s\".\nfseek: ",
            pathname) ;
        status = errno ;
        goto CleanUp ;
    }
#endif

#define  ID3_COPY_BLOCK_SIZE  (128*1024UL)
    copyBuffer = malloc (ID3_COPY_BLOCK_SIZE) ;
    if (copyBuffer == NULL) {
        LGE "(id3WriteTags) Error allocating %"PRIuSIZE_T"-byte transfer buffer.\nmalloc: ",
            ID3_COPY_BLOCK_SIZE) ;
        status = errno ;
        goto CleanUp ;
    }

/* Copy the audio data, block by block, from the original file to the
   new file. */

    for ( ; ; ) {

        numBytesRead = fread (copyBuffer, 1, ID3_COPY_BLOCK_SIZE, file) ;
        if (numBytesRead < 1)  break ;

        LGI "(id3WriteTags) Copying %"PRIuSIZE_T" bytes ...\n", numBytesRead) ;

        if (fwrite (copyBuffer, numBytesRead, 1, newFile) != 1) {
            LGE "(id3WriteTags) Error writing %"PRIuSIZE_T" bytes to \"%s\".\nfwrite: ",
                numBytesRead, newPathname) ;
            status = errno ;
            goto CleanUp ;
        }

    }

/*******************************************************************************
    Finally, replace the old file with the new file.
*******************************************************************************/

/* Close the files.  Check for errors to prevent undesirable-in-case-of-error
  deletion of the old file. */

    if (fclose (file)) {
        LGE "(id3WriteTags) Error closing \"%s\".\nfclose: ", pathname) ;
        file = NULL ;
        status = errno ;
        goto CleanUp ;
    }
    file = NULL ;

    if (fclose (newFile)) {
        LGE "(id3WriteTags) Error closing \"%s\".\nfclose: ", newPathname) ;
        newFile = NULL ;
        status = errno ;
        goto CleanUp ;
    }
    newFile = NULL ;

/* Delete the old file and rename the new file. */

    if (remove (pathname)) {
        LGE "(id3WriteTags) Error deleting \"%s\".\nremove: ", pathname) ;
        status = errno ;
        goto CleanUp ;
    }

    if (rename (newPathname, pathname)) {
        LGE "(id3WriteTags) Error renaming \"%s\".\nremove: ", newPathname) ;
        status = errno ;
        goto CleanUp ;
    }

    status = 0 ;	/* Success! */
    goto CleanUp ;

/*******************************************************************************
    Clean up temporary items before returning.
*******************************************************************************/

CleanUp:

    if (pathname != NULL)  free (pathname) ;
    if (newPathname != NULL)  free (newPathname) ;
    if (file != NULL)  fclose (file) ;
    if (newFile != NULL)  fclose (newFile) ;
    if (oldV1 != NULL)  id3TagDestroy (oldV1) ;
    if (oldV2 != NULL)  id3TagDestroy (oldV2) ;
    if (newV1 != NULL)  id3TagDestroy (newV1) ;
    if (newV2 != NULL)  id3TagDestroy (newV2) ;
    if (v2buffer != NULL)  free (v2buffer) ;
    if (copyBuffer != NULL)  free (copyBuffer) ;

    return (status) ;

}

/*!*****************************************************************************

Procedure:

    id3GetAlbum ()

    Get an ID3 Tag's Album Field.


Purpose:

    Function id3GetAlbum() gets an ID3 tag's album field.


    Invocation:

        album = id3GetAlbum (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <album>		- O
            returns the album name from the tag; NULL is returned if there
            is no album field or in the event of an error.  The returned
            string is stored internally in the tag and should be used or
            duplicated before updating the field in the tag and before
            deleting the tag.

*******************************************************************************/


const  char  *id3GetAlbum (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    const  char  *album ;



    album = id3GetText (tag, ID3_ALBUM, NULL) ;
    if (album == NULL)  return (NULL) ;

/* If the ID3 tag is version 2 or greater, then return the full name. */

    if (tag->version >= ID3V_V2)  return (album) ;

/* If it is an ID3v1 tag, then create a truncated copy of the name. */

    if (tag->album == NULL) {
        tag->album = strndup (album, MAX_ALBUM_LENGTH) ;
        if (tag->album == NULL) {
            LGE "(id3GetAlbum) Error duplicating ID3v1 string: \"%s\"\nstrndup: ",
                album) ;
            return (NULL) ;
        }
    }

    return (tag->album) ;

}

/*!*****************************************************************************

Procedure:

    id3GetArtist ()

    Get an ID3 Tag's Artist Field.


Purpose:

    Function id3GetArtist() gets an ID3 tag's artist field.


    Invocation:

        artist = id3GetArtist (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <artist>		- O
            returns the artist name from the tag; NULL is returned if there
            is no artist field or in the event of an error.  The returned
            string is stored internally in the tag and should be used or
            duplicated before updating the field in the tag and before
            deleting the tag.

*******************************************************************************/


const  char  *id3GetArtist (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    const  char  *artist ;



    artist = id3GetText (tag, ID3_ARTIST, NULL) ;
    if (artist == NULL)  return (NULL) ;

/* If the ID3 tag is version 2 or greater, then return the full name. */

    if (tag->version >= ID3V_V2)  return (artist) ;

/* If it is an ID3v1 tag, then create a truncated copy of the name. */

    if (tag->artist == NULL) {
        tag->artist = strndup (artist, MAX_ARTIST_LENGTH) ;
        if (tag->artist == NULL) {
            LGE "(id3GetArtist) Error duplicating ID3v1 string: \"%s\"\nstrndup: ",
                artist) ;
            return (NULL) ;
        }
    }

    return (tag->artist) ;

}

/*!*****************************************************************************

Procedure:

    id3GetComment ()

    Get an ID3 Tag's Comment Field.


Purpose:

    Function id3GetComment() gets an ID3 tag's comment field.


    Invocation:

        comment = id3GetComment (tag, language, description) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <language>	- I
            is the 3-character code for the language of the comment;
            if this argument is NULL, the language defaults to "eng".
        <description>	- I
            is a short description of the comment; if this argument is NULL,
            the description in the tag will be "" (a zero-length string).
        <comment>	- O
            returns the comment from the tag; NULL is returned if there
            is no comment field or in the event of an error.  The returned
            string is stored internally in the tag and should be used or
            duplicated before updating the field in the tag and before
            deleting the tag.

*******************************************************************************/


const  char  *id3GetComment (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *language,
        const  char  *description)
#    else
        tag, language, description)

        Id3Tag  tag ;
        char  *language ;
        char  *description ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;

    frame = id3TagFindFrame (tag, ID3_COMMENT, language, description) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

/* Get the comment text. */

    if (frame->fdata.comm.text == NULL) {
        SET_ERRNO (EEXIST) ;
        LGE "(id3GetComment) NULL field value: ") ;
        return (NULL) ;
    }

/* If the ID3 tag is version 2 or greater, then return the full comment. */

    if (tag->version >= ID3V_V2)  return (frame->fdata.comm.text) ;

/* If it is an ID3v1 tag, then create a truncated copy of the comment. */

    if (tag->comment == NULL) {
        tag->comment = strndup (frame->fdata.comm.text, MAX_COMMENT_LENGTH) ;
        if (tag->comment == NULL) {
            LGE "(id3GetComment) Error duplicating ID3v1 string: \"%s\"\nstrndup: ",
                frame->fdata.comm.text) ;
            return (NULL) ;
        }
    }

    return (tag->comment) ;

}

/*!*****************************************************************************

Procedure:

    id3GetCover ()

    Get an ID3 Tag's Cover Picture.


Purpose:

    Function id3GetCover() gets an ID3 tag's cover picture.


    Invocation:

        image = id3GetCover (tag, pictureType, description,
                             &length, &mimeType) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <pictureType>	- I
            is the ID3 picture type of the desired cover; see "id3_util.h".
        <description>	- I
            is a short description of the desired cover; if NULL, the default
            is "" (a zero-length string).  ID3 allows multiple images of the
            same picture type, each with different descriptions.  Therefore,
            this argument must match the description of the desired cover in
            the tag.
        <length>	- O
            returns the length in bytes of the image data.
        <mimeType>	- O
            returns the MIME type of the image; e.g., "image/jpeg" and
            "image/png".  This argument can be NULL if the MIME type
            is not needed.
        <image>		- O
            returns a pointer to the image data.  The data is stored
            internally in the tag, so it should be used or duplicated
            before the tag is destroyed or the cover is changed via
            id3SetCover().  NULL is returned if no matching cover is
            found or in the event of an error.

*******************************************************************************/


const  char  *id3GetCover (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3PictureType  pictureType,
        const  char  *description,
        size_t  *length,
        const  char  **mimeType)
#    else
        tag, pictureType, description, length, mimeType)

        Id3Tag  tag ;
        Id3PictureType  pictureType ;
        char  **description ;
        size_t  *length ;
        char  **mimeType ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;

    frame = id3TagFindFrame (tag, ID3_ATTACHEDPIC, pictureType, description) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

    if (length != NULL)  *length = frame->fdata.apic.length ;
    if (mimeType != NULL)  *mimeType = frame->fdata.apic.mimeType ;

    return (frame->fdata.apic.image) ;

}

/*!*****************************************************************************

Procedure:

    id3GetGenre ()

    Get an ID3 Tag's Genre Field.


Purpose:

    Function id3GetGenre() gets an ID3 tag's genre field.


    Invocation:

        genre = id3GetGenre (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <genre>		- O
            returns the genre name from the tag; NULL is returned if there
            is no genre field or in the event of an error.  The returned
            string is stored internally in the tag and should be used or
            duplicated before updating the field in the tag and before
            deleting the tag.

*******************************************************************************/


const  char  *id3GetGenre (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    const  char  *genre ;



/* Get the string value of the tag's genre frame. */

    genre = id3GetText (tag, ID3_GENRE, NULL) ;
    if (genre == NULL)  return (NULL) ;

/* Update the cached, ID3v1 integer value in the tag. */

    if (tag->genre <= 0)  tag->genre = id3ToGenre (genre) ;

    return (genre) ;

}

/*!*****************************************************************************

Procedure:

    id3GetObject ()

    Get a General Encapsulated Object from an ID3 Tag.


Purpose:

    Function id3GetObject() gets a general encapsulated object, identified
    by its description, from an ID3 tag.


    Invocation:

        object = id3GetObject (tag, description,
                               &length, &mimeType, &fileName) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <description>	- I
            is the description of the desired object; if NULL, the default
            is "" (a zero-length string).  ID3 allows multiple general
            objects, each with different descriptions.  Therefore, this
            argument must match the description of the desired object
            in the tag.
        <length>	- O
            returns the length in bytes of the object data.
        <mimeType>	- O
            returns the MIME type of the object; e.g., "audio/midi".
            This argument can be NULL if the MIME type is not needed.
        <object>	- O
            returns a pointer to the object data.  The data is stored
            internally in the tag, so it should be used or duplicated
            before the tag is destroyed or the object is changed via
            id3SetObject().  NULL is returned if no matching object
            is found or in the event of an error.

*******************************************************************************/


const  char  *id3GetObject (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *description,
        size_t  *length,
        const  char  **mimeType,
        const  char  **fileName)
#    else
        tag, description, length, mimeType, fileName)

        Id3Tag  tag ;
        char  **description ;
        size_t  *length ;
        char  **mimeType ;
        char  **fileName ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;

    frame = id3TagFindFrame (tag, ID3_GENERALOBJ, description) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

    if (length != NULL)  *length = frame->fdata.geob.length ;
    if (mimeType != NULL)  *mimeType = frame->fdata.geob.mimeType ;
    if (fileName != NULL)  *fileName = frame->fdata.geob.fileName ;

    return (frame->fdata.geob.object) ;

}

/*!*****************************************************************************

Procedure:

    id3GetPlayCount ()

    Get an ID3 Tag's Play Count or Popularimenter Field.


Purpose:

    Function id3GetPlayCount() gets an ID3 tag's play count from
    a Play Counter (PCNT) or Popularimeter (POPM) frame.


    Invocation:

        count = id3GetPlayCount (tag, email, &rating) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <email>		- I
            specifies the email address in a Popularimeter (POPM) frame.
            If this argument is NULL, the play count is retrieved from a
            PCNT frame; otherwise the play count is retrieved from the
            POPM frame with a matching email address.
        <rating>	- O
            returns the rating from the POPM frame; this argument can be
            NULL if the caller is not interested in the rating.  If the
            EMAIL argument is NULL (i.e., get the PCNT counter), this
            RATING argument is ignored.
        <count>		- O
            returns the play counter from the tag.  Zero is returned
            if no matching frame is found.

*******************************************************************************/


size_t  id3GetPlayCount (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *email,
        unsigned  int  *rating)
#    else
        tag, email, rating)

        Id3Tag  tag ;
        char  *email ;
        unsigned  int  *rating ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (0) ;

    if (email == NULL)					/* Seeking PCNT? */
        frame = id3TagFindFrame (tag, ID3_PLAYCOUNT) ;
    else						/* Seeking POPM. */
        frame = id3TagFindFrame (tag, ID3_POPULARIMETER, email) ;

    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (0) ;
    }

/* Get the rating (POPM frame only). */

    if ((email != NULL) && (rating != NULL)) {
        *rating = frame->fdata.play.rating ;
    }

/* Get the counter itself. */

    return (frame->fdata.play.counter) ;

}

/*!*****************************************************************************

Procedure:

    id3GetPrivate ()

    Get a Private Data Field from an ID3 Tag.


Purpose:

    Function id3GetPrivate() gets the data from a Private (PRIV) frame,
    identified by its owner ID, in an ID3 tag.


    Invocation:

        data = id3GetPrivate (tag, ownerID, &length) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <ownerID>	- I
            is the owner ID of the desired private data; if NULL, the
            default is "" (a zero-length string).  ID3 allows multiple
            PRIV frames, each with different owner IDs.  Therefore,
            this argument must match the owner ID of the desired PRIV
            frame in the tag.
        <length>	- O
            returns the length in bytes of the private data.
        <data>		- O
            returns a pointer to the PRIV frame's data.  The data is stored
            internally in the tag, so it should be used or duplicated before
            the tag is destroyed or the private data is changed via
            id3SetPrivate().  NULL is returned if no matching PRIV frame
            is found or in the event of an error.

*******************************************************************************/


const  char  *id3GetPrivate (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *ownerID,
        size_t  *length)
#    else
        tag, ownerID, length)

        Id3Tag  tag ;
        char  *ownerID ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if ((tag == NULL) || (ownerID == NULL))  return (NULL) ;

    frame = id3TagFindFrame (tag, ID3_PRIVATE, ownerID) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

    if (length != NULL)  *length = frame->fdata.priv.length ;

    return (frame->fdata.priv.data) ;

}

/*!*****************************************************************************

Procedure:

    id3GetRegistration ()

    Get an Encryption/Group-ID Registration Field from an ID3 Tag.


Purpose:

    Function id3GetRegistration() gets encryption method and group ID
    registrations from ENCR and GRID frames, respectively, in an ID3 tag.


    Invocation:

        ownerID = id3GetRegistration (tag, type, symbol, &data, &length) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            is the type of frame being queried, ID3_ENCRYPTREG for an
            encryption method registration (ENCR) and ID3_GROUPIDREG
            for a group ID registration (GRID).
        <symbol>	- I
            is the encryption method or group ID, both 0..255.
        <data>		- O
            returns a pointer to the encryption- or group-dependent
            data buffer.  The data is stored internally in the tag,
            so it should be used or duplicated before the tag is
            destroyed or the data is changed via id3SetRegistration().
        <length>	- O
            returns the length in bytes of the data buffer.
        <ownerID>	- I
            returns the owner ID from the registration.  As with the
            data buffer, the owner ID should be used or duplicated
            before the tag is destroyed or changed.  NULL is returned
            if no matching frame is found or in the event of an error.

*******************************************************************************/


const  char  *id3GetRegistration (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        unsigned  int  symbol,
        const  char  **data,
        size_t  *length)
#    else
        tag, type, symbol, data, length)

        Id3Tag  tag ;
        Id3FrameType  type ;
        unsigned  int  symbol ;
        char  **data ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;
    if ((type != ID3_ENCRYPTREG) && (type != ID3_GROUPIDREG))  return (NULL) ;

    frame = id3TagFindFrame (tag, type, ID3_PRIVATE, symbol) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

    if (data != NULL)  *data = frame->fdata.regi.data ;
    if (length != NULL)  *length = frame->fdata.regi.length ;

    return (frame->fdata.regi.ownerID) ;

}

/*!*****************************************************************************

Procedure:

    id3GetSong ()

    Get an ID3 Tag's Song Field.


Purpose:

    Function id3GetSong() gets an ID3 tag's song title.


    Invocation:

        song = id3GetSong (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <song>		- O
            returns the song title from the tag; NULL is returned if there
            is no song field or in the event of an error.  The returned
            string is stored internally in the tag and should be used or
            duplicated before updating the field in the tag and before
            deleting the tag.

*******************************************************************************/


const  char  *id3GetSong (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    const  char  *song ;



    song = id3GetText (tag, ID3_TITLE, NULL) ;
    if (song == NULL)  return (NULL) ;

/* If the ID3 tag is version 2 or greater, then return the full title. */

    if (tag->version >= ID3V_V2)  return (song) ;

/* If it is an ID3v1 tag, then create a truncated copy of the title. */

    if (tag->song == NULL) {
        tag->song = strndup (song, MAX_SONG_LENGTH) ;
        if (tag->song == NULL) {
            LGE "(id3GetSong) Error duplicating ID3v1 string: \"%s\"\nstrndup: ",
                song) ;
            return (NULL) ;
        }
    }

    return (tag->song) ;

}

/*!*****************************************************************************

Procedure:

    id3GetText ()

    Get a Text Information Field from an ID3 Tag.


Purpose:

    Function id3GetText() is a general-purpose function for retrieving
    arbitrary text information frame types from an ID3 tag.  The caller
    specifies which frame type to get and, in the case of a user-defined
    text information frame, the description of the text.


    Invocation:

        text = id3GetText (tag, type, description) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            specifies the text information frame type (one of the
            Id3MarshalText or Id3MarshalUText types in "id3_util.h/.c").
        <description>	- I
            is the description of the text field if the frame type is
            ID3_TXXX_GENERIC; this argument is ignored for the other
            text information frame types.  An existing ID3_TXXX_GENERIC
            frame matching this description, if any, will be retrieved
            and its text value returned.
        <text>		- O
            returns the text string.  The string is stored internally
            in the tag/frame and it should be used or duplicated before
            destroying the tag or frame.  NULL is returned in the event
            of an error (including an absence of the desired frame type).

*******************************************************************************/


const  char  *id3GetText (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        const  char  *description)
#    else
        tag, type, description)

        Id3Tag  tag ;
        Id3FrameType  type ;
        const  char  *description ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;

/* Locate the frame in question. */

    frame = id3TagFindFrame (tag, type, description) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

/* Verify that the frame type is a text information frame type by examining
   the type's marshaling method. */

    if ((frame->info->marshalType != Id3MarshalText) &&
        (frame->info->marshalType != Id3MarshalUText)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3GetText) Not a text information frame type: %s (%d)",
            frame->info->name, (int) type) ;
        return (NULL) ;
    }

/* Return the text string. */

    if (frame->fdata.text.text == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3GetText) NULL field value: ") ;
        return (NULL) ;
    }

    return (frame->fdata.text.text) ;

}

/*!*****************************************************************************

Procedure:

    id3GetTrack ()

    Get an ID3 Tag's Track Field.


Purpose:

    Function id3GetTrack() gets an ID3 tag's track number.


    Invocation:

        track = id3GetTrack (tag, &total) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <total>		- O
            returns the total number of tracks, if specified in the tag.
            If this argument is NULL, don't return the total.
        <track>		- O
            returns the track number from the tag; -1 is returned if there
            is no track field or in the event of an error.

*******************************************************************************/


long  id3GetTrack (

#    if PROTOTYPES
        Id3Tag  tag,
        long  *total)
#    else
        tag, total)

        Id3Tag  tag ;
        long  *total ;
#    endif

{    /* Local variables. */
    char  *s ;
    Id3Frame  frame ;
    long  track ;



    if (tag == NULL)  return (-1) ;

    frame = id3TagFindFrame (tag, ID3_TRACK) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (-1) ;
    }

    if (frame->fdata.text.text == NULL) {
        SET_ERRNO (EEXIST) ;
        LGE "(id3GetTrack) NULL field value: ") ;
        return (-1) ;
    }

    track = atol (frame->fdata.text.text) ;

    if (total != NULL) {
        s = strchr (frame->fdata.text.text, '/') ;	/* "<track>/<total>" */
        if (s == NULL)
            *total = 0 ;
        else
            *total = atol (++s) ;
    }

/* Update the cached ID3v1 value in the tag. */

    if (tag->track <= 0)  tag->track = track ;

    return (track) ;

}

/*!*****************************************************************************

Procedure:

    id3GetUFID ()

    Get a Unique File ID from an ID3 Tag.


Purpose:

    Function id3GetUFID() gets a unique file ID (UFID), identified by its
    owner ID, from an ID3 tag.


    Invocation:

        fileID = id3GetUFID (tag, ownerID, &length) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <ownerID>	- I
            is the owner ID of the desired UFID; if NULL, the default
            is "" (a zero-length string).  ID3 allows multiple UFIDs,
            each with different owner IDs.  Therefore, this argument
            must match the owner ID of the desired UFID in the tag.
        <length>	- O
            returns the length in bytes of the UFID's file ID.
        <fileID>	- O
            returns a pointer to the UFID's file ID.  The ID is stored
            internally in the tag, so it should be used or duplicated
            before the tag is destroyed or the UFID is changed via
            id3SetUFID().  NULL is returned if no matching UFID is
            found or in the event of an error.

*******************************************************************************/


const  char  *id3GetUFID (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *ownerID,
        size_t  *length)
#    else
        tag, ownerID, length)

        Id3Tag  tag ;
        char  *ownerID ;
        size_t  *length ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if ((tag == NULL) || (ownerID == NULL))  return (NULL) ;

    frame = id3TagFindFrame (tag, ID3_UNIQUEFILEID, ownerID) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

    if (length != NULL)  *length = frame->fdata.ufid.length ;

    return (frame->fdata.ufid.fileID) ;

}

/*!*****************************************************************************

Procedure:

    id3GetUnsyncLyrics ()

    Get an ID3 Tag's Unsynchronized Lyrics.


Purpose:

    Function id3GetUnsyncLyrics() gets an ID3 tag's unsynchronized lyrics.


    Invocation:

        lyrics = id3GetUnsyncLyrics (tag, language, description) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <language>	- I
            is the 3-character code for the language of the lyrics;
            if this argument is NULL, the language defaults to "eng".
        <description>	- I
            is a description of the lyrics; if this argument is NULL,
            the description in the tag will be "" (a zero-length string).
        <lyrics>	- O
            returns the unsynchronized lyrics from the tag; NULL is returned
            if there is no lyrics frame or in the event of an error.  The
            returned string is stored internally in the tag and should be
            used or duplicated before updating the lyrics in the tag and
            before deleting the tag.

*******************************************************************************/


const  char  *id3GetUnsyncLyrics (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *language,
        const  char  *description)
#    else
        tag, language, description)

        Id3Tag  tag ;
        char  *language ;
        char  *description ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;

    frame = id3TagFindFrame (tag, ID3_UNSYNCEDLYRICS, language, description) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

/* Get the lyrics. */

    if (frame->fdata.uslt.lyrics == NULL) {
        SET_ERRNO (EEXIST) ;
        LGE "(id3GetUnsyncLyrics) NULL field value: ") ;
        return (NULL) ;
    }

    return (frame->fdata.uslt.lyrics) ;

}

/*!*****************************************************************************

Procedure:

    id3GetURL ()

    Get a URL Link from an ID3 Tag.


Purpose:

    Function id3GetURL() is a general-purpose function for retrieving
    arbitrary URL links from an ID3 tag.  The caller specifies which
    URL link frame type to get and, in the case of a user-defined
    URL link frame, the description of the link.


    Invocation:

        url = id3GetURL (tag, type, description) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            specifies the URL link frame type (one of the ID3_WWW... types
            in "id3_util.h").
        <description>	- I
            is the description of the URL link if the frame type is
            ID3_WXXX_GENERIC; this argument is ignored for the other
            URL link frame types.  An existing ID3_WXXX_GENERIC frame
            matching this description, if any, will be retrieved and
            its URL returned.
        <url>		- O
            returns the URL.  The URL string is stored internally in the
            tag/frame and it should be used or duplicated before destroying
            the tag or frame.  NULL is returned in the event of an error
            (including an absence of the desired frame type).

*******************************************************************************/


const  char  *id3GetURL (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        const  char  *description)
#    else
        tag, type, description)

        Id3Tag  tag ;
        Id3FrameType  type ;
        const  char  *description ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL)  return (NULL) ;

/* Locate the frame in question. */

    frame = id3TagFindFrame (tag, type, description) ;
    if (frame == NULL) {
        SET_ERRNO (EEXIST) ;
        return (NULL) ;
    }

/* Verify that the frame type is a URL link frame type by examining the
   type's marshaling method. */

    if ((frame->info->marshalType != Id3MarshalURL) &&
        (frame->info->marshalType != Id3MarshalUURL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3GetURL) Not a URL frame type: %s (%d)",
            frame->info->name, (int) type) ;
        return (NULL) ;
    }

/* Return the URL string. */

    if (frame->fdata.wurl.url == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3GetURL) NULL field value: ") ;
        return (NULL) ;
    }

    return (frame->fdata.wurl.url) ;

}

/*!*****************************************************************************

Procedure:

    id3GetVersion ()

    Get an ID3 Tag's Version.


Purpose:

    Function id3GetVersion() returns the version (including the major and
    revision numbers) of an ID3 tag.


    Invocation:

        version = id3GetVersion (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <version>	- O
            returns ID3 tag's version; see the Id3Version type definition
            in "id3_util.h".

*******************************************************************************/


Id3Version  id3GetVersion (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3GetVersion) NULL tag handle: ") ;
        return (0L) ;
    }

    return (tag->version) ;

}

/*!*****************************************************************************

Procedure:

    id3GetYear ()

    Get an ID3 Tag's Year Field.


Purpose:

    Function id3GetYear() gets an ID3 tag's year.


    Invocation:

        year = id3GetYear (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <year>		- O
            returns the year from the tag; -1 is returned if there
            is no year field or in the event of an error.

*******************************************************************************/


long  id3GetYear (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{    /* Local variables. */
    const  char  *s ;
    long  year ;



/* Get the text string from the year frame and convert it to an integer. */

    s = id3GetText (tag, ID3_YEAR, NULL) ;
    if (s == NULL)  return (-1) ;

    year = atol (s) ;

/* Update the cached ID3v1 value in the tag. */

    if (tag->year <= 0)  tag->year = year ;

    return (year) ;

}

/*!*****************************************************************************

Procedure:

    id3SetAlbum ()

    Set an ID3 Tag's Album Field.


Purpose:

    Function id3SetAlbum() sets an ID3 tag's album field.


    Invocation:

        status = id3SetAlbum (tag, album) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <album>		- I
            specifies the album name to be set in the ID3 tag.  (Upon
            encoding into an ID3v1 tag, the name will be truncated to
            30 characters.)  To clear the field, specify a NULL name.
        <status>	- O
            returns the status of setting the field, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetAlbum (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *album)
#    else
        tag, album)

        Id3Tag  tag ;
        char  *album ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetAlbum) NULL tag handle: ") ;
        return (errno) ;
    }

/* Add a new frame or replace the existing frame for the album name. */

    if (id3SetText (tag, ID3_ALBUM, album, NULL)) {
        LGE "(id3SetAlbum) Error setting text frame for \"%s\".\nid3SetText: ",
            (album == NULL) ? "<null>" : album) ;
        return (errno) ;
    }

/* Delete the cached, truncated ID3v1 string for id3GetAlbum(). */

    if (tag->album != NULL) {
        free (tag->album) ;
        tag->album = NULL ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetArtist ()

    Set an ID3 Tag's Artist Field.


Purpose:

    Function id3SetArtist() sets an ID3 tag's artist field.


    Invocation:

        status = id3SetArtist (tag, artist) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <artist>		- I
            specifies the artist name to be set in the ID3 tag.  (Upon
            encoding into an ID3v1 tag, the name will be truncated to
            30 characters.)  To clear the field, specify a NULL name.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetArtist (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *artist)
#    else
        tag, artist)

        Id3Tag  tag ;
        char  *artist ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetArtist) NULL tag handle: ") ;
        return (errno) ;
    }

/* Add a new frame or replace the existing frame for the artist name. */

    if (id3SetText (tag, ID3_ARTIST, artist, NULL)) {
        LGE "(id3SetArtist) Error setting text frame for \"%s\".\nid3SetText: ",
            (artist == NULL) ? "<null>" : artist) ;
        return (errno) ;
    }

/* Delete the cached, truncated ID3v1 string for id3GetArtist(). */

    if (tag->artist != NULL) {
        free (tag->artist) ;
        tag->artist = NULL ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetComment ()

    Set an ID3 Tag's Comment Field.


Purpose:

    Function id3SetComment() sets an ID3 tag's comment field.


    Invocation:

        status = id3SetComment (tag, delete, language, description, comment) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <delete>	- I
            specifies if the target frame is to be deleted.  If true, then
            the COMM frame with the matching language and description is
            marked for deletion.  If false, then the value of a new or
            existing frame is set.
        <language>	- I
            is the 3-character code for the language of the comment;
            if this argument is NULL, the language defaults to "eng".
        <description>	- I
            is a short description of the comment; if this argument is NULL,
            the description in the tag will be "" (a zero-length string).
        <comment>		- I
            specifies the comment to be set in the ID3 tag.  (Upon
            encoding into an ID3v1 tag, the comment will be truncated
            to 28 characters.)  To clear the field, specify a NULL name.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetComment (

#    if PROTOTYPES
        Id3Tag  tag,
        bool  delete,
        const  char  *language,
        const  char  *description,
        const  char  *comment)
#    else
        tag, delete, language, description, comment)

        Id3Tag  tag ;
        bool  delete ;
        char  *language ;
        char  *description ;
        char  *comment ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetComment) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (ID3_COMMENT, &frame)) {
        LGE "(id3SetComment) Error creating frame for \"%s\".\nid3FrameCreate: ",
            comment) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if (delete || (comment == NULL) || (strlen (comment) == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        comment = "" ;
    }

/* Set the 3-character language code in the ID3_UTIL comment frame. */

    if (language == NULL)  language = "eng" ;

    strncpym (frame->fdata.comm.language, language,
              3, sizeof frame->fdata.comm.language) ;

/* Set the short description of the comment. */

    if (description == NULL)  description = "" ;

    frame->fdata.comm.description = strdup (description) ;
    if (frame->fdata.comm.description == NULL) {
        LGE "(id3SetComment) Error duplicating description of \"%s\".\nstrdup: ",
            description) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the comment text itself. */

    frame->fdata.comm.text = strdup (comment) ;
    if (frame->fdata.comm.text == NULL) {
        LGE "(id3SetComment) Error duplicating text of \"%s\".\nstrdup: ",
            comment) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetComment) Error adding frame for \"%s\" to tag.\nid3TagAddFrame: ",
            comment) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Delete the cached, truncated ID3v1 string for id3GetComment(). */

    if (tag->comment != NULL) {
        free (tag->comment) ;
        tag->comment = NULL ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetCover ()

    Set an ID3 Tag's Cover Picture.


Purpose:

    Function id3SetCover() sets an ID3 tag's cover picture.


    Invocation:

        status = id3SetCover (tag, image, length, makeCopy,
                              mimeType, pictureType, description) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <image>		- I
            is a buffer containing the image data.  To delete the cover
            picture, specify a NULL buffer.
        <length>	- I
            is the length in bytes of the image data.
        <makeCopy>	- I
            specifies whether (true) or not (false) to make a copy of
            the image data buffer.  For example, if this argument is
            false (don't make a copy), the caller-supplied buffer will
            become the property of the tag and it will be free(3)ed
            when the tag is destroyed.
        <mimeType>	- I
            is the MIME type of the image; e.g., "image/jpeg" and "image/png".
        <pictureType>	- I
            is the ID3 picture type of the image; see "id3_util.h".
        <description>	- I
            is a short description of the picture; if this argument is NULL,
            the description in the tag will be "" (a zero-length string).
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetCover (

#    if PROTOTYPES
        Id3Tag  tag,
        char  *image,
        size_t  length,
        bool  makeCopy,
        const  char  *mimeType,
        Id3PictureType  pictureType,
        const  char  *description)
#    else
        tag, image, length, makeCopy, mimeType, pictureType, description)

        Id3Tag  tag ;
        char  *image ;
        size_t  length ;
        bool  makeCopy ;
        const  char  *mimeType ;
        Id3PictureType  pictureType ;
        char  *description ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetCover) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (ID3_ATTACHEDPIC, &frame)) {
        LGE "(id3SetCover) Error creating frame for %"PRIuSIZE_T"-byte image.\nid3FrameCreate: ",
            length) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if ((image == NULL) || (length == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        image = NULL ;  length = 0  ;
    }

/* Set the MIME type for the image. */

    if (mimeType == NULL)  mimeType = "image/jpeg" ;	/* Default. */

    frame->fdata.apic.mimeType = strdup (mimeType) ;
    if (frame->fdata.apic.mimeType == NULL) {
        LGE "(id3SetCover) Error duplicating MIME type of \"%s\".\nstrdup: ",
            mimeType) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the picture type. */

    frame->fdata.apic.type = pictureType ;

/* Set the image description. */

    if (description == NULL)  description = "" ;

    frame->fdata.apic.description = strdup (description) ;
    if (frame->fdata.apic.description == NULL) {
        LGE "(id3SetCover) Error duplicating description of \"%s\".\nstrdup: ",
            description) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the image data and length. */

    if (makeCopy) {
        frame->fdata.apic.image = memdup ((image == NULL) ? "" : image,
                                          (image == NULL) ? 1 : length) ;
        if (frame->fdata.apic.image == NULL) {
            LGE "(id3SetCover) Error duplicating %"PRIuSIZE_T"-byte image data.\nmemdup: ",
                length) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {
        frame->fdata.apic.image = image ;
    }

    frame->fdata.apic.length = length ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetCover) Error adding frame for %"PRIuSIZE_T"-byte image to tag.\nid3TagAddFrame: ",
            length) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetGenre ()

    Set an ID3 Tag's Genre Field.


Purpose:

    Function id3SetGenre() sets an ID3 tag's genre field.


    Invocation:

        status = id3SetGenre (tag, genre) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <genre>		- I
            specifies the genre name to be set in the ID3 tag.  (Upon
            encoding into an ID3v1 tag, the name will be mapped to
            the corresponding ID3v1 genre byte.)  To clear the field,
            specify a NULL name.
        <status>	- O
            returns the status of setting the field, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetGenre (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *genre)
#    else
        tag, genre)

        Id3Tag  tag ;
        char  *genre ;
#    endif

{    /* Local variables. */
    char  *s ;
    long  number ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetGenre) NULL tag handle: ") ;
        return (errno) ;
    }

/* If the field argument is an ID3v1 genre number, map it to its text name. */

    number = strtol (genre, &s, 0) ;
    if (*s == '\0') {
        s = (char *) id3FromGenre (number) ;
        if (s != NULL)  genre = s ;		/* Name found?  Use it. */
    }

/* Add a new frame or replace the existing frame for the genre name. */

    if (id3SetText (tag, ID3_GENRE, genre, NULL)) {
        LGE "(id3SetGenre) Error setting text frame for \"%s\".\nid3SetText: ",
            (genre == NULL) ? "<null>" : genre) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetList ()

    Set a Paired-Value List in an ID3 Tag.


Purpose:

    Function id3SetList() is a general-purpose function for adding/replacing
    arbitrary paired-value list frame types to/in an ID3 tag.  Paired-value
    lists are ID3's "involved people list"s.  The caller specifies which
    frame type to set and the list.  The list is specified as a string in
    the following format:

        "<role1>:<person1>;<role2>:<person2>;...;<roleN>:<personN>;"

    where <role> and <person> are pretty loosely defined depending on the
    intent of the target frame type.


    Invocation:

        status = id3SetList (tag, type, listSpec, qualifier) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            specifies the paired-value list frame type (the Id3MarshalPVList
            types in "id3_util.h/.c").
        <listSpec>	- I
            is the list specification string in the format described above.
            The specification is parsed into a list of values in the frame.
            To delete the existing frame of this type, specify a NULL list
            specification string.
        <status>	- O
            returns the status of setting the paired-value list,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetList (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        const  char  *listSpec)
#    else
        tag, type, listSpec)

        Id3Tag  tag ;
        Id3FrameType  type ;
        char  *listSpec ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetList) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create an empty frame of the specified type.  The frame is not really
   needed until later, but this makes it easier to check if the frame type
   is a list frame type (via the frame's "info" link). */

    if (id3FrameCreate (type, &frame)) {
        LGE "(id3SetList) Error creating frame for \"%s\".\nid3FrameCreate: ",
            (listSpec == NULL) ? "<null>" : listSpec) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if ((listSpec == NULL) || (strlen (listSpec) == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        listSpec = "" ;
    }

/* Verify that the frame type is a paired-value list frame type by examining
   the frame type's marshaling method. */

    if (frame->info->marshalType != Id3MarshalPVList) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetList) Not a paired-value list frame type: %s (%d)",
            frame->info->name, (int) type) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Parse the list specification and save the paired values in the frame. */

    if (id3FramePVLfromS (frame, listSpec, true)) {
        LGE "(id3SetList) Error constructing list from \"%s\".\nid3FramePVLfromS: ",
            listSpec) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetList) Error adding frame for \"%s\" to tag.\nid3TagAddFrame: ",
            listSpec) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetObject ()

    Set a General Encapsulated Object in an ID3 Tag.


Purpose:

    Function id3SetObject() sets a general encapsulated object in an ID3 tag.
    These objects allow arbitrary blocks of data to be stored in a tag.
    The accompanying but optional MIME type gives an application some idea
    of how to interpret the block of data.


    Invocation:

        status = id3SetObject (tag, object, length, makeCopy,
                               mimeType, fileName, description) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <object>		- I
            is a buffer containing the object data.  To delete the object
            identifed by the description field, specify a NULL buffer.
        <length>	- I
            is the length in bytes of the object data.
        <makeCopy>	- I
            specifies whether (true) or not (false) to make a copy of
            the object data buffer.  For example, if this argument is
            false (don't make a copy), the caller-supplied buffer will
            become the property of the tag and it will be free(3)ed
            when the tag is destroyed.
        <mimeType>	- I
            is the MIME type of the object; e.g., "audio/midi".
            If this argument is NULL, the MIME type in the tag
            will be "" (a zero-length string).
        <fileName>	- I
            is the name of the source file.  This is optional since,
            instead of being read from a file, the object data could
            have been generated on-the-fly by the application writing
            the tag.  If this argument is NULL, the file name type in
            the tag will be "" (a zero-length string).
        <description>	- I
            is a description of the object; if this argument is NULL,
            the description in the tag will be "" (a zero-length string).
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetObject (

#    if PROTOTYPES
        Id3Tag  tag,
        char  *object,
        size_t  length,
        bool  makeCopy,
        const  char  *mimeType,
        const  char  *fileName,
        const  char  *description)
#    else
        tag, object, length, makeCopy, mimeType, fileName, description)

        Id3Tag  tag ;
        char  *object ;
        size_t  length ;
        bool  makeCopy ;
        const  char  *mimeType ;
        const  char  *fileName ;
        char  *description ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetObject) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (ID3_GENERALOBJ, &frame)) {
        LGE "(id3SetObject) Error creating frame for %"PRIuSIZE_T"-byte object.\nid3FrameCreate: ",
            length) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if ((object == NULL) || (length == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        object = NULL ;  length = 0  ;
    }

/* Set the MIME type for the object. */

    if (mimeType == NULL)  mimeType = "" ;

    frame->fdata.geob.mimeType = strdup (mimeType) ;
    if (frame->fdata.geob.mimeType == NULL) {
        LGE "(id3SetObject) Error duplicating MIME type of \"%s\".\nstrdup: ",
            mimeType) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the object's file name. */

    if (fileName == NULL)  fileName = "" ;

    frame->fdata.geob.fileName = strdup (fileName) ;
    if (frame->fdata.geob.fileName == NULL) {
        LGE "(id3SetObject) Error duplicating file name of \"%s\".\nstrdup: ",
            fileName) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the object description. */

    if (description == NULL)  description = "" ;

    frame->fdata.geob.description = strdup (description) ;
    if (frame->fdata.geob.description == NULL) {
        LGE "(id3SetObject) Error duplicating description of \"%s\".\nstrdup: ",
            description) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the object data and length. */

    if (makeCopy) {
        frame->fdata.geob.object = memdup ((object == NULL) ? "" : object,
                                          (object == NULL) ? 1 : length) ;
        if (frame->fdata.geob.object == NULL) {
            LGE "(id3SetObject) Error duplicating %"PRIuSIZE_T"-byte object data.\nmemdup: ",
                length) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {
        frame->fdata.geob.object = object ;
    }

    frame->fdata.geob.length = length ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetObject) Error adding frame for %"PRIuSIZE_T"-byte object to tag.\nid3TagAddFrame: ",
            length) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetPlayCount ()

    Set an ID3 Tag's Play Count or Popularimenter Field.


Purpose:

    Function id3SetPlayCount() sets an ID3 tag's play count by adding
    or replacing a a Play Counter (PCNT) or Popularimeter (POPM) frame.
    If an email address is supplied, a POPM frame is added/replaced.
    Otherwise a PCNT frame is added/replaced.

    A Play Counter frame only contains a play counter.  Only a single
    PCNT frame is allowed in a tag.  A Popularimeter frame adds an
    email address and a track rating to a tag.  Multiple POPM frames
    with different email addresses are allowed.


    Invocation:

        status = id3SetPlayCount (tag, delete, email, rating, count) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <delete>	- I
            specifies if the target frame is to be deleted.  If true and
            the email address is NULL, then the PCNT frame in the overlay
            tag is marked for deletion.  If true and the email address is
            not NULL, then the POPM frame with the matching email address
            is marked for deletion.  If false, then the value of a new or
            existing frame is set.
        <email>		- I
            specifies the email address in a Popularimeter (POPM) frame.
            If this argument is NULL, the play count is set in a PCNT
            frame.  Otherwise the play count is set in a POPM frame with
            a matching email address; if no matching POPM frame is found,
            a new frame is added.
        <rating>	- O
            is the track rating (1..255) stored in a POPM frame.
        <count>		- O
            is the counter stored in both PCNT and POPM frames.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetPlayCount (

#    if PROTOTYPES
        Id3Tag  tag,
        bool  delete,
        const  char  *email,
        unsigned  int  rating,
        size_t  count)
#    else
        tag, delete, email, rating, count)

        Id3Tag  tag ;
        bool  delete ;
        char  *email ;
        unsigned  int  rating ;
        size_t  count ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;
    Id3FrameType  type ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetPlayCount) NULL tag handle: ") ;
        return (errno) ;
    }

    type = (email == NULL) ? ID3_PLAYCOUNT : ID3_POPULARIMETER ;

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (type, &frame)) {
        LGE "(id3SetPlayCount) Error creating %s frame.\nid3FrameCreate: ",
            (type == ID3_PLAYCOUNT) ? "PCNT" : "POPM") ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if (delete)  frame->delete = true ;		/* Mark frame for deletion. */

/* Set the email address and rating in a POPM frame. */

    if (type == ID3_POPULARIMETER) {
        frame->fdata.play.email = strdup (email) ;
        if (frame->fdata.play.email == NULL) {
            LGE "(id3SetPlayCount) Error duplicating email \"%s\" for POPM frame.\nstrdup: ",
            email) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
        frame->fdata.play.rating = rating ;
    }

/* Set the play count for both PCNT and POPM frames. */

    frame->fdata.play.counter = count ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetPlayCount) Error adding %s frame to tag.\nid3TagAddFrame: ",
            (type == ID3_PLAYCOUNT) ? "PCNT" : "POPM") ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetPrivate ()

    Set a Private Data Field in an ID3 Tag.


Purpose:

    Function id3SetPrivate() stores private data in a Private (PRIV) frame,
    identified by its owner ID, in an ID3 tag.


    Invocation:

        status = id3SetPrivate (tag, delete, ownerID, data, length, makeCopy) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <delete>	- I
            specifies if the target frame is to be deleted.  If true, then
            the PRIV frame with the matching owner ID is marked for deletion.
            If false, then the value of a new or existing frame is set.
        <ownerID>	- I
            is the owner ID of the private data; if this argument is NULL,
            the owner ID in the tag will be "" (a zero-length string).
        <data>	- I
            is the address of a buffer of arbitrary binary data.
        <length>	- I
            is the length in bytes of the private data buffer.
        <makeCopy>	- I
            specifies whether (true) or not (false) to make a copy of the
            data buffer.  For example, if this argument is false (don't
            make a copy), the caller-supplied buffer will become the property
            of the tag and it will be free(3)ed when the tag is destroyed.
            If this argument is true, a copy is made of the caller-supplied
            buffer and the caller can do what it wants with its buffer.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetPrivate (

#    if PROTOTYPES
        Id3Tag  tag,
        bool  delete,
        const  char  *ownerID,
        char  *data,
        size_t  length,
        bool  makeCopy)
#    else
        tag, delete, ownerID, data, length, makeCopy)

        Id3Tag  tag ;
        bool  delete,
        char  *ownerID,
        char  *data,
        size_t  length ;
        bool  makeCopy ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetPrivate) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (ID3_PRIVATE, &frame)) {
        LGE "(id3SetPrivate) Error creating frame for %"PRIuSIZE_T"-byte PRIV data.\nid3FrameCreate: ",
            length) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if (delete || (data == NULL) || (length == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        data = NULL ;  length = 0  ;
    }

/* Set the owner ID. */

    if (ownerID == NULL)  ownerID = "" ;

    frame->fdata.priv.ownerID = strdup (ownerID) ;
    if (frame->fdata.priv.ownerID == NULL) {
        LGE "(id3SetPrivate) Error duplicating owner ID of \"%s\".\nstrdup: ",
            ownerID) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the private data and length. */

    if (makeCopy) {
        frame->fdata.priv.data = memdup ((data == NULL) ? "" : data,
                                         (data == NULL) ? 1 : length) ;
        if (frame->fdata.priv.data == NULL) {
            LGE "(id3SetPrivate) Error duplicating %"PRIuSIZE_T"-byte data.\nmemdup: ",
                length) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {
        frame->fdata.priv.data = data ;
    }

    frame->fdata.priv.length = length ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetPrivate) Error adding frame for %"PRIuSIZE_T"-byte PRIV data to tag.\nid3TagAddFrame: ",
            length) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetRegistration ()

    Set an Encryption/Group-ID Registration Field in an ID3 Tag.


Purpose:

    Function id3SetRegistration() sets encryption method and group ID
    registrations in ENCR and GRID frames, respectively, in an ID3 tag.


    Invocation:

        status = id3SetRegistration (tag, type, delete, symbol,
                                     ownerID, data, length, makeCopy) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            is the type of frame being set, ID3_ENCRYPTREG for an
            encryption method registration (ENCR) and ID3_GROUPIDREG
            for a group ID registration (GRID).
        <delete>	- I
            specifies if the target frame is to be deleted.  If true, then
            the ENCR/GRID frame with the matching symbol is marked for
            deletion.  If false, then the value of a new or existing frame
            is set.
        <symbol>	- I
            is the encryption method or group ID, both 0..255.
        <ownerID>	- I
            is the owner ID of the registration; if this argument is NULL,
            the owner ID in the tag will be "" (a zero-length string).
        <data>	- I
            is the address of a buffer of arbitrary binary data.
        <length>	- I
            is the length in bytes of the data buffer.
        <makeCopy>	- I
            specifies whether (true) or not (false) to make a copy of the
            data buffer.  For example, if this argument is false (don't
            make a copy), the caller-supplied buffer will become the property
            of the tag and it will be free(3)ed when the tag is destroyed.
            If this argument is true, a copy is made of the caller-supplied
            buffer and the caller can do what it wants with its buffer.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetRegistration (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        bool  delete,
        unsigned  int  symbol,
        const  char  *ownerID,
        char  *data,
        size_t  length,
        bool  makeCopy)
#    else
        tag, type, delete, symbol, ownerID, data, length, makeCopy)

        Id3Tag  tag ;
        Id3FrameType  type ;
        bool  delete ;
        unsigned  int  symbol ;
        char  *ownerID ;
        char  *data ;
        size_t  length ;
        bool  makeCopy ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetRegistration) NULL tag handle: ") ;
        return (errno) ;
    }

    if ((type != ID3_ENCRYPTREG) && (type != ID3_GROUPIDREG)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetRegistration) Invalid frame type: %s\n",
            id3FrameTypeName (type)) ;
        return (errno) ;
    }

    if (symbol > 255) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetRegistration) Invalid %s symbol 0x%04X (0..255): ",
            id3FrameTypeName (type), symbol) ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (type, &frame)) {
        LGE "(id3SetRegistration) Error creating frame for %s symbol 0x%02X.\nid3FrameCreate: ",
            id3FrameTypeName (type), symbol, length) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if (delete) {
        frame->delete = true ;		/* Mark frame for deletion. */
        data = NULL ;  length = 0  ;
    }

/* Set the owner ID. */

    if (ownerID == NULL)  ownerID = "" ;

    frame->fdata.regi.ownerID = strdup (ownerID) ;
    if (frame->fdata.regi.ownerID == NULL) {
        LGE "(id3SetRegistration) Error duplicating %s owner ID of \"%s\".\nstrdup: ",
            id3FrameTypeName (type), ownerID) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the encryption method or group ID symbol. */

    frame->fdata.regi.symbol = symbol ;

/* Set the private data and length. */

    if (makeCopy) {
        frame->fdata.regi.data = memdup ((data == NULL) ? "" : data,
                                         (data == NULL) ? 1 : length) ;
        if (frame->fdata.regi.data == NULL) {
            LGE "(id3SetRegistration) Error duplicating %s %"PRIuSIZE_T"-byte data.\nmemdup: ",
                id3FrameTypeName (type), length) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {
        frame->fdata.regi.data = data ;
    }

    frame->fdata.regi.length = length ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetRegistration) Error adding frame for %s symbol 0x%02X to tag.\nid3TagAddFrame: ",
            id3FrameTypeName (type), length) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetSong ()

    Set an ID3 Tag's Song Field.


Purpose:

    Function id3SetSong() sets an ID3 tag's song field.


    Invocation:

        status = id3SetSong (tag, song) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <song>		- I
            specifies the song title to be set in the ID3 tag.  (Upon
            encoding into an ID3v1 tag, the title will be truncated to
            30 characters.)  To clear the field, specify a NULL name.
        <status>	- O
            returns the status of setting the field, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetSong (

#    if PROTOTYPES
        Id3Tag  tag,
        const  char  *song)
#    else
        tag, song)

        Id3Tag  tag ;
        char  *song ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetSong) NULL tag handle: ") ;
        return (errno) ;
    }

/* Add a new frame or replace the existing frame for the song title. */

    if (id3SetText (tag, ID3_TITLE, song, NULL)) {
        LGE "(id3SetSong) Error setting text frame for \"%s\".\nid3SetText: ",
            (song == NULL) ? "<null>" : song) ;
        return (errno) ;
    }

/* Delete the cached, truncated ID3v1 string for id3GetSong(). */

    if (tag->song != NULL) {
        free (tag->song) ;
        tag->song = NULL ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetText ()

    Set a Text Information Field in an ID3 Tag.


Purpose:

    Function id3SetText() is a general-purpose function for adding/replacing
    arbitrary text information frame types to/in an ID3 tag.  The caller
    specifies which frame type to set and, in the case of a user-defined
    text information frame, the description of the text.


    Invocation:

        status = id3SetText (tag, type, text, qualifier) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            specifies the text information frame type (one of the
            Id3MarshalText or Id3MarshalUText types in "id3_util.h/.c").
        <text>		- I
            is the text string.  This is nominally a UTF-8 string of Unicode
            characters which will be encoded into the in-file tag according
            to the tag's text encoding byte.  To delete the existing frame
            of this type, specify a NULL text string.  A copy of the string
            is made for storage in the new frame.
        <qualifier>	- I
            specifies further qualification for certain frame types.  If the
            frame type is ID3_TEXT_GENERIC, this argument is the 4-character
            frame ID to be used when the frame is encoded.  If the frame type
            is ID3_TXXX_GENERIC, this argument is the description of the text
            field.  This argument is ignored for the other text information
            frame types.  An existing ID3_TEXT_GENERIC or ID3_TXXX_GENERIC
            frame with the same qualifier will be deleted.  A copy of this
            argument is made for storage in the new frame.
        <status>	- O
            returns the status of setting the text information field,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetText (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        const  char  *text,
        const  char  *qualifier)
#    else
        tag, type, text, qualifier)

        Id3Tag  tag ;
        Id3FrameType  type ;
        char  *text ;
        char  *qualifier ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetText) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create an empty frame of the specified type.  The frame is not really
   needed until later, but this makes it easier to check if the frame type
   is a text information frame type (via the frame's "info" link). */

    if (id3FrameCreate (type, &frame)) {
        LGE "(id3SetText) Error creating frame for \"%s\".\nid3FrameCreate: ",
            (text == NULL) ? "<null>" : text) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if ((text == NULL) || (strlen (text) == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        text = "" ;
    }

/* Verify that the frame type is a text information frame type by examining
   the frame type's marshaling method. */

    if ((frame->info->marshalType != Id3MarshalText) &&
        (frame->info->marshalType != Id3MarshalUText)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetText) Not a text information frame type: %s (%d)",
            frame->info->name, (int) type) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Save the text (and qualifier if necessary) in the frame. */

    frame->fdata.text.text = strdup (text) ;
    if (frame->fdata.text.text == NULL) {
        LGE "(id3SetText) Error duplicating text: \"%s\".\nstrdup: ", text) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    if ((type == ID3_TEXT_GENERIC) && (qualifier != NULL)) {
        frame->frameID = strdup (qualifier) ;
        if (frame->frameID == NULL) {
            LGE "(id3SetText) Error duplicating generic text frame ID: \"%s\".\nstrdup: ",
                qualifier) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    }

    if ((type == ID3_TXXX_GENERIC) && (qualifier != NULL)) {
        frame->fdata.text.description = strdup (qualifier) ;
        if (frame->fdata.text.description == NULL) {
            LGE "(id3SetText) Error duplicating TXXX description: \"%s\".\nstrdup: ",
                qualifier) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    }

/* If this is a qualified, user-defined frame type ("TXXX:<description>"),
   then substitute the qualified type for the generic frame type. */

    if ((type == ID3_TXXX_GENERIC) && id3FrameUDUpgrade (frame))
        type = frame->type ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetText) Error adding frame for \"%s\" to tag.\nid3TagAddFrame: ",
            text) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetTrack ()

    Set an ID3 Tag's Track Field.


Purpose:

    Function id3SetTrack() sets an ID3 tag's track field.


    Invocation:

        status = id3SetTrack (tag, track, total) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <track>		- I
            specifies the track number to be set in the ID3 tag.
            (Upon encoding into an ID3v2 tag, the name will formatted
            into a string for the TRCK frame.)  To clear the field,
            specify a value less than or equal to zero.
        <total>		- I
            specifies the total number of tracks; a total less than or
            equal to zero indicates that the total should not be set.
        <status>	- O
            returns the status of setting the field, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetTrack (

#    if PROTOTYPES
        Id3Tag  tag,
        long  track,
        long  total)
#    else
        tag, track, total)

        Id3Tag  tag ;
        long  track ;
        long  total ;
#    endif

{    /* Local variables. */
    char  text[42] ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetTrack) NULL tag handle: ") ;
        return (errno) ;
    }

/* Add a new frame or replace the existing frame for the track number. */

    if (total > 0)
        sprintf (text, "%ld/%ld", track, total) ;
    else
        sprintf (text, "%ld", track) ;

    if (id3SetText (tag, ID3_TRACK, (track <= 0) ? NULL : text, NULL)) {
        LGE "(id3SetTrack) Error setting text frame for \"%s\".\nid3SetText: ",
            (track <= 0) ? "<null>" : text) ;
        return (errno) ;
    }

/* Delete the cached ID3v1 value for id3GetTrack(). */

    tag->track = -1 ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetUFID ()

    Set a Unique File ID in an ID3 Tag.


Purpose:

    Function id3SetUFID() sets a unique file ID, identified by its owner ID,
    in an ID3 tag.  A file ID is arbitrary binary data up to 64 bytes in length.


    Invocation:

        status = id3SetUFID (tag, delete, ownerID, fileID, length, makeCopy) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <delete>	- I
            specifies if the target frame is to be deleted.  If true, then
            the UFID frame with the matching owner ID is marked for deletion.
            If false, then the value of a new or existing frame is set.
        <ownerID>	- I
            is the owner ID of the UFID; if this argument is NULL,
            the owner ID in the tag will be "" (a zero-length string).
        <fileID>	- I
            is the address of a buffer of arbitrary binary data.
        <length>	- I
            is the length in bytes of the file ID buffer.
        <makeCopy>	- I
            specifies whether (true) or not (false) to make a copy of the
            file ID buffer.  For example, if this argument is false (don't
            make a copy), the caller-supplied buffer will become the property
            of the tag and it will be free(3)ed when the tag is destroyed.
            If this argument is true, a copy is made of the caller-supplied
            buffer and the caller can do what it wants with its buffer.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetUFID (

#    if PROTOTYPES
        Id3Tag  tag,
        bool  delete,
        const  char  *ownerID,
        char  *fileID,
        size_t  length,
        bool  makeCopy)
#    else
        tag, delete, ownerID, fileID, length, makeCopy)

        Id3Tag  tag ;
        bool  delete,
        char  *ownerID,
        char  *fileID,
        size_t  length ;
        bool  makeCopy ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetUFID) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (ID3_UNIQUEFILEID, &frame)) {
        LGE "(id3SetUFID) Error creating frame for %"PRIuSIZE_T"-byte UFID.\nid3FrameCreate: ",
            length) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if (delete || (fileID == NULL) || (length == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        fileID = NULL ;  length = 0  ;
    }

/* Set the owner ID. */

    if (ownerID == NULL)  ownerID = "" ;

    frame->fdata.ufid.ownerID = strdup (ownerID) ;
    if (frame->fdata.ufid.ownerID == NULL) {
        LGE "(id3SetUFID) Error duplicating owner ID of \"%s\".\nstrdup: ",
            ownerID) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the file ID's data and length. */

    if (makeCopy) {
        frame->fdata.ufid.fileID = memdup ((fileID == NULL) ? "" : fileID,
                                          (fileID == NULL) ? 1 : length) ;
        if (frame->fdata.ufid.fileID == NULL) {
            LGE "(id3SetUFID) Error duplicating %"PRIuSIZE_T"-byte file ID.\nmemdup: ",
                length) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else {
        frame->fdata.ufid.fileID = fileID ;
    }

    frame->fdata.ufid.length = length ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetUFID) Error adding frame for %"PRIuSIZE_T"-byte UFID to tag.\nid3TagAddFrame: ",
            length) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetUnsyncLyrics ()

    Set an ID3 Tag's Unsynchronized Lyrics.


Purpose:

    Function id3SetUnsyncLyrics() sets an ID3 tag's unsynchronized lyrics.


    Invocation:

        status = id3SetUnsyncLyrics (tag, delete,
                                     language, description, lyrics) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <delete>	- I
            specifies if the target frame is to be deleted.  If true, then
            the USLT frame with the matching language and description is
            marked for deletion.  If false, then the value of a new or
            existing frame is set.
        <language>	- I
            is the 3-character code for the language of the lyrics;
            if this argument is NULL, the language defaults to "eng".
        <description>	- I
            is a description of the lyrics; if this argument is NULL,
            the description in the tag will be "" (a zero-length string).
        <lyrics>	- I
            specifies the lyrics to be set in the ID3 tag.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetUnsyncLyrics (

#    if PROTOTYPES
        Id3Tag  tag,
        bool  delete,
        const  char  *language,
        const  char  *description,
        const  char  *lyrics)
#    else
        tag, delete, language, description, lyrics)

        Id3Tag  tag ;
        bool  delete ;
        char  *language ;
        char  *description ;
        char  *lyrics ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetUnsyncLyrics) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create a frame of the desired type and set its value. */

    if (id3FrameCreate (ID3_UNSYNCEDLYRICS, &frame)) {
        LGE "(id3SetUnsyncLyrics) Error creating frame for \"%s\".\nid3FrameCreate: ",
            lyrics) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if (delete || (lyrics == NULL) || (strlen (lyrics) == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        lyrics = "" ;
    }

/* Set the 3-character language code in the USLT frame. */

    if (language == NULL)  language = "eng" ;

    strncpym (frame->fdata.uslt.language, language,
              3, sizeof frame->fdata.uslt.language) ;

/* Set the description of the lyrics. */

    if (description == NULL)  description = "" ;

    frame->fdata.uslt.description = strdup (description) ;
    if (frame->fdata.uslt.description == NULL) {
        LGE "(id3SetUnsyncLyrics) Error duplicating description of \"%s\".\nstrdup: ",
            description) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Set the lyrics text itself. */

    frame->fdata.uslt.lyrics = strdup (lyrics) ;
    if (frame->fdata.uslt.lyrics == NULL) {
        LGE "(id3SetUnsyncLyrics) Error duplicating lyrics.\nstrdup: ") ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetUnsyncLyrics) Error adding frame for \"%s\" to tag.\nid3TagAddFrame: ",
            lyrics) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetURL ()

    Set a URL Link in an ID3 Tag.


Purpose:

    Function id3SetURL() is a general-purpose function for adding/replacing
    arbitrary URL link frame types to/in an ID3 tag.  The caller specifies
    which frame type to set and, in the case of a user-defined URL link frame,
    the description of the URL.


    Invocation:

        status = id3SetURL (tag, type, url, qualifier) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <type>		- I
            specifies the URL frame type (one of the ID3_WWW... or
            ID3_WXXX_GENERIC types in "id3_util.h").
        <url>		- I
            is the URL.  To delete the existing frame of this type in the tag,
            specify a NULL URL.  A copy of the URL is made for storage in the
            new frame.
        <qualifier>	- I
            specifies further qualification for certain frame types.  If the
            frame type is ID3_WURL_GENERIC, this argument is the 4-character
            frame ID to be used when the frame is encoded.  If the frame type
            is ID3_WXXX_GENERIC, this argument is the description of the URL
            field.  This argument is ignored for the other URL frame types.
            An existing ID3_WURL_GENERIC or ID3_WXXX_GENERIC frame with the
            same qualifier will be deleted.  A copy of this argument is made
            for storage in the new frame.
        <status>	- O
            returns the status of setting the URL, zero if there were no
            errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetURL (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3FrameType  type,
        const  char  *url,
        const  char  *qualifier)
#    else
        tag, type, url, qualifier)

        Id3Tag  tag ;
        Id3FrameType  type ;
        char  *url ;
        char  *qualifier ;
#    endif

{    /* Local variables. */
    Id3Frame  frame ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetURL) NULL tag handle: ") ;
        return (errno) ;
    }

/* Create an empty frame of the specified type.  The frame is not really
   needed until later, but this makes it easier to check if the frame type
   is a URL link frame type (via the frame's "info" link). */

    if (id3FrameCreate (type, &frame)) {
        LGE "(id3SetURL) Error creating frame for \"%s\".\nid3FrameCreate: ",
            (url == NULL) ? "<null>" : url) ;
        return (errno) ;
    }

    frame->parent = tag ;

/* If the frame is to be deleted from the tag, mark the new frame for
   deletion.  The new "deleted" frame will replace the old frame in
   the tag, but is itself not actually deleted.  Not doing so allows
   the "deleted" tag to propagate through to, but not be included in,
   the final tag written to the audio file. */

    if ((url == NULL) || (strlen (url) == 0)) {
        frame->delete = true ;		/* Mark frame for deletion. */
        url = "" ;
    }

/* Verify that the frame type is a URL link frame type by examining the
   frame type's marshaling method. */

    if ((frame->info->marshalType != Id3MarshalURL) &&
        (frame->info->marshalType != Id3MarshalUURL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetURL) Not a URL frame type: %s (%d)",
            frame->info->name, (int) type) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Save the URL (and description if necessary) in the frame. */

    frame->fdata.wurl.url = strdup (url) ;
    if (frame->fdata.wurl.url == NULL) {
        LGE "(id3SetURL) Error duplicating URL: \"%s\".\nstrdup: ", url) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    if ((type == ID3_WURL_GENERIC) && (qualifier != NULL)) {
        frame->frameID = strdup (qualifier) ;
        if (frame->frameID == NULL) {
            LGE "(id3SetURL) Error duplicating generic URL frame ID: \"%s\".\nstrdup: ",
                qualifier) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    }

    if ((type == ID3_WXXX_GENERIC) && (qualifier != NULL)) {
        frame->fdata.wurl.description = strdup (qualifier) ;
        if (frame->fdata.wurl.description == NULL) {
            LGE "(id3SetURL) Error duplicating WXXX description: \"%s\".\nstrdup: ",
                qualifier) ;
            PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
            return (errno) ;
        }
    }

/* If this is a qualified, user-defined frame type ("TXXX:<description>"),
   then substitute the qualified type for the generic frame type. */

    if ((type == ID3_WXXX_GENERIC) && id3FrameUDUpgrade (frame))
        type = frame->type ;

/* Finally, add the new frame to the tag; id3TagAddFrame() will delete
   the old frame. */

    if (id3TagAddFrame (tag, frame, true)) {
        LGE "(id3SetURL) Error adding frame for \"%s\" to tag.\nid3TagAddFrame: ",
            url) ;
        PUSH_ERRNO ;  id3FrameDestroy (frame) ;  POP_ERRNO ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3SetVersion ()

    Set an ID3 Tag's Version.


Purpose:

    Function id3SetVersion() sets the version (including the major and
    revision numbers) of an ID3 tag.


    Invocation:

        status = id3SetVersion (tag, version) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <version>	- I
            is tag's new version; see the Id3Version type definition
            in "id3_util.h".
        <status>	- O
            returns the status of setting the version, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetVersion (

#    if PROTOTYPES
        Id3Tag  tag,
        Id3Version  version)
#    else
        tag, version)

        Id3Tag  tag ;
        Id3Version  version ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetVersion) NULL tag handle: ") ;
        return (0L) ;
    }

    if (ID3V_VERSION (version) == 1) {
        if (ID3V_MAJOR (version) > 1) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3SetVersion) Invalid version %06lX: ", (long) version) ;
            return (0L) ;
        }
    } else if (ID3V_VERSION (version) == 2) {
        if ((ID3V_MAJOR (version) < 3) || (ID3V_MAJOR (version) > 4)) {
            SET_ERRNO (EINVAL) ;
            LGE "(id3SetVersion) Invalid version %06lX: ", (long) version) ;
            return (0L) ;
        }
    } else {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetVersion) Invalid version %06lX: ", (long) version) ;
        return (0L) ;
    }

    tag->version = version ;
    tag->overrideVersion = true ;	/* Explicitly set, so override when
					   merging with existing tag. */

    return (tag->version) ;

}

/*!*****************************************************************************

Procedure:

    id3SetYear ()

    Set an ID3 Tag's Year Field.


Purpose:

    Function id3SetYear() sets an ID3 tag's year.


    Invocation:

        status = id3SetYear (tag, year) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <year>		- I
            specifies the year to be set in the ID3 tag.  (Upon encoding
            into an ID3v2 tag, the name will formatted into a string for
            the TYER frame.)  To clear the field, specify a value less
            than or equal to zero.
        <status>	- O
            returns the status of setting the field, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  id3SetYear (

#    if PROTOTYPES
        Id3Tag  tag,
        long  year)
#    else
        tag, year)

        Id3Tag  tag ;
        long  year ;
#    endif

{    /* Local variables. */
    char  text[24] ;



    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3SetYear) NULL tag handle: ") ;
        return (errno) ;
    }

/* Add a new frame or replace the existing frame for the year. */

    sprintf (text, "%ld", year) ;

    if (id3SetText (tag, ID3_YEAR, (year <= 0) ? NULL : text, NULL)) {
        LGE "(id3SetYear) Error setting text frame for %ld.\nid3SetText: ",
            year) ;
        return (errno) ;
    }

/* Delete the cached ID3v1 value for id3GetYear(). */

    tag->year = -1 ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    id3Flags ()

    Get an ID3v2 Tag's Header Flags.


Purpose:

    Function id3Flags() returns the ID3v2 flags found in an ID3v2 tag's header.


    Invocation:

        flags = id3Flags (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <flags>		- O
            returns the ID3v2 header flags from the ID3 tag; zero is returned
            if the ID3 tag is not version 2.

*******************************************************************************/


int  id3Flags (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3Flags) NULL tag handle: ") ;
        return (0) ;
    }

    return (tag->v2Flags) ;

}

/*!*****************************************************************************

Procedure:

    id3Size ()

    Get an ID3 Tag's Size.


Purpose:

    Function id3Size() returns the size, in bytes, of an ID3 tag.
    For ID3v1 tags, the size is always 128 bytes.  For ID3v2 tags,
    the size excludes the 10-byte ID3v2 header.


    Invocation:

        size = id3Size (tag) ;

    where

        <tag>		- I
            is the tag handle returned by id3TagCreate().
        <size>		- O
            returns the size in bytes of the ID3 tag.  NOTE that for ID3v2 tags,
            the size *excludes* the 10-byte header.

*******************************************************************************/


ssize_t  id3Size (

#    if PROTOTYPES
        Id3Tag  tag)
#    else
        tag)

        Id3Tag  tag ;
#    endif

{

    if (tag == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(id3Size) NULL tag handle: ") ;
        return (0) ;
    }

    return ((ID3V_VERSION (tag->version) == 2) ? tag->v2Size
                                               : ID3_V1_TRAILER_SIZE) ;

}

/*!*****************************************************************************

Procedure:

    id3FromGenre ()

    Translate a Genre Number to its Name.


Purpose:

    Function id3FromGenre() looks up a genre number and returns its
    corresponding name.


    Invocation:

        name = id3FromGenre (number) ;

    where

        <number>	- I
            is the number (0..125) for the genre.
        <name>		- O
            returns the genre name; NULL is returned for an invalid number.

*******************************************************************************/


const  char  *id3FromGenre (

#    if PROTOTYPES
        long  number)
#    else
        number)

        long  number ;
#    endif

{

    return (nnlNameOf (genreLUT, number)) ;

}

/*!*****************************************************************************

Procedure:

    id3ToGenre ()

    Translate a Genre Name to its Number.


Purpose:

    Function id3ToGenre() looks up a genre name and returns its corresponding
    number.


    Invocation:

        number = id3ToGenre (name) ;

    where

        <name>		- I
            is the name of the genre.
        <number>	- O
            returns the genre number (0..125) or -1 for an invalid name.

*******************************************************************************/


long  id3ToGenre (

#    if PROTOTYPES
        const  char  *name)
#    else
        name)

        char  *name ;
#    endif

{

    return (nnlNumberOf (genreLUT, name, -1, true, true)) ;

}

/*!*****************************************************************************

Procedure:

    id3FromPictureType ()

    Translate a Picture Type to its Name.


Purpose:

    Function id3FromPictureType() looks up a picture type and returns its
    corresponding name.


    Invocation:

        name = id3FromPictureType (type) ;

    where

        <number>	- I
            is the ID3 picture type; see "id3_util.h" for the defined types.
        <name>		- O
            returns the picture type name; NULL is returned for an invalid
            number.

*******************************************************************************/


const  char  *id3FromPictureType (

#    if PROTOTYPES
        Id3PictureType  type)
#    else
        type)

        Id3PictureType  type ;
#    endif

{

    return (nnlNameOf (pictureTypeLUT, type)) ;

}

/*!*****************************************************************************

Procedure:

    id3ToPictureType ()

    Translate a Picture Type Name to its Number.


Purpose:

    Function id3ToPictureType() looks up a picture type name and returns
    its corresponding number.


    Invocation:

        number = id3ToPictureType (name) ;

    where

        <name>		- I
            is the name of the picture type.
        <number>	- O
            returns the picture type (0..20) or -1 for an invalid name.

*******************************************************************************/


Id3PictureType  id3ToPictureType (

#    if PROTOTYPES
        const  char  *name)
#    else
        name)

        char  *name ;
#    endif

{

    return (nnlNumberOf (pictureTypeLUT, name, -1, true, true)) ;

}
