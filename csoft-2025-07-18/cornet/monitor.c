/* $Id: monitor.c,v 1.9 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    monitor.c

    Monitor CORTEX Tables.


Author:    Alex Measday


Purpose:

    A collection of functions for monitoring CORTEX configuration/status tables.


Public Procedures:

    monitorTable() - initiates the periodic request for and receipt of
        a configuration/status table from a CORTEX unit.

Private Procedures:

    inputTableCB() - reads monitoring messages from a CORTEX unit.
    requestTableCB() - periodically requests monitoring updates
        from a CORTEX unit.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "cornet.h"			/* CORNET definitions. */


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  inputTableCB (
#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData
#    endif
    ) ;

static  errno_t  requestTableCB (
#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData
#    endif
    ) ;

/*******************************************************************************

Procedure:

    monitorTable ()

    Monitor CORTEX Status Tables.


Purpose:

    Function monitorTable() connects to a CORTEX unit's monitoring port
    and periodically requests the current contents of the desired table.


    Invocation:

        status = monitorTable (host, isVME, offset, flowID, name) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX unit.
        <isVME>		- I
            specifies whether the CORTEX unit is VME-based (true) or
            NT-based (false).
        <offset>	- I
            is an offset to be added to the CORTEX MON port number (i.e.,
            port 3000).  Specifying a non-zero offset allows CORNET to
            connect to "virtual" CORTEX units listening at different
            service ports on a single host.
        <flowID>	- I
            is the monitoring data flow identification.
        <name>		- I
            is the name of the table to be monitored.
        <status>	- O
            returns the status of initiating the monitoring,
            zero if there were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  monitorTable (

#    if PROTOTYPES
        const  char  *host,
        bool  isVME,
        int  offset,
        int32_t  flowID,
        const  char  *name)
#    else
        host, isVME, offset, flowID, name)

        char  *host ;
        bool  isVME ;
        int  offset ;
        int32_t  flowID ;
        char  *name ;
#    endif

{    /* Local variables. */
    char  *s ;
    in_port_t  port ;
    int  table ;
    Link  link ;




/* Translate the table name to its component number. */

    table = strtoul (name, &s, 0) ;
    if (*s != 0) {
        table = insPart (name, isVME) ;
        if (table < 0) {
            SET_ERRNO (EINVAL) ;
            LGE "(monitorTable) Error translating table name: %s\ninsPart: ",
                name) ;
            return (errno) ;
        }
    }

/* Establish a network connection to the monitoring port on the CORTEX unit. */

    if (insPort ("MON", offset, &port)) {
        LGE "(monitorTable) Invalid port \"MON\" and offset %d.\n", offset) ;
        return (errno) ;
    }

    if (linkOpen (host, port, isVME, &link)) {
        LGE "(monitorTable) Error connecting to %s CORTEX monitoring port.\nlinkOpen: ",
            host) ;
        return (errno) ;
    }
    link->flowID = flowID ;

/* Register a function to be called when a table is received. */

    ioxOnIO (dispatcher, inputTableCB, link, IoxRead, insFd (link->stream)) ;

/* Make an initial request for the desired table from the CORTEX and schedule
  a periodic timer to request subsequent updates. */

    link->table = table ;

    ioxEvery (dispatcher, requestTableCB, link, 0.0, 30.0) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    inputTableCB ()

    Read Table from a CORTEX Unit.


Purpose:

    Function inputTableCB() reads and displays tables waiting to be read
    from the monitoring port of a CORTEX unit.  When a network connection
    is established with the CORTEX unit, the connection is registered with
    the IOX dispatcher as an input source.  Thereafter, whenever input is
    detected on the connection, the IOX dispatcher automatically invokes
    inputTableCB() to read the messages from the CORTEX unit.


    Invocation:

        status = inputTableCB (callback, reason, userData) ;

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
            inputTableCB() directly.

*******************************************************************************/


static  errno_t  inputTableCB (

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
    int  numFields ;
    int32_t  *body, table ;




/* While more input is available, read and display the next message. */

    while (insIsReadable (link->stream)) {
					/* Read the next message. */
        if (insRead (link->stream, -1.0, &header, (char **) &body)) {
            LGE "(inputTableCB) Error reading from %s.\ninsRead: ",
                insName (link->stream)) ;
            break ;
        }

        insDecode ((char *) body, (xdrproc_t) xdr_int32_t, sizeof (int32_t),
                   header.length, (char *) body) ;

        table = *body++ ;

        printf ("\n%s  (%ld bytes)  Flow ID: %ld  Table: %s\n",
                insName (link->stream), (long) header.length,
                (long) header.flowID, insPartName (table)) ;

        header.length -= sizeof (int32_t) ;
        numFields = header.length / sizeof (int32_t) ;

        if (table == insPart ("Version", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 1, 0) ;
        } else if (table == insPart ("Global", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("TCU", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("TMS", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("RAU", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("RAU-1", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 0) ;
        } else if (table == insPart ("RAU-2", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 0) ;
        } else if (table == insPart ("IFM", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 0) ;
        } else if (table == insPart ("IFM-1", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("IFM-2", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("IFR-1", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("IFR-2", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("IFR-3", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("PMR-A", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 0) ;
        } else if (table == insPart ("PMR-B", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 0) ;
        } else if (table == insPart ("PMR-C", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 0) ;
        } else if (table == insPart ("TMU-A", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("TMU-B", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("TMU-C", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("TMU-D", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("TMU-E", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("TMU-F", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, 1) ;
        } else if (table == insPart ("Noise", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("Project", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else if (table == insPart ("All", link->isVME)) {
            dumpData (stdout, 0, numFields, body, 0, !link->isVME) ;
        } else {
            dumpData (stdout, 0, numFields, body, 1, !link->isVME) ;
        }

    }

/* Check to see if the stream's network connection has been broken.  If so,
   close the connection and exit the program. */

    if (!insIsUp (link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(inputTableCB) Broken connection to %s.\ninsIsUp: ",
            insName (link->stream)) ;
        ioxCancel (callback) ;
        linkClose (link) ;
    }

    return (0) ;

}

/*******************************************************************************

    requestTableCB ()

    Request a Table from the CORTEX Unit.


    Function requestTableCB() is inovked periodically to send a table request
    to the CORTEX unit's monitoring port.


    Invocation:

        status = requestTableCB (callback, reason, userData) ;

    where:

        <callback>	- I
            is the handle assigned to the callback by ioxOnIO().
        <reason>	- I
            is the reason (i.e., IoxFire) the callback is being invoked.
        <userData>	- I
            is the address of the CORTEX link structure returned by linkOpen().
        <status>	- O
            returns the status of handling the timeout, zero if there were
            no errors and ERRNO otherwise.  The status value is ignored by
            the IOX dispatcher, but it may be of use if the application calls
            requestTableCB() directly.

*******************************************************************************/


static  errno_t  requestTableCB (

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
    int32_t  table ;



/* Send a request for the table to the CORTEX unit's monitoring port. */

    table = (int32_t) htonl (link->table) ;
    header.length = sizeof (table) ;  header.flowID = link->flowID ;

    if (insWrite (link->stream, -1.0, &header, (char *) &table)) {
        LGE "(requestTableCB) Error sending 0x%lX table request to CORTEX unit: %s\ninsWrite: ",
            (long) link->table, insName (link->stream)) ;
        return (errno) ;
    }

    return (0) ;

}
