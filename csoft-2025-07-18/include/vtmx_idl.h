/* $Id: vtmx_idl.h,v 1.1 2021/09/13 02:42:42 alex Exp $ */
/*******************************************************************************
    ../idl/EnhancedViewOfTime/CosClockService.idl
*******************************************************************************/
/*  <stdin> : line 1  */
/*  <built-in> : line 1  */
/*  <command-line> : line 1  */
/*  <stdin> : line 1  */

/* Module: CosClockService */

#ifndef IDL_MODULE_CosClockService
#    define  IDL_MODULE_CosClockService  1
#endif

/* Module: CosClockService::ClockProperty */

#ifndef IDL_MODULE_CosClockService__ClockProperty
#    define  IDL_MODULE_CosClockService__ClockProperty  1
#endif

typedef  unsigned  long  Resolution ;

typedef  short  Precision ;

typedef  unsigned  short  Width ;

typedef  char  *Stability_Description ;

typedef  short  Coordination ;

#define  Uncoordinated  (0)
#define  Coordinated  (1)
#define  Faulty  (2)

typedef  struct  Offset {
    LONGLONG  measured ;
    LONGLONG  deliberate ;
}  Offset ;

typedef  short  Measurement ;

#define  Not_Determined  (0)
#define  Discontinuous  (1)
#define  Available  (2)

typedef  float  Hz ;

typedef  struct  Skew {
    Measurement  available ;
    Hz  measured ;
    Hz  deliberate ;
}  Skew ;

typedef  float  HzPerSec ;

typedef  struct  Drift {
    Measurement  available ;
    HzPerSec  measured ;
    HzPerSec  deliberate ;
}  Drift ;

typedef  short  TimeScale ;

#define  TIMESCALE_Unknown  (- 1)
#define  TIMESCALE_TAI  (0)
#define  TIMESCALE_UT0  (1)
#define  TIMESCALE_UT1  (2)
#define  TIMESCALE_UTC  (3)
#define  TIMESCALE_TT  (4)
#define  TIMESCALE_TDB  (5)
#define  TIMESCALE_TCG  (6)
#define  TIMESCALE_TCB  (7)
#define  TIMESCALE_Sidereal  (8)
#define  TIMESCALE__Local  (9)
#define  TIMESCALE_GPS  (10)
#define  TIMESCALE_Other  (0x7fff)

typedef  short  Stratum ;

#define  unspecified  (0)
#define  primary_reference  (1)
#define  secondary_reference_base  (2)

typedef  IOR  CoordinationSource ;

typedef  char  *Comments ;

typedef  IOR  Clock ;

/*===== "TimeComparison" already defined =====*/

/*===== "ComparisonType" already defined =====*/

/*===== "OverlapType" already defined =====*/

typedef  _valuetype_  TimeSpan ;	/* "valuetype" */
typedef  _valuetype_  UTC ;	/* "valuetype" */

typedef  IOR  UtcTimeService ;
typedef  IOR  ClockCatalog ;

typedef  struct  ClockEntry {
    Clock  subject ;
    char  *name ;
}  ClockEntry ;

typedef  SEQUENCE (ClockEntry, ClockEntries) ;

typedef  IOR  SynchronizeBase ;

typedef  struct  SyncReading {
    TimeT  local_send ;
    TimeT  local_receive ;
    TimeT  remote_reading ;
}  SyncReading ;

typedef  IOR  SynchronizedClock ;
typedef  IOR  Synchronizable ;

#define  Forever  (0x7FFFFFFFFFFFFFFF)

/* Module: CosClockService::PeriodicExecution */

#ifndef IDL_MODULE_CosClockService__PeriodicExecution
#    define  IDL_MODULE_CosClockService__PeriodicExecution  1
#endif

typedef  short  ControllerUpdateStrategy ;

#define  UNDEFINED  (- 1)
#define  CANCEL_ALL  (0)
#define  ENFORCE_INTERVAL  (1)
#define  ENFORCE_DEADLINE  (2)
#define  USER_DEFINED_0  (3)
#define  USER_DEFINED_1  (4)
#define  USER_DEFINED_2  (5)

typedef  IOR  Periodic ;

/* Module: CosClockService::Monotonicity */

#ifndef IDL_MODULE_CosClockService__Monotonicity
#    define  IDL_MODULE_CosClockService__Monotonicity  1
#endif

typedef  enum  RecoveryStrategy {
    IGNORE,
    SLOW_DOWN,
    STALL
}  RecoveryStrategy ;

typedef  struct  Violation {
    long  min_interval ;
    long  max_interval ;
    RecoveryStrategy  strategy ;
}  Violation ;

/*******************************************************************************
    Tables for mapping enumerated values to names and vice-versa;
    see the coliToName() and coliToNumber() functions.
*******************************************************************************/

#if !defined(IDL_MODULE_CosClockService__Monotonicity) || IDL_MODULE_CosClockService__Monotonicity
    extern  const  ColiMap  RecoveryStrategyLUT[]  OCD ("Monotoni") ;
#endif

/*******************************************************************************
    Public functions.
*******************************************************************************/

/* Marshaling functions for the defined data types. */

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxClock(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxClockCatalog(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
    extern  errno_t  vtmxClockEntries P_((ComxChannel channel,
                                          ClockEntries *value))
        OCD ("CosClock") ;
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
    extern  errno_t  vtmxClockEntry P_((ComxChannel channel,
                                        ClockEntry *value))
        OCD ("CosClock") ;
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxComments(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__PeriodicExecution) || IDL_MODULE_CosClockService__PeriodicExecution
#    define  vtmxControllerUpdateStrategy(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxCoordination(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxCoordinationSource(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
    extern  errno_t  vtmxDrift P_((ComxChannel channel,
                                   Drift *value))
        OCD ("ClockPro") ;
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxHz(channel,value)  comxFloat ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxHzPerSec(channel,value)  comxFloat ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxMeasurement(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
    extern  errno_t  vtmxOffset P_((ComxChannel channel,
                                    Offset *value))
        OCD ("ClockPro") ;
#endif

#if !defined(IDL_MODULE_CosClockService__PeriodicExecution) || IDL_MODULE_CosClockService__PeriodicExecution
#    define  vtmxPeriodic(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxPrecision(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__Monotonicity) || IDL_MODULE_CosClockService__Monotonicity
    extern  errno_t  vtmxRecoveryStrategy P_((ComxChannel channel,
                                              RecoveryStrategy *value))
        OCD ("Monotoni") ;
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxResolution(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
    extern  errno_t  vtmxSkew P_((ComxChannel channel,
                                  Skew *value))
        OCD ("ClockPro") ;
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxStability_Description(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxStratum(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxSynchronizable(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxSynchronizeBase(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxSynchronizedClock(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
    extern  errno_t  vtmxSyncReading P_((ComxChannel channel,
                                         SyncReading *value))
        OCD ("CosClock") ;
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxTimeScale(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxTimeSpan(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxUTC(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService) || IDL_MODULE_CosClockService
#    define  vtmxUtcTimeService(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosClockService__Monotonicity) || IDL_MODULE_CosClockService__Monotonicity
    extern  errno_t  vtmxViolation P_((ComxChannel channel,
                                       Violation *value))
        OCD ("Monotoni") ;
#endif

#if !defined(IDL_MODULE_CosClockService__ClockProperty) || IDL_MODULE_CosClockService__ClockProperty
#    define  vtmxWidth(channel,value)  comxUShort ((channel), (value))
#endif
