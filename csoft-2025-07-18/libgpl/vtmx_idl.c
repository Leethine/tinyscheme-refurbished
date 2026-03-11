/* $Id$ */
/*******************************************************************************
    Convenience Macros (from comx_util.c).
*******************************************************************************/

/* CHECK(status) - return ERRNO on non-zero status. */

#ifndef CHECK
#    define  CHECK(status)			\
        if ((status))  return (errno) ;
#endif

/* NULL_OR(pointer,field) - pass NULL if structure pointer is NULL; else
   pass address of field in structure. */

#ifndef NULL_OR
#    define  NULL_OR(pointer, field)		\
        (((pointer) == NULL) ? NULL : &(pointer)->field)
#endif

/* RETURN_IF_NULL(pointer) - return EINVAL if pointer is NULL. */

#ifndef RETURN_IF_NULL
#    define  RETURN_IF_NULL(pointer)		\
        if ((pointer) == NULL) {		\
            SET_ERRNO (EINVAL) ;		\
            return (errno) ;			\
        }
#endif

/*!*****************************************************************************
    vtmxOffset() - decode/encode/erase a CORBA Offset structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty

errno_t  vtmxOffset (

#    if PROTOTYPES
        ComxChannel  channel,
        Offset  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Offset  *value ;
#    endif

{
    CHECK (comxLongLong (channel, NULL_OR (value, measured))) ;
    CHECK (comxLongLong (channel, NULL_OR (value, deliberate))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService__ClockProperty */

/*!*****************************************************************************
    vtmxSkew() - decode/encode/erase a CORBA Skew structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty

errno_t  vtmxSkew (

#    if PROTOTYPES
        ComxChannel  channel,
        Skew  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Skew  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, available))) ;
    CHECK (comxFloat (channel, NULL_OR (value, measured))) ;
    CHECK (comxFloat (channel, NULL_OR (value, deliberate))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService__ClockProperty */

/*!*****************************************************************************
    vtmxDrift() - decode/encode/erase a CORBA Drift structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty

errno_t  vtmxDrift (

#    if PROTOTYPES
        ComxChannel  channel,
        Drift  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Drift  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, available))) ;
    CHECK (comxFloat (channel, NULL_OR (value, measured))) ;
    CHECK (comxFloat (channel, NULL_OR (value, deliberate))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService__ClockProperty */

/*!*****************************************************************************
    vtmxClockEntry() - decode/encode/erase a CORBA ClockEntry structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService

errno_t  vtmxClockEntry (

#    if PROTOTYPES
        ComxChannel  channel,
        ClockEntry  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ClockEntry  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, subject))) ;
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService */

/*!*****************************************************************************
    vtmxClockEntries() - decode/encode/erase a sequence of CORBA ClockEntry structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService

errno_t  vtmxClockEntries (

#    if PROTOTYPES
        ComxChannel  channel,
        ClockEntries  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ClockEntries  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) vtmxClockEntry, sizeof (ClockEntry))) ;
}

#endif	/* IDL_MODULE_CosClockService */

/*!*****************************************************************************
    vtmxSyncReading() - decode/encode/erase a CORBA SyncReading structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService

errno_t  vtmxSyncReading (

#    if PROTOTYPES
        ComxChannel  channel,
        SyncReading  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SyncReading  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, local_send))) ;
    CHECK (comxULongLong (channel, NULL_OR (value, local_receive))) ;
    CHECK (comxULongLong (channel, NULL_OR (value, remote_reading))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService */

/*!*****************************************************************************
    vtmxRecoveryStrategy() - decode/encode/erase a CORBA RecoveryStrategy enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__Monotonicity) || IDL_MODULE_CosClockService__Monotonicity

errno_t  vtmxRecoveryStrategy (

#    if PROTOTYPES
        ComxChannel  channel,
        RecoveryStrategy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RecoveryStrategy  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (RecoveryStrategy) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService__Monotonicity */

/*!*****************************************************************************
    vtmxViolation() - decode/encode/erase a CORBA Violation structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__Monotonicity) || IDL_MODULE_CosClockService__Monotonicity

errno_t  vtmxViolation (

#    if PROTOTYPES
        ComxChannel  channel,
        Violation  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Violation  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, min_interval))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_interval))) ;
    CHECK (vtmxRecoveryStrategy (channel, NULL_OR (value, strategy))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosClockService__Monotonicity */

/*******************************************************************************
    Lookup Tables - for converting named constants to numbers and vice-versa;
        see the coliToName() and coliToNumber() functions.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__Monotonicity) || IDL_MODULE_CosClockService__Monotonicity
    const  ColiMap  RecoveryStrategyLUT[] = {
      { (long) IGNORE, "IGNORE" },
      { (long) SLOW_DOWN, "SLOW_DOWN" },
      { (long) STALL, "STALL" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosClockService__Monotonicity */
