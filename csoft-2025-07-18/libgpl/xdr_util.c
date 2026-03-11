/* $Id: xdr_util.c,v 1.3 2021/04/21 02:11:04 alex Exp alex $ */
/*******************************************************************************

File:

    xdr_util.c

    XDR Functions and Utilities.


Author:    Alex Measday


Purpose:

    The XDR_UTIL package provides additional XDR functions and utilities
    that I've needed or thought I needed.

    ENUMs
    -----

    Enumerated types in C are problematic with respect to XDR.  According to
    the C Standard, enumerated values are integer constants.  Therefore, the
    storage allocated for a variable of a given enumerated type only needs
    to be large enough to hold the enumerated values of that type.  For
    example, variables of type

        enum GuitarString { broken = 0,
                            Elow = 1, A = 2, D = 3,
                            G = 5, B = 5, Ehigh = 6 } ;

    only require one byte to hold the values 0-6.  On the other hand,
    variables of type

        enum GaxaxySize { blackhole =        0LL,
                          dwarf =      5F5E100LL,	-- 10^8
                          giant = 5AF3107A4000LL } ;	-- 10^14

    need at least 48 bits to hold the possible values (from Wikipedia
    "Galaxy" entry).

    Sun's original RPC assumed enums were signed, 16-bit shorts or signed,
    32-bit longs to be exchanged with xdr_long() or xdr_short(), respectively.
    The xdr_enum() function begins with a simple type declaration:

                       "enum sizecheck { SIZEVAL };"

    The size of this type is compared to sizeof (short) and sizeof (long)
    and xdr_short() or xdr_long() is called as necessary.  On a compiler
    that optimizes such things, however, "enum sizecheck" only requires
    one byte, a size unlikely to be equal to the size of a short or long
    integer.  The BSD-XDR library I'm using adds a little more nuance to
    the code when an int and long differ in size, but it's still basically
    the same as the original Sun RPC implementation.

    The xdr_enum() function takes a pointer to a variable of type "enum_t",
    which is defined as a 32-bit, signed integer.  By using an intermediate
    "enum_t" variable, this function can be clumsily used to encode/decode
    8-, 16-, and 32-bit enumerated types:

        enum  GuitarString  which = Elow ;
        enum_t  whatever ;
        XDR  xdrStream ;

        ... initialize XDR stream structure ...

        whatever = (enum_t) which ;		-- 8-bit enum to 32-bit value.
        xdrStream.x_op = XDR_ENCODE ;
        xdr_enum (&xdrStream, &whatever) ;	-- Encode before sending.

        ... send and then receive same type back ...

        xdrStream.x_op = XDR_DECODE ;
        xdr_enum (&xdrStream, &whatever) ;	-- Decode after receiving.
        which = (enum GuitarString) whatever ;	-- 32-bit value to 8-bit enum.

    That is a workable solution, but it doesn't handle enumerated types
    wider than 32 bits.  So I wrote some XDR functions for size-specific,
    signed/unsigned, enumerated types:

        xdr_enum8_t(), xdr_enum8u_t(), ...,
            ..., xdr_enum64_t(), xdr_enum64u_t()

    These functions encode/decode their arguments as "intN_t"s or "uintN_t"s.
    Knowing the range of enumerated values of an enum type, an application
    can choose the appropriate function to call.  This is fine if both sides
    of a transmission stream agree on the width of the enum.  If they don't,
    the chosen function can still be called since it covers the range of
    enumerated values, but one or both sides of the stream will have to use
    the intermediate value hack above.

    The 3-argument xdr_enumN_t() and xdr_enumNu_t() functions (where "N" is
    the character "N" and not a stand-in for a bit width) have the same
    problem as the functions above if the two sides don't agree on the
    width of an enum.

        TRIVIA:  I was testing some XDR-based programs on the Palm Pilot
        emulator (POSE) and they would crash almost right away.  I knew the
        GCC cross-compiler used one-byte enums and I thought that was my
        problem.  Therefore, I wrote these XDR enum functions.  It turned
        out that the problem *was* with one-byte enums, but not with XDR.
        (Yet.  The one-byte enum still needed to be handled correctly with
        XDR.)  The GCC cross-compiler aligns function arguments on even
        addresses on the call stack.  The PalmOS SDK's header file for
        variable-length argument lists, "unix_stdarg.h", adds the size of
        the current argument to the stack address to get the location of
        the next argument.  Advancing to the argument following a one-byte
        enum results in an odd address and a crash.  The solution was to
        use GCC's <stdarg.h>, not the SDK's header file.

    TIME_Ts, TIMESPECs, and TIMEVALs
    --------------------------------

    UNIX "struct timeval"s have two fields: (i) number of seconds and
    (ii) number of microseconds.  Early on, both fields were long
    integers, typically 32-bits wide:

        struct  timeval {
            long  tv_sec ;
            long  tv_usec ;
        }

    Now, the standard definition of this structure is as follows:

        struct  timeval {
            time_t  tv_sec ;
            suseconds_t  tv_usec ;
        }

    where "suseconds_t" is a signed integer that is wide enough to
    hold numbers in the range -1..1,000,000.  There is some confusion
    about "time_t": the Open Group Base Specification 2018 calls it
    an integer type whereas the ISO draft C17 standard calls it a
    real type.  (The 2004 Open Group specification said, "time_t and
    clock_t shall be integer or real-floating types".  At some point
    between 2004 and 2018, they changed "time_t" to only being an
    integer type; "clock_t" can still be of either type.)

    Regardless of the confusion, the size of "time_t" can vary on
    different platforms, most likely between the old, but still used,
    convention of a signed, 32-bit integer (which will overflow in
    January 2038) and the more recent convention of a signed, 64-bit
    integer.  To handle both the 32- and 64-bit sizes somewhat
    transparently, my xdr_time_t() function transfers the time
    as a 64-bit, IEEE 754 double-precision, floating-point number.
    (IEEE 754 is also the standard, over-the-wire representation of
    floating-point numbers used by XDR.)

    IEEE double-precision floats have a 53-bit, unsigned mantissa
    (52 explicit bits plus the implied bit for normalized numbers);
    the sign bit is stored separately.  IEEE floats can therefore
    handle over 9 quadrillion consecutive integers:

                   0 .. 2^53 - 1 = 9,007,199,254,740,991

    Dividing by 60, 60, 24, and 365.2422 will show that an IEEE float
    can represent, in integer seconds, times over 285-million years
    in the future and an equal number of years in the past.

    Programs on platforms with 32-bit "time_t"s still face the possibility
    of receiving times that are too large.  Hmmm, maybe I should do some
    more bit twiddling in xdr_time_t() and just use plain, old 64-bit
    integers ...


Public Procedures:

    xdr_enumN_t() - encodes/decodes variable-width, signed enumerations.
    xdr_enumNu_t() - encodes/decodes variable-width, usigned enumerations.
    xdr_enum8_t() - encodes/decodes 8-bit signed enumerations.
    xdr_enum8u_t() - encodes/decodes 8-bit unsigned enumerations.
    xdr_enum16_t() - encodes/decodes 16-bit signed enumerations.
    xdr_enum16u_t() - encodes/decodes 16-bit unsigned enumerations.
    xdr_enum32_t() - encodes/decodes 32-bit signed enumerations.
    xdr_enum32u_t() - encodes/decodes 32-bit unsigned enumerations.
    xdr_enum64_t() - encodes/decodes 64-bit signed enumerations.
    xdr_enum64u_t() - encodes/decodes 64-bit unsigned enumerations.
    xdr_time_t() - encodes/decodes a time_t time in seconds.
    xdr_timespec() - encodes/decodes a timespec structure.
    xdr_timeval() - encodes/decodes a timeval structure.
    xdr_timeval32() - encodes/decodes a timeval structure with 32-bit fields.

*******************************************************************************/


#include  "pragmatics.h"		/* Compiler, OS, logging definitions. */

#include  "xdr_util.h"			/* XDR functions and utilities. */

/*!*****************************************************************************

Procedures:

    xdr_enumN_t ()

    Encode/Decode Variable-Width, Signed Enumerations.


Purpose:

    Function xdr_enumN_t() is similar to the xdr_enum8_t(), etc. functions
    except that the caller must specify the size in bytes, obtained with
    sizeof(), of the enumerated type.  1-, 2-, 4-, and 8-byte enumerations
    are the only sizes handled.

        NOTE:  Like xdr_string(), xdr_enumN_t() has 3 arguments and thus
        cannot be passed to RPC functions expecting the normal 2-argument
        XDR functions.  Unfortunately, I can't think of an xdr_wrapstring()
        counterpart for xdr_enumN_t().


    Invocation:

        status = xdr_enumN_t (&xdrStream, &value, size) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <value>		- I/O
            is a (VOID *) pointer to an enum-type variable.  If the XDR
            operation is XDR_ENCODE, the variable's value, treated as an
            integer of the specified size, is converted to its XDR
            representation and added to the XDR stream (standard I/O,
            memory, or record TCP/IP).  If the XDR operation is XDR_DECODE,
            an integer of the specified size is removed from the stream,
            converted from its XDR representation to its host representation,
            and stored in the variable addressed by the VALUE pointer.
        <size>		- I
            is the size in bytes of the enum-type value, which can be
            obtained with the Standard C sizeof() function.  Allowed
            sizes are 1, 2, 4, and 8 bytes.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise.

*******************************************************************************/


bool_t  xdr_enumN_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        void  *value,
        size_t  size)
#    else
        xdrStream, value, size)

        XDR  *xdrStream ;
        void  *value ;
        size_t  size ;
#    endif

{    /* Local variables. */
    int32_t  value32bits ;



#if !defined(HAVE_64_BIT_INTEGERS) || HAVE_64_BIT_INTEGERS
    if (size == 8)  return (xdr_int64_t (xdrStream, (int64_t *) value)) ;
#endif

    if (xdrStream->x_op == XDR_ENCODE) {

        switch (size) {
        case 1:
            value32bits = (int32_t) *((int8_t *) value) ;
            break ;
        case 2:
            value32bits = (int32_t) *((int16_t *) value) ;
            break ;
        case 4:
            value32bits = *((int32_t *) value) ;
            break ;
        default:
            SET_ERRNO (EINVAL) ;
            LGE "(xdr_enumN_t) Invalid enum size: %lu bytes\n",
                (unsigned long) size) ;
            return (FALSE) ;
        }

        return (xdr_int32_t (xdrStream, &value32bits)) ;

    } else if (xdrStream->x_op == XDR_ENCODE) {

        if (!xdr_int32_t (xdrStream, &value32bits))  return (FALSE) ;

        switch (size) {
        case 1:
            *((int8_t *) value) = (int8_t) value32bits ;
            return (TRUE) ;
        case 2:
            *((int16_t *) value) = (int16_t) value32bits ;
            return (TRUE) ;
        case 4:
            *((int32_t *) value) = value32bits ;
            return (TRUE) ;
        default:
            SET_ERRNO (EINVAL) ;
            LGE "(xdr_enumN_t) Invalid enum size: %lu bytes\n",
                (unsigned long) size) ;
            return (FALSE) ;
        }

    } else {

        return (TRUE) ;			/* x_op == XDR_FREE */

    }

}

/*!*****************************************************************************

Procedures:

    xdr_enumNu_t ()

    Encode/Decode Variable-Width, Unsigned Enumerations.


Purpose:

    Function xdr_enumNu_t() is similar to the xdr_enum8u_t(), etc. functions
    except that the caller must specify the size in bytes, obtained with
    sizeof(), of the enumerated type.  1-, 2-, 4-, and 8-byte enumerations
    are the only sizes handled.

        NOTE:  Like xdr_string(), xdr_enumNu_t() has 3 arguments and thus
        cannot be passed to RPC functions expecting the normal 2-argument
        XDR functions.  Unfortunately, I can't think of an xdr_wrapstring()
        counterpart for xdr_enumNu_t().


    Invocation:

        status = xdr_enumNu_t (&xdrStream, &value, size) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <value>		- I/O
            is a (VOID *) pointer to an enum-type variable.  If the XDR
            operation is XDR_ENCODE, the variable's value, treated as an
            integer of the specified size, is converted to its XDR
            representation and added to the XDR stream (standard I/O,
            memory, or record TCP/IP).  If the XDR operation is XDR_DECODE,
            an integer of the specified size is removed from the stream,
            converted from its XDR representation to its host representation,
            and stored in the variable addressed by the VALUE pointer.
        <size>		- I
            is the size in bytes of the enum-type value, which can be
            obtained with the Standard C sizeof() function.  Allowed
            sizes are 1, 2, 4, and 8 bytes.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise.

*******************************************************************************/


bool_t  xdr_enumNu_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        void  *value,
        size_t  size)
#    else
        xdrStream, value, size)

        XDR  *xdrStream ;
        void  *value ;
        size_t  size ;
#    endif

{    /* Local variables. */
    uint32_t  value32bits ;



#if !defined(HAVE_64_BIT_INTEGERS) || HAVE_64_BIT_INTEGERS
    if (size == 8)  return (xdr_uint64_t (xdrStream, (uint64_t *) value)) ;
#endif

    if (xdrStream->x_op == XDR_ENCODE) {

        switch (size) {
        case 1:
            value32bits = (uint32_t) *((uint8_t *) value) ;
            break ;
        case 2:
            value32bits = (uint32_t) *((uint16_t *) value) ;
            break ;
        case 4:
            value32bits = *((uint32_t *) value) ;
            break ;
        default:
            SET_ERRNO (EINVAL) ;
            LGE "(xdr_enumNu_t) Invalid enum size: %lu bytes\n",
                (unsigned long) size) ;
            return (FALSE) ;
        }

        return (xdr_uint32_t (xdrStream, &value32bits)) ;

    } else if (xdrStream->x_op == XDR_DECODE) {

        if (!xdr_uint32_t (xdrStream, &value32bits))  return (FALSE) ;

        switch (size) {
        case 1:
            *((uint8_t *) value) = (uint8_t) value32bits ;
            return (TRUE) ;
        case 2:
            *((uint16_t *) value) = (uint16_t) value32bits ;
            return (TRUE) ;
        case 4:
            *((uint32_t *) value) = value32bits ;
            return (TRUE) ;
        default:
            SET_ERRNO (EINVAL) ;
            LGE "(xdr_enumNu_t) Invalid enum size: %lu bytes\n",
                (unsigned long) size) ;
            return (FALSE) ;
        }

    } else {

        return (TRUE) ;			/* x_op == XDR_FREE */

    }

}

/*!*****************************************************************************

Procedures:

    xdr_enum8_t ()
    xdr_enum8u_t ()

    xdr_enum16_t ()
    xdr_enum16u_t ()

    xdr_enum32_t ()
    xdr_enum32u_t ()

    xdr_enum64_t ()
    xdr_enum64u_t ()

    Encode/Decode 8-, 16-, 32-, or 64-Bit Enumerations.


Purpose:

    The standard XDR function for enumerated types, xdr_enum(), is
    terribly broken.  Originally, I guess, Sun assumed all enums
    would be 32-bits wide, which was a reasonably (but not completely)
    valid assumption back in the 1980s.  However, the C Standard says
    only that the representation of a particular enumerated type be
    an integer wide enough to hold the values in the enumerated type.
    And, with that, compilers began implementing variable-width
    enumerated types (e.g., "short enums" as GCC calls them).

    For example,

        enum  color  { red, green, blue } ;

    has possible values of 0, 1, and 2.  Consequently, the compiler only
    has to allocate one 8-bit integer to hold these enumerated values.
    Things start to get complex when you specify values for the enumerations:

        enum  cosmos  { paleBlueDot = 1,
                        billionsAndBillions =
                            INT64_C(1000000000) * INT64_C(1000000000) } ;

    The above type needs a 64-bit integer to hold the two possible
    enumerations.  (I haven't compiled the code above, but I hope the
    general idea is apparent.  Incidentally, one short-scale billion
    times one billion is "0x 0DE0 B6B3 A764 0000", requring at least
    61 bits for its two's-complement representation.)

        ... problems in xdr_enum() ...

    The following 4 functions convert enumerated values of different
    bit widths to and from their XDR representation.  The enumerated
    values are passed in and returned as the corresponding integer
    types of the same width:

        XDR function        enum width        value type
        ------------        ----------        ----------
        xdr_enum8_t()          8 bits           int8_t
        xdr_enum16_t()        16 bits           int16_t
        xdr_enum32_t()        32 bits           int32_t
        xdr_enum64_t()        64 bits           int64_t

    The caller is responsible for providing a pointer to an integer of the
    specified type and for converting the enumerated value to and from the
    integer.  In the following example, a 16-bit integer is required to hold
    the possible values of the enumerated type, GameState--I'm looking at you,
    greenLight!

			-- The enumerated type and the variables.
        enum GameState { redLight = 0, greenLight = 31416 } ;
        enum GameState  currentState = redLight ;
        int16_t  currentStateI ;
        XDR  xdrStream ;

			-- Initialize the XDR structure using
			-- one of the _create() functions.
        xdrmem_create (&xdrStream, memory, memoryLength, XDR_ENCODE) ;

			-- Encode the enumerated value into the
			-- memory buffer.
        xdrStream.x_op = XDR_ENCODE ;
        currentStateI = (int16_t) currentState ;
        xdr_enum16_t (&xdrStream, &currentStateI) ;
        ... send the memory buffer to the receiver ...

			-- Decode the enumerated value from the
			-- memory buffer.
        ... receive the memory buffer from the sender ...
        xdrStream.x_op = XDR_DECODE ;
        xdr_enum16_t (&xdrStream, &currentStateI) ;
        currentState = (enum GameState) currentStateI ;


    Invocation:

        status = xdr_enum8_t (&xdrStream, &value) ;
        status = xdr_enum8u_t (&xdrStream, &value) ;

        status = xdr_enum16_t (&xdrStream, &value) ;
        status = xdr_enum16u_t (&xdrStream, &value) ;

        status = xdr_enum32_t (&xdrStream, &value) ;
        status = xdr_enum32u_t (&xdrStream, &value) ;

        status = xdr_enum64_t (&xdrStream, &value) ;
        status = xdr_enum64u_t (&xdrStream, &value) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <value>		- I/O
            is a pointer to an integer of the width specified in the
            function name.  If the XDR operation is XDR_ENCODE, the
            integer pointed to by the VALUE argument is converted to its
            XDR representation and added to the XDR stream (standard I/O,
            memory, or record TCP/IP).  If the XDR operation is XDR_DECODE,
            an integer of the specified width is removed from the stream,
            converted from its XDR representation to its host representation,
            and stored in the integer addressed by the VALUE pointer.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise.

*******************************************************************************/

/*******************************************************************************
    Encode/decode 8-bit enumerated values.
*******************************************************************************/

bool_t  xdr_enum8_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        int8_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        int8_t  *value ;
#    endif

{
    return (xdr_enumN_t (xdrStream, value, sizeof (int8_t))) ;
}


bool_t  xdr_enum8u_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        uint8_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        uint8_t  *value ;
#    endif

{
    return (xdr_enumNu_t (xdrStream, value, sizeof (uint8_t))) ;
}

/*******************************************************************************
    Encode/decode 16-bit enumerated values.
*******************************************************************************/

bool_t  xdr_enum16_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        int16_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        int16_t  *value ;
#    endif

{
    return (xdr_enumN_t (xdrStream, value, sizeof (int16_t))) ;
}


bool_t  xdr_enum16u_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        uint16_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        uint16_t  *value ;
#    endif

{
    return (xdr_enumNu_t (xdrStream, value, sizeof (uint16_t))) ;
}

/*******************************************************************************
    Encode/decode 32-bit enumerated values.
*******************************************************************************/

bool_t  xdr_enum32_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        int32_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        int32_t  *value ;
#    endif

{
    return (xdr_enumN_t (xdrStream, value, sizeof (int32_t))) ;
}


bool_t  xdr_enum32u_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        uint32_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        uint32_t  *value ;
#    endif

{
    return (xdr_enumNu_t (xdrStream, value, sizeof (uint32_t))) ;
}

/*******************************************************************************
    Encode/decode 64-bit enumerated values (if the host system supports
    64-bit integers).
*******************************************************************************/

#if !defined(HAVE_64_BIT_INTEGERS) || HAVE_64_BIT_INTEGERS

bool_t  xdr_enum64_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        int64_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        int64_t  *value ;
#    endif

{
    return (xdr_enumN_t (xdrStream, value, sizeof (int64_t))) ;
}


bool_t  xdr_enum64u_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        uint64_t  *value)
#    else
        xdrStream, value)

        XDR  *xdrStream ;
        uint64_t  *value ;
#    endif

{
    return (xdr_enumNu_t (xdrStream, value, sizeof (uint64_t))) ;
}

#endif	/* HAVE_64_BIT_INTEGERS */

/*!*****************************************************************************

Procedures:

    xdr_time_t ()

    Encode/Decode a time_t Time in Seconds.


Purpose:

    Function xdr_time_t() is an XDR(3)-compatible function that
    encodes/decodes a UNIX time_t value to/from XDR format.


    Invocation:

        status = xdr_time_t (&xdrStream, &seconds) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <seconds>	- I/O
            is a pointer to a time_t variable.  If the XDR operation is
            XDR_ENCODE, the variable's value is converted to its XDR
            representation and added to the XDR stream (standard I/O,
            memory, or record TCP/IP).  If the XDR operation is XDR_DECODE,
            a time_t value is removed from the stream, converted from its
            XDR representation to its host representation, and stored in
            the variable.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise.

*******************************************************************************/


bool_t  xdr_time_t (

#    if PROTOTYPES
        XDR  *xdrStream,
        time_t  *seconds)
#    else
        xdrStream, seconds)

        XDR  *xdrStream ;
        time_t  *seconds ;
#    endif

{    /* Local variables. */
    double  time ;



    if (seconds == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(xdr_time_t) NULL seconds pointer.\n") ;
        return (FALSE) ;
    }

    time = (double) *seconds ;
    if (!xdr_double (xdrStream, &time))  return (FALSE) ;
    *seconds = (time_t) time ;

    return (TRUE) ;		/* XDR_DECODE or XDR_FREE */

}

/*!*****************************************************************************

Procedure:

    xdr_timeval ()

    Encode/Decode a UNIX "timeval" Structure.


Purpose:

    Function xdr_timeval() is an XDR(3)-compatible function that
    encodes/decodes a UNIX "timeval" structure into/from XDR format.


    Invocation:

        status = xdr_timeval (xdrStream, &time) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <time>		- I/O
            is a pointer to a UNIX "struct timeval" variable.  If the XDR
            operation is XDR_ENCODE, the variable's value is converted to
            its XDR representation and added to the XDR stream (standard
            I/O, memory, or record TCP/IP).  If the XDR operation is
            XDR_DECODE, a "timespec" value is removed from the stream,
            converted from its XDR representation to its host representation,
            and stored in the variable.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise.

*******************************************************************************/


bool_t  xdr_timespec (

#    if PROTOTYPES
        XDR  *xdrStream,
        struct  timespec  *time)
#    else
        xdrStream, time)

        XDR  *xdrStream ;
        struct  timespec  *time ;
#    endif

{    /* Local variables. */
    int32_t  nanoseconds ;
    time_t  seconds ;



    if (time == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(xdr_timespec) NULL timespec pointer.\n") ;
        return (FALSE) ;
    }

/* Encode/decode the first element of the "timeval" structure,
   a time_t count of seconds. */

    seconds = time->tv_sec ;
    if (!xdr_time_t (xdrStream, &seconds))  return (FALSE) ;
    time->tv_sec = seconds ;

/* Now encode/decode the nanoseconds field in the "timespec" structure. */

    nanoseconds = time->tv_nsec ;
    if (!xdr_int32_t (xdrStream, &nanoseconds))  return (FALSE) ;
    time->tv_nsec = nanoseconds ;

    return (TRUE) ;		/* XDR_ENCODE, XDR_DECODE and XDR_FREE */

}

/*!*****************************************************************************

Procedure:

    xdr_timeval ()

    Encode/Decode a UNIX "timeval" Structure.


Purpose:

    Function xdr_timeval() is an XDR(3)-compatible function that
    encodes/decodes a UNIX "timeval" structure to/from XDR format.


    Invocation:

        status = xdr_timeval (xdrStream, &time) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <time>		- I/O
            is a pointer to a "struct timeval" variable.  If the XDR
            operation is XDR_ENCODE, the variable's value is converted
            to its XDR representation and added to the XDR stream
            (standard I/O, memory, or record TCP/IP).  If the XDR
            operation is XDR_DECODE, a timeval value is removed from
            the stream, converted from its XDR representation to its
            host representation, and stored in the variable.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise.

*******************************************************************************/


bool_t  xdr_timeval (

#    if PROTOTYPES
        XDR  *xdrStream,
        struct  timeval  *time)
#    else
        xdrStream, time)

        XDR  *xdrStream ;
        struct  timeval  *time ;
#    endif

{    /* Local variables. */
    int32_t  microseconds ;
    time_t  seconds ;



    if (time == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(xdr_timeval) NULL timeval pointer.\n") ;
        return (FALSE) ;
    }

/* Encode/decode the first element of the "timeval" structure,
   a time_t count of seconds. */

    seconds = (time_t) time->tv_sec ;
    if (!xdr_time_t (xdrStream, &seconds))  return (FALSE) ;
    time->tv_sec = seconds ;

/* Now, encode/decode the microseconds field in the "timeval" structure. */

    microseconds = (int32_t) time->tv_usec ;
    if (!xdr_int32_t (xdrStream, &microseconds))  return (FALSE) ;
    time->tv_usec = microseconds ;

    return (TRUE) ;		/* XDR_ENCODE, XDR_DECODE and XDR_FREE */

}

/*!*****************************************************************************

Procedure:

    xdr_timeval32 ()

    Encode/Decode a UNIX "timeval" Structure with 32-bit fields.


Purpose:

    Function xdr_timeval32() is an XDR(3)-compatible function that
    encodes/decodes a UNIX "timeval" structure to/from XDR format,
    with the seconds and microseconds fields transferred as signed
    32-bit integers.  I wrote this function for compatibility with
    an old protocol that handled a "timeval" structure as two
    xdr_long()s.

    NOTE that the actual definition of "timeval" on the host computer
    need not have 32-bit fields--xdr_timeval32() converts the fields
    to/from 32-bit integers.  Consequently, there may be a possibility
    of overflow or underflow in the seconds field.  When encoding the
    time into an XDR stream, xdr_timeval32() checks if the seconds
    field will fit in a 32-bit integer and fails if it won't.


    Invocation:

        status = xdr_timeval32 (xdrStream, &time) ;

    where

        <xdrStream>	- I
            is a pointer to the XDR stream structure.  The "x_op" field
            must be set to the appropriate value: XDR_ENCODE, XDR_DECODE,
            or XDR_FREE (which has no effect).
        <time>		- I/O
            is a pointer to a "struct timeval" variable.  If the XDR
            operation is XDR_ENCODE, the variable's fields are encoded
            with two xdr_int32_t() calls.  If the XDR operation is
            XDR_DECODE, two xdr_int32_t() calls remove the values from
            the stream and assign them to the two fields in the variable.
        <status>	- O
            returns the status of encoding/decoding the value: TRUE (1) if
            the operation was successful and FALSE (0) otherwise, the latter
            including the case of the "struct timeval" seconds field being
            outside the range of a signed 32-bit integer.

*******************************************************************************/


bool_t  xdr_timeval32 (

#    if PROTOTYPES
        XDR  *xdrStream,
        struct  timeval  *time)
#    else
        xdrStream, time)

        XDR  *xdrStream ;
        struct  timeval  *time ;
#    endif

{    /* Local variables. */
    int32_t  microseconds, seconds ;



    if (time == NULL) {
        SET_ERRNO (EINVAL) ;
        LGE "(xdr_timeval32) NULL timeval pointer.\n") ;
        return (FALSE) ;
    }

/* Report an error if the seconds value in the "timeval" structure
   will not fit into a signed 32-bit integer. */

    if ((xdrStream->x_op == XDR_ENCODE) &&
        ((time->tv_sec < INT32_MIN) || (INT32_MAX < time->tv_sec))) {
        SET_ERRNO (EOVERFLOW) ;
        LGE "(xdr_timeval32) # of seconds outside 32-bit range.\n") ;
        return (FALSE) ;
    }

/* Encode/decode the seconds field in the "timeval" structure. */

    seconds = (int32_t) time->tv_sec ;
    if (!xdr_int32_t (xdrStream, &seconds))  return (FALSE) ;
    time->tv_sec = seconds ;

/* Now, encode/decode the microseconds field in the "timeval" structure. */

    microseconds = (int32_t) time->tv_usec ;
    if (!xdr_int32_t (xdrStream, &microseconds))  return (FALSE) ;
    time->tv_usec = microseconds ;

    return (TRUE) ;		/* XDR_ENCODE, XDR_DECODE and XDR_FREE */

}
