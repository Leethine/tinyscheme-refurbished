/* $Id: talknet.c,v 1.11 2021/09/20 03:57:59 alex Exp $ */
/*******************************************************************************

    talknet.c

    Network Talk Utility.


    Alex Measday


    TALKNET is a program that allows you to interactively establish
    and communicate over a network connection.

    TALKNET can function in either of two modes: as a client or as a
    server.  If the program you want to talk to over the network is a
    server program, bring TALKNET up in client mode (its default).
    TALKNET will contact the server, establish a network connection
    with it, and then let you talk to the server.  Anything you type
    in at the keyboard is transmitted to the server; anything the
    server sends back over the network is displayed on your screen.

    If the program you want to talk to over the network is a client
    program, TALKNET can be brought up in server mode (with the
    "-listen" command line option).  In this case, TALKNET listens
    at the network port, waiting for a connection request from the
    client program.  When a call comes in, TALKNET answers it,
    establishes the connection with the client, and then lets you
    talk to the client.

    TALKNET has a lot of command line options, but you can ignore most
    of them.  The most common invocation is:

         % talknet -hex <service>

    ("%" is the Unix C Shell prompt.)  This command tells TALKNET to
    contact the specified server on your own machine; all data
    received from the server is displayed on your screen in a combined
    hexadecimal and ASCII dump ("-hex" for hexadecimal/ASCII dump).
    If the server is on another machine, simply specify the remote
    host's name:

         % talknet -hex <service>@<host>

    If the program you're talking to expects XDR strings (Sun's
    eXternal Data Representation protocol), also specify the "-xdr"
    option:

         % talknet -hex -xdr <service>@<host>

    To run TALKNET as a server, add the "-listen" option; no host name is
    required or allowed:

         % talknet -hex -listen <service>

    To terminate TALKNET, just type ^C (control-C).

    NOTE:  If no format option ("-decimal", "-octal", or "-hexadecimal")
    is specified, the data input from the server is simply written to the
    user's terminal as ASCII text.


    Invocation:

        % talknet [-backlog <number>] [-crlf] [-debug] [-decimal] [-drain]
                  [-hexadecimal] [-listen] [-octal] [-text] [-udp] [-xdr]
                  <service|port>[@<host>]

    where:

        "-backlog <number>"
            specifies the maximum number of outstanding connection requests
            when TALKNET is run as a TCP/IP server (see the "-listen" option).
            The default number is the system maximum.
        "-crlf"
            append CR/LFs to user input before writing it to the network.
        "-debug"
            turns debug on.
        "-decimal"
            dumps each byte as a decimal number.
        "-drain"
            drains the data from the network connection, effectively sending
            it to /dev/null.
        "-hexadecimal"
            dumps every 4 bytes as a hexadecimal number.
        "-listen"
            puts TALKNET in server mode, i.e., rather than talking to a
            server, TALKNET talks to a client - TALKNET is the server.
            TALKNET listens for a connection request from a client, answers
            it, and then carries on a conversation with the client, just
            like it (TALKNET) normally does with a server.
        "-octal"
            dumps each bye as an octal number.
        "-text"
            dumps each byte as text, i.e., the data is not dumped in numeric
            form.
        "-udp"
            specifies that a UDP/IP network connection is to be established.
            The default is to establish a TCP/IP connection.
        "-xdr"
            sets XDR mode.  In this mode, a line of data typed in by the
            user is output on the network as an XDR counted byte string
            embedded in an RPC record.
        "<service|port>[@<host>]"
            specifies the name of the server and, optionally, the name of
            the host on which the server is running.  The server port can
            be specified as a service name (which must be defined in the
            local "/etc/services" file) or as a literal port number.
            If no host is specified, the local host is assumed.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#ifdef _WIN32
#    include  <conio.h>			/* Console I/O functions. */
#endif
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#if defined(VMS)
#    include  <unixio.h>		/* VMS-emulation of UNIX I/O. */
#    include  "vmstypes.h"		/* System type definitions (VMS). */
#elif defined(VXWORKS)
#    include  <selectLib.h>		/* SELECT(2) definitions. */
#    include  <sockLib.h>		/* Socket library definitions. */
#    include  <types.h>			/* System type definitions. */
#    include  <unistd.h>		/* UNIX-specific definitions. */
#    define  exit  return
#else
#    include  <sys/types.h>		/* System type definitions. */
#endif
#include  "meo_util.h"			/* Memory operations. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "skt_util.h"			/* Socket support functions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "tcp_util.h"			/* TCP/IP networking utilities. */
#include  "udp_util.h"			/* UDP/IP networking utilities. */
#include  "aperror.h"			/* APERROR() definitions. */
#include  "xnet_util.h"			/* XNET definitions. */

#define  MAX_STRING  8192

					/* Non-local variables. */
static  bool  useUDP = false ;
static  bool  useXDR = false ;
static  TcpEndpoint  tcpConnection = NULL ;
static  TcpEndpoint  tcpListeningPoint = NULL ;
static  UdpEndpoint  udpDestination = NULL ;
static  UdpEndpoint  udpEndpoint = NULL ;
static  UdpEndpoint  udpSource = NULL ;
static  XnetStream  stream = NULL ;

/*******************************************************************************
    TALKNET's Main Program.
*******************************************************************************/

#ifdef VXWORKS

int  talknet (

#    if PROTOTYPES
        char  *commandLine)
#    else
        commandLine)

        char  *commandLine ;
#    endif

#else

int  main (

#    if PROTOTYPES
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

#endif

{  /* Local variables. */
    bool  appendCRLF, drain, endOfUserInput, isServer ;
    char  *argument, buffer[MAX_STRING], *serverName ;
    const  char  *name ;
    int  backlog, errflg ;
    int  numActive, option ;
    IoFd  fd ;
    fd_set  readMask, readMaskSave ;
    MeoBase  dumpMode ;
    OptContext  scan ;
    size_t  length ;
#ifdef vms
    int  channel ;
    struct  timeval  timeout ;
#endif
#ifdef _WIN32
    struct  timeval  timeout ;
#endif

    static  const  char  *optionList[] = {	/* Command line options. */
        "{backlog:}", "{crlf}", "{debug}", "{decimal}", "{drain}",
        "{hexadecimal}", "{listen}", "{octal}", "{text}", "{udp}", "{xdr}",
        NULL
    } ;





#ifdef VXWORKS
    char  **argv ;
    int  argc ;
		/* Parse command string into an ARGC/ARGV array of arguments. */
    opt_create_argv ("talknet", commandLine, &argc, &argv) ;
#endif

    aperror_print = 1 ;

/* Ignore SIGPIPE signals generated by attempting to write to a broken
   connection. */

#if HAVE_SIGNAL && defined(SIGPIPE)
    signal (SIGPIPE, SIG_IGN) ;
#endif

/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    appendCRLF = false ;  backlog = 99 ;  drain = false ;  dumpMode = MeoText ;
    isServer = false ;  serverName = NULL ;  useUDP = false ;  useXDR = false ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;

    while ((option = opt_get (scan, &argument))) {

        switch (option) {
        case 1:			/* "-backlog <number>" */
            backlog = atoi (argument) ;
            break ;
        case 2:			/* "-crlf" */
            appendCRLF = true ;
            break ;
        case 3:			/* "-debug" */
            tcp_util_debug = 1 ;  udp_util_debug = 1 ;  xnet_util_debug = 1 ;
            break ;
        case 4:			/* "-decimal" */
            dumpMode = MeoDecimal ;
            break ;
        case 5:			/* "-drain" */
            drain = true ;
            break ;
        case 6:			/* "-hexadecimal" */
            dumpMode = MeoHexadecimal ;
            break ;
        case 7:			/* "-listen" */
            isServer = true ;
            break ;
        case 8:			/* "-octal" */
            dumpMode = MeoOctal ;
            break ;
        case 9:			/* "-text" */
            dumpMode = MeoText ;
            break ;
        case 10:		/* "-udp" */
            if (!useXDR)  useUDP = true ;
            break ;
        case 11:		/* "-xdr" */
            useXDR = true ;  useUDP = false ;
            break ;
        case NONOPT:
            serverName = argument ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (scan) ;

    if (errflg || (serverName == NULL)) {
        fprintf (stderr, "Usage:  talknet [-backlog <number>] [-crlf] [-debug] [-decimal] [-drain]\n") ;
        fprintf (stderr, "                [-hexadecimal] [-listen] [-octal] [-text] [-udp] [-xdr]\n") ;
        fprintf (stderr, "                <service|port>[@<host>]\n") ;
        exit (EINVAL) ;
    }

/*******************************************************************************
    Establish a connection with the host/server.
*******************************************************************************/

    if (sktStartup ()) {		/* Initialize networking. */
        exit (errno) ;
    }

    tcpListeningPoint = NULL ;
    tcpConnection = NULL ;
    udpEndpoint = NULL ;
    udpSource = NULL ;
    udpDestination = NULL ;
    stream = NULL ;

    if (isServer) {

        fprintf (stderr, "... \"%s\" waiting for connection request ...\n",
                 serverName) ;

        if (useUDP) {
            if (udpCreate (serverName, NULL, &udpEndpoint)) {
                LGE "[TalkNet] Error creating UDP endpoint.\nudpCreate: ") ;
                exit (errno) ;
            }
            fd = udpFd (udpEndpoint) ;
            name = udpName (udpEndpoint) ;
        } else {
            if (tcpListen (serverName, backlog, &tcpListeningPoint)) {
                LGE "[TalkNet] Error listening for connection requests.\ntcpListen: ") ;
                exit (errno) ;
            }
            if (tcpAnswer (tcpListeningPoint, -1.0, &tcpConnection)) {
                LGE "[TalkNet] Error answering connection request.\ntcpAnswer: ") ;
                exit (errno) ;
            }
            fd = tcpFd (tcpConnection) ;
            name = tcpName (tcpConnection) ;
            if (useXDR && xnetCreate (tcpConnection, NULL, &stream)) {
                LGE "[TalkNet] Error creating XDR stream.\nxnetCreate: ") ;
                exit (errno) ;
            }
        }

    } else {

        if (useUDP) {
            if (udpCreate (NULL, NULL, &udpEndpoint)) {
                LGE "[TalkNet] Error creating UDP endpoint.\nudpCreate: ") ;
                exit (errno) ;
            }
            fd = udpFd (udpEndpoint) ;
            name = udpName (udpEndpoint) ;
            if (udpCreate (serverName, udpEndpoint, &udpDestination)) {
                LGE "[TalkNet] Error creating UDP destination.\nudpCreate: ") ;
                exit (errno) ;
            }
        } else {
            if (tcpCall (serverName, 0, &tcpConnection)) {
                LGE "[TalkNet] Error establishing connection.\ntcpCall: ") ;
                exit (errno) ;
            }
            fd = tcpFd (tcpConnection) ;
            name = tcpName (tcpConnection) ;
            if (useXDR && xnetCreate (tcpConnection, NULL, &stream)) {
                LGE "[TalkNet] Error creating XDR stream.\nxnetCreate: ") ;
                exit (errno) ;
            }
        }

    }

    printf ("\n==>/ TalkNET /==>    %s\n\n", name) ;

#ifdef vms

/* Under VMS, assign a channel to the terminal.  UNIX allows arbitrary file
   descriptors to be specified in SELECT(2) bit masks, but VMS only allows
   sockets.  Consequently, VMS TALKNET will have to periodically poll the
   standard input channel to check for keyboard input. */

    tty_create (NULL, &channel) ;

#endif

/*******************************************************************************
    Wait for input typed in by the user or from the network.  User input is
    written out to the server over the network.  Network input is dumped to
    the user's terminal.
*******************************************************************************/

    endOfUserInput = false ;

    for ( ; ; ) {

        FD_ZERO (&readMaskSave) ;
        FD_SET (fd, &readMaskSave) ;

        for ( ; ; ) {
            readMask = readMaskSave ;
#if defined(vms) || defined(_WIN32)
            timeout.tv_sec = 0 ;  timeout.tv_usec = 500000 ;
            numActive = select (FD_SETSIZE, &readMask, NULL, NULL, &timeout) ;
#else
            if (!endOfUserInput)  FD_SET (fileno (stdin), &readMask) ;
            numActive = select (FD_SETSIZE, &readMask, NULL, NULL, NULL) ;
#endif
            if (numActive >= 0)  break ;
            if (errno == EINTR)  continue ;	/* SELECT interrupted by signal - try again. */
            LGE "[TalkNet] Error selecting input.\nselect: ") ;
            exit (errno) ;
        }

#if defined(vms)
        if (!endOfUserInput && (tty_poll (channel) > 0)) {
#elif defined(_WIN32)
        if (kbhit ()) {
#else
        if (FD_ISSET (fileno (stdin), &readMask)) {
#endif

            if (fgets (buffer, sizeof buffer, stdin) == NULL) {
                if (feof (stdin)) {
                    fprintf (stderr,
 "[TalkNet] End of user input - reading from network only (^C to exit) ...\n") ;
                    endOfUserInput = true ;
                    continue ;
                } else {
                    LGE "[TalkNet] Error reading user input.\nfgets: ") ;
                    exit (errno) ;
                }
            }

            length = strlen (buffer) ;
            if ((length > 0) && (buffer[length-1] == '\n'))
                buffer[length-1] = '\0' ;
            if (appendCRLF)  strcat (buffer, "\r\n") ;
#ifdef NOT_YET
            strConvert (buffer) ;	/* Convert embedded "\nnn" sequences. */
#endif
            length = strlen (buffer) ;

            if (useUDP) {
                if (udpWrite (udpDestination, -1.0, length, buffer)) {
                    LGE "[TalkNet] Error writing to network.\nudpWrite: ") ;
                }
            } else if (useXDR) {
                if (xnetWrite (stream, -1.0, "%.*s", (int) length, buffer)) {
                    LGE "[TalkNet] Error writing to network.\nxnetWrite: ") ;
                    exit (errno) ;
                }
            } else {
                if (tcpWrite (tcpConnection, -1.0, length, buffer, NULL)) {
                    LGE "[TalkNet] Error writing to network.\ntcpWrite: ") ;
                    exit (errno) ;
                }
            }

        }     /* User input? */


        if (FD_ISSET (fd, &readMask)) {

            if (useUDP) {
                if (udpRead (udpEndpoint, -1.0, (ssize_t) MAX_STRING,
                             buffer, &length, &udpSource)) {
                    LGE "[TalkNet] Error reading network input.\nudpRead: ") ;
                    exit (errno) ;
                }
                udpDestination = udpSource ;
            } else {
                if (tcpRead (tcpConnection, -1.0, (ssize_t) -MAX_STRING,
                             buffer, &length)) {
                    if (errno == EPIPE) {
                        LGE "[TalkNet] End of connection.\ntcpRead: ") ;
                        exit (0) ;
                    } else {
                        LGE "[TalkNet] Error reading network input.\ntcpRead: ") ;
                        exit (errno) ;
                    }
                } else {
                    printf ("[TalkNet] Read %"PRIuSIZE_T" bytes.\n", length) ;
                }
            }

            if (!drain)  switch (dumpMode) {

            case MeoDecimal:
                meoDumpD (stdout, NULL, 0, buffer, length) ;
                break ;

            case MeoOctal:
                meoDumpO (stdout, NULL, 0, buffer, length) ;
                break ;

            case MeoText:
                buffer[length] = '\0' ;
                printf ("%s", buffer) ;
                break ;

            case MeoHexadecimal:
            default:
                meoDumpX (stdout, NULL, 0, buffer, length) ;
                break ;

            }

        }     /* Network input? */


    }     /* Wait for input. */

}
