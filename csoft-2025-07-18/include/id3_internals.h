/* $Id: id3_internals.h,v 1.9 2024/05/28 11:32:48 alex Exp alex $ */
/*******************************************************************************

  id3_internals.h

  ID3 Tag Internal Definitions.

*******************************************************************************/


#ifndef  ID3_INTERNALS_H	/* Has the file been INCLUDE'd already? */
#define  ID3_INTERNALS_H  yes


#include  <stdio.h>			/* Standard I/O definitions. */
#include  "adx_util.h"			/* Array Descriptor utilities. */
#include  "mdx_util.h"			/* Memory Descriptor utilities. */
#include  "id3_util.h"			/* ID3 tag utilities. */


/*******************************************************************************
    ID3V2 Frame Information Lookup Table Structures.
*******************************************************************************/

typedef  enum  Id3MarshalType {
    Id3MarshalNull = 0,
    Id3MarshalComment,
    Id3MarshalGenObj,
    Id3MarshalPassthrough,
    Id3MarshalPicture,
    Id3MarshalPlayCount,
    Id3MarshalPrivate,
    Id3MarshalPVList,
    Id3MarshalRegistration,
    Id3MarshalSyncLyrics,
    Id3MarshalText,
    Id3MarshalUFID,
    Id3MarshalUnsyncLyrics,
    Id3MarshalURL,
    Id3MarshalUText,
    Id3MarshalUURL
}  Id3MarshalType ;

typedef  struct  Id3FrameLUTEntry {
    Id3FrameType  type ;
    const  char  *name ;
    const  char  *v2_3FrameID ;
    const  char  *v2_4FrameID ;
    Id3MarshalType  marshalType ;
}  Id3FrameLUTEntry ;

extern  Id3FrameLUTEntry  id3FrameLUT[] ;


/*******************************************************************************
    ID3 Tag Data Structures.
*******************************************************************************/

/* Id3Value - a data item in an ID3 frame. */

typedef  enum  Id3ValueType {
    Id3ValueUnknownType = 0,
    Id3ValueCustom,			/* Special handling. */
    Id3ValueInteger,
    Id3ValueOctets,
    Id3ValueReal,
    Id3ValueText
}  Id3ValueType ;

typedef  struct  _Id3Value {
    Id3ValueType  type ;
    union  vdata {
        long  integer ;
        double  real ;
        char  *octets ;			/* Binary data value. */
        char  *text ;			/* Text frame value. */
    }  vdata ;
    size_t  length ;			/* # of bytes in binary data value. */
}  _Id3Value, *Id3Value ;

/*******************************************************************************

    Id3Frame - an ID3 frame.

    The "fdata" union holds structures containing the frame-type-specific
    fields.  If you add another structure for a new frame type XYZ:

      - In id3_frame.c:
          + Add support for XYZ to the id3FrameDataCopy(), ...DataErase(),
            and ...DataInit() functions.
          + Add new functions id3FrameDecodeXYZ() and id3FrameEncodeXYZ().
          + Add handling of XYZ to id3FrameDecode() and id3FrameEncode().
          + Add support for XYZ to id3FrameDump().

      - In id3_tag.c:
          + Add support for XYZ to id3TagAddFrame() if necessary.
          + Add support for XYZ to id3TagFindFrame() if necessary.

      - In id3_util.c:
          + Add id3GetXYZ() and id3SetXYZ() functions to create XYZ frames,
            if desired.

*******************************************************************************/

/* ID3v2 frame status flags */

#define  ID3_FRAME_TAG_ALTER		0x8000
#define  ID3_FRAME_FILE_ALTER		0x4000
#define  ID3_FRAME_READ_ONLY		0x2000

/* ID3v2.3 frame encoding/format flags. */

#define  ID3_FRAME_V2_3_COMPRESSED	0x0080	/* Size appended to header. */
#define  ID3_FRAME_V2_3_ENCRYPTED	0x0040	/* Method appended. */
#define  ID3_FRAME_V2_3_GROUPED		0x0020	/* Group ID appended. */

/* ID3v2.4 frame encoding/format flags. */

#define  ID3_FRAME_V2_4_GROUPED		0x0040	/* Group ID appended. */
#define  ID3_FRAME_V2_4_COMPRESSED	0x0008	/* Size appended. */
#define  ID3_FRAME_V2_4_ENCRYPTED	0x0004	/* Method appended. */
#define  ID3_FRAME_V2_4_UNSYNCED	0x0002
#define  ID3_FRAME_V2_4_DATA_LENGTH	0x0001	/* Synchsafe size appended. */

#define  ID3_EXTH_V2_3_CRC_PRESENT	0x8000	/* ID3v2.3 flag */
#define  ID3_EXTH_TAG_IS_UPDATE		0x40	/* ID3v2.4 flags */

typedef  struct  _Id3Frame {
    Id3FrameType  type ;
    Id3FrameLUTEntry  *info ;		/* Frame information table entry. */
    Id3Tag  parent ;			/* Set by id3TagAddFrame(). */
    bool  delete ;			/* Overlay delete; see id3TagMerge(). */
    size_t  size ;			/* # bytes in body, excludes header. */
    long  flags ;			/* From header. */
    char  groupID ;			/* Group ID if flagged in header. */
    Id3TextEncoding  encoding ;		/* Encoding of text in frame. */
    char  *frameID ;			/* ID3_TEXT_GENERIC/ID3_WURL_GENERIC */
    Id3MarshalType  marshalType ;
    union  fdata {
        struct  apic {
            char  *mimeType ;		/* E.g., "image/jpeg" and "image/png". */
            long  type ;		/* Picture type. */
            char  *description ;
            void  *image ;		/* Image data. */
            size_t  length ;		/* Length in bytes of image data. */
        }  apic ;
        struct  geob {
            char  *mimeType ;		/* E.g., "application/pdf". */
            char  *fileName ;
            char  *description ;
            void  *object ;		/* Object data. */
            size_t  length ;		/* Length in bytes of object data. */
        }  geob ;
        struct  comm {
            char  language[4] ;		/* 3-character language code. */
            char  *description ;
            char  *text ;
        }  comm ;
        struct  play {			/* Play counter/Popularimeter. */
            char  *email ;		/* POPM only. */
            unsigned  int  rating ;	/* POPM only. */
            bool  omit ;		/* POPM only. */
            size_t  counter ;
#define  MIN_PLAYCOUNT_BYTE_WIDTH  4	/* Minimum 32 bits per standard. */
        }  play ;
        struct  priv {			/* Private frame. */
            char  *ownerID ;
            size_t  length ;		/* # of data bytes. */
            void  *data ;
        }  priv ;
        struct  ptru {			/* Passthrough (raw) frames. */
            char  *buffer ;		/* Full frame beginning with header. */
            size_t  length ;
        }  ptru ;
        struct  regi {			/* ENCR/GRID frames. */
            char  *ownerID ;
            unsigned  int  symbol ;
            size_t  length ;		/* # of data bytes. */
            void  *data ;
        }  regi ;
       struct  text {			/* Text information frames. */
            char  *description ;	/* TXXX only. */
            char  *text ;
        }  text ;
        struct  ufid {			/* Unique File ID. */
            char  *ownerID ;
            size_t  length ;		/* # of bytes in binary file ID. */
#define  MAX_FILEID_LENGTH  64		/* Max 64 bytes per standard. */
            void  *fileID ;
        }  ufid ;
        struct  uslt {
            char  language[4] ;		/* 3-character language code. */
            char  *description ;
            char  *lyrics ;
        }  uslt ;
        struct  wurl {			/* URL link frames. */
            char  *description ;	/* WXXX only. */
            char  *url ;
        }  wurl ;
    }  fdata ;
    ArrayDx  values ;			/* Additional values in the frame. */
}  _Id3Frame, *Id3Frame ;


/* Id3ExtHeader - extension to core tag header (ID3v2). */

typedef  struct  Id3ExtHeader {
    Id3Version  version ;		/* Version of source tag. */
    size_t  length ;			/* Length (includes size field). */
    long  fields ;			/* Present fields (use V2.4 EXTH flag
					   bits for both v2.3/v2.4 headers). */
    long  flags ;			/* Flags (v2.3/16 bits, v2.4/8 bits). */
    size_t  padding ;			/* Padding size (v2.3 only). */
    unsigned  long  originalCRC ;
    unsigned  long  computedCRC ;
    bool  isUpdate ;			/* (v2.4) Update of earlier tag? */
    long  restrictions ;		/* (v2.4) Size/encoding restrictions. */
    char  *header ;			/* Full header (includes size field). */
}  Id3ExtHeader ;

#define  MAX_SONG_LENGTH  30		/* ID3v1 field lengths. */
#define  MAX_ARTIST_LENGTH  30
#define  MAX_ALBUM_LENGTH  30
#define  MAX_YEAR_LENGTH  4
#define  MAX_COMMENT_LENGTH  28

#define  ID3_V2_UNSYNCHRONIZED_TAG	0x80	/* ID3v2 flags */
#define  ID3_V2_EXTENDED_HEADER		0x40
#define  ID3_V2_EXPERIMENTAL		0x20
#define  ID3_V2_UNSYNCHRONIZED_FRAME	0x0002

#define  ID3_V2_3_EXTHDR_BASE_SIZE  10	/* Size, flags, padding ... */
#define  ID3_V2_3_EXTHDR_MAX_SIZE  14	/* + CRC */
#define  ID3_V2_4_EXTHDR_BASE_SIZE  6	/* Size, flags ... */
#define  ID3_V2_4_EXTHDR_MAX_SIZE  15	/* + update, CRC, restrictions */
#define  ID3_EXTHDR_MAX_SIZE  ID3_V2_4_EXTHDR_MAX_SIZE

#define  ID3_EXTH_V2_3_CRC_PRESENT	0x8000	/* ID3v2.3 flag */
#define  ID3_EXTH_TAG_IS_UPDATE		0x40	/* ID3v2.4 flags */
#define  ID3_EXTH_CRC_PRESENT		0x20
#define  ID3_EXTH_TAG_RESTRICTIONS	0x10

/* Id3Tag - an ID3 tag (used for all ID3 versions). */

typedef  struct  _Id3Tag {
    Id3Version  version ;
    bool  overrideVersion ;		/* Override existing version on merge? */
    Id3TextEncoding  encoding ;		/* Preferred text encoding in frames. */
    long  flags ;			/* Which fields are defined, etc. */
    char  *song ;			/* Cached ID3v1.1 fields ... */
    char  *artist ;
    char  *album ;
    char  *comment ;
    long  year ;			/* 1970, for example. */
    ssize_t  track ;			/* 1..N */
    long  genre ;			/* ID3v1 number, 0..255 */
    char  *genreName ;			/* ID3v2 or matching ID3v1 name. */
    int  v2Flags ;			/* ID3v2 flags. */
    size_t  v2Size ;			/* ID3v2 tag size, excluding header. */
    char  *v2Buffer ;
    bool  unsync ;			/* Unsync on encode? */
    size_t  numUnsyncFrames ;		/* Used in ID3v2.4 encoding. */
    Id3ExtHeader  exth ;		/* Extended header. */
    ArrayDx  frames ;			/* ID3 frames composing the tag. */
}  _Id3Tag ;

/*******************************************************************************
    ID3 Tag functions.
*******************************************************************************/

errno_t  id3TagAddFrame P_((Id3Tag tag,
                            Id3Frame frame,
                            bool unique))
    OCD ("id3_util") ;

Id3Tag  id3TagCopy P_((Id3Tag oldTag))
    OCD ("id3_util") ;

errno_t  id3TagCreate P_((Id3Version version,
                          Id3Tag *tag))
    OCD ("id3_util") ;

extern  errno_t  id3TagDecodeV1 P_((char *buffer,
                                    Id3Tag *tag))
    OCD ("id3_util") ;

extern  errno_t  id3TagDecodeV2 P_((char *buffer,
                                    size_t length,
                                    Id3Tag *tag))
    OCD ("id3_util") ;

errno_t  id3TagDeleteFrame P_((Id3Tag tag,
                               Id3Frame frame))
    OCD ("id3_util") ;

errno_t  id3TagDeletePicture P_((Id3Tag tag,
                                 Id3PictureType type,
                                 const char *description))
    OCD ("id3_util") ;

errno_t  id3TagDestroy P_((Id3Tag tag))
    OCD ("id3_util") ;

errno_t  id3TagDump P_((FILE *file,
                        const char *indentation,
                        Id3Tag tag))
    OCD ("id3_util") ;

errno_t  id3TagDumpLUT P_((FILE *file,
                           const char *indentation))
    OCD ("id3_util") ;

extern  errno_t  id3TagEncodeV1 P_((const Id3Tag tag,
                                    char *buffer))
    OCD ("id3_util") ;

extern  errno_t  id3TagEncodeV2 P_((const Id3Tag tag,
                                    char **buffer,
                                    size_t *length))
    OCD ("id3_util") ;

errno_t  id3TagErase P_((Id3Tag tag))
    OCD ("id3_util") ;

Id3Frame  id3TagFindFrame P_((Id3Tag tag,
                              Id3FrameType type,
                              ...))
    OCD ("id3_util") ;

size_t  id3TagFrameCount P_((Id3Tag tag))
    OCD ("id3_util") ;

Id3Frame  id3TagGetFrame P_((Id3Tag tag,
                             size_t which))
    OCD ("id3_util") ;

Id3Tag  id3TagMerge P_((Id3Tag primaryTag,
                        Id3Tag overlayTag))
    OCD ("id3_util") ;

/*******************************************************************************
    ID3 Frame functions.
*******************************************************************************/

errno_t  id3FrameAddStringV P_((Id3Frame frame,
                                const char *string,
                                ssize_t length))
    OCD ("id3_util") ;

errno_t  id3FrameAddValue P_((Id3Frame frame,
                              Id3Value value))
    OCD ("id3_util") ;

Id3Frame  id3FrameCopy P_((Id3Frame oldFrame))
    OCD ("id3_util") ;

errno_t  id3FrameCreate P_((Id3FrameType type,
                            Id3Frame *frame))
    OCD ("id3_util") ;

errno_t  id3FrameDataCopy P_((Id3Frame source,
                              Id3Frame target))
    OCD ("id3_util") ;

errno_t  id3FrameDataErase P_((Id3Frame frame))
    OCD ("id3_util") ;

errno_t  id3FrameDataInit P_((Id3Frame frame))
    OCD ("id3_util") ;

errno_t  id3FrameDecode P_((Id3Tag tag,
                            char *buffer,
                            size_t length,
                            Id3Frame *frame,
                            size_t *skip))
    OCD ("id3_util") ;

errno_t  id3FrameDestroy P_((Id3Frame frame))
    OCD ("id3_util") ;

errno_t  id3FrameDump P_((FILE *file,
                          const char *indentation,
                          Id3Frame frame))
    OCD ("id3_util") ;

errno_t  id3FrameDumpLUT P_((FILE *file,
                             const char *indentation))
    OCD ("id3_util") ;

extern  errno_t  id3FrameEncode P_((Id3Tag tag,
                                    Id3Frame frame,
                                    MemoryDx mdx,
                                    size_t *length))
    OCD ("id3_util") ;

errno_t  id3FrameErase P_((Id3Frame frame))
    OCD ("id3_util") ;

Id3Value  id3FrameGetValue P_((Id3Frame frame,
                               size_t which))
    OCD ("id3_util") ;

Id3FrameLUTEntry  *id3FrameInfoByID P_((const char *frameID))
    OCD ("id3_util") ;

Id3FrameLUTEntry  *id3FrameInfoByName P_((const char *typeName))
    OCD ("id3_util") ;

Id3FrameLUTEntry  *id3FrameInfoByType P_((Id3FrameType frameType))
    OCD ("id3_util") ;

bool  id3FrameParsePair P_((const char *listSpec,
                            char **value1,
                            size_t *length1,
                            char **value2,
                            size_t *length2,
                            char **next))
    OCD ("id3_util") ;

errno_t  id3FramePVLfromS P_((Id3Frame frame,
                              const char *listSpec,
                              bool replace))
    OCD ("id3_util") ;

char  *id3FramePVLtoS P_((Id3Frame frame))
    OCD ("id3_util") ;

const  char  *id3FrameTypeName P_((Id3FrameType frameType))
    OCD ("id3_util") ;

const  char  *id3FrameUDDescOf P_((Id3FrameType frameType))
    OCD ("id3_util") ;

Id3FrameType  id3FrameUDTypeOf P_((const char *frameID,
                                   const char *description))
    OCD ("id3_util") ;

bool  id3FrameUDUpgrade P_((Id3Frame frame))
    OCD ("id3_util") ;

size_t  id3FrameValueCount P_((Id3Frame frame))
    OCD ("id3_util") ;

/*******************************************************************************
    ID3 Frame Value functions.
*******************************************************************************/

Id3Value  id3ValueCopy P_((Id3Value oldValue))
    OCD ("id3_util") ;

errno_t  id3ValueCreate P_((Id3Value *value))
    OCD ("id3_util") ;

errno_t  id3ValueDestroy P_((Id3Value value))
    OCD ("id3_util") ;

errno_t  id3ValueDump P_((FILE *file,
                          const char *indentation,
                          Id3Value value))
    OCD ("id3_util") ;

errno_t  id3ValueErase P_((Id3Value value))
    OCD ("id3_util") ;


#endif				/* If this file was not INCLUDE'd previously. */
