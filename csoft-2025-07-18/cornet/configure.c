/* $Id: configure.c,v 1.8 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    configure.c

    Configure CORTEX Components.


Author:    Alex Measday


Purpose:

    A collection of functions for configuring components in a CORTEX unit.


Public Procedures:

    configure() - configures a component on a CORTEX unit.

Private Procedures:

    ackInputCB() - reads acknowledgement messages from a CORTEX control port.

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

    configure ()

    Configure a CORTEX Component.


Purpose:

    Function configure() connects to a CORTEX unit's control port, constructs
    a control message to configure a component, and sends the message to the
    CORTEX unit.  The network connection to the CORTEX is left open so the
    configuration acknowledgement message can be received.


    Invocation:

        status = configure (host, isVME, offset, configuration) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX unit.
        <isVME>		- I
            specifies whether the CORTEX unit is VME-based (true) or
            NT-based (false).
        <offset>	- I
            is an offset to be added to the CORTEX CTRL port number (i.e.,
            port 3001).  Specifying a non-zero offset allows CORNET to
            connect to "virtual" CORTEX units listening at different
            service ports on a single host.
        <configuration>	- I
            specifies the component to be configured, the parameters
            to be changed, and the new values for those parameters.
            The configuration is specified as a string containing a
            list of fields separated by commas.  The first field in
            the string is the name of the component (table) being
            reconfigured.  Subsequent pairs of fields specify a
            parameter (identified by its offset in the component's
            table) and its new value.  The value MUST be specified
            according to the type of its parameter; e.g., a float
            parameter being set to 123 must be specified as "123.0".
        <status>	- O
            returns the status of sending the configuration message to the
            CORTEX unit's control port, zero if there were no errors and
            ERRNO otherwise.

*******************************************************************************/


errno_t  configure (

#    if PROTOTYPES
        const  char  *host,
        bool  isVME,
        int  offset,
        const  char  *configuration)
#    else
        host, isVME, offset, configuration)

        char  *host ;
        bool  isVME ;
        int  offset ;
        char  *configuration ;
#    endif

{    /* Local variables. */
    char  *arg, *s, *spec ;
    CortexHeader  header ;
    in_port_t  port ;
    int32_t  field[256] ;
    int  numFields ;
    Link  link ;




/* Construct the control message while parsing the configuration string.
   First, the name of the component being configured ... */

    spec = strdup (configuration) ;
    if (spec == NULL) {
        LGE "(configure) Error duplicating %s's configuration string: \"%s\"\nstrdup: ",
            host, configuration) ;
        return (errno) ;
    }

    arg = strtok (spec, ",") ;
    if (arg == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(configure) No component name in %s's configuration string: \"%s\"\n",
            host, configuration) ;
        free (spec) ;
        return (errno) ;
    }

    numFields = 0 ;
    field[numFields++] = htonl (insPart (arg, isVME)) ;
    field[numFields++] = '?' ;				/* # of parameters. */

/* Each pair of subsequent fields consists of a parameter (specified by its
   offset in the component's monitoring table) and the parameter's value
   (which must be specified according to the type of the parameter). */

    while (NULL != (arg = strtok (NULL, ","))) {

        field[numFields++] = htonl (atoi (arg)) ;	/* Parameter offset. */

        if (NULL == (arg = strtok (NULL, ","))) {	/* Parameter value. */
            SET_ERRNO (EINVAL) ;
            LGE "(configure) Missing parameter value in %s's configuration string: \"%s\"\n",
                host, configuration) ;
            free (spec) ;
            return (errno) ;
        }

        field[numFields] = strtol (arg, &s, 0) ;	/* Try as integer. */
        if (*s != 0) {					/* Field consumed? */
            float  real = (float) strtod (arg, &s) ;	/* No, try as float. */
            if (*s != 0) {
                SET_ERRNO (EINVAL) ;
                LGE "(configure) Invalid parameter value in %s's configuration string: \"%s\"\n",
                    host, configuration) ;
                free (spec) ;
                return (errno) ;
            }
            memcpy (&field[numFields], &real, sizeof (int32_t)) ;
        }

        field[numFields] = htonl (field[numFields]) ;
        numFields++ ;

    }

/* Go back and set the "# of parameters" field in the message. */

    field[1] = htonl ((numFields - 2) / 2) ;		/* # of parameters. */

    free (spec) ;

/* Establish a network connection with the CORTEX unit's control port. */

    if (insPort ("CTRL", offset, &port)) {
        LGE "(configure) Invalid port \"CTRL\" and offset %d.\n", offset) ;
        return (errno) ;
    }

    if (linkOpen (host, port, isVME, &link)) {
        LGE "(configure) Error connecting to %s's control port\nlinkOpen: ", host) ;
        return (errno) ;
    }

/* Send the configuration message to the CORTEX unit. */

    header.length = numFields * sizeof (int32_t) ;  header.flowID = 1 ;

    if (insWrite (link->stream, -1.0, &header, (char *) field)) {
        LGE "(configure) Error sending configuration message to %s.\ninsWrite: ",
            insName (link->stream)) ;
        return (errno) ;
    }

/* Register an input callback for the configuration acknowledgement returned
   by the CORTEX unit. */

    ioxOnIO (dispatcher, ackInputCB, link, IoxRead, insFd (link->stream)) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    ackInputCB ()

    Read Acknowledgement Messages from a CORTEX Control Port.


Purpose:

    Function ackInputCB() reads and displays any acknowledgement messages
    waiting to be read from the control port on a CORTEX unit.  When a
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

        if (header.length == 4) {
            switch (body[0]) {
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
                printf ("    Acknowledgement: invalid syntax\n") ;
                break ;
            case 2:
                printf ("    Acknowledgement: in local mode\n") ;
                break ;
            case 3:
                printf ("    Acknowledgement: component not mounted\n") ;
                break ;
            case 4:
                printf ("    Acknowledgement: not a remotely configurable parameter\n") ;
                break ;
            case 5:
                printf ("    Acknowledgement: not available in current version\n") ;
                break ;
            case 6:
                printf ("    Acknowledgement: TCU mode change impossible (connection active)\n") ;
                break ;
            case 7:
                printf ("    Acknowledgement: bad command size\n") ;
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
