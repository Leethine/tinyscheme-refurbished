/* $Id: gimx_idl.h,v 1.16 2021/09/15 04:18:16 alex Exp $ */
/*******************************************************************************
    Declarations for IOR and IOR-related types.
*******************************************************************************/

/* Module: IOP */

typedef  unsigned  long  ComponentId ;

typedef  struct  TaggedComponent {
    ComponentId  tag ;
    OctetSeq  component_data ;
}  TaggedComponent ;

typedef  SEQUENCE (TaggedComponent, MultipleComponentProfile) ;

typedef  struct  ProfileBody {
    Version  iiop_version ;
    char  *host ;
    unsigned  short  port ;
    ObjectKey  object_key ;
    SEQUENCE (TaggedComponent, components) ;	/* Ignore in IIOP 1.0! */
}  ProfileBody ;

typedef  unsigned  long  ProfileId ;

typedef  struct  TaggedProfile {
    ProfileId  which ;
    union {
			/* TAG_INTERNET_IOP */
        ProfileBody  iiop_body ;
			/* TAG_MULTIPLE_COMPONENTS */
        MultipleComponentProfile  components ;
			/* <default> */
        OctetSeq  profile_data ;
    }  data ;
}  TaggedProfile ;

typedef  struct  IOR {
    char  *type_id ;
    SEQUENCE (TaggedProfile, profiles) ;
}  IOR ;

typedef  IOR  Object ;


/*******************************************************************************
    Declaration for valuetype (which is unsupported).
        "valuetype" is a keyword in CORBA IDL that is used for
            (i) declaring "valuetype" types,
            (ii) providing forward references to "valuetype" types, and
            (iii) as the type for fields, function arguments, and function
                  return values.
        For (iii), define a typedef for the "base" valuetype as an OctetSeq,
        which was chosen arbitrarily.
*******************************************************************************/

typedef  OctetSeq  _valuetype_ ;
typedef  _valuetype_  valuetype ;	/* "valuetype"s are not supported. */

/*******************************************************************************
    gimx_all.idl
*******************************************************************************/
/*  <stdin> : line 1  */
/*  <built-in> : line 1  */
/*  <command-line> : line 1  */
/*  <stdin> : line 1  */
/*  ../idl/CORBA_IDL/orb.idl : line 1  */

/* Module: CORBA */

#ifndef IDL_MODULE_CORBA
#    define  IDL_MODULE_CORBA  1
#endif

typedef  IOR  ConstructionPolicy ;
typedef  IOR  DomainManager ;
typedef  IOR  Policy ;
typedef  IOR  AbstractInterfaceDef ;
typedef  IOR  AliasDef ;
typedef  IOR  ArrayDef ;
typedef  IOR  AttributeDef ;
typedef  IOR  ConstantDef ;
typedef  IOR  Contained ;
typedef  IOR  Container ;
typedef  IOR  EnumDef ;
typedef  IOR  ExceptionDef ;
typedef  IOR  ExtInterfaceDef ;
typedef  IOR  ExtValueDef ;
typedef  IOR  ExtAbstractInterfaceDef ;
typedef  IOR  ExtLocalInterfaceDef ;
typedef  IOR  FixedDef ;
typedef  IOR  IDLType ;
typedef  IOR  InterfaceDef ;
typedef  IOR  IRObject ;
typedef  IOR  LocalInterfaceDef ;
typedef  IOR  ModuleDef ;
typedef  IOR  NativeDef ;
typedef  IOR  OperationDef ;
typedef  IOR  PrimitiveDef ;
typedef  IOR  Repository ;
typedef  IOR  SequenceDef ;
typedef  IOR  StringDef ;
typedef  IOR  StructDef ;
typedef  IOR  TypeCode ;
typedef  IOR  TypedefDef ;
typedef  IOR  UnionDef ;
typedef  IOR  ValueDef ;
typedef  IOR  ValueBoxDef ;
typedef  IOR  ValueMemberDef ;
typedef  IOR  WstringDef ;

typedef  char  *Identifier ;
/*  ../idl/CORBA_IDL/CORBA_StandardExceptions.idl : line 1  */

#define  CORBA_OMGVMCID  (0x4f4d0000UL)

typedef  enum  EXCEPTcompletion_status {
    COMPLETED_YES,
    COMPLETED_NO,
    COMPLETED_MAYBE
}  EXCEPTcompletion_status ;

typedef  enum  exception_type {
    EXCEPT_NO_EXCEPTION,
    EXCEPT_USER_EXCEPTION,
    EXCEPT_SYSTEM_EXCEPTION
}  exception_type ;
/*  ../idl/CORBA_IDL/CORBA_Current.idl : line 1  */
/*  ../idl/CORBA_IDL/CORBA_Policy.idl : line 1  */

typedef  unsigned  long  PolicyType ;

typedef  SEQUENCE (Policy, PolicyList) ;

typedef  SEQUENCE (PolicyType, PolicyTypeSeq) ;

typedef  short  PolicyErrorCode ;

#define  CORBA_BAD_POLICY  (0)
#define  CORBA_UNSUPPORTED_POLICY  (1)
#define  CORBA_BAD_POLICY_TYPE  (2)
#define  CORBA_BAD_POLICY_VALUE  (3)
#define  CORBA_UNSUPPORTED_POLICY_VALUE  (4)
/*  ../idl/CORBA_IDL/CORBA_DomainManager.idl : line 1  */
#define  CORBA_SecConstruction  (11)

typedef  SEQUENCE (DomainManager, DomainManagersList) ;
/*  ../idl/CORBA_IDL/CORBA_Pollable.idl : line 1  */
/*  ../idl/CORBA_IDL/CORBA_InterfaceRepository.idl : line 1  */

typedef  char  *ScopedName ;

typedef  char  *RepositoryId ;

typedef  enum  TCKind {
    tk_null,
    tk_void,
    tk_short,
    tk_long,
    tk_ushort,
    tk_ulong,
    tk_float,
    tk_double,
    tk_boolean,
    tk_char,
    tk_octet,
    tk_any,
    tk_TypeCode,
    tk_Principal,
    tk_objref,
    tk_struct,
    tk_union,
    tk_enum,
    tk_string,
    tk_sequence,
    tk_array,
    tk_alias,
    tk_except,
    tk_longlong,
    tk_ulonglong,
    tk_longdouble,
    tk_wchar,
    tk_wstring,
    tk_fixed,
    tk_value,
    tk_value_box,
    tk_native,
    tk_abstract_interface,
    tk_local_interface,
    tk_component,
    tk_home,
    tk_event
}  TCKind ;

typedef  struct  Any {
    TCKind  which ;
    union {
			/* tk_null (Implemented) */
			/* tk_void (Implemented) */
			/* tk_short */
        short  vShort ;
			/* tk_long */
        long  vLong ;
			/* tk_ushort */
        unsigned  short  vUShort ;
			/* tk_ulong */
        unsigned  long  vULong ;
			/* tk_float */
        float  vFloat ;
			/* tk_double */
        double  vDouble ;
			/* tk_boolean */
        bool  vBoolean ;
			/* tk_char */
        char  vChar ;
			/* tk_octet */
        octet  vOctet ;
			/* tk_any (Not Implemented) */
			/* tk_TypeCode */
        TCKind  vTypeCode ;
			/* tk_Principal */
        OctetSeq  vPrincipal ;
			/* tk_objref (N/I) */
			/* tk_struct (N/I) */
			/* tk_union (N/I) */
			/* tk_enum (N/I) */
			/* tk_string */
        char  *vString ;
			/* tk_sequence (N/I) */
			/* tk_array (N/I) */
			/* tk_alias (N/I) */
			/* tk_except (N/I) */
			/* tk_longlong */
        LONGLONG  vLongLong ;
			/* tk_ulonglong */
        ULONGLONG  vULongLong ;
			/* tk_longdouble */
        LONGDOUBLE  vLongDouble ;
			/* tk_wchar */
        wchar_t  vWChar ;
			/* tk_wstring */
        wchar_t  *vWString ;
			/* tk_fixed (N/I) */
			/* tk_value (N/I) */
			/* tk_value_box (N/I) */
			/* tk_native (N/I) */
			/* tk_abstract_interface (N/I) */
			/* tk_local_interface (N/I) */
			/* tk_component (N/I) */
			/* tk_home (N/I) */
			/* tk_event (N/I) */
			/* 0xFFFFFFFF */
        long  vIndirection ;
    }  data ;
}  Any ;

typedef  SEQUENCE (Any, AnySeq) ;

typedef  enum  DefinitionKind {
    dk_none,
    dk_all,
    dk_Attribute,
    dk_Constant,
    dk_Exception,
    dk_Interface,
    dk_Module,
    dk_Operation,
    dk_Typedef,
    dk_Alias,
    dk_Struct,
    dk_Union,
    dk_Enum,
    dk_Primitive,
    dk_String,
    dk_Sequence,
    dk_Array,
    dk_Repository,
    dk_Wstring,
    dk_Fixed,
    dk_Value,
    dk_ValueBox,
    dk_ValueMember,
    dk_Native,
    dk_AbstractInterface,
    dk_LocalInterface,
    dk_Component,
    dk_Home,
    dk_Factory,
    dk_Finder,
    dk_Emits,
    dk_Publishes,
    dk_Consumes,
    dk_Provides,
    dk_Uses,
    dk_Event
}  DefinitionKind ;

typedef  char  *VersionSpec ;

typedef  struct  Description {
    DefinitionKind  kind ;
    Any  value ;
}  Description ;

typedef  SEQUENCE (InterfaceDef, InterfaceDefSeq) ;

typedef  SEQUENCE (ValueDef, ValueDefSeq) ;

typedef  SEQUENCE (AbstractInterfaceDef, AbstractInterfaceDefSeq) ;

typedef  SEQUENCE (LocalInterfaceDef, LocalInterfaceDefSeq) ;

typedef  SEQUENCE (ExtInterfaceDef, ExtInterfaceDefSeq) ;

typedef  SEQUENCE (ExtValueDef, ExtValueDefSeq) ;

typedef  SEQUENCE (ExtAbstractInterfaceDef, ExtAbstractInterfaceDefSeq) ;

typedef  SEQUENCE (ExtLocalInterfaceDef, ExtLocalInterfaceDefSeq) ;

typedef  SEQUENCE (Contained, ContainedSeq) ;

typedef  struct  StructMember {
    Identifier  name ;
    TypeCode  type ;
    IDLType  type_def ;
}  StructMember ;

typedef  SEQUENCE (StructMember, StructMemberSeq) ;

typedef  struct  Initializer {
    StructMemberSeq  members ;
    Identifier  name ;
}  Initializer ;

typedef  SEQUENCE (Initializer, InitializerSeq) ;

typedef  struct  UnionMember {
    Identifier  name ;
    Any  label ;
    TypeCode  type ;
    IDLType  type_def ;
}  UnionMember ;

typedef  struct  ExceptionDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  type ;
}  ExceptionDescription ;

typedef  SEQUENCE (ExceptionDescription, ExcDescriptionSeq) ;

typedef  struct  ExtInitializer {
    StructMemberSeq  members ;
    ExcDescriptionSeq  exceptions ;
    Identifier  name ;
}  ExtInitializer ;

typedef  SEQUENCE (ExtInitializer, ExtInitializerSeq) ;

typedef  SEQUENCE (UnionMember, UnionMemberSeq) ;

typedef  SEQUENCE (Identifier, EnumMemberSeq) ;

/*===== "Description" already defined =====*/

typedef  SEQUENCE (Description, DescriptionSeq) ;

typedef  enum  PrimitiveKind {
    pk_null,
    pk_void,
    pk_short,
    pk_long,
    pk_ushort,
    pk_ulong,
    pk_float,
    pk_double,
    pk_boolean,
    pk_char,
    pk_octet,
    pk_any,
    pk_TypeCode,
    pk_Principal,
    pk_string,
    pk_objref,
    pk_longlong,
    pk_ulonglong,
    pk_longdouble,
    pk_wchar,
    pk_wstring,
    pk_value_base
}  PrimitiveKind ;

typedef  struct  ModuleDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
}  ModuleDescription ;

typedef  struct  ConstantDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  type ;
    Any  value ;
}  ConstantDescription ;

typedef  struct  TypeDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  type ;
}  TypeDescription ;

typedef  enum  AttributeMode {
    ATTR_NORMAL,
    ATTR_READONLY
}  AttributeMode ;

typedef  struct  AttributeDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  type ;
    AttributeMode  mode ;
}  AttributeDescription ;

typedef  struct  ExtAttributeDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  type ;
    AttributeMode  mode ;
    ExcDescriptionSeq  get_exceptions ;
    ExcDescriptionSeq  put_exceptions ;
}  ExtAttributeDescription ;

typedef  IOR  ExtAttributeDef ;

typedef  enum  OperationMode {
    OP_NORMAL,
    OP_ONEWAY
}  OperationMode ;

typedef  enum  ParameterMode {
    PARAM_IN,
    PARAM_OUT,
    PARAM_INOUT
}  ParameterMode ;

typedef  struct  ParameterDescription {
    Identifier  name ;
    TypeCode  type ;
    IDLType  type_def ;
    ParameterMode  mode ;
}  ParameterDescription ;

typedef  SEQUENCE (ParameterDescription, ParDescriptionSeq) ;

typedef  char  *ContextIdentifier ;

typedef  SEQUENCE (ContextIdentifier, ContextIdSeq) ;

typedef  SEQUENCE (ExceptionDef, ExceptionDefSeq) ;

typedef  struct  OperationDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  result ;
    OperationMode  mode ;
    ContextIdSeq  contexts ;
    ParDescriptionSeq  parameters ;
    ExcDescriptionSeq  exceptions ;
}  OperationDescription ;

typedef  SEQUENCE (RepositoryId, RepositoryIdSeq) ;

typedef  SEQUENCE (OperationDescription, OpDescriptionSeq) ;

typedef  SEQUENCE (AttributeDescription, AttrDescriptionSeq) ;

typedef  SEQUENCE (ExtAttributeDescription, ExtAttrDescriptionSeq) ;

typedef  struct  FullInterfaceDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    OpDescriptionSeq  operations ;
    AttrDescriptionSeq  attributes ;
    RepositoryIdSeq  base_interfaces ;
    TypeCode  type ;
    bool  is_abstract ;
}  FullInterfaceDescription ;

typedef  struct  InterfaceDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryIdSeq  base_interfaces ;
    bool  is_abstract ;
}  InterfaceDescription ;

typedef  IOR  InterfaceAttrExtension ;

typedef  struct  ExtFullInterfaceDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    OpDescriptionSeq  operations ;
    ExtAttrDescriptionSeq  attributes ;
    RepositoryIdSeq  base_interfaces ;
    TypeCode  type ;
}  ExtFullInterfaceDescription ;

typedef  short  Visibility ;

#define  CORBA_PRIVATE_MEMBER  (0)
#define  CORBA_PUBLIC_MEMBER  (1)

typedef  struct  ValueMember {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    TypeCode  type ;
    IDLType  type_def ;
    Visibility  access ;
}  ValueMember ;

typedef  SEQUENCE (ValueMember, ValueMemberSeq) ;

typedef  struct  FullValueDescription {
    Identifier  name ;
    RepositoryId  id ;
    bool  is_abstract ;
    bool  is_custom ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    OpDescriptionSeq  operations ;
    AttrDescriptionSeq  attributes ;
    ValueMemberSeq  members ;
    InitializerSeq  initializers ;
    RepositoryIdSeq  supported_interfaces ;
    RepositoryIdSeq  abstract_base_values ;
    bool  is_truncatable ;
    RepositoryId  base_value ;
    TypeCode  type ;
}  FullValueDescription ;

typedef  struct  ValueDescription {
    Identifier  name ;
    RepositoryId  id ;
    bool  is_abstract ;
    bool  is_custom ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryIdSeq  supported_interfaces ;
    RepositoryIdSeq  abstract_base_values ;
    bool  is_truncatable ;
    RepositoryId  base_value ;
}  ValueDescription ;

typedef  struct  ExtFullValueDescription {
    Identifier  name ;
    RepositoryId  id ;
    bool  is_abstract ;
    bool  is_custom ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    OpDescriptionSeq  operations ;
    ExtAttrDescriptionSeq  attributes ;
    ValueMemberSeq  members ;
    ExtInitializerSeq  initializers ;
    RepositoryIdSeq  supported_interfaces ;
    RepositoryIdSeq  abstract_base_values ;
    bool  is_truncatable ;
    RepositoryId  base_value ;
    TypeCode  type ;
}  ExtFullValueDescription ;

/* Module: CORBA::ComponentIR */

#ifndef IDL_MODULE_CORBA__ComponentIR
#    define  IDL_MODULE_CORBA__ComponentIR  1
#endif

typedef  IOR  ComponentDef ;
typedef  IOR  HomeDef ;
typedef  IOR  EventDef ;
typedef  IOR  ProvidesDef ;

typedef  struct  ProvidesDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryId  interface_type ;
}  ProvidesDescription ;

typedef  IOR  UsesDef ;

typedef  struct  UsesDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryId  interface_type ;
    bool  is_multiple ;
}  UsesDescription ;

typedef  IOR  EventPortDef ;

typedef  struct  EventPortDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryId  event ;
}  EventPortDescription ;

typedef  IOR  EmitsDef ;
typedef  IOR  PublishesDef ;
typedef  IOR  ConsumesDef ;

typedef  SEQUENCE (ProvidesDescription, ProvidesDescriptionSeq) ;

typedef  SEQUENCE (UsesDescription, UsesDescriptionSeq) ;

typedef  SEQUENCE (EventPortDescription, EventPortDescriptionSeq) ;

typedef  struct  ComponentDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryId  base_component ;
    RepositoryIdSeq  supported_interfaces ;
    ProvidesDescriptionSeq  provided_interfaces ;
    UsesDescriptionSeq  used_interfaces ;
    EventPortDescriptionSeq  emits_events ;
    EventPortDescriptionSeq  publishes_events ;
    EventPortDescriptionSeq  consumes_events ;
    ExtAttrDescriptionSeq  attributes ;
    TypeCode  type ;
}  ComponentDescription ;

typedef  IOR  FactoryDef ;
typedef  IOR  FinderDef ;

typedef  struct  HomeDescription {
    Identifier  name ;
    RepositoryId  id ;
    RepositoryId  defined_in ;
    VersionSpec  version ;
    RepositoryId  base_home ;
    RepositoryId  managed_component ;
    ValueDescription  primary_key ;
    OpDescriptionSeq  factories ;
    OpDescriptionSeq  finders ;
    OpDescriptionSeq  operations ;
    ExtAttrDescriptionSeq  attributes ;
    TypeCode  type ;
}  HomeDescription ;
/*  ../idl/CORBA_IDL/CORBA_TypeCode.idl : line 1  */

typedef  short  ValueModifier ;

#define  CORBA_VM_NONE  (0)
#define  CORBA_VM_CUSTOM  (1)
#define  CORBA_VM_ABSTRACT  (2)
#define  CORBA_VM_TRUNCATABLE  (3)
/*  ../idl/CORBA_IDL/CORBA_CustomMarshal.idl : line 1  */
/*  ../idl/CORBA_IDL/CORBA_Stream.idl : line 1  */

/*===== "AnySeq" already defined =====*/

/*===== "BooleanSeq" already defined =====*/

/*===== "CharSeq" already defined =====*/

/*===== "WCharSeq" already defined =====*/

/*===== "OctetSeq" already defined =====*/

/*===== "ShortSeq" already defined =====*/

/*===== "UShortSeq" already defined =====*/

/*===== "LongSeq" already defined =====*/

/*===== "ULongSeq" already defined =====*/

/*===== "LongLongSeq" already defined =====*/

/*===== "ULongLongSeq" already defined =====*/

/*===== "FloatSeq" already defined =====*/

/*===== "DoubleSeq" already defined =====*/

/*===== "LongDoubleSeq" already defined =====*/

/*===== "StringSeq" already defined =====*/

/*===== "WStringSeq" already defined =====*/
/*  ../idl/Interop/BiDirPolicy.idl : line 1  */
/*  ../idl/CORBA_IDL/orb.idl : line 1  */

/* Module: BiDirPolicy */

#ifndef IDL_MODULE_BiDirPolicy
#    define  IDL_MODULE_BiDirPolicy  1
#endif

typedef  unsigned  short  BidirectionalPolicyValue ;

#define  NORMAL  (0)
#define  BOTH  (1)
#define  BIDIRECTIONAL_POLICY_TYPE  (37)

typedef  IOR  BidirectionalPolicy ;
/*  ../idl/Interop/CONV_FRAME.idl : line 1  */

/* Module: CONV_FRAME */

#ifndef IDL_MODULE_CONV_FRAME
#    define  IDL_MODULE_CONV_FRAME  1
#endif

typedef  unsigned  long  CodeSetId ;

typedef  struct  CodeSetComponent {
    CodeSetId  native_code_set ;
    SEQUENCE (CodeSetId, conversion_code_sets) ;
}  CodeSetComponent ;

typedef  struct  CodeSetComponentInfo {
    CodeSetComponent  ForCharData ;
    CodeSetComponent  ForWcharData ;
}  CodeSetComponentInfo ;

typedef  struct  CodeSetContext {
    CodeSetId  char_data ;
    CodeSetId  wchar_data ;
}  CodeSetContext ;
/*  ../idl/Interop/CSIIOP.idl : line 1  */
/*  ../idl/Interop/IOP.idl : line 1  */

/* Module: IOP */

#ifndef IDL_MODULE_IOP
#    define  IDL_MODULE_IOP  1
#endif

#define  IOP_TAG_INTERNET_IOP  (0)
#define  IOP_TAG_MULTIPLE_COMPONENTS  (1)
#define  IOP_TAG_SCCP_IOP  (2)

#define  IOP_TAG_ORB_TYPE  (0)
#define  IOP_TAG_CODE_SETS  (1)
#define  IOP_TAG_POLICIES  (2)
#define  IOP_TAG_ALTERNATE_IIOP_ADDRESS  (3)
#define  IOP_TAG_ASSOCIATION_OPTIONS  (13)
#define  IOP_TAG_SEC_NAME  (14)
#define  IOP_TAG_SPKM_1_SEC_MECH  (15)
#define  IOP_TAG_SPKM_2_SEC_MECH  (16)
#define  IOP_TAG_KerberosV5_SEC_MECH  (17)
#define  IOP_TAG_CSI_ECMA_Secret_SEC_MECH  (18)
#define  IOP_TAG_CSI_ECMA_Hybrid_SEC_MECH  (19)
#define  IOP_TAG_SSL_SEC_TRANS  (20)
#define  IOP_TAG_CSI_ECMA_Public_SEC_MECH  (21)
#define  IOP_TAG_GENERIC_SEC_MECH  (22)
#define  IOP_TAG_FIREWALL_TRANS  (23)
#define  IOP_TAG_SCCP_CONTACT_INFO  (24)
#define  IOP_TAG_JAVA_CODEBASE  (25)
#define  IOP_TAG_TRANSACTION_POLICY  (26)
#define  IOP_TAG_MESSAGE_ROUTER  (30)
#define  IOP_TAG_OTS_POLICY  (31)
#define  IOP_TAG_INV_POLICY  (32)
#define  IOP_TAG_CSI_SEC_MECH_LIST  (33)
#define  IOP_TAG_NULL_TAG  (34)
#define  IOP_TAG_SECIOP_SEC_TRANS  (35)
#define  IOP_TAG_TLS_SEC_TRANS  (36)
#define  IOP_TAG_ACTIVITY_POLICY  (37)
#define  IOP_TAG_COMPLETE_OBJECT_KEY  (5)
#define  IOP_TAG_ENDPOINT_ID_POSITION  (6)
#define  IOP_TAG_LOCATION_POLICY  (12)
#define  IOP_TAG_DCE_STRING_BINDING  (100)
#define  IOP_TAG_DCE_BINDING_NAME  (101)
#define  IOP_TAG_DCE_NO_PIPES  (102)
#define  IOP_TAG_DCE_SEC_MECH  (103)
#define  IOP_TAG_INET_SEC_TRANS  (123)

typedef  unsigned  long  ServiceId ;

typedef  struct  ServiceContext {
    ServiceId  context_id ;
    OctetSeq  context_data ;
}  ServiceContext ;

typedef  SEQUENCE (ServiceContext, ServiceContextList) ;

#define  IOP_TransactionService  (0)
#define  IOP_CodeSets  (1)
#define  IOP_ChainBypassCheck  (2)
#define  IOP_ChainBypassInfo  (3)
#define  IOP_LogicalThreadId  (4)
#define  IOP_BI_DIR_IIOP  (5)
#define  IOP_SendingContextRunTime  (6)
#define  IOP_INVOCATION_POLICIES  (7)
#define  IOP_FORWARDED_IDENTITY  (8)
#define  IOP_UnknownExceptionInfo  (9)
#define  IOP_RTCorbaPriority  (10)
#define  IOP_RTCorbaPriorityRange  (11)
#define  IOP_FT_GROUP_VERSION  (12)
#define  IOP_FT_REQUEST  (13)
#define  IOP_ExceptionDetailMessage  (14)
#define  IOP_SecurityAttributeService  (15)
#define  IOP_ActivityService  (16)

typedef  short  EncodingFormat ;

#define  IOP_ENCODING_CDR_ENCAPS  (0)

typedef  struct  Encoding {
    EncodingFormat  format ;
    octet  major_version ;
    octet  minor_version ;
}  Encoding ;
/*  ../idl/Interop/CSI.idl : line 1  */

/* Module: CSI */

#ifndef IDL_MODULE_CSI
#    define  IDL_MODULE_CSI  1
#endif

#define  CSI_OMGVMCID  (0x4F4D0UL)

typedef  OctetSeq  X509CertificateChain ;

typedef  OctetSeq  X501DistinguishedName ;

typedef  OctetSeq  UTF8String ;

typedef  OctetSeq  OID ;

typedef  SEQUENCE (OID, OIDList) ;

typedef  OctetSeq  GSSToken ;

typedef  OctetSeq  GSS_NT_ExportedName ;

typedef  SEQUENCE (GSS_NT_ExportedName, GSS_NT_ExportedNameList) ;

typedef  short  MsgType ;

#define  MTEstablishContext  (0)
#define  MTCompleteEstablishContext  (1)
#define  MTContextError  (4)
#define  MTMessageInContext  (5)

typedef  ULONGLONG  ContextId ;

typedef  unsigned  long  AuthorizationElementType ;

#define  X509AttributeCertChain  (OMGVMCID | 1)

typedef  OctetSeq  AuthorizationElementContents ;

typedef  struct  AuthorizationElement {
    AuthorizationElementType  the_type ;
    AuthorizationElementContents  the_element ;
}  AuthorizationElement ;

typedef  SEQUENCE (AuthorizationElement, AuthorizationToken) ;

typedef  unsigned  long  IdentityTokenType ;

#define  ITTAbsent  (0)
#define  ITTAnonymous  (1)
#define  ITTPrincipalName  (2)
#define  ITTX509CertChain  (4)
#define  ITTDistinguishedName  (8)

typedef  OctetSeq  IdentityExtension ;

typedef  struct  IdentityToken {
    IdentityTokenType  which ;
    union {
			/* ITTAbsent */
        bool  absent ;
			/* ITTAnonymous */
        bool  anonymous ;
			/* ITTPrincipalName */
        GSS_NT_ExportedName  principal_name ;
			/* ITTX509CertChain */
        X509CertificateChain  certificate_chain ;
			/* ITTDistinguishedName */
        X501DistinguishedName  dn ;
			/* <default> */
        IdentityExtension  id ;
    }  data ;
}  IdentityToken ;

typedef  struct  EstablishContext {
    ContextId  client_context_id ;
    AuthorizationToken  authorization_token ;
    IdentityToken  identity_token ;
    GSSToken  client_authentication_token ;
}  EstablishContext ;

typedef  struct  CompleteEstablishContext {
    ContextId  client_context_id ;
    bool  context_stateful ;
    GSSToken  final_context_token ;
}  CompleteEstablishContext ;

typedef  struct  ContextError {
    ContextId  client_context_id ;
    long  major_status ;
    long  minor_status ;
    GSSToken  error_token ;
}  ContextError ;

typedef  struct  MessageInContext {
    ContextId  client_context_id ;
    bool  discard_context ;
}  MessageInContext ;

typedef  struct  SASContextBody {
    MsgType  which ;
    union {
			/* MTEstablishContext */
        EstablishContext  establish_msg ;
			/* MTCompleteEstablishContext */
        CompleteEstablishContext  complete_msg ;
			/* MTContextError */
        ContextError  error_msg ;
			/* MTMessageInContext */
        MessageInContext  in_context_msg ;
    }  data ;
}  SASContextBody ;

typedef  char  *StringOID ;

#define  KRB5MechOID  ("oid:1.2.840.113554.1.2.2")
#define  GSS_NT_Export_Name_OID  ("oid:1.3.6.1.5.6.4")
#define  GSS_NT_Scoped_Username_OID  ("oid:2.23.130.1.2.1")

/* Module: CSIIOP */

#ifndef IDL_MODULE_CSIIOP
#    define  IDL_MODULE_CSIIOP  1
#endif

typedef  unsigned  short  AssociationOptions ;

#define  NoProtection  (1)
#define  Integrity  (2)
#define  Confidentiality  (4)
#define  DetectReplay  (8)
#define  DetectMisordering  (16)
#define  EstablishTrustInTarget  (32)
#define  EstablishTrustInClient  (64)
#define  NoDelegation  (128)
#define  SimpleDelegation  (256)
#define  CompositeDelegation  (512)
#define  IdentityAssertion  (1024)
#define  DelegationByClient  (2048)

typedef  unsigned  long  ServiceConfigurationSyntax ;

#define  SCS_GeneralNames  (OMGVMCID | 0)
#define  SCS_GSSExportedName  (OMGVMCID | 1)

typedef  OctetSeq  ServiceSpecificName ;

typedef  struct  ServiceConfiguration {
    ServiceConfigurationSyntax  syntax ;
    ServiceSpecificName  name ;
}  ServiceConfiguration ;

typedef  SEQUENCE (ServiceConfiguration, ServiceConfigurationList) ;

typedef  struct  AS_ContextSec {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    OID  client_authentication_mech ;
    GSS_NT_ExportedName  target_name ;
}  AS_ContextSec ;

typedef  struct  SAS_ContextSec {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    ServiceConfigurationList  privilege_authorities ;
    OIDList  supported_naming_mechanisms ;
    IdentityTokenType  supported_identity_types ;
}  SAS_ContextSec ;

typedef  struct  CompoundSecMech {
    AssociationOptions  target_requires ;
    TaggedComponent  transport_mech ;
    AS_ContextSec  as_context_mech ;
    SAS_ContextSec  sas_context_mech ;
}  CompoundSecMech ;

typedef  SEQUENCE (CompoundSecMech, CompoundSecMechanisms) ;

typedef  struct  CompoundSecMechList {
    bool  stateful ;
    CompoundSecMechanisms  mechanism_list ;
}  CompoundSecMechList ;

typedef  struct  TransportAddress {
    char  *host_name ;
    unsigned  short  port ;
}  TransportAddress ;

typedef  SEQUENCE (TransportAddress, TransportAddressList) ;

/*===== CSIIOP::TAG_SECIOP_SEC_TRANS previously defined as IOP_TAG_SECIOP_SEC_TRANS in IOP =====*/

typedef  struct  SECIOP_SEC_TRANS {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    OID  mech_oid ;
    GSS_NT_ExportedName  target_name ;
    TransportAddressList  addresses ;
}  SECIOP_SEC_TRANS ;

/*===== CSIIOP::TAG_TLS_SEC_TRANS previously defined as IOP_TAG_TLS_SEC_TRANS in IOP =====*/

typedef  struct  TLS_SEC_TRANS {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    TransportAddressList  addresses ;
}  TLS_SEC_TRANS ;
/*  ../idl/Interop/GIOP.idl : line 1  */

/* Module: GIOP */

#ifndef IDL_MODULE_GIOP
#    define  IDL_MODULE_GIOP  1
#endif

/* GIOP MessageHeader flags. */

#define  ENDIAN_MASK  0x01		/* 0 = big-endian, 1 = little-endian */
#define  FRAGMENT_MASK  0x02		/* 0 = last fragment, 1 = more fragments */

/*******************************************************************************
    Define generic enumerated types for GIOP version-specific enumerations.
    These need to be updated in "gimx_pre.py" when new CORBA versions are
    issued.
*******************************************************************************/

typedef  enum  LocateStatusType_1_2 {
    UNKNOWN_OBJECT,
    OBJECT_HERE,
    OBJECT_FORWARD,			/* GIOP 1.0/1.1 stop here. */
    OBJECT_FORWARD_PERM,
    LOC_SYSTEM_EXCEPTION,
    LOC_NEEDS_ADDRESSING_MODE
}  LocateStatusType ;

typedef  enum  MsgType_1_1 {
    Request = 0,
    Reply,
    CancelRequest,
    LocateRequest,
    LocateReply,
    CloseConnection,
    MessageError,			/* GIOP 1.0 stops here. */
    Fragment
}  GIOPMsgType ;			/* MsgType would conflict with CSI.idl type. */

typedef  enum  ReplyStatusType_1_2 {
    NO_EXCEPTION,
    USER_EXCEPTION,
    SYSTEM_EXCEPTION,
    LOCATION_FORWARD,			/* GIOP 1.0/1.1 stop here. */
    LOCATION_FORWARD_PERM,
    NEEDS_ADDRESSING_MODE
}  ReplyStatusType ;

/*===== "Version" already defined =====*/

typedef  OctetSeq  Principal ;

/*===== "GIOPMsgType" already defined =====*/

/*===== "GIOPMsgType" already defined =====*/

/*===== "GIOPMsgType" already defined =====*/

typedef  struct  MessageHeader_1_0 {
    char  magic[4] ;
    Version  GIOP_version ;
    bool  byte_order ;
    octet  message_type ;
    unsigned  long  message_size ;
}  MessageHeader_1_0 ;

typedef  struct  MessageHeader {
    char  magic[4] ;
    Version  GIOP_version ;
    octet  flags ;
    octet  message_type ;
    unsigned  long  message_size ;
}  MessageHeader ;

/*===== "MessageHeader" already defined =====*/

/*===== "MessageHeader" already defined =====*/

typedef  struct  RequestHeader_1_0 {
    ServiceContextList  service_context ;
    unsigned  long  request_id ;
    bool  response_expected ;
    OctetSeq  object_key ;
    char  *operation ;
    Principal  requesting_principal ;
}  RequestHeader_1_0 ;

typedef  struct  RequestHeader_1_1 {
    ServiceContextList  service_context ;
    unsigned  long  request_id ;
    bool  response_expected ;
    octet  reserved[3] ;
    OctetSeq  object_key ;
    char  *operation ;
    Principal  requesting_principal ;
}  RequestHeader_1_1 ;

typedef  short  AddressingDisposition ;

#define  KeyAddr  (0)
#define  ProfileAddr  (1)
#define  ReferenceAddr  (2)

typedef  struct  IORAddressingInfo {
    unsigned  long  selected_profile_index ;
    IOR  ior ;
}  IORAddressingInfo ;

typedef  struct  TargetAddress {
    AddressingDisposition  which ;
    union {
			/* KeyAddr */
        OctetSeq  object_key ;
			/* ProfileAddr */
        TaggedProfile  profile ;
			/* ReferenceAddr */
        IORAddressingInfo  ior ;
    }  data ;
}  TargetAddress ;

typedef  struct  RequestHeader {
    unsigned  long  request_id ;
    octet  response_flags ;
    octet  reserved[3] ;
    TargetAddress  target ;
    char  *operation ;
    ServiceContextList  service_context ;
}  RequestHeader ;

/*===== "RequestHeader" already defined =====*/

/*===== "ReplyStatusType" already defined =====*/

typedef  struct  ReplyHeader_1_0 {
    ServiceContextList  service_context ;
    unsigned  long  request_id ;
    ReplyStatusType  reply_status ;
}  ReplyHeader_1_0 ;

typedef  ReplyHeader_1_0  ReplyHeader_1_1 ;

typedef  struct  ReplyHeader {
    unsigned  long  request_id ;
    ReplyStatusType  reply_status ;
    ServiceContextList  service_context ;
}  ReplyHeader ;

/*===== "ReplyHeader" already defined =====*/

typedef  struct  SystemExceptionReplyBody {
    char  *exception_id ;
    unsigned  long  minor_code_value ;
    unsigned  long  completion_status ;
}  SystemExceptionReplyBody ;

typedef  struct  CancelRequestHeader {
    unsigned  long  request_id ;
}  CancelRequestHeader ;

typedef  struct  LocateRequestHeader_1_0 {
    unsigned  long  request_id ;
    OctetSeq  object_key ;
}  LocateRequestHeader_1_0 ;

typedef  LocateRequestHeader_1_0  LocateRequestHeader_1_1 ;

typedef  struct  LocateRequestHeader {
    unsigned  long  request_id ;
    TargetAddress  target ;
}  LocateRequestHeader ;

/*===== "LocateRequestHeader" already defined =====*/

/*===== "LocateStatusType" already defined =====*/

typedef  struct  LocateReplyHeader_1_0 {
    unsigned  long  request_id ;
    LocateStatusType  locate_status ;
}  LocateReplyHeader_1_0 ;

typedef  LocateReplyHeader_1_0  LocateReplyHeader_1_1 ;

typedef  struct  LocateReplyHeader {
    unsigned  long  request_id ;
    LocateStatusType  locate_status ;
}  LocateReplyHeader ;

/*===== "LocateReplyHeader" already defined =====*/

typedef  struct  FragmentHeader {
    unsigned  long  request_id ;
}  FragmentHeader ;

/*===== "FragmentHeader" already defined =====*/
/*  ../idl/Interop/GSSUP.idl : line 1  */

/* Module: GSSUP */

#ifndef IDL_MODULE_GSSUP
#    define  IDL_MODULE_GSSUP  1
#endif

#define  GSSUPMechOID  ("oid:2.23.130.1.1.1")

typedef  struct  InitialContextToken {
    UTF8String  username ;
    UTF8String  password ;
    GSS_NT_ExportedName  target_name ;
}  InitialContextToken ;

typedef  unsigned  long  ErrorCode ;

typedef  struct  ErrorToken {
    ErrorCode  error_code ;
}  ErrorToken ;

#define  GSS_UP_S_G_UNSPECIFIED  (1)
#define  GSS_UP_S_G_NOUSER  (2)
#define  GSS_UP_S_G_BAD_PASSWORD  (3)
#define  GSS_UP_S_G_BAD_TARGET  (4)
/*  ../idl/Interop/IIOP.idl : line 1  */

/* Module: IIOP */

#ifndef IDL_MODULE_IIOP
#    define  IDL_MODULE_IIOP  1
#endif

/*===== "Version" already defined =====*/

/*===== "ProfileBody" already defined =====*/

/*===== "ProfileBody" already defined =====*/

typedef  struct  ListenPoint {
    char  *host ;
    unsigned  short  port ;
}  ListenPoint ;

typedef  SEQUENCE (ListenPoint, ListenPointList) ;

typedef  struct  BiDirIIOPServiceContext {
    ListenPointList  listen_points ;
}  BiDirIIOPServiceContext ;
/*  ../idl/Interop/IOP_DCE.idl : line 1  */

/* Module: IOP */

#ifndef IDL_MODULE_IOP
#    define  IDL_MODULE_IOP  1
#endif

typedef  struct  EndpointIdPositionComponent {
    unsigned  short  begin ;
    unsigned  short  end ;
}  EndpointIdPositionComponent ;
/*  ../idl/Interop/SendingContext.idl : line 1  */

/* Module: SendingContext */

#ifndef IDL_MODULE_SendingContext
#    define  IDL_MODULE_SendingContext  1
#endif

typedef  IOR  RunTime ;
typedef  IOR  CodeBase ;

typedef  char  *URL ;

typedef  SEQUENCE (URL, URLSeq) ;

typedef  SEQUENCE (FullValueDescription, ValueDescSeq) ;
/*  ../idl/CORBAservices/ConcurrencyControl.idl : line 1  */
/*  ../idl/CORBAservices/CosTransactions.idl : line 1  */

/* Module: CosTransactions */

#ifndef IDL_MODULE_CosTransactions
#    define  IDL_MODULE_CosTransactions  1
#endif

typedef  IOR  Current ;
typedef  IOR  TransactionFactory ;
typedef  IOR  Control ;
typedef  IOR  Terminator ;
typedef  IOR  Coordinator ;
typedef  IOR  RecoveryCoordinator ;
typedef  IOR  Resource ;
typedef  IOR  Synchronization ;
typedef  IOR  SubtransactionAwareResource ;
typedef  IOR  TransactionalObject ;

typedef  enum  Status {
    StatusActive,
    StatusMarkedRollback,
    StatusPrepared,
    StatusCommitted,
    StatusRolledBack,
    StatusUnknown,
    StatusNoTransaction,
    StatusPreparing,
    StatusCommitting,
    StatusRollingBack
}  Status ;

typedef  enum  Vote {
    VoteCommit,
    VoteRollback,
    VoteReadOnly
}  Vote ;

typedef  struct  otid_t {
    long  formatID ;
    long  bqual_length ;
    OctetSeq  tid ;
}  otid_t ;

typedef  struct  TransIdentity {
    Coordinator  coord ;
    Terminator  term ;
    otid_t  otid ;
}  TransIdentity ;

typedef  struct  PropagationContext {
    unsigned  long  timeout ;
    TransIdentity  current ;
    SEQUENCE (TransIdentity, parents) ;
    Any  implementation_specific_data ;
}  PropagationContext ;

/* Module: CosConcurrencyControl */

#ifndef IDL_MODULE_CosConcurrencyControl
#    define  IDL_MODULE_CosConcurrencyControl  1
#endif

typedef  enum  lock_mode {
    Read,
    Write,
    Upgrade,
    Intention_read,
    Intention_write
}  lock_mode ;

typedef  IOR  LockCoordinator ;
typedef  IOR  LockSet ;
typedef  IOR  TransactionalLockSet ;
typedef  IOR  LockSetFactory ;
/*  ../idl/CORBAservices/CosCollection.idl : line 1  */

/* Module: CosCollection */

#ifndef IDL_MODULE_CosCollection
#    define  IDL_MODULE_CosCollection  1
#endif

typedef  IOR  Collection ;

typedef  char  *Istring ;

typedef  struct  NVPair {
    Istring  name ;
    Any  value ;
}  NVPair ;

typedef  SEQUENCE (NVPair, ParameterList) ;

typedef  enum  IteratorInvalidReason {
    is_invalid,
    is_not_for_collection,
    is_const
}  IteratorInvalidReason ;

typedef  enum  ElementInvalidReason {
    element_type_invalid,
    positioning_property_invalid,
    element_exists
}  ElementInvalidReason ;

typedef  IOR  Operations ;
typedef  IOR  Command ;
typedef  IOR  Comparator ;
typedef  IOR  Iterator ;
typedef  IOR  OrderedIterator ;
typedef  IOR  SequentialIterator ;
typedef  IOR  KeyIterator ;
typedef  IOR  EqualityIterator ;
typedef  IOR  EqualityKeyIterator ;
typedef  IOR  SortedIterator ;

typedef  enum  LowerBoundStyle {
    equal_lo,
    greater,
    greater_or_equal
}  LowerBoundStyle ;

typedef  enum  UpperBoundStyle {
    equal_up,
    less,
    less_or_equal
}  UpperBoundStyle ;

typedef  IOR  KeySortedIterator ;
typedef  IOR  EqualitySortedIterator ;
typedef  IOR  EqualityKeySortedIterator ;
typedef  IOR  EqualitySequentialIterator ;
typedef  IOR  OrderedCollection ;
typedef  IOR  SequentialCollection ;
typedef  IOR  SortedCollection ;
typedef  IOR  EqualityCollection ;
typedef  IOR  KeyCollection ;
typedef  IOR  EqualityKeyCollection ;
typedef  IOR  KeySortedCollection ;
typedef  IOR  EqualitySortedCollection ;
typedef  IOR  EqualityKeySortedCollection ;
typedef  IOR  EqualitySequentialCollection ;
typedef  IOR  KeySet ;
typedef  IOR  KeyBag ;
typedef  IOR  Map ;
typedef  IOR  Relation ;
typedef  IOR  Set ;
typedef  IOR  Bag ;
typedef  IOR  KeySortedSet ;
typedef  IOR  KeySortedBag ;
typedef  IOR  SortedMap ;
typedef  IOR  SortedRelation ;
typedef  IOR  SortedSet ;
typedef  IOR  SortedBag ;
typedef  IOR  CSequence ;
typedef  IOR  EqualitySequence ;
typedef  IOR  Heap ;
typedef  IOR  RestrictedAccessCollection ;
typedef  IOR  Queue ;
typedef  IOR  Deque ;
typedef  IOR  Stack ;
typedef  IOR  PriorityQueue ;
typedef  IOR  CollectionFactory ;
typedef  IOR  CollectionFactories ;
typedef  IOR  RACollectionFactory ;
typedef  IOR  RACollectionFactories ;
typedef  IOR  KeySetFactory ;
typedef  IOR  KeyBagFactory ;
typedef  IOR  MapFactory ;
typedef  IOR  RelationFactory ;
typedef  IOR  SetFactory ;
typedef  IOR  BagFactory ;
typedef  IOR  KeySortedSetFactory ;
typedef  IOR  KeySortedBagFactory ;
typedef  IOR  SortedMapFactory ;
typedef  IOR  SortedRelationFactory ;
typedef  IOR  SortedSetFactory ;
typedef  IOR  SortedBagFactory ;
typedef  IOR  SequenceFactory ;
typedef  IOR  EqualitySequenceFactory ;
typedef  IOR  HeapFactory ;
typedef  IOR  QueueFactory ;
typedef  IOR  StackFactory ;
typedef  IOR  DequeFactory ;
typedef  IOR  PriorityQueueFactory ;
/*  ../idl/CORBAservices/CosExternalizationContainment.idl : line 1  */
/*  ../idl/CORBAservices/CosContainment.idl : line 1  */
/*  ../idl/CORBAservices/CosGraphs.idl : line 1  */
/*  ../idl/CORBAservices/CosRelationships.idl : line 1  */
/*  ../idl/CORBAservices/CosObjectIdentity.idl : line 1  */

/* Module: CosObjectIdentity */

#ifndef IDL_MODULE_CosObjectIdentity
#    define  IDL_MODULE_CosObjectIdentity  1
#endif

typedef  unsigned  long  ObjectIdentifier ;

typedef  IOR  IdentifiableObject ;

/* Module: CosRelationships */

#ifndef IDL_MODULE_CosRelationships
#    define  IDL_MODULE_CosRelationships  1
#endif

typedef  IOR  RoleFactory ;
typedef  IOR  RelationshipFactory ;
typedef  IOR  Relationship ;
typedef  IOR  Role ;
typedef  IOR  RelationshipIterator ;

typedef  IOR  RelatedObject ;

typedef  SEQUENCE (Role, Roles) ;

typedef  char  *RoleName ;

typedef  SEQUENCE (RoleName, RoleNames) ;

typedef  struct  NamedRole {
    RoleName  name ;
    Role  aRole ;
}  NamedRole ;

typedef  SEQUENCE (NamedRole, NamedRoles) ;

typedef  struct  RelationshipHandle {
    Relationship  the_relationship ;
    ObjectIdentifier  constant_random_id ;
}  RelationshipHandle ;

typedef  SEQUENCE (RelationshipHandle, RelationshipHandles) ;

typedef  struct  NamedRoleType {
    RoleName  name ;
    InterfaceDef  named_role_type ;
}  NamedRoleType ;

typedef  SEQUENCE (NamedRoleType, NamedRoleTypes) ;

typedef  SEQUENCE (InterfaceDef, InterfaceDefs) ;

/* Module: CosGraphs */

#ifndef IDL_MODULE_CosGraphs
#    define  IDL_MODULE_CosGraphs  1
#endif

typedef  IOR  TraversalFactory ;
typedef  IOR  Traversal ;
typedef  IOR  TraversalCriteria ;
typedef  IOR  Node ;
typedef  IOR  NodeFactory ;
typedef  IOR  EdgeIterator ;

typedef  struct  NodeHandle {
    Node  the_node ;
    ObjectIdentifier  constant_random_id ;
}  NodeHandle ;

typedef  SEQUENCE (NodeHandle, NodeHandles) ;

/*===== "NamedRole" already defined =====*/

/*===== "NamedRoles" already defined =====*/

typedef  struct  EndPoint {
    NodeHandle  the_node ;
    NamedRole  the_role ;
}  EndPoint ;

typedef  SEQUENCE (EndPoint, EndPoints) ;

typedef  struct  Edge {
    EndPoint  from ;
    RelationshipHandle  the_relationship ;
    EndPoints  relatives ;
}  Edge ;

typedef  SEQUENCE (Edge, Edges) ;

typedef  enum  PropagationValue {
    deep,
    shallow,
    none,
    inhibit
}  PropagationValue ;

typedef  enum  Mode {
    depthFirst,
    breadthFirst,
    bestFirst
}  Mode ;

typedef  unsigned  long  TraversalScopedId ;

typedef  struct  ScopedEndPoint {
    EndPoint  point ;
    TraversalScopedId  id ;
}  ScopedEndPoint ;

typedef  SEQUENCE (ScopedEndPoint, ScopedEndPoints) ;

typedef  struct  ScopedRelationship {
    RelationshipHandle  scoped_relationship ;
    TraversalScopedId  id ;
}  ScopedRelationship ;

typedef  struct  ScopedEdge {
    ScopedEndPoint  from ;
    ScopedRelationship  the_relationship ;
    ScopedEndPoints  relatives ;
}  ScopedEdge ;

typedef  SEQUENCE (ScopedEdge, ScopedEdges) ;

typedef  struct  WeightedEdge {
    Edge  the_edge ;
    unsigned  long  weight ;
    SEQUENCE (NodeHandle, next_nodes) ;
}  WeightedEdge ;

typedef  SEQUENCE (WeightedEdge, WeightedEdges) ;

/*===== "Roles" already defined =====*/

/* Module: CosContainment */

#ifndef IDL_MODULE_CosContainment
#    define  IDL_MODULE_CosContainment  1
#endif

typedef  IOR  ContainsRole ;
typedef  IOR  ContainedInRole ;
/*  ../idl/CORBAservices/CosStream.idl : line 1  */
/*  ../idl/CORBAservices/CosLifeCycle.idl : line 1  */
/*  ../idl/CORBAservices/CosNaming.idl : line 1  */

/* Module: CosNaming */

#ifndef IDL_MODULE_CosNaming
#    define  IDL_MODULE_CosNaming  1
#endif

/*===== "Istring" already defined =====*/

typedef  struct  NameComponent {
    Istring  id ;
    Istring  kind ;
}  NameComponent ;

typedef  SEQUENCE (NameComponent, Name) ;

typedef  enum  BindingType {
    nobject,
    ncontext
}  BindingType ;

typedef  struct  Binding {
    Name  binding_name ;
    BindingType  binding_type ;
}  Binding ;

typedef  SEQUENCE (Binding, BindingList) ;

typedef  IOR  BindingIterator ;
typedef  IOR  NamingContext ;

typedef  enum  NotFoundReason {
    missing_node,
    not_context,
    not_object
}  NotFoundReason ;

/* Module: CosLifeCycle */

#ifndef IDL_MODULE_CosLifeCycle
#    define  IDL_MODULE_CosLifeCycle  1
#endif

typedef  Name  Key ;

typedef  IOR  Factory ;

typedef  SEQUENCE (Factory, Factories) ;

typedef  struct  NameValuePair {
    Istring  name ;
    Any  value ;
}  NameValuePair ;

typedef  SEQUENCE (NameValuePair, Criteria) ;

typedef  IOR  FactoryFinder ;
typedef  IOR  LifeCycleObject ;
typedef  IOR  GenericFactory ;

/* Module: CosStream */

#ifndef IDL_MODULE_CosStream
#    define  IDL_MODULE_CosStream  1
#endif

typedef  IOR  StreamIO ;
typedef  IOR  Streamable ;
typedef  IOR  StreamableFactory ;

/*===== "RelationshipHandle" already defined =====*/
typedef  IOR  PropagationCriteriaFactory ;

/* Module: CosExternalizationContainment */

#ifndef IDL_MODULE_CosExternalizationContainment
#    define  IDL_MODULE_CosExternalizationContainment  1
#endif
/*  ../idl/CORBAservices/CosExternalization.idl : line 1  */

/* Module: CosExternalization */

#ifndef IDL_MODULE_CosExternalization
#    define  IDL_MODULE_CosExternalization  1
#endif

typedef  IOR  Stream ;
typedef  IOR  StreamFactory ;
typedef  IOR  FileStreamFactory ;
/*  ../idl/CORBAservices/CosExternalizationReference.idl : line 1  */
/*  ../idl/CORBAservices/CosReference.idl : line 1  */

/* Module: CosReference */

#ifndef IDL_MODULE_CosReference
#    define  IDL_MODULE_CosReference  1
#endif

typedef  IOR  ReferencesRole ;
typedef  IOR  ReferencedByRole ;

/* Module: CosExternalizationReference */

#ifndef IDL_MODULE_CosExternalizationReference
#    define  IDL_MODULE_CosExternalizationReference  1
#endif
/*  ../idl/CORBAservices/CosLicensingManager.idl : line 1  */
/*  ../idl/CORBAservices/CosEventComm.idl : line 1  */

/* Module: CosEventComm */

#ifndef IDL_MODULE_CosEventComm
#    define  IDL_MODULE_CosEventComm  1
#endif

typedef  IOR  PushConsumer ;
typedef  IOR  PushSupplier ;
typedef  IOR  PullSupplier ;
typedef  IOR  PullConsumer ;
/*  ../idl/CORBAservices/CosPropertyService.idl : line 1  */

/* Module: CosPropertyService */

#ifndef IDL_MODULE_CosPropertyService
#    define  IDL_MODULE_CosPropertyService  1
#endif

typedef  char  *PropertyName ;

typedef  struct  Property {
    PropertyName  property_name ;
    Any  property_value ;
}  Property ;

typedef  enum  PropertyModeType {
    normal,
    read_only,
    fixed_normal,
    fixed_readonly,
    undefined
}  PropertyModeType ;

typedef  struct  PropertyDef {
    PropertyName  property_name ;
    Any  property_value ;
    PropertyModeType  property_mode ;
}  PropertyDef ;

typedef  struct  PropertyMode {
    PropertyName  property_name ;
    PropertyModeType  property_mode ;
}  PropertyMode ;

typedef  SEQUENCE (PropertyName, PropertyNames) ;

typedef  SEQUENCE (Property, Properties) ;

typedef  SEQUENCE (PropertyDef, PropertyDefs) ;

typedef  SEQUENCE (PropertyMode, PropertyModes) ;

typedef  SEQUENCE (TypeCode, PropertyTypes) ;

typedef  IOR  PropertyNamesIterator ;
typedef  IOR  PropertiesIterator ;
typedef  IOR  PropertySetFactory ;
typedef  IOR  PropertySetDef ;
typedef  IOR  PropertySet ;

typedef  enum  ExceptionReason {
    invalid_property_name,
    conflicting_property,
    property_not_found,
    unsupported_type_code,
    unsupported_property,
    unsupported_mode,
    fixed_property,
    read_only_property
}  ExceptionReason ;

typedef  struct  PropertyException {
    ExceptionReason  reason ;
    PropertyName  failing_property_name ;
}  PropertyException ;

typedef  SEQUENCE (PropertyException, PropertyExceptions) ;

typedef  IOR  PropertySetDefFactory ;

/* Module: CosLicensingManager */

#ifndef IDL_MODULE_CosLicensingManager
#    define  IDL_MODULE_CosLicensingManager  1
#endif

typedef  IOR  ProducerSpecificNotification ;

typedef  enum  ActionRequired {
    Continue,
    Terminate
}  ActionRequired ;

typedef  enum  Answer {
    yes,
    no
}  Answer ;

typedef  struct  Action {
    ActionRequired  action ;
    Answer  notification_required ;
    Answer  wait_for_user_confirmation_after_notification ;
    unsigned  long  notification_duration ;
    ProducerSpecificNotification  producer_notification ;
    char  *notification_text ;
}  Action ;

typedef  struct  ChallengeData {
    unsigned  long  challenge_index ;
    unsigned  long  random_number ;
    char  *digest ;
}  ChallengeData ;

typedef  enum  ChallengeProtocol {
    default_protocol,
    producer_defined
}  ChallengeProtocol ;

typedef  struct  Challenge {
    ChallengeProtocol  challenge_protocol ;
    unsigned  long  challenge_data_size ;
    Any  challenge_data ;
}  Challenge ;

typedef  Any  LicenseHandle ;

typedef  IOR  ProducerSpecificLicenseService ;
typedef  IOR  LicenseServiceManager ;
/*  ../idl/CORBAservices/CosLifeCycleContainment.idl : line 1  */
/*  ../idl/CORBAservices/CosCompoundLifeCycle.idl : line 1  */

/* Module: CosCompoundLifeCycle */

#ifndef IDL_MODULE_CosCompoundLifeCycle
#    define  IDL_MODULE_CosCompoundLifeCycle  1
#endif

typedef  IOR  OperationsFactory ;

typedef  enum  Operation {
    Copy,
    Move,
    Remove
}  Operation ;

/*===== "RelationshipHandle" already defined =====*/

/* Module: CosLifeCycleContainment */

#ifndef IDL_MODULE_CosLifeCycleContainment
#    define  IDL_MODULE_CosLifeCycleContainment  1
#endif
/*  ../idl/CORBAservices/CosLifeCycleReference.idl : line 1  */

/* Module: CosLifeCycleReference */

#ifndef IDL_MODULE_CosLifeCycleReference
#    define  IDL_MODULE_CosLifeCycleReference  1
#endif
/*  ../idl/CORBAservices/CosPersistenceDS_CLI.idl : line 1  */
/*  ../idl/CORBAservices/CosPersistenceDDO.idl : line 1  */
/*  ../idl/CORBAservices/CosPersistencePID.idl : line 1  */

/* Module: CosPersistencePID */

#ifndef IDL_MODULE_CosPersistencePID
#    define  IDL_MODULE_CosPersistencePID  1
#endif

typedef  IOR  PID ;

/* Module: CosPersistenceDDO */

#ifndef IDL_MODULE_CosPersistenceDDO
#    define  IDL_MODULE_CosPersistenceDDO  1
#endif

typedef  IOR  DDO ;

/* Module: CosPersistenceDS_CLI */

#ifndef IDL_MODULE_CosPersistenceDS_CLI
#    define  IDL_MODULE_CosPersistenceDS_CLI  1
#endif

typedef  IOR  UserEnvironment ;
typedef  IOR  Connection ;
typedef  IOR  ConnectionFactory ;
typedef  IOR  Cursor ;
typedef  IOR  CursorFactory ;
typedef  IOR  PID_CLI ;
typedef  IOR  Datastore_CLI ;
/*  ../idl/CORBAservices/CosPersistencePDS_DA.idl : line 1  */
/*  ../idl/CORBAservices/CosPersistencePDS.idl : line 1  */

/* Module: CosPersistencePDS */

#ifndef IDL_MODULE_CosPersistencePDS
#    define  IDL_MODULE_CosPersistencePDS  1
#endif

typedef  IOR  PDS ;

/* Module: CosPersistencePDS_DA */

#ifndef IDL_MODULE_CosPersistencePDS_DA
#    define  IDL_MODULE_CosPersistencePDS_DA  1
#endif

typedef  char  *DAObjectID ;

typedef  IOR  PID_DA ;
typedef  IOR  DAObject ;
typedef  IOR  DAObjectFactory ;
typedef  IOR  DAObjectFactoryFinder ;
typedef  IOR  PDS_DA ;
/*  ../idl/CORBAservices/CosPersistencePO.idl : line 1  */

/* Module: CosPersistencePO */

#ifndef IDL_MODULE_CosPersistencePO
#    define  IDL_MODULE_CosPersistencePO  1
#endif

typedef  IOR  PO ;
typedef  IOR  SD ;
/*  ../idl/CORBAservices/CosPersistencePOM.idl : line 1  */

/* Module: CosPersistencePOM */

#ifndef IDL_MODULE_CosPersistencePOM
#    define  IDL_MODULE_CosPersistencePOM  1
#endif

typedef  IOR  POM ;
/*  ../idl/CORBAservices/CosQuery.idl : line 1  */
/*  ../idl/CORBAservices/CosQueryCollection.idl : line 1  */

/* Module: CosQueryCollection */

#ifndef IDL_MODULE_CosQueryCollection
#    define  IDL_MODULE_CosQueryCollection  1
#endif

typedef  enum  ValueType {
    TypeBoolean,
    TypeChar,
    TypeOctet,
    TypeShort,
    TypeUShort,
    TypeLong,
    TypeULong,
    TypeFloat,
    TypeDouble,
    TypeString,
    TypeObject,
    TypeAny,
    TypeSmallInt,
    TypeInteger,
    TypeReal,
    TypeDoublePrecision,
    TypeCharacter,
    TypeDecimal,
    TypeNumeric
}  ValueType ;

typedef  struct  Decimal {
    long  precision ;
    long  scale ;
    OctetSeq  value ;
}  Decimal ;

typedef  struct  Value {
    ValueType  which ;
    union {
			/* TypeBoolean */
        bool  b ;
			/* TypeChar */
        char  c ;
			/* TypeOctet */
        octet  o ;
			/* TypeShort */
        short  s ;
			/* TypeUShort */
        unsigned  short  us ;
			/* TypeLong */
        long  l ;
			/* TypeULong */
        unsigned  long  ul ;
			/* TypeFloat */
        float  f ;
			/* TypeDouble */
        double  d ;
			/* TypeString */
        char  *str ;
			/* TypeObject */
        Object  obj ;
			/* TypeAny */
        Any  a ;
			/* TypeSmallInt */
        short  si ;
			/* TypeInteger */
        long  i ;
			/* TypeReal */
        float  r ;
			/* TypeDoublePrecision */
        double  dp ;
			/* TypeCharacter */
        char  *ch ;
			/* TypeDecimal */
        Decimal  dec ;
			/* TypeNumeric */
        Decimal  n ;
    }  data ;
}  Value ;

typedef  bool  Null ;

typedef  struct  FieldValue {
    Null  which ;
    union {
			/* FALSE */
        Value  v ;
    }  data ;
}  FieldValue ;

typedef  SEQUENCE (FieldValue, Record) ;

/*===== "Istring" already defined =====*/

/*===== "NVPair" already defined =====*/

/*===== "ParameterList" already defined =====*/

/* Module: CosQuery */

#ifndef IDL_MODULE_CosQuery
#    define  IDL_MODULE_CosQuery  1
#endif

typedef  enum  QueryStatus {
    complete,
    incomplete
}  QueryStatus ;

/*===== "ParameterList" already defined =====*/

typedef  IOR  QLType ;

typedef  IOR  Query ;
typedef  IOR  QueryLanguageType ;
typedef  IOR  SQLQuery ;
typedef  IOR  SQL_92Query ;
typedef  IOR  OQL ;
typedef  IOR  OQLBasic ;
typedef  IOR  OQL_93 ;
typedef  IOR  OQL_93Basic ;
typedef  IOR  QueryEvaluator ;

typedef  SEQUENCE (QLType, QLTypes) ;

typedef  IOR  QueryableCollection ;
typedef  IOR  QueryManager ;
/*  ../idl/CORBAservices/CosTimerEvent.idl : line 1  */
/*  ../idl/CORBAservices/CosTime.idl : line 1  */
/*  ../idl/CORBAservices/TimeBase.idl : line 1  */

/* Module: TimeBase */

#ifndef IDL_MODULE_TimeBase
#    define  IDL_MODULE_TimeBase  1
#endif

typedef  ULONGLONG  TimeT ;

typedef  ULONGLONG  InaccuracyT ;

typedef  short  TdfT ;

typedef  struct  UtcT {
    TimeT  time ;
    unsigned  long  inacclo ;
    unsigned  short  inacchi ;
    TdfT  tdf ;
}  UtcT ;

typedef  struct  IntervalT {
    TimeT  lower_bound ;
    TimeT  upper_bound ;
}  IntervalT ;

/* Module: CosTime */

#ifndef IDL_MODULE_CosTime
#    define  IDL_MODULE_CosTime  1
#endif

typedef  enum  TimeComparison {
    TCEqualTo,
    TCLessThan,
    TCGreaterThan,
    TCIndeterminate
}  TimeComparison ;

typedef  enum  ComparisonType {
    IntervalC,
    MidC
}  ComparisonType ;

typedef  enum  OverlapType {
    OTContainer,
    OTContained,
    OTOverlap,
    OTNoOverlap
}  OverlapType ;

typedef  IOR  TIO ;
typedef  IOR  UTO ;
typedef  IOR  TimeService ;

/* Module: CosTimerEvent */

#ifndef IDL_MODULE_CosTimerEvent
#    define  IDL_MODULE_CosTimerEvent  1
#endif

typedef  enum  TimeType {
    TTAbsolute,
    TTRelative,
    TTPeriodic
}  TimeType ;

typedef  enum  EventStatus {
    ESTimeSet,
    ESTimeCleared,
    ESTriggered,
    ESFailedTrigger
}  EventStatus ;

typedef  struct  TimerEventT {
    UtcT  utc ;
    Any  event_data ;
}  TimerEventT ;

typedef  IOR  TimerEventHandler ;
typedef  IOR  TimerEventService ;
/*  ../idl/CORBAservices/CosTradingDynamic.idl : line 1  */
/*  ../idl/CORBAservices/CosTrading.idl : line 1  */

/* Module: CosTrading */

#ifndef IDL_MODULE_CosTrading
#    define  IDL_MODULE_CosTrading  1
#endif

typedef  IOR  Lookup ;
typedef  IOR  Register ;
typedef  IOR  Link ;
typedef  IOR  Proxy ;
typedef  IOR  Admin ;
typedef  IOR  OfferIterator ;
typedef  IOR  OfferIdIterator ;

/*===== "Istring" already defined =====*/

typedef  IOR  TypeRepository ;

/*===== "PropertyName" already defined =====*/

typedef  SEQUENCE (PropertyName, PropertyNameSeq) ;

typedef  Any  PropertyValue ;

/*===== "Property" already defined =====*/

typedef  SEQUENCE (Property, PropertySeq) ;

typedef  struct  Offer {
    Object  reference ;
    PropertySeq  properties ;
}  Offer ;

typedef  SEQUENCE (Offer, OfferSeq) ;

typedef  char  *OfferId ;

typedef  SEQUENCE (OfferId, OfferIdSeq) ;

typedef  char  *ServiceTypeName ;

typedef  char  *Constraint ;

typedef  enum  FollowOption {
    local_only,
    if_no_local,
    always
}  FollowOption ;

typedef  char  *LinkName ;

typedef  SEQUENCE (LinkName, LinkNameSeq) ;

typedef  LinkNameSeq  TraderName ;

typedef  char  *PolicyName ;

typedef  SEQUENCE (PolicyName, PolicyNameSeq) ;

typedef  Any  PolicyValue ;

typedef  struct  COSTPolicy {
    PolicyName  name ;
    PolicyValue  value ;
}  COSTPolicy ;

typedef  SEQUENCE (COSTPolicy, COSTPolicySeq) ;

typedef  IOR  TraderComponents ;
typedef  IOR  SupportAttributes ;
typedef  IOR  ImportAttributes ;
typedef  IOR  LinkAttributes ;

typedef  char  *Preference ;

typedef  enum  HowManyProps {
    props_none,
    props_some,
    props_all
}  HowManyProps ;

typedef  struct  SpecifiedProps {
    HowManyProps  which ;
    union {
			/* props_some */
        PropertyNameSeq  prop_names ;
    }  data ;
}  SpecifiedProps ;

typedef  struct  OfferInfo {
    Object  reference ;
    ServiceTypeName  type ;
    PropertySeq  properties ;
}  OfferInfo ;

typedef  struct  LinkInfo {
    Lookup  target ;
    Register  target_reg ;
    FollowOption  def_pass_on_follow_rule ;
    FollowOption  limiting_follow_rule ;
}  LinkInfo ;

typedef  char  *ConstraintRecipe ;

typedef  struct  ProxyInfo {
    ServiceTypeName  type ;
    Lookup  target ;
    PropertySeq  properties ;
    bool  if_match_all ;
    ConstraintRecipe  recipe ;
    COSTPolicySeq  policies_to_pass_on ;
}  ProxyInfo ;

/*===== "OctetSeq" already defined =====*/

/* Module: CosTradingDynamic */

#ifndef IDL_MODULE_CosTradingDynamic
#    define  IDL_MODULE_CosTradingDynamic  1
#endif

typedef  IOR  DynamicPropEval ;

typedef  struct  DynamicProp {
    DynamicPropEval  eval_if ;
    TypeCode  returned_type ;
    Any  extra_info ;
}  DynamicProp ;
/*  ../idl/CORBAservices/CosTradingRepos.idl : line 1  */

/* Module: CosTradingRepos */

#ifndef IDL_MODULE_CosTradingRepos
#    define  IDL_MODULE_CosTradingRepos  1
#endif

typedef  IOR  ServiceTypeRepository ;

typedef  SEQUENCE (ServiceTypeName, ServiceTypeNameSeq) ;

/*===== "PropertyMode" already defined =====*/

typedef  struct  PropStruct {
    PropertyName  name ;
    TypeCode  value_type ;
    PropertyMode  mode ;
}  PropStruct ;

typedef  SEQUENCE (PropStruct, PropStructSeq) ;

/*===== "Identifier" already defined =====*/

typedef  struct  IncarnationNumber {
    unsigned  long  high ;
    unsigned  long  low ;
}  IncarnationNumber ;

typedef  struct  TypeStruct {
    Identifier  if_name ;
    PropStructSeq  props ;
    ServiceTypeNameSeq  super_types ;
    bool  masked ;
    IncarnationNumber  incarnation ;
}  TypeStruct ;

typedef  enum  ListOption {
    all,
    since
}  ListOption ;

typedef  struct  SpecifiedServiceTypes {
    ListOption  which ;
    union {
			/* since */
        IncarnationNumber  incarnation ;
    }  data ;
}  SpecifiedServiceTypes ;
/*  ../idl/CORBAservices/CosTSPortability.idl : line 1  */

/* Module: CosTSPortability */

#ifndef IDL_MODULE_CosTSPortability
#    define  IDL_MODULE_CosTSPortability  1
#endif

typedef  long  ReqId ;

typedef  IOR  Sender ;
typedef  IOR  Receiver ;
/*  ../idl/CORBAservices/CosTypedEventChannelAdmin.idl : line 1  */
/*  ../idl/CORBAservices/CosEventChannelAdmin.idl : line 1  */

/* Module: CosEventChannelAdmin */

#ifndef IDL_MODULE_CosEventChannelAdmin
#    define  IDL_MODULE_CosEventChannelAdmin  1
#endif

typedef  IOR  ProxyPushConsumer ;
typedef  IOR  ProxyPullSupplier ;
typedef  IOR  ProxyPullConsumer ;
typedef  IOR  ProxyPushSupplier ;
typedef  IOR  ConsumerAdmin ;
typedef  IOR  SupplierAdmin ;
typedef  IOR  EventChannel ;
/*  ../idl/CORBAservices/CosTypedEventComm.idl : line 1  */

/* Module: CosTypedEventComm */

#ifndef IDL_MODULE_CosTypedEventComm
#    define  IDL_MODULE_CosTypedEventComm  1
#endif

typedef  IOR  TypedPushConsumer ;
typedef  IOR  TypedPullSupplier ;

/* Module: CosTypedEventChannelAdmin */

#ifndef IDL_MODULE_CosTypedEventChannelAdmin
#    define  IDL_MODULE_CosTypedEventChannelAdmin  1
#endif

/*===== "Key" already defined =====*/

typedef  IOR  TypedProxyPushConsumer ;
typedef  IOR  TypedProxyPullSupplier ;
typedef  IOR  TypedSupplierAdmin ;
typedef  IOR  TypedConsumerAdmin ;
typedef  IOR  TypedEventChannel ;
/*  ../idl/CORBAservices/DCE_CIOPSecurity.idl : line 1  */

/* Module: DCE_CIOPSecurity */

#ifndef IDL_MODULE_DCE_CIOPSecurity
#    define  IDL_MODULE_DCE_CIOPSecurity  1
#endif

/*===== DCE_CIOPSecurity::TAG_DCE_SEC_MECH previously defined as IOP_TAG_DCE_SEC_MECH in IOP =====*/

typedef  unsigned  short  DCEAuthorization ;

#define  DCEAuthorizationNone  (0)
#define  DCEAuthorizationName  (1)
#define  DCEAuthorizationDCE  (2)

typedef  struct  DCESecurityMechanismInfo {
    DCEAuthorization  authorization_service ;
    SEQUENCE (TaggedComponent, components) ;
}  DCESecurityMechanismInfo ;
/*  ../idl/CORBAservices/LifeCycleService.idl : line 1  */

/* Module: LifeCycleService */

#ifndef IDL_MODULE_LifeCycleService
#    define  IDL_MODULE_LifeCycleService  1
#endif

/*===== "PolicyList" already defined =====*/

typedef  SEQUENCE (Key, COSLCSKeys) ;

typedef  SEQUENCE (NameValuePair, PropertyList) ;

typedef  SEQUENCE (NameComponent, NameComponents) ;

typedef  IOR  LifeCycleServiceAdmin ;
/*  ../idl/CORBAservices/Lname-library.idl : line 1  */

typedef  IOR  LNameComponent ;
typedef  IOR  LName ;
/*  ../idl/CORBAservices/NRService.idl : line 1  */
/*  ../idl/CORBAservices/SecurityLevel2.idl : line 1  */
/*  ../idl/CORBAservices/SecurityLevel1.idl : line 1  */
/*  ../idl/CORBAservices/Security.idl : line 1  */

/* Module: Security */

#ifndef IDL_MODULE_Security
#    define  IDL_MODULE_Security  1
#endif

typedef  char  *SecurityName ;

typedef  OctetSeq  Opaque ;

#define  SecurityLevel1  (1)
#define  SecurityLevel2  (2)
#define  NonRepudiation  (3)
#define  SecurityORBServiceRaady  (4)
#define  SecurityServiceReady  (5)
#define  ReplaceORBServices  (6)
#define  ReplaceSecurityServices  (7)
#define  StandardSecureInteroperability  (8)
#define  DCESecureInteroperability  (9)
#define  CommonInteroperabilityLevel0  (10)
#define  CommonInteroperabilityLevel1  (11)
#define  CommonInteroperabilityLevel2  (12)
#define  SecurityMechanismType  (1)
#define  SecurityAttribute  (2)

typedef  struct  ExtensibleFamily {
    unsigned  short  family_definer ;
    unsigned  short  family ;
}  ExtensibleFamily ;

typedef  char  *MechanismType ;

typedef  struct  SecurityMechandName {
    MechanismType  mech_type ;
    SecurityName  security_name ;
}  SecurityMechandName ;

typedef  SEQUENCE (MechanismType, MechanismTypeList) ;

typedef  SEQUENCE (SecurityMechandName, SecurityMechandNameList) ;

typedef  unsigned  long  SecurityAttributeType ;

#define  AuditId  (1)
#define  AccountingId  (2)
#define  NonRepudiationId  (3)
#define  Public  (1)
#define  AccessId  (2)
#define  PrimaryGroupId  (3)
#define  GroupId  (4)
#define  Role  (5)
#define  AttributeSet  (6)
#define  Clearance  (7)
#define  Capability  (8)

typedef  struct  AttributeType {
    ExtensibleFamily  attribute_family ;
    SecurityAttributeType  attribute_type ;
}  AttributeType ;

typedef  SEQUENCE (AttributeType, AttributeTypeList) ;

typedef  struct  SecAttribute {
    AttributeType  attribute_type ;
    Opaque  defining_authority ;
    Opaque  value ;
}  SecAttribute ;

typedef  SEQUENCE (SecAttribute, AttributeList) ;

typedef  enum  AuthenticationStatus {
    SecAuthSuccess,
    SecAuthFailure,
    SecAuthContinue,
    SecAuthExpired
}  AuthenticationStatus ;

typedef  enum  AssociationStatus {
    SecAssocSuccess,
    SecAssocFailure,
    SecAssocContinue
}  AssociationStatus ;

typedef  unsigned  long  AuthenticationMethod ;

typedef  enum  CredentialType {
    SecInvocationCredentials,
    SecNRCredentials
}  CredentialType ;

typedef  struct  Right {
    ExtensibleFamily  rights_family ;
    char  *right ;
}  Right ;

typedef  SEQUENCE (Right, RightsList) ;

typedef  enum  RightsCombinator {
    SecAllRights,
    SecAnyRight
}  RightsCombinator ;

typedef  enum  DelegationState {
    SecInitiator,
    SecDelegate
}  DelegationState ;

/*===== "UtcT" already defined =====*/

/*===== "IntervalT" already defined =====*/

/*===== "TimeT" already defined =====*/

typedef  enum  SecurityFeature {
    SecNoDelegation,
    SecSimpleDelegation,
    SecCompositeDelegation,
    SecNoProtection,
    SecIntegrity,
    SecConfidentiality,
    SecIntegrityAndConfidentiality,
    SecDetectReplay,
    SecDetectMisordering,
    SecEstablishTrustInTarget
}  SecurityFeature ;

typedef  struct  SecurityFeatureValue {
    SecurityFeature  feature ;
    bool  value ;
}  SecurityFeatureValue ;

typedef  SEQUENCE (SecurityFeatureValue, SecurityFeatureValueList) ;

typedef  enum  QOP {
    SecQOPNoProtection,
    SecQOPIntegrity,
    SecQOPConfidentiality,
    SecQOPIntegrityAndConfidentiality
}  QOP ;

/*===== "AssociationOptions" already defined =====*/

/*===== Security::NoProtection previously defined as NoProtection in CSIIOP =====*/

/*===== Security::Integrity previously defined as Integrity in CSIIOP =====*/

/*===== Security::Confidentiality previously defined as Confidentiality in CSIIOP =====*/

/*===== Security::DetectReplay previously defined as DetectReplay in CSIIOP =====*/

/*===== Security::DetectMisordering previously defined as DetectMisordering in CSIIOP =====*/

/*===== Security::EstablishTrustInTarget previously defined as EstablishTrustInTarget in CSIIOP =====*/

/*===== Security::EstablishTrustInClient previously defined as EstablishTrustInClient in CSIIOP =====*/

typedef  enum  RequiresSupports {
    SecRequires,
    SecSupports
}  RequiresSupports ;

typedef  enum  CommunicationDirection {
    SecDirectionBoth,
    SecDirectionRequest,
    SecDirectionReply
}  CommunicationDirection ;

typedef  struct  OptionsDirectionPair {
    AssociationOptions  options ;
    CommunicationDirection  direction ;
}  OptionsDirectionPair ;

typedef  SEQUENCE (OptionsDirectionPair, OptionsDirectionPairList) ;

typedef  enum  DelegationMode {
    SecDelModeNoDelegation,
    SecDelModeSimpleDelegation,
    SecDelModeCompositeDelegation
}  DelegationMode ;

typedef  struct  MechandOptions {
    MechanismType  mechanism_type ;
    AssociationOptions  options_supported ;
}  MechandOptions ;

typedef  SEQUENCE (MechandOptions, MechandOptionsList) ;

typedef  unsigned  long  AuditChannelId ;

typedef  unsigned  short  COSSEventType ;

#define  AuditAll  (0)
#define  AuditPrincipalAuth  (1)
#define  AuditSessionAuth  (2)
#define  AuditAuthorization  (3)
#define  AuditInvocation  (4)
#define  AuditSecEnvChange  (5)
#define  AuditPolicyChange  (6)
#define  AuditObjectCreation  (7)
#define  AuditObjectDestruction  (8)
#define  AuditNonRepudiation  (9)

typedef  struct  AuditEventType {
    ExtensibleFamily  event_family ;
    COSSEventType  event_type ;
}  AuditEventType ;

typedef  SEQUENCE (AuditEventType, AuditEventTypeList) ;

typedef  unsigned  long  SelectorType ;

#define  InterfaceRef  (1)
#define  ObjectRef  (2)
#define  COSSOperation  (3)
#define  Initiator  (4)
#define  SuccessFailure  (5)
#define  Time  (6)

typedef  struct  SelectorValue {
    SelectorType  selector ;
    Any  value ;
}  SelectorValue ;

typedef  SEQUENCE (SelectorValue, SelectorValueList) ;

#define  SecClientInvocationAccess  (1)
#define  SecTargetInvocationAccess  (2)
#define  SecApplicationAccess  (3)
#define  SecClientInvocationAudit  (4)
#define  SecTargetInvocationAudit  (5)
#define  SecApplicationAudit  (6)
#define  SecDelegation  (7)
#define  SecClientSecureInvocation  (8)
#define  SecTargetSecureInvocation  (9)
#define  SecNonRepudiation  (10)
#define  SecMechanismsPolicy  (12)
#define  SecCredentialsPolicy  (13)
#define  SecFeaturesPolicy  (14)
#define  SecQOPPolicy  (15)

/* Module: SecurityLevel1 */

#ifndef IDL_MODULE_SecurityLevel1
#    define  IDL_MODULE_SecurityLevel1  1
#endif

/* Module: SecurityLevel2 */

#ifndef IDL_MODULE_SecurityLevel2
#    define  IDL_MODULE_SecurityLevel2  1
#endif

typedef  IOR  PrincipalAuthenticator ;
typedef  IOR  Credentials ;

typedef  SEQUENCE (Credentials, CredentialsList) ;

typedef  IOR  RequiredRights ;
typedef  IOR  AuditChannel ;
typedef  IOR  AuditDecision ;
typedef  IOR  AccessDecision ;
typedef  IOR  QOPPolicy ;
typedef  IOR  MechanismPolicy ;
typedef  IOR  SecurityFeaturesPolicy ;
typedef  IOR  InvocationCredentialsPolicy ;

/*===== "DelegationMode" already defined =====*/

/* Module: NRService */

#ifndef IDL_MODULE_NRService
#    define  IDL_MODULE_NRService  1
#endif

typedef  char  *NRMech ;

typedef  ExtensibleFamily  NRPolicyId ;

typedef  enum  EvidenceType {
    SecProofofCreation,
    SecProofofReceipt,
    SecProofofApproval,
    SecProofofRetrieval,
    SecProofofOrigin,
    SecProofofDelivery,
    SecNoEvidence
}  EvidenceType ;

typedef  enum  NRVerificationResult {
    SecNRInvalid,
    SecNRValid,
    SecNRConditionallyValid
}  NRVerificationResult ;

typedef  unsigned  long  DurationInMinutes ;

#define  DurationHour  (60)
#define  DurationDay  (1440)
#define  DurationWeek  (10080)
#define  DurationMonth  (43200)
#define  DurationYear  (525600)

typedef  long  TimeOffsetInMinutes ;

typedef  struct  NRPolicyFeatures {
    NRPolicyId  policy_id ;
    unsigned  long  policy_version ;
    NRMech  mechanism ;
}  NRPolicyFeatures ;

typedef  SEQUENCE (NRPolicyFeatures, NRPolicyFeaturesList) ;

typedef  struct  RequestFeatures {
    NRPolicyFeatures  requested_policy ;
    EvidenceType  requested_evidence ;
    char  *requested_evidence_generators ;
    char  *requested_evidence_recipients ;
    bool  include_this_token_in_evidence ;
}  RequestFeatures ;

typedef  struct  EvidenceDescriptor {
    EvidenceType  evidence_type ;
    DurationInMinutes  evidence_validity_duration ;
    bool  must_use_trusted_time ;
}  EvidenceDescriptor ;

typedef  SEQUENCE (EvidenceDescriptor, EvidenceDescriptorList) ;

typedef  struct  AuthorityDescriptor {
    char  *authority_name ;
    char  *authority_role ;
    TimeOffsetInMinutes  last_revocation_check_offset ;
}  AuthorityDescriptor ;

typedef  SEQUENCE (AuthorityDescriptor, AuthorityDescriptorList) ;

typedef  struct  MechanismDescriptor {
    NRMech  mech_type ;
    AuthorityDescriptorList  authority_list ;
    TimeOffsetInMinutes  max_time_skew ;
}  MechanismDescriptor ;

typedef  SEQUENCE (MechanismDescriptor, MechanismDescriptorList) ;

typedef  IOR  NRCredentials ;
typedef  IOR  NRPolicy ;
/*  ../idl/CORBAservices/SECIOP.idl : line 1  */

/* Module: SECIOP */

#ifndef IDL_MODULE_SECIOP
#    define  IDL_MODULE_SECIOP  1
#endif

/*===== SECIOP::TAG_GENERIC_SEC_MECH previously defined as IOP_TAG_GENERIC_SEC_MECH in IOP =====*/

/*===== SECIOP::TAG_ASSOCIATION_OPTIONS previously defined as IOP_TAG_ASSOCIATION_OPTIONS in IOP =====*/

/*===== SECIOP::TAG_SEC_NAME previously defined as IOP_TAG_SEC_NAME in IOP =====*/

typedef  struct  TargetAssociationOptions {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
}  TargetAssociationOptions ;

typedef  struct  GenericMechanismInfo {
    OctetSeq  security_mechanism_type ;
    OctetSeq  mech_specific_data ;
    SEQUENCE (TaggedComponent, components) ;
}  GenericMechanismInfo ;

/*===== "MsgType" already defined =====*/

/*===== "ContextId" already defined =====*/

typedef  enum  ContextIdDefn {
    CIDClient,
    CIDPeer,
    CIDSender
}  ContextIdDefn ;

/*===== "EstablishContext" already defined =====*/

/*===== "CompleteEstablishContext" already defined =====*/

typedef  struct  ContinueEstablishContext {
    ContextId  client_context_id ;
    OctetSeq  continuation_context_token ;
}  ContinueEstablishContext ;

typedef  struct  DiscardContext {
    ContextIdDefn  message_context_id_defn ;
    ContextId  message_context_id ;
    OctetSeq  discard_context_token ;
}  DiscardContext ;

typedef  struct  SECIOPMessageError {
    ContextIdDefn  message_context_id_defn ;
    ContextId  message_context_id ;
    long  major_status ;
    long  minor_status ;
}  SECIOPMessageError ;

typedef  enum  ContextTokenType {
    SecTokenTypeWrap,
    SecTokenTypeMIC
}  ContextTokenType ;

/*===== "MessageInContext" already defined =====*/

typedef  struct  SequencingHeader {
    octet  control_state ;
    unsigned  long  direct_sequence_number ;
    unsigned  long  reverse_sequence_number ;
    unsigned  long  reverse_window ;
}  SequencingHeader ;

/*===== "SecurityName" already defined =====*/

typedef  unsigned  short  CryptographicProfile ;

typedef  SEQUENCE (CryptographicProfile, CryptographicProfileList) ;

#define  MD5_RSA  (20)
#define  MD5_DES_CBC  (21)
#define  DES_CBC  (22)
#define  MD5_DES_CBC_SOURCE  (23)
#define  DES_CBC_SOURCE  (24)

/*===== SECIOP::TAG_SPKM_1_SEC_MECH previously defined as IOP_TAG_SPKM_1_SEC_MECH in IOP =====*/

typedef  struct  SPKM_1 {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    CryptographicProfileList  crypto_profile ;
    SecurityName  security_name ;
}  SPKM_1 ;

/*===== SECIOP::TAG_SPKM_2_SEC_MECH previously defined as IOP_TAG_SPKM_2_SEC_MECH in IOP =====*/

typedef  struct  SPKM_2 {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    CryptographicProfileList  crypto_profile ;
    SecurityName  security_name ;
}  SPKM_2 ;

#define  DES_CBC_DES_MAC  (10)
#define  DES_CBC_MD5  (11)
#define  DES_MAC  (12)
#define  MD5  (13)

/*===== SECIOP::TAG_KerberosV5_SEC_MECH previously defined as IOP_TAG_KerberosV5_SEC_MECH in IOP =====*/

typedef  struct  KerberosV5 {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    CryptographicProfileList  crypto_profile ;
    SecurityName  security_name ;
}  KerberosV5 ;

#define  FullSecurity  (1)
#define  NoDataConfidentiality  (2)
#define  LowGradeConfidentiality  (3)
#define  AgreedDefault  (5)

/*===== SECIOP::TAG_CSI_ECMA_Secret_SEC_MECH previously defined as IOP_TAG_CSI_ECMA_Secret_SEC_MECH in IOP =====*/

typedef  struct  CSI_ECMA_Secret {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    CryptographicProfileList  crypto_profile ;
    SecurityName  security_name ;
}  CSI_ECMA_Secret ;

/*===== SECIOP::TAG_CSI_ECMA_Hybrid_SEC_MECH previously defined as IOP_TAG_CSI_ECMA_Hybrid_SEC_MECH in IOP =====*/

typedef  struct  CSI_ECMA_Hybrid {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    CryptographicProfileList  crypto_profile ;
    SecurityName  security_name ;
}  CSI_ECMA_Hybrid ;

/*===== SECIOP::TAG_CSI_ECMA_Public_SEC_MECH previously defined as IOP_TAG_CSI_ECMA_Public_SEC_MECH in IOP =====*/

typedef  struct  CSI_ECMA_Public {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    CryptographicProfileList  crypto_profile ;
    SecurityName  security_name ;
}  CSI_ECMA_Public ;
/*  ../idl/CORBAservices/SecurityAdmin.idl : line 1  */

/* Module: SecurityAdmin */

#ifndef IDL_MODULE_SecurityAdmin
#    define  IDL_MODULE_SecurityAdmin  1
#endif

typedef  IOR  AccessPolicy ;
typedef  IOR  DomainAccessPolicy ;
typedef  IOR  AuditPolicy ;
typedef  IOR  SecureInvocationPolicy ;
typedef  IOR  DelegationPolicy ;
/*  ../idl/CORBAservices/SecurityReplaceable.idl : line 1  */

/* Module: SecurityReplaceable */

#ifndef IDL_MODULE_SecurityReplaceable
#    define  IDL_MODULE_SecurityReplaceable  1
#endif

typedef  IOR  SecurityContext ;
typedef  IOR  Vault ;
/*  ../idl/CORBAservices/SSLIOP.idl : line 1  */

/* Module: SSLIOP */

#ifndef IDL_MODULE_SSLIOP
#    define  IDL_MODULE_SSLIOP  1
#endif

#define  TAG_SSL_SEC_TRANS  (20)

typedef  struct  SSL {
    AssociationOptions  target_supports ;
    AssociationOptions  target_requires ;
    unsigned  short  port ;
}  SSL ;
/*  ../idl/Misc/Messaging.idl : line 1  */
/*  ../idl/Misc/Dynamic.idl : line 1  */

/* Module: Dynamic */

#ifndef IDL_MODULE_Dynamic
#    define  IDL_MODULE_Dynamic  1
#endif

typedef  struct  Parameter {
    Any  argument ;
    ParameterMode  mode ;
}  Parameter ;

/*===== "ParameterList" already defined =====*/

typedef  StringSeq  ContextList ;

typedef  SEQUENCE (TypeCode, ExceptionList) ;

typedef  StringSeq  RequestContext ;

/* Module: Messaging */

#ifndef IDL_MODULE_Messaging
#    define  IDL_MODULE_Messaging  1
#endif

typedef  short  RebindMode ;

#define  MESSAGING_TRANSPARENT_REBIND  (0)
#define  MESSAGING_NO_REBIND  (1)
#define  MESSAGING_NO_RECONNECT  (2)

typedef  short  SyncScope ;

#define  MESSAGING_SYNC_NONE  (0)
#define  MESSAGING_SYNC_WITH_TRANSPORT  (1)
#define  MESSAGING_SYNC_WITH_SERVER  (2)
#define  MESSAGING_SYNC_WITH_TARGET  (3)

typedef  short  RoutingType ;

#define  MESSAGING_ROUTE_NONE  (0)
#define  MESSAGING_ROUTE_FORWARD  (1)
#define  MESSAGING_ROUTE_STORE_AND_FORWARD  (2)

typedef  short  Priority ;

typedef  unsigned  short  Ordering ;

#define  MESSAGING_ORDER_ANY  (0x01)
#define  MESSAGING_ORDER_TEMPORAL  (0x02)
#define  MESSAGING_ORDER_PRIORITY  (0x04)
#define  MESSAGING_ORDER_DEADLINE  (0x08)
#define  MESSAGING_REBIND_POLICY_TYPE  (23)
#define  MESSAGING_SYNC_SCOPE_POLICY_TYPE  (24)
#define  MESSAGING_REQUEST_PRIORITY_POLICY_TYPE  (25)

typedef  struct  PriorityRange {
    Priority  min ;
    Priority  max ;
}  PriorityRange ;

#define  MESSAGING_REPLY_PRIORITY_POLICY_TYPE  (26)

typedef  IOR  ReplyPriorityPolicy ;

#define  MESSAGING_REQUEST_START_TIME_POLICY_TYPE  (27)
#define  MESSAGING_REQUEST_END_TIME_POLICY_TYPE  (28)
#define  MESSAGING_REPLY_START_TIME_POLICY_TYPE  (29)
#define  MESSAGING_REPLY_END_TIME_POLICY_TYPE  (30)
#define  MESSAGING_RELATIVE_REQ_TIMEOUT_POLICY_TYPE  (31)
#define  MESSAGING_RELATIVE_RT_TIMEOUT_POLICY_TYPE  (32)
#define  MESSAGING_ROUTING_POLICY_TYPE  (33)

typedef  struct  RoutingTypeRange {
    RoutingType  min ;
    RoutingType  max ;
}  RoutingTypeRange ;

#define  MESSAGING_MAX_HOPS_POLICY_TYPE  (34)
#define  MESSAGING_QUEUE_ORDER_POLICY_TYPE  (35)

/*===== "PolicyValue" already defined =====*/

typedef  SEQUENCE (PolicyValue, PolicyValueSeq) ;

typedef  _valuetype_  ExceptionHolder ;	/* "valuetype" */

typedef  IOR  ReplyHandler ;

typedef  _valuetype_  Poller ;	/* "valuetype" */

/*******************************************************************************
    Tables for mapping enumerated values to names and vice-versa;
    see the coliToName() and coliToNumber() functions.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  const  ColiMap  ActionRequiredLUT[]  OCD ("CosLicen") ;
#endif
#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  const  ColiMap  AnswerLUT[]  OCD ("CosLicen") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  AssociationStatusLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  AttributeModeLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  AuthenticationStatusLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  const  ColiMap  BindingTypeLUT[]  OCD ("CosNamin") ;
#endif
#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  const  ColiMap  ChallengeProtocolLUT[]  OCD ("CosLicen") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  CommunicationDirectionLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    extern  const  ColiMap  ComparisonTypeLUT[]  OCD ("CosTime") ;
#endif
#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  const  ColiMap  ContextIdDefnLUT[]  OCD ("SECIOP") ;
#endif
#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  const  ColiMap  ContextTokenTypeLUT[]  OCD ("SECIOP") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  CredentialTypeLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  DefinitionKindLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  DelegationModeLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  DelegationStateLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  const  ColiMap  ElementInvalidReasonLUT[]  OCD ("CosColle") ;
#endif
#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    extern  const  ColiMap  EventStatusLUT[]  OCD ("CosTimer") ;
#endif
#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  const  ColiMap  EvidenceTypeLUT[]  OCD ("NRServic") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  EXCEPTcompletion_statusLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  const  ColiMap  ExceptionReasonLUT[]  OCD ("CosPrope") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  exception_typeLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  const  ColiMap  FollowOptionLUT[]  OCD ("CosTradi") ;
#endif
#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  const  ColiMap  GIOPMsgTypeLUT[]  OCD ("GIOP") ;
#endif
#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  const  ColiMap  HowManyPropsLUT[]  OCD ("CosTradi") ;
#endif
#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  const  ColiMap  IteratorInvalidReasonLUT[]  OCD ("CosColle") ;
#endif
#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  const  ColiMap  ListOptionLUT[]  OCD ("CosTradi") ;
#endif
#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  const  ColiMap  LocateStatusTypeLUT[]  OCD ("GIOP") ;
#endif
#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
    extern  const  ColiMap  lock_modeLUT[]  OCD ("CosConcu") ;
#endif
#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  const  ColiMap  LowerBoundStyleLUT[]  OCD ("CosColle") ;
#endif
#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  const  ColiMap  ModeLUT[]  OCD ("CosGraph") ;
#endif
#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  const  ColiMap  NotFoundReasonLUT[]  OCD ("CosNamin") ;
#endif
#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  const  ColiMap  NRVerificationResultLUT[]  OCD ("NRServic") ;
#endif
#if !defined(IDL_MODULE_CosCompoundLifeCycle) || IDL_MODULE_CosCompoundLifeCycle
    extern  const  ColiMap  OperationLUT[]  OCD ("CosCompo") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  OperationModeLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    extern  const  ColiMap  OverlapTypeLUT[]  OCD ("CosTime") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  ParameterModeLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  PrimitiveKindLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  const  ColiMap  PropagationValueLUT[]  OCD ("CosGraph") ;
#endif
#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  const  ColiMap  PropertyModeTypeLUT[]  OCD ("CosPrope") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  QOPLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
    extern  const  ColiMap  QueryStatusLUT[]  OCD ("CosQuery") ;
#endif
#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  const  ColiMap  ReplyStatusTypeLUT[]  OCD ("GIOP") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  RequiresSupportsLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  RightsCombinatorLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  const  ColiMap  SecurityFeatureLUT[]  OCD ("Security") ;
#endif
#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  const  ColiMap  StatusLUT[]  OCD ("CosTrans") ;
#endif
#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  const  ColiMap  TCKindLUT[]  OCD ("CORBA") ;
#endif
#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    extern  const  ColiMap  TimeComparisonLUT[]  OCD ("CosTime") ;
#endif
#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    extern  const  ColiMap  TimeTypeLUT[]  OCD ("CosTimer") ;
#endif
#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  const  ColiMap  UpperBoundStyleLUT[]  OCD ("CosColle") ;
#endif
#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    extern  const  ColiMap  ValueTypeLUT[]  OCD ("CosQuery") ;
#endif
#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  const  ColiMap  VoteLUT[]  OCD ("CosTrans") ;
#endif

/*******************************************************************************
    Public functions.
*******************************************************************************/

/* Marshaling functions for the defined data types. */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxAbstractInterfaceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxAbstractInterfaceDefSeq P_((ComxChannel channel,
                                                     AbstractInterfaceDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxAccessDecision(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_SecurityAdmin) || IDL_MODULE_SecurityAdmin
#    define  gimxAccessPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  errno_t  gimxAction P_((ComxChannel channel,
                                    Action *value))
        OCD ("CosLicen") ;
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  errno_t  gimxActionRequired P_((ComxChannel channel,
                                            ActionRequired *value))
        OCD ("CosLicen") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
#    define  gimxAddressingDisposition(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxAdmin(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxAliasDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  errno_t  gimxAnswer P_((ComxChannel channel,
                                    Answer *value))
        OCD ("CosLicen") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxAnySeq P_((ComxChannel channel,
                                    AnySeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxArrayDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
#    define  gimxAssociationOptions(channel,value)  comxUShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAssociationStatus P_((ComxChannel channel,
                                               AssociationStatus *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxAS_ContextSec P_((ComxChannel channel,
                                           AS_ContextSec *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxAttrDescriptionSeq P_((ComxChannel channel,
                                                AttrDescriptionSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxAttributeDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxAttributeDescription P_((ComxChannel channel,
                                                  AttributeDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAttributeList P_((ComxChannel channel,
                                           AttributeList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxAttributeMode P_((ComxChannel channel,
                                           AttributeMode *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAttributeType P_((ComxChannel channel,
                                           AttributeType *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAttributeTypeList P_((ComxChannel channel,
                                               AttributeTypeList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxAuditChannel(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxAuditChannelId(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxAuditDecision(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAuditEventType P_((ComxChannel channel,
                                            AuditEventType *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAuditEventTypeList P_((ComxChannel channel,
                                                AuditEventTypeList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_SecurityAdmin) || IDL_MODULE_SecurityAdmin
#    define  gimxAuditPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxAuthenticationMethod(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxAuthenticationStatus P_((ComxChannel channel,
                                                  AuthenticationStatus *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxAuthorityDescriptor P_((ComxChannel channel,
                                                 AuthorityDescriptor *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxAuthorityDescriptorList P_((ComxChannel channel,
                                                     AuthorityDescriptorList *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxAuthorizationElement P_((ComxChannel channel,
                                                  AuthorizationElement *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxAuthorizationElementContents(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxAuthorizationElementType(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxAuthorizationToken P_((ComxChannel channel,
                                                AuthorizationToken *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxBag(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxBagFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_BiDirPolicy) || IDL_MODULE_BiDirPolicy
#    define  gimxBidirectionalPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_BiDirPolicy) || IDL_MODULE_BiDirPolicy
#    define  gimxBidirectionalPolicyValue(channel,value)  comxUShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_IIOP) || IDL_MODULE_IIOP
    extern  errno_t  gimxBiDirIIOPServiceContext P_((ComxChannel channel,
                                                     BiDirIIOPServiceContext *value))
        OCD ("IIOP") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  errno_t  gimxBinding P_((ComxChannel channel,
                                     Binding *value))
        OCD ("CosNamin") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
#    define  gimxBindingIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  errno_t  gimxBindingList P_((ComxChannel channel,
                                         BindingList *value))
        OCD ("CosNamin") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  errno_t  gimxBindingType P_((ComxChannel channel,
                                         BindingType *value))
        OCD ("CosNamin") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxCancelRequestHeader P_((ComxChannel channel,
                                                 CancelRequestHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  errno_t  gimxChallenge P_((ComxChannel channel,
                                       Challenge *value))
        OCD ("CosLicen") ;
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  errno_t  gimxChallengeData P_((ComxChannel channel,
                                           ChallengeData *value))
        OCD ("CosLicen") ;
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    extern  errno_t  gimxChallengeProtocol P_((ComxChannel channel,
                                               ChallengeProtocol *value))
        OCD ("CosLicen") ;
#endif

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext
#    define  gimxCodeBase(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME
    extern  errno_t  gimxCodeSetComponent P_((ComxChannel channel,
                                              CodeSetComponent *value))
        OCD ("CONV_FRA") ;
#endif

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME
    extern  errno_t  gimxCodeSetComponentInfo P_((ComxChannel channel,
                                                  CodeSetComponentInfo *value))
        OCD ("CONV_FRA") ;
#endif

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME
    extern  errno_t  gimxCodeSetContext P_((ComxChannel channel,
                                            CodeSetContext *value))
        OCD ("CONV_FRA") ;
#endif

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME
#    define  gimxCodeSetId(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxCollectionFactories(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxCollectionFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxCommand(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxCommunicationDirection P_((ComxChannel channel,
                                                    CommunicationDirection *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxComparator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    extern  errno_t  gimxComparisonType P_((ComxChannel channel,
                                            ComparisonType *value))
        OCD ("CosTime") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxCompleteEstablishContext P_((ComxChannel channel,
                                                      CompleteEstablishContext *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxComponentDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxComponentDescription P_((ComxChannel channel,
                                                  ComponentDescription *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
#    define  gimxComponentId(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxCompoundSecMech P_((ComxChannel channel,
                                             CompoundSecMech *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxCompoundSecMechanisms P_((ComxChannel channel,
                                                   CompoundSecMechanisms *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxCompoundSecMechList P_((ComxChannel channel,
                                                 CompoundSecMechList *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxConnection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxConnectionFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxConstantDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxConstantDescription P_((ComxChannel channel,
                                                 ConstantDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxConstraint(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxConstraintRecipe(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxConstructionPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxConsumerAdmin(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxConsumesDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxContained(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosContainment) || IDL_MODULE_CosContainment
#    define  gimxContainedInRole(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxContainedSeq P_((ComxChannel channel,
                                          ContainedSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxContainer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosContainment) || IDL_MODULE_CosContainment
#    define  gimxContainsRole(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxContextError P_((ComxChannel channel,
                                          ContextError *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxContextId(channel,value)  comxULongLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxContextIdDefn P_((ComxChannel channel,
                                           ContextIdDefn *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxContextIdentifier(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxContextIdSeq P_((ComxChannel channel,
                                          ContextIdSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_Dynamic) || IDL_MODULE_Dynamic
#    define  gimxContextList(channel,value)  comxStringSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxContextTokenType P_((ComxChannel channel,
                                              ContextTokenType *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxContinueEstablishContext P_((ComxChannel channel,
                                                      ContinueEstablishContext *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxControl(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxCoordinator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService
    extern  errno_t  gimxCOSLCSKeys P_((ComxChannel channel,
                                        COSLCSKeys *value))
        OCD ("LifeCycl") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxCOSSEventType(channel,value)  comxUShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxCOSTPolicy P_((ComxChannel channel,
                                        COSTPolicy *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxCOSTPolicySeq P_((ComxChannel channel,
                                           COSTPolicySeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxCredentials(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
    extern  errno_t  gimxCredentialsList P_((ComxChannel channel,
                                             CredentialsList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxCredentialType P_((ComxChannel channel,
                                            CredentialType *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
    extern  errno_t  gimxCriteria P_((ComxChannel channel,
                                      Criteria *value))
        OCD ("CosLifeC") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
#    define  gimxCryptographicProfile(channel,value)  comxUShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxCryptographicProfileList P_((ComxChannel channel,
                                                      CryptographicProfileList *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxCSequence(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxCSI_ECMA_Hybrid P_((ComxChannel channel,
                                             CSI_ECMA_Hybrid *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxCSI_ECMA_Public P_((ComxChannel channel,
                                             CSI_ECMA_Public *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxCSI_ECMA_Secret P_((ComxChannel channel,
                                             CSI_ECMA_Secret *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxCurrent(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxCursor(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxCursorFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePDS_DA) || IDL_MODULE_CosPersistencePDS_DA
#    define  gimxDAObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePDS_DA) || IDL_MODULE_CosPersistencePDS_DA
#    define  gimxDAObjectFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePDS_DA) || IDL_MODULE_CosPersistencePDS_DA
#    define  gimxDAObjectFactoryFinder(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePDS_DA) || IDL_MODULE_CosPersistencePDS_DA
#    define  gimxDAObjectID(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxDatastore_CLI(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_DCE_CIOPSecurity) || IDL_MODULE_DCE_CIOPSecurity
#    define  gimxDCEAuthorization(channel,value)  comxUShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_DCE_CIOPSecurity) || IDL_MODULE_DCE_CIOPSecurity
    extern  errno_t  gimxDCESecurityMechanismInfo P_((ComxChannel channel,
                                                      DCESecurityMechanismInfo *value))
        OCD ("DCE_CIOP") ;
#endif

#if !defined(IDL_MODULE_CosPersistenceDDO) || IDL_MODULE_CosPersistenceDDO
#    define  gimxDDO(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    extern  errno_t  gimxDecimal P_((ComxChannel channel,
                                     Decimal *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxDefinitionKind P_((ComxChannel channel,
                                            DefinitionKind *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxDelegationMode P_((ComxChannel channel,
                                            DelegationMode *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_SecurityAdmin) || IDL_MODULE_SecurityAdmin
#    define  gimxDelegationPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxDelegationState P_((ComxChannel channel,
                                             DelegationState *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxDeque(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxDequeFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxDescription P_((ComxChannel channel,
                                         Description *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxDescriptionSeq P_((ComxChannel channel,
                                            DescriptionSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxDiscardContext P_((ComxChannel channel,
                                            DiscardContext *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_SecurityAdmin) || IDL_MODULE_SecurityAdmin
#    define  gimxDomainAccessPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxDomainManager(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxDomainManagersList P_((ComxChannel channel,
                                                DomainManagersList *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
#    define  gimxDurationInMinutes(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTradingDynamic) || IDL_MODULE_CosTradingDynamic
    extern  errno_t  gimxDynamicProp P_((ComxChannel channel,
                                         DynamicProp *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTradingDynamic) || IDL_MODULE_CosTradingDynamic
#    define  gimxDynamicPropEval(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxEdge P_((ComxChannel channel,
                                  Edge *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxEdgeIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxEdges P_((ComxChannel channel,
                                   Edges *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  errno_t  gimxElementInvalidReason P_((ComxChannel channel,
                                                  ElementInvalidReason *value))
        OCD ("CosColle") ;
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxEmitsDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxEncoding P_((ComxChannel channel,
                                      Encoding *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
#    define  gimxEncodingFormat(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxEndPoint P_((ComxChannel channel,
                                      EndPoint *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxEndpointIdPositionComponent P_((ComxChannel channel,
                                                         EndpointIdPositionComponent *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxEndPoints P_((ComxChannel channel,
                                       EndPoints *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxEnumDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxEnumMemberSeq P_((ComxChannel channel,
                                           EnumMemberSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualityCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualityIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualityKeyCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualityKeyIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualityKeySortedCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualityKeySortedIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualitySequence(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualitySequenceFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualitySequentialCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualitySequentialIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualitySortedCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxEqualitySortedIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_GSSUP) || IDL_MODULE_GSSUP
#    define  gimxErrorCode(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_GSSUP) || IDL_MODULE_GSSUP
    extern  errno_t  gimxErrorToken P_((ComxChannel channel,
                                        ErrorToken *value))
        OCD ("GSSUP") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxEstablishContext P_((ComxChannel channel,
                                              EstablishContext *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxEventChannel(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxEventDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxEventPortDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxEventPortDescription P_((ComxChannel channel,
                                                  EventPortDescription *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxEventPortDescriptionSeq P_((ComxChannel channel,
                                                     EventPortDescriptionSeq *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    extern  errno_t  gimxEventStatus P_((ComxChannel channel,
                                         EventStatus *value))
        OCD ("CosTimer") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxEvidenceDescriptor P_((ComxChannel channel,
                                                EvidenceDescriptor *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxEvidenceDescriptorList P_((ComxChannel channel,
                                                    EvidenceDescriptorList *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxEvidenceType P_((ComxChannel channel,
                                          EvidenceType *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExcDescriptionSeq P_((ComxChannel channel,
                                               ExcDescriptionSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxEXCEPTcompletion_status P_((ComxChannel channel,
                                                     EXCEPTcompletion_status *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxExceptionDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExceptionDefSeq P_((ComxChannel channel,
                                             ExceptionDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExceptionDescription P_((ComxChannel channel,
                                                  ExceptionDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxExceptionHolder(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_Dynamic) || IDL_MODULE_Dynamic
    extern  errno_t  gimxExceptionList P_((ComxChannel channel,
                                           ExceptionList *value))
        OCD ("Dynamic") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxExceptionReason P_((ComxChannel channel,
                                             ExceptionReason *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxException_type P_((ComxChannel channel,
                                            exception_type *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxExtAbstractInterfaceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtAbstractInterfaceDefSeq P_((ComxChannel channel,
                                                        ExtAbstractInterfaceDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtAttrDescriptionSeq P_((ComxChannel channel,
                                                   ExtAttrDescriptionSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxExtAttributeDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtAttributeDescription P_((ComxChannel channel,
                                                     ExtAttributeDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxExtensibleFamily P_((ComxChannel channel,
                                              ExtensibleFamily *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtFullInterfaceDescription P_((ComxChannel channel,
                                                         ExtFullInterfaceDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtFullValueDescription P_((ComxChannel channel,
                                                     ExtFullValueDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtInitializer P_((ComxChannel channel,
                                            ExtInitializer *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtInitializerSeq P_((ComxChannel channel,
                                               ExtInitializerSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxExtInterfaceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtInterfaceDefSeq P_((ComxChannel channel,
                                                ExtInterfaceDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxExtLocalInterfaceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtLocalInterfaceDefSeq P_((ComxChannel channel,
                                                     ExtLocalInterfaceDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxExtValueDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxExtValueDefSeq P_((ComxChannel channel,
                                            ExtValueDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
    extern  errno_t  gimxFactories P_((ComxChannel channel,
                                       Factories *value))
        OCD ("CosLifeC") ;
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
#    define  gimxFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxFactoryDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
#    define  gimxFactoryFinder(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    extern  errno_t  gimxFieldValue P_((ComxChannel channel,
                                        FieldValue *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_CosExternalization) || IDL_MODULE_CosExternalization
#    define  gimxFileStreamFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxFinderDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxFixedDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxFollowOption P_((ComxChannel channel,
                                          FollowOption *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxFragmentHeader P_((ComxChannel channel,
                                            FragmentHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxFullInterfaceDescription P_((ComxChannel channel,
                                                      FullInterfaceDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxFullValueDescription P_((ComxChannel channel,
                                                  FullValueDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
#    define  gimxGenericFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxGenericMechanismInfo P_((ComxChannel channel,
                                                  GenericMechanismInfo *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxGIOPMsgType P_((ComxChannel channel,
                                         GIOPMsgType *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxGSSToken(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxGSS_NT_ExportedName(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxGSS_NT_ExportedNameList P_((ComxChannel channel,
                                                     GSS_NT_ExportedNameList *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxHeap(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxHeapFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxHomeDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxHomeDescription P_((ComxChannel channel,
                                             HomeDescription *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxHowManyProps P_((ComxChannel channel,
                                          HowManyProps *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosObjectIdentity) || IDL_MODULE_CosObjectIdentity
#    define  gimxIdentifiableObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxIdentifier(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxIdentityExtension(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxIdentityToken P_((ComxChannel channel,
                                           IdentityToken *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxIdentityTokenType(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxIDLType(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxImportAttributes(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase
#    define  gimxInaccuracyT(channel,value)  comxULongLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxIncarnationNumber P_((ComxChannel channel,
                                               IncarnationNumber *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_GSSUP) || IDL_MODULE_GSSUP
    extern  errno_t  gimxInitialContextToken P_((ComxChannel channel,
                                                 InitialContextToken *value))
        OCD ("GSSUP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxInitializer P_((ComxChannel channel,
                                         Initializer *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxInitializerSeq P_((ComxChannel channel,
                                            InitializerSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxInterfaceAttrExtension(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxInterfaceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxInterfaceDefs P_((ComxChannel channel,
                                           InterfaceDefs *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxInterfaceDefSeq P_((ComxChannel channel,
                                             InterfaceDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxInterfaceDescription P_((ComxChannel channel,
                                                  InterfaceDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase
    extern  errno_t  gimxIntervalT P_((ComxChannel channel,
                                       IntervalT *value))
        OCD ("TimeBase") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxInvocationCredentialsPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxIOR P_((ComxChannel channel,
                                 IOR *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxIORAddressingInfo P_((ComxChannel channel,
                                               IORAddressingInfo *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxIRObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxIstring(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  errno_t  gimxIteratorInvalidReason P_((ComxChannel channel,
                                                   IteratorInvalidReason *value))
        OCD ("CosColle") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxKerberosV5 P_((ComxChannel channel,
                                        KerberosV5 *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
    extern  errno_t  gimxKey P_((ComxChannel channel,
                                 Key *value))
        OCD ("CosLifeC") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeyBag(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeyBagFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeyCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeyIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySetFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySortedBag(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySortedBagFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySortedCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySortedIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySortedSet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxKeySortedSetFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
#    define  gimxLicenseHandle(channel,value)  gimxAny ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
#    define  gimxLicenseServiceManager(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
#    define  gimxLifeCycleObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService
#    define  gimxLifeCycleServiceAdmin(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxLink(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxLinkAttributes(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxLinkInfo P_((ComxChannel channel,
                                      LinkInfo *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxLinkName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxLinkNameSeq P_((ComxChannel channel,
                                         LinkNameSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_IIOP) || IDL_MODULE_IIOP
    extern  errno_t  gimxListenPoint P_((ComxChannel channel,
                                         ListenPoint *value))
        OCD ("IIOP") ;
#endif

#if !defined(IDL_MODULE_IIOP) || IDL_MODULE_IIOP
    extern  errno_t  gimxListenPointList P_((ComxChannel channel,
                                             ListenPointList *value))
        OCD ("IIOP") ;
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxListOption P_((ComxChannel channel,
                                        ListOption *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_gimx) || IDL_MODULE_gimx
#    define  gimxLName(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_gimx) || IDL_MODULE_gimx
#    define  gimxLNameComponent(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxLocalInterfaceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxLocalInterfaceDefSeq P_((ComxChannel channel,
                                                  LocalInterfaceDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxLocateReplyHeader P_((ComxChannel channel,
                                               LocateReplyHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxLocateReplyHeader_1_0 P_((ComxChannel channel,
                                                   LocateReplyHeader_1_0 *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxLocateRequestHeader P_((ComxChannel channel,
                                                 LocateRequestHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxLocateRequestHeader_1_0 P_((ComxChannel channel,
                                                     LocateRequestHeader_1_0 *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
#    define  gimxLocateRequestHeader_1_1(channel,value)  gimxLocateRequestHeader_1_0 ((channel), (value))
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxLocateStatusType P_((ComxChannel channel,
                                              LocateStatusType *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
#    define  gimxLockCoordinator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
#    define  gimxLockSet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
#    define  gimxLockSetFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
    extern  errno_t  gimxLock_mode P_((ComxChannel channel,
                                       lock_mode *value))
        OCD ("CosConcu") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxLookup(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  errno_t  gimxLowerBoundStyle P_((ComxChannel channel,
                                             LowerBoundStyle *value))
        OCD ("CosColle") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxMap(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxMapFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxMechandOptions P_((ComxChannel channel,
                                            MechandOptions *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxMechandOptionsList P_((ComxChannel channel,
                                                MechandOptionsList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxMechanismDescriptor P_((ComxChannel channel,
                                                 MechanismDescriptor *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxMechanismDescriptorList P_((ComxChannel channel,
                                                     MechanismDescriptorList *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxMechanismPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxMechanismType(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxMechanismTypeList P_((ComxChannel channel,
                                               MechanismTypeList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxMessageHeader P_((ComxChannel channel,
                                           MessageHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxMessageHeader_1_0 P_((ComxChannel channel,
                                               MessageHeader_1_0 *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxMessageInContext P_((ComxChannel channel,
                                              MessageInContext *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxMode P_((ComxChannel channel,
                                  Mode *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxModuleDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxModuleDescription P_((ComxChannel channel,
                                               ModuleDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxMsgType(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxMultipleComponentProfile P_((ComxChannel channel,
                                                      MultipleComponentProfile *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  errno_t  gimxName P_((ComxChannel channel,
                                  Name *value))
        OCD ("CosNamin") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  errno_t  gimxNameComponent P_((ComxChannel channel,
                                           NameComponent *value))
        OCD ("CosNamin") ;
#endif

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService
    extern  errno_t  gimxNameComponents P_((ComxChannel channel,
                                            NameComponents *value))
        OCD ("LifeCycl") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxNamedRole P_((ComxChannel channel,
                                       NamedRole *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxNamedRoles P_((ComxChannel channel,
                                        NamedRoles *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxNamedRoleType P_((ComxChannel channel,
                                           NamedRoleType *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxNamedRoleTypes P_((ComxChannel channel,
                                            NamedRoleTypes *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle
    extern  errno_t  gimxNameValuePair P_((ComxChannel channel,
                                           NameValuePair *value))
        OCD ("CosLifeC") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
#    define  gimxNamingContext(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxNativeDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxNode(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxNodeFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxNodeHandle P_((ComxChannel channel,
                                        NodeHandle *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxNodeHandles P_((ComxChannel channel,
                                         NodeHandles *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    extern  errno_t  gimxNotFoundReason P_((ComxChannel channel,
                                            NotFoundReason *value))
        OCD ("CosNamin") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
#    define  gimxNRCredentials(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
#    define  gimxNRMech(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
#    define  gimxNRPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxNRPolicyFeatures P_((ComxChannel channel,
                                              NRPolicyFeatures *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxNRPolicyFeaturesList P_((ComxChannel channel,
                                                  NRPolicyFeaturesList *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
#    define  gimxNRPolicyId(channel,value)  gimxExtensibleFamily ((channel), (value))
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxNRVerificationResult P_((ComxChannel channel,
                                                  NRVerificationResult *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
#    define  gimxNull(channel,value)  comxBoolean ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  errno_t  gimxNVPair P_((ComxChannel channel,
                                    NVPair *value))
        OCD ("CosColle") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
#    define  gimxObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosObjectIdentity) || IDL_MODULE_CosObjectIdentity
#    define  gimxObjectIdentifier(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxOffer P_((ComxChannel channel,
                                   Offer *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxOfferId(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxOfferIdIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxOfferIdSeq P_((ComxChannel channel,
                                        OfferIdSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxOfferInfo P_((ComxChannel channel,
                                       OfferInfo *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxOfferIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxOfferSeq P_((ComxChannel channel,
                                      OfferSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxOID(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxOIDList P_((ComxChannel channel,
                                     OIDList *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxOpaque(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxOpDescriptionSeq P_((ComxChannel channel,
                                              OpDescriptionSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosCompoundLifeCycle) || IDL_MODULE_CosCompoundLifeCycle
    extern  errno_t  gimxOperation P_((ComxChannel channel,
                                       Operation *value))
        OCD ("CosCompo") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxOperationDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxOperationDescription P_((ComxChannel channel,
                                                  OperationDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxOperationMode P_((ComxChannel channel,
                                           OperationMode *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxOperations(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCompoundLifeCycle) || IDL_MODULE_CosCompoundLifeCycle
#    define  gimxOperationsFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxOptionsDirectionPair P_((ComxChannel channel,
                                                  OptionsDirectionPair *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxOptionsDirectionPairList P_((ComxChannel channel,
                                                      OptionsDirectionPairList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxOQL(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxOQLBasic(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxOQL_93(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxOQL_93Basic(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxOrderedCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxOrderedIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxOrdering(channel,value)  comxUShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  errno_t  gimxOtid_t P_((ComxChannel channel,
                                    otid_t *value))
        OCD ("CosTrans") ;
#endif

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    extern  errno_t  gimxOverlapType P_((ComxChannel channel,
                                         OverlapType *value))
        OCD ("CosTime") ;
#endif

#if !defined(IDL_MODULE_Dynamic) || IDL_MODULE_Dynamic
    extern  errno_t  gimxParameter P_((ComxChannel channel,
                                       Parameter *value))
        OCD ("Dynamic") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxParameterDescription P_((ComxChannel channel,
                                                  ParameterDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  errno_t  gimxParameterList P_((ComxChannel channel,
                                           ParameterList *value))
        OCD ("CosColle") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxParameterMode P_((ComxChannel channel,
                                           ParameterMode *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxParDescriptionSeq P_((ComxChannel channel,
                                               ParDescriptionSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosPersistencePDS) || IDL_MODULE_CosPersistencePDS
#    define  gimxPDS(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePDS_DA) || IDL_MODULE_CosPersistencePDS_DA
#    define  gimxPDS_DA(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePID) || IDL_MODULE_CosPersistencePID
#    define  gimxPID(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxPID_CLI(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePDS_DA) || IDL_MODULE_CosPersistencePDS_DA
#    define  gimxPID_DA(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePO) || IDL_MODULE_CosPersistencePO
#    define  gimxPO(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxPolicyErrorCode(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxPolicyList P_((ComxChannel channel,
                                        PolicyList *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxPolicyName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxPolicyNameSeq P_((ComxChannel channel,
                                           PolicyNameSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxPolicyType(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxPolicyTypeSeq P_((ComxChannel channel,
                                           PolicyTypeSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxPolicyValue(channel,value)  gimxAny ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
    extern  errno_t  gimxPolicyValueSeq P_((ComxChannel channel,
                                            PolicyValueSeq *value))
        OCD ("Messagin") ;
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxPoller(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPersistencePOM) || IDL_MODULE_CosPersistencePOM
#    define  gimxPOM(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxPreference(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxPrimitiveDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxPrimitiveKind P_((ComxChannel channel,
                                           PrimitiveKind *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
#    define  gimxPrincipal(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxPrincipalAuthenticator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxPriority(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxPriorityQueue(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxPriorityQueueFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
    extern  errno_t  gimxPriorityRange P_((ComxChannel channel,
                                           PriorityRange *value))
        OCD ("Messagin") ;
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
#    define  gimxProducerSpecificLicenseService(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
#    define  gimxProducerSpecificNotification(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
#    define  gimxProfileId(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  errno_t  gimxPropagationContext P_((ComxChannel channel,
                                                PropagationContext *value))
        OCD ("CosTrans") ;
#endif

#if !defined(IDL_MODULE_CosStream) || IDL_MODULE_CosStream
#    define  gimxPropagationCriteriaFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxPropagationValue P_((ComxChannel channel,
                                              PropagationValue *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxProperties P_((ComxChannel channel,
                                        Properties *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertiesIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxProperty P_((ComxChannel channel,
                                      Property *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyDef P_((ComxChannel channel,
                                         PropertyDef *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyDefs P_((ComxChannel channel,
                                          PropertyDefs *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyException P_((ComxChannel channel,
                                               PropertyException *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyExceptions P_((ComxChannel channel,
                                                PropertyExceptions *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService
    extern  errno_t  gimxPropertyList P_((ComxChannel channel,
                                          PropertyList *value))
        OCD ("LifeCycl") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyMode P_((ComxChannel channel,
                                          PropertyMode *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyModes P_((ComxChannel channel,
                                           PropertyModes *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyModeType P_((ComxChannel channel,
                                              PropertyModeType *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertyName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyNames P_((ComxChannel channel,
                                           PropertyNames *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxPropertyNameSeq P_((ComxChannel channel,
                                             PropertyNameSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertyNamesIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxPropertySeq P_((ComxChannel channel,
                                         PropertySeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertySet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertySetDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertySetDefFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
#    define  gimxPropertySetFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    extern  errno_t  gimxPropertyTypes P_((ComxChannel channel,
                                           PropertyTypes *value))
        OCD ("CosPrope") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxPropertyValue(channel,value)  gimxAny ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxPropStruct P_((ComxChannel channel,
                                        PropStruct *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxPropStructSeq P_((ComxChannel channel,
                                           PropStructSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxProvidesDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxProvidesDescription P_((ComxChannel channel,
                                                 ProvidesDescription *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxProvidesDescriptionSeq P_((ComxChannel channel,
                                                    ProvidesDescriptionSeq *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxProxy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxProxyInfo P_((ComxChannel channel,
                                       ProxyInfo *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxProxyPullConsumer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxProxyPullSupplier(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxProxyPushConsumer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxProxyPushSupplier(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxPublishesDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventComm) || IDL_MODULE_CosEventComm
#    define  gimxPullConsumer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventComm) || IDL_MODULE_CosEventComm
#    define  gimxPullSupplier(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventComm) || IDL_MODULE_CosEventComm
#    define  gimxPushConsumer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventComm) || IDL_MODULE_CosEventComm
#    define  gimxPushSupplier(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxQLType(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
    extern  errno_t  gimxQLTypes P_((ComxChannel channel,
                                     QLTypes *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxQOP P_((ComxChannel channel,
                                 QOP *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxQOPPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxQuery(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxQueryableCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxQueryEvaluator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxQueryLanguageType(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxQueryManager(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
    extern  errno_t  gimxQueryStatus P_((ComxChannel channel,
                                         QueryStatus *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxQueue(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxQueueFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxRACollectionFactories(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxRACollectionFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxRebindMode(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTSPortability) || IDL_MODULE_CosTSPortability
#    define  gimxReceiver(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    extern  errno_t  gimxRecord P_((ComxChannel channel,
                                    Record *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxRecoveryCoordinator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosReference) || IDL_MODULE_CosReference
#    define  gimxReferencedByRole(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosReference) || IDL_MODULE_CosReference
#    define  gimxReferencesRole(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxRegister(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRelatedObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxRelation(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxRelationFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRelationship(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRelationshipFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxRelationshipHandle P_((ComxChannel channel,
                                                RelationshipHandle *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxRelationshipHandles P_((ComxChannel channel,
                                                 RelationshipHandles *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRelationshipIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxReplyHandler(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxReplyHeader P_((ComxChannel channel,
                                         ReplyHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxReplyHeader_1_0 P_((ComxChannel channel,
                                             ReplyHeader_1_0 *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxReplyPriorityPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxReplyStatusType P_((ComxChannel channel,
                                             ReplyStatusType *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxRepository(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxRepositoryId(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxRepositoryIdSeq P_((ComxChannel channel,
                                             RepositoryIdSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosTSPortability) || IDL_MODULE_CosTSPortability
#    define  gimxReqId(channel,value)  comxLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_Dynamic) || IDL_MODULE_Dynamic
#    define  gimxRequestContext(channel,value)  comxStringSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    extern  errno_t  gimxRequestFeatures P_((ComxChannel channel,
                                             RequestFeatures *value))
        OCD ("NRServic") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxRequestHeader P_((ComxChannel channel,
                                           RequestHeader *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxRequestHeader_1_0 P_((ComxChannel channel,
                                               RequestHeader_1_0 *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxRequestHeader_1_1 P_((ComxChannel channel,
                                               RequestHeader_1_1 *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxRequiredRights(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxRequiresSupports P_((ComxChannel channel,
                                              RequiresSupports *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxResource(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxRestrictedAccessCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxRight P_((ComxChannel channel,
                                   Right *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxRightsCombinator P_((ComxChannel channel,
                                              RightsCombinator *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxRightsList P_((ComxChannel channel,
                                        RightsList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRole(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRoleFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
#    define  gimxRoleName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxRoleNames P_((ComxChannel channel,
                                       RoleNames *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships
    extern  errno_t  gimxRoles P_((ComxChannel channel,
                                   Roles *value))
        OCD ("CosRelat") ;
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxRoutingType(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
    extern  errno_t  gimxRoutingTypeRange P_((ComxChannel channel,
                                              RoutingTypeRange *value))
        OCD ("Messagin") ;
#endif

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext
#    define  gimxRunTime(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
    extern  errno_t  gimxSASContextBody P_((ComxChannel channel,
                                            SASContextBody *value))
        OCD ("CSI") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxSAS_ContextSec P_((ComxChannel channel,
                                            SAS_ContextSec *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxScopedEdge P_((ComxChannel channel,
                                        ScopedEdge *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxScopedEdges P_((ComxChannel channel,
                                         ScopedEdges *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxScopedEndPoint P_((ComxChannel channel,
                                            ScopedEndPoint *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxScopedEndPoints P_((ComxChannel channel,
                                             ScopedEndPoints *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxScopedName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxScopedRelationship P_((ComxChannel channel,
                                                ScopedRelationship *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosPersistencePO) || IDL_MODULE_CosPersistencePO
#    define  gimxSD(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSecAttribute P_((ComxChannel channel,
                                          SecAttribute *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxSECIOPMessageError P_((ComxChannel channel,
                                                SECIOPMessageError *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxSECIOP_SEC_TRANS P_((ComxChannel channel,
                                              SECIOP_SEC_TRANS *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_SecurityAdmin) || IDL_MODULE_SecurityAdmin
#    define  gimxSecureInvocationPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxSecurityAttributeType(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_SecurityReplaceable) || IDL_MODULE_SecurityReplaceable
#    define  gimxSecurityContext(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSecurityFeature P_((ComxChannel channel,
                                             SecurityFeature *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2
#    define  gimxSecurityFeaturesPolicy(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSecurityFeatureValue P_((ComxChannel channel,
                                                  SecurityFeatureValue *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSecurityFeatureValueList P_((ComxChannel channel,
                                                      SecurityFeatureValueList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSecurityMechandName P_((ComxChannel channel,
                                                 SecurityMechandName *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSecurityMechandNameList P_((ComxChannel channel,
                                                     SecurityMechandNameList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxSecurityName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
#    define  gimxSelectorType(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSelectorValue P_((ComxChannel channel,
                                           SelectorValue *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    extern  errno_t  gimxSelectorValueList P_((ComxChannel channel,
                                               SelectorValueList *value))
        OCD ("Security") ;
#endif

#if !defined(IDL_MODULE_CosTSPortability) || IDL_MODULE_CosTSPortability
#    define  gimxSender(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxSequenceDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSequenceFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxSequencingHeader P_((ComxChannel channel,
                                              SequencingHeader *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSequentialCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSequentialIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxServiceConfiguration P_((ComxChannel channel,
                                                  ServiceConfiguration *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxServiceConfigurationList P_((ComxChannel channel,
                                                      ServiceConfigurationList *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
#    define  gimxServiceConfigurationSyntax(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxServiceContext P_((ComxChannel channel,
                                            ServiceContext *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxServiceContextList P_((ComxChannel channel,
                                                ServiceContextList *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
#    define  gimxServiceId(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
#    define  gimxServiceSpecificName(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxServiceTypeName(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxServiceTypeNameSeq P_((ComxChannel channel,
                                                ServiceTypeNameSeq *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
#    define  gimxServiceTypeRepository(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSetFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedBag(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedBagFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedCollection(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedIterator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedMap(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedMapFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedRelation(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedRelationFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedSet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxSortedSetFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxSpecifiedProps P_((ComxChannel channel,
                                            SpecifiedProps *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxSpecifiedServiceTypes P_((ComxChannel channel,
                                                   SpecifiedServiceTypes *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxSPKM_1 P_((ComxChannel channel,
                                    SPKM_1 *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxSPKM_2 P_((ComxChannel channel,
                                    SPKM_2 *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxSQLQuery(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
#    define  gimxSQL_92Query(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_SSLIOP) || IDL_MODULE_SSLIOP
    extern  errno_t  gimxSSL P_((ComxChannel channel,
                                 SSL *value))
        OCD ("SSLIOP") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxStack(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
#    define  gimxStackFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  errno_t  gimxStatus P_((ComxChannel channel,
                                    Status *value))
        OCD ("CosTrans") ;
#endif

#if !defined(IDL_MODULE_CosExternalization) || IDL_MODULE_CosExternalization
#    define  gimxStream(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosStream) || IDL_MODULE_CosStream
#    define  gimxStreamable(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosStream) || IDL_MODULE_CosStream
#    define  gimxStreamableFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosExternalization) || IDL_MODULE_CosExternalization
#    define  gimxStreamFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosStream) || IDL_MODULE_CosStream
#    define  gimxStreamIO(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxStringDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxStringOID(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxStructDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxStructMember P_((ComxChannel channel,
                                          StructMember *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxStructMemberSeq P_((ComxChannel channel,
                                             StructMemberSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxSubtransactionAwareResource(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosEventChannelAdmin) || IDL_MODULE_CosEventChannelAdmin
#    define  gimxSupplierAdmin(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxSupportAttributes(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxSynchronization(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging
#    define  gimxSyncScope(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxSystemExceptionReplyBody P_((ComxChannel channel,
                                                      SystemExceptionReplyBody *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP
    extern  errno_t  gimxTaggedComponent P_((ComxChannel channel,
                                             TaggedComponent *value))
        OCD ("IOP") ;
#endif

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP
    extern  errno_t  gimxTargetAddress P_((ComxChannel channel,
                                           TargetAddress *value))
        OCD ("GIOP") ;
#endif

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    extern  errno_t  gimxTargetAssociationOptions P_((ComxChannel channel,
                                                      TargetAssociationOptions *value))
        OCD ("SECIOP") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxTCKind P_((ComxChannel channel,
                                    TCKind *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase
#    define  gimxTdfT(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxTerminator(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    extern  errno_t  gimxTimeComparison P_((ComxChannel channel,
                                            TimeComparison *value))
        OCD ("CosTime") ;
#endif

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
#    define  gimxTimeOffsetInMinutes(channel,value)  comxLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
#    define  gimxTimerEventHandler(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
#    define  gimxTimerEventService(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    extern  errno_t  gimxTimerEventT P_((ComxChannel channel,
                                         TimerEventT *value))
        OCD ("CosTimer") ;
#endif

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
#    define  gimxTimeService(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase
#    define  gimxTimeT(channel,value)  comxULongLong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    extern  errno_t  gimxTimeType P_((ComxChannel channel,
                                      TimeType *value))
        OCD ("CosTimer") ;
#endif

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
#    define  gimxTIO(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxTLS_SEC_TRANS P_((ComxChannel channel,
                                           TLS_SEC_TRANS *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxTraderComponents(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    extern  errno_t  gimxTraderName P_((ComxChannel channel,
                                        TraderName *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
#    define  gimxTransactionalLockSet(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxTransactionalObject(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
#    define  gimxTransactionFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  errno_t  gimxTransIdentity P_((ComxChannel channel,
                                           TransIdentity *value))
        OCD ("CosTrans") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxTransportAddress P_((ComxChannel channel,
                                              TransportAddress *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP
    extern  errno_t  gimxTransportAddressList P_((ComxChannel channel,
                                                  TransportAddressList *value))
        OCD ("CSIIOP") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxTraversal(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxTraversalCriteria(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxTraversalFactory(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
#    define  gimxTraversalScopedId(channel,value)  comxULong ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxTypeCode(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTypedEventChannelAdmin) || IDL_MODULE_CosTypedEventChannelAdmin
#    define  gimxTypedConsumerAdmin(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxTypedefDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxTypeDescription P_((ComxChannel channel,
                                             TypeDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosTypedEventChannelAdmin) || IDL_MODULE_CosTypedEventChannelAdmin
#    define  gimxTypedEventChannel(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTypedEventChannelAdmin) || IDL_MODULE_CosTypedEventChannelAdmin
#    define  gimxTypedProxyPullSupplier(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTypedEventChannelAdmin) || IDL_MODULE_CosTypedEventChannelAdmin
#    define  gimxTypedProxyPushConsumer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTypedEventComm) || IDL_MODULE_CosTypedEventComm
#    define  gimxTypedPullSupplier(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTypedEventComm) || IDL_MODULE_CosTypedEventComm
#    define  gimxTypedPushConsumer(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTypedEventChannelAdmin) || IDL_MODULE_CosTypedEventChannelAdmin
#    define  gimxTypedSupplierAdmin(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
#    define  gimxTypeRepository(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    extern  errno_t  gimxTypeStruct P_((ComxChannel channel,
                                        TypeStruct *value))
        OCD ("CosTradi") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxUnionDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxUnionMember P_((ComxChannel channel,
                                         UnionMember *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxUnionMemberSeq P_((ComxChannel channel,
                                            UnionMemberSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    extern  errno_t  gimxUpperBoundStyle P_((ComxChannel channel,
                                             UpperBoundStyle *value))
        OCD ("CosColle") ;
#endif

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext
#    define  gimxURL(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext
    extern  errno_t  gimxURLSeq P_((ComxChannel channel,
                                    URLSeq *value))
        OCD ("SendingC") ;
#endif

#if !defined(IDL_MODULE_CosPersistenceDS_CLI) || IDL_MODULE_CosPersistenceDS_CLI
#    define  gimxUserEnvironment(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
#    define  gimxUsesDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxUsesDescription P_((ComxChannel channel,
                                             UsesDescription *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR
    extern  errno_t  gimxUsesDescriptionSeq P_((ComxChannel channel,
                                                UsesDescriptionSeq *value))
        OCD ("Componen") ;
#endif

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase
    extern  errno_t  gimxUtcT P_((ComxChannel channel,
                                  UtcT *value))
        OCD ("TimeBase") ;
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxUTF8String(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
#    define  gimxUTO(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    extern  errno_t  gimxValue P_((ComxChannel channel,
                                   Value *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxValueBoxDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxValueDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxValueDefSeq P_((ComxChannel channel,
                                         ValueDefSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxValueDescription P_((ComxChannel channel,
                                              ValueDescription *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext
    extern  errno_t  gimxValueDescSeq P_((ComxChannel channel,
                                          ValueDescSeq *value))
        OCD ("SendingC") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxValueMember P_((ComxChannel channel,
                                         ValueMember *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxValueMemberDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    extern  errno_t  gimxValueMemberSeq P_((ComxChannel channel,
                                            ValueMemberSeq *value))
        OCD ("CORBA") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxValueModifier(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    extern  errno_t  gimxValueType P_((ComxChannel channel,
                                       ValueType *value))
        OCD ("CosQuery") ;
#endif

#if !defined(IDL_MODULE_SecurityReplaceable) || IDL_MODULE_SecurityReplaceable
#    define  gimxVault(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxVersionSpec(channel,value)  comxString ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxVisibility(channel,value)  comxShort ((channel), (value))
#endif

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    extern  errno_t  gimxVote P_((ComxChannel channel,
                                  Vote *value))
        OCD ("CosTrans") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxWeightedEdge P_((ComxChannel channel,
                                          WeightedEdge *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    extern  errno_t  gimxWeightedEdges P_((ComxChannel channel,
                                           WeightedEdges *value))
        OCD ("CosGraph") ;
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimxWstringDef(channel,value)  gimxIOR ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxX501DistinguishedName(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI
#    define  gimxX509CertificateChain(channel,value)  comxOctetSeq ((channel), (value))
#endif

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
#    define  gimx_valuetype_(channel,value)  comxOctetSeq ((channel), (value))
#endif
