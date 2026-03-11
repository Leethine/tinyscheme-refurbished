/* $Id: ddmx_idl.c,v 1.5 2021/09/20 02:06:28 alex Exp $ */
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
    ddmxBuiltinTopicKey_t() - decode/encode/erase a CORBA BuiltinTopicKey_t structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxBuiltinTopicKey_t (

#    if PROTOTYPES
        ComxChannel  channel,
        BuiltinTopicKey_t  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        BuiltinTopicKey_t  *value ;
#    endif

{
    CHECK (comxArray (channel, NULL_OR (value, value), (ComxFunc) comxLong, sizeof (long), 3)) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxInstanceHandleSeq() - decode/encode/erase a sequence of CORBA InstanceHandle_t structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxInstanceHandleSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        InstanceHandleSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InstanceHandleSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxLong, sizeof (long))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDuration_t() - decode/encode/erase a CORBA Duration_t structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDuration_t (

#    if PROTOTYPES
        ComxChannel  channel,
        Duration_t  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Duration_t  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, sec))) ;
    CHECK (comxULong (channel, NULL_OR (value, nanosec))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxTime_t() - decode/encode/erase a CORBA Time_t structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxTime_t (

#    if PROTOTYPES
        ComxChannel  channel,
        Time_t  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Time_t  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, sec))) ;
    CHECK (comxULong (channel, NULL_OR (value, nanosec))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxInconsistentTopicStatus() - decode/encode/erase a CORBA InconsistentTopicStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxInconsistentTopicStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        InconsistentTopicStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InconsistentTopicStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSampleLostStatus() - decode/encode/erase a CORBA SampleLostStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSampleLostStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        SampleLostStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SampleLostStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSampleRejectedStatusKind() - decode/encode/erase a CORBA SampleRejectedStatusKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSampleRejectedStatusKind (

#    if PROTOTYPES
        ComxChannel  channel,
        SampleRejectedStatusKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SampleRejectedStatusKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (SampleRejectedStatusKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSampleRejectedStatus() - decode/encode/erase a CORBA SampleRejectedStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSampleRejectedStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        SampleRejectedStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SampleRejectedStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (ddmxSampleRejectedStatusKind (channel, NULL_OR (value, last_reason))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_instance_handle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxLivelinessLostStatus() - decode/encode/erase a CORBA LivelinessLostStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxLivelinessLostStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        LivelinessLostStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LivelinessLostStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxLivelinessChangedStatus() - decode/encode/erase a CORBA LivelinessChangedStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxLivelinessChangedStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        LivelinessChangedStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LivelinessChangedStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, alive_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, not_alive_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, alive_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, not_alive_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_publication_handle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxOfferedDeadlineMissedStatus() - decode/encode/erase a CORBA OfferedDeadlineMissedStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxOfferedDeadlineMissedStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        OfferedDeadlineMissedStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OfferedDeadlineMissedStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_instance_handle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxRequestedDeadlineMissedStatus() - decode/encode/erase a CORBA RequestedDeadlineMissedStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxRequestedDeadlineMissedStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        RequestedDeadlineMissedStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequestedDeadlineMissedStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_instance_handle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxQosPolicyCount() - decode/encode/erase a CORBA QosPolicyCount structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxQosPolicyCount (

#    if PROTOTYPES
        ComxChannel  channel,
        QosPolicyCount  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        QosPolicyCount  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, policy_id))) ;
    CHECK (comxLong (channel, NULL_OR (value, count))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxQosPolicyCountSeq() - decode/encode/erase a sequence of CORBA QosPolicyCount structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxQosPolicyCountSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        QosPolicyCountSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        QosPolicyCountSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) ddmxQosPolicyCount, sizeof (QosPolicyCount))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxOfferedIncompatibleQosStatus() - decode/encode/erase a CORBA OfferedIncompatibleQosStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxOfferedIncompatibleQosStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        OfferedIncompatibleQosStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OfferedIncompatibleQosStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_policy_id))) ;
    CHECK (ddmxQosPolicyCountSeq (channel, NULL_OR (value, policies))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxRequestedIncompatibleQosStatus() - decode/encode/erase a CORBA RequestedIncompatibleQosStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxRequestedIncompatibleQosStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        RequestedIncompatibleQosStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequestedIncompatibleQosStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_policy_id))) ;
    CHECK (ddmxQosPolicyCountSeq (channel, NULL_OR (value, policies))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxPublicationMatchedStatus() - decode/encode/erase a CORBA PublicationMatchedStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxPublicationMatchedStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        PublicationMatchedStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PublicationMatchedStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, current_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, current_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_subscription_handle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSubscriptionMatchedStatus() - decode/encode/erase a CORBA SubscriptionMatchedStatus structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSubscriptionMatchedStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        SubscriptionMatchedStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SubscriptionMatchedStatus  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, total_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, total_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, current_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, current_count_change))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_publication_handle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDataReaderSeq() - decode/encode/erase a sequence of CORBA DataReader structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDataReaderSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        DataReaderSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DataReaderSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxConditionSeq() - decode/encode/erase a sequence of CORBA Condition structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxConditionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ConditionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ConditionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxUserDataQosPolicy() - decode/encode/erase a CORBA UserDataQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxUserDataQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        UserDataQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UserDataQosPolicy  *value ;
#    endif

{
    CHECK (comxOctetSeq (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxTopicDataQosPolicy() - decode/encode/erase a CORBA TopicDataQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxTopicDataQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        TopicDataQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TopicDataQosPolicy  *value ;
#    endif

{
    CHECK (comxOctetSeq (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxGroupDataQosPolicy() - decode/encode/erase a CORBA GroupDataQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxGroupDataQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        GroupDataQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        GroupDataQosPolicy  *value ;
#    endif

{
    CHECK (comxOctetSeq (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxTransportPriorityQosPolicy() - decode/encode/erase a CORBA TransportPriorityQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxTransportPriorityQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        TransportPriorityQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TransportPriorityQosPolicy  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxLifespanQosPolicy() - decode/encode/erase a CORBA LifespanQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxLifespanQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        LifespanQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LifespanQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDuration_t (channel, NULL_OR (value, duration))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDurabilityQosPolicyKind() - decode/encode/erase a CORBA DurabilityQosPolicyKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDurabilityQosPolicyKind (

#    if PROTOTYPES
        ComxChannel  channel,
        DurabilityQosPolicyKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DurabilityQosPolicyKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (DurabilityQosPolicyKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDurabilityQosPolicy() - decode/encode/erase a CORBA DurabilityQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDurabilityQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        DurabilityQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DurabilityQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDurabilityQosPolicyKind (channel, NULL_OR (value, kind))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxPresentationQosPolicyAccessScopeKind() - decode/encode/erase a CORBA PresentationQosPolicyAccessScopeKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxPresentationQosPolicyAccessScopeKind (

#    if PROTOTYPES
        ComxChannel  channel,
        PresentationQosPolicyAccessScopeKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PresentationQosPolicyAccessScopeKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (PresentationQosPolicyAccessScopeKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxPresentationQosPolicy() - decode/encode/erase a CORBA PresentationQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxPresentationQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        PresentationQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PresentationQosPolicy  *value ;
#    endif

{
    CHECK (ddmxPresentationQosPolicyAccessScopeKind (channel, NULL_OR (value, access_scope))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, coherent_access))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, ordered_access))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDeadlineQosPolicy() - decode/encode/erase a CORBA DeadlineQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDeadlineQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        DeadlineQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DeadlineQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDuration_t (channel, NULL_OR (value, period))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxLatencyBudgetQosPolicy() - decode/encode/erase a CORBA LatencyBudgetQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxLatencyBudgetQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        LatencyBudgetQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LatencyBudgetQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDuration_t (channel, NULL_OR (value, duration))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxOwnershipQosPolicyKind() - decode/encode/erase a CORBA OwnershipQosPolicyKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxOwnershipQosPolicyKind (

#    if PROTOTYPES
        ComxChannel  channel,
        OwnershipQosPolicyKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OwnershipQosPolicyKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (OwnershipQosPolicyKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxOwnershipQosPolicy() - decode/encode/erase a CORBA OwnershipQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxOwnershipQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        OwnershipQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OwnershipQosPolicy  *value ;
#    endif

{
    CHECK (ddmxOwnershipQosPolicyKind (channel, NULL_OR (value, kind))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxOwnershipStrengthQosPolicy() - decode/encode/erase a CORBA OwnershipStrengthQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxOwnershipStrengthQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        OwnershipStrengthQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OwnershipStrengthQosPolicy  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxLivelinessQosPolicyKind() - decode/encode/erase a CORBA LivelinessQosPolicyKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxLivelinessQosPolicyKind (

#    if PROTOTYPES
        ComxChannel  channel,
        LivelinessQosPolicyKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LivelinessQosPolicyKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (LivelinessQosPolicyKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxLivelinessQosPolicy() - decode/encode/erase a CORBA LivelinessQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxLivelinessQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        LivelinessQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LivelinessQosPolicy  *value ;
#    endif

{
    CHECK (ddmxLivelinessQosPolicyKind (channel, NULL_OR (value, kind))) ;
    CHECK (ddmxDuration_t (channel, NULL_OR (value, lease_duration))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxTimeBasedFilterQosPolicy() - decode/encode/erase a CORBA TimeBasedFilterQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxTimeBasedFilterQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        TimeBasedFilterQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TimeBasedFilterQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDuration_t (channel, NULL_OR (value, minimum_separation))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxPartitionQosPolicy() - decode/encode/erase a CORBA PartitionQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxPartitionQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        PartitionQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PartitionQosPolicy  *value ;
#    endif

{
    CHECK (comxStringSeq (channel, NULL_OR (value, name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxReliabilityQosPolicyKind() - decode/encode/erase a CORBA ReliabilityQosPolicyKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxReliabilityQosPolicyKind (

#    if PROTOTYPES
        ComxChannel  channel,
        ReliabilityQosPolicyKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReliabilityQosPolicyKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ReliabilityQosPolicyKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxReliabilityQosPolicy() - decode/encode/erase a CORBA ReliabilityQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxReliabilityQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        ReliabilityQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReliabilityQosPolicy  *value ;
#    endif

{
    CHECK (ddmxReliabilityQosPolicyKind (channel, NULL_OR (value, kind))) ;
    CHECK (ddmxDuration_t (channel, NULL_OR (value, max_blocking_time))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDestinationOrderQosPolicyKind() - decode/encode/erase a CORBA DestinationOrderQosPolicyKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDestinationOrderQosPolicyKind (

#    if PROTOTYPES
        ComxChannel  channel,
        DestinationOrderQosPolicyKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DestinationOrderQosPolicyKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (DestinationOrderQosPolicyKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDestinationOrderQosPolicy() - decode/encode/erase a CORBA DestinationOrderQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDestinationOrderQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        DestinationOrderQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DestinationOrderQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDestinationOrderQosPolicyKind (channel, NULL_OR (value, kind))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxHistoryQosPolicyKind() - decode/encode/erase a CORBA HistoryQosPolicyKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxHistoryQosPolicyKind (

#    if PROTOTYPES
        ComxChannel  channel,
        HistoryQosPolicyKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        HistoryQosPolicyKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (HistoryQosPolicyKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxHistoryQosPolicy() - decode/encode/erase a CORBA HistoryQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxHistoryQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        HistoryQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        HistoryQosPolicy  *value ;
#    endif

{
    CHECK (ddmxHistoryQosPolicyKind (channel, NULL_OR (value, kind))) ;
    CHECK (comxLong (channel, NULL_OR (value, depth))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxResourceLimitsQosPolicy() - decode/encode/erase a CORBA ResourceLimitsQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxResourceLimitsQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        ResourceLimitsQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ResourceLimitsQosPolicy  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, max_samples))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_instances))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_samples_per_instance))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxEntityFactoryQosPolicy() - decode/encode/erase a CORBA EntityFactoryQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxEntityFactoryQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        EntityFactoryQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EntityFactoryQosPolicy  *value ;
#    endif

{
    CHECK (comxBoolean (channel, NULL_OR (value, autoenable_created_entities))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxWriterDataLifecycleQosPolicy() - decode/encode/erase a CORBA WriterDataLifecycleQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxWriterDataLifecycleQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        WriterDataLifecycleQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        WriterDataLifecycleQosPolicy  *value ;
#    endif

{
    CHECK (comxBoolean (channel, NULL_OR (value, autodispose_unregistered_instances))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxReaderDataLifecycleQosPolicy() - decode/encode/erase a CORBA ReaderDataLifecycleQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxReaderDataLifecycleQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        ReaderDataLifecycleQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReaderDataLifecycleQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDuration_t (channel, NULL_OR (value, autopurge_nowriter_samples_delay))) ;
    CHECK (ddmxDuration_t (channel, NULL_OR (value, autopurge_disposed_samples_delay))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDurabilityServiceQosPolicy() - decode/encode/erase a CORBA DurabilityServiceQosPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDurabilityServiceQosPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        DurabilityServiceQosPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DurabilityServiceQosPolicy  *value ;
#    endif

{
    CHECK (ddmxDuration_t (channel, NULL_OR (value, service_cleanup_delay))) ;
    CHECK (ddmxHistoryQosPolicyKind (channel, NULL_OR (value, history_kind))) ;
    CHECK (comxLong (channel, NULL_OR (value, history_depth))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_samples))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_instances))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_samples_per_instance))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDomainParticipantFactoryQos() - decode/encode/erase a CORBA DomainParticipantFactoryQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDomainParticipantFactoryQos (

#    if PROTOTYPES
        ComxChannel  channel,
        DomainParticipantFactoryQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DomainParticipantFactoryQos  *value ;
#    endif

{
    CHECK (ddmxEntityFactoryQosPolicy (channel, NULL_OR (value, entity_factory))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDomainParticipantQos() - decode/encode/erase a CORBA DomainParticipantQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDomainParticipantQos (

#    if PROTOTYPES
        ComxChannel  channel,
        DomainParticipantQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DomainParticipantQos  *value ;
#    endif

{
    CHECK (ddmxUserDataQosPolicy (channel, NULL_OR (value, user_data))) ;
    CHECK (ddmxEntityFactoryQosPolicy (channel, NULL_OR (value, entity_factory))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxTopicQos() - decode/encode/erase a CORBA TopicQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxTopicQos (

#    if PROTOTYPES
        ComxChannel  channel,
        TopicQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TopicQos  *value ;
#    endif

{
    CHECK (ddmxTopicDataQosPolicy (channel, NULL_OR (value, topic_data))) ;
    CHECK (ddmxDurabilityQosPolicy (channel, NULL_OR (value, durability))) ;
    CHECK (ddmxDurabilityServiceQosPolicy (channel, NULL_OR (value, durability_service))) ;
    CHECK (ddmxDeadlineQosPolicy (channel, NULL_OR (value, deadline))) ;
    CHECK (ddmxLatencyBudgetQosPolicy (channel, NULL_OR (value, latency_budget))) ;
    CHECK (ddmxLivelinessQosPolicy (channel, NULL_OR (value, liveliness))) ;
    CHECK (ddmxReliabilityQosPolicy (channel, NULL_OR (value, reliability))) ;
    CHECK (ddmxDestinationOrderQosPolicy (channel, NULL_OR (value, destination_order))) ;
    CHECK (ddmxHistoryQosPolicy (channel, NULL_OR (value, history))) ;
    CHECK (ddmxResourceLimitsQosPolicy (channel, NULL_OR (value, resource_limits))) ;
    CHECK (ddmxTransportPriorityQosPolicy (channel, NULL_OR (value, transport_priority))) ;
    CHECK (ddmxLifespanQosPolicy (channel, NULL_OR (value, lifespan))) ;
    CHECK (ddmxOwnershipQosPolicy (channel, NULL_OR (value, ownership))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDataWriterQos() - decode/encode/erase a CORBA DataWriterQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDataWriterQos (

#    if PROTOTYPES
        ComxChannel  channel,
        DataWriterQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DataWriterQos  *value ;
#    endif

{
    CHECK (ddmxDurabilityQosPolicy (channel, NULL_OR (value, durability))) ;
    CHECK (ddmxDurabilityServiceQosPolicy (channel, NULL_OR (value, durability_service))) ;
    CHECK (ddmxDeadlineQosPolicy (channel, NULL_OR (value, deadline))) ;
    CHECK (ddmxLatencyBudgetQosPolicy (channel, NULL_OR (value, latency_budget))) ;
    CHECK (ddmxLivelinessQosPolicy (channel, NULL_OR (value, liveliness))) ;
    CHECK (ddmxReliabilityQosPolicy (channel, NULL_OR (value, reliability))) ;
    CHECK (ddmxDestinationOrderQosPolicy (channel, NULL_OR (value, destination_order))) ;
    CHECK (ddmxHistoryQosPolicy (channel, NULL_OR (value, history))) ;
    CHECK (ddmxResourceLimitsQosPolicy (channel, NULL_OR (value, resource_limits))) ;
    CHECK (ddmxTransportPriorityQosPolicy (channel, NULL_OR (value, transport_priority))) ;
    CHECK (ddmxLifespanQosPolicy (channel, NULL_OR (value, lifespan))) ;
    CHECK (ddmxUserDataQosPolicy (channel, NULL_OR (value, user_data))) ;
    CHECK (ddmxOwnershipQosPolicy (channel, NULL_OR (value, ownership))) ;
    CHECK (ddmxOwnershipStrengthQosPolicy (channel, NULL_OR (value, ownership_strength))) ;
    CHECK (ddmxWriterDataLifecycleQosPolicy (channel, NULL_OR (value, writer_data_lifecycle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxPublisherQos() - decode/encode/erase a CORBA PublisherQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxPublisherQos (

#    if PROTOTYPES
        ComxChannel  channel,
        PublisherQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PublisherQos  *value ;
#    endif

{
    CHECK (ddmxPresentationQosPolicy (channel, NULL_OR (value, presentation))) ;
    CHECK (ddmxPartitionQosPolicy (channel, NULL_OR (value, partition))) ;
    CHECK (ddmxGroupDataQosPolicy (channel, NULL_OR (value, group_data))) ;
    CHECK (ddmxEntityFactoryQosPolicy (channel, NULL_OR (value, entity_factory))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDataReaderQos() - decode/encode/erase a CORBA DataReaderQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDataReaderQos (

#    if PROTOTYPES
        ComxChannel  channel,
        DataReaderQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DataReaderQos  *value ;
#    endif

{
    CHECK (ddmxDurabilityQosPolicy (channel, NULL_OR (value, durability))) ;
    CHECK (ddmxDeadlineQosPolicy (channel, NULL_OR (value, deadline))) ;
    CHECK (ddmxLatencyBudgetQosPolicy (channel, NULL_OR (value, latency_budget))) ;
    CHECK (ddmxLivelinessQosPolicy (channel, NULL_OR (value, liveliness))) ;
    CHECK (ddmxReliabilityQosPolicy (channel, NULL_OR (value, reliability))) ;
    CHECK (ddmxDestinationOrderQosPolicy (channel, NULL_OR (value, destination_order))) ;
    CHECK (ddmxHistoryQosPolicy (channel, NULL_OR (value, history))) ;
    CHECK (ddmxResourceLimitsQosPolicy (channel, NULL_OR (value, resource_limits))) ;
    CHECK (ddmxUserDataQosPolicy (channel, NULL_OR (value, user_data))) ;
    CHECK (ddmxOwnershipQosPolicy (channel, NULL_OR (value, ownership))) ;
    CHECK (ddmxTimeBasedFilterQosPolicy (channel, NULL_OR (value, time_based_filter))) ;
    CHECK (ddmxReaderDataLifecycleQosPolicy (channel, NULL_OR (value, reader_data_lifecycle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSubscriberQos() - decode/encode/erase a CORBA SubscriberQos structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSubscriberQos (

#    if PROTOTYPES
        ComxChannel  channel,
        SubscriberQos  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SubscriberQos  *value ;
#    endif

{
    CHECK (ddmxPresentationQosPolicy (channel, NULL_OR (value, presentation))) ;
    CHECK (ddmxPartitionQosPolicy (channel, NULL_OR (value, partition))) ;
    CHECK (ddmxGroupDataQosPolicy (channel, NULL_OR (value, group_data))) ;
    CHECK (ddmxEntityFactoryQosPolicy (channel, NULL_OR (value, entity_factory))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxParticipantBuiltinTopicData() - decode/encode/erase a CORBA ParticipantBuiltinTopicData structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxParticipantBuiltinTopicData (

#    if PROTOTYPES
        ComxChannel  channel,
        ParticipantBuiltinTopicData  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ParticipantBuiltinTopicData  *value ;
#    endif

{
    CHECK (ddmxBuiltinTopicKey_t (channel, NULL_OR (value, key))) ;
    CHECK (ddmxUserDataQosPolicy (channel, NULL_OR (value, user_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxTopicBuiltinTopicData() - decode/encode/erase a CORBA TopicBuiltinTopicData structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxTopicBuiltinTopicData (

#    if PROTOTYPES
        ComxChannel  channel,
        TopicBuiltinTopicData  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TopicBuiltinTopicData  *value ;
#    endif

{
    CHECK (ddmxBuiltinTopicKey_t (channel, NULL_OR (value, key))) ;
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, type_name))) ;
    CHECK (ddmxDurabilityQosPolicy (channel, NULL_OR (value, durability))) ;
    CHECK (ddmxDurabilityServiceQosPolicy (channel, NULL_OR (value, durability_service))) ;
    CHECK (ddmxDeadlineQosPolicy (channel, NULL_OR (value, deadline))) ;
    CHECK (ddmxLatencyBudgetQosPolicy (channel, NULL_OR (value, latency_budget))) ;
    CHECK (ddmxLivelinessQosPolicy (channel, NULL_OR (value, liveliness))) ;
    CHECK (ddmxReliabilityQosPolicy (channel, NULL_OR (value, reliability))) ;
    CHECK (ddmxTransportPriorityQosPolicy (channel, NULL_OR (value, transport_priority))) ;
    CHECK (ddmxLifespanQosPolicy (channel, NULL_OR (value, lifespan))) ;
    CHECK (ddmxDestinationOrderQosPolicy (channel, NULL_OR (value, destination_order))) ;
    CHECK (ddmxHistoryQosPolicy (channel, NULL_OR (value, history))) ;
    CHECK (ddmxResourceLimitsQosPolicy (channel, NULL_OR (value, resource_limits))) ;
    CHECK (ddmxOwnershipQosPolicy (channel, NULL_OR (value, ownership))) ;
    CHECK (ddmxTopicDataQosPolicy (channel, NULL_OR (value, topic_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxPublicationBuiltinTopicData() - decode/encode/erase a CORBA PublicationBuiltinTopicData structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxPublicationBuiltinTopicData (

#    if PROTOTYPES
        ComxChannel  channel,
        PublicationBuiltinTopicData  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PublicationBuiltinTopicData  *value ;
#    endif

{
    CHECK (ddmxBuiltinTopicKey_t (channel, NULL_OR (value, key))) ;
    CHECK (ddmxBuiltinTopicKey_t (channel, NULL_OR (value, participant_key))) ;
    CHECK (comxString (channel, NULL_OR (value, topic_name))) ;
    CHECK (comxString (channel, NULL_OR (value, type_name))) ;
    CHECK (ddmxDurabilityQosPolicy (channel, NULL_OR (value, durability))) ;
    CHECK (ddmxDurabilityServiceQosPolicy (channel, NULL_OR (value, durability_service))) ;
    CHECK (ddmxDeadlineQosPolicy (channel, NULL_OR (value, deadline))) ;
    CHECK (ddmxLatencyBudgetQosPolicy (channel, NULL_OR (value, latency_budget))) ;
    CHECK (ddmxLivelinessQosPolicy (channel, NULL_OR (value, liveliness))) ;
    CHECK (ddmxReliabilityQosPolicy (channel, NULL_OR (value, reliability))) ;
    CHECK (ddmxLifespanQosPolicy (channel, NULL_OR (value, lifespan))) ;
    CHECK (ddmxUserDataQosPolicy (channel, NULL_OR (value, user_data))) ;
    CHECK (ddmxOwnershipQosPolicy (channel, NULL_OR (value, ownership))) ;
    CHECK (ddmxOwnershipStrengthQosPolicy (channel, NULL_OR (value, ownership_strength))) ;
    CHECK (ddmxDestinationOrderQosPolicy (channel, NULL_OR (value, destination_order))) ;
    CHECK (ddmxPresentationQosPolicy (channel, NULL_OR (value, presentation))) ;
    CHECK (ddmxPartitionQosPolicy (channel, NULL_OR (value, partition))) ;
    CHECK (ddmxTopicDataQosPolicy (channel, NULL_OR (value, topic_data))) ;
    CHECK (ddmxGroupDataQosPolicy (channel, NULL_OR (value, group_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSubscriptionBuiltinTopicData() - decode/encode/erase a CORBA SubscriptionBuiltinTopicData structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSubscriptionBuiltinTopicData (

#    if PROTOTYPES
        ComxChannel  channel,
        SubscriptionBuiltinTopicData  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SubscriptionBuiltinTopicData  *value ;
#    endif

{
    CHECK (ddmxBuiltinTopicKey_t (channel, NULL_OR (value, key))) ;
    CHECK (ddmxBuiltinTopicKey_t (channel, NULL_OR (value, participant_key))) ;
    CHECK (comxString (channel, NULL_OR (value, topic_name))) ;
    CHECK (comxString (channel, NULL_OR (value, type_name))) ;
    CHECK (ddmxDurabilityQosPolicy (channel, NULL_OR (value, durability))) ;
    CHECK (ddmxDeadlineQosPolicy (channel, NULL_OR (value, deadline))) ;
    CHECK (ddmxLatencyBudgetQosPolicy (channel, NULL_OR (value, latency_budget))) ;
    CHECK (ddmxLivelinessQosPolicy (channel, NULL_OR (value, liveliness))) ;
    CHECK (ddmxReliabilityQosPolicy (channel, NULL_OR (value, reliability))) ;
    CHECK (ddmxOwnershipQosPolicy (channel, NULL_OR (value, ownership))) ;
    CHECK (ddmxDestinationOrderQosPolicy (channel, NULL_OR (value, destination_order))) ;
    CHECK (ddmxUserDataQosPolicy (channel, NULL_OR (value, user_data))) ;
    CHECK (ddmxTimeBasedFilterQosPolicy (channel, NULL_OR (value, time_based_filter))) ;
    CHECK (ddmxPresentationQosPolicy (channel, NULL_OR (value, presentation))) ;
    CHECK (ddmxPartitionQosPolicy (channel, NULL_OR (value, partition))) ;
    CHECK (ddmxTopicDataQosPolicy (channel, NULL_OR (value, topic_data))) ;
    CHECK (ddmxGroupDataQosPolicy (channel, NULL_OR (value, group_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSampleInfo() - decode/encode/erase a CORBA SampleInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSampleInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        SampleInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SampleInfo  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, sample_state))) ;
    CHECK (comxULong (channel, NULL_OR (value, view_state))) ;
    CHECK (comxULong (channel, NULL_OR (value, instance_state))) ;
    CHECK (ddmxTime_t (channel, NULL_OR (value, source_timestamp))) ;
    CHECK (comxLong (channel, NULL_OR (value, instance_handle))) ;
    CHECK (comxLong (channel, NULL_OR (value, publication_handle))) ;
    CHECK (comxLong (channel, NULL_OR (value, disposed_generation_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, no_writers_generation_count))) ;
    CHECK (comxLong (channel, NULL_OR (value, sample_rank))) ;
    CHECK (comxLong (channel, NULL_OR (value, generation_rank))) ;
    CHECK (comxLong (channel, NULL_OR (value, absolute_generation_rank))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, valid_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSampleInfoSeq() - decode/encode/erase a sequence of CORBA SampleInfo structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSampleInfoSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        SampleInfoSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SampleInfoSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) ddmxSampleInfo, sizeof (SampleInfo))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxReferenceScope() - decode/encode/erase a CORBA ReferenceScope enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxReferenceScope (

#    if PROTOTYPES
        ComxChannel  channel,
        ReferenceScope  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReferenceScope  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ReferenceScope) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxObjectScope() - decode/encode/erase a CORBA ObjectScope enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxObjectScope (

#    if PROTOTYPES
        ComxChannel  channel,
        ObjectScope  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ObjectScope  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ObjectScope) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDCPSState() - decode/encode/erase a CORBA DCPSState enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDCPSState (

#    if PROTOTYPES
        ComxChannel  channel,
        DCPSState  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DCPSState  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (DCPSState) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxCacheUsage() - decode/encode/erase a CORBA CacheUsage enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxCacheUsage (

#    if PROTOTYPES
        ComxChannel  channel,
        CacheUsage  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CacheUsage  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (CacheUsage) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxObjectState() - decode/encode/erase a CORBA ObjectState enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxObjectState (

#    if PROTOTYPES
        ComxChannel  channel,
        ObjectState  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ObjectState  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ObjectState) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxDLRLOid() - decode/encode/erase a CORBA DLRLOid structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxDLRLOid (

#    if PROTOTYPES
        ComxChannel  channel,
        DLRLOid  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DLRLOid  *value ;
#    endif

{
    CHECK (comxArray (channel, NULL_OR (value, value), (ComxFunc) comxLong, sizeof (long), 3)) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxObjectRootSeq() - decode/encode/erase a sequence of CORBA ObjectRoot structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxObjectRootSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ObjectRootSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ObjectRootSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxOctetSeq, sizeof (OctetSeq))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxObjectHomeSeq() - decode/encode/erase a sequence of CORBA ObjectHome structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxObjectHomeSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ObjectHomeSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ObjectHomeSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxObjectListenerSeq() - decode/encode/erase a sequence of CORBA ObjectListener structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxObjectListenerSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ObjectListenerSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ObjectListenerSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxSelectionSeq() - decode/encode/erase a sequence of CORBA Selection structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxSelectionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        SelectionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SelectionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxCacheBaseSeq() - decode/encode/erase a sequence of CORBA CacheBase structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxCacheBaseSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        CacheBaseSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CacheBaseSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxCacheAccessSeq() - decode/encode/erase a sequence of CORBA CacheAccess structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxCacheAccessSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        CacheAccessSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CacheAccessSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxCacheListenerSeq() - decode/encode/erase a sequence of CORBA CacheListener structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxCacheListenerSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        CacheListenerSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CacheListenerSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxContractSeq() - decode/encode/erase a sequence of CORBA Contract structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxContractSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ContractSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContractSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxRelationKind() - decode/encode/erase a CORBA RelationKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxRelationKind (

#    if PROTOTYPES
        ComxChannel  channel,
        RelationKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RelationKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (RelationKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxRelationDescriptionSeq() - decode/encode/erase a sequence of CORBA RelationDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxRelationDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        RelationDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RelationDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxOctetSeq, sizeof (OctetSeq))) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxCriterionKind() - decode/encode/erase a CORBA CriterionKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxCriterionKind (

#    if PROTOTYPES
        ComxChannel  channel,
        CriterionKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CriterionKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (CriterionKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxMembershipState() - decode/encode/erase a CORBA MembershipState enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxMembershipState (

#    if PROTOTYPES
        ComxChannel  channel,
        MembershipState  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MembershipState  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (MembershipState) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*!*****************************************************************************
    ddmxCacheKind() - decode/encode/erase a CORBA CacheKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS

errno_t  ddmxCacheKind (

#    if PROTOTYPES
        ComxChannel  channel,
        CacheKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CacheKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (CacheKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_DDS */

/*******************************************************************************
    Lookup Tables - for converting named constants to numbers and vice-versa;
        see the coliToName() and coliToNumber() functions.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  CacheKindLUT[] = {
      { (long) CACHE_KIND, "CACHE_KIND" },
      { (long) CACHEACCESS_KIND, "CACHEACCESS_KIND" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  CacheUsageLUT[] = {
      { (long) READ_ONLY, "READ_ONLY" },
      { (long) WRITE_ONLY, "WRITE_ONLY" },
      { (long) READ_WRITE, "READ_WRITE" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  CriterionKindLUT[] = {
      { (long) QUERY, "QUERY" },
      { (long) FILTER, "FILTER" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  DCPSStateLUT[] = {
      { (long) DCPS_INITIAL, "DCPS_INITIAL" },
      { (long) DCPS_REGISTERED, "DCPS_REGISTERED" },
      { (long) DCPS_ENABLED, "DCPS_ENABLED" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  DestinationOrderQosPolicyKindLUT[] = {
      { (long) BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS, "BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS" },
      { (long) BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS, "BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  DurabilityQosPolicyKindLUT[] = {
      { (long) VOLATILE_DURABILITY_QOS, "VOLATILE_DURABILITY_QOS" },
      { (long) TRANSIENT_LOCAL_DURABILITY_QOS, "TRANSIENT_LOCAL_DURABILITY_QOS" },
      { (long) TRANSIENT_DURABILITY_QOS, "TRANSIENT_DURABILITY_QOS" },
      { (long) PERSISTENT_DURABILITY_QOS, "PERSISTENT_DURABILITY_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  HistoryQosPolicyKindLUT[] = {
      { (long) KEEP_LAST_HISTORY_QOS, "KEEP_LAST_HISTORY_QOS" },
      { (long) KEEP_ALL_HISTORY_QOS, "KEEP_ALL_HISTORY_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  LivelinessQosPolicyKindLUT[] = {
      { (long) AUTOMATIC_LIVELINESS_QOS, "AUTOMATIC_LIVELINESS_QOS" },
      { (long) MANUAL_BY_PARTICIPANT_LIVELINESS_QOS, "MANUAL_BY_PARTICIPANT_LIVELINESS_QOS" },
      { (long) MANUAL_BY_TOPIC_LIVELINESS_QOS, "MANUAL_BY_TOPIC_LIVELINESS_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  MembershipStateLUT[] = {
      { (long) UNDEFINED_MEMBERSHIP, "UNDEFINED_MEMBERSHIP" },
      { (long) ALREADY_MEMBER, "ALREADY_MEMBER" },
      { (long) NOT_MEMBER, "NOT_MEMBER" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  ObjectScopeLUT[] = {
      { (long) SIMPLE_OBJECT_SCOPE, "SIMPLE_OBJECT_SCOPE" },
      { (long) CONTAINED_OBJECTS_SCOPE, "CONTAINED_OBJECTS_SCOPE" },
      { (long) RELATED_OBJECTS_SCOPE, "RELATED_OBJECTS_SCOPE" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  ObjectStateLUT[] = {
      { (long) OBJECT_VOID, "OBJECT_VOID" },
      { (long) OBJECT_NEW, "OBJECT_NEW" },
      { (long) OBJECT_NOT_MODIFIED, "OBJECT_NOT_MODIFIED" },
      { (long) OBJECT_MODIFIED, "OBJECT_MODIFIED" },
      { (long) OBJECT_DELETED, "OBJECT_DELETED" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  OwnershipQosPolicyKindLUT[] = {
      { (long) SHARED_OWNERSHIP_QOS, "SHARED_OWNERSHIP_QOS" },
      { (long) EXCLUSIVE_OWNERSHIP_QOS, "EXCLUSIVE_OWNERSHIP_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  PresentationQosPolicyAccessScopeKindLUT[] = {
      { (long) INSTANCE_PRESENTATION_QOS, "INSTANCE_PRESENTATION_QOS" },
      { (long) TOPIC_PRESENTATION_QOS, "TOPIC_PRESENTATION_QOS" },
      { (long) GROUP_PRESENTATION_QOS, "GROUP_PRESENTATION_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  ReferenceScopeLUT[] = {
      { (long) SIMPLE_CONTENT_SCOPE, "SIMPLE_CONTENT_SCOPE" },
      { (long) REFERENCED_CONTENTS_SCOPE, "REFERENCED_CONTENTS_SCOPE" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  RelationKindLUT[] = {
      { (long) REF_RELATION, "REF_RELATION" },
      { (long) LIST_RELATION, "LIST_RELATION" },
      { (long) INT_MAP_RELATION, "INT_MAP_RELATION" },
      { (long) STR_MAP_RELATION, "STR_MAP_RELATION" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  ReliabilityQosPolicyKindLUT[] = {
      { (long) BEST_EFFORT_RELIABILITY_QOS, "BEST_EFFORT_RELIABILITY_QOS" },
      { (long) RELIABLE_RELIABILITY_QOS, "RELIABLE_RELIABILITY_QOS" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    const  ColiMap  SampleRejectedStatusKindLUT[] = {
      { (long) NOT_REJECTED, "NOT_REJECTED" },
      { (long) REJECTED_BY_INSTANCES_LIMIT, "REJECTED_BY_INSTANCES_LIMIT" },
      { (long) REJECTED_BY_SAMPLES_LIMIT, "REJECTED_BY_SAMPLES_LIMIT" },
      { (long) REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT, "REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_DDS */
