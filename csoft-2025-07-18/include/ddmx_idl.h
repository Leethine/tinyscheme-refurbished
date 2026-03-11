/* $Id: ddmx_idl.h,v 1.6 2021/09/15 04:18:16 alex Exp $ */
/*******************************************************************************
    ../idl/DDS/dds_dlrl.idl
*******************************************************************************/
/*  <stdin> : line 1  */
/*  <built-in> : line 1  */
/*  <command-line> : line 1  */
/*  <stdin> : line 1  */
/*  ../idl/DDS/dds_dcps.idl : line 1  */

/* Module: DDS */

#ifndef IDL_MODULE_DDS
#    define  IDL_MODULE_DDS  1
#endif

typedef  long  DomainId_t ;

typedef  long  InstanceHandle_t ;

typedef  struct  BuiltinTopicKey_t {
    long  value[3] ;
}  BuiltinTopicKey_t ;

typedef  SEQUENCE (InstanceHandle_t, InstanceHandleSeq) ;

typedef  long  ReturnCode_t ;

typedef  long  QosPolicyId_t ;

/*===== "StringSeq" already defined =====*/

typedef  struct  Duration_t {
    long  sec ;
    unsigned  long  nanosec ;
}  Duration_t ;

typedef  struct  Time_t {
    long  sec ;
    unsigned  long  nanosec ;
}  Time_t ;

#define  HANDLE_NIL  (0)
#define  LENGTH_UNLIMITED  (- 1L)
#define  DURATION_INFINITE_SEC  (0x7fffffffL)
#define  DURATION_INFINITE_NSEC  (0x7fffffffUL)
#define  DURATION_ZERO_SEC  (0L)
#define  DURATION_ZERO_NSEC  (0UL)
#define  TIME_INVALID_SEC  (- 1L)
#define  TIME_INVALID_NSEC  (0xffffffffUL)
#define  RETCODE_OK  (0)
#define  RETCODE_ERROR  (1)
#define  RETCODE_UNSUPPORTED  (2)
#define  RETCODE_BAD_PARAMETER  (3)
#define  RETCODE_PRECONDITION_NOT_MET  (4)
#define  RETCODE_OUT_OF_RESOURCES  (5)
#define  RETCODE_NOT_ENABLED  (6)
#define  RETCODE_IMMUTABLE_POLICY  (7)
#define  RETCODE_INCONSISTENT_POLICY  (8)
#define  RETCODE_ALREADY_DELETED  (9)
#define  RETCODE_TIMEOUT  (10)
#define  RETCODE_NO_DATA  (11)
#define  RETCODE_ILLEGAL_OPERATION  (12)

typedef  unsigned  long  StatusKind ;

typedef  unsigned  long  StatusMask ;

#define  INCONSISTENT_TOPIC_STATUS  (0x0001 << 0)
#define  OFFERED_DEADLINE_MISSED_STATUS  (0x0001 << 1)
#define  REQUESTED_DEADLINE_MISSED_STATUS  (0x0001 << 2)
#define  OFFERED_INCOMPATIBLE_QOS_STATUS  (0x0001 << 5)
#define  REQUESTED_INCOMPATIBLE_QOS_STATUS  (0x0001 << 6)
#define  SAMPLE_LOST_STATUS  (0x0001 << 7)
#define  SAMPLE_REJECTED_STATUS  (0x0001 << 8)
#define  DATA_ON_READERS_STATUS  (0x0001 << 9)
#define  DATA_AVAILABLE_STATUS  (0x0001 << 10)
#define  LIVELINESS_LOST_STATUS  (0x0001 << 11)
#define  LIVELINESS_CHANGED_STATUS  (0x0001 << 12)
#define  PUBLICATION_MATCHED_STATUS  (0x0001 << 13)
#define  SUBSCRIPTION_MATCHED_STATUS  (0x0001 << 14)

typedef  struct  InconsistentTopicStatus {
    long  total_count ;
    long  total_count_change ;
}  InconsistentTopicStatus ;

typedef  struct  SampleLostStatus {
    long  total_count ;
    long  total_count_change ;
}  SampleLostStatus ;

typedef  enum  SampleRejectedStatusKind {
    NOT_REJECTED,
    REJECTED_BY_INSTANCES_LIMIT,
    REJECTED_BY_SAMPLES_LIMIT,
    REJECTED_BY_SAMPLES_PER_INSTANCE_LIMIT
}  SampleRejectedStatusKind ;

typedef  struct  SampleRejectedStatus {
    long  total_count ;
    long  total_count_change ;
    SampleRejectedStatusKind  last_reason ;
    InstanceHandle_t  last_instance_handle ;
}  SampleRejectedStatus ;

typedef  struct  LivelinessLostStatus {
    long  total_count ;
    long  total_count_change ;
}  LivelinessLostStatus ;

typedef  struct  LivelinessChangedStatus {
    long  alive_count ;
    long  not_alive_count ;
    long  alive_count_change ;
    long  not_alive_count_change ;
    InstanceHandle_t  last_publication_handle ;
}  LivelinessChangedStatus ;

typedef  struct  OfferedDeadlineMissedStatus {
    long  total_count ;
    long  total_count_change ;
    InstanceHandle_t  last_instance_handle ;
}  OfferedDeadlineMissedStatus ;

typedef  struct  RequestedDeadlineMissedStatus {
    long  total_count ;
    long  total_count_change ;
    InstanceHandle_t  last_instance_handle ;
}  RequestedDeadlineMissedStatus ;

typedef  struct  QosPolicyCount {
    QosPolicyId_t  policy_id ;
    long  count ;
}  QosPolicyCount ;

typedef  SEQUENCE (QosPolicyCount, QosPolicyCountSeq) ;

typedef  struct  OfferedIncompatibleQosStatus {
    long  total_count ;
    long  total_count_change ;
    QosPolicyId_t  last_policy_id ;
    QosPolicyCountSeq  policies ;
}  OfferedIncompatibleQosStatus ;

typedef  struct  RequestedIncompatibleQosStatus {
    long  total_count ;
    long  total_count_change ;
    QosPolicyId_t  last_policy_id ;
    QosPolicyCountSeq  policies ;
}  RequestedIncompatibleQosStatus ;

typedef  struct  PublicationMatchedStatus {
    long  total_count ;
    long  total_count_change ;
    long  current_count ;
    long  current_count_change ;
    InstanceHandle_t  last_subscription_handle ;
}  PublicationMatchedStatus ;

typedef  struct  SubscriptionMatchedStatus {
    long  total_count ;
    long  total_count_change ;
    long  current_count ;
    long  current_count_change ;
    InstanceHandle_t  last_publication_handle ;
}  SubscriptionMatchedStatus ;

typedef  IOR  Listener ;
typedef  IOR  Entity ;
typedef  IOR  TopicDescription ;
typedef  IOR  Topic ;
typedef  IOR  ContentFilteredTopic ;
typedef  IOR  MultiTopic ;
typedef  IOR  DataWriter ;
typedef  IOR  DataReader ;
typedef  IOR  Subscriber ;
typedef  IOR  Publisher ;

typedef  SEQUENCE (DataReader, DataReaderSeq) ;

typedef  IOR  TopicListener ;
typedef  IOR  DataWriterListener ;
typedef  IOR  PublisherListener ;
typedef  IOR  DataReaderListener ;
typedef  IOR  SubscriberListener ;
typedef  IOR  DomainParticipantListener ;
typedef  IOR  Condition ;

typedef  SEQUENCE (Condition, ConditionSeq) ;

typedef  IOR  WaitSet ;
typedef  IOR  GuardCondition ;
typedef  IOR  StatusCondition ;

typedef  unsigned  long  SampleStateKind ;

#define  READ_SAMPLE_STATE  (0x0001 << 0)
#define  NOT_READ_SAMPLE_STATE  (0x0001 << 1)

typedef  unsigned  long  SampleStateMask ;

#define  ANY_SAMPLE_STATE  (0xffff)

typedef  unsigned  long  ViewStateKind ;

#define  NEW_VIEW_STATE  (0x0001 << 0)
#define  NOT_NEW_VIEW_STATE  (0x0001 << 1)

typedef  unsigned  long  ViewStateMask ;

#define  ANY_VIEW_STATE  (0xffff)

typedef  unsigned  long  InstanceStateKind ;

#define  ALIVE_INSTANCE_STATE  (0x0001 << 0)
#define  NOT_ALIVE_DISPOSED_INSTANCE_STATE  (0x0001 << 1)
#define  NOT_ALIVE_NO_WRITERS_INSTANCE_STATE  (0x0001 << 2)

typedef  unsigned  long  InstanceStateMask ;

#define  ANY_INSTANCE_STATE  (0xffff)
#define  NOT_ALIVE_INSTANCE_STATE  (0x006)

typedef  IOR  ReadCondition ;
typedef  IOR  QueryCondition ;

#define  USERDATA_QOS_POLICY_NAME  ("UserData")
#define  DURABILITY_QOS_POLICY_NAME  ("Durability")
#define  PRESENTATION_QOS_POLICY_NAME  ("Presentation")
#define  DEADLINE_QOS_POLICY_NAME  ("Deadline")
#define  LATENCYBUDGET_QOS_POLICY_NAME  ("LatencyBudget")
#define  OWNERSHIP_QOS_POLICY_NAME  ("Ownership")
#define  OWNERSHIPSTRENGTH_QOS_POLICY_NAME  ("OwnershipStrength")
#define  LIVELINESS_QOS_POLICY_NAME  ("Liveliness")
#define  TIMEBASEDFILTER_QOS_POLICY_NAME  ("TimeBasedFilter")
#define  PARTITION_QOS_POLICY_NAME  ("Partition")
#define  RELIABILITY_QOS_POLICY_NAME  ("Reliability")
#define  DESTINATIONORDER_QOS_POLICY_NAME  ("DestinationOrder")
#define  HISTORY_QOS_POLICY_NAME  ("History")
#define  RESOURCELIMITS_QOS_POLICY_NAME  ("ResourceLimits")
#define  ENTITYFACTORY_QOS_POLICY_NAME  ("EntityFactory")
#define  WRITERDATALIFECYCLE_QOS_POLICY_NAME  ("WriterDataLifecycle")
#define  READERDATALIFECYCLE_QOS_POLICY_NAME  ("ReaderDataLifecycle")
#define  TOPICDATA_QOS_POLICY_NAME  ("TopicData")
#define  GROUPDATA_QOS_POLICY_NAME  ("TransportPriority")
#define  LIFESPAN_QOS_POLICY_NAME  ("Lifespan")
#define  DURABILITYSERVICE_POLICY_NAME  ("DurabilityService")
#define  INVALID_QOS_POLICY_ID  (0)
#define  USERDATA_QOS_POLICY_ID  (1)
#define  DURABILITY_QOS_POLICY_ID  (2)
#define  PRESENTATION_QOS_POLICY_ID  (3)
#define  DEADLINE_QOS_POLICY_ID  (4)
#define  LATENCYBUDGET_QOS_POLICY_ID  (5)
#define  OWNERSHIP_QOS_POLICY_ID  (6)
#define  OWNERSHIPSTRENGTH_QOS_POLICY_ID  (7)
#define  LIVELINESS_QOS_POLICY_ID  (8)
#define  TIMEBASEDFILTER_QOS_POLICY_ID  (9)
#define  PARTITION_QOS_POLICY_ID  (10)
#define  RELIABILITY_QOS_POLICY_ID  (11)
#define  DESTINATIONORDER_QOS_POLICY_ID  (12)
#define  HISTORY_QOS_POLICY_ID  (13)
#define  RESOURCELIMITS_QOS_POLICY_ID  (14)
#define  ENTITYFACTORY_QOS_POLICY_ID  (15)
#define  WRITERDATALIFECYCLE_QOS_POLICY_ID  (16)
#define  READERDATALIFECYCLE_QOS_POLICY_ID  (17)
#define  TOPICDATA_QOS_POLICY_ID  (18)
#define  GROUPDATA_QOS_POLICY_ID  (19)
#define  TRANSPORTPRIORITY_QOS_POLICY_ID  (20)
#define  LIFESPAN_QOS_POLICY_ID  (21)
#define  DURABILITYSERVICE_QOS_POLICY_ID  (22)

typedef  struct  UserDataQosPolicy {
    OctetSeq  value ;
}  UserDataQosPolicy ;

typedef  struct  TopicDataQosPolicy {
    OctetSeq  value ;
}  TopicDataQosPolicy ;

typedef  struct  GroupDataQosPolicy {
    OctetSeq  value ;
}  GroupDataQosPolicy ;

typedef  struct  TransportPriorityQosPolicy {
    long  value ;
}  TransportPriorityQosPolicy ;

typedef  struct  LifespanQosPolicy {
    Duration_t  duration ;
}  LifespanQosPolicy ;

typedef  enum  DurabilityQosPolicyKind {
    VOLATILE_DURABILITY_QOS,
    TRANSIENT_LOCAL_DURABILITY_QOS,
    TRANSIENT_DURABILITY_QOS,
    PERSISTENT_DURABILITY_QOS
}  DurabilityQosPolicyKind ;

typedef  struct  DurabilityQosPolicy {
    DurabilityQosPolicyKind  kind ;
}  DurabilityQosPolicy ;

typedef  enum  PresentationQosPolicyAccessScopeKind {
    INSTANCE_PRESENTATION_QOS,
    TOPIC_PRESENTATION_QOS,
    GROUP_PRESENTATION_QOS
}  PresentationQosPolicyAccessScopeKind ;

typedef  struct  PresentationQosPolicy {
    PresentationQosPolicyAccessScopeKind  access_scope ;
    bool  coherent_access ;
    bool  ordered_access ;
}  PresentationQosPolicy ;

typedef  struct  DeadlineQosPolicy {
    Duration_t  period ;
}  DeadlineQosPolicy ;

typedef  struct  LatencyBudgetQosPolicy {
    Duration_t  duration ;
}  LatencyBudgetQosPolicy ;

typedef  enum  OwnershipQosPolicyKind {
    SHARED_OWNERSHIP_QOS,
    EXCLUSIVE_OWNERSHIP_QOS
}  OwnershipQosPolicyKind ;

typedef  struct  OwnershipQosPolicy {
    OwnershipQosPolicyKind  kind ;
}  OwnershipQosPolicy ;

typedef  struct  OwnershipStrengthQosPolicy {
    long  value ;
}  OwnershipStrengthQosPolicy ;

typedef  enum  LivelinessQosPolicyKind {
    AUTOMATIC_LIVELINESS_QOS,
    MANUAL_BY_PARTICIPANT_LIVELINESS_QOS,
    MANUAL_BY_TOPIC_LIVELINESS_QOS
}  LivelinessQosPolicyKind ;

typedef  struct  LivelinessQosPolicy {
    LivelinessQosPolicyKind  kind ;
    Duration_t  lease_duration ;
}  LivelinessQosPolicy ;

typedef  struct  TimeBasedFilterQosPolicy {
    Duration_t  minimum_separation ;
}  TimeBasedFilterQosPolicy ;

typedef  struct  PartitionQosPolicy {
    StringSeq  name ;
}  PartitionQosPolicy ;

typedef  enum  ReliabilityQosPolicyKind {
    BEST_EFFORT_RELIABILITY_QOS,
    RELIABLE_RELIABILITY_QOS
}  ReliabilityQosPolicyKind ;

typedef  struct  ReliabilityQosPolicy {
    ReliabilityQosPolicyKind  kind ;
    Duration_t  max_blocking_time ;
}  ReliabilityQosPolicy ;

typedef  enum  DestinationOrderQosPolicyKind {
    BY_RECEPTION_TIMESTAMP_DESTINATIONORDER_QOS,
    BY_SOURCE_TIMESTAMP_DESTINATIONORDER_QOS
}  DestinationOrderQosPolicyKind ;

typedef  struct  DestinationOrderQosPolicy {
    DestinationOrderQosPolicyKind  kind ;
}  DestinationOrderQosPolicy ;

typedef  enum  HistoryQosPolicyKind {
    KEEP_LAST_HISTORY_QOS,
    KEEP_ALL_HISTORY_QOS
}  HistoryQosPolicyKind ;

typedef  struct  HistoryQosPolicy {
    HistoryQosPolicyKind  kind ;
    long  depth ;
}  HistoryQosPolicy ;

typedef  struct  ResourceLimitsQosPolicy {
    long  max_samples ;
    long  max_instances ;
    long  max_samples_per_instance ;
}  ResourceLimitsQosPolicy ;

typedef  struct  EntityFactoryQosPolicy {
    bool  autoenable_created_entities ;
}  EntityFactoryQosPolicy ;

typedef  struct  WriterDataLifecycleQosPolicy {
    bool  autodispose_unregistered_instances ;
}  WriterDataLifecycleQosPolicy ;

typedef  struct  ReaderDataLifecycleQosPolicy {
    Duration_t  autopurge_nowriter_samples_delay ;
    Duration_t  autopurge_disposed_samples_delay ;
}  ReaderDataLifecycleQosPolicy ;

typedef  struct  DurabilityServiceQosPolicy {
    Duration_t  service_cleanup_delay ;
    HistoryQosPolicyKind  history_kind ;
    long  history_depth ;
    long  max_samples ;
    long  max_instances ;
    long  max_samples_per_instance ;
}  DurabilityServiceQosPolicy ;

typedef  struct  DomainParticipantFactoryQos {
    EntityFactoryQosPolicy  entity_factory ;
}  DomainParticipantFactoryQos ;

typedef  struct  DomainParticipantQos {
    UserDataQosPolicy  user_data ;
    EntityFactoryQosPolicy  entity_factory ;
}  DomainParticipantQos ;

typedef  struct  TopicQos {
    TopicDataQosPolicy  topic_data ;
    DurabilityQosPolicy  durability ;
    DurabilityServiceQosPolicy  durability_service ;
    DeadlineQosPolicy  deadline ;
    LatencyBudgetQosPolicy  latency_budget ;
    LivelinessQosPolicy  liveliness ;
    ReliabilityQosPolicy  reliability ;
    DestinationOrderQosPolicy  destination_order ;
    HistoryQosPolicy  history ;
    ResourceLimitsQosPolicy  resource_limits ;
    TransportPriorityQosPolicy  transport_priority ;
    LifespanQosPolicy  lifespan ;
    OwnershipQosPolicy  ownership ;
}  TopicQos ;

typedef  struct  DataWriterQos {
    DurabilityQosPolicy  durability ;
    DurabilityServiceQosPolicy  durability_service ;
    DeadlineQosPolicy  deadline ;
    LatencyBudgetQosPolicy  latency_budget ;
    LivelinessQosPolicy  liveliness ;
    ReliabilityQosPolicy  reliability ;
    DestinationOrderQosPolicy  destination_order ;
    HistoryQosPolicy  history ;
    ResourceLimitsQosPolicy  resource_limits ;
    TransportPriorityQosPolicy  transport_priority ;
    LifespanQosPolicy  lifespan ;
    UserDataQosPolicy  user_data ;
    OwnershipQosPolicy  ownership ;
    OwnershipStrengthQosPolicy  ownership_strength ;
    WriterDataLifecycleQosPolicy  writer_data_lifecycle ;
}  DataWriterQos ;

typedef  struct  PublisherQos {
    PresentationQosPolicy  presentation ;
    PartitionQosPolicy  partition ;
    GroupDataQosPolicy  group_data ;
    EntityFactoryQosPolicy  entity_factory ;
}  PublisherQos ;

typedef  struct  DataReaderQos {
    DurabilityQosPolicy  durability ;
    DeadlineQosPolicy  deadline ;
    LatencyBudgetQosPolicy  latency_budget ;
    LivelinessQosPolicy  liveliness ;
    ReliabilityQosPolicy  reliability ;
    DestinationOrderQosPolicy  destination_order ;
    HistoryQosPolicy  history ;
    ResourceLimitsQosPolicy  resource_limits ;
    UserDataQosPolicy  user_data ;
    OwnershipQosPolicy  ownership ;
    TimeBasedFilterQosPolicy  time_based_filter ;
    ReaderDataLifecycleQosPolicy  reader_data_lifecycle ;
}  DataReaderQos ;

typedef  struct  SubscriberQos {
    PresentationQosPolicy  presentation ;
    PartitionQosPolicy  partition ;
    GroupDataQosPolicy  group_data ;
    EntityFactoryQosPolicy  entity_factory ;
}  SubscriberQos ;

typedef  struct  ParticipantBuiltinTopicData {
    BuiltinTopicKey_t  key ;
    UserDataQosPolicy  user_data ;
}  ParticipantBuiltinTopicData ;

typedef  struct  TopicBuiltinTopicData {
    BuiltinTopicKey_t  key ;
    char  *name ;
    char  *type_name ;
    DurabilityQosPolicy  durability ;
    DurabilityServiceQosPolicy  durability_service ;
    DeadlineQosPolicy  deadline ;
    LatencyBudgetQosPolicy  latency_budget ;
    LivelinessQosPolicy  liveliness ;
    ReliabilityQosPolicy  reliability ;
    TransportPriorityQosPolicy  transport_priority ;
    LifespanQosPolicy  lifespan ;
    DestinationOrderQosPolicy  destination_order ;
    HistoryQosPolicy  history ;
    ResourceLimitsQosPolicy  resource_limits ;
    OwnershipQosPolicy  ownership ;
    TopicDataQosPolicy  topic_data ;
}  TopicBuiltinTopicData ;

typedef  struct  PublicationBuiltinTopicData {
    BuiltinTopicKey_t  key ;
    BuiltinTopicKey_t  participant_key ;
    char  *topic_name ;
    char  *type_name ;
    DurabilityQosPolicy  durability ;
    DurabilityServiceQosPolicy  durability_service ;
    DeadlineQosPolicy  deadline ;
    LatencyBudgetQosPolicy  latency_budget ;
    LivelinessQosPolicy  liveliness ;
    ReliabilityQosPolicy  reliability ;
    LifespanQosPolicy  lifespan ;
    UserDataQosPolicy  user_data ;
    OwnershipQosPolicy  ownership ;
    OwnershipStrengthQosPolicy  ownership_strength ;
    DestinationOrderQosPolicy  destination_order ;
    PresentationQosPolicy  presentation ;
    PartitionQosPolicy  partition ;
    TopicDataQosPolicy  topic_data ;
    GroupDataQosPolicy  group_data ;
}  PublicationBuiltinTopicData ;

typedef  struct  SubscriptionBuiltinTopicData {
    BuiltinTopicKey_t  key ;
    BuiltinTopicKey_t  participant_key ;
    char  *topic_name ;
    char  *type_name ;
    DurabilityQosPolicy  durability ;
    DeadlineQosPolicy  deadline ;
    LatencyBudgetQosPolicy  latency_budget ;
    LivelinessQosPolicy  liveliness ;
    ReliabilityQosPolicy  reliability ;
    OwnershipQosPolicy  ownership ;
    DestinationOrderQosPolicy  destination_order ;
    UserDataQosPolicy  user_data ;
    TimeBasedFilterQosPolicy  time_based_filter ;
    PresentationQosPolicy  presentation ;
    PartitionQosPolicy  partition ;
    TopicDataQosPolicy  topic_data ;
    GroupDataQosPolicy  group_data ;
}  SubscriptionBuiltinTopicData ;

typedef  IOR  DomainParticipant ;
typedef  IOR  DomainParticipantFactory ;
typedef  IOR  TypeSupport ;

typedef  struct  SampleInfo {
    SampleStateKind  sample_state ;
    ViewStateKind  view_state ;
    InstanceStateKind  instance_state ;
    Time_t  source_timestamp ;
    InstanceHandle_t  instance_handle ;
    InstanceHandle_t  publication_handle ;
    long  disposed_generation_count ;
    long  no_writers_generation_count ;
    long  sample_rank ;
    long  generation_rank ;
    long  absolute_generation_rank ;
    bool  valid_data ;
}  SampleInfo ;

typedef  SEQUENCE (SampleInfo, SampleInfoSeq) ;

/* Module: DDS */

#ifndef IDL_MODULE_DDS
#    define  IDL_MODULE_DDS  1
#endif

typedef  enum  ReferenceScope {
    SIMPLE_CONTENT_SCOPE,
    REFERENCED_CONTENTS_SCOPE
}  ReferenceScope ;

typedef  enum  ObjectScope {
    SIMPLE_OBJECT_SCOPE,
    CONTAINED_OBJECTS_SCOPE,
    RELATED_OBJECTS_SCOPE
}  ObjectScope ;

typedef  enum  DCPSState {
    DCPS_INITIAL,
    DCPS_REGISTERED,
    DCPS_ENABLED
}  DCPSState ;

typedef  enum  CacheUsage {
    READ_ONLY,
    WRITE_ONLY,
    READ_WRITE
}  CacheUsage ;

typedef  enum  ObjectState {
    OBJECT_VOID,
    OBJECT_NEW,
    OBJECT_NOT_MODIFIED,
    OBJECT_MODIFIED,
    OBJECT_DELETED
}  ObjectState ;

typedef  struct  DLRLOid {
    long  value[3] ;
}  DLRLOid ;

/*===== "LongSeq" already defined =====*/

typedef  char  *ClassName ;

typedef  char  *CacheName ;

typedef  char  *RelationName ;

typedef  _valuetype_  ObjectRoot ;	/* "valuetype" */

typedef  SEQUENCE (ObjectRoot, ObjectRootSeq) ;

/* Forward references for needed types. */

typedef  IOR  ObjectHome ;	/* "local interface" */
typedef  IOR  ObjectListener ;	/* "local interface" */
typedef  IOR  Selection ;	/* "local interface" */
typedef  IOR  CacheBase ;	/* "local interface" */
typedef  IOR  CacheAccess ;	/* "local interface" */
typedef  IOR  CacheListener ;	/* "local interface" */
typedef  IOR  Cache ;	/* "local interface" */
typedef  IOR  Contract ;	/* "local interface" */

typedef  SEQUENCE (ObjectHome, ObjectHomeSeq) ;

typedef  SEQUENCE (ObjectListener, ObjectListenerSeq) ;

typedef  SEQUENCE (Selection, SelectionSeq) ;

typedef  SEQUENCE (CacheBase, CacheBaseSeq) ;

typedef  SEQUENCE (CacheAccess, CacheAccessSeq) ;

typedef  SEQUENCE (CacheListener, CacheListenerSeq) ;

typedef  SEQUENCE (Contract, ContractSeq) ;

typedef  enum  RelationKind {
    REF_RELATION,
    LIST_RELATION,
    INT_MAP_RELATION,
    STR_MAP_RELATION
}  RelationKind ;

typedef  _valuetype_  RelationDescription ;	/* "valuetype" */
typedef  _valuetype_  ListRelationDescription ;	/* "valuetype" */
typedef  _valuetype_  IntMapRelationDescription ;	/* "valuetype" */
typedef  _valuetype_  StrMapRelationDescription ;	/* "valuetype" */

typedef  SEQUENCE (RelationDescription, RelationDescriptionSeq) ;

typedef  short  RelatedObjectDepth ;

#define  UNLIMITED_RELATED_OBJECTS  (- 1)

typedef  enum  CriterionKind {
    QUERY,
    FILTER
}  CriterionKind ;

typedef  enum  MembershipState {
    UNDEFINED_MEMBERSHIP,
    ALREADY_MEMBER,
    NOT_MEMBER
}  MembershipState ;

typedef  enum  CacheKind {
    CACHE_KIND,
    CACHEACCESS_KIND
}  CacheKind ;

typedef  _valuetype_  CacheDescription ;	/* "valuetype" */

/*******************************************************************************
    Tables for mapping enumerated values to names and vice-versa;
    see the coliToName() and coliToNumber() functions.
*******************************************************************************/

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  CacheKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  CacheUsageLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  CriterionKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  DCPSStateLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  DestinationOrderQosPolicyKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  DurabilityQosPolicyKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  HistoryQosPolicyKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  LivelinessQosPolicyKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  MembershipStateLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  ObjectScopeLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  ObjectStateLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  OwnershipQosPolicyKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  PresentationQosPolicyAccessScopeKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  ReferenceScopeLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  RelationKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  ReliabilityQosPolicyKindLUT[]  OCD ("DDS") ;
#endif
#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  const  ColiMap  SampleRejectedStatusKindLUT[]  OCD ("DDS") ;
#endif

/*******************************************************************************
    Public functions.
*******************************************************************************/

/* Marshaling functions for the defined data types. */

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxBuiltinTopicKey_t P_((ComxChannel channel,
                                               BuiltinTopicKey_t *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCache(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCacheAccess(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxCacheAccessSeq P_((ComxChannel channel,
                                            CacheAccessSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCacheBase(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxCacheBaseSeq P_((ComxChannel channel,
                                          CacheBaseSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCacheDescription(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxCacheKind P_((ComxChannel channel,
                                       CacheKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCacheListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxCacheListenerSeq P_((ComxChannel channel,
                                              CacheListenerSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCacheName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxCacheUsage P_((ComxChannel channel,
                                        CacheUsage *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxClassName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxCondition(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxConditionSeq P_((ComxChannel channel,
                                          ConditionSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxContentFilteredTopic(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxContract(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxContractSeq P_((ComxChannel channel,
                                         ContractSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxCriterionKind P_((ComxChannel channel,
                                           CriterionKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDataReader(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDataReaderListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDataReaderQos P_((ComxChannel channel,
                                           DataReaderQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDataReaderSeq P_((ComxChannel channel,
                                           DataReaderSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDataWriter(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDataWriterListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDataWriterQos P_((ComxChannel channel,
                                           DataWriterQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDCPSState P_((ComxChannel channel,
                                       DCPSState *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDeadlineQosPolicy P_((ComxChannel channel,
                                               DeadlineQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDestinationOrderQosPolicy P_((ComxChannel channel,
                                                       DestinationOrderQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDestinationOrderQosPolicyKind P_((ComxChannel channel,
                                                           DestinationOrderQosPolicyKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDLRLOid P_((ComxChannel channel,
                                     DLRLOid *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDomainId_t(channel,value)  comxLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDomainParticipant(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDomainParticipantFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDomainParticipantFactoryQos P_((ComxChannel channel,
                                                         DomainParticipantFactoryQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxDomainParticipantListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDomainParticipantQos P_((ComxChannel channel,
                                                  DomainParticipantQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDurabilityQosPolicy P_((ComxChannel channel,
                                                 DurabilityQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDurabilityQosPolicyKind P_((ComxChannel channel,
                                                     DurabilityQosPolicyKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDurabilityServiceQosPolicy P_((ComxChannel channel,
                                                        DurabilityServiceQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxDuration_t P_((ComxChannel channel,
                                        Duration_t *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxEntity(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxEntityFactoryQosPolicy P_((ComxChannel channel,
                                                    EntityFactoryQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxGroupDataQosPolicy P_((ComxChannel channel,
                                                GroupDataQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxGuardCondition(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxHistoryQosPolicy P_((ComxChannel channel,
                                              HistoryQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxHistoryQosPolicyKind P_((ComxChannel channel,
                                                  HistoryQosPolicyKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxInconsistentTopicStatus P_((ComxChannel channel,
                                                     InconsistentTopicStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxInstanceHandleSeq P_((ComxChannel channel,
                                               InstanceHandleSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxInstanceHandle_t(channel,value)  comxLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxInstanceStateKind(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxInstanceStateMask(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxIntMapRelationDescription(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxLatencyBudgetQosPolicy P_((ComxChannel channel,
                                                    LatencyBudgetQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxLifespanQosPolicy P_((ComxChannel channel,
                                               LifespanQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxListRelationDescription(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxLivelinessChangedStatus P_((ComxChannel channel,
                                                     LivelinessChangedStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxLivelinessLostStatus P_((ComxChannel channel,
                                                  LivelinessLostStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxLivelinessQosPolicy P_((ComxChannel channel,
                                                 LivelinessQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxLivelinessQosPolicyKind P_((ComxChannel channel,
                                                     LivelinessQosPolicyKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxMembershipState P_((ComxChannel channel,
                                             MembershipState *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxMultiTopic(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxObjectHome(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxObjectHomeSeq P_((ComxChannel channel,
                                           ObjectHomeSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxObjectListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxObjectListenerSeq P_((ComxChannel channel,
                                               ObjectListenerSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxObjectRoot(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxObjectRootSeq P_((ComxChannel channel,
                                           ObjectRootSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxObjectScope P_((ComxChannel channel,
                                         ObjectScope *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxObjectState P_((ComxChannel channel,
                                         ObjectState *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxOfferedDeadlineMissedStatus P_((ComxChannel channel,
                                                         OfferedDeadlineMissedStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxOfferedIncompatibleQosStatus P_((ComxChannel channel,
                                                          OfferedIncompatibleQosStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxOwnershipQosPolicy P_((ComxChannel channel,
                                                OwnershipQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxOwnershipQosPolicyKind P_((ComxChannel channel,
                                                    OwnershipQosPolicyKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxOwnershipStrengthQosPolicy P_((ComxChannel channel,
                                                        OwnershipStrengthQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxParticipantBuiltinTopicData P_((ComxChannel channel,
                                                         ParticipantBuiltinTopicData *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxPartitionQosPolicy P_((ComxChannel channel,
                                                PartitionQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxPresentationQosPolicy P_((ComxChannel channel,
                                                   PresentationQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxPresentationQosPolicyAccessScopeKind P_((ComxChannel channel,
                                                                  PresentationQosPolicyAccessScopeKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxPublicationBuiltinTopicData P_((ComxChannel channel,
                                                         PublicationBuiltinTopicData *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxPublicationMatchedStatus P_((ComxChannel channel,
                                                      PublicationMatchedStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxPublisher(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxPublisherListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxPublisherQos P_((ComxChannel channel,
                                          PublisherQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxQosPolicyCount P_((ComxChannel channel,
                                            QosPolicyCount *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxQosPolicyCountSeq P_((ComxChannel channel,
                                               QosPolicyCountSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxQosPolicyId_t(channel,value)  comxLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxQueryCondition(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxReadCondition(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxReaderDataLifecycleQosPolicy P_((ComxChannel channel,
                                                          ReaderDataLifecycleQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxReferenceScope P_((ComxChannel channel,
                                            ReferenceScope *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxRelatedObjectDepth(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxRelationDescription(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxRelationDescriptionSeq P_((ComxChannel channel,
                                                    RelationDescriptionSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxRelationKind P_((ComxChannel channel,
                                          RelationKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxRelationName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxReliabilityQosPolicy P_((ComxChannel channel,
                                                  ReliabilityQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxReliabilityQosPolicyKind P_((ComxChannel channel,
                                                      ReliabilityQosPolicyKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxRequestedDeadlineMissedStatus P_((ComxChannel channel,
                                                           RequestedDeadlineMissedStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxRequestedIncompatibleQosStatus P_((ComxChannel channel,
                                                            RequestedIncompatibleQosStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxResourceLimitsQosPolicy P_((ComxChannel channel,
                                                     ResourceLimitsQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxReturnCode_t(channel,value)  comxLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSampleInfo P_((ComxChannel channel,
                                        SampleInfo *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSampleInfoSeq P_((ComxChannel channel,
                                           SampleInfoSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSampleLostStatus P_((ComxChannel channel,
                                              SampleLostStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSampleRejectedStatus P_((ComxChannel channel,
                                                  SampleRejectedStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSampleRejectedStatusKind P_((ComxChannel channel,
                                                      SampleRejectedStatusKind *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxSampleStateKind(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxSampleStateMask(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxSelection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSelectionSeq P_((ComxChannel channel,
                                          SelectionSeq *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxStatusCondition(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxStatusKind(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxStatusMask(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxStrMapRelationDescription(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxSubscriber(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxSubscriberListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSubscriberQos P_((ComxChannel channel,
                                           SubscriberQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSubscriptionBuiltinTopicData P_((ComxChannel channel,
                                                          SubscriptionBuiltinTopicData *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxSubscriptionMatchedStatus P_((ComxChannel channel,
                                                       SubscriptionMatchedStatus *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxTimeBasedFilterQosPolicy P_((ComxChannel channel,
                                                      TimeBasedFilterQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxTime_t P_((ComxChannel channel,
                                    Time_t *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxTopic(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxTopicBuiltinTopicData P_((ComxChannel channel,
                                                   TopicBuiltinTopicData *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxTopicDataQosPolicy P_((ComxChannel channel,
                                                TopicDataQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxTopicDescription(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxTopicListener(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxTopicQos P_((ComxChannel channel,
                                      TopicQos *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxTransportPriorityQosPolicy P_((ComxChannel channel,
                                                        TransportPriorityQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxTypeSupport(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxUserDataQosPolicy P_((ComxChannel channel,
                                               UserDataQosPolicy *value))
        OCD ("DDS") ;
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxViewStateKind(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxViewStateMask(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
#    define  ddmxWaitSet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DDS) || IDL_MODULE_DDS
    extern  errno_t  ddmxWriterDataLifecycleQosPolicy P_((ComxChannel channel,
                                                          WriterDataLifecycleQosPolicy *value))
        OCD ("DDS") ;
#endif
