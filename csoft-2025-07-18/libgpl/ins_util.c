/* $Id: ins_util.c,v 1.9 2004/06/21 20:06:31 alex Exp alex $ */
/*******************************************************************************

File:

    ins_util.c

    IN-SNEC CORTEX Utilities.


Author:    Alex Measday


Purpose:

    The INS_UTIL package provides a simplified means of communicating with an
    IN-SNEC CORTEX Command, Ranging, and Telemetry Unit over TCP/IP network
    connections.  Layered on top of the lower-level TCP_UTIL package, the
    INS_UTIL package can be used for both the VME- and NT-based CORTEX units.

    A CORTEX message is composed of a fixed-length message header, a
    variable-length message body, and a fixed-length message trailer; one of
    the fields in the header specifies the message length (header, body, and
    trailer).  The INS_UTIL functions are used to read messages from a stream
    and to write messages to a stream.

    A CORTEX stream is created on a previously established network connection.
    As a result, the INS_UTIL package can be used for implementing both client
    and server CORTEX applications.  The following code fragment connects to
    the logging port on a CORTEX unit:

        #include  <stdio.h>			-- Standard I/O definitions.
        #include  "tcp_util.h"			-- TCP/IP networking utilities.
        #include  "ins_util.h"			-- IN-SNEC CORTEX utilities.

        char  name[64] ;
        CortexStream  stream ;
        in_port_t  port ;
        TcpEndpoint  connection ;
        ...
        insPort ("LOG", 0, &port) ;
        sprintf (name, "%hu@<host>", port) ;
        tcpCall (name, 0, &connection) ;	-- Call LOG unit ("3040@<host>").
        insCreate (connection, &stream) ;	-- Create CORTEX stream.

    The insPort() function used above provides a convenient means for
    determining the CORTEX ports by name; insPortName() is available for
    performing the reverse translation.

    insRead() reads the next message (header, body, and trailer) from a CORTEX
    stream:

        CortexHeader  header ;
        char  *body ;
        ...
        insRead (stream, -1.0, &header, &body) ;

    insRead() fills in the header and returns a pointer to the message body
    (which is stored in memory private to the INS_UTIL package); the "length"
    field in the header contains the length in bytes of the message body
    (excluding the header and trailer).

    Before writing a message to a CORTEX stream, the application must fill in
    the header with the length of the message body (excluding header and
    trailer) and the desired flow ID; insWrite() is then called to output the
    message to the stream:

        #include  <stdlib.h>			-- Standard C Library definitions.
        char  body[100] ;
        ...
        ... fill in the message body ...
        ...
        header.length = sizeof body ;
        header.flowID = 0 ;
        insWrite (stream, -1.0, &header, body) ;

    insWrite() automatically adds the header and trailer lengths to the length
    field in the header of the outgoing message.  Both insRead() and insWrite()
    take a timeout argument that allows the application to limit the amount of
    time these routines wait to perform their respective functions.

    The CORTEX unit transmits and receives data in XDR format; consequently,
    the data must be converted to and from the client's host-machine format.
    insRead() and insWrite() take care of decoding and encoding, respectively,
    the fields in the caller's header structure.  The message body is another
    matter.  The VME-based CORTEX only uses integers in its messages, so
    a blanket conversion of each 32-bit quantity in a message body would
    probably suffice and could conceivably be done automatically by insRead()
    and insWrite().  The NT-based CORTEX, however, has some single-precision
    floating-point fields to which xdr_float(3) must be applied.  Therefore,
    the INS_UTIL utilities leave it to the application to perform the
    appropriate conversions, although not without without a little help.

        NOTE:  The XDR representations of a host CPU's "int"s and "long"s
        are both 32 bits wide.  To avoid problems with "int"s less than
        32 bits wide and "long"s greater than 32 bits wide, use C's data
        type "int32_t" for variables and xdr_int32_t(3) to encode/decode
        these variables.

    insDecode() converts data received on a CORTEX stream from XDR format to
    host-machine format by applying an XDR conversion function to the input
    data.  The conversion function can be for a user-defined structure that
    encompasses the entire message body:

        extern  xdrproc_t  xdr_TelemetryFrame ;
        char  *body ;
        CortexHeader  header ;
        TelemetryFrame  frame ;
        ...
        insRead (stream, -1.0, &header, &body) ;
        insDecode (body, xdr_TelemetryFrame, 0, header.length, &frame) ;
        ...
        ... converted fields stored in FRAME ...
        ...

    or for a primitive data type that is repeated throughout the message
    body:

        int32_t  numbers[256] ;
        ...
        insDecode (body, xdr_int32_t, BYTES_PER_XDR_UNIT, header.length,
                   &numbers) ;
        ...
        ... header.length/4 integers are converted and stored in NUMBERS ...
        ...

    (If the widths of the host-machine data types are identical to the widths
    of the corresponding XDR types, the conversion could take place in the same
    memory buffer.)

    insEncode() is insDecode()'s counterpart for converting data from
    host-machine format to XDR format before calling insWrite() to output
    the message on a CORTEX stream:

        char  body[sizeof (int32_t) * 256] ;
        int32_t  numbers[256] ;
        ...
        ... fill in NUMBERS array ...
        ...
        header.length = insEncode (numbers, xdr_int32_t, BYTES_PER_XDR_UNIT,
                                   sizeof body, body) ;
        header.flowID = 0 ;
        insWrite (stream, -1.0, &header, body) ;

    The actual frame data in a telemetry message, being a byte sequence, can
    be used directly as received from the CORTEX unit.

    The integrity of CORTEX TC (telecommand) request and instruction messages
    is guaranteed (more or less!) by a checksum computed and appended to
    the messages.  The INS_UTIL package expects the application to handle
    checksum processing.  Before calling insWrite() to send a TC message,
    the application must encode the message body (excluding the checksum)
    in XDR format, compute the checksum, and add it to the message body in
    network byte order.  The following code fragment sends an "Execute"
    instruction to the CORTEX unit:

        char  body[5 * sizeof (int32_t)] ;
        int32_t  checksum, numbers[4] ;
        size_t  i = 0 ;
        ...
        numbers[i++] = 4 ;			-- "Execute" request code.
        numbers[i++] = 1234 ;			-- # of Execute pulses.
        numbers[i++] = 100000 ;			-- Execute pulse width.
        numbers[i++] = 10000 ;			-- Execute pulse period.

        header.length = insEncode (numbers, xdr_int32_t, BYTES_PER_XDR_UNIT,
                                   i * sizeof (int32_t), body) ;
        header.flowID = <TC flow ID> ;

        checksum = insChecksum (&header, body) ;
        *((uint32_t *) &body[header.length]) = htonl (checksum) ;
        header.length += BYTES_PER_XDR_UNIT ;

        insWrite (stream, -1.0, &header, body) ;

    After calling insRead() to input a TC message, the application must compute
    the checksum for the XDR-encoded message body (excluding the checksum) and
    compare the checksum to the last integer (converted to host byte order) in
    the body.

        char  *body ;
        int32_t  numbers[256] ;
        ...
        insRead (stream, -1.0, &header, &body) ;

        if (insChecksum (&header, body) !=
            ntohl (*((int32_t *) &body[header.length - BYTES_PER_XDR_UNIT]))) {
            ... checksum error ...
        }

        insDecode (body, xdr_int32_t, BYTES_PER_XDR_UNIT, header.length,
                   &numbers) ;

    In event-driven applications (e.g., those based on the X Toolkit or the
    IOX dispatcher), the socket connection underlying the CORTEX stream,
    returned by insFd(), can be monitored for input by your event dispatcher.
    Because input is buffered, the input callback must repeatedly call
    insRead() while insIsReadable() is true.

    Configuration and monitoring messages sent to and received from a CORTEX
    unit contain a numeric code identifying the CORTEX component or table with
    which the message is concerned.  insPart() and insPartName(), respectively,
    provide a convenient means of converting a component/table name to a
    numeric code and vice-versa.  Identically named components have different
    numeric codes in the VME- and NT-based CORTEX units, so the type of the
    CORTEX unit must be passed to insPart().

    When a CORTEX stream is no longer needed, a single call will close the
    network connection and discard the stream:

        insDestroy (stream) ;


Public Procedures:

    insChecksum() - compute the checksum for a message.
    insCreate() - creates a CORTEX network stream.
    insDecode() - decodes XDR-encoded data.
    insDestroy() - deletes a CORTEX network stream.
    insEncode() - encodes data in XDR format.
    insFd() - returns a CORTEX stream's socket number.
    insIsReadable() - checks if input is waiting to be read from a stream.
    insIsUp() - checks if a CORTEX stream is up.
    insIsWriteable() - checks if data can be written to a stream.
    insName() - returns the name of a CORTEX stream.
    insPart() - converts a component name to a component code.
    insPartName() - converts a component code to a component name.
    insPort() - converts a port name to a port number.
    insPortName() - converts a port number to a port name.
    insRead() - reads the next message from a CORTEX stream.
    insTime() - converts a CORTEX time tag to a UNIX timeval.
    insWrite() - writes a message to a CORTEX stream.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "bio_util.h"			/* Buffered I/O streams. */
#include  "tcp_util.h"			/* TCP/IP networking utilities. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#include  "ins_util.h"			/* IN-SNEC CORTEX utilities. */


/*******************************************************************************
    CORTEX Stream - contains information about a CORTEX network stream,
        including the underlying network connection and other attributes.
*******************************************************************************/

#define  MAX_MSG_SIZE  (128UL*1024UL)	/* Arbitrarily chosen! */

typedef  struct  _CortexStream {
    TcpEndpoint  connection ;		/* TCP/IP connection. */
    BioStream  input ;			/* Buffered input stream. */
    char  *inputMessage ;		/* Last message read. */
    size_t  inputLength ;		/* Allocated size of input message. */
    char  *outputMessage ;		/* Working storage for insWrite(). */
    size_t  outputLength ;		/* Allocated size of output message. */
}  _CortexStream ;


int  ins_util_debug = 0 ;		/* Global debug switch (1/0 = yes/no). */
#undef  I_DEFAULT_GUARD
#define  I_DEFAULT_GUARD  ins_util_debug

					/* Size of XDR-encoded header (3 int's). */
#define  HEADER_SIZE  (3UL * BYTES_PER_XDR_UNIT)
					/* Size of XDR-encoded trailer (1 int). */
#define  TRAILER_SIZE  (1UL * BYTES_PER_XDR_UNIT)

#define  DESTROY(stream) \
    (insDestroy (stream), stream = NULL)

/*******************************************************************************
    Component Name/Code Tables - map component names to codes and vice-versa.
*******************************************************************************/

typedef  struct  ComponentInfo {
    int32_t  code ;
    const  char  *name ;
}  ComponentInfo ;

static  const  ComponentInfo  componentInfoNT[] = {
  { INT32_C(0x0000), "Version" },
  { INT32_C(0x1001), "Global Table" },
  { INT32_C(0x1010), "TCU" },
  { INT32_C(0x1020), "TMS" },
  { INT32_C(0x1034), "RAU" },
  { INT32_C(0x1040), "IFM-1" },
  { INT32_C(0x1041), "IFM-2" },
  { INT32_C(0x1050), "IFR-1" },
  { INT32_C(0x1051), "IFR-2" },
  { INT32_C(0x1052), "IFR-3" },
  { INT32_C(0x1060), "TMU-A" },
  { INT32_C(0x1061), "TMU-B" },
  { INT32_C(0x1062), "TMU-C" },
  { INT32_C(0x1063), "TMU-D" },
  { INT32_C(0x1064), "TMU-E" },
  { INT32_C(0x1065), "TMU-F" },
  { INT32_C(0x1070), "Noise Generator" },
  { INT32_C(0x1080), "Project-specific" },
  { INT32_C(0x1100), "All Tables" },
						/* Abbreviations. */
  { INT32_C(0x1001), "Global" },
  { INT32_C(0x1070), "Noise" },
  { INT32_C(0x1080), "Project" },
  { INT32_C(0x1100), "All" },
  { INT32_C(0), NULL }				/* Last entry in table. */
} ;

static  const  ComponentInfo  componentInfoVME[] = {
  { INT32_C(0x0000), "Version" },
  { INT32_C(0x0001), "Global Table" },
  { INT32_C(0x0010), "TCU" },
  { INT32_C(0x0020), "TMS" },
  { INT32_C(0x0030), "RAU-1" },
  { INT32_C(0x0034), "RAU-2" },
  { INT32_C(0x0040), "IFM" },
  { INT32_C(0x0050), "PMR-A" },
  { INT32_C(0x0051), "PMR-B" },
  { INT32_C(0x0052), "PMR-C" },
  { INT32_C(0x0060), "TMU-A" },
  { INT32_C(0x0061), "TMU-B" },
  { INT32_C(0x0062), "TMU-C" },
  { INT32_C(0x0070), "Noise Generator" },
  { INT32_C(0x0080), "Project-specific" },
  { INT32_C(0x0100), "All Tables" },
						/* Abbreviations. */
  { INT32_C(0x0001), "Global" },
  { INT32_C(0x0070), "Noise" },
  { INT32_C(0x0080), "Project" },
  { INT32_C(0x0100), "All" },
  { INT32_C(0), NULL }				/* Last entry in table. */
} ;

/*******************************************************************************
    Port Name/Number Tables - map port names to numbers and vice-versa.
*******************************************************************************/

typedef  struct  PortInfo {
    long  port ;
    const  char  *name ;
}  PortInfo ;

static  const  PortInfo  portInfo[] = {
						/* Common ports. */
  { 3000, "MON" },
  { 3001, "CTRL" },
  { 3002, "RST" },
  { 3020, "TC" },
  { 3035, "MEAS" },
  { 3040, "LOG" },
  { ~0, "All" },
						/* NT-specific ports. */
  { 3021, "SIM" },
  { 3034, "RNG" },
  { 3050, "SPA" },
  { 3060, "DOP" },
  { 3070, "TM" },
						/* VME-specific ports. */
  { 3010, "TMA-R" },
  { 3011, "TMA-H" },
  { 3012, "TMB-R" },
  { 3013, "TMB-H" },
  { 3014, "TMC-R" },
  { 3015, "TMC-H" },
  { 3021, "TMS" },
  { 3030, "RNG-1" },
  { 3034, "RNG-2" },
  { 0, NULL }				/* Last entry in table. */
} ;

/*!*****************************************************************************

Procedure:

    insChecksum ()

    Compute the Checksum for a CORTEX Message.


Purpose:

    Function insChecksum() computes the checksum for a CORTEX message
    (in particular, telecommand messages) that is encoded in XDR format.


    Invocation:

        checksum = insChecksum (&header, &body) ;

    where

        <header>	- I
            is a pointer to the CORTEX header for the message.  The numeric
            fields in the header should be in host-byte order.  The caller
            is responsible for setting the length and the flow ID in the
            header.  NOTE that the caller-specified length is the length
            of the XDR-encoded message body *only* and excludes the header,
            checksum, and postamble.  NOTE also that the length is assumed
            to be an exact multiple of 4 (the size of an XDR unit).
        <body>		- I
            is the XDR-encoded message body, the length of which is
            specified in the message header.  NULL can be specified
            if there is no message body.
        <checksum>
            returns the 2's complement of the 32-bit checksum of the message.

*******************************************************************************/


int32_t  insChecksum (

#    if PROTOTYPES
        const  CortexHeader  *header,
        const  char  *body)
#    else
        header, body)

        CortexHeader  *header ;
        char  *body ;
#    endif

{    /* Local variables. */
    int32_t  checksum, *value ;
    size_t  i, numIntegers ;



/* Initialize the checksum to the contents of the message header: the full
   message length (header, body, checksum, and trailer) plus the flow ID. */

    checksum = InsPreamble + InsPostamble +	/* Cancel each other out! */
               HEADER_SIZE + header->length +
               BYTES_PER_XDR_UNIT + TRAILER_SIZE +
               header->flowID ;

/* Add in the 32-bit chunks from the message body. */

    if (body != NULL) {
        value = (int32_t *) body ;
        numIntegers = header->length / BYTES_PER_XDR_UNIT ;
        for (i = 0 ;  i < numIntegers ;  i++) {
            checksum += (int32_t) ntohl (value[i]) ;
        }
    }

/* Return the 2's complement of the checksum. */

    return (0 - checksum) ;

}

/*!*****************************************************************************

Procedure:

    insCreate ()

    Create a CORTEX Stream.


Purpose:

    Function insCreate() creates a CORTEX communications stream on top of
    a previously-created network connection.


    Invocation:

        status = insCreate (connection, &stream) ;

    where

        <connection>	- I
            is the previously-created network endpoint for the underlying
            network connection.  (See "tcp_util.c" for more information
            about network endpoints.)  NOTE that the "connection" endpoint
            is automatically destroyed (i.e., the socket is closed) when
            the CORTEX stream is destroyed.
        <stream>	- O
            returns a handle for the new CORTEX stream.  This handle is used
            in calls to the other INS functions.
        <status>	- O
            returns the status of creating the stream, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  insCreate (

#    if PROTOTYPES
        TcpEndpoint  connection,
        CortexStream  *stream)
#    else
        connection, stream)

        TcpEndpoint  connection ;
        CortexStream  *stream ;
#    endif

{

/* Create and initialize a CORTEX stream structure for the network
   connection. */

    *stream = (_CortexStream *) malloc (sizeof (_CortexStream)) ;
    if (*stream == NULL) {
        LGE "(insCreate) Error allocating stream structure for \"%s\".\nmalloc: ",
            tcpName (connection)) ;
        return (errno) ;
    }

    (*stream)->connection = connection ;
    (*stream)->inputMessage = NULL ;
    (*stream)->inputLength = 0 ;
    (*stream)->outputMessage = NULL ;
    (*stream)->outputLength = 0 ;

/* Buffer input on the network connection. */

    if (bioCreate (connection, (BioInputF) tcpRead, 0, NULL, 0,
                   &(*stream)->input)) {
        LGE "(insCreate) Error creating buffered input stream for \"%s\".\nbioCreate: ",
            tcpName (connection)) ;
        PUSH_ERRNO ;  insDestroy (*stream) ;  POP_ERRNO ;
        return (errno) ;
    }

    LGI "(insCreate) Created CORTEX network stream %s, socket %d.\n",
        insName (*stream), (int) insFd (*stream)) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    insDecode ()

    Decode XDR-encoded Data.


Purpose:

    Function insDecode() decodes XDR-encoded data received in a CORTEX
    message.


    Invocation:

        decoded = insDecode (buffer, decodeF, step, length, data) ;

    where

        <buffer>	- I
            is the address of a buffer of XDR-encoded data.  The length of
            this buffer is specified by the LENGTH argument below.
        <decodeF>	- I
            is the address of an XDR function used to decode the data; e.g.,
            xdr_long() or xdr_float().  The type of such a function, xdrproc_t,
            is defined in "rpc/xdr.h":
                bool_t  xdr_XYZ (XDRS *handle, void *data) ;
        <step>		- I
            specifies the stride (in bytes) to use for the resulting data
            when the decode function is repeatedly applied to the input
            buffer.  If STEP is zero, the decode function is applied once.
            If STEP is greater than zero, the decode function is repeatedly
            applied to the input buffer until the buffer is exhausted.
            On each iteration, the pointer into the resulting data is
            incremented by STEP, so STEP should normally be "sizeof (XYZ)",
            where XYZ is the data type being decoded.  (Consequently,
            variable-length data types cannot be handled by insDecode().)
        <length>	- I
            is the length (in bytes) of the XDR-encoded data (see BUFFER).
        <data>		- O
            is the address of a buffer into which the decoded data will be
            written.
        <decoded>	- O
            returns the number of bytes in the input buffer that were
            scanned and decoded.

*******************************************************************************/


size_t  insDecode (

#    if PROTOTYPES
        const  char  *buffer,
        const  xdrproc_t  decodeF,
        size_t  step,
        size_t  length,
        char  *data)
#    else
        buffer, decodeF, step, length, data)

        char  *buffer ;
        xdrproc_t  decodeF ;
        size_t  step ;
        size_t  length ;
        char  *data ;
#    endif

{    /* Local variables. */
    XDR  memoryStream ;



/* Create a memory-based XDR stream for the input buffer. */

    xdrmem_create (&memoryStream, (char *) buffer, (u_int) length, XDR_DECODE) ;

/* Convert the XDR-encoded data to the host representation. */


    for ( ; ; ) {
        if (!decodeF (&memoryStream, data))  break ;
        if (step == 0)  break ;
        data = data + step ;
    }

/* Return the number of input bytes consumed. */

    return (xdr_getpos (&memoryStream)) ;

}

/*!*****************************************************************************

Procedure:

    insDestroy ()

    Delete a CORTEX Stream.


Purpose:

    Function insDestroy() destroys a CORTEX stream.  The underlying network
    connection is closed and the CORTEX stream structure is deallocated.


    Invocation:

        status = insDestroy (stream) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <status>	- O
            returns the status of deleting the stream, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  insDestroy (

#    if PROTOTYPES
        CortexStream  stream)
#    else
        stream)

        CortexStream  stream ;
#    endif

{

    if (stream == NULL)  return (0) ;

    LGI "(insDestroy) Closing %s stream ...\n", insName (stream)) ;

/* Close the buffered input stream. */

    bioDestroy (stream->input) ;

/* Close the underlying network connection. */

    tcpDestroy (stream->connection) ;

/* Deallocate the CORTEX stream structure. */

    if (stream->inputMessage != NULL) {
        free (stream->inputMessage) ;  stream->inputMessage = NULL ;
    }
    if (stream->outputMessage != NULL) {
        free (stream->outputMessage) ;  stream->outputMessage = NULL ;
    }
    free (stream) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    insEncode ()

    Encode Data into XDR Format.


Purpose:

    Function insEncode() encodes data in XDR format for sending in a CORTEX
    message.


    Invocation:

        encoded = insEncode (data, encodeF, step, length, buffer) ;

    where

        <data>		- I
            is the address of the data to be encoded.
        <encodeF>	- I
            is the address of an XDR function used to encode the data; e.g.,
            xdr_long() or xdr_float().  The type of such a function, xdrproc_t,
            is defined in "rpc/xdr.h":
                bool_t  xdr_XYZ (XDRS *handle, void *data) ;
        <step>		- I
            specifies the stride (in bytes) to use for the input data when
            the encode function is repeatedly applied to that data.  If STEP
            is zero, the encode function is applied once.  If STEP is greater
            than zero, the encode function is repeatedly applied to the input
            data until the data is exhausted.  On each iteration, the pointer
            into the data is incremented by STEP, so STEP should normally be
            "sizeof (XYZ)", where XYZ is the data type being encoded.
            (Consequently, variable-length data types cannot be handled by
            insEncode().)
        <length>	- I
            is the length (in bytes) of the buffer (next argument) into which
            the encoded data will be written.
        <buffer>	- O
            is the address of a buffer into which the encoded data will be
            written.
        <encoded>	- O
            returns the number of bytes of encoded data in the output buffer.

*******************************************************************************/


size_t  insEncode (

#    if PROTOTYPES
        const  char  *data,
        const  xdrproc_t  encodeF,
        size_t  step,
        size_t  length,
        char  *buffer)
#    else
        data, encodeF, step, length, buffer)

        char  *data ;
        xdrproc_t  encodeF ;
        size_t  step ;
        size_t  length ;
        char  *buffer ;
#    endif

{    /* Local variables. */
    XDR  memoryStream ;



/* Create a memory-based XDR stream for the output buffer. */

    xdrmem_create (&memoryStream, buffer, (unsigned int) length, XDR_ENCODE) ;

/* Convert the data from host representation to XDR. */

    for ( ; ; ) {
        if (!encodeF (&memoryStream, (char *) data))  break ;
        if (step == 0)  break ;
        data = data + step ;
    }

/* Return the number of output bytes generated. */

    return (xdr_getpos (&memoryStream)) ;

}

/*!*****************************************************************************

Procedure:

    insFd ()

    Get a CORTEX Stream's Socket.


Purpose:

    Function insFd() returns the Unix file descriptor for the socket connection
    associated with a CORTEX stream.


    Invocation:

        fd = insFd (stream) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <fd>		- O
            returns the UNIX file descriptor for the stream's socket.

*******************************************************************************/


IoFd  insFd (

#    if PROTOTYPES
        CortexStream  stream)
#    else
        stream)

        CortexStream  stream ;
#    endif

{

    return ((stream == NULL) ? INVALID_SOCKET : tcpFd (stream->connection)) ;

}

/*!*****************************************************************************

Procedure:

    insIsReadable ()

    Check if Data is Waiting to be Read.


Purpose:

    The insIsReadable() function checks to see if data is waiting to be read
    from a CORTEX stream.


    Invocation:

        isReadable = insIsReadable (stream) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <isReadable>	- O
            returns true (a non-zero value) if data is available for
            reading and false (zero) otherwise.

*******************************************************************************/


bool  insIsReadable (

#    if PROTOTYPES
        CortexStream  stream)
#    else
        stream)

        CortexStream  stream ;
#    endif

{
    if (stream == NULL)
        return (false) ;
    else if (bioPendingInput (stream->input))		/* Buffered input? */
        return (true) ;
    else
        return (tcpIsReadable (stream->connection)) ;	/* Real input? */
}

/*!*****************************************************************************

Procedure:

    insIsUp ()

    Check if a Stream's Network Connection is Up.


Purpose:

    The insIsUp() function checks to see if a CORTEX stream's underlying
    network connection is still up.


    Invocation:

        isUp = insIsUp (stream) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <isUp>		- O
            returns true (a non-zero value) if the stream's network
            connection is up and false (zero) otherwise.

*******************************************************************************/


bool  insIsUp (

#    if PROTOTYPES
        CortexStream  stream)
#    else
        stream)

        CortexStream  stream ;
#    endif

{

    return ((stream == NULL) ? false : tcpIsUp (stream->connection)) ;

}

/*!*****************************************************************************

Procedure:

    insIsWriteable ()

    Check if a Stream is Ready for Writing.


Purpose:

    The insIsWriteable() function checks to see if data can be written to
    a CORTEX stream.


    Invocation:

        isWriteable = insIsWriteable (stream) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <isWriteable>	- O
            returns true (a non-zero value) if the CORTEX stream is ready
            for writing and false (zero) otherwise.

*******************************************************************************/


bool  insIsWriteable (

#    if PROTOTYPES
        CortexStream  stream)
#    else
        stream)

        CortexStream  stream ;
#    endif

{

    return ((stream == NULL) ? false : tcpIsWriteable (stream->connection)) ;

}

/*!*****************************************************************************

Procedure:

    insName ()

    Get a CORTEX Stream's Name.


Purpose:

    Function insName() returns a CORTEX stream's name.


    Invocation:

        name = insName (stream) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <name>		- O
            returns the stream's name.  The name is stored in memory local
            to the INS utilities and it should not be modified or freed by
            the caller.

*******************************************************************************/


const  char  *insName (

#    if PROTOTYPES
        CortexStream  stream)
#    else
        stream)

        CortexStream  stream ;
#    endif

{

    return ((stream == NULL) ? "" : tcpName (stream->connection)) ;

}

/*!*****************************************************************************

Procedure:

    insPart ()

    Convert a Textual Name to a CORTEX Component Code.


Purpose:

    Function insPart() returns the CORTEX component code corresponding to a
    component name; e.g., 0x1010 is returned for "TCU".


    Invocation:

        code = insPart (name, isVME) ;

    where

        <name>	- I
            is the component name.
        <isVME>	- I
            specifies the type of the CORTEX unit, VME (true) or NT (false).
            The component names and codes differ between the two types.
        <code>	- O
            returns the component code (listed under "Monitoring Request" in
            the CORTEX ICD).  -1 is returned for invalid component names.

*******************************************************************************/


int32_t  insPart (

#    if PROTOTYPES
        const  char  *name,
        bool  isVME)
#    else
        name, isVME)

        char  *name ;
        bool  isVME ;
#    endif

{    /* Local variables. */
    const  ComponentInfo  *info = (isVME ? componentInfoVME : componentInfoNT) ;
    size_t  i ;



/* Locate the name in the component name table and return the corresponding
   component code. */

    for (i = 0 ;  info[i].name != NULL ;  i++) {
        if (strcasecmp (info[i].name, name) == 0)
            return (info[i].code) ;			/* Found. */
    }

    return (INT32_C(-1)) ;				/* Not found. */

}

/*!*****************************************************************************

Procedure:

    insPartName ()

    Convert a CORTEX Component Code to its Textual Name.


Purpose:

    Function insPartName() returns the textual name of a CORTEX component
    code; e.g., "TCU" is returned for component code 0x1010.


    Invocation:

        name = insPartName (code) ;

    where

        <code>	- I
            is a component code (listed under "Monitoring Request" in the
            CORTEX ICD).
        <name>	- O
            returns the component name corresponding to the component code.
            The name is stored in memory local to the INS utilities and it
            should not be modified or freed by the caller.

*******************************************************************************/


const  char  *insPartName (

#    if PROTOTYPES
        int32_t  code)
#    else
        code)

        int32_t  code ;
#    endif

{    /* Local variables. */
    size_t  i ;
    static  char  buffer[16] ;



/* Lookup the code in the NT component code table and, if found, return
   the corresponding component name. */

    for (i = 0 ;  componentInfoNT[i].name != NULL ;  i++) {
        if (componentInfoNT[i].code == code)
            return (componentInfoNT[i].name) ;
    }

/* Lookup the code in the VME component code table and, if found, return
   the corresponding component name. */

    for (i = 0 ;  componentInfoVME[i].name != NULL ;  i++) {
        if (componentInfoVME[i].code == code)
            return (componentInfoVME[i].name) ;
    }

/* If the code was not found in either of the tables, then return its
   numeric value in ASCII. */

    sprintf (buffer, "%ld", (long) code) ;

    return (buffer) ;

}

/*!*****************************************************************************

Procedure:

    insPort ()

    Convert a Textual Name to a CORTEX Port Number.


Purpose:

    Function insPort() returns the CORTEX port number corresponding to a
    port name; e.g., 3000 is returned for "MON".


    Invocation:

        status = insPort (name, offset, &port) ;

    where

        <name>		- I
            is the port name.
        <offset>	- I
            is an offset added to the port number.  Since the standard CORTEX
            ports are in the low 3000 range, the offset should not drop the
            port number below 1 or raise it above 65535.
        <port>		- O
            returns the port number (listed under "TCP-IP Server" in the
            CORTEX ICD) plus the offset.
        <status>	- O
            returns zero if there were no errors and ERRNO otherwise.  EINVAL
            is returned if an invalid port name was specified or if the port
            number plus offset is out of range.

*******************************************************************************/


errno_t  insPort (

#    if PROTOTYPES
        const  char  *name,
        int  offset,
        in_port_t  *port)
#    else
        name, offset, port)

        char  *name ;
        int  offset ;
        in_port_t  *port
#    endif

{    /* Local variables. */
    long  number ;
    size_t  i ;



    if ((name == NULL) || (port == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(insPort) NULL port name or port number return value.\n") ;
        return (0) ;
    }

/* Locate the name in the port name table and return the corresponding port
   number. */

    for (i = 0 ;  portInfo[i].name != NULL ;  i++) {
        if (strcasecmp (portInfo[i].name, name) == 0)  break ;
    }

    if (portInfo[i].name == NULL) {			/* Name not found? */
        SET_ERRNO (EINVAL) ;
        LGE "(insPort) Invalid port name: \"%s\"\n", name) ;
        return (errno) ;
    }

    number = portInfo[i].port + offset ;		/* Name found. */
    if ((0 <= number) && (number <= IN_PORT_MAX)) {	/* Valid range? */
        *port = (in_port_t) number ;
        return (0) ;
    } else {
        SET_ERRNO (EINVAL) ;
        LGE "(insPort) Invalid offset: %s + %d\n", name, offset) ;
        return (errno) ;
    }

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    insPortName ()

    Convert a CORTEX Port Number to its Textual Name.


Purpose:

    Function insPortName() returns the textual name of a CORTEX port number;
    e.g., "LOG" is returned for port number 3040.


    Invocation:

        name = insPortName (port, offset, isVME) ;

    where

        <port>		- I
            is a port number.  The offset, if any, is subtracted from the
            port number to get a standard CORTEX port number (listed under
            "TCP-IP Server" in the CORTEX ICD).
        <offset>	- I
            was the offset, if any, added to the original CORTEX port
            number; see the insPort() function.  The offset is subtracted
            from the port argument, giving a standard CORTEX port whose
            name is returned to the caller.
        <isVME>		- I
            specifies the type of the CORTEX unit, VME (true) or NT (false).
            The names of some ports differ between the two types; e.g., the
            simulator port, 3021, is called "TMS" in the VME ICD and "SIM"
            in the NT ICD.
        <name>		- O
            returns the port name corresponding to the port number.  The name
            is stored in memory local to the INS utilities and it should not
            be modified or freed by the caller.  A zero-length string ("")
            is returned if an invalid port number and offset was specified.

*******************************************************************************/


const  char  *insPortName (

#    if PROTOTYPES
        in_port_t  port,
        int  offset,
        bool  isVME)
#    else
        port, offset, isVME)

        in_port_t  port ;
        int  offset ;
        bool  isVME ;
#    endif

{    /* Local variables. */
    long  number ;
    size_t  i ;
    static  char  buffer[16] ;



/* Subtract the offset from the port to get a standard CORTEX port number. */

    number = port - offset ;
    if ((number < 1) || (IN_PORT_MAX < number)) {	/* Valid range? */
        SET_ERRNO (EINVAL) ;
        LGE "(insPortName) Invalid port and offset: %hu - %d\n", port, offset) ;
        return ("") ;
    }

/* If looking up a VME port number, scan the port information table from last
   entry to first.  (The VME-specific information appears after the NT-specific
   information in the table.)  If the number is found, return the corresponding
   port name. */

    if (isVME) {

        for (i = 0 ;  portInfo[i].name != NULL ;  i++)
            ;	/* Find last entry. */

        while (i-- > 0) {
            if (portInfo[i].port == number)
                return (portInfo[i].name) ;
        }

    }

/* If looking up an NT port number, scan the port information table from first
   entry to last.  (The NT-specific information appears before the VME-specific
   information in the table.)  If the number is found, return the corresponding
   port name. */

    else {

        for (i = 0 ;  portInfo[i].name != NULL ;  i++) {
            if (portInfo[i].port == number)
                return (portInfo[i].name) ;
        }

    }

/* If the port number was not found, then return its numeric value in ASCII. */

    sprintf (buffer, "%ld", number) ;

    return (buffer) ;

}

/*!*****************************************************************************

Procedure:

    insRead ()

    Read the Next Message from a CORTEX Stream.


Purpose:

    Function insRead() reads the next message from a CORTEX network stream.
    The timeout argument provides a way of limiting how long insRead() waits
    to *begin* reading a message.

    The CORTEX unit transmits data in XDR format.  Since insRead() does not
    know the type of message being received, the caller is responsible for
    correctly decoding the message body.


    Invocation:

        status = insRead (stream, timeout, &header, &body) ;

    where

        <stream>	- I
            is the stream handle returned by insCreate().
        <timeout>	- I
            specifies the maximum amount of time (in seconds) that the
            caller wishes to wait for the next message to be read.
            A fractional time can be specified; e.g., 2.5 seconds.
            A negative timeout (e.g., -1.0) causes an infinite wait;
            a zero timeout (0.0) allows a read only if input is
            immediately available.
        <header>	- O
            returns the message header.  The numeric fields in the header
            are in host-byte order, as opposed to the network-byte order
            in which they were transferred over the network.  NOTE that
            the length field returned in the header specifies the length
            in bytes of the message body *only* and excludes the header
            and the postamble.
        <body>		- O
            returns a pointer to the message body; NULL is returned if the
            message has no body (e.g., a simple command with no accompanying
            data).  The message body must be used or duplicated before calling
            insRead() again.  If the caller is not interested in the message
            body, specify this argument as NULL, in which case the message
            body will be discarded.  NOTE that the message body is encoded
            in XDR format by the CORTEX unit; the caller is responsible for
            decoding it per the CORTEX ICD.
        <status>
            returns the status of reading a message, zero if no errors
            occurred, EWOULDBLOCK if the timeout interval expired with
            no input, and ERRNO otherwise.

*******************************************************************************/


errno_t  insRead (

#    if PROTOTYPES
        CortexStream  stream,
        double  timeout,
        CortexHeader  *header,
        char  **body)
#    else
        stream, timeout, header, body)

        CortexStream  stream ;
        double  timeout ;
        CortexHeader  *header ;
        char  **body ;
#    endif

{

    if (body != NULL)  *body = NULL ;

    if ((stream == NULL) || (header == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(insRead) NULL stream handle or header address: ") ;
        return (errno) ;
    }

/* Read the message header. */

    if (bioRead (stream->input, timeout, HEADER_SIZE, (char *) header, NULL)) {
        if (errno != EWOULDBLOCK) {
            LGE "(insRead) Error reading message header from %s.\nbioRead: ",
                insName (stream)) ;
        }
        return (errno) ;
    }

/* Convert the fields in the message header to host-byte order. */

    header->preamble = ntohl (header->preamble) ;
    header->length = ntohl (header->length) ;
    header->flowID = ntohl (header->flowID) ;

/* Allocate a buffer for the incoming message. */

    if ((header->length < HEADER_SIZE) || (MAX_MSG_SIZE < header->length)) {
        SET_ERRNO (EFBIG) ;
        LGE "(insRead) Invalid length (%ld) in message received from %s.\n",
            (long) header->length, insName (stream)) ;
        return (errno) ;
    }

    if (stream->inputMessage != NULL) {
        free (stream->inputMessage) ;  stream->inputMessage = NULL ;
    }

    stream->inputMessage = malloc (header->length) ;
    if (stream->inputMessage == NULL) {
        LGE "(insRead) Error allocating %ld-byte buffer for incoming messsage from %s.\n",
            (long) header->length, insName (stream)) ;
        return (errno) ;
    }

/* Read the message body. */

    (void) memcpy (stream->inputMessage, header, HEADER_SIZE) ;

    if (bioRead (stream->input, -1.0, header->length - HEADER_SIZE,
                 &stream->inputMessage[HEADER_SIZE], NULL)) {
        if (errno != EWOULDBLOCK) {
            LGE "(insRead) Error reading message body from %s.\nbioRead: ",
                insName (stream)) ;
        }
        return (errno) ;
    }

    if (body != NULL)  *body = stream->inputMessage + HEADER_SIZE ;
    header->length = header->length - HEADER_SIZE - TRAILER_SIZE ;

    LGI "(insRead) %ld-byte message (%ld) from %s.\n",
        (long) header->length, (long) header->flowID, insName (stream)) ;

    return (0) ;

}

/*!*****************************************************************************

Procedure:

    insWrite ()

    Write a Message to a CORTEX Stream.


Purpose:

    Function insWrite() writes a message to a CORTEX network stream.
    The timeout argument provides a way of limiting how long insWrite()
    waits to *begin* outputting the message.

    The CORTEX unit transmits data in XDR format.  Since insWrite() does
    not know the type of message being sent, the caller is responsible
    for correctly encoding the message body.


    Invocation:

        status = insWrite (stream, timeout, header, body) ;

    where

        <stream>		- I
            is the stream handle returned by insCreate().
        <timeout>		- I
            specifies the maximum amount of time (in seconds) that the
            caller wishes to wait for the message to be written.
            A fractional time can be specified; e.g., 2.5 seconds.
            A negative timeout (e.g., -1.0) causes an infinite wait;
            a zero timeout (0.0) only writes the message if it can
            be output immediately.
        <header>	- I
            is a pointer to the CORTEX header of the message being written.
            The numeric fields in the header should be in host-byte order;
            they will be automatically converted to network-byte order when
            transferred over the network.  The caller is responsible for
            setting the length and the flow ID in the header; insWrite()
            supplies the other fields and the message postamble.  NOTE
            that the caller-specified length is the length of the message
            body *only* and excludes the header and postamble; insWrite()
            adjusts the length field to cover the header and postamble
            (per the CORTEX ICD) when it outputs the message.
        <body>		- I
            is the message body, the length of which is specified in the
            message header.  NULL can be specified if there is no message
            body.  NOTE that the message body must already be encoded in
            the XDR format expected by the CORTEX unit; see the CORTEX ICD.
        <status>	- O
            returns the status of writing the message, zero if no errors
            occurred, EWOULDBLOCK if the timeout interval expired with
            no output, and ERRNO otherwise.

*******************************************************************************/


errno_t  insWrite (

#    if PROTOTYPES
        CortexStream  stream,
        double  timeout,
        const  CortexHeader  *header,
        const  char  *body)
#    else
        stream, timeout, header, body)

        CortexStream  stream ;
        double  timeout ;
        CortexHeader  *header ;
        char  *body ;
#    endif

{    /* Local variables. */
    int32_t  *integer ;
    size_t  length ;




    if ((stream == NULL) || (header == NULL)) {
        SET_ERRNO (EINVAL) ;
        LGE "(insWrite) NULL stream handle or header address: ") ;
        return (errno) ;
    }

/* Calculate the size of the full message (header, body, and postamble);
   if necessary, pad the body so the postamble is aligned on a 32-bit
   boundary. */

    length = HEADER_SIZE +
             ((body == NULL) ? 0 : header->length) +
             TRAILER_SIZE ;
    length = ((length + BYTES_PER_XDR_UNIT - 1) / BYTES_PER_XDR_UNIT)
             * BYTES_PER_XDR_UNIT ;

    if (length > MAX_MSG_SIZE) {
        SET_ERRNO (EFBIG) ;
        LGE "(insWrite) %lu-byte message is too large for %s.\n",
            length, insName (stream)) ;
        return (errno) ;
    }

/* Allocate a buffer to hold the header, body, and postamble of the outgoing
   message. */

    if (length > stream->outputLength) {	/* Existing buffer too small? */
        if (stream->outputMessage != NULL)  free (stream->outputMessage) ;
        stream->outputLength = 0 ;
        stream->outputMessage = malloc (length) ;
        if (stream->outputMessage == NULL) {
            LGE "(insWrite) Error allocating %lu-byte message buffer for %s.\nmalloc: ",
                (unsigned long) length, insName (stream)) ;
            return (errno) ;
        }
        stream->outputLength = length ;
    }

/* Place the message header in the output buffer.  Convert numeric fields in
   the header from host-byte-order to network-byte-order. */

    integer = (int32_t *) stream->outputMessage ;
    integer[0] = htonl (InsPreamble) ;
    integer[1] = htonl ((int32_t) length) ;
    integer[2] = htonl (header->flowID) ;

/* Append the message body to the header. */

    memset (&stream->outputMessage[HEADER_SIZE], 0,
            length - HEADER_SIZE - TRAILER_SIZE) ;
    if ((header->length > 0) && (body != NULL)) {
        (void) memcpy (&stream->outputMessage[HEADER_SIZE], body,
                       header->length) ;
    }

/* Append the message postamble to the body. */

    integer = (int32_t *) (stream->outputMessage + length - TRAILER_SIZE) ;
    integer[0] = htonl (InsPostamble) ;

/* Output the complete message to the network. */

    if (tcpWrite (stream->connection, timeout,
                  length, stream->outputMessage, NULL)) {
        LGE "(insWrite) Error writing %lu-byte message to %s.\ntcpWrite: ",
            (unsigned long) length, insName (stream)) ;
        return (errno) ;
    }

    LGI "(insWrite) %ld-byte message (%ld) to %s.\n",
        (long) header->length, (long) header->flowID, insName (stream)) ;

    return (0) ;

}

#ifdef  TEST

/*******************************************************************************

    Program to test the INS_UTIL functions.

    Under UNIX:
        Compile and link as follows:
            % cc -DTEST ins_util.c <libraries> -o ins_test
        First run the server with the following command line:
            % ins_test server <name> &
        Then, run the client (preferably in a separate window):
            % ins_test client <name>
        The client sends 16 messages to the server and the server
        reads them.

    Under VxWorks:
        Compile and link as follows:
            % cc -DTEST ins_util.c <libraries> -o ins_test.vx.o
        First, load and run the server with the following commands:
            -> ld <ins_test.vx.o
            -> sp ins_test, "server <name>"
        Then, load (to be on the safe side) and run the client:
            -> ld <ins_test.vx.o
            -> sp ins_test, "client <name>"
        The client sends 16 messages to the server and the server
        reads them.

*******************************************************************************/


int  main (argc, argv)

    int  argc ;
    char  *argv[] ;

{    /* Local variables. */
    CortexStream  stream ;
    CortexHeader  header ;
    CortexTelemetryRequest  request ;
    int32_t  *body, i ;
    TcpEndpoint  connection ;


    ins_util_debug = 1 ;
    tcp_util_debug = 1 ;

    if (argc < 2) {
        fprintf (stderr, "Usage:  %s <port>[@<host>]\n", argv[0]) ;
        exit (EINVAL) ;
    }

/* Connect to CORTEX service. */

    if (tcpCall (argv[1], 0, &connection))  exit (errno) ;
    insCreate (connection, &stream) ;

/* Construct telemetry request. */

    request.channel = 0 ;
    request.numBuffered = 0 ;
    request.flowType = 0 ;
    request.frameMask[0] = 0 ;
    request.frameMask[1] = 0 ;
    request.expectedValue[0] = 0 ;
    request.expectedValue[1] = 0 ;
    for (i = 0 ;  i < 5 ;  i++)
        request.unused[i] = 0 ;

/* Send request to CORTEX. */

    header.length = sizeof (CortexTelemetryRequest) ;
    header.flowID = 1234567L ;
    if (insWrite (stream, -1.0, &header, (char *) &request))  exit (errno) ;

/* Read frames from CORTEX. */

    for ( ; ; ) {
        if (insRead (stream, -1.0, &header, (char **) &body))  break ;
        if (header.flowID == 0)  break ;
        printf ("Frame %ld received at %g seconds.\n", ntohl (body[2]),
                (double) ntohl (body[0]) + (ntohl (body[1]) / 1000.0)) ;
    }

/* Close connection to CORTEX. */

    insDestroy (stream) ;

    exit (0) ;

}

#endif  /* TEST */
