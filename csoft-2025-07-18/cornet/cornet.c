/* $Id: cornet.c,v 1.11 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

Process:

    cornet

    CORTEX Network Tool


Author:    Alex Measday


Purpose:

    Program CORNET is a simple tool used to monitor events, status, and
    telemetry from an IN-SNEC CORTEX unit.


    Invocation:

        % cornet [-debug] [-help]
                 [-nt] [-vme]
                 [-flow <ID>] [-norq] [-offset <number>]
                 <host>
                 [-calibrate PHASE|TIME|FLOAT]
                 [-configure <component>,[<parameter>,<value>]*]
                 [-connect <port>] [-events] [-initialize]
                 [-monitor <table>]
                 [-range PHASE|TIME|FLOAT|<sequence>]
                 [-reset <port>]
                 [-tc <operation>]
                 [-tmh A|B|C|D|E|F] [-tmr A|B|C|D|E|F]

    where

        "-debug"
            enables debug output (written to STDOUT).
        "-help"
            displays information about running CORNET.
        "-nt"
        "-vme"
            specifies the type of CORTEX unit, NT-based (the default) or
            VME-based.
        "-flow <ID>"
            specifies the data flow indentification (NT-based CORTEX only)
            used in subsequent options; the default ID is 0.
        "-norq"
            specifies that no telemetry request is to be sent to an NT-based
            CORTEX.  Normally, a telemetry request containing the desired TMU
            channel must be sent to a real CORTEX-NT in order to initiate the
            flow of telemetry.  Some single-channel, CORTEX-NT-emulating
            simulators, however, don't want such a request.
        "-offset <number>"
            specifies an offset to be added to the standard CORTEX port numbers
            (e.g., CORTEX-NT's TM port 3070) for subsequent operations.  This
            option allows CORNET to connect to "virtual" CORTEX units listening
            at different service ports on a single host.
        "<host>"
            specifies the host name or IP address of the CORTEX unit.
        "-calibrate PHASE|TIME|FLOAT"
            requests a calibration loop measurement.  The argument to this
            option specifies what to measure (case is ignored): PHASE for phase
            and time measurements (integer), TIME for time measurements only,
            or FLOAT for phase and time measurements (float).  (CORTEX-VME
            requests go to the RAU-2 ranging unit; ranging via RAU-1 is not
            supported.)
        "-configure <component>,[<parameter>,<value>]*"
            configures a component on the CORTEX unit.  The option argument
            is a string containing a list of fields separated by commas with
            no intervening spaces.  The first field in the string is the name
            of the component (table) being reconfigured.  Subsequent pairs of
            fields specify a parameter (identified by its offset in the
            component's table) and its new value.  The value MUST be specified
            according to the type of its parameter; e.g., a float parameter
            being set to 123 must be specified as "123.0".
        "-connect <port>"
            connects to an arbitrary port on the CORTEX unit; the port must be
            specified by number, not by name.
        "-events"
            specifies that CORTEX event messages are to received and displayed.
        "-monitor <table>"
            causes the specified status table(s) to be periodically requested
            from the CORTEX unit and, when the table is received, displayed.
        "-range PHASE|TIME|FLOAT|<sequence>"
            requests a ranging measurement.  The argument to this option
            specifies what to measure (case is ignored): PHASE for phase and
            time measurements (integer), TIME for time measurements only, or
            FLOAT for phase and time measurements (float).  (CORTEX-VME
            requests go to the RAU-2 ranging unit.)  If the argument to this
            option is a number, that many measurement sequences are requested
            from the RAU-1 ranging unit on a CORTEX-VME unit.
        "-reset <port>"
            causes the CORTEX unit to disconnect any clients on the specified
            port.  The port may be specified by name or as a number (see the
            "Reset Port" section of the ICD.
        "-tc <operation>"
            sends a telecommand instruction or request to the CORTEX unit.
            Allowed operations are "COMMAND" (a dummy command), "EXECUTE",
            and "NOP".
        "-tmh A|B|C|D|E|F"
        "-tmr A|B|C|D|E|F"
            initiates the sending of frames from the specified telemetry
            channel.  (CORTEX-VME only supports channels A, B, and C.)  The
            "-tmr" option directs the CORTEX unit to send real-time telemetry.
            The "-tmh" option directs the CORTEX unit to first send any
            buffered frames (i.e., history) and then send real-time telemetry.
            CORTEX-NT handles all telemetry channels through a single server
            port; CORTEX-VME has separate server ports for each channel.
            Consequently, it is important that the CORTEX type ("-nt" or
            "-vme") be specified on the command line.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <ctype.h>			/* Standard character functions. */
#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C Library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "net_util.h"			/* Networking utilities. */
#include  "opt_util.h"			/* Option scanning definitions. */
#include  "str_util.h"			/* String manipulation functions. */
#include  "tcp_util.h"			/* TCP/IP networking utilities. */
#include  "ins_util.h"			/* IN-SNEC CORTEX utilities. */
#include  "cornet.h"			/* CORNET definitions. */


int  debug = 0 ;			/* Debug switch. */
IoxDispatcher  dispatcher = NULL ;	/* I/O event dispatcher. */


/*******************************************************************************
    Private Functions.
*******************************************************************************/

static  errno_t  evtInputCB (
#    if PROTOTYPES
        IoxCallback  callback,
        IoxReason  reason,
        void  *userData
#    endif
    ) ;

/*******************************************************************************
    CORNET's Main Program.
*******************************************************************************/

extern  int  main P_((int argc, char *argv[]))  OCD ("sio_appl") ;

int  main (

#    if PROTOTYPES
        int  argc,
        char  *argv[])
#    else
        argc, argv)

        int  argc ;
        char  *argv[] ;
#    endif

{  /* Local variables. */
    bool  help, isVME, noRequest ;
    char  *argument, *host, *s ;
    CortexHeader  header ;
    in_port_t  port ;
    int  errflg, offset, option ;
    int32_t  field[16], flowID, value ;
    Link  link ;
    OptContext  scan ;

    static  const  char  *optionList[] = {	/* Command line options. */
        "{calibrate:}", "{configure:}", "{connect:}",
        "{debug}", "{events}", "{flow:}", "{help}",
        "{initialize}", "{monitor:}",
        "{norq}", "{nt}", "{offset:}",
        "{range:}", "{reset:}",
        "{tc:}", "{tmh:}", "{tmr:}",
        "{vme}", NULL
    } ;





#if HAVE_SIGNAL && defined(SIGPIPE)
    signal (SIGPIPE, SIG_IGN) ;
#endif
    aperror_print = 1 ;			/* Enable APERROR output. */


/*******************************************************************************
    Scan the command line options.
*******************************************************************************/

    if (sktStartup ()) {		/* Initialize networking. */
        exit (errno) ;
    }

    ioxCreate (&dispatcher) ;

    host = strdup (netHostOf (netAddrOf (NULL), 0)) ;
    flowID = 0 ;  isVME = false ;  noRequest = false ;  offset = 0 ;

    opt_init (argc, argv, NULL, optionList, &scan) ;
    errflg = 0 ;  help = false ;

    while ((option = opt_get (scan, &argument))) {

        switch (option) {
        case 1:			/* "-calibrate PHASE|TIME|FLOAT" */
            if (measure (host, isVME, offset, flowID, true, argument))  errflg++ ;
            break ;
        case 2:			/* "-configure <component>,<parameter>,<value>" */
            if (configure (host, isVME, offset, argument))  errflg++ ;
            break ;
        case 3:			/* "-connect <port>" */
            if (linkOpen (host, atoi (argument), isVME, &link)) {
                errflg++ ;  break ;
            }
            ioxOnIO (dispatcher, inputCB, link, IoxRead, insFd (link->stream)) ;
            break ;
        case 4:			/* "-debug" */
            ins_util_debug = 1 ;
            tcp_util_debug = 1 ;
            break ;
        case 5:			/* "-events" */
            if (insPort ("LOG", offset, &port)) {
                errflg++ ;  break ;
            }
            if (linkOpen (host, port, isVME, &link)) {
                errflg++ ;  break ;
            }
            ioxOnIO (dispatcher, evtInputCB, link, IoxRead,
                     insFd (link->stream)) ;
            break ;
        case 6:			/* "-flow <ID>" */
            flowID = (int32_t) strtol (argument, &s, 0) ;
            if (*s != '\0')  errflg++ ;
            break ;
        case 7:			/* "-help" */
            help = true ;
            break ;
        case 8:			/* "-initialize" */
            if (insPort ("CTRL", offset, &port)) {
                errflg++ ;  break ;
            }
            if (linkOpen (host, port, isVME, &link)) {
                errflg++ ;  break ;
            }
            ioxOnIO (dispatcher, inputCB, link, IoxRead, insFd (link->stream)) ;
            field[0] = 0 ;
            field[1] = htonl (1) ;
            field[2] = ~0 ;
            field[3] = 0 ;
            header.length = sizeof (int32_t) * 4 ;  header.flowID = 1 ;
            insWrite (link->stream, -1.0, &header, (char *) field) ;
            break ;
        case 9:			/* "-monitor <table>" */
            if (monitorTable (host, isVME, offset, flowID, argument))
                errflg++ ;
            break ;
        case 10:		/* "-norq" */
            noRequest = true ;
            break ;
        case 11:		/* "-nt" */
            isVME = false ;
            break ;
        case 12:		/* "-offset <number>" */
            offset = (int) strtol (argument, &s, 0) ;
            if (*s != 0)  errflg++ ;
            break ;
        case 13:		/* "-range PHASE|TIME|FLOAT|<sequence>" */
            value = (int32_t) strtoul (argument, &s, 0) ;
            if (*s == 0) {		/* Ranging sequence from VME RAU-1. */
                if (sequence (host, offset, flowID, (int) value))
                    errflg++ ;
            } else {			/* Measure from VME RAU-2 or NT. */
                if (measure (host, isVME, offset, flowID, false, argument))
                    errflg++ ;
            }
            break ;
        case 14:		/* "-reset <port>" */
            value = (int32_t) strtol (argument, &s, 0) ;
            if (*s == 0) {
                value += offset ;
                if ((value < 1) || (IN_PORT_MAX < value)) {
                    errflg++ ;  break ;
                }
            } else {
                if (insPort (argument, 0, &port)) {
                    errflg++ ;  break ;
                }
                value = (int32_t) port ;
            }
            if (insPort ("RST", offset, &port)) {
                errflg++ ;  break ;
            }
            if (linkOpen (host, port, isVME, &link)) {
                errflg++ ;  break ;
            }
            value = (int32_t) htonl (value) ;
            header.length = sizeof value ;  header.flowID = 0xFFFFFFFF ;
            insWrite (link->stream, -1.0, &header, (char *) &value) ;
            insDestroy (link->stream) ;
            linkClose (link) ;
            exit (0) ;
        case 15:		/* "-tc <operation>" */
            if (sendTC (host, isVME, offset, flowID, argument))  errflg++ ;
            break ;
        case 16:		/* "-tmh A|B|C|D|E|F" */
            if (startTLM (host, isVME, noRequest, offset, flowID,
                          toupper ((unsigned char) *argument)
                          - (int) ((unsigned char) 'A'), true))
                errflg++ ;
            break ;
        case 17:		/* "-tmr A|B|C|D|E|F" */
            if (startTLM (host, isVME, noRequest, offset, flowID,
                          toupper ((unsigned char) *argument)
                          - (int) ((unsigned char) 'A'), false))
                errflg++ ;
            break ;
        case 18:		/* "-vme" */
            isVME = true ;
            break ;
        case NONOPT:
            host = argument ;
            break ;
        case OPTERR:
            errflg++ ;  break ;
        default :  break ;
        }

    }

    opt_term (scan) ;

    if (errflg || help || (host == NULL)) {

        if (help)  fprintf (stderr, "\n") ;

        fprintf (stderr, "Usage:  cornet [-debug] [-flow <ID>] [-help] [-norq] [-nt] [-vme]\n") ;
        fprintf (stderr, "               <host>\n") ;
        fprintf (stderr, "               [-calibrate PHASE|TIME|FLOAT]\n") ;
        fprintf (stderr, "               [-configure <table>,[<parameter>,<value>]*]\n") ;
        fprintf (stderr, "               [-connect <port>] [-events] [-initialize\n") ;
        fprintf (stderr, "               [-monitor <table>] [-offset <number>]\n") ;
        fprintf (stderr, "               [-range PHASE|TIME|FLOAT|<sequence>]\n") ;
        fprintf (stderr, "               [-reset <port>] [-tc <operation>]\n") ;
        fprintf (stderr, "               [-tmh A|B|C|D|E|F] [-tmr A|B|C|D|E|F]\n") ;

        if (help) {
            fprintf (stderr, "\n") ;
            fprintf (stderr, "Notes: - You must specify \"-vme\" for VME-based CORTEX units;\n") ;
            fprintf (stderr, "         NT is the default.  Some single-channel simulators\n") ;
            fprintf (stderr, "         emulate the CORTEX-NT, but do not accept a TM request;\n") ;
            fprintf (stderr, "         specify the \"-norq\" option to prevent the request from\n") ;
            fprintf (stderr, "         being sent.\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - You must specify the host (either name or IP address)\n") ;
            fprintf (stderr, "         before any of the options that actually establish a\n") ;
            fprintf (stderr, "         connection with the CORTEX unit.\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - NT ports (case ignored):\n") ;
            fprintf (stderr, "            MON     TC      SIM     DOP\n") ;
            fprintf (stderr, "            CTRL    MEAS    RNG     TM\n") ;
            fprintf (stderr, "            RST     LOG     SPA     All\n") ;
            fprintf (stderr, "       - VME ports (case ignored):\n") ;
            fprintf (stderr, "            MON     TC      TMA-R   TMB-R   TMC-R   RNG-1\n") ;
            fprintf (stderr, "            CTRL    MEAS    TMA-H   TMB-H   TMC-H   RNG-2\n") ;
            fprintf (stderr, "            RST     LOG     TMS     All\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - NT tables (case ignored):\n") ;
            fprintf (stderr, "            TCU     IFR-1   TMU-A   Version\n") ;
            fprintf (stderr, "            TMS     IFR-2   TMU-B   Global [Table]\n") ;
            fprintf (stderr, "            RAU     IFR-3   TMU-C   Noise [Generator]\n") ;
            fprintf (stderr, "                    IFM-1   TMU-D   Project[-specific]\n") ;
            fprintf (stderr, "                    IFM-2   TMU-E   All [Tables]\n") ;
            fprintf (stderr, "                            TMU-F\n") ;
            fprintf (stderr, "       - VME tables (case ignored):\n") ;
            fprintf (stderr, "            TCU     PMR-A   TMU-A   Version\n") ;
            fprintf (stderr, "            TMS     PMR-B   TMU-B   Global [Table]\n") ;
            fprintf (stderr, "            RAU-1   PMR-C   TMU-C   Noise [Generator]\n") ;
            fprintf (stderr, "            RAU-2   IFM             Project[-specific]\n") ;
            fprintf (stderr, "                                    All [Tables]\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - The \"-configure\" option sets the values of zero or more\n") ;
            fprintf (stderr, "         parameters in the specified table.  Each parameter is\n") ;
            fprintf (stderr, "         specified by its offset in the table (see the CORTEX\n") ;
            fprintf (stderr, "         ICD).  The parameter's value MUST be correctly expressed\n") ;
            fprintf (stderr, "         as an integer or a floating point number, depending on\n") ;
            fprintf (stderr, "         its type in the ICD.\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - The \"-tmh\" and \"-tmr\" options initiate the reception of\n") ;
            fprintf (stderr, "         telemetry.  The only difference between the two is that\n") ;
            fprintf (stderr, "         \"-tmh\" causes the CORTEX to initially send any buffered\n") ;
            fprintf (stderr, "         frames, after which both \"-tmh\" and \"-tmr\" receive\n") ;
            fprintf (stderr, "         real-time data.  The VME-based CORTEX only supports\n") ;
            fprintf (stderr, "         channels A, B, and C (case is ignored).\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - The \"-calibrate\" and \"-range\" options request the\n") ;
            fprintf (stderr, "         specified type of measurement: calibration loop or range.\n") ;
            fprintf (stderr, "         The argument to these options specifies what to measure\n") ;
            fprintf (stderr, "         (case is ignored): PHASE for phase and time measurements\n") ;
            fprintf (stderr, "         (integer), TIME for time measurements only, or FLOAT for\n") ;
            fprintf (stderr, "         phase and time measurements (float).  (VME-based CORTEX\n") ;
            fprintf (stderr, "         requests go to the RAU-2 ranging unit.)\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - The \"-range <sequence>\" option is used to collect ranging\n") ;
            fprintf (stderr, "         measurements from a CORTEX-VME RAU-1 ranging unit.  The\n") ;
            fprintf (stderr, "         argument specifies the number of ranging sequences the unit\n") ;
            fprintf (stderr, "         is to perform: 1 for mono-sequence ranging and up to 32 for\n") ;
            fprintf (stderr, "         multi-sequence ranging.\n") ;
            fprintf (stderr, "\n") ;
            fprintf (stderr, "       - The \"-debug\" option allows you to see the raw data being\n") ;
            fprintf (stderr, "         sent back and forth between CORNET and the CORTEX unit.\n") ;
        }

        exit (EINVAL) ;
    }


/*******************************************************************************
    Loop forever, processing input events as they occur.
*******************************************************************************/

    ioxMonitor (dispatcher, -1.0) ;

    exit (errno) ;


}

/*******************************************************************************

Procedure:

    evtInputCB ()

    Read Events from a CORTEX Unit.


Purpose:

    Function evtInputCB() reads and displays any event messages waiting to be
    read from a CORTEX unit.  When a network connection is established with
    the CORTEX unit, the connection is registered with the IOX dispatcher as
    an input source.  Thereafter, whenever input is detected on the connection,
    the IOX dispatcher automatically invokes evtInputCB() to read the messages
    from the CORTEX unit.


    Invocation:

        status = evtInputCB (callback, reason, userData) ;

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
            evtInputCB() directly.

*******************************************************************************/


static  errno_t  evtInputCB (

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
    bool  asFloat, inHex ;
    Link  link = (Link) userData ;
    CortexHeader  header ;
    int  i ;
    int32_t  *body ;




/* While more input is available, read and display the next message. */

    while (insIsReadable (link->stream)) {
					/* Read the next message. */
        if (insRead (link->stream, -1.0, &header, (char **) &body)) {
            LGE "(evtInputCB) Error reading from %s.\ninsRead: ",
                insName (link->stream)) ;
            break ;
        }

        printf ("\n%s  (%ld bytes)  Flow ID: %ld\n",
                insName (link->stream),
                (long) header.length, (long) header.flowID) ;

        insDecode ((char *) body, (xdrproc_t) xdr_int32_t, sizeof (int32_t),
                   header.length, (char *) body) ;

        asFloat = false ;  inHex = false ;

        switch (body[2]) {
        case 0:
            printf ("    Time: %9ld.%03ld    CONFIGURATION CHANGE (%s)\n",
                    (long) body[0], (long) body[1], insPartName (body[3])) ;
            i = 4 ;  asFloat = (body[3] > insPart ("All", 1)) ;	/* NT table? */
            break ;
        case 1:
            printf ("    Time: %9ld.%03ld    TC REQUEST (%s)\n",
                    (long) body[0], (long) body[1], tcRequestOf (body[3])) ;
            i = 4 ;  inHex = true ;
            break ;
        case 2:
            printf ("    Time: %9ld.%03ld    TC ACKNOWLEDGEMENT (%s)\n",
                    (long) body[0], (long) body[1], tcRequestOf (body[3])) ;
            printf ("    Data: %8ld    Time: %9ld.%03ld    Status: %s\n",
                    (long) body[4], (long) body[5], (long) body[6],
                    tcStatusOf (body[7])) ;
            i = 8 ;  inHex = true ;
            break ;
        default:
            printf ("    Time: %9ld.%03ld    UNKNOWN (%ld)\n",
                    (long) body[0], (long) body[1], (long) body[2]) ;
            i = 3 ;  asFloat = !link->isVME ;  inHex = true ;
            break ;
        }

        header.length -= i * sizeof (int32_t) ;
        dumpData (stdout, i, header.length / sizeof (int32_t), &body[i],
                  inHex, asFloat) ;

    }

/* Check to see if the stream's network connection has been broken.  If so,
   close the connection and exit the program. */

    if (!insIsUp (link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(evtInputCB) Broken connection to %s.\ninsIsUp: ",
            insName (link->stream)) ;
        ioxCancel (callback) ;
        linkClose (link) ;
    }

    return (0) ;

}
