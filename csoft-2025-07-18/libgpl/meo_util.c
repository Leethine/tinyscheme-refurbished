/* $Id: meo_util.c,v 1.20 2022/12/02 03:25:29 alex Exp alex $ */
/*******************************************************************************

File:

    meo_util.c

    Memory Operations Package.


Author:    Alex Measday


Purpose:

    The MEO_UTIL functions perform various operations on memory regions.

    The meoDump() functions generate VMS-style dumps of arbitrary regions
    of memory.  Each line of output includes the address of the memory being
    dumped, the data (formatted in octal, decimal, or hexadecimal), and the
    ASCII equivalent of the data:

        ...
        00000060:  60616263 64656667 68696A6B 6C6D6E6F  "`abcdefghijklmno"
        00000070:  70717273 74757677 78797A7B 7C7D7E7F  "pqrstuvwxyz{|}~."
        00000080:  80818283 84858687 88898A8B 8C8D8E8F  "................"
        ...

    The MEO_UTIL package also provides a simple means of saving the contents
    of an arbitrary memory region to a file:

        #include  <stdio.h>		-- Standard I/O definitions.
        #include  "meo_util.h"		-- Memory operations.
        char  oldBuffer[1234] ;
        ...
        meoSave (oldBuffer, sizeof oldBuffer, "buffer.dat", 0) ;

    The contents of a file can be loaded into an existing region of memory:

        char  *newBuffer = oldBuffer ;
        int  numBytes = sizeof oldBuffer ;
        ...
        meoLoad ("buffer.dat", 0, &newBuffer, &numBytes) ;

    or into memory dynamically allocated by meoLoad():

        char  *newBuffer = NULL ;
        int  numBytes ;
        ...
        meoLoad ("buffer.dat", 0, &newBuffer, &numBytes) ;
        ... use the new buffer ...
        free (newBuffer) ;


Public Procedures (* defined as macros):

    meoDump() - outputs an ASCII dump of a memory region to a file.
  * meoDumpD() - outputs a decimal ASCII dump to a file.
  * meoDumpO() - outputs an octal ASCII dump to a file.
  * meoDumpT() - outputs a text ASCII dump to a file.
  * meoDumpX() - outputs a hexadecimal ASCII dump to a file.
    meoFromHex() - converts a hexadecimal ASCII string to a byte array.
    meoLoad() - loads the contents of a file into memory.
    meoSave() - saves the contents of memory to a file.
    meoToHex() - converts a byte array to a hexadecimal ASCII string.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* Standard C string functions. */
#ifdef _WIN32
#    include  <io.h>			/* Low-level I/O definitions. */
#endif
#if !defined(HAVE_STAT_H) || HAVE_STAT_H
#    include  <sys/stat.h>		/* File status definitions. */
#endif
#ifndef isascii
#    define  isascii(c)  ((unsigned char) (c) <= 0177)
#endif
#include  "fnm_util.h"			/* Filename utilities. */
#include  "meo_util.h"			/* Memory operations. */


int  meo_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  meo_util_debug

/*!*****************************************************************************

Procedure:

    meoDump ()

    Generate an ASCII Dump of a Memory Region.


Purpose:

    Function meoDump() formats the binary contents of a memory region in ASCII
    and writes the ASCII dump to a file.  Each output line looks as follows:

        <address>:  <data1> <data2> ... <dataN>  "data/ASCII"

    The data items, DATA1, DATA2, etc., are formatted in decimal, hexadecimal,
    or octal notation.


    Invocation:

        status = meoDump (file, indentation, base, numBytesPerLine,
                          address, buffer, numBytesToDump) ;

    where

        <file>			- I
            is the Unix FILE* handle for the output file.  If FILE is NULL,
            the dump is written to standard output.
        <indentation>		- I
            is a text string used to indent each line of the dump.  The
            string is used as the format string in an FPRINTF(3) statement,
            so you can embed anything you want.  This argument can be NULL.
        <base>			- I
            specifies the output base for the dump: "MeoOctal" for octal,
            "MeoDecimal" for decimal, "MeoHexadecimal" for hexadecimal,
            and "MeoText" for text.  (These are enumerated values defined
            in the "meo_util.h" header file.)
        <numBytesPerLine>	- I
            specifies the number of bytes in the buffer to be formatted and
            dumped on a single line of output.  Good values are 8 values per
            line for octal, 8 for decimal, 16 for hexadecimal, and 40 for text.
        <address>		- I
            is the value to be displayed in the address field.  It can be
            different than the actual buffer address.
        <buffer>		- I
            is the buffer of data to be dumped.
        <numBytesToDump>	- I
            is exactly what it says it is!
        <status>		- O
            returns the status of generating the dump, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  meoDump (

#    if PROTOTYPES
        FILE  *file,
        const  char  *indentation,
        MeoBase  base,
        size_t  numBytesPerLine,
        void  *address,
        const  void  *buffer,
        size_t  numBytesToDump)
#    else
        file, indentation, base, numBytesPerLine,
        address, buffer, numBytesToDump)

        FILE  *file ;
        char  *indentation ;
        MeoBase  base ;
        size_t  numBytesPerLine ;
        void  *address ;
        void  *buffer ;
        size_t  numBytesToDump ;
#    endif

{    /* Local variables. */
    char  *out, *outbuf ;
    size_t  numBytesDumped, numBytesThisChunk, numBytesThisLine ;
    unsigned  char  *buf, *inbuf ;




    if (file == NULL)  file = stdout ;


/* Allocate an input buffer to hold the bytes to be dumped.  Doing this
   prevents bus errors should the input buffer be misaligned (i.e., on
   an odd-byte boundary). */

    inbuf = (unsigned char *) malloc (numBytesPerLine) ;
    if (inbuf == NULL) {
        LGE "(meoDump) Error allocating temporary, %d-byte input buffer.\nmalloc: ",
            numBytesPerLine) ;
        return (errno) ;
    }


/* Allocate a string in which to build each output line. */

    outbuf = (char *) malloc (numBytesPerLine * 6) ;
    if (outbuf == NULL) {
        LGE "(meoDump) Error allocating temporary, %d-byte output buffer.\nmalloc: ",
            numBytesPerLine * 6) ;
        return (errno) ;
    }


/*******************************************************************************
    Generate each line of the dump.
*******************************************************************************/

    while (numBytesToDump > 0) {


        numBytesThisLine = (numBytesToDump > numBytesPerLine)
                           ? numBytesPerLine : numBytesToDump ;

        memset (inbuf, '\0', numBytesPerLine) ;		/* Zero trailing bytes. */
        (void) memcpy (inbuf, buffer, numBytesThisLine) ;


/* Output the line indentation and the memory address. */

        if (indentation != NULL)  fprintf (file, "%s", indentation) ;
#ifdef YOU_WANT_VARIABLE_LENGTH_ADDRESS
        fprintf (file, "%p:\t", address) ;
#elif defined(__palmos__)
        HostFPrintF (HostLogFile (), "%08lX: ", (long) address) ;
#elif defined(DATA_MODEL_64_BIT) && (DATA_MODEL_64_BIT == LLP64)
        fprintf (file, "%08llX: ", (long long) address) ;
#else
        fprintf (file, "%08lX: ", (long) address) ;
#endif


/* Format the data in the requested base. */

        buf = (unsigned char *) inbuf ;  numBytesDumped = 0 ;
        out = outbuf ;


        switch (base) {

/* Base 8 - display the contents of each byte as an octal number. */

        case MeoOctal:
            while (numBytesDumped < numBytesPerLine) {
                if (numBytesDumped++ < numBytesThisLine)
                    sprintf (out, " %3.3o", *buf++) ;
                else
                    sprintf (out, " %3s", " ") ;
                out += strlen (out) ;
            }
            break ;

/* Base 10 - display the contents of each byte as a decimal number. */

        case MeoDecimal:
            while (numBytesDumped < numBytesPerLine) {
                if (numBytesDumped++ < numBytesThisLine)
                    sprintf (out, " %3u", *buf++) ;
                else
                    sprintf (out, " %3s", " ") ;
                out += strlen (out) ;
            }
            break ;

/* Base 16 - display the contents of each integer as a hexadecimal number. */

        case MeoHexadecimal:
#define  HEX_CHUNK_SIZE  4	/* Dump data in 4-byte (8-character) groups. */
            while (numBytesDumped < numBytesPerLine) {
                if (numBytesDumped < numBytesThisLine) {
                    numBytesThisChunk = numBytesThisLine - numBytesDumped ;
                    if (numBytesThisChunk > HEX_CHUNK_SIZE)
                        numBytesThisChunk = HEX_CHUNK_SIZE ;
                    *out++ = ' ' ;
                    out += meoToHex (buf, numBytesThisChunk, out, false) ;
                    buf += numBytesThisChunk ;
                    while (numBytesThisChunk++ < HEX_CHUNK_SIZE) {
                        *out++ = ' ' ;  *out++ = ' ' ;
                    }
                    *out = '\0' ;
                } else {
                    strcpy (out, "         ") ;
                    out += strlen (out) ;
                }
                numBytesDumped += HEX_CHUNK_SIZE ;
            }
            break ;

/* "Base 26" - treat the data as ASCII text. */

        case MeoText:
        default:
            break ;

        }


/* Append the ASCII version of the buffer. */

        if (base != MeoText) {
            *out++ = ' ' ;  *out++ = ' ' ;
        }
        *out++ = '"' ;
        (void) memcpy (out, buffer, numBytesThisLine) ;
        numBytesDumped = 0 ;
        while (numBytesDumped++ < numBytesThisLine) {
            unsigned  char  c = *out ;
            if (!isascii (c) || !isprint (c))  *out = '.' ;
            out++ ;
        }
        *out++ = '"' ;  *out++ = '\0' ;


/* Output the dump line to the file. */

#ifdef __palmos__
        HostFPrintF (HostLogFile (), "%s\n", outbuf) ;
#else
        fprintf (file, "%s\n", outbuf) ;
#endif


/* Adjust the pointers and counters for the next line. */

        address = (char *) address + numBytesThisLine ;
        buffer = (char *) buffer + numBytesThisLine ;
        numBytesToDump -= numBytesThisLine ;

    }


/* Deallocate the temporary input/output buffers. */

    free (inbuf) ;
    free (outbuf) ;


    return (0) ;

}

/*!*****************************************************************************

Procedure:

    meoFromHex ()

    Convert Hexadecimal ASCII String to Byte Array.


Purpose:

    Function meoFromHex() converts a hexadecimal ASCII string to the
    corresponding array of bytes, with each byte's binary value taken
    from two hexadecimal characters in the string.  For example, the
    following code:

        const  char  *string = "123456789ABCDEF0" ;

        unsigned  char  byteArray[sizeof string / 2] ;

        meoFromHex (string, -1, byteArray) ;

    produces the following sequence of byte values:

        0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0


    Invocation:

        numBytes = meoFromHex (string, length, bytes) ;

    where:

        <string>	- I
            is a string containing the hexadecimal ASCII representation of
            a sequence of bytes.  Two hexadecimal characters specify the
            upper and lower nibbles of a byte's value.
        <length>	- I
            is the length of the string; supply a length of -1 to have
            meoFromHex() determine the length of the string.
        <bytes>		- O
            is the array of bytes into which the converted values will be
            written.  Since two characters in the hexadecimal string map to
            a single byte, the length of the byte array must, at a minimum,
            be half the length of the string.
        <numBytes>	- O
            returns the actual number of bytes converted.  The conversion
            scan may be terminated prematurely if a non-hexadecimal ASCII
            character is encountered in the string or if the string consists
            of an odd number of hexadecimal characters.

*******************************************************************************/


size_t  meoFromHex (

#    if PROTOTYPES
        const  char  *string,
        ssize_t  length,
        void  *bytes)
#    else
        string, length, bytes)

        char  *string ;
        ssize_t  length ;
        void  *bytes ;
#    endif

{    /* Local variables. */
    size_t  i, maxBytes ;
    unsigned  char  *b = bytes ;



    if ((string == NULL) || (bytes == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(meoFromHex) NULL string or byte array.\n") ;
        return (0) ;
    }

    maxBytes = ((length < 0) ? strlen (string) : length) / 2 ;

/* Convert each pair of hexadecimal ASCII characters in the string to the
   pair's corresponding byte value. */

    for (i = 0 ;  i < maxBytes ;  i++) {
        unsigned  char  c0 = string[i*2], c1 = string[i*2+1] ;

        if (!isxdigit (c0) || !isxdigit (c1)) {
            SET_ERRNO (EINVAL) ;
            LGE "(meoFromHex) Invalid hexadecimal coding at \"%.2s\".\n",
                &string[i*2]) ;
            return (i) ;
        }

        b[i] = isdigit (c0) ? (c0 - '0') : ((toupper (c0) - 'A') + 10) ;
        b[i] = (b[i] << 4)
               |  (isdigit (c1) ? (c1 - '0') : ((toupper (c1) - 'A') + 10)) ;

    }

    return (i) ;

}

/*!*****************************************************************************

Procedure:

    meoLoad ()

    Load Memory from a File.


Purpose:

    Function meoLoad() loads the binary contents of a memory region from a
    disk file.  The contents must have been previously saved using meoSave().


    Invocation (dynamically allocated buffer):

        void  *startAddress = NULL ;
        ...
        status = meoLoad (fileName, offset, &startAddress, &numBytes) ;

    Invocation (caller-specified buffer):

        void  *startAddress = buffer ;
        long  numBytes = sizeof buffer ;
        ...
        status = meoLoad (fileName, offset, &startAddress, &numBytes) ;

    where:

        <fileName>	- I
            is the name of the file from which the memory contents will be
            loaded.  Environment variables may be embedded in the file name.
        <offset>	- I
            is the byte offset within the file from which the load will begin.
        <startAddress>	- I/O
            is the address of a (VOID *) pointer that specifies or returns the
            address where the contents of the file will be stored.  If the
            (VOID *) pointer is NULL, meoLoad() will MALLOC(3) a buffer for
            the file contents and return its address through this argument;
            the caller is responsible for FREE(3)ing the memory when it is
            no longer needed.  If the (VOID *) pointer is NOT NULL, meoLoad()
            uses it as the address of a caller-allocated buffer in which the
            file contents are to be stored; the size of the buffer is
            specified by the NUMBYTES argument.
        <numBytes>	- I/O
            is the address of a longword that specifies or returns the size of
            the memory buffer.  If the memory buffer is dynamically allocated
            by meoLoad(), this argument returns the size of the buffer.  If
            meoLoad() uses a caller-allocated buffer, this argument specifies
            the size of the buffer.
        <status>	- O
            returns the status of loading the memory from a file, zero
            if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  meoLoad (

#    if PROTOTYPES
        const  char  *fileName,
        long  offset,
        void  **startAddress,
        size_t  *numBytes)
#    else
        fileName, offset, startAddress, numBytes)

        char  *fileName ;
        long  offset ;
        void  **startAddress ;
        size_t  *numBytes ;
#    endif

{    /* Local variables. */
    char  *pathname ;
    FILE  *file ;
    size_t  fileSize ;
#if !defined(HAVE_STAT_H) || HAVE_STAT_H
    struct  stat  info ;
#endif



/* Open the input file. */

    pathname = fnmBuild (FnmPath, fileName, NULL) ;
    if (pathname == NULL) {
        LGE "(meoLoad) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
        return (errno) ;
    }

    file = fopen (fileName, "rb") ;
    if (file == NULL) {
        LGE "(meoLoad) Error opening %s.\n", pathname) ;
        PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

    free (pathname) ;

/* Determine the amount of data to be loaded from the file. */

#if defined(HAVE_STAT_H) && !HAVE_STAT_H
#    warning  meoLoad: No fstat(2); file size defaults to 1024.
    fileSize = 1024 ;			/* Until I figure out another way ... */
#else
    if (fstat (fileno (file), &info)) {
        LGE "(meoLoad) Error determining the size of %s.\n", fileName) ;
        return (errno) ;
    }

    fileSize = info.st_size - offset ;
#endif

/* Allocate a memory buffer, if necessary. */

    if (*startAddress == NULL) {
        *numBytes = fileSize ;
        *startAddress = malloc (fileSize) ;
        if (*startAddress == NULL) {
            LGE "(meoLoad) Error allocating %lu-byte memory buffer for %s.\n",
                (unsigned long) fileSize, fileName) ;
            return (errno) ;
        }
    }

/* Read the (possibly truncated) contents of the file into the memory pool. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  meoLoad: No fseek(2); offset ignored.
#else
    if (fseek (file, offset, SEEK_SET) != offset) {
        LGE "(meoLoad) Error positioning to offset %ld in %s.\nfseek: ",
            offset, fileName) ;
        return (errno) ;
    }
#endif

    if (fileSize > *numBytes)  fileSize = *numBytes ;

    if (fread (*startAddress, fileSize, 1, file) != 1) {
        LGE "(meoLoad) Error loading %lu bytes from %s to %p.\nfread: ",
            (unsigned long) fileSize, fileName, *startAddress) ;
        return (errno) ;
    }

/* Close the file. */

    fclose (file) ;

    LGI "(meoLoad) Loaded %lu bytes from %s to %p.\n",
        (unsigned long) fileSize, fileName, *startAddress) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    meoSave ()

    Save Memory to a File.


Purpose:

    Function meoSave() saves the binary contents of a memory region to a
    disk file.  The contents can be reloaded at a later time using meoLoad().


    Invocation:

        status = meoSave (startAddress, numBytes, fileName, offset) ;

    where:

        <startAddress>	- I
            specifies the start of the memory region that is to be saved.
        <numBytes>	- I
            specifies the number of bytes of data to save.
        <fileName>	- I
            is the name of the file to which the memory contents will be
            written.  Environment variables may be embedded in the file name.
        <offset>	- I
            is the byte offset within the file at which the save will begin.
        <status>	- O
            returns the status of saving the memory to a file, zero if
            there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  meoSave (

#    if PROTOTYPES
        void  *startAddress,
        size_t  numBytes,
        const  char  *fileName,
        long  offset)
#    else
        startAddress, numBytes, fileName, offset)

        void  *startAddress ;
        size_t  numBytes ;
        char  *fileName ;
        long  offset ;
#    endif

{    /* Local variables. */
    bool  exists ;
    char  *pathname ;
    FILE  *file ;



/* Open the output file.  Due to the way file streams work in ANSI C, some
   care must be taken with the mode in which the file is opened.  If the file
   already exists, open the file in read/write mode so that meoSave() can
   position the write pointer to an arbitrary location in the file.  If the
   file does NOT exist, open the file in write mode so that an empty file is
   created; positioning to an arbitrary location will then be simulated by
   explicitly writing zeroes to the file prior to the desired location. */

    pathname = fnmBuild (FnmPath, fileName, NULL) ;
    if (pathname == NULL) {
        LGE "(meoSave) Error parsing \"%s\".\nfnmBuild: ", fileName) ;
        return (errno) ;
    }

    exists = fnmExists (pathname) ;	/* Existence of file affects open mode. */

    file = fopen (pathname, exists ? "r+b" : "wb") ;
    if (file == NULL) {
        LGE "(meoSave) Error opening \"%s\" to save memory at %p.\n",
            pathname, startAddress) ;
        PUSH_ERRNO ;  free (pathname) ;  POP_ERRNO ;
        return (errno) ;
    }

    free (pathname) ;

/* Position to the specified location in the output file.  Due to the way
   file streams work in ANSI C, either of two methods is employed depending
   on whether the file already exists.  If the file exists, a straightforward
   seek to the location is possible.  If this is a new file (i.e., an empty
   file), zeroes must be written to the file up until the desired location. */

#if defined(HAVE_FSEEK) && !HAVE_FSEEK
#    warning  meoSave: No fseek(2); offset ignored.
#else
    if (exists && (offset > 0)) {
				/* Existing file; seek to offset. */
        if (fseek (file, offset, SEEK_SET) < 0) {
            LGE "(meoSave) Error positioning to offset %ld in %s.\nfseek: ",
                offset, fileName) ;
            PUSH_ERRNO ;  fclose (file) ;  POP_ERRNO ;
            return (errno) ;
        }
    } else if (!exists && (offset > 0)) {
        size_t  i ;		/* New file; simulate seek by writing zeroes
				   at beginning of file. */
        for (i = 0 ;  i < (size_t) offset ;  i++) {
            char  zero = 0 ;	/* ANSI C can be excruciating! */
            if (fwrite (&zero, 1, 1, file) != 1) {
                LGE "(meoSave) Error writing %ld zeroes at beginning of %s.\n",
                    offset, fileName) ;
                PUSH_ERRNO ;  fclose (file) ;  POP_ERRNO ;
                return (errno) ;
            }
        }
    }
#endif

/* Write the contents of the memory out to the file. */

    if (fwrite (startAddress, numBytes, 1, file) != 1) {
        LGE "(meoSave) Error saving %lu bytes at %p to %s.\n",
            (unsigned long) numBytes, startAddress, fileName) ;
        PUSH_ERRNO ;  fclose (file) ;  POP_ERRNO ;
        return (errno) ;
    }

/* Close the file. */

    if (fclose (file)) {
        LGE "(meoSave) Error closing %s for memory at %p.\n",
            fileName, startAddress) ;
        return (errno) ;
    }

    LGI "(meoSave) Saved %lu bytes at %p to %s\n",
        (unsigned long) numBytes, startAddress, fileName) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    meoToHex ()

    Convert Byte Array to Hexadecimal ASCII String.


Purpose:

    Function meoToHex() converts an array of bytes to the hexadecimal ASCII
    string representation of the data, with 2 hexadecimal characters for each
    byte.  For example, the following code:

        unsigned  char  byteArray[] = { 0x12, 0x34, 0x56, 0x78,
                                        0x9A, 0xBC, 0xDE, 0xF0 } ;

        char  string[(sizeof byteArray * 2) + 1] ;

        meoToHex (byteArray, sizeof byteArray, string, true) ;

    would produce the following, NUL-terminated ASCII string:

        "123456789ABCDEF0"


    Invocation:

        length = meoToHex (bytes, numBytes, string, addNUL) ;

    where:

        <bytes>		- I
            is the array of bytes whose binary values are to be converted.
        <numBytes>	- I
            is the number of bytes in the array.
        <string>	- O
            is the address of a string buffer in which the hexadecimal ASCII
            string is to be written.  Two ASCII characters are needed to
            represent the value of a single byte, so the string buffer must
            be large enough to hold twice the number of characters as there
            are bytes in the array (plus one character for the terminating
            NUL if desired).
        <addNUL>	- I
            specifies whether (true) or not (false) a NUL character should be
            appended to the hexadecimal ASCII string.
        <length>	- O
            returns the number of hexadecimal ASCII characters written to the
            string.  This value will always be twice the number of bytes in
            the array and thus exclude the NUL terminator if added.

*******************************************************************************/


size_t  meoToHex (

#    if PROTOTYPES
        const  void  *bytes,
        size_t  numBytes,
        char  *string,
        bool  addNUL)
#    else
        bytes, numBytes, string, addNUL)

        void  *bytes ;
        size_t  numBytes ;
        char  *string ;
        bool  addNUL ;
#    endif

{    /* Local variables. */
    char  *s = string ;
    size_t  i ;
    const  unsigned  char  *b = bytes ;
    static  const  char  *hexToASCII = "0123456789ABCDEF" ;



    if ((bytes == NULL) || (string == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(meoToHex) NULL byte array or string.\n") ;
        return (0) ;
    }

/* Convert each binary byte value to its two-character hexadecimal
   ASCII representation. */

    for (i = 0 ;  i < numBytes ;  i++) {
        *s++ = hexToASCII[(b[i] >> 4) & 0x0F] ;
        *s++ = hexToASCII[b[i] & 0x0F] ;
    }

/* Add a NUL terminator to the string if requested. */

    if (addNUL)  *s = '\0' ;

    return (s - string) ;

}

#ifdef  TEST

/*******************************************************************************

    Program to test the MEO_UTIL() functions.

    Under UNIX,
        compile and link as follows:
            % cc -g -DTEST meo_util.c -I<... includes ...> <libraries ...>
        and run with the following command line:
            % a.out <wildcard_file_spec>

    Under VxWorks,
        compile and link as follows:
            % cc -g -c -DTEST -DVXWORKS meo_util.c -I<... includes ...> \
                       -o test_drs.o
            % ld -r test_drs.o <libraries ...> -o test_drs.vx.o
        load as follows:
            -> ld <test_drs.vx.o
        and run with the following command line:
            -> test_drs.vx.o "<wildcard_file_spec>"

*******************************************************************************/

#ifdef VXWORKS

test_meo (fileName)
    char  *fileName ;
{    /* Local variables. */

#else

int  main (argc, argv)
    int  argc ;
    char  *argv[] ;
{    /* Local variables. */
    char  *fileName = (argc > 1) ? argv[1] : NULL ;

#endif

    unsigned  char  buffer[256] ;
    size_t  i, numBytes ;
    void  *newBuffer ;




    aperror_print = 1 ;
    meo_util_debug = 1 ;

    for (i = 0 ;  i < sizeof buffer ;  i++)
        buffer[i] = i ;

    if (fileName == NULL) {
        newBuffer = buffer ;
        numBytes = sizeof buffer ;
    } else {
		/* Save 256-byte buffer starting at 128th byte in file. */
        meoSave (buffer, sizeof buffer, fileName, 128) ;
        newBuffer = NULL ;
        meoLoad (fileName, 0, &newBuffer, &numBytes) ;
    }

    printf ("\nOctal Dump:\n\n") ;
    meoDumpO (NULL, "    ", 0, newBuffer, numBytes) ;
    printf ("\nDecimal Dump:\n\n") ;
    meoDumpD (NULL, "    ", 0, newBuffer, numBytes) ;
    printf ("\nHexadecimal Dump:\n\n") ;
    meoDumpX (NULL, "    ", 0, newBuffer, numBytes) ;
    printf ("\nText Dump:\n\n") ;
    meoDumpT (NULL, "    ", 0, newBuffer, numBytes) ;

  { char  bytes[64], string[128] ;
    size_t  length ;

    printf ("\n") ;
    strcpy (string, "123456789AbCdEf0") ;
    length = meoFromHex (string, -1, bytes) ;
    printf ("meoFromHex(\"%s\") = %lu:\n",
            string, (unsigned long) length) ;
    printf ("    0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\n",
            (unsigned char) bytes[0], (unsigned char) bytes[1],
            (unsigned char) bytes[2], (unsigned char) bytes[3],
            (unsigned char) bytes[4], (unsigned char) bytes[5],
            (unsigned char) bytes[6], (unsigned char) bytes[7]) ;

    printf ("\n") ;
    memset (string, 0x5A, sizeof string) ;
    length = meoToHex (bytes, 8, string, true) ;
    printf ("meoToHex() = %lu: \"%s\"\n", (unsigned long) length, string) ;

  }

}
#endif
