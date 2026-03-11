/* $Id: sequence.c,v 1.9 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    sequence.c

    Collect Ranging Sequence Data.


Author:    Alex Measday


Purpose:

    A collection of functions for requesting and receiving ranging sequence
    data from a CORTEX-VME unit.


Public Procedures:

    sequence() - requests a range sequence from a CORTEX-VME unit.

Private Procedures:

    rspInputCB() - reads ranging transaction responses from a CORTEX unit.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "cornet.h"			/* CORNET definitions. */


/*******************************************************************************
    Transaction - contains information about a ranging transaction.
*******************************************************************************/

typedef  struct  Transaction {
    int32_t  numSequences ;		/* # of sequences requested. */
    Link  link ;			/* Link to CORTEX-VME ranging port. */
    int  stage ;			/* 0: request submitted,
					   1: primary response received,
					   2: ranging response received. */
}  Transaction ;


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  rspInputCB (
#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData
#    endif
    ) ;

/*******************************************************************************

Procedure:

    sequence ()

    Request a Ranging Sequence.


Purpose:

    Function sequence() connects to a CORTEX-VME unit's RNG-1 ranging port
    and submits a request for mono- or multi-sequence ranging.  The network
    connection is left open until the ranging transaction responses are
    received.


    Invocation:

        status = sequence (host, offset, flowID, numSequences) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX-VME unit.
        <offset>	- I
            is an offset to be added to the CORTEX RNG-1 port number (i.e.,
            port 3030).  Specifying a non-zero offset allows CORNET to connect
            to "virtual" CORTEX units listening at different service ports on
            a single host.
        <flowID>	- I
            is the ranging data flow identification.
        <numSequences>	- I
            specifies the number (1..32) of ranging sequences to request.
        <status>	- O
            returns the status of initiating the measurement, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  sequence (

#    if PROTOTYPES
        const  char  *host,
        int  offset,
        int32_t  flowID,
        int32_t  numSequences)
#    else
        host, offset, flowID, numSequences)

        char  *host ;
        int  offset ;
        int32_t  flowID ;
        int32+t  numSequences ;
#    endif

{    /* Local variables. */
    CortexHeader  header ;
    in_port_t  port ;
    int32_t  request ;
    Transaction  *transaction ;




/* Create a transaction object. */

    transaction = (Transaction *) malloc (sizeof (Transaction)) ;
    if (transaction == NULL) {
        LGE "(sequence) Error creating transaction object for %s\nmalloc: ",
            host) ;
        return (errno) ;
    }
    transaction->numSequences = numSequences ;
    transaction->link = NULL ;
    transaction->stage = 0 ;

/* Open a connection to the CORTEX-VME unit's RNG-1 ranging port and register
   a function to be called when transaction responses are received. */

    if (insPort ("RNG-1", offset, &port)) {
        LGE "(sequence) Invalid port \"RNG-1\" and offset %d.\n", offset) ;
        return (errno) ;
    }

    if (linkOpen (host, port, true, &transaction->link)) {
        LGE "(sequence) Error connecting to CORTEX-VME RNG-1 port: %s\nlinkOpen: ",
            host) ;
        return (errno) ;
    }

    ioxOnIO (dispatcher, rspInputCB, transaction, IoxRead,
             insFd (transaction->link->stream)) ;

/* Send a range request to the CORTEX unit. */

    request = htonl (numSequences) ;
    header.length = sizeof request ;  header.flowID = flowID ;
    if (insWrite (transaction->link->stream, -1.0,
                  &header, (char *) &request)) {
        LGE "(measure) Error sending %ld-sequence ranging request to CORTEX-VME unit: %s\ninsWrite: ",
            (long) numSequences, insName (transaction->link->stream)) ;
    }

    return (0) ;

}

/*******************************************************************************

Procedure:

    rspInputCB ()

    Read Response Messages from a CORTEX-VME Ranging Port.


Purpose:

    Function rspInputCB() reads and displays any transaction response
    messages waiting to be read from the RNG-1 ranging port on a
    CORTEX-VME unit.  When a network connection is established with
    the CORTEX-VME unit, the connection is registered with the IOX
    dispatcher as an input source.  Thereafter, whenever input is
    detected on the connection, the IOX dispatcher automatically
    invokes rspInputCB() to read the messages from the CORTEX-VME unit.


    Invocation:

        status = rspInputCB (callback, reason, userData) ;

    where:

        <callback>	- I
            is the handle assigned to the callback by ioxOnIO().
        <reason>	- I
            is the reason (e.g., IoxRead) the callback is being invoked.
        <userData>	- I
            is the address of the transaction structure created by sequence().
        <status>	- O
            returns the status of reading the input, zero if there were no
            errors and ERRNO otherwise.  The status value is ignored by the
            IOX dispatcher, but it may be of use if the application calls
            rspInputCB() directly.

*******************************************************************************/


static  errno_t  rspInputCB (

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
    CortexHeader  header ;
    int  first, tone ;
    int32_t  *body, numMeasurements, toneID ;
    Transaction  *transaction = (Transaction *) userData ;




/*******************************************************************************
    While more input is available, read and display the next message.
*******************************************************************************/

    while (insIsReadable (transaction->link->stream)) {
					/* Read the next message. */
        if (insRead (transaction->link->stream, -1.0,
                     &header, (char **) &body)) {
            LGE "(rspInputCB) Error reading from %s.\ninsRead: ",
                insName (transaction->link->stream)) ;
            break ;
        }

        printf ("\n%s  (%ld bytes)  Flow ID: %ld\n",
                insName (transaction->link->stream),
                (long) header.length, (long) header.flowID) ;

/* If a primary transaction or premature termination response was received
   from the ranging port, process it. */

        if (header.length == 4) {

            insDecode ((char *) body, (xdrproc_t) xdr_int32_t,
                       sizeof (int32_t), header.length, (char *) body) ;

            switch (body[0]) {
            case -2:
                printf ("    Response: connection rejected\n") ;
                break ;
            case -1:
                printf ("    Response: unidentified message\n") ;
                break ;
            case 0:
                printf ("    Response: OK\n") ;
                break ;
            case 1:
                printf ("    Response: incorrect number of sequences (%ld)\n",
                        (long) transaction->numSequences) ;
                break ;
            case 2:
                printf ("    Response: RAU in self-test mode\n") ;
                break ;
            case 3:
                printf ("    Response: number of measurements = 0\n") ;
                break ;
            case 10:
                printf ("    Response: CORTEX in local mode\n") ;
                break ;
            case 20:
                printf ("    Response: open loop\n") ;
                break ;
            case 21:
                printf ("    Response: loss of lock\n") ;
                break ;
            case 22:
                printf ("    Response: time-out: no RAU response\n") ;
                break ;
            case 23:
                printf ("    Response: 10 MHz failure\n") ;
                break ;
            default:
                printf ("    Response: (%ld)\n", (long) body[0]) ;
                break ;
            }

            if ((++transaction->stage < 2) && (body[0] == 0))  continue ;

            ioxCancel (callback) ;
            linkClose (transaction->link) ;
            transaction->link = NULL ;
            free (transaction) ;

            return (0) ;

        }

/* If a ranging transaction response was received, display it. */

        else if (header.length == (4060-16)) {

            insDecode ((char *) body, (xdrproc_t) xdr_int32_t,
                       sizeof (int32_t),
                       3 * sizeof (int32_t),
                       (char *) body) ;

            printf ("    Time: %9ld.%03ld\n",
                    (long) body[4-3], (long) body[5-3]) ;

            first = 6 - 3 ;

            for (tone = 1 ;  tone <= 4 ;  tone++, first += 2+250) {

                insDecode ((char *) &body[first], (xdrproc_t) xdr_int32_t,
                           0, sizeof (int32_t), (char *) &toneID) ;
                insDecode ((char *) &body[first+1], (xdrproc_t) xdr_int32_t,
                           0, sizeof (int32_t), (char *) &numMeasurements) ;

                printf ("%s Tone\t(%s) \t%3ld measurement%s\n",
                        ((tone == 1) ? "FIRST" :
                         (tone == 2) ? "SECOND" :
                         (tone == 3) ? "THIRD" : "FOURTH"),
                        ((toneID == 0) ? "35 Hz" :
                         (toneID == 1) ? "283 Hz" :
                         (toneID == 2) ? "3968 Hz" : "27.777 kHz"),
                        (long) numMeasurements,
                        (numMeasurements == 1) ? "" : "s") ;

                insDecode ((char *) &body[first+2], (xdrproc_t) xdr_int32_t,
                           sizeof (int32_t), numMeasurements * sizeof (int32_t),
                           (char *) &body[first+2]) ;

                dumpData (stdout, 1, numMeasurements, &body[first+2], 0, 0) ;

            }

/* If additional responses are expected (i.e., more than one sequence was
   requested), then leave the link to the ranging port open; otherwise,
   close it. */

            if (--transaction->numSequences <= 0) {
                ioxCancel (callback) ;
                linkClose (transaction->link) ;
                transaction->link = NULL ;
                free (transaction) ;
            }

            return (0) ;

        }

    }


/* Check to see if the stream's network connection has been broken.  If so,
   close it. */

    if (!insIsUp (transaction->link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(rspInputCB) Broken connection to %s.\ninsIsUp: ",
            insName (transaction->link->stream)) ;
        ioxCancel (callback) ;
        linkClose (transaction->link) ;
        free (transaction) ;
    }


    return (0) ;

}
