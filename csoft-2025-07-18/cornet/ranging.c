/* $Id: ranging.c,v 1.9 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    ranging.c

    Collect Ranging Data.


Author:    Alex Measday


Purpose:

    A collection of functions for requesting and receiving ranging data
    from a CORTEX unit.


Public Procedures:

    measure() - requests a range measurement from a CORTEX unit.

Private Procedures:

    inputResponseCB() - reads ranging transaction responses from a CORTEX unit.

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
    Link  rangingLink ;			/* Link to CORTEX ranging port. */
    IoxCallback  rangingID ;		/* IOX input source ID. */
    Link  measurementLink ;		/* Link to CORTEX measurement port. */
    IoxCallback  measurementID ;	/* IOX input source ID. */
    int  stage ;			/* 0: request submitted,
					   1: primary response received,
					   2: secondary response received. */
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

    measure ()

    Request a Ranging Measurement.


Purpose:

    Function measure() connects to a CORTEX unit's ranging port and submits
    a ranging request; a connection is also opened to the measurement port
    in order to receive the resulting measurements.  The network connections
    are left open until the ranging transaction responses are received.


    Invocation:

        status = measure (host, isVME, offset, flowID, calibrate, what) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX unit.
        <isVME>		- I
            specifies whether the CORTEX unit is VME-based (true) or
            NT-based (false).
        <offset>	- I
            is an offset to be added to the CORTEX RNG-2/RNG and MEAS port
            numbers (i.e., ports 3034 and 3035, respectively).  Specifying
            a non-zero offset allows CORNET to connect to "virtual" CORTEX
            units listening at different service ports on a single host.
        <flowID>	- I
            is the ranging data flow identification.
        <calibrate>	- I
            specifies the measurement type: false (zero) for range
            measurement and true (non-zero) for calibration loop.
        <what>		- I
            specifies what measurements are desired: "PHASE" for phase and
            time measurements (integer), "TIME" for time measurements only,
            and "FLOAT" for phase and time measurements (float).
        <status>	- O
            returns the status of initiating the measurement, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  measure (

#    if PROTOTYPES
        const  char  *host,
        bool  isVME,
        int  offset,
        int32_t  flowID,
        bool  calibrate,
        const  char  *what)
#    else
        host, isVME, offset, flowID, calibrate, what)

        char  *host ;
        bool  isVME ;
        int  offset ;
        int32_t  flowID ;
        bool  calibrate ;
        char  *what ;
#    endif

{    /* Local variables. */
    CortexHeader  header ;
    in_port_t  port ;
    int32_t  request[2] ;
    Transaction  *transaction ;




/* Create a transaction object. */

    transaction = (Transaction *) malloc (sizeof (Transaction)) ;
    if (transaction == NULL) {
        LGE "(measure) Error creating transaction object for %s\nmalloc: ",
            host) ;
        return (errno) ;
    }
    transaction->rangingLink = NULL ;
    transaction->rangingID = NULL ;
    transaction->measurementLink = NULL ;
    transaction->measurementID = NULL ;
    transaction->stage = 0 ;

/* Open a connection to the CORTEX unit's measurement port and register
   a function to be called when a transaction response is received. */

    if (insPort ("MEAS", offset, &port)) {
        LGE "(measure) Invalid port \"MEAS\" and offset %d.\n", offset) ;
        return (errno) ;
    }

    if (linkOpen (host, port, isVME, &transaction->measurementLink)) {
        LGE "(measure) Error connecting to CORTEX measurement port: %s\nlinkOpen: ",
            host) ;
        return (errno) ;
    }

    transaction->measurementID =
        ioxOnIO (dispatcher, rspInputCB, transaction, IoxRead,
                 insFd (transaction->measurementLink->stream)) ;

/* Open a connection to the CORTEX unit's ranging port and register
   a function to be called when a transaction response is received. */

    if (insPort (isVME ? "RNG-2" : "RNG", offset, &port)) {
        LGE "(measure) Invalid port \"%s\" and offset %d.\n",
            isVME ? "RNG-2" : "RNG", offset) ;
        return (errno) ;
    }

    if (linkOpen (host, port, isVME, &transaction->rangingLink)) {
        LGE "(measure) Error connecting to CORTEX ranging port: %s\nlinkOpen: ",
            host) ;
        return (errno) ;
    }

    transaction->rangingID =
        ioxOnIO (dispatcher, rspInputCB, transaction, IoxRead,
                 insFd (transaction->rangingLink->stream)) ;

/* Send a range request to the CORTEX unit. */

    request[3-3] = calibrate ? htonl (1) : htonl (0) ;
    if (strcasecmp (what, "PHASE") == 0) {
        request[4-3] = htonl (0) ;
    } else if (strcasecmp (what, "TIME") == 0) {
        request[4-3] = htonl (1) ;
    } else if (!isVME && (strcasecmp (what, "FLOAT") == 0)) {
        request[4-3] = htonl (2) ;
    } else {
        SET_ERRNO (EINVAL) ;
        LGE "(measure) Invalid measurement requested: %s\n", what) ;
        return (errno) ;
    }

    header.length = sizeof request ;  header.flowID = flowID ;
    if (insWrite (transaction->rangingLink->stream, -1.0,
                  &header, (char *) &request)) {
        LGE "(measure) Error sending %s request to CORTEX unit: %s\ninsWrite: ",
            calibrate ? "calibration" : "ranging",
            insName (transaction->rangingLink->stream)) ;
    }

    return (0) ;

}

/*******************************************************************************

Procedure:

    rspInputCB ()

    Read Response Messages from a CORTEX Ranging Port.


Purpose:

    Function rspInputCB() reads and displays any transaction response
    messages waiting to be read from the ranging or measurement port on a
    CORTEX unit.  When a network connection is established with the CORTEX
    unit, the connection is registered with the IOX dispatcher as an input
    source.  Thereafter, whenever input is detected on the connection, the
    IOX dispatcher automatically invokes rspInputCB() to read the messages
    from the CORTEX unit.


    Invocation:

        status = rspInputCB (callback, reason, userData) ;

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
    char  whichTone[16] ;
    CortexHeader  header ;
    int  first, i ;
    int32_t  *body, numMeasurements ;
    Link  link ;
    Transaction  *transaction = (Transaction *) userData ;




/* Determine whether the message is from the ranging or measurement port. */

    if (callback == transaction->rangingID) {
        link = transaction->rangingLink ;
    } else if (callback == transaction->measurementID) {
        link = transaction->measurementLink ;
    } else {
        SET_ERRNO (EINVAL) ;
        LGE "(rspInputCB) Input ID (%p) doesn't match ranging or measurement link.\n",
            callback) ;
        return (errno) ;
    }


/*******************************************************************************
    While more input is available, read and display the next message.
*******************************************************************************/

    while (insIsReadable (link->stream)) {
					/* Read the next message. */
        if (insRead (link->stream, -1.0, &header, (char **) &body)) {
            LGE "(rspInputCB) Error reading from %s.\ninsRead: ",
                insName (link->stream)) ;
            break ;
        }

        printf ("\n%s  (%ld bytes)  Flow ID: %ld\n",
                insName (link->stream),
                (long) header.length, (long) header.flowID) ;

/* If a primary or secondary response was received from the ranging port,
   process it.  After the secondary response is received, close the link
   to the ranging port. */

        if (header.length == 4) {

            insDecode ((char *) body, (xdrproc_t) xdr_int32_t, sizeof (int32_t),
                       header.length, (char *) body) ;

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
                printf ("    Response: invalid syntax\n") ;
                break ;
            case 2:
                printf ("    Response: RAU in self-test mode\n") ;
                break ;
            case 3:
                printf ("    Response: incorrect RAU programming\n") ;
                break ;
            case 10:
                printf ("    Response: CORTEX in local mode\n") ;
                break ;
            case 11:
                printf ("    Response: ranging request in progress\n") ;
                break ;
            case 12:
                printf ("    Response: RAU M&C alarm\n") ;
                break ;
            case 20:
                printf ("    Response: open loop\n") ;
                break ;
            case 21:
                printf ("    Response: loss of lock\n") ;
                break ;
            case 23:
                printf ("    Response: hardware problem\n") ;
                break ;
            case 24:
                printf ("    Response: stop measurement requested\n") ;
                break ;
            default:
                printf ("    Response: (%ld)\n", (long) body[0]) ;
                break ;
            }

            if ((++transaction->stage < 2) && (body[0] == 0))  continue ;

            ioxCancel (callback) ;
            transaction->rangingID = NULL ;
            linkClose (link) ;
            transaction->rangingLink = NULL ;
            if ((body[0] != 0) && (transaction->measurementLink != NULL)) {
                ioxCancel (transaction->measurementID) ;
                linkClose (transaction->measurementLink) ;
                transaction->measurementLink = NULL ;
            }
            if (transaction->measurementLink == NULL)  free (transaction) ;

            return (0) ;

        }

/* If measurement data was received, display it and close the link to the
   measurement port. */

        else {

            insDecode ((char *) body, (xdrproc_t) xdr_int32_t,
                       sizeof (int32_t), 4 * sizeof (int32_t), (char *) body) ;

            printf ("    Time: %9ld.%03ld    %s, %s\n",
                    (long) body[4-3], (long) body[5-3],
                    (body[6-3] == 0) ? "Range Measurement"
                                     : "Calibration Loop",
                    (body[3-3] == 2) ? "Phase/Time (float)"
                                     : "Phase/Time (integer)") ;

            first = 9 - 3 ;

            insDecode ((char *) &body[first-1],
                       (xdrproc_t) xdr_int32_t,
                       0,
                       sizeof (int32_t),
                       (char *) &numMeasurements) ;

            insDecode ((char *) &body[first],
                       (xdrproc_t) xdr_int32_t,
                       sizeof (int32_t),
                       numMeasurements * sizeof (int32_t),
                       (char *) &body[first]) ;

            if (body[3-3] == 2) {
                insDecode ((char *) &body[first-2], (xdrproc_t) xdr_float,
                           0, sizeof (float), (char *) &body[first-2]) ;
                printf ("    # of Time Measurements: %ld    ARQ: %g\n",
                        (long) numMeasurements,
                        *((float *) (&body[first-2]))) ;
            } else {
                insDecode ((char *) &body[first-2], (xdrproc_t) xdr_int32_t,
                           0, sizeof (int32_t), (char *) &body[first-2]) ;
                printf ("    # of Time Measurements: %ld    ARQ: %ld\n",
                        (long) numMeasurements, (long) body[first-2]) ;
            }

            if (header.length >=
                ((first + numMeasurements) * (int) sizeof (int32_t))) {
                dumpData (stdout, first+3, numMeasurements,
                          &body[first], 0, 0) ;
            }

            if (header.length >
                ((first + numMeasurements) * (int) sizeof (int32_t))) {

                for (i = 0 ;  i < 7 ;  i++) {

                    first = first + numMeasurements + 3 ;

                    insDecode ((char *) &body[first-1], (xdrproc_t) xdr_int32_t,
                               0, sizeof (int32_t), (char *) &numMeasurements) ;

                    if (i == 0)
                        strcpy (whichTone, "MAJOR") ;
                    else
                        sprintf (whichTone, "MINOR #%d", i) ;

                    if (body[3-3] == 2) {
                        insDecode ((char *) &body[first-2],
                                   (xdrproc_t) xdr_float, 0, sizeof (float),
                                   (char *) &body[first-2]) ;
                        insDecode ((char *) &body[first],
                                   (xdrproc_t) xdr_float, sizeof (float),
                                   numMeasurements * sizeof (float),
                                   (char *) &body[first]) ;
                        printf ("    # of Phase Measurements: %ld    Correction: %g    %s\n",
                                (long) numMeasurements,
                                *((float *) (&body[first-2])),
                                whichTone) ;
                        if (header.length >=
                            ((first + numMeasurements) *
                             (int) sizeof (int32_t))) {
                            dumpData (stdout, first+3, numMeasurements,
                                      &body[first], 1, 1) ;
                        }
                    } else {
                        insDecode ((char *) &body[first-2],
                                   (xdrproc_t) xdr_int32_t,
                                   0, sizeof (int32_t),
                                   (char *) &body[first-2]) ;
                        insDecode ((char *) &body[first],
                                   (xdrproc_t) xdr_int32_t,
                                   sizeof (int32_t),
                                   numMeasurements * sizeof (int32_t),
                                   (char *) &body[first]) ;
                        printf ("    # of Phase Measurements: %ld    Correction: %ld    %s\n",
                                (long) numMeasurements,
                                (long) body[first-2], whichTone) ;
                        if (header.length >=
                            ((first + numMeasurements) *
                             (int) sizeof (int32_t))) {
                            dumpData (stdout, first+3, numMeasurements,
                                      &body[first], 0, 0) ;
                        }
                    }

                }

            }

            ioxCancel (callback) ;
            transaction->measurementID = NULL ;
            linkClose (link) ;
            transaction->measurementLink = NULL ;
            if (transaction->rangingLink == NULL)  free (transaction) ;

            return (0) ;

        }

    }


/* Check to see if the stream's network connection has been broken.  If so,
   close both the ranging and measurement connections. */

    if (!insIsUp (link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(rspInputCB) Broken connection to %s.\ninsIsUp: ",
            insName (link->stream)) ;
        if (transaction->rangingLink != NULL) {
            ioxCancel (transaction->rangingID) ;
            linkClose (transaction->rangingLink) ;
        }
        if (transaction->measurementLink != NULL) {
            ioxCancel (transaction->measurementID) ;
            linkClose (transaction->measurementLink) ;
        }
        free (transaction) ;
    }


    return (0) ;

}
