# $Id: gimx_pre.py,v 1.7 2021/09/15 04:15:43 alex Exp $
#


global gAliasMap, gCDRMap, gCustomTypes
global gEnumModuleMap, gEnumValueMap, gNewEnums
global gHookMap, gMarshalMap, gRenameMap
global gTypeModuleMap


#    Use type name "Any" in place of "any".  The CORBA specification
#    uses "Any" in its text, but "any" in its IDL as the type name.
#    (The type is not actually defined in the IDL; its name is used
#    as the base-type in "typedef"s, for the types of fields in
#    structures, etc.)

gCDRMap["any"]		= "Any"		# Not really a CDR primitive type!

#    Custom data types (i.e., not automatically generated).

gCustomTypes = set (["Any", "ObjectKey", "ProfileBody",
                     "TaggedProfile", "Timeval", "TimevalSeq"])

gMarshalMap["ObjectKey"]		= "gimxObjectKey"
gMarshalMap["ProfileBody"]		= "gimxProfileBody"
gMarshalMap["TaggedProfile"]		= "gimxTaggedProfile"

gTypeModuleMap["Timeval"]		= "UNIX"
addAlias ("Timeval", "struct  timeval")
gMarshalMap["struct  timeval"]		= "gimxTimeval"

gTypeModuleMap["TimevalSeq"]		= "UNIX"
addAlias ("sequence<Timeval>", "TimevalSeq")
addAlias ("sequence<struct  timeval>", "TimevalSeq")
gMarshalMap["TimevalSeq"]		= "gimxTimevalSeq"

#    Ignore the following two enumeration types defined in
#    "CORBA_StandardExceptions.idl".  One of the compilers I use has trouble
#    with the "completion_status" type and a field of the same name in the
#    "GIOP::SystemExceptionReplyBody" structure.  The "exception_type"
#    enumerations conflict with the "GIOP::ReplyStatusType" enumerations.

gRenameMap["CORBA::completion_status"]  = "EXCEPTcompletion_status"
gEnumValueMap["CORBA::exception_type"]	= "^EXCEPT_"
# gRenameMap["GIOP::NO_EXCEPTION"]  = "GIOP_NO_EXCEPTION"
# gRenameMap["GIOP::USER_EXCEPTION"]  = "GIOP_USER_EXCEPTION"
# gRenameMap["GIOP::SYSTEM_EXCEPTION"]  = "GIOP_SYSTEM_EXCEPTION"
# gRenameMap["GIOP::LOCATION_FORWARD"]  = "GIOP_LOCATION_FORWARD"
# gRenameMap["GIOP::LOCATION_FORWARD_PERM"]  = "GIOP_LOCATION_FORWARD_PERM"
# gRenameMap["GIOP::NEEDS_ADDRESSING_MODE"]  = "GIOP_NEEDS_ADDRESSING_MODE"

#    Rename the "MessageError" structure defined in "CORBAservices/SECIOP.idl";
#    it conflicts with an enumerated value of the same name in the
#    "GIOP::MsgType_1_1" enumeration.

gRenameMap["SECIOP::MessageError"]	= "SECIOPMessageError"

# addAlias ("MessageError", "SECIOPMessageError")
# gHookMap["SECIOP::MessageError"]	= "ignore"
# gTypeModuleMap["MessageError"]	= None

#    Rename "Keys" in "CORBAservices/LifeCycleService.idl"; it conflicts
#    with a Nintendo DS definition in "libnds/include/nds/arm9/keyboard.h".

gRenameMap["LifeCycleService::Keys"]	= "COSLCSKeys"

#    Rename "EventType" in "CORBAservices/Security.idl"; it conflicts
#    with a PalmOS definition in "Core/UI/Event.h".

gRenameMap["Security::EventType"]	= "COSSEventType"

#    Rename some CosTrading types and enumerations whose names conflict
#    with core CORBA names.

gRenameMap["CosTrading::Policy"]	= "COSTPolicy"
gRenameMap["CosTrading::PolicySeq"]	= "COSTPolicySeq"
gRenameMap["HowManyProps::none"]	= "props_none"
gRenameMap["HowManyProps::some"]	= "props_some"
gRenameMap["HowManyProps::all"]		= "props_all"

#    Rename the latest versions of version-specific GIOP enumerated types
#    so that they have version-less type names.  Skip the earlier versions
#    of the types to prevent conflicting enumeration value names.

gEnumModuleMap["LocateStatusType"]		= "GIOP"
gTypeModuleMap["LocateStatusType"]		= "GIOP"
gNewEnums.add ("LocateStatusType")
gRenameMap["GIOP::LocateStatusType_1_0"]	= "LocateStatusType"
gRenameMap["GIOP::LocateStatusType_1_2"]	= "LocateStatusType"

gEnumModuleMap["GIOPMsgType"]			= "GIOP"
gTypeModuleMap["GIOPMsgType"]			= "GIOP"
gNewEnums.add ("GIOPMsgType")
gRenameMap["GIOP::MsgType_1_0"]			= "GIOPMsgType"
gRenameMap["GIOP::MsgType_1_1"]			= "GIOPMsgType"
gRenameMap["GIOP::MsgType_1_2"]			= "GIOPMsgType"
gRenameMap["GIOP::MsgType_1_3"]			= "GIOPMsgType"

gEnumModuleMap["ReplyStatusType"]		= "GIOP"
gTypeModuleMap["ReplyStatusType"]		= "GIOP"
gNewEnums.add ("ReplyStatusType")
gRenameMap["GIOP::ReplyStatusType_1_0"]		= "ReplyStatusType"
gRenameMap["GIOP::ReplyStatusType_1_1"]		= "ReplyStatusType"
gRenameMap["GIOP::ReplyStatusType_1_2"]		= "ReplyStatusType"

#    Rename the latest versions of version-specific GIOP/IIOP structure types
#    so that they have version-less type names.  The earlier versions of the
#    types will be processed, with the version numbers remaining in their names.

gRenameMap["GIOP::FragmentHeader_1_2"]		= "FragmentHeader"
gRenameMap["GIOP::FragmentHeader_1_3"]		= "FragmentHeader"

gRenameMap["GIOP::LocateReplyHeader_1_2"]	= "LocateReplyHeader"
gRenameMap["GIOP::LocateReplyHeader_1_3"]	= "LocateReplyHeader"

gRenameMap["GIOP::LocateRequestHeader_1_2"]	= "LocateRequestHeader"
gRenameMap["GIOP::LocateRequestHeader_1_3"]	= "LocateRequestHeader"

gRenameMap["GIOP::MessageHeader_1_1"]		= "MessageHeader"
gRenameMap["GIOP::MessageHeader_1_2"]		= "MessageHeader"
gRenameMap["GIOP::MessageHeader_1_3"]		= "MessageHeader"

gRenameMap["IIOP::ProfileBody_1_0"]		= "ProfileBody"
gRenameMap["IIOP::ProfileBody_1_1"]		= "ProfileBody"

gRenameMap["GIOP::ReplyHeader_1_2"]		= "ReplyHeader"
gRenameMap["GIOP::ReplyHeader_1_3"]		= "ReplyHeader"

gRenameMap["GIOP::RequestHeader_1_2"]		= "RequestHeader"
gRenameMap["GIOP::RequestHeader_1_3"]		= "RequestHeader"

#    Rename constants that conflict with CORBA or TAO definitions.

gRenameMap["CORBA::OMGVMCID"] = "CORBA_OMGVMCID"

gRenameMap["CORBA::BAD_POLICY"] = "CORBA_BAD_POLICY"
gRenameMap["CORBA::UNSUPPORTED_POLICY"] = "CORBA_UNSUPPORTED_POLICY"
gRenameMap["CORBA::BAD_POLICY_TYPE"] = "CORBA_BAD_POLICY_TYPE"
gRenameMap["CORBA::BAD_POLICY_VALUE"] = "CORBA_BAD_POLICY_VALUE"
gRenameMap["CORBA::UNSUPPORTED_POLICY_VALUE"] = "CORBA_UNSUPPORTED_POLICY_VALUE"

gRenameMap["CORBA::SecConstruction"] = "CORBA_SecConstruction"

gRenameMap["CORBA::PRIVATE_MEMBER"] = "CORBA_PRIVATE_MEMBER"
gRenameMap["CORBA::PUBLIC_MEMBER"] = "CORBA_PUBLIC_MEMBER"

gRenameMap["CORBA::VM_NONE"] = "CORBA_VM_NONE"
gRenameMap["CORBA::VM_CUSTOM"] = "CORBA_VM_CUSTOM"
gRenameMap["CORBA::VM_ABSTRACT"] = "CORBA_VM_ABSTRACT"
gRenameMap["CORBA::VM_TRUNCATABLE"] = "CORBA_VM_TRUNCATABLE"

gRenameMap["IOP::TAG_INTERNET_IOP"] = "IOP_TAG_INTERNET_IOP"
gRenameMap["IOP::TAG_MULTIPLE_COMPONENTS"] = "IOP_TAG_MULTIPLE_COMPONENTS"
gRenameMap["IOP::TAG_SCCP_IOP"] = "IOP_TAG_SCCP_IOP"

gRenameMap["IOP::TAG_ORB_TYPE"] = "IOP_TAG_ORB_TYPE"
gRenameMap["IOP::TAG_CODE_SETS"] = "IOP_TAG_CODE_SETS"
gRenameMap["IOP::TAG_POLICIES"] = "IOP_TAG_POLICIES"
gRenameMap["IOP::TAG_ALTERNATE_IIOP_ADDRESS"] = "IOP_TAG_ALTERNATE_IIOP_ADDRESS"
gRenameMap["IOP::TAG_ASSOCIATION_OPTIONS"] = "IOP_TAG_ASSOCIATION_OPTIONS"
gRenameMap["IOP::TAG_SEC_NAME"] = "IOP_TAG_SEC_NAME"
gRenameMap["IOP::TAG_SPKM_1_SEC_MECH"] = "IOP_TAG_SPKM_1_SEC_MECH"
gRenameMap["IOP::TAG_SPKM_2_SEC_MECH"] = "IOP_TAG_SPKM_2_SEC_MECH"
gRenameMap["IOP::TAG_KerberosV5_SEC_MECH"] = "IOP_TAG_KerberosV5_SEC_MECH"
gRenameMap["IOP::TAG_CSI_ECMA_Secret_SEC_MECH"] = "IOP_TAG_CSI_ECMA_Secret_SEC_MECH"
gRenameMap["IOP::TAG_CSI_ECMA_Hybrid_SEC_MECH"] = "IOP_TAG_CSI_ECMA_Hybrid_SEC_MECH"
gRenameMap["IOP::TAG_SSL_SEC_TRANS"] = "IOP_TAG_SSL_SEC_TRANS"
gRenameMap["IOP::TAG_CSI_ECMA_Public_SEC_MECH"] = "IOP_TAG_CSI_ECMA_Public_SEC_MECH"
gRenameMap["IOP::TAG_GENERIC_SEC_MECH"] = "IOP_TAG_GENERIC_SEC_MECH"
gRenameMap["IOP::TAG_FIREWALL_TRANS"] = "IOP_TAG_FIREWALL_TRANS"
gRenameMap["IOP::TAG_SCCP_CONTACT_INFO"] = "IOP_TAG_SCCP_CONTACT_INFO"
gRenameMap["IOP::TAG_JAVA_CODEBASE"] = "IOP_TAG_JAVA_CODEBASE"
gRenameMap["IOP::TAG_TRANSACTION_POLICY"] = "IOP_TAG_TRANSACTION_POLICY"
gRenameMap["IOP::TAG_MESSAGE_ROUTER"] = "IOP_TAG_MESSAGE_ROUTER"
gRenameMap["IOP::TAG_OTS_POLICY"] = "IOP_TAG_OTS_POLICY"
gRenameMap["IOP::TAG_INV_POLICY"] = "IOP_TAG_INV_POLICY"
gRenameMap["IOP::TAG_CSI_SEC_MECH_LIST"] = "IOP_TAG_CSI_SEC_MECH_LIST"
gRenameMap["IOP::TAG_NULL_TAG"] = "IOP_TAG_NULL_TAG"
gRenameMap["IOP::TAG_SECIOP_SEC_TRANS"] = "IOP_TAG_SECIOP_SEC_TRANS"
gRenameMap["IOP::TAG_TLS_SEC_TRANS"] = "IOP_TAG_TLS_SEC_TRANS"
gRenameMap["IOP::TAG_ACTIVITY_POLICY"] = "IOP_TAG_ACTIVITY_POLICY"
gRenameMap["IOP::TAG_COMPLETE_OBJECT_KEY"] = "IOP_TAG_COMPLETE_OBJECT_KEY"
gRenameMap["IOP::TAG_ENDPOINT_ID_POSITION"] = "IOP_TAG_ENDPOINT_ID_POSITION"
gRenameMap["IOP::TAG_LOCATION_POLICY"] = "IOP_TAG_LOCATION_POLICY"
gRenameMap["IOP::TAG_DCE_STRING_BINDING"] = "IOP_TAG_DCE_STRING_BINDING"
gRenameMap["IOP::TAG_DCE_BINDING_NAME"] = "IOP_TAG_DCE_BINDING_NAME"
gRenameMap["IOP::TAG_DCE_NO_PIPES"] = "IOP_TAG_DCE_NO_PIPES"
gRenameMap["IOP::TAG_DCE_SEC_MECH"] = "IOP_TAG_DCE_SEC_MECH"
gRenameMap["IOP::TAG_INET_SEC_TRANS"] = "IOP_TAG_INET_SEC_TRANS"

gRenameMap["IOP::TransactionService"] = "IOP_TransactionService"
gRenameMap["IOP::CodeSets"] = "IOP_CodeSets"
gRenameMap["IOP::ChainBypassCheck"] = "IOP_ChainBypassCheck"
gRenameMap["IOP::ChainBypassInfo"] = "IOP_ChainBypassInfo"
gRenameMap["IOP::LogicalThreadId"] = "IOP_LogicalThreadId"
gRenameMap["IOP::BI_DIR_IIOP"] = "IOP_BI_DIR_IIOP"
gRenameMap["IOP::SendingContextRunTime"] = "IOP_SendingContextRunTime"
gRenameMap["IOP::INVOCATION_POLICIES"] = "IOP_INVOCATION_POLICIES"
gRenameMap["IOP::FORWARDED_IDENTITY"] = "IOP_FORWARDED_IDENTITY"
gRenameMap["IOP::UnknownExceptionInfo"] = "IOP_UnknownExceptionInfo"
gRenameMap["IOP::RTCorbaPriority"] = "IOP_RTCorbaPriority"
gRenameMap["IOP::RTCorbaPriorityRange"] = "IOP_RTCorbaPriorityRange"
gRenameMap["IOP::FT_GROUP_VERSION"] = "IOP_FT_GROUP_VERSION"
gRenameMap["IOP::FT_REQUEST"] = "IOP_FT_REQUEST"
gRenameMap["IOP::ExceptionDetailMessage"] = "IOP_ExceptionDetailMessage"
gRenameMap["IOP::SecurityAttributeService"] = "IOP_SecurityAttributeService"
gRenameMap["IOP::ActivityService"] = "IOP_ActivityService"

gRenameMap["IOP::ENCODING_CDR_ENCAPS"] = "IOP_ENCODING_CDR_ENCAPS"

gRenameMap["CSI::OMGVMCID"] = "CSI_OMGVMCID"

gRenameMap["CSIIOP::TAG_SECIOP_SEC_TRANS"] = "IOP_TAG_SECIOP_SEC_TRANS"
gRenameMap["CSIIOP::TAG_TLS_SEC_TRANS"] = "IOP_TAG_TLS_SEC_TRANS"

gRenameMap["DCE_CIOPSecurity::TAG_DCE_SEC_MECH"] = "IOP_TAG_DCE_SEC_MECH"

gRenameMap["SECIOP::TAG_GENERIC_SEC_MECH"] = "IOP_TAG_GENERIC_SEC_MECH"
gRenameMap["SECIOP::TAG_ASSOCIATION_OPTIONS"] = "IOP_TAG_ASSOCIATION_OPTIONS"
gRenameMap["SECIOP::TAG_SEC_NAME"] = "IOP_TAG_SEC_NAME"
gRenameMap["SECIOP::TAG_SPKM_1_SEC_MECH"] = "IOP_TAG_SPKM_1_SEC_MECH"
gRenameMap["SECIOP::TAG_SPKM_2_SEC_MECH"] = "IOP_TAG_SPKM_2_SEC_MECH"
gRenameMap["SECIOP::TAG_KerberosV5_SEC_MECH"] = "IOP_TAG_KerberosV5_SEC_MECH"
gRenameMap["SECIOP::TAG_CSI_ECMA_Secret_SEC_MECH"] = "IOP_TAG_CSI_ECMA_Secret_SEC_MECH"
gRenameMap["SECIOP::TAG_CSI_ECMA_Hybrid_SEC_MECH"] = "IOP_TAG_CSI_ECMA_Hybrid_SEC_MECH"
gRenameMap["SECIOP::TAG_CSI_ECMA_Public_SEC_MECH"] = "IOP_TAG_CSI_ECMA_Public_SEC_MECH"

gRenameMap["Messaging::TRANSPARENT_REBIND"] = "MESSAGING_TRANSPARENT_REBIND"
gRenameMap["Messaging::NO_REBIND"] = "MESSAGING_NO_REBIND"
gRenameMap["Messaging::NO_RECONNECT"] = "MESSAGING_NO_RECONNECT"

gRenameMap["Messaging::SYNC_NONE"] = "MESSAGING_SYNC_NONE"
gRenameMap["Messaging::SYNC_WITH_TRANSPORT"] = "MESSAGING_SYNC_WITH_TRANSPORT"
gRenameMap["Messaging::SYNC_WITH_SERVER"] = "MESSAGING_SYNC_WITH_SERVER"
gRenameMap["Messaging::SYNC_WITH_TARGET"] = "MESSAGING_SYNC_WITH_TARGET"

gRenameMap["Messaging::ROUTE_NONE"] = "MESSAGING_ROUTE_NONE"
gRenameMap["Messaging::ROUTE_FORWARD"] = "MESSAGING_ROUTE_FORWARD"
gRenameMap["Messaging::ROUTE_STORE_AND_FORWARD"] = "MESSAGING_ROUTE_STORE_AND_FORWARD"

gRenameMap["Messaging::ORDER_ANY"] = "MESSAGING_ORDER_ANY"
gRenameMap["Messaging::ORDER_TEMPORAL"] = "MESSAGING_ORDER_TEMPORAL"
gRenameMap["Messaging::ORDER_PRIORITY"] = "MESSAGING_ORDER_PRIORITY"
gRenameMap["Messaging::ORDER_DEADLINE"] = "MESSAGING_ORDER_DEADLINE"

gRenameMap["Messaging::REBIND_POLICY_TYPE"] = "MESSAGING_REBIND_POLICY_TYPE"
gRenameMap["Messaging::SYNC_SCOPE_POLICY_TYPE"] = "MESSAGING_SYNC_SCOPE_POLICY_TYPE"
gRenameMap["Messaging::REQUEST_PRIORITY_POLICY_TYPE"] = "MESSAGING_REQUEST_PRIORITY_POLICY_TYPE"
gRenameMap["Messaging::REPLY_PRIORITY_POLICY_TYPE"] = "MESSAGING_REPLY_PRIORITY_POLICY_TYPE"
gRenameMap["Messaging::REQUEST_START_TIME_POLICY_TYPE"] = "MESSAGING_REQUEST_START_TIME_POLICY_TYPE"
gRenameMap["Messaging::REQUEST_END_TIME_POLICY_TYPE"] = "MESSAGING_REQUEST_END_TIME_POLICY_TYPE"
gRenameMap["Messaging::REPLY_START_TIME_POLICY_TYPE"] = "MESSAGING_REPLY_START_TIME_POLICY_TYPE"
gRenameMap["Messaging::REPLY_END_TIME_POLICY_TYPE"] = "MESSAGING_REPLY_END_TIME_POLICY_TYPE"
gRenameMap["Messaging::RELATIVE_REQ_TIMEOUT_POLICY_TYPE"] = "MESSAGING_RELATIVE_REQ_TIMEOUT_POLICY_TYPE"
gRenameMap["Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE"] = "MESSAGING_RELATIVE_RT_TIMEOUT_POLICY_TYPE"
gRenameMap["Messaging::ROUTING_POLICY_TYPE"] = "MESSAGING_ROUTING_POLICY_TYPE"
gRenameMap["Messaging::MAX_HOPS_POLICY_TYPE"] = "MESSAGING_MAX_HOPS_POLICY_TYPE"
gRenameMap["Messaging::QUEUE_ORDER_POLICY_TYPE"] = "MESSAGING_QUEUE_ORDER_POLICY_TYPE"

gRenameMap["Security::Operation"] = "COSSOperation"

#*******************************************************************************
#
#    declareAny() - outputs declarations for the CORBA Any and AnySeq types.
#
#    Any - the top-level, semi-abstract, pseudo-object that can hold
#    any other object.  (It is referred to as "Any" in the specification
#    and as "any" in the IDL.)  An "Any" object consists of:
#
#        (1)     TypeCode - a pseudo-object that describes the data type
#                of some object.  The initial field ...
#        (1a)    TCKind - specifies what kind of object the object is.
#        (1b)    There follow zero or more parameters describing the
#                generic form of the object's data type: it's layout,
#                the data types of the fields, and more.
#        (2)     Object Instance - of the TypeCode-described data type.
#
#    The TypeCode for an enum includes its repository name, it's type name,
#    a count of all the enumerated values, and all the possible enumerated
#    values themselves.
#
#    For a struct, the TypeCode assembles the repository ID, the struct
#    name, the number of fields, and, for each field, the field's name and
#    a TypeCode for the field's data type.  I'm guessing the TypeCode for
#    a field may pull in the full description of the field's data type.
#
#    The Any class appends the instantiation of an object -- with values! --
#    to the TypeCode description of the object's type.  This big blob from
#    the bog can now be sent back and forth over the internet.  Using the
#    TypeCode information, the applications on either side can decode the
#    values of objects received and encode the values of objects to be sent.
#
#    I haven't implemented all of the possible TCKind objects in the
#    TypeCode.
#
#*******************************************************************************

def declareAny (name):
    global gCustomTypes, gEnumModuleMap, gHookMap
    global gLastPrint, gMarshalMap, gTypeModuleMap

    if os.path.basename (gFileName) != "CORBA_InterfaceRepository.idl":  return

				# So it doesn't get called again.
    if name in gHookMap:  del gHookMap[name]

    print ()
    print ("typedef  enum  TCKind {")
    print ("    tk_null,")
    print ("    tk_void,")
    print ("    tk_short,")
    print ("    tk_long,")
    print ("    tk_ushort,")
    print ("    tk_ulong,")
    print ("    tk_float,")
    print ("    tk_double,")
    print ("    tk_boolean,")
    print ("    tk_char,")
    print ("    tk_octet,")
    print ("    tk_any,")
    print ("    tk_TypeCode,")
    print ("    tk_Principal,")
    print ("    tk_objref,")
    print ("    tk_struct,")
    print ("    tk_union,")
    print ("    tk_enum,")
    print ("    tk_string,")
    print ("    tk_sequence,")
    print ("    tk_array,")
    print ("    tk_alias,")
    print ("    tk_except,")
    print ("    tk_longlong,")
    print ("    tk_ulonglong,")
    print ("    tk_longdouble,")
    print ("    tk_wchar,")
    print ("    tk_wstring,")
    print ("    tk_fixed,")
    print ("    tk_value,")
    print ("    tk_value_box,")
    print ("    tk_native,")
    print ("    tk_abstract_interface,")
    print ("    tk_local_interface,")
    print ("    tk_component,")
    print ("    tk_home,")
    print ("    tk_event")
    print ("}  TCKind ;")
    gLastPrint = "enum"
    gEnumModuleMap["TCKind"] = "CORBA"
    gTypeModuleMap["TCKind"] = "CORBA"
    gNewEnums.add ("TCKind")
    gMarshalMap["TCKind"] = "gimxTCKind"

    print ()
    print ("typedef  struct  Any {")
    print ("    TCKind  which ;")
    print ("    union {")
    print ("			/* tk_null (Implemented) */")
    print ("			/* tk_void (Implemented) */")
    print ("			/* tk_short */")
    print ("        short  vShort ;")
    print ("			/* tk_long */")
    print ("        long  vLong ;")
    print ("			/* tk_ushort */")
    print ("        unsigned  short  vUShort ;")
    print ("			/* tk_ulong */")
    print ("        unsigned  long  vULong ;")
    print ("			/* tk_float */")
    print ("        float  vFloat ;")
    print ("			/* tk_double */")
    print ("        double  vDouble ;")
    print ("			/* tk_boolean */")
    print ("        bool  vBoolean ;")
    print ("			/* tk_char */")
    print ("        char  vChar ;")
    print ("			/* tk_octet */")
    print ("        octet  vOctet ;")
    print ("			/* tk_any (Not Implemented) */")
    print ("			/* tk_TypeCode */")
    print ("        TCKind  vTypeCode ;")
    print ("			/* tk_Principal */")
    print ("        OctetSeq  vPrincipal ;")
    print ("			/* tk_objref (N/I) */")
    print ("			/* tk_struct (N/I) */")
    print ("			/* tk_union (N/I) */")
    print ("			/* tk_enum (N/I) */")
    print ("			/* tk_string */")
    print ("        char  *vString ;")
    print ("			/* tk_sequence (N/I) */")
    print ("			/* tk_array (N/I) */")
    print ("			/* tk_alias (N/I) */")
    print ("			/* tk_except (N/I) */")
    print ("			/* tk_longlong */")
    print ("        LONGLONG  vLongLong ;")
    print ("			/* tk_ulonglong */")
    print ("        ULONGLONG  vULongLong ;")
    print ("			/* tk_longdouble */")
    print ("        LONGDOUBLE  vLongDouble ;")
    print ("			/* tk_wchar */")
    print ("        wchar_t  vWChar ;")
    print ("			/* tk_wstring */")
    print ("        wchar_t  *vWString ;")
    print ("			/* tk_fixed (N/I) */")
    print ("			/* tk_value (N/I) */")
    print ("			/* tk_value_box (N/I) */")
    print ("			/* tk_native (N/I) */")
    print ("			/* tk_abstract_interface (N/I) */")
    print ("			/* tk_local_interface (N/I) */")
    print ("			/* tk_component (N/I) */")
    print ("			/* tk_home (N/I) */")
    print ("			/* tk_event (N/I) */")
    print ("			/* 0xFFFFFFFF */")
    print ("        long  vIndirection ;")
    print ("    }  data ;")
    print ("}  Any ;")
    gLastPrint = "struct"
    gTypeModuleMap["Any"] = "CORBA"
    gMarshalMap["Any"] = "gimxAny"
			# CORBA IDL uses "any"; for some reason,
			# the addAlias() here doesn't work, but
			# adding "any" to the gCDRMap (at the
			# top of this file) does.  I used the
			# CDR map technique when I originally
			# wrote this 20 years ago, but it seems
			# like the addAlias() *should* work!
    addAlias ("any", "Any")

    print ()
    print ("typedef  SEQUENCE (Any, AnySeq) ;")
    gLastPrint = "typedef"
    gTypeModuleMap["AnySeq"] = "CORBA"
    gMarshalMap["AnySeq"] = "gimxAnySeq"
    alias = "sequence<Any>"
    if alias in gAliasMap:  addAlias ("AnySeq", alias)
				# Shorten CosCollection.idl's AnySequence.
    addAlias ("AnySequence", "AnySeq")
    gHookMap["CosCollection::AnySequence"] = "ignore"
    return

#*******************************************************************************
#    declareIOR() - outputs declarations for the CORBA IOR and related types.
#*******************************************************************************

def declareIOR ():
    global gLastPrint, gTypeModuleMap

    print ()
    print ("/* Module: IOP */")

    print ()
    print ("typedef  unsigned  long  ComponentId ;")
    addAlias ("ComponentId", "unsigned  long")
    gTypeModuleMap["ComponentId"] = "IOP"
    gMarshalMap["ComponentId"] = "#define"
    gHookMap["IOP::ComponentId"] = "ignore"

    print ()
    print ("typedef  struct  TaggedComponent {")
    print ("    ComponentId  tag ;")
    print ("    OctetSeq  component_data ;")
    print ("}  TaggedComponent ;")
    gTypeModuleMap["TaggedComponent"] = "IOP"
    gMarshalMap["TaggedComponent"] = "gimxTaggedComponent"
    gHookMap["IOP::TaggedComponent"] = "ignore"

    print ()
    print ("typedef  SEQUENCE (TaggedComponent, MultipleComponentProfile) ;")
    gTypeModuleMap["MultipleComponentProfile"] = "IOP"
    gMarshalMap["MultiComponentProfile"] = "gimxMultiComponentProfile"
    gHookMap["IOP::MultipleComponentProfile"] = "ignore"

    print ()
    print ("typedef  struct  ProfileBody {")
    print ("    Version  iiop_version ;")
    print ("    char  *host ;")
    print ("    unsigned  short  port ;")
    print ("    ObjectKey  object_key ;")
    print ("    SEQUENCE (TaggedComponent, components) ;	/* Ignore in IIOP 1.0! */")
    print ("}  ProfileBody ;")
    gTypeModuleMap["ProfileBody"] = "IOP"
    gMarshalMap["ProfileBody"] = "gimxProfileBody"

    print ()
    print ("typedef  unsigned  long  ProfileId ;")
    addAlias ("ProfileId", "unsigned  long")
    gTypeModuleMap["ProfileId"] = "IOP"
    gMarshalMap["ProfileId"] = "#define"
    gHookMap["IOP::ProfileId"] = "ignore"

    print ()
    print ("typedef  struct  TaggedProfile {")
    print ("    ProfileId  which ;")
    print ("    union {")
    print ("			/* TAG_INTERNET_IOP */")
    print ("        ProfileBody  iiop_body ;")
    print ("			/* TAG_MULTIPLE_COMPONENTS */")
    print ("        MultipleComponentProfile  components ;")
    print ("			/* <default> */")
    print ("        OctetSeq  profile_data ;")
    print ("    }  data ;")
    print ("}  TaggedProfile ;")
    gTypeModuleMap["TaggedProfile"] = "IOP"
    gMarshalMap["TaggedProfile"] = "gimxTaggedProfile"
    gHookMap["IOP::TaggedProfile"] = "ignore"

    print ()
    print ("typedef  struct  IOR {")
    print ("    char  *type_id ;")
    print ("    SEQUENCE (TaggedProfile, profiles) ;")
    print ("}  IOR ;")
    gTypeModuleMap["IOR"] = "IOP"
    gMarshalMap["IOR"] = "gimxIOR"
    gHookMap["IOP::IOR"] = "ignore"

    print ()
    print ("typedef  IOR  Object ;")
    gLastPrint = "struct"
    addAlias ("Object", "IOR")
    gTypeModuleMap["Object"] = "IOP"
    gMarshalMap["Object"] = "#define"
    gHookMap["IOP::Object"] = "ignore"

    return

#*******************************************************************************
#    declareGIOP() - outputs declarations for things in the GIOP module.
#*******************************************************************************

def declareGIOP (name):
    global gLastPrint, gTypeModuleMap

				# So it doesn't get called again.
    if name in gHookMap:  del gHookMap[name]

    print ()
    print ("/* GIOP MessageHeader flags. */")
    print ()
    print ("#define  ENDIAN_MASK  0x01		/* 0 = big-endian, 1 = little-endian */")
    print ("#define  FRAGMENT_MASK  0x02		/* 0 = last fragment, 1 = more fragments */")
    print ("")
    print ("/*******************************************************************************")
    print ("    Define generic enumerated types for GIOP version-specific enumerations.")
    print ("    These need to be updated in \"gimx_pre.py\" when new CORBA versions are")
    print ("    issued.")
    print ("*******************************************************************************/")
    print ()
    print ("typedef  enum  LocateStatusType_1_2 {")
    print ("    UNKNOWN_OBJECT,")
    print ("    OBJECT_HERE,")
    print ("    OBJECT_FORWARD,			/* GIOP 1.0/1.1 stop here. */")
    print ("    OBJECT_FORWARD_PERM,")
    print ("    LOC_SYSTEM_EXCEPTION,")
    print ("    LOC_NEEDS_ADDRESSING_MODE")
    print ("}  LocateStatusType ;")
    print ()
    print ("typedef  enum  MsgType_1_1 {")
    print ("    Request = 0,")
    print ("    Reply,")
    print ("    CancelRequest,")
    print ("    LocateRequest,")
    print ("    LocateReply,")
    print ("    CloseConnection,")
    print ("    MessageError,			/* GIOP 1.0 stops here. */")
    print ("    Fragment")
    print ("}  GIOPMsgType ;			/* MsgType would conflict with CSI.idl type. */")
    print ()
    print ("typedef  enum  ReplyStatusType_1_2 {")
    print ("    NO_EXCEPTION,")
    print ("    USER_EXCEPTION,")
    print ("    SYSTEM_EXCEPTION,")
    print ("    LOCATION_FORWARD,			/* GIOP 1.0/1.1 stop here. */")
    print ("    LOCATION_FORWARD_PERM,")
    print ("    NEEDS_ADDRESSING_MODE")
    print ("}  ReplyStatusType ;")

#*******************************************************************************
#    declareGIOPHeader() - outputs declarations for earlier versions of
#        GIOP request and reply headers.
#*******************************************************************************

def declareGIOPHeader (name):
    global gLastPrint, gTypeModuleMap

    if name == "GIOP::LocateReplyHeader_1_2":
        print ()
        print ("typedef  struct  LocateReplyHeader_1_0 {")
        print ("    unsigned  long  request_id ;")
        print ("    LocateStatusType  locate_status ;")
        print ("}  LocateReplyHeader_1_0 ;")
        print ()
        print ("typedef  LocateReplyHeader_1_0  LocateReplyHeader_1_1 ;")
        gLastPrint = "struct"
        gTypeModuleMap["LocateReplyHeader_1_0"] = "GIOP"
        addAlias ("LocateReplyHeader_1_1", "LocateReplyHeader_1_0")

    elif name == "GIOP::ReplyHeader_1_2":
        print ()
        print ("typedef  struct  ReplyHeader_1_0 {")
        print ("    ServiceContextList  service_context ;")
        print ("    unsigned  long  request_id ;")
        print ("    ReplyStatusType  reply_status ;")
        print ("}  ReplyHeader_1_0 ;")
        print ()
        print ("typedef  ReplyHeader_1_0  ReplyHeader_1_1 ;")
        gLastPrint = "struct"
        gTypeModuleMap["ReplyHeader_1_0"] = "GIOP"
        addAlias ("ReplyHeader_1_1", "ReplyHeader_1_0")

    return ""

#*******************************************************************************
#    Pre-declare types and such that are not handled as desired in the IDL.
#*******************************************************************************

print ("/*******************************************************************************")
print ("    Declarations for IOR and IOR-related types.")
print ("*******************************************************************************/")

declareIOR ()

print ()
print ()
print ("/*******************************************************************************")
print ("    Declaration for valuetype (which is unsupported).")
print ("        \"valuetype\" is a keyword in CORBA IDL that is used for")
print ("            (i) declaring \"valuetype\" types,")
print ("            (ii) providing forward references to \"valuetype\" types, and")
print ("            (iii) as the type for fields, function arguments, and function")
print ("                  return values.")
print ("        For (iii), define a typedef for the \"base\" valuetype as an OctetSeq,")
print ("        which was chosen arbitrarily.")
print ("*******************************************************************************/")
print ()

print ("typedef  OctetSeq  _valuetype_ ;")
addAlias ("_valuetype_", "OctetSeq")
gTypeModuleMap["_valuetype_"] = "CORBA"
gMarshalMap["_valuetype_"] = "#define"

print ("typedef  _valuetype_  valuetype ;	/* \"valuetype\"s are not supported. */")
addAlias ("valuetype", "_valuetype_")

print ("\f")

#*******************************************************************************
#    Insert old versions of GIOP headers that are #ifdef'ed out.
#*******************************************************************************

gHookMap["GIOP::LocateReplyHeader_1_2"] = declareGIOPHeader
gHookMap["GIOP::ReplyHeader_1_2"] = declareGIOPHeader
gHookMap["GIOP::"] = declareGIOP

#*******************************************************************************
#
#    CORBA's <orb.idl> file includes the other basic CORBA IDL files.
#    An actual, implementation-specific "orb.idl" file is supposed to
#    be supplied by your ORB provider.  Since I am the ORB provider in
#    this case and the example "orb.idl" file is sufficient for my needs,
#    I use it.
#
#    "CORBA_InterfaceRepository.idl" is included before "CORBA_TypeCode.idl"
#    because the latter file is dependent on some of the definitions in the
#    former file.  However, the IR file does use the "Any" type, which is
#    not defined anywhere in IDL but which does depend on the "TCKind"
#    enumerated type defined in the TypeCode file.  I provide a partial
#    implementation of the "Any" type, which uses "TCKind" (per the text
#    in the CORBA specification).
#
#    To get around this, I "hook" the declaration of the "TCKind" and
#    "Any" types to the IR file's declaration of "DefinitionKind".  When
#    that enumeration is encountered in the IR file, the hook function,
#    declareAny(), is called to insert declarations of "TCKind" and "Any".
#    A second hook causes the "TCKind" declaration in the TypeCode file
#    to be ignored.  (I could have simply edited "orb.idl", but I don't
#    want to modify the CORBA IDL files if I can help it.)
#
#    I am monumentally sick of CORBA and I haven't even begun my rant
#    about "valuetype"s ... ugh!
#
#*******************************************************************************

gHookMap["CORBA::DefinitionKind"] = declareAny
gHookMap["CORBA::TCKind"] = "ignore"
