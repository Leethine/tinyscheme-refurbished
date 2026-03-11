/* $Id: telemetry.c,v 1.8 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    telemetry.c

    CORNET Telemetry Reception.


Author:    Alex Measday


Purpose:

    A collection of functions for requesting and processing telemetry from
    a CORTEX unit.


Public Procedures:

    startTLM() - initiates the reception of telemetry from a CORTEX unit.

Private Procedures:

    tlmInputCB() - reads telemetry messages from a CORTEX unit.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "meo_util.h"			/* Memory operations. */
#include  "tv_util.h"			/* "timeval" manipulation functions. */
#include  "cornet.h"			/* CORNET definitions. */


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  tlmInputCB (
#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData
#    endif
    ) ;

/*******************************************************************************

Procedure:

    startTLM ()

    Start the Flow of Telemetry.


Purpose:

    Function startTLM() initiates the reception of telemetry data from the
    CORTEX unit.


    Invocation:

        status = startTLM (host, isVME, noRequest, offset, flowID,
                           channel, history) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX unit.
        <isVME>		- I
            specifies whether the CORTEX unit is VME-based (true) or
            NT-based (false).
        <noRequest>	- I
            specifies whether or not a TM request message should be sent
            to an NT-based CORTEX.
        <offset>	- I
            is an offset to be added to the CORTEX TM port number (e.g.,
            CORTEX-NT's port 3070).  Specifying a non-zero offset allows
            CORNET to connect to "virtual" CORTEX units listening at
            different service ports on a single host.
        <flowID>	- I
            is the telemetry data flow identification (NT-based CORTEX only).
        <channel>	- I
            specifies the desired channel of telemetry: 0 for A, 1 for B,
            2 for C, 3 for D, 4 for E, and 5 for F.
        <history>	- I
            specifies whether (true) or not (false) buffered telemetry frames
            are to be sent before the real-time telemetry.
        <status>	- O
            returns the status of starting telemetry, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  startTLM (

#    if PROTOTYPES
        const  char  *host,
        bool  isVME,
        bool  noRequest,
        int  offset,
        int32_t  flowID,
        int32_t  channel,
        bool  history)
#    else
        host, isVME, noRequest, offset, flowID, channel, history)

        char  *host ;
        bool  isVME ;
        bool  noRequest ;
        int  offset ;
        int32_t  flowID ;
        int32_t  channel ;
        bool  history ;
#    endif

{    /* Local variables. */
    Link  link ;
    CortexHeader  header ;
    in_port_t  port ;
    int32_t  request[12] ;




/* Determine which telemetry port to use.  The NT-based CORTEX has a single
   port for all channels; the VME-based unit has separate ports for each
   channel. */

    if (isVME && (0 <= channel) && (channel <= 2)) {
        insPort ("TMA-R", offset + (2 * channel), &port) ;
        if (history)  port++ ;				/* Use "-H" port. */
    } else if (!isVME && (0 <= channel) && (channel <= 5)) {
        insPort ("TM", offset, &port) ;
    } else {
        SET_ERRNO (EINVAL) ;
        LGE "(startTLM) Invalid channel: %ld\n", (long) channel) ;
        return (errno) ;
    }

/* Establish a network connection to the telemetry port on the CORTEX unit. */

    if (linkOpen (host, port, isVME, &link)) {
        LGE "(startTLM) Error connecting to CORTEX: %s\nlinkOpen: ", host) ;
        return (errno) ;
    }

/* Register a function to be called when a frame is received. */

    ioxOnIO (dispatcher, tlmInputCB, link, IoxRead, insFd (link->stream)) ;

/* If the CORTEX unit is NT-based, send a telemetry request to the unit. */

    if (!isVME && !noRequest) {
        memset (request, 0, sizeof request) ;
        request[3-3] = htonl ((int32_t) channel) ;
        if (history)  request[4-3] = htonl (INT32_C(1024)) ;
        request[5-3] = htonl (INT32_C(2)) ;
        header.length = sizeof request ;  header.flowID = flowID ;
        if (insWrite (link->stream, -1.0, &header, (char *) &request)) {
            LGE "(startTLM) Error sending telemetry request to CORTEX unit: %s\ninsWrite: ",
                insName (link->stream)) ;
        }
    }

    return (0) ;

}

/*******************************************************************************

Procedure:

    tlmInputCB ()

    Read Telemetry from a CORTEX Unit.


Purpose:

    Function tlmInputCB() reads and displays any telemetry messages waiting to
    be read from a CORTEX unit.  When a network connection is established with
    the CORTEX unit, the connection is registered with the IOX dispatcher as
    an input source.  Thereafter, whenever input is detected on the connection,
    the IOX dispatcher automatically invokes tlmInputCB() to read the messages
    from the CORTEX unit.


    Invocation:

        status = tlmInputCB (callback, reason, userData) ;

    where:

        <callback>	- I
            is the handle assigned to the callback by ioxOnIO().
        <reason>	- I
            is the reason (e.g., IoxRead) the callback is being invoked.
        <userData>	- I
            is the address of the CORTEX link structure returned by linkOpen().
        <status>	- O
            returns the status of reading the input, zero if there were no
            errors and ERRNO otherwise.  The status value is ignored by the
            IOX dispatcher, but it may be of use if the application calls
            tlmInputCB() directly.

*******************************************************************************/


static  errno_t  tlmInputCB (

#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData)
#    else
        dispatcher, reason, userData)

        IoxCallback  callback ;
        IoxReason  reason ;
        void  *userData ;
#    endif

{    /* Local variables. */
    Link  link = (Link) userData ;
    CortexHeader  header ;
    int32_t  *body ;




/* While more input is available, read and display the next message. */

    while (insIsReadable (link->stream)) {
					/* Read the next message. */
        if (insRead (link->stream, -1.0, &header, (char **) &body)) {
            LGE "(tlmInputCB) Error reading from %s.\ninsRead: ",
                insName (link->stream)) ;
            break ;
        }

        printf ("\n%s  (%ld bytes)  Flow ID: %ld\n",
                insName (link->stream),
                (long) header.length, (long) header.flowID) ;

        if (link->isVME)
            insDecode ((char *) body,
                       (xdrproc_t) xdr_int32_t,
                       sizeof (int32_t),
                       (header.length > 28) ? 28 : header.length,
                       (char *) body) ;
        else
            insDecode ((char *) body,
                       (xdrproc_t) xdr_int32_t,
                       sizeof (int32_t),
                       (header.length > 52) ? 52 : header.length,
                       (char *) body) ;

        if (header.length == 4) {
            switch (body[0]) {
            case -2:
                printf ("    Acknowledgement: connection rejected\n") ;
                break ;
            case -1:
                printf ("    Acknowledgement: unidentified message\n") ;
                break ;
            case 0:
                printf ("    Acknowledgement: accepted\n") ;
                break ;
            case 1:
                printf ("    Acknowledgement: invalid syntax\n") ;
                break ;
            case 2:
                printf ("    Acknowledgement: TM chain not mounted\n") ;
                break ;
            default:
                printf ("    Acknowledgement: (%ld)\n", (long) body[0]) ;
                break ;
            }
            continue ;
        }

#ifdef FORMATTED_TIME
        printf ("    Time: %s    Count: %-9ld       Check: %s\n",
                tvShow (tvCreate (body[0], body[1] * 1000), 1, "%c"),
                (long) body[2],
                body[3] ? "ERROR" : "OK") ;
#elif defined(TIME_SINCE_BEGINNING_OF_YEAR)
        printf ("    Time: %s    Count: %-9ld       Check: %s\n",
                tvShow (tvAdd (tvTruncate (tvTOD (), TvYear),
                               tvCreate (body[0], body[1] * 1000)),
                        1, "%c"),
                (long) body[2],
                body[3] ? "ERROR" : "OK") ;
#else
        printf ("    Time: %9ld.%03ld    Count: %-9ld       Check: %s\n",
                (long) body[0], (long) body[1], (long) body[2],
                body[3] ? "ERROR" : "OK") ;
#endif
        printf ("    Sync: %-13s    Delay: %-9ld    Bit Slip: %s\n",
                (body[4] == 0) ? "OFF" :
                                 (body[4] == 1) ? "LOCK" :
                                 (body[4] == 2) ? "FLYWHEEL" :
                                 (body[4] == 3) ? "SEARCH" : "UNKNOWN",
                (long) body[6],
                body[5] ? "YES" : "NO") ;

/* The check for "missing frame data" below was added when debugging
   the emulator (ENTICE), which was sending 16-byte, only header plus
   postamble telemetry messages for some reason, causing CORNET to crash
   with a core dump. */

        if (link->isVME) {
            long  frameLength = header.length - (7 * sizeof (int32_t)) ;
            if (frameLength > 0) {
                meoDumpX (stdout, "    ", 0, (char *) &body[7], frameLength) ;
            } else {
                LGI "(tlmInputCB) Missing frame data in %ld-byte VME message.\n",
                    (long) header.length) ;
            }
        } else {
            long  frameLength = header.length - (13 * sizeof (int32_t)) ;
            if (frameLength > 0) {
                meoDumpX (stdout, "    ", 0, (char *) &body[13], frameLength) ;
            } else {
                LGI "(tlmInputCB) Missing frame data in %ld-byte NT message.\n",
                    (long) header.length) ;
            }
        }

    }

/* Check to see if the stream's network connection has been broken.  If so,
   close the connection and exit the program. */

    if (!insIsUp (link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(tlmInputCB) Broken connection to %s.\ninsIsUp: ",
            insName (link->stream)) ;
        ioxCancel (callback) ;
        linkClose (link) ;
    }

    return (0) ;

}
