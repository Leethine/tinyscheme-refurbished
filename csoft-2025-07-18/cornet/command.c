/* $Id: command.c,v 1.7 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    command.c

    CORNET Telecommanding.


Author:    Alex Measday


Purpose:

    A collection of functions for requesting sending spacecraft commands to
    a CORTEX unit.


Public Procedures:

    sendTC() - sends a telecommand instruction or request to a CORTEX unit.

Private Procedures:

    ackInputCB() - reads acknowledgement messages from a CORTEX
        telecommand port.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "cornet.h"			/* CORNET definitions. */


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  ackInputCB (
#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData
#    endif
    ) ;

/*******************************************************************************

Procedure:

    sendTC ()

    Send a Telecommand Instruction or Request to a CORTEX Unit.


Purpose:

    Function sendTC() sends a telecommand instruction or a request to a CORTEX
    telecommand unit.  The network connection to the CORTEX is left open so the
    command acknowledgement message can be received.


    Invocation:

        status = sendTC (host, isVME, offset, flowID, operation) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX unit.
        <isVME>		- I
            specifies whether the CORTEX unit is VME-based (true) or
            NT-based (false).
        <offset>	- I
            is an offset to be added to the CORTEX TC port number (i.e.,
            port 3020).  Specifying a non-zero offset allows CORNET to
            connect to "virtual" CORTEX units listening at different
            service ports on a single host.
        <flowID>	- I
            is the telecommand data flow identification.
        <operation>	- I
            is the instruction or request to be performed.
        <status>	- O
            returns the status of sending the NOP instruction to the CORTEX
            unit's telecommand port, zero if there were no errors and ERRNO
            otherwise.

*******************************************************************************/


errno_t  sendTC (

#    if PROTOTYPES
        const  char  *host,
        bool  isVME,
        int  offset,
        int32_t  flowID,
        const  char  *operation)
#    else
        host, isVME, offset, flowID, operation)

        char  *host ;
        bool  isVME ;
        int  offset ;
        int32_t  flowID ;
        char  *operation ;
#    endif

{    /* Local variables. */
    CortexHeader  header ;
    in_port_t  port ;
    int  numFields ;
    Link  link ;




/* Establish a network connection with the CORTEX unit's telecommand port. */

    if (insPort ("TC", offset, &port)) {
        LGE "(sendTC) Invalid port \"TC\" and offset %d.\n", offset) ;
        return (errno) ;
    }

    if (linkOpen (host, port, isVME, &link)) {
        LGE "(sendTC) Error connecting to %s's telecommand port\nlinkOpen: ",
            host) ;
        return (errno) ;
    }

/* Send a spacecraft command to the CORTEX unit. */

    if (strcasecmp (operation, "COMMAND") == 0) {

#define  TC_FRAME_LENGTH  512
        int32_t  field[(TC_FRAME_LENGTH/8)+4], i ;

        numFields = 0 ;
        field[numFields++] = htonl (1) ;		/* Clear TC request. */
        field[numFields++] = INT32_C(0x12345678) ;	/* Command tag. */
        field[numFields++] = htonl (TC_FRAME_LENGTH) ;	/* Frame length in bits. */
        for (i = 0 ;  i < (TC_FRAME_LENGTH/8) ;  i++)
            field[numFields++] = htonl (i) ;
        header.length = numFields * sizeof (int32_t) ;
        field[numFields++] = htonl (insChecksum (&header, (char *) field)) ;

        header.length = numFields * sizeof (int32_t) ;
        header.flowID = flowID ;

        if (insWrite (link->stream, -1.0, &header, (char *) field)) {
            LGE "(sendTC) Error sending Clear TC request to %s.\ninsWrite: ",
                insName (link->stream)) ;
            return (errno) ;
        }

    }

/* Send an EXECUTE instruction to the CORTEX unit. */

    else if (strcasecmp (operation, "EXECUTE") == 0) {

        int32_t  field[5] ;

        numFields = 0 ;
        field[numFields++] = htonl (4) ;		/* Execute instruction. */
        field[numFields++] = 1 ;			/* # of pulses. */
        field[numFields++] = 2 ;			/* Pulse width. */
        field[numFields++] = 3 ;			/* Pulse period. */
        header.length = numFields * sizeof (int32_t) ;
        field[numFields++] = htonl (insChecksum (&header, (char *) field)) ;

        header.length = numFields * sizeof (int32_t) ;
        header.flowID = flowID ;

        if (insWrite (link->stream, -1.0, &header, (char *) field)) {
            LGE "(sendTC) Error sending EXECUTE instruction to %s.\ninsWrite: ",
                insName (link->stream)) ;
            return (errno) ;
        }

    }

/* Send a NOP instruction to the CORTEX unit. */

    else if (strcasecmp (operation, "NOP") == 0) {

        int32_t  field[2] ;

        numFields = 0 ;
        field[numFields++] = htonl (8) ;		/* NOP instruction. */
        header.length = numFields * sizeof (int32_t) ;
        field[numFields++] = htonl (insChecksum (&header, (char *) field)) ;

        header.length = numFields * sizeof (int32_t) ;
        header.flowID = flowID ;

        if (insWrite (link->stream, -1.0, &header, (char *) field)) {
            LGE "(sendTC) Error sending NOP instruction to %s.\ninsWrite: ",
                insName (link->stream)) ;
            return (errno) ;
        }

    }

/* Register an input callback for the command acknowledgement returned by
   the CORTEX unit. */

    ioxOnIO (dispatcher, ackInputCB, link, IoxRead, insFd (link->stream)) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    ackInputCB ()

    Read Acknowledgement Messages from a CORTEX Telecommand Port.


Purpose:

    Function ackInputCB() reads and displays any acknowledgement messages
    waiting to be read from the telecommand port on a CORTEX unit.  When a
    network connection is established with the CORTEX unit, the connection
    is registered with the IOX dispatcher as an input source.  Thereafter,
    whenever input is detected on the connection, the IOX dispatcher
    automatically invokes ackInputCB() to read the messages from the
    CORTEX unit.


    Invocation:

        status = ackInputCB (callback, reason, userData) ;

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
            ackInputCB() directly.

*******************************************************************************/


static  errno_t  ackInputCB (

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
            LGE "(ackInputCB) Error reading from %s.\ninsRead: ",
                insName (link->stream)) ;
            break ;
        }

        printf ("\n%s  (%ld bytes)  Flow ID: %ld\n",
                insName (link->stream), (long) header.length,
                (long) header.flowID) ;

        insDecode ((char *) body, (xdrproc_t) xdr_int32_t, sizeof (int32_t),
                   header.length, (char *) body) ;

        if (header.length == 24) {

            printf ("    Operation: %ld  Tag: %ld  Checksum: %ld\n",
                    (long) body[0], (long) body[1], (long) body[5]) ;

            switch (body[4]) {
            case -2:
                printf ("    Acknowledgement: connection rejected\n") ;
                break ;
            case -1:
                printf ("    Acknowledgement: unidentified message\n") ;
                break ;
            case 0:
                printf ("    Acknowledgement: OK\n") ;
                break ;
            case 1:
                printf ("    Acknowledgement: TC encoder locked out\n") ;
                break ;
            case 2:
                printf ("    Acknowledgement: invalid checksum/syntax/mode\n") ;
                break ;
            case 3:
                printf ("    Acknowledgement: verification failed after retries\n") ;
                break ;
            case 4:
                printf ("    Acknowledgement: CMM1 checking failed\n") ;
                break ;
            case 5:
                printf ("    Acknowledgement: CMM2 checking failed\n") ;
                break ;
            case 6:
                printf ("    Acknowledgement: group rejected\n") ;
                break ;
            case 7:
                printf ("    Acknowledgement: TCU failure\n") ;
                break ;
            default:
                printf ("    Acknowledgement: (%ld)\n", (long) body[0]) ;
                break ;
            }

        } else {

            dumpData (stdout, 3, header.length / sizeof (int32_t), body, 1, 0) ;

        }

    }

/* Check to see if the stream's network connection has been broken.  If so,
   close the connection and exit the program. */

    if (!insIsUp (link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(ackInputCB) Broken connection to %s.\ninsIsUp: ",
            insName (link->stream)) ;
        ioxCancel (callback) ;
        linkClose (link) ;
    }

    return (0) ;

}
