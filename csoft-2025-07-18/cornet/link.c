/* $Id: link.c,v 1.3 2003/09/16 19:28:37 alex Exp alex $ */
/*******************************************************************************

File:

    link.c

    CORTEX Link Objects.


Author:    Alex Measday


Purpose:

    A Link object represents the network connection with a CORTEX unit.


Public Procedures:

    linkClose() - closes a link.
    linkOpen() - establishes a network connection with a CORTEX unit.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "tcp_util.h"			/* TCP/IP networking utilities. */
#include  "cornet.h"			/* CORNET definitions. */

/*******************************************************************************

Procedure:

    linkClose ()

    Close a Link to a CORTEX Unit.


Purpose:

    Function linkClose() closes the network connection to a CORTEX unit.


    Invocation:

        status = linkClose (link) ;

    where:

        <link>		- I
            is the link handle returned by linkOpen().
        <status>	- O
            returns the status of closing the link, zero if there were
            no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  linkClose (

#    if PROTOTYPES
        Link  link)
#    else
        link)

        Link  link ;
#    endif

{

    if (link == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(linkClose) NULL link handle.\n") ;
        return (errno) ;
    }

/* Close the network connection to the CORTEX unit. */

    if (link->stream != NULL) {
        insDestroy (link->stream) ;
        link->stream = NULL ;
    }

/* Deallocate the Link structure. */

    free (link) ;

    return (0) ;

}

/*******************************************************************************

Procedure:

    linkOpen ()

    Open a Link to a CORTEX Unit.


Purpose:

    Function linkOpen() establishes a network connection with a specified port
    on a CORTEX unit.  The calling program can access the network connection
    using the "stream" member of the Link structure.


    Invocation:

        status = linkOpen (host, port, isVME, &link) ;

    where:

        <host>		- I
            is the host name or IP address of the CORTEX unit.
        <port>		- I
            is the service port to which a connection is established.
        <isVME>		- I
            specifies whether the CORTEX unit is VME-based (true) or
            NT-based (false).
        <link>		- O
            returns a pointer to a dynamically-allocated link structure.
        <status>	- O
            returns the status of establishing the connection, zero if there
            were no errors and ERRNO otherwise.

*******************************************************************************/


errno_t  linkOpen (

#    if PROTOTYPES
        const  char  *host,
        int  port,
        bool  isVME,
        Link  *link)
#    else
        host, port, isVME, link)

        char  *host ;
        int  port ;
        bool  isVME ;
        Link  *link ;
#    endif

{    /* Local variables. */
    char  serverName[128] ;
    TcpEndpoint  connection ;




    if (host == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(linkOpen) No host name for CORTEX.\n") ;
        return (errno) ;
    }

/* Establish a network connection with the CORTEX unit. */

    sprintf (serverName, "%d@%s", port, host) ;

    if (tcpCall (serverName, 0, &connection)) {
        LGE "(linkOpen) Error connecting to %s.\ntcpCall: ", serverName) ;
        return (errno) ;
    }

/* Allocate a link structure for the connection. */

    *link = calloc (1, sizeof (_Link)) ;
    if (*link == NULL) {
        LGE "(linkOpen) Error allocating link structure for %s.\ncalloc: ",
            serverName) ;
        return (errno) ;
    }

    (*link)->port = port ;
    (*link)->isVME = isVME ;

/* Create a CORTEX communications stream for the connection. */

    if (insCreate (connection, &(*link)->stream)) {
        LGE "(linkOpen) Error creating CORTEX stream for %s.\ninsCreate: ",
            serverName) ;
        return (errno) ;
    }

    return (0) ;

}
