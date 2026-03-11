/* $Id: utility.c,v 1.6 2016/03/13 01:19:22 alex Exp alex $ */
/*******************************************************************************

File:

    utility.c

    CORNET Utilities.


Author:    Alex Measday


Purpose:

    A collection of miscellaneous utility functions.


Public Procedures:

    dumpData() - dumps data received from a CORTEX unit.
    inputCB() - is a generic callback that reads data from a CORTEX unit.
    tcRequestOf() - returns the name of a CORTEX TC request code.
    tcStatusOf() - returns the name of a CORTEX TC acknowledgement status code.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  <stdio.h>			/* Standard I/O definitions. */
#include  <stdlib.h>			/* Standard C library definitions. */
#include  <string.h>			/* C Library string functions. */
#include  "cornet.h"			/* CORNET definitions. */

/*******************************************************************************

Procedure:

    dumpData ()

    Dump Data Received from a CORTEX Unit.


Purpose:

    Function dumpData() dumps a buffer of CORTEX data in integer and,
    optionally, floating-point format.


    Invocation:

        status = dumpData (file, firstField, numFields, data, inHex, asFloat) ;

    where:

        <file>		- I
            is the output file to which the data is written.
        <firstField>	- I
            is a number printed with the first field to be dumped and
            incremented afterwards for each subsequent field.  This
            number should usually be chosen so the dumped fields can
            be keyed to the field numbers in the CORTEX ICD.
        <numFields>	- I
            is the number of fields to be dumped, where a field is an integer
            or a floating-point number.
        <data>		- I
            is the address of the data to be dumped; it should at least be
            as long as numFields * sizeof (int).
        <inHex>		- I
            specifies whether (true) or not (false) the data is to be dumped
            as integers in hexadecimal format.
        <asFloat>	- I
            specifies whether (true) or not (false) the data is to be dumped
            as floating-point numbers.  Setting asFloat true results in each
            field being dumped in both integer and floating-point formats.

*******************************************************************************/


void  dumpData (

#    if PROTOTYPES
        FILE  *file,
        int  firstField,
        int  numFields,
        void  *data,
        bool  inHex,
        bool  asFloat)
#    else
        file, firstField, numFields, data, inHex, asFloat)

        FILE  *file ;
        int  firstField ;
        int  numFields ;
        void  *data ;
        bool  inHex ;
        bool  asFloat ;
#    endif

{    /* Local variables. */
    float  *real = (float *) data ;
    int32_t  *integer = (int32_t *) data ;
    int  column, i, line, numColumns, numLines ;




    if (file == NULL)  file = stdout ;

    if (asFloat) {

        numColumns = 2 ;
        numLines = (numFields + numColumns - 1) / numColumns ;

        for (line = 0 ;  line < numLines ;  line++) {
            for (column = 0 ;  column < numColumns ;  column++) {
                i = (column * numLines) + line ;
                if (i >= numFields)  break ;
                fprintf (file,
                         inHex ? "%s%4d: 0x%8.8lX %11g" : "%s%4d:%11ld %11g",
                         (column == 0) ? "" : "          ",
                         firstField + i,
                         (long) integer[i],
                         real[i]) ;
            }
            fprintf (file, "\n") ;
        }

    } else {

        numColumns = 4 ;
        numLines = (numFields + numColumns - 1) / numColumns ;

        for (line = 0 ;  line < numLines ;  line++) {
            for (column = 0 ;  column < numColumns ;  column++) {
                i = (column * numLines) + line ;
                if (i >= numFields)  break ;
                fprintf (file,
                         inHex ? "%s%4d: 0x%8.8lX" : "%s%4d:%11ld",
                         (column == 0) ? "" : "   ",
                         firstField + i,
                         (long) integer[i]) ;
            }
            fprintf (file, "\n") ;
        }

    }

    return ;

}

/*******************************************************************************

Procedure:

    inputCB ()

    Read Input from a CORTEX Unit.


Purpose:

    Function inputCB() reads and displays any messages waiting to be read
    from a CORTEX unit.  When a network connection is established with the
    CORTEX unit, the connection is registered with the IOX dispatcher as an
    input source.  Thereafter, whenever input is detected on the connection,
    the IOX dispatcher automatically invokes inputCB() to read the messages
    from the CORTEX unit.


    Invocation:

        status = inputCB (callback, reason, userData) ;

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
            inputCB() directly.

*******************************************************************************/


errno_t  inputCB (

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
            LGE "(inputCB) Error reading from %s.\ninsRead: ",
                insName (link->stream)) ;
            break ;
        }

        printf ("\n%s  (%ld bytes)  Flow ID: %ld\n", insName (link->stream),
                (long) header.length, (long) header.flowID) ;

        insDecode ((char *) body, (xdrproc_t) xdr_int32_t, sizeof (int32_t),
                   header.length, (char *) body) ;

        dumpData (stdout, 3, header.length / sizeof (int32_t), body,
                  1, !link->isVME) ;

    }

/* Check to see if the stream's network connection has been broken.  If so,
   close the connection and exit the program. */

    if (!insIsUp (link->stream)) {
        SET_ERRNO (EPIPE) ;
        LGE "(inputCB) Broken connection to %s.\ninsIsUp: ",
            insName (link->stream)) ;
        ioxCancel (callback) ;
        linkClose (link) ;
    }

    return (0) ;

}

/*******************************************************************************

Procedure:

    tcRequestOf ()

    Get Name of TC Request Code.


Purpose:

    Function tcRequestOf() returns the textual name corresponding to a TC
    request code.


    Invocation:

        request = tcRequestOf (code) ;

    where:

        <code>		- I
            is the TC request code (see the CORTEX ICD).
        <request>	- O
            returns the name corresponding to the request code.

*******************************************************************************/


const  char  *tcRequestOf (

#    if PROTOTYPES
        int32_t  code)
#    else
        code)

        int32_t  code ;
#    endif

{

    switch (code) {
    case 1:
        return ("Clear Satellite TC (+LV)") ;
    case 2:
        return ("Clear Satellite TC (-LV)") ;
    case 3:
        return ("Wait & Verify") ;
    case 4:
        return ("Execute") ;
    case 5:
        return ("Pause") ;
    case 6:
        return ("Wait for Absolute Time") ;
    case 7:
        return ("TCU Unlock") ;
    case 8:
        return ("NOP") ;
    case 9:
        return ("Stop Idling") ;
    case 10:
        return ("Group") ;
    case 13:
        return ("Encrypted Satellite TC (-LV)") ;
    case 14:
        return ("Encrypted Satellite TC (+LV)") ;
    case 15:
        return ("Set Delay") ;
    case 16:
        return ("Wait for TC Sync Pulse") ;
    case 17:
        return ("Send Pseudo-earth Pulse") ;
    default:
        return ("<unknown>") ;
    }

}

/*******************************************************************************

Procedure:

    tcStatusOf ()

    Get Description of TC Acknowledgement Status.


Purpose:

    Function tcStatusOf() returns the textual description of a TC
    acknowledgement status code.


    Invocation:

        description = tcStatusOf (status) ;

    where:

        <status>	- I
            is the TC acknowledgement status code (see the CORTEX ICD).
        <description>	- O
            returns the description corresponding to the request code.

*******************************************************************************/


const  char  *tcStatusOf (

#    if PROTOTYPES
        int32_t  status)
#    else
        status)

        int32_t  status ;
#    endif

{

    switch (status) {
    case 0:
        return ("Successful") ;
    case 1:
        return ("Ignored (TC encoder locked out)") ;
    case 2:
        return ("Ignored (invalid checksum or syntax") ;
    case 3:
        return ("Failed (check/verification)") ;
    case 4:
        return ("Ignored (CMM1/encrypter/group)") ;
    case 5:
        return ("Ignored (CMM2 check failed)") ;
    case 6:
        return ("Ignored (group rejected)") ;
    case 7:
        return ("Ignored (TCU failure)") ;
    case 8:
        return ("Failed (bad TC demodulation)") ;
    case 9:
        return ("Ignored (timeout)") ;
    default:
        return ("<unknown>") ;
    }

}
