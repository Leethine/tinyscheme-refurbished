/* $Id: damx_idl.h,v 1.9 2021/09/15 04:18:16 alex Exp $ */
/*******************************************************************************
    damx_all.idl
*******************************************************************************/
/*  <stdin> : line 1  */
/*  <built-in> : line 1  */
/*  <command-line> : line 1  */
/*  <stdin> : line 1  */
/*  ../idl/DAIS/DAFEvents.idl : line 1  */
/*  ../idl/DAIS/DAFIdentifiers.idl : line 1  */

/* Module: DAFIdentifiers */

#ifndef IDL_MODULE_DAFIdentifiers
#    define  IDL_MODULE_DAFIdentifiers  1
#endif

typedef  char  *URI ;

typedef  SEQUENCE (URI, URISequence) ;

typedef  struct  ResourceID {
    ULONGLONG  container ;
    ULONGLONG  fragment ;
}  ResourceID ;

typedef  SEQUENCE (ResourceID, ResourceIDSequence) ;

typedef  IOR  ResourceIDService ;
/*  ../idl/CORBAservices/CosEventChannelAdmin.idl : line 1  */
/*  ../idl/CORBAservices/CosEventComm.idl : line 1  */

/* Module: CosEventComm */

#ifndef IDL_MODULE_CosEventComm
#    define  IDL_MODULE_CosEventComm  1
#endif

/* Module: CosEventChannelAdmin */

#ifndef IDL_MODULE_CosEventChannelAdmin
#    define  IDL_MODULE_CosEventChannelAdmin  1
#endif

/* Module: DAFEvents */

#ifndef IDL_MODULE_DAFEvents
#    define  IDL_MODULE_DAFEvents  1
#endif

typedef  struct  ResourceChangeEvent {
    ResourceIDSequence  affected ;
}  ResourceChangeEvent ;

typedef  IOR  ResourceEventSource ;
/*  ../idl/DAIS/DAFQuery.idl : line 1  */
/*  ../idl/DAIS/DAFDescriptions.idl : line 1  */
/*  ../idl/CORBAservices/TimeBase.idl : line 1  */

/* Module: TimeBase */

#ifndef IDL_MODULE_TimeBase
#    define  IDL_MODULE_TimeBase  1
#endif

/*===== "TimeT" already defined =====*/

/*===== "InaccuracyT" already defined =====*/

/*===== "TdfT" already defined =====*/

/*===== "UtcT" already defined =====*/

/*===== "IntervalT" already defined =====*/

/* Module: DAFDescriptions */

#ifndef IDL_MODULE_DAFDescriptions
#    define  IDL_MODULE_DAFDescriptions  1
#endif

typedef  ULONGLONG  DateTime ;

typedef  struct  Complex {
    double  real ;
    double  imaginary ;
}  Complex ;

typedef  short  SimpleValueType ;

#define  RESOURCE_TYPE  (1)
#define  URI_TYPE  (2)
#define  STRING_TYPE  (3)
#define  BOOLEAN_TYPE  (4)
#define  INT_TYPE  (5)
#define  UNSIGNED_TYPE  (6)
#define  DOUBLE_TYPE  (7)
#define  COMPLEX_TYPE  (8)
#define  DATE_TIME_TYPE  (9)
#define  ULONG_LONG_TYPE  (10)

typedef  struct  SimpleValue {
    SimpleValueType  which ;
    union {
			/* RESOURCE_TYPE */
        ResourceID  resource_value ;
			/* URI_TYPE */
        URI  uri_value ;
			/* STRING_TYPE */
        char  *string_value ;
			/* BOOLEAN_TYPE */
        bool  boolean_value ;
			/* INT_TYPE */
        long  int_value ;
			/* UNSIGNED_TYPE */
        unsigned  long  unsigned_value ;
			/* DOUBLE_TYPE */
        double  double_value ;
			/* COMPLEX_TYPE */
        Complex  complex_value ;
			/* DATE_TIME_TYPE */
        DateTime  date_time_value ;
			/* ULONG_LONG_TYPE */
        ULONGLONG  ulong_long_value ;
    }  data ;
}  SimpleValue ;

typedef  ResourceID  PropertyID ;

/*===== "PropertyValue" already defined =====*/

typedef  SEQUENCE (PropertyValue, PropertyValueSequence) ;

typedef  struct  ResourceDescription {
    ResourceID  id ;
    PropertyValueSequence  values ;
}  ResourceDescription ;

typedef  SEQUENCE (ResourceDescription, ResourceDescriptionSequence) ;

typedef  IOR  ResourceDescriptionIterator ;

/* Module: DAFQuery */

#ifndef IDL_MODULE_DAFQuery
#    define  IDL_MODULE_DAFQuery  1
#endif

typedef  ResourceID  ClassID ;

/*===== "PropertyID" already defined =====*/

typedef  ResourceIDSequence  PropertySequence ;

typedef  struct  Association {
    PropertyID  property ;
    ClassID  type ;
    bool  inverse ;
}  Association ;

typedef  SEQUENCE (Association, AssociationSequence) ;

typedef  IOR  ResourceQueryService ;

/*******************************************************************************
    Public functions.
*******************************************************************************/

/* Marshaling functions for the defined data types. */

#if !defined(IDL_MODULE_DAFQuery) || IDL_MODULE_DAFQuery
    extern  errno_t  damxAssociation P_((ComxChannel channel,
                                         Association *value))
        OCD ("DAFQuery") ;
#endif

#if !defined(IDL_MODULE_DAFQuery) || IDL_MODULE_DAFQuery
    extern  errno_t  damxAssociationSequence P_((ComxChannel channel,
                                                 AssociationSequence *value))
        OCD ("DAFQuery") ;
#endif

#if !defined(IDL_MODULE_DAFQuery) || IDL_MODULE_DAFQuery
#    define  damxClassID(channel,value)  damxResourceID ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    extern  errno_t  damxComplex P_((ComxChannel channel,
                                     Complex *value))
        OCD ("DAFDescr") ;
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
#    define  damxDateTime(channel,value)  comxULongLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
#    define  damxPropertyID(channel,value)  damxResourceID ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFQuery) || IDL_MODULE_DAFQuery
    extern  errno_t  damxPropertySequence P_((ComxChannel channel,
                                              PropertySequence *value))
        OCD ("DAFQuery") ;
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    extern  errno_t  damxPropertyValueSequence P_((ComxChannel channel,
                                                   PropertyValueSequence *value))
        OCD ("DAFDescr") ;
#endif

#if !defined(IDL_MODULE_DAFEvents) || IDL_MODULE_DAFEvents
    extern  errno_t  damxResourceChangeEvent P_((ComxChannel channel,
                                                 ResourceChangeEvent *value))
        OCD ("DAFEvent") ;
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    extern  errno_t  damxResourceDescription P_((ComxChannel channel,
                                                 ResourceDescription *value))
        OCD ("DAFDescr") ;
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
#    define  damxResourceDescriptionIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    extern  errno_t  damxResourceDescriptionSequence P_((ComxChannel channel,
                                                         ResourceDescriptionSequence *value))
        OCD ("DAFDescr") ;
#endif

#if !defined(IDL_MODULE_DAFEvents) || IDL_MODULE_DAFEvents
#    define  damxResourceEventSource(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFIdentifiers) || IDL_MODULE_DAFIdentifiers
    extern  errno_t  damxResourceID P_((ComxChannel channel,
                                        ResourceID *value))
        OCD ("DAFIdent") ;
#endif

#if !defined(IDL_MODULE_DAFIdentifiers) || IDL_MODULE_DAFIdentifiers
    extern  errno_t  damxResourceIDSequence P_((ComxChannel channel,
                                                ResourceIDSequence *value))
        OCD ("DAFIdent") ;
#endif

#if !defined(IDL_MODULE_DAFIdentifiers) || IDL_MODULE_DAFIdentifiers
#    define  damxResourceIDService(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFQuery) || IDL_MODULE_DAFQuery
#    define  damxResourceQueryService(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
    extern  errno_t  damxSimpleValue P_((ComxChannel channel,
                                         SimpleValue *value))
        OCD ("DAFDescr") ;
#endif

#if !defined(IDL_MODULE_DAFDescriptions) || IDL_MODULE_DAFDescriptions
#    define  damxSimpleValueType(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFIdentifiers) || IDL_MODULE_DAFIdentifiers
#    define  damxURI(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_DAFIdentifiers) || IDL_MODULE_DAFIdentifiers
    extern  errno_t  damxURISequence P_((ComxChannel channel,
                                         URISequence *value))
        OCD ("DAFIdent") ;
#endif
