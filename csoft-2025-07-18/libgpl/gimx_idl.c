/* $Id: gimx_idl.c,v 1.13 2021/09/20 02:06:28 alex Exp $ */
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
    gimxTaggedComponent() - decode/encode/erase a CORBA TaggedComponent structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxTaggedComponent (

#    if PROTOTYPES
        ComxChannel  channel,
        TaggedComponent  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TaggedComponent  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, tag))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, component_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxMultipleComponentProfile() - decode/encode/erase a sequence of CORBA TaggedComponent structures.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxMultipleComponentProfile (

#    if PROTOTYPES
        ComxChannel  channel,
        MultipleComponentProfile  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MultipleComponentProfile  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxTaggedComponent, sizeof (TaggedComponent))) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxIOR() - decode/encode/erase a CORBA IOR structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxIOR (

#    if PROTOTYPES
        ComxChannel  channel,
        IOR  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        IOR  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, type_id))) ;
    CHECK (comxSequence (channel, NULL_OR (value, profiles), (ComxFunc) gimxTaggedProfile, sizeof (TaggedProfile))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxEXCEPTcompletion_status() - decode/encode/erase a CORBA EXCEPTcompletion_status enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxEXCEPTcompletion_status (

#    if PROTOTYPES
        ComxChannel  channel,
        EXCEPTcompletion_status  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EXCEPTcompletion_status  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (EXCEPTcompletion_status) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxException_type() - decode/encode/erase a CORBA exception_type enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxException_type (

#    if PROTOTYPES
        ComxChannel  channel,
        exception_type  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        exception_type  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (exception_type) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxPolicyList() - decode/encode/erase a sequence of CORBA Policy structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxPolicyList (

#    if PROTOTYPES
        ComxChannel  channel,
        PolicyList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PolicyList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxPolicyTypeSeq() - decode/encode/erase a sequence of CORBA PolicyType structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxPolicyTypeSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        PolicyTypeSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PolicyTypeSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxULong, sizeof (unsigned long))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxDomainManagersList() - decode/encode/erase a sequence of CORBA DomainManager structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxDomainManagersList (

#    if PROTOTYPES
        ComxChannel  channel,
        DomainManagersList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DomainManagersList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxTCKind() - decode/encode/erase a CORBA TCKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxTCKind (

#    if PROTOTYPES
        ComxChannel  channel,
        TCKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TCKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (TCKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxAnySeq() - decode/encode/erase a sequence of CORBA Any structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxAnySeq (

#    if PROTOTYPES
        ComxChannel  channel,
        AnySeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AnySeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAny, sizeof (Any))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxDefinitionKind() - decode/encode/erase a CORBA DefinitionKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxDefinitionKind (

#    if PROTOTYPES
        ComxChannel  channel,
        DefinitionKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DefinitionKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (DefinitionKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxDescription() - decode/encode/erase a CORBA Description structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        Description  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Description  *value ;
#    endif

{
    CHECK (gimxDefinitionKind (channel, NULL_OR (value, kind))) ;
    CHECK (gimxAny (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxInterfaceDefSeq() - decode/encode/erase a sequence of CORBA InterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxInterfaceDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        InterfaceDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InterfaceDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxValueDefSeq() - decode/encode/erase a sequence of CORBA ValueDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxValueDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ValueDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ValueDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxAbstractInterfaceDefSeq() - decode/encode/erase a sequence of CORBA AbstractInterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxAbstractInterfaceDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        AbstractInterfaceDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AbstractInterfaceDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxLocalInterfaceDefSeq() - decode/encode/erase a sequence of CORBA LocalInterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxLocalInterfaceDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        LocalInterfaceDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LocalInterfaceDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtInterfaceDefSeq() - decode/encode/erase a sequence of CORBA ExtInterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtInterfaceDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtInterfaceDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtInterfaceDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtValueDefSeq() - decode/encode/erase a sequence of CORBA ExtValueDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtValueDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtValueDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtValueDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtAbstractInterfaceDefSeq() - decode/encode/erase a sequence of CORBA ExtAbstractInterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtAbstractInterfaceDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtAbstractInterfaceDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtAbstractInterfaceDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtLocalInterfaceDefSeq() - decode/encode/erase a sequence of CORBA ExtLocalInterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtLocalInterfaceDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtLocalInterfaceDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtLocalInterfaceDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxContainedSeq() - decode/encode/erase a sequence of CORBA Contained structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxContainedSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ContainedSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContainedSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxStructMember() - decode/encode/erase a CORBA StructMember structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxStructMember (

#    if PROTOTYPES
        ComxChannel  channel,
        StructMember  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        StructMember  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type_def))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxStructMemberSeq() - decode/encode/erase a sequence of CORBA StructMember structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxStructMemberSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        StructMemberSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        StructMemberSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxStructMember, sizeof (StructMember))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxInitializer() - decode/encode/erase a CORBA Initializer structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxInitializer (

#    if PROTOTYPES
        ComxChannel  channel,
        Initializer  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Initializer  *value ;
#    endif

{
    CHECK (gimxStructMemberSeq (channel, NULL_OR (value, members))) ;
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxInitializerSeq() - decode/encode/erase a sequence of CORBA Initializer structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxInitializerSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        InitializerSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InitializerSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxInitializer, sizeof (Initializer))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxUnionMember() - decode/encode/erase a CORBA UnionMember structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxUnionMember (

#    if PROTOTYPES
        ComxChannel  channel,
        UnionMember  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UnionMember  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxAny (channel, NULL_OR (value, label))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type_def))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExceptionDescription() - decode/encode/erase a CORBA ExceptionDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExceptionDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ExceptionDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExceptionDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExcDescriptionSeq() - decode/encode/erase a sequence of CORBA ExceptionDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExcDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExcDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExcDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxExceptionDescription, sizeof (ExceptionDescription))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtInitializer() - decode/encode/erase a CORBA ExtInitializer structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtInitializer (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtInitializer  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtInitializer  *value ;
#    endif

{
    CHECK (gimxStructMemberSeq (channel, NULL_OR (value, members))) ;
    CHECK (gimxExcDescriptionSeq (channel, NULL_OR (value, exceptions))) ;
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtInitializerSeq() - decode/encode/erase a sequence of CORBA ExtInitializer structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtInitializerSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtInitializerSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtInitializerSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxExtInitializer, sizeof (ExtInitializer))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxUnionMemberSeq() - decode/encode/erase a sequence of CORBA UnionMember structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxUnionMemberSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        UnionMemberSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UnionMemberSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxUnionMember, sizeof (UnionMember))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxEnumMemberSeq() - decode/encode/erase a sequence of CORBA Identifier structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxEnumMemberSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        EnumMemberSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EnumMemberSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxDescriptionSeq() - decode/encode/erase a sequence of CORBA Description structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        DescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxDescription, sizeof (Description))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxPrimitiveKind() - decode/encode/erase a CORBA PrimitiveKind enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxPrimitiveKind (

#    if PROTOTYPES
        ComxChannel  channel,
        PrimitiveKind  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PrimitiveKind  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (PrimitiveKind) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxModuleDescription() - decode/encode/erase a CORBA ModuleDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxModuleDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ModuleDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ModuleDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxConstantDescription() - decode/encode/erase a CORBA ConstantDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxConstantDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ConstantDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ConstantDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxAny (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxTypeDescription() - decode/encode/erase a CORBA TypeDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxTypeDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        TypeDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TypeDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxAttributeMode() - decode/encode/erase a CORBA AttributeMode enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxAttributeMode (

#    if PROTOTYPES
        ComxChannel  channel,
        AttributeMode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AttributeMode  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (AttributeMode) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxAttributeDescription() - decode/encode/erase a CORBA AttributeDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxAttributeDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        AttributeDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AttributeDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxAttributeMode (channel, NULL_OR (value, mode))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtAttributeDescription() - decode/encode/erase a CORBA ExtAttributeDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtAttributeDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtAttributeDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtAttributeDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxAttributeMode (channel, NULL_OR (value, mode))) ;
    CHECK (gimxExcDescriptionSeq (channel, NULL_OR (value, get_exceptions))) ;
    CHECK (gimxExcDescriptionSeq (channel, NULL_OR (value, put_exceptions))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxOperationMode() - decode/encode/erase a CORBA OperationMode enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxOperationMode (

#    if PROTOTYPES
        ComxChannel  channel,
        OperationMode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OperationMode  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (OperationMode) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxParameterMode() - decode/encode/erase a CORBA ParameterMode enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxParameterMode (

#    if PROTOTYPES
        ComxChannel  channel,
        ParameterMode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ParameterMode  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ParameterMode) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxParameterDescription() - decode/encode/erase a CORBA ParameterDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxParameterDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ParameterDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ParameterDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type_def))) ;
    CHECK (gimxParameterMode (channel, NULL_OR (value, mode))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxParDescriptionSeq() - decode/encode/erase a sequence of CORBA ParameterDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxParDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ParDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ParDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxParameterDescription, sizeof (ParameterDescription))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxContextIdSeq() - decode/encode/erase a sequence of CORBA ContextIdentifier structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxContextIdSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ContextIdSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContextIdSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExceptionDefSeq() - decode/encode/erase a sequence of CORBA ExceptionDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExceptionDefSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExceptionDefSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExceptionDefSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxOperationDescription() - decode/encode/erase a CORBA OperationDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxOperationDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        OperationDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OperationDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, result))) ;
    CHECK (gimxOperationMode (channel, NULL_OR (value, mode))) ;
    CHECK (gimxContextIdSeq (channel, NULL_OR (value, contexts))) ;
    CHECK (gimxParDescriptionSeq (channel, NULL_OR (value, parameters))) ;
    CHECK (gimxExcDescriptionSeq (channel, NULL_OR (value, exceptions))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxRepositoryIdSeq() - decode/encode/erase a sequence of CORBA RepositoryId structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxRepositoryIdSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        RepositoryIdSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RepositoryIdSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxOpDescriptionSeq() - decode/encode/erase a sequence of CORBA OperationDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxOpDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        OpDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OpDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxOperationDescription, sizeof (OperationDescription))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxAttrDescriptionSeq() - decode/encode/erase a sequence of CORBA AttributeDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxAttrDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        AttrDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AttrDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAttributeDescription, sizeof (AttributeDescription))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtAttrDescriptionSeq() - decode/encode/erase a sequence of CORBA ExtAttributeDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtAttrDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtAttrDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtAttrDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxExtAttributeDescription, sizeof (ExtAttributeDescription))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxFullInterfaceDescription() - decode/encode/erase a CORBA FullInterfaceDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxFullInterfaceDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        FullInterfaceDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        FullInterfaceDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, operations))) ;
    CHECK (gimxAttrDescriptionSeq (channel, NULL_OR (value, attributes))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, base_interfaces))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_abstract))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxInterfaceDescription() - decode/encode/erase a CORBA InterfaceDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxInterfaceDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        InterfaceDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InterfaceDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, base_interfaces))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_abstract))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtFullInterfaceDescription() - decode/encode/erase a CORBA ExtFullInterfaceDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtFullInterfaceDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtFullInterfaceDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtFullInterfaceDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, operations))) ;
    CHECK (gimxExtAttrDescriptionSeq (channel, NULL_OR (value, attributes))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, base_interfaces))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxValueMember() - decode/encode/erase a CORBA ValueMember structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxValueMember (

#    if PROTOTYPES
        ComxChannel  channel,
        ValueMember  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ValueMember  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type_def))) ;
    CHECK (comxShort (channel, NULL_OR (value, access))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxValueMemberSeq() - decode/encode/erase a sequence of CORBA ValueMember structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxValueMemberSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ValueMemberSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ValueMemberSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxValueMember, sizeof (ValueMember))) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxFullValueDescription() - decode/encode/erase a CORBA FullValueDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxFullValueDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        FullValueDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        FullValueDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_abstract))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_custom))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, operations))) ;
    CHECK (gimxAttrDescriptionSeq (channel, NULL_OR (value, attributes))) ;
    CHECK (gimxValueMemberSeq (channel, NULL_OR (value, members))) ;
    CHECK (gimxInitializerSeq (channel, NULL_OR (value, initializers))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, supported_interfaces))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, abstract_base_values))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_truncatable))) ;
    CHECK (comxString (channel, NULL_OR (value, base_value))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxValueDescription() - decode/encode/erase a CORBA ValueDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxValueDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ValueDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ValueDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_abstract))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_custom))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, supported_interfaces))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, abstract_base_values))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_truncatable))) ;
    CHECK (comxString (channel, NULL_OR (value, base_value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxExtFullValueDescription() - decode/encode/erase a CORBA ExtFullValueDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA

errno_t  gimxExtFullValueDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtFullValueDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtFullValueDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_abstract))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_custom))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, operations))) ;
    CHECK (gimxExtAttrDescriptionSeq (channel, NULL_OR (value, attributes))) ;
    CHECK (gimxValueMemberSeq (channel, NULL_OR (value, members))) ;
    CHECK (gimxExtInitializerSeq (channel, NULL_OR (value, initializers))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, supported_interfaces))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, abstract_base_values))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_truncatable))) ;
    CHECK (comxString (channel, NULL_OR (value, base_value))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA */

/*!*****************************************************************************
    gimxProvidesDescription() - decode/encode/erase a CORBA ProvidesDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxProvidesDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ProvidesDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ProvidesDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (comxString (channel, NULL_OR (value, interface_type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxUsesDescription() - decode/encode/erase a CORBA UsesDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxUsesDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        UsesDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UsesDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (comxString (channel, NULL_OR (value, interface_type))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, is_multiple))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxEventPortDescription() - decode/encode/erase a CORBA EventPortDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxEventPortDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        EventPortDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EventPortDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (comxString (channel, NULL_OR (value, event))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxProvidesDescriptionSeq() - decode/encode/erase a sequence of CORBA ProvidesDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxProvidesDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ProvidesDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ProvidesDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxProvidesDescription, sizeof (ProvidesDescription))) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxUsesDescriptionSeq() - decode/encode/erase a sequence of CORBA UsesDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxUsesDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        UsesDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UsesDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxUsesDescription, sizeof (UsesDescription))) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxEventPortDescriptionSeq() - decode/encode/erase a sequence of CORBA EventPortDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxEventPortDescriptionSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        EventPortDescriptionSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EventPortDescriptionSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxEventPortDescription, sizeof (EventPortDescription))) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxComponentDescription() - decode/encode/erase a CORBA ComponentDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxComponentDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        ComponentDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ComponentDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (comxString (channel, NULL_OR (value, base_component))) ;
    CHECK (gimxRepositoryIdSeq (channel, NULL_OR (value, supported_interfaces))) ;
    CHECK (gimxProvidesDescriptionSeq (channel, NULL_OR (value, provided_interfaces))) ;
    CHECK (gimxUsesDescriptionSeq (channel, NULL_OR (value, used_interfaces))) ;
    CHECK (gimxEventPortDescriptionSeq (channel, NULL_OR (value, emits_events))) ;
    CHECK (gimxEventPortDescriptionSeq (channel, NULL_OR (value, publishes_events))) ;
    CHECK (gimxEventPortDescriptionSeq (channel, NULL_OR (value, consumes_events))) ;
    CHECK (gimxExtAttrDescriptionSeq (channel, NULL_OR (value, attributes))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxHomeDescription() - decode/encode/erase a CORBA HomeDescription structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CORBA__ComponentIR) || IDL_MODULE_CORBA__ComponentIR

errno_t  gimxHomeDescription (

#    if PROTOTYPES
        ComxChannel  channel,
        HomeDescription  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        HomeDescription  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, defined_in))) ;
    CHECK (comxString (channel, NULL_OR (value, version))) ;
    CHECK (comxString (channel, NULL_OR (value, base_home))) ;
    CHECK (comxString (channel, NULL_OR (value, managed_component))) ;
    CHECK (gimxValueDescription (channel, NULL_OR (value, primary_key))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, factories))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, finders))) ;
    CHECK (gimxOpDescriptionSeq (channel, NULL_OR (value, operations))) ;
    CHECK (gimxExtAttrDescriptionSeq (channel, NULL_OR (value, attributes))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CORBA__ComponentIR */

/*!*****************************************************************************
    gimxCodeSetComponent() - decode/encode/erase a CORBA CodeSetComponent structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME

errno_t  gimxCodeSetComponent (

#    if PROTOTYPES
        ComxChannel  channel,
        CodeSetComponent  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CodeSetComponent  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, native_code_set))) ;
    CHECK (comxSequence (channel, NULL_OR (value, conversion_code_sets), (ComxFunc) comxULong, sizeof (CodeSetId))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CONV_FRAME */

/*!*****************************************************************************
    gimxCodeSetComponentInfo() - decode/encode/erase a CORBA CodeSetComponentInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME

errno_t  gimxCodeSetComponentInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        CodeSetComponentInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CodeSetComponentInfo  *value ;
#    endif

{
    CHECK (gimxCodeSetComponent (channel, NULL_OR (value, ForCharData))) ;
    CHECK (gimxCodeSetComponent (channel, NULL_OR (value, ForWcharData))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CONV_FRAME */

/*!*****************************************************************************
    gimxCodeSetContext() - decode/encode/erase a CORBA CodeSetContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CONV_FRAME) || IDL_MODULE_CONV_FRAME

errno_t  gimxCodeSetContext (

#    if PROTOTYPES
        ComxChannel  channel,
        CodeSetContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CodeSetContext  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, char_data))) ;
    CHECK (comxULong (channel, NULL_OR (value, wchar_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CONV_FRAME */

/*!*****************************************************************************
    gimxServiceContext() - decode/encode/erase a CORBA ServiceContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxServiceContext (

#    if PROTOTYPES
        ComxChannel  channel,
        ServiceContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ServiceContext  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, context_id))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, context_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxServiceContextList() - decode/encode/erase a sequence of CORBA ServiceContext structures.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxServiceContextList (

#    if PROTOTYPES
        ComxChannel  channel,
        ServiceContextList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ServiceContextList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxServiceContext, sizeof (ServiceContext))) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxEncoding() - decode/encode/erase a CORBA Encoding structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxEncoding (

#    if PROTOTYPES
        ComxChannel  channel,
        Encoding  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Encoding  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, format))) ;
    CHECK (comxOctet (channel, NULL_OR (value, major_version))) ;
    CHECK (comxOctet (channel, NULL_OR (value, minor_version))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxOIDList() - decode/encode/erase a sequence of CORBA OID structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxOIDList (

#    if PROTOTYPES
        ComxChannel  channel,
        OIDList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OIDList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxOctetSeq, sizeof (OctetSeq))) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxGSS_NT_ExportedNameList() - decode/encode/erase a sequence of CORBA GSS_NT_ExportedName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxGSS_NT_ExportedNameList (

#    if PROTOTYPES
        ComxChannel  channel,
        GSS_NT_ExportedNameList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        GSS_NT_ExportedNameList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxOctetSeq, sizeof (OctetSeq))) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxAuthorizationElement() - decode/encode/erase a CORBA AuthorizationElement structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxAuthorizationElement (

#    if PROTOTYPES
        ComxChannel  channel,
        AuthorizationElement  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuthorizationElement  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, the_type))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, the_element))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxAuthorizationToken() - decode/encode/erase a sequence of CORBA AuthorizationElement structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxAuthorizationToken (

#    if PROTOTYPES
        ComxChannel  channel,
        AuthorizationToken  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuthorizationToken  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAuthorizationElement, sizeof (AuthorizationElement))) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxIdentityToken() - decode/encode/erase a CORBA IdentityToken structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxIdentityToken (

#    if PROTOTYPES
        ComxChannel  channel,
        IdentityToken  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        IdentityToken  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case ITTAbsent:
        CHECK (comxBoolean (channel, NULL_OR (value, data.absent))) ;
        break ;
    case ITTAnonymous:
        CHECK (comxBoolean (channel, NULL_OR (value, data.anonymous))) ;
        break ;
    case ITTPrincipalName:
        CHECK (comxOctetSeq (channel, NULL_OR (value, data.principal_name))) ;
        break ;
    case ITTX509CertChain:
        CHECK (comxOctetSeq (channel, NULL_OR (value, data.certificate_chain))) ;
        break ;
    case ITTDistinguishedName:
        CHECK (comxOctetSeq (channel, NULL_OR (value, data.dn))) ;
        break ;
    default:
        CHECK (comxOctetSeq (channel, NULL_OR (value, data.id))) ;
        break ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxEstablishContext() - decode/encode/erase a CORBA EstablishContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxEstablishContext (

#    if PROTOTYPES
        ComxChannel  channel,
        EstablishContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EstablishContext  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, client_context_id))) ;
    CHECK (gimxAuthorizationToken (channel, NULL_OR (value, authorization_token))) ;
    CHECK (gimxIdentityToken (channel, NULL_OR (value, identity_token))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, client_authentication_token))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxCompleteEstablishContext() - decode/encode/erase a CORBA CompleteEstablishContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxCompleteEstablishContext (

#    if PROTOTYPES
        ComxChannel  channel,
        CompleteEstablishContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CompleteEstablishContext  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, client_context_id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, context_stateful))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, final_context_token))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxContextError() - decode/encode/erase a CORBA ContextError structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxContextError (

#    if PROTOTYPES
        ComxChannel  channel,
        ContextError  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContextError  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, client_context_id))) ;
    CHECK (comxLong (channel, NULL_OR (value, major_status))) ;
    CHECK (comxLong (channel, NULL_OR (value, minor_status))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, error_token))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxMessageInContext() - decode/encode/erase a CORBA MessageInContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxMessageInContext (

#    if PROTOTYPES
        ComxChannel  channel,
        MessageInContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MessageInContext  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, client_context_id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, discard_context))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxSASContextBody() - decode/encode/erase a CORBA SASContextBody structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSI) || IDL_MODULE_CSI

errno_t  gimxSASContextBody (

#    if PROTOTYPES
        ComxChannel  channel,
        SASContextBody  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SASContextBody  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case MTEstablishContext:
        CHECK (gimxEstablishContext (channel, NULL_OR (value, data.establish_msg))) ;
        break ;
    case MTCompleteEstablishContext:
        CHECK (gimxCompleteEstablishContext (channel, NULL_OR (value, data.complete_msg))) ;
        break ;
    case MTContextError:
        CHECK (gimxContextError (channel, NULL_OR (value, data.error_msg))) ;
        break ;
    case MTMessageInContext:
        CHECK (gimxMessageInContext (channel, NULL_OR (value, data.in_context_msg))) ;
        break ;
    default:
        SET_ERRNO (EINVAL) ;
        LGE "(gimxSASContextBody) Invalid switch: %lu\n", (unsigned long) value->which) ;
        return (errno) ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_CSI */

/*!*****************************************************************************
    gimxServiceConfiguration() - decode/encode/erase a CORBA ServiceConfiguration structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxServiceConfiguration (

#    if PROTOTYPES
        ComxChannel  channel,
        ServiceConfiguration  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ServiceConfiguration  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, syntax))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxServiceConfigurationList() - decode/encode/erase a sequence of CORBA ServiceConfiguration structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxServiceConfigurationList (

#    if PROTOTYPES
        ComxChannel  channel,
        ServiceConfigurationList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ServiceConfigurationList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxServiceConfiguration, sizeof (ServiceConfiguration))) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxAS_ContextSec() - decode/encode/erase a CORBA AS_ContextSec structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxAS_ContextSec (

#    if PROTOTYPES
        ComxChannel  channel,
        AS_ContextSec  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AS_ContextSec  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, client_authentication_mech))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, target_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxSAS_ContextSec() - decode/encode/erase a CORBA SAS_ContextSec structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxSAS_ContextSec (

#    if PROTOTYPES
        ComxChannel  channel,
        SAS_ContextSec  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SAS_ContextSec  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxServiceConfigurationList (channel, NULL_OR (value, privilege_authorities))) ;
    CHECK (gimxOIDList (channel, NULL_OR (value, supported_naming_mechanisms))) ;
    CHECK (comxULong (channel, NULL_OR (value, supported_identity_types))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxCompoundSecMech() - decode/encode/erase a CORBA CompoundSecMech structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxCompoundSecMech (

#    if PROTOTYPES
        ComxChannel  channel,
        CompoundSecMech  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CompoundSecMech  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxTaggedComponent (channel, NULL_OR (value, transport_mech))) ;
    CHECK (gimxAS_ContextSec (channel, NULL_OR (value, as_context_mech))) ;
    CHECK (gimxSAS_ContextSec (channel, NULL_OR (value, sas_context_mech))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxCompoundSecMechanisms() - decode/encode/erase a sequence of CORBA CompoundSecMech structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxCompoundSecMechanisms (

#    if PROTOTYPES
        ComxChannel  channel,
        CompoundSecMechanisms  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CompoundSecMechanisms  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxCompoundSecMech, sizeof (CompoundSecMech))) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxCompoundSecMechList() - decode/encode/erase a CORBA CompoundSecMechList structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxCompoundSecMechList (

#    if PROTOTYPES
        ComxChannel  channel,
        CompoundSecMechList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CompoundSecMechList  *value ;
#    endif

{
    CHECK (comxBoolean (channel, NULL_OR (value, stateful))) ;
    CHECK (gimxCompoundSecMechanisms (channel, NULL_OR (value, mechanism_list))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxTransportAddress() - decode/encode/erase a CORBA TransportAddress structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxTransportAddress (

#    if PROTOTYPES
        ComxChannel  channel,
        TransportAddress  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TransportAddress  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, host_name))) ;
    CHECK (comxUShort (channel, NULL_OR (value, port))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxTransportAddressList() - decode/encode/erase a sequence of CORBA TransportAddress structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxTransportAddressList (

#    if PROTOTYPES
        ComxChannel  channel,
        TransportAddressList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TransportAddressList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxTransportAddress, sizeof (TransportAddress))) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxSECIOP_SEC_TRANS() - decode/encode/erase a CORBA SECIOP_SEC_TRANS structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxSECIOP_SEC_TRANS (

#    if PROTOTYPES
        ComxChannel  channel,
        SECIOP_SEC_TRANS  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SECIOP_SEC_TRANS  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, mech_oid))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, target_name))) ;
    CHECK (gimxTransportAddressList (channel, NULL_OR (value, addresses))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxTLS_SEC_TRANS() - decode/encode/erase a CORBA TLS_SEC_TRANS structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CSIIOP) || IDL_MODULE_CSIIOP

errno_t  gimxTLS_SEC_TRANS (

#    if PROTOTYPES
        ComxChannel  channel,
        TLS_SEC_TRANS  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TLS_SEC_TRANS  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxTransportAddressList (channel, NULL_OR (value, addresses))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CSIIOP */

/*!*****************************************************************************
    gimxLocateStatusType() - decode/encode/erase a CORBA LocateStatusType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxLocateStatusType (

#    if PROTOTYPES
        ComxChannel  channel,
        LocateStatusType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LocateStatusType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (LocateStatusType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxGIOPMsgType() - decode/encode/erase a CORBA GIOPMsgType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxGIOPMsgType (

#    if PROTOTYPES
        ComxChannel  channel,
        GIOPMsgType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        GIOPMsgType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (GIOPMsgType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxReplyStatusType() - decode/encode/erase a CORBA ReplyStatusType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxReplyStatusType (

#    if PROTOTYPES
        ComxChannel  channel,
        ReplyStatusType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReplyStatusType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ReplyStatusType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxMessageHeader_1_0() - decode/encode/erase a CORBA MessageHeader_1_0 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxMessageHeader_1_0 (

#    if PROTOTYPES
        ComxChannel  channel,
        MessageHeader_1_0  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MessageHeader_1_0  *value ;
#    endif

{
    CHECK (comxArray (channel, NULL_OR (value, magic), (ComxFunc) comxChar, sizeof (char), 4)) ;
    CHECK (comxVersion (channel, NULL_OR (value, GIOP_version))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, byte_order))) ;
    CHECK (comxOctet (channel, NULL_OR (value, message_type))) ;
    CHECK (comxULong (channel, NULL_OR (value, message_size))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxMessageHeader() - decode/encode/erase a CORBA MessageHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxMessageHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        MessageHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MessageHeader  *value ;
#    endif

{
    CHECK (comxArray (channel, NULL_OR (value, magic), (ComxFunc) comxChar, sizeof (char), 4)) ;
    CHECK (comxVersion (channel, NULL_OR (value, GIOP_version))) ;
    CHECK (comxOctet (channel, NULL_OR (value, flags))) ;
    CHECK (comxOctet (channel, NULL_OR (value, message_type))) ;
    CHECK (comxULong (channel, NULL_OR (value, message_size))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxRequestHeader_1_0() - decode/encode/erase a CORBA RequestHeader_1_0 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxRequestHeader_1_0 (

#    if PROTOTYPES
        ComxChannel  channel,
        RequestHeader_1_0  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequestHeader_1_0  *value ;
#    endif

{
    CHECK (gimxServiceContextList (channel, NULL_OR (value, service_context))) ;
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, response_expected))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, object_key))) ;
    CHECK (comxString (channel, NULL_OR (value, operation))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, requesting_principal))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxRequestHeader_1_1() - decode/encode/erase a CORBA RequestHeader_1_1 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxRequestHeader_1_1 (

#    if PROTOTYPES
        ComxChannel  channel,
        RequestHeader_1_1  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequestHeader_1_1  *value ;
#    endif

{
    CHECK (gimxServiceContextList (channel, NULL_OR (value, service_context))) ;
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, response_expected))) ;
    CHECK (comxArray (channel, NULL_OR (value, reserved), (ComxFunc) comxOctet, sizeof (octet), 3)) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, object_key))) ;
    CHECK (comxString (channel, NULL_OR (value, operation))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, requesting_principal))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxIORAddressingInfo() - decode/encode/erase a CORBA IORAddressingInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxIORAddressingInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        IORAddressingInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        IORAddressingInfo  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, selected_profile_index))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, ior))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxTargetAddress() - decode/encode/erase a CORBA TargetAddress structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxTargetAddress (

#    if PROTOTYPES
        ComxChannel  channel,
        TargetAddress  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TargetAddress  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case KeyAddr:
        CHECK (comxOctetSeq (channel, NULL_OR (value, data.object_key))) ;
        break ;
    case ProfileAddr:
        CHECK (gimxTaggedProfile (channel, NULL_OR (value, data.profile))) ;
        break ;
    case ReferenceAddr:
        CHECK (gimxIORAddressingInfo (channel, NULL_OR (value, data.ior))) ;
        break ;
    default:
        SET_ERRNO (EINVAL) ;
        LGE "(gimxTargetAddress) Invalid switch: %lu\n", (unsigned long) value->which) ;
        return (errno) ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxRequestHeader() - decode/encode/erase a CORBA RequestHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxRequestHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        RequestHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequestHeader  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (comxOctet (channel, NULL_OR (value, response_flags))) ;
    CHECK (comxArray (channel, NULL_OR (value, reserved), (ComxFunc) comxOctet, sizeof (octet), 3)) ;
    CHECK (gimxTargetAddress (channel, NULL_OR (value, target))) ;
    CHECK (comxString (channel, NULL_OR (value, operation))) ;
    CHECK (gimxServiceContextList (channel, NULL_OR (value, service_context))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxReplyHeader_1_0() - decode/encode/erase a CORBA ReplyHeader_1_0 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxReplyHeader_1_0 (

#    if PROTOTYPES
        ComxChannel  channel,
        ReplyHeader_1_0  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReplyHeader_1_0  *value ;
#    endif

{
    CHECK (gimxServiceContextList (channel, NULL_OR (value, service_context))) ;
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (gimxReplyStatusType (channel, NULL_OR (value, reply_status))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxReplyHeader() - decode/encode/erase a CORBA ReplyHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxReplyHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        ReplyHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ReplyHeader  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (gimxReplyStatusType (channel, NULL_OR (value, reply_status))) ;
    CHECK (gimxServiceContextList (channel, NULL_OR (value, service_context))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxSystemExceptionReplyBody() - decode/encode/erase a CORBA SystemExceptionReplyBody structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxSystemExceptionReplyBody (

#    if PROTOTYPES
        ComxChannel  channel,
        SystemExceptionReplyBody  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SystemExceptionReplyBody  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, exception_id))) ;
    CHECK (comxULong (channel, NULL_OR (value, minor_code_value))) ;
    CHECK (comxULong (channel, NULL_OR (value, completion_status))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxCancelRequestHeader() - decode/encode/erase a CORBA CancelRequestHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxCancelRequestHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        CancelRequestHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CancelRequestHeader  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxLocateRequestHeader_1_0() - decode/encode/erase a CORBA LocateRequestHeader_1_0 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxLocateRequestHeader_1_0 (

#    if PROTOTYPES
        ComxChannel  channel,
        LocateRequestHeader_1_0  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LocateRequestHeader_1_0  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, object_key))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxLocateRequestHeader() - decode/encode/erase a CORBA LocateRequestHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxLocateRequestHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        LocateRequestHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LocateRequestHeader  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (gimxTargetAddress (channel, NULL_OR (value, target))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxLocateReplyHeader_1_0() - decode/encode/erase a CORBA LocateReplyHeader_1_0 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxLocateReplyHeader_1_0 (

#    if PROTOTYPES
        ComxChannel  channel,
        LocateReplyHeader_1_0  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LocateReplyHeader_1_0  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (gimxLocateStatusType (channel, NULL_OR (value, locate_status))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxLocateReplyHeader() - decode/encode/erase a CORBA LocateReplyHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxLocateReplyHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        LocateReplyHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LocateReplyHeader  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    CHECK (gimxLocateStatusType (channel, NULL_OR (value, locate_status))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxFragmentHeader() - decode/encode/erase a CORBA FragmentHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GIOP) || IDL_MODULE_GIOP

errno_t  gimxFragmentHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        FragmentHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        FragmentHeader  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, request_id))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GIOP */

/*!*****************************************************************************
    gimxInitialContextToken() - decode/encode/erase a CORBA InitialContextToken structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GSSUP) || IDL_MODULE_GSSUP

errno_t  gimxInitialContextToken (

#    if PROTOTYPES
        ComxChannel  channel,
        InitialContextToken  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InitialContextToken  *value ;
#    endif

{
    CHECK (comxOctetSeq (channel, NULL_OR (value, username))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, password))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, target_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GSSUP */

/*!*****************************************************************************
    gimxErrorToken() - decode/encode/erase a CORBA ErrorToken structure.
*******************************************************************************/

#if !defined(IDL_MODULE_GSSUP) || IDL_MODULE_GSSUP

errno_t  gimxErrorToken (

#    if PROTOTYPES
        ComxChannel  channel,
        ErrorToken  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ErrorToken  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, error_code))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_GSSUP */

/*!*****************************************************************************
    gimxListenPoint() - decode/encode/erase a CORBA ListenPoint structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IIOP) || IDL_MODULE_IIOP

errno_t  gimxListenPoint (

#    if PROTOTYPES
        ComxChannel  channel,
        ListenPoint  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ListenPoint  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, host))) ;
    CHECK (comxUShort (channel, NULL_OR (value, port))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IIOP */

/*!*****************************************************************************
    gimxListenPointList() - decode/encode/erase a sequence of CORBA ListenPoint structures.
*******************************************************************************/

#if !defined(IDL_MODULE_IIOP) || IDL_MODULE_IIOP

errno_t  gimxListenPointList (

#    if PROTOTYPES
        ComxChannel  channel,
        ListenPointList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ListenPointList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxListenPoint, sizeof (ListenPoint))) ;
}

#endif	/* IDL_MODULE_IIOP */

/*!*****************************************************************************
    gimxBiDirIIOPServiceContext() - decode/encode/erase a CORBA BiDirIIOPServiceContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IIOP) || IDL_MODULE_IIOP

errno_t  gimxBiDirIIOPServiceContext (

#    if PROTOTYPES
        ComxChannel  channel,
        BiDirIIOPServiceContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        BiDirIIOPServiceContext  *value ;
#    endif

{
    CHECK (gimxListenPointList (channel, NULL_OR (value, listen_points))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IIOP */

/*!*****************************************************************************
    gimxEndpointIdPositionComponent() - decode/encode/erase a CORBA EndpointIdPositionComponent structure.
*******************************************************************************/

#if !defined(IDL_MODULE_IOP) || IDL_MODULE_IOP

errno_t  gimxEndpointIdPositionComponent (

#    if PROTOTYPES
        ComxChannel  channel,
        EndpointIdPositionComponent  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EndpointIdPositionComponent  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, begin))) ;
    CHECK (comxUShort (channel, NULL_OR (value, end))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_IOP */

/*!*****************************************************************************
    gimxURLSeq() - decode/encode/erase a sequence of CORBA URL structures.
*******************************************************************************/

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext

errno_t  gimxURLSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        URLSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        URLSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_SendingContext */

/*!*****************************************************************************
    gimxValueDescSeq() - decode/encode/erase a sequence of CORBA FullValueDescription structures.
*******************************************************************************/

#if !defined(IDL_MODULE_SendingContext) || IDL_MODULE_SendingContext

errno_t  gimxValueDescSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ValueDescSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ValueDescSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxFullValueDescription, sizeof (FullValueDescription))) ;
}

#endif	/* IDL_MODULE_SendingContext */

/*!*****************************************************************************
    gimxStatus() - decode/encode/erase a CORBA Status enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions

errno_t  gimxStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        Status  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Status  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (Status) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTransactions */

/*!*****************************************************************************
    gimxVote() - decode/encode/erase a CORBA Vote enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions

errno_t  gimxVote (

#    if PROTOTYPES
        ComxChannel  channel,
        Vote  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Vote  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (Vote) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTransactions */

/*!*****************************************************************************
    gimxOtid_t() - decode/encode/erase a CORBA otid_t structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions

errno_t  gimxOtid_t (

#    if PROTOTYPES
        ComxChannel  channel,
        otid_t  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        otid_t  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, formatID))) ;
    CHECK (comxLong (channel, NULL_OR (value, bqual_length))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, tid))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTransactions */

/*!*****************************************************************************
    gimxTransIdentity() - decode/encode/erase a CORBA TransIdentity structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions

errno_t  gimxTransIdentity (

#    if PROTOTYPES
        ComxChannel  channel,
        TransIdentity  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TransIdentity  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, coord))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, term))) ;
    CHECK (gimxOtid_t (channel, NULL_OR (value, otid))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTransactions */

/*!*****************************************************************************
    gimxPropagationContext() - decode/encode/erase a CORBA PropagationContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions

errno_t  gimxPropagationContext (

#    if PROTOTYPES
        ComxChannel  channel,
        PropagationContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropagationContext  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, timeout))) ;
    CHECK (gimxTransIdentity (channel, NULL_OR (value, current))) ;
    CHECK (comxSequence (channel, NULL_OR (value, parents), (ComxFunc) gimxTransIdentity, sizeof (TransIdentity))) ;
    CHECK (gimxAny (channel, NULL_OR (value, implementation_specific_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTransactions */

/*!*****************************************************************************
    gimxLock_mode() - decode/encode/erase a CORBA lock_mode enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl

errno_t  gimxLock_mode (

#    if PROTOTYPES
        ComxChannel  channel,
        lock_mode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        lock_mode  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (lock_mode) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosConcurrencyControl */

/*!*****************************************************************************
    gimxNVPair() - decode/encode/erase a CORBA NVPair structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection

errno_t  gimxNVPair (

#    if PROTOTYPES
        ComxChannel  channel,
        NVPair  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NVPair  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxAny (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosCollection */

/*!*****************************************************************************
    gimxParameterList() - decode/encode/erase a sequence of CORBA NVPair structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection

errno_t  gimxParameterList (

#    if PROTOTYPES
        ComxChannel  channel,
        ParameterList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ParameterList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNVPair, sizeof (NVPair))) ;
}

#endif	/* IDL_MODULE_CosCollection */

/*!*****************************************************************************
    gimxIteratorInvalidReason() - decode/encode/erase a CORBA IteratorInvalidReason enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection

errno_t  gimxIteratorInvalidReason (

#    if PROTOTYPES
        ComxChannel  channel,
        IteratorInvalidReason  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        IteratorInvalidReason  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (IteratorInvalidReason) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosCollection */

/*!*****************************************************************************
    gimxElementInvalidReason() - decode/encode/erase a CORBA ElementInvalidReason enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection

errno_t  gimxElementInvalidReason (

#    if PROTOTYPES
        ComxChannel  channel,
        ElementInvalidReason  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ElementInvalidReason  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ElementInvalidReason) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosCollection */

/*!*****************************************************************************
    gimxLowerBoundStyle() - decode/encode/erase a CORBA LowerBoundStyle enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection

errno_t  gimxLowerBoundStyle (

#    if PROTOTYPES
        ComxChannel  channel,
        LowerBoundStyle  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LowerBoundStyle  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (LowerBoundStyle) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosCollection */

/*!*****************************************************************************
    gimxUpperBoundStyle() - decode/encode/erase a CORBA UpperBoundStyle enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection

errno_t  gimxUpperBoundStyle (

#    if PROTOTYPES
        ComxChannel  channel,
        UpperBoundStyle  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UpperBoundStyle  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (UpperBoundStyle) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosCollection */

/*!*****************************************************************************
    gimxRoles() - decode/encode/erase a sequence of CORBA Role structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxRoles (

#    if PROTOTYPES
        ComxChannel  channel,
        Roles  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Roles  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxRoleNames() - decode/encode/erase a sequence of CORBA RoleName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxRoleNames (

#    if PROTOTYPES
        ComxChannel  channel,
        RoleNames  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RoleNames  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxNamedRole() - decode/encode/erase a CORBA NamedRole structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxNamedRole (

#    if PROTOTYPES
        ComxChannel  channel,
        NamedRole  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NamedRole  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, aRole))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxNamedRoles() - decode/encode/erase a sequence of CORBA NamedRole structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxNamedRoles (

#    if PROTOTYPES
        ComxChannel  channel,
        NamedRoles  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NamedRoles  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNamedRole, sizeof (NamedRole))) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxRelationshipHandle() - decode/encode/erase a CORBA RelationshipHandle structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxRelationshipHandle (

#    if PROTOTYPES
        ComxChannel  channel,
        RelationshipHandle  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RelationshipHandle  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, the_relationship))) ;
    CHECK (comxULong (channel, NULL_OR (value, constant_random_id))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxRelationshipHandles() - decode/encode/erase a sequence of CORBA RelationshipHandle structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxRelationshipHandles (

#    if PROTOTYPES
        ComxChannel  channel,
        RelationshipHandles  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RelationshipHandles  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxRelationshipHandle, sizeof (RelationshipHandle))) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxNamedRoleType() - decode/encode/erase a CORBA NamedRoleType structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxNamedRoleType (

#    if PROTOTYPES
        ComxChannel  channel,
        NamedRoleType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NamedRoleType  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, named_role_type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxNamedRoleTypes() - decode/encode/erase a sequence of CORBA NamedRoleType structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxNamedRoleTypes (

#    if PROTOTYPES
        ComxChannel  channel,
        NamedRoleTypes  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NamedRoleTypes  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNamedRoleType, sizeof (NamedRoleType))) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxInterfaceDefs() - decode/encode/erase a sequence of CORBA InterfaceDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosRelationships) || IDL_MODULE_CosRelationships

errno_t  gimxInterfaceDefs (

#    if PROTOTYPES
        ComxChannel  channel,
        InterfaceDefs  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        InterfaceDefs  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CosRelationships */

/*!*****************************************************************************
    gimxNodeHandle() - decode/encode/erase a CORBA NodeHandle structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxNodeHandle (

#    if PROTOTYPES
        ComxChannel  channel,
        NodeHandle  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NodeHandle  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, the_node))) ;
    CHECK (comxULong (channel, NULL_OR (value, constant_random_id))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxNodeHandles() - decode/encode/erase a sequence of CORBA NodeHandle structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxNodeHandles (

#    if PROTOTYPES
        ComxChannel  channel,
        NodeHandles  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NodeHandles  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNodeHandle, sizeof (NodeHandle))) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxEndPoint() - decode/encode/erase a CORBA EndPoint structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxEndPoint (

#    if PROTOTYPES
        ComxChannel  channel,
        EndPoint  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EndPoint  *value ;
#    endif

{
    CHECK (gimxNodeHandle (channel, NULL_OR (value, the_node))) ;
    CHECK (gimxNamedRole (channel, NULL_OR (value, the_role))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxEndPoints() - decode/encode/erase a sequence of CORBA EndPoint structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxEndPoints (

#    if PROTOTYPES
        ComxChannel  channel,
        EndPoints  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EndPoints  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxEndPoint, sizeof (EndPoint))) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxEdge() - decode/encode/erase a CORBA Edge structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxEdge (

#    if PROTOTYPES
        ComxChannel  channel,
        Edge  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Edge  *value ;
#    endif

{
    CHECK (gimxEndPoint (channel, NULL_OR (value, from))) ;
    CHECK (gimxRelationshipHandle (channel, NULL_OR (value, the_relationship))) ;
    CHECK (gimxEndPoints (channel, NULL_OR (value, relatives))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxEdges() - decode/encode/erase a sequence of CORBA Edge structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxEdges (

#    if PROTOTYPES
        ComxChannel  channel,
        Edges  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Edges  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxEdge, sizeof (Edge))) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxPropagationValue() - decode/encode/erase a CORBA PropagationValue enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxPropagationValue (

#    if PROTOTYPES
        ComxChannel  channel,
        PropagationValue  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropagationValue  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (PropagationValue) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxMode() - decode/encode/erase a CORBA Mode enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxMode (

#    if PROTOTYPES
        ComxChannel  channel,
        Mode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Mode  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (Mode) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxScopedEndPoint() - decode/encode/erase a CORBA ScopedEndPoint structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxScopedEndPoint (

#    if PROTOTYPES
        ComxChannel  channel,
        ScopedEndPoint  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ScopedEndPoint  *value ;
#    endif

{
    CHECK (gimxEndPoint (channel, NULL_OR (value, point))) ;
    CHECK (comxULong (channel, NULL_OR (value, id))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxScopedEndPoints() - decode/encode/erase a sequence of CORBA ScopedEndPoint structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxScopedEndPoints (

#    if PROTOTYPES
        ComxChannel  channel,
        ScopedEndPoints  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ScopedEndPoints  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxScopedEndPoint, sizeof (ScopedEndPoint))) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxScopedRelationship() - decode/encode/erase a CORBA ScopedRelationship structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxScopedRelationship (

#    if PROTOTYPES
        ComxChannel  channel,
        ScopedRelationship  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ScopedRelationship  *value ;
#    endif

{
    CHECK (gimxRelationshipHandle (channel, NULL_OR (value, scoped_relationship))) ;
    CHECK (comxULong (channel, NULL_OR (value, id))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxScopedEdge() - decode/encode/erase a CORBA ScopedEdge structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxScopedEdge (

#    if PROTOTYPES
        ComxChannel  channel,
        ScopedEdge  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ScopedEdge  *value ;
#    endif

{
    CHECK (gimxScopedEndPoint (channel, NULL_OR (value, from))) ;
    CHECK (gimxScopedRelationship (channel, NULL_OR (value, the_relationship))) ;
    CHECK (gimxScopedEndPoints (channel, NULL_OR (value, relatives))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxScopedEdges() - decode/encode/erase a sequence of CORBA ScopedEdge structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxScopedEdges (

#    if PROTOTYPES
        ComxChannel  channel,
        ScopedEdges  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ScopedEdges  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxScopedEdge, sizeof (ScopedEdge))) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxWeightedEdge() - decode/encode/erase a CORBA WeightedEdge structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxWeightedEdge (

#    if PROTOTYPES
        ComxChannel  channel,
        WeightedEdge  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        WeightedEdge  *value ;
#    endif

{
    CHECK (gimxEdge (channel, NULL_OR (value, the_edge))) ;
    CHECK (comxULong (channel, NULL_OR (value, weight))) ;
    CHECK (comxSequence (channel, NULL_OR (value, next_nodes), (ComxFunc) gimxNodeHandle, sizeof (NodeHandle))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxWeightedEdges() - decode/encode/erase a sequence of CORBA WeightedEdge structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs

errno_t  gimxWeightedEdges (

#    if PROTOTYPES
        ComxChannel  channel,
        WeightedEdges  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        WeightedEdges  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxWeightedEdge, sizeof (WeightedEdge))) ;
}

#endif	/* IDL_MODULE_CosGraphs */

/*!*****************************************************************************
    gimxNameComponent() - decode/encode/erase a CORBA NameComponent structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming

errno_t  gimxNameComponent (

#    if PROTOTYPES
        ComxChannel  channel,
        NameComponent  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NameComponent  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, id))) ;
    CHECK (comxString (channel, NULL_OR (value, kind))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosNaming */

/*!*****************************************************************************
    gimxName() - decode/encode/erase a sequence of CORBA NameComponent structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming

errno_t  gimxName (

#    if PROTOTYPES
        ComxChannel  channel,
        Name  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Name  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNameComponent, sizeof (NameComponent))) ;
}

#endif	/* IDL_MODULE_CosNaming */

/*!*****************************************************************************
    gimxBindingType() - decode/encode/erase a CORBA BindingType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming

errno_t  gimxBindingType (

#    if PROTOTYPES
        ComxChannel  channel,
        BindingType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        BindingType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (BindingType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosNaming */

/*!*****************************************************************************
    gimxBinding() - decode/encode/erase a CORBA Binding structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming

errno_t  gimxBinding (

#    if PROTOTYPES
        ComxChannel  channel,
        Binding  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Binding  *value ;
#    endif

{
    CHECK (gimxName (channel, NULL_OR (value, binding_name))) ;
    CHECK (gimxBindingType (channel, NULL_OR (value, binding_type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosNaming */

/*!*****************************************************************************
    gimxBindingList() - decode/encode/erase a sequence of CORBA Binding structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming

errno_t  gimxBindingList (

#    if PROTOTYPES
        ComxChannel  channel,
        BindingList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        BindingList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxBinding, sizeof (Binding))) ;
}

#endif	/* IDL_MODULE_CosNaming */

/*!*****************************************************************************
    gimxNotFoundReason() - decode/encode/erase a CORBA NotFoundReason enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming

errno_t  gimxNotFoundReason (

#    if PROTOTYPES
        ComxChannel  channel,
        NotFoundReason  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NotFoundReason  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (NotFoundReason) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosNaming */

/*!*****************************************************************************
    gimxFactories() - decode/encode/erase a sequence of CORBA Factory structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle

errno_t  gimxFactories (

#    if PROTOTYPES
        ComxChannel  channel,
        Factories  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Factories  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CosLifeCycle */

/*!*****************************************************************************
    gimxNameValuePair() - decode/encode/erase a CORBA NameValuePair structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle

errno_t  gimxNameValuePair (

#    if PROTOTYPES
        ComxChannel  channel,
        NameValuePair  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NameValuePair  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxAny (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLifeCycle */

/*!*****************************************************************************
    gimxCriteria() - decode/encode/erase a sequence of CORBA NameValuePair structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLifeCycle) || IDL_MODULE_CosLifeCycle

errno_t  gimxCriteria (

#    if PROTOTYPES
        ComxChannel  channel,
        Criteria  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Criteria  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNameValuePair, sizeof (NameValuePair))) ;
}

#endif	/* IDL_MODULE_CosLifeCycle */

/*!*****************************************************************************
    gimxProperty() - decode/encode/erase a CORBA Property structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxProperty (

#    if PROTOTYPES
        ComxChannel  channel,
        Property  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Property  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, property_name))) ;
    CHECK (gimxAny (channel, NULL_OR (value, property_value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyModeType() - decode/encode/erase a CORBA PropertyModeType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyModeType (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyModeType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyModeType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (PropertyModeType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyDef() - decode/encode/erase a CORBA PropertyDef structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyDef (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyDef  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyDef  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, property_name))) ;
    CHECK (gimxAny (channel, NULL_OR (value, property_value))) ;
    CHECK (gimxPropertyModeType (channel, NULL_OR (value, property_mode))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyMode() - decode/encode/erase a CORBA PropertyMode structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyMode (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyMode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyMode  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, property_name))) ;
    CHECK (gimxPropertyModeType (channel, NULL_OR (value, property_mode))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyNames() - decode/encode/erase a sequence of CORBA PropertyName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyNames (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyNames  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyNames  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxProperties() - decode/encode/erase a sequence of CORBA Property structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxProperties (

#    if PROTOTYPES
        ComxChannel  channel,
        Properties  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Properties  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxProperty, sizeof (Property))) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyDefs() - decode/encode/erase a sequence of CORBA PropertyDef structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyDefs (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyDefs  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyDefs  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxPropertyDef, sizeof (PropertyDef))) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyModes() - decode/encode/erase a sequence of CORBA PropertyMode structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyModes (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyModes  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyModes  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxPropertyMode, sizeof (PropertyMode))) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyTypes() - decode/encode/erase a sequence of CORBA TypeCode structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyTypes (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyTypes  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyTypes  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxExceptionReason() - decode/encode/erase a CORBA ExceptionReason enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxExceptionReason (

#    if PROTOTYPES
        ComxChannel  channel,
        ExceptionReason  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExceptionReason  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ExceptionReason) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyException() - decode/encode/erase a CORBA PropertyException structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyException (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyException  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyException  *value ;
#    endif

{
    CHECK (gimxExceptionReason (channel, NULL_OR (value, reason))) ;
    CHECK (comxString (channel, NULL_OR (value, failing_property_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxPropertyExceptions() - decode/encode/erase a sequence of CORBA PropertyException structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService

errno_t  gimxPropertyExceptions (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyExceptions  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyExceptions  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxPropertyException, sizeof (PropertyException))) ;
}

#endif	/* IDL_MODULE_CosPropertyService */

/*!*****************************************************************************
    gimxActionRequired() - decode/encode/erase a CORBA ActionRequired enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager

errno_t  gimxActionRequired (

#    if PROTOTYPES
        ComxChannel  channel,
        ActionRequired  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ActionRequired  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ActionRequired) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLicensingManager */

/*!*****************************************************************************
    gimxAnswer() - decode/encode/erase a CORBA Answer enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager

errno_t  gimxAnswer (

#    if PROTOTYPES
        ComxChannel  channel,
        Answer  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Answer  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (Answer) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLicensingManager */

/*!*****************************************************************************
    gimxAction() - decode/encode/erase a CORBA Action structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager

errno_t  gimxAction (

#    if PROTOTYPES
        ComxChannel  channel,
        Action  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Action  *value ;
#    endif

{
    CHECK (gimxActionRequired (channel, NULL_OR (value, action))) ;
    CHECK (gimxAnswer (channel, NULL_OR (value, notification_required))) ;
    CHECK (gimxAnswer (channel, NULL_OR (value, wait_for_user_confirmation_after_notification))) ;
    CHECK (comxULong (channel, NULL_OR (value, notification_duration))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, producer_notification))) ;
    CHECK (comxString (channel, NULL_OR (value, notification_text))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLicensingManager */

/*!*****************************************************************************
    gimxChallengeData() - decode/encode/erase a CORBA ChallengeData structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager

errno_t  gimxChallengeData (

#    if PROTOTYPES
        ComxChannel  channel,
        ChallengeData  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ChallengeData  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, challenge_index))) ;
    CHECK (comxULong (channel, NULL_OR (value, random_number))) ;
    CHECK (comxString (channel, NULL_OR (value, digest))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLicensingManager */

/*!*****************************************************************************
    gimxChallengeProtocol() - decode/encode/erase a CORBA ChallengeProtocol enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager

errno_t  gimxChallengeProtocol (

#    if PROTOTYPES
        ComxChannel  channel,
        ChallengeProtocol  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ChallengeProtocol  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ChallengeProtocol) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLicensingManager */

/*!*****************************************************************************
    gimxChallenge() - decode/encode/erase a CORBA Challenge structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager

errno_t  gimxChallenge (

#    if PROTOTYPES
        ComxChannel  channel,
        Challenge  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Challenge  *value ;
#    endif

{
    CHECK (gimxChallengeProtocol (channel, NULL_OR (value, challenge_protocol))) ;
    CHECK (comxULong (channel, NULL_OR (value, challenge_data_size))) ;
    CHECK (gimxAny (channel, NULL_OR (value, challenge_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosLicensingManager */

/*!*****************************************************************************
    gimxOperation() - decode/encode/erase a CORBA Operation enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosCompoundLifeCycle) || IDL_MODULE_CosCompoundLifeCycle

errno_t  gimxOperation (

#    if PROTOTYPES
        ComxChannel  channel,
        Operation  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Operation  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (Operation) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosCompoundLifeCycle */

/*!*****************************************************************************
    gimxValueType() - decode/encode/erase a CORBA ValueType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection

errno_t  gimxValueType (

#    if PROTOTYPES
        ComxChannel  channel,
        ValueType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ValueType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ValueType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosQueryCollection */

/*!*****************************************************************************
    gimxDecimal() - decode/encode/erase a CORBA Decimal structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection

errno_t  gimxDecimal (

#    if PROTOTYPES
        ComxChannel  channel,
        Decimal  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Decimal  *value ;
#    endif

{
    CHECK (comxLong (channel, NULL_OR (value, precision))) ;
    CHECK (comxLong (channel, NULL_OR (value, scale))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosQueryCollection */

/*!*****************************************************************************
    gimxValue() - decode/encode/erase a CORBA Value structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection

errno_t  gimxValue (

#    if PROTOTYPES
        ComxChannel  channel,
        Value  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Value  *value ;
#    endif

{
    CHECK (gimxValueType (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case TypeBoolean:
        CHECK (comxBoolean (channel, NULL_OR (value, data.b))) ;
        break ;
    case TypeChar:
        CHECK (comxChar (channel, NULL_OR (value, data.c))) ;
        break ;
    case TypeOctet:
        CHECK (comxOctet (channel, NULL_OR (value, data.o))) ;
        break ;
    case TypeShort:
        CHECK (comxShort (channel, NULL_OR (value, data.s))) ;
        break ;
    case TypeUShort:
        CHECK (comxUShort (channel, NULL_OR (value, data.us))) ;
        break ;
    case TypeLong:
        CHECK (comxLong (channel, NULL_OR (value, data.l))) ;
        break ;
    case TypeULong:
        CHECK (comxULong (channel, NULL_OR (value, data.ul))) ;
        break ;
    case TypeFloat:
        CHECK (comxFloat (channel, NULL_OR (value, data.f))) ;
        break ;
    case TypeDouble:
        CHECK (comxDouble (channel, NULL_OR (value, data.d))) ;
        break ;
    case TypeString:
        CHECK (comxString (channel, NULL_OR (value, data.str))) ;
        break ;
    case TypeObject:
        CHECK (gimxIOR (channel, NULL_OR (value, data.obj))) ;
        break ;
    case TypeAny:
        CHECK (gimxAny (channel, NULL_OR (value, data.a))) ;
        break ;
    case TypeSmallInt:
        CHECK (comxShort (channel, NULL_OR (value, data.si))) ;
        break ;
    case TypeInteger:
        CHECK (comxLong (channel, NULL_OR (value, data.i))) ;
        break ;
    case TypeReal:
        CHECK (comxFloat (channel, NULL_OR (value, data.r))) ;
        break ;
    case TypeDoublePrecision:
        CHECK (comxDouble (channel, NULL_OR (value, data.dp))) ;
        break ;
    case TypeCharacter:
        CHECK (comxString (channel, NULL_OR (value, data.ch))) ;
        break ;
    case TypeDecimal:
        CHECK (gimxDecimal (channel, NULL_OR (value, data.dec))) ;
        break ;
    case TypeNumeric:
        CHECK (gimxDecimal (channel, NULL_OR (value, data.n))) ;
        break ;
    default:
        SET_ERRNO (EINVAL) ;
        LGE "(gimxValue) Invalid switch: %lu\n", (unsigned long) value->which) ;
        return (errno) ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_CosQueryCollection */

/*!*****************************************************************************
    gimxFieldValue() - decode/encode/erase a CORBA FieldValue structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection

errno_t  gimxFieldValue (

#    if PROTOTYPES
        ComxChannel  channel,
        FieldValue  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        FieldValue  *value ;
#    endif

{
    CHECK (comxBoolean (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case FALSE:
        CHECK (gimxValue (channel, NULL_OR (value, data.v))) ;
        break ;
    default:
        SET_ERRNO (EINVAL) ;
        LGE "(gimxFieldValue) Invalid switch: %lu\n", (unsigned long) value->which) ;
        return (errno) ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_CosQueryCollection */

/*!*****************************************************************************
    gimxRecord() - decode/encode/erase a sequence of CORBA FieldValue structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection

errno_t  gimxRecord (

#    if PROTOTYPES
        ComxChannel  channel,
        Record  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Record  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxFieldValue, sizeof (FieldValue))) ;
}

#endif	/* IDL_MODULE_CosQueryCollection */

/*!*****************************************************************************
    gimxQueryStatus() - decode/encode/erase a CORBA QueryStatus enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery

errno_t  gimxQueryStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        QueryStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        QueryStatus  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (QueryStatus) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosQuery */

/*!*****************************************************************************
    gimxQLTypes() - decode/encode/erase a sequence of CORBA QLType structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery

errno_t  gimxQLTypes (

#    if PROTOTYPES
        ComxChannel  channel,
        QLTypes  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        QLTypes  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_CosQuery */

/*!*****************************************************************************
    gimxUtcT() - decode/encode/erase a CORBA UtcT structure.
*******************************************************************************/

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase

errno_t  gimxUtcT (

#    if PROTOTYPES
        ComxChannel  channel,
        UtcT  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        UtcT  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, time))) ;
    CHECK (comxULong (channel, NULL_OR (value, inacclo))) ;
    CHECK (comxUShort (channel, NULL_OR (value, inacchi))) ;
    CHECK (comxShort (channel, NULL_OR (value, tdf))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_TimeBase */

/*!*****************************************************************************
    gimxIntervalT() - decode/encode/erase a CORBA IntervalT structure.
*******************************************************************************/

#if !defined(IDL_MODULE_TimeBase) || IDL_MODULE_TimeBase

errno_t  gimxIntervalT (

#    if PROTOTYPES
        ComxChannel  channel,
        IntervalT  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        IntervalT  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, lower_bound))) ;
    CHECK (comxULongLong (channel, NULL_OR (value, upper_bound))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_TimeBase */

/*!*****************************************************************************
    gimxTimeComparison() - decode/encode/erase a CORBA TimeComparison enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime

errno_t  gimxTimeComparison (

#    if PROTOTYPES
        ComxChannel  channel,
        TimeComparison  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TimeComparison  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (TimeComparison) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTime */

/*!*****************************************************************************
    gimxComparisonType() - decode/encode/erase a CORBA ComparisonType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime

errno_t  gimxComparisonType (

#    if PROTOTYPES
        ComxChannel  channel,
        ComparisonType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ComparisonType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ComparisonType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTime */

/*!*****************************************************************************
    gimxOverlapType() - decode/encode/erase a CORBA OverlapType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime

errno_t  gimxOverlapType (

#    if PROTOTYPES
        ComxChannel  channel,
        OverlapType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OverlapType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (OverlapType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTime */

/*!*****************************************************************************
    gimxTimeType() - decode/encode/erase a CORBA TimeType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent

errno_t  gimxTimeType (

#    if PROTOTYPES
        ComxChannel  channel,
        TimeType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TimeType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (TimeType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTimerEvent */

/*!*****************************************************************************
    gimxEventStatus() - decode/encode/erase a CORBA EventStatus enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent

errno_t  gimxEventStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        EventStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EventStatus  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (EventStatus) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTimerEvent */

/*!*****************************************************************************
    gimxTimerEventT() - decode/encode/erase a CORBA TimerEventT structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent

errno_t  gimxTimerEventT (

#    if PROTOTYPES
        ComxChannel  channel,
        TimerEventT  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TimerEventT  *value ;
#    endif

{
    CHECK (gimxUtcT (channel, NULL_OR (value, utc))) ;
    CHECK (gimxAny (channel, NULL_OR (value, event_data))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTimerEvent */

/*!*****************************************************************************
    gimxPropertyNameSeq() - decode/encode/erase a sequence of CORBA PropertyName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxPropertyNameSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyNameSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyNameSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxPropertySeq() - decode/encode/erase a sequence of CORBA Property structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxPropertySeq (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertySeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertySeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxProperty, sizeof (Property))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxOffer() - decode/encode/erase a CORBA Offer structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxOffer (

#    if PROTOTYPES
        ComxChannel  channel,
        Offer  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Offer  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, reference))) ;
    CHECK (gimxPropertySeq (channel, NULL_OR (value, properties))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxOfferSeq() - decode/encode/erase a sequence of CORBA Offer structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxOfferSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        OfferSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OfferSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxOffer, sizeof (Offer))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxOfferIdSeq() - decode/encode/erase a sequence of CORBA OfferId structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxOfferIdSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        OfferIdSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OfferIdSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxFollowOption() - decode/encode/erase a CORBA FollowOption enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxFollowOption (

#    if PROTOTYPES
        ComxChannel  channel,
        FollowOption  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        FollowOption  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (FollowOption) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxLinkNameSeq() - decode/encode/erase a sequence of CORBA LinkName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxLinkNameSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        LinkNameSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LinkNameSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxPolicyNameSeq() - decode/encode/erase a sequence of CORBA PolicyName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxPolicyNameSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        PolicyNameSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PolicyNameSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxCOSTPolicy() - decode/encode/erase a CORBA COSTPolicy structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxCOSTPolicy (

#    if PROTOTYPES
        ComxChannel  channel,
        COSTPolicy  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        COSTPolicy  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxAny (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxCOSTPolicySeq() - decode/encode/erase a sequence of CORBA COSTPolicy structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxCOSTPolicySeq (

#    if PROTOTYPES
        ComxChannel  channel,
        COSTPolicySeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        COSTPolicySeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxCOSTPolicy, sizeof (COSTPolicy))) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxHowManyProps() - decode/encode/erase a CORBA HowManyProps enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxHowManyProps (

#    if PROTOTYPES
        ComxChannel  channel,
        HowManyProps  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        HowManyProps  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (HowManyProps) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxSpecifiedProps() - decode/encode/erase a CORBA SpecifiedProps structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxSpecifiedProps (

#    if PROTOTYPES
        ComxChannel  channel,
        SpecifiedProps  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SpecifiedProps  *value ;
#    endif

{
    CHECK (gimxHowManyProps (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case props_some:
        CHECK (gimxPropertyNameSeq (channel, NULL_OR (value, data.prop_names))) ;
        break ;
    default:
        SET_ERRNO (EINVAL) ;
        LGE "(gimxSpecifiedProps) Invalid switch: %lu\n", (unsigned long) value->which) ;
        return (errno) ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxOfferInfo() - decode/encode/erase a CORBA OfferInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxOfferInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        OfferInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OfferInfo  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, reference))) ;
    CHECK (comxString (channel, NULL_OR (value, type))) ;
    CHECK (gimxPropertySeq (channel, NULL_OR (value, properties))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxLinkInfo() - decode/encode/erase a CORBA LinkInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxLinkInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        LinkInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        LinkInfo  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, target))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, target_reg))) ;
    CHECK (gimxFollowOption (channel, NULL_OR (value, def_pass_on_follow_rule))) ;
    CHECK (gimxFollowOption (channel, NULL_OR (value, limiting_follow_rule))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxProxyInfo() - decode/encode/erase a CORBA ProxyInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading

errno_t  gimxProxyInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        ProxyInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ProxyInfo  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, type))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, target))) ;
    CHECK (gimxPropertySeq (channel, NULL_OR (value, properties))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, if_match_all))) ;
    CHECK (comxString (channel, NULL_OR (value, recipe))) ;
    CHECK (gimxCOSTPolicySeq (channel, NULL_OR (value, policies_to_pass_on))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTrading */

/*!*****************************************************************************
    gimxDynamicProp() - decode/encode/erase a CORBA DynamicProp structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingDynamic) || IDL_MODULE_CosTradingDynamic

errno_t  gimxDynamicProp (

#    if PROTOTYPES
        ComxChannel  channel,
        DynamicProp  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DynamicProp  *value ;
#    endif

{
    CHECK (gimxIOR (channel, NULL_OR (value, eval_if))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, returned_type))) ;
    CHECK (gimxAny (channel, NULL_OR (value, extra_info))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTradingDynamic */

/*!*****************************************************************************
    gimxServiceTypeNameSeq() - decode/encode/erase a sequence of CORBA ServiceTypeName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxServiceTypeNameSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        ServiceTypeNameSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ServiceTypeNameSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxPropStruct() - decode/encode/erase a CORBA PropStruct structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxPropStruct (

#    if PROTOTYPES
        ComxChannel  channel,
        PropStruct  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropStruct  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, name))) ;
    CHECK (gimxIOR (channel, NULL_OR (value, value_type))) ;
    CHECK (gimxPropertyMode (channel, NULL_OR (value, mode))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxPropStructSeq() - decode/encode/erase a sequence of CORBA PropStruct structures.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxPropStructSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        PropStructSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropStructSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxPropStruct, sizeof (PropStruct))) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxIncarnationNumber() - decode/encode/erase a CORBA IncarnationNumber structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxIncarnationNumber (

#    if PROTOTYPES
        ComxChannel  channel,
        IncarnationNumber  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        IncarnationNumber  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, high))) ;
    CHECK (comxULong (channel, NULL_OR (value, low))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxTypeStruct() - decode/encode/erase a CORBA TypeStruct structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxTypeStruct (

#    if PROTOTYPES
        ComxChannel  channel,
        TypeStruct  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TypeStruct  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, if_name))) ;
    CHECK (gimxPropStructSeq (channel, NULL_OR (value, props))) ;
    CHECK (gimxServiceTypeNameSeq (channel, NULL_OR (value, super_types))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, masked))) ;
    CHECK (gimxIncarnationNumber (channel, NULL_OR (value, incarnation))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxListOption() - decode/encode/erase a CORBA ListOption enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxListOption (

#    if PROTOTYPES
        ComxChannel  channel,
        ListOption  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ListOption  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ListOption) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxSpecifiedServiceTypes() - decode/encode/erase a CORBA SpecifiedServiceTypes structure.
*******************************************************************************/

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos

errno_t  gimxSpecifiedServiceTypes (

#    if PROTOTYPES
        ComxChannel  channel,
        SpecifiedServiceTypes  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SpecifiedServiceTypes  *value ;
#    endif

{
    CHECK (gimxListOption (channel, NULL_OR (value, which))) ;
    switch (value->which) {
    case since:
        CHECK (gimxIncarnationNumber (channel, NULL_OR (value, data.incarnation))) ;
        break ;
    default:
        SET_ERRNO (EINVAL) ;
        LGE "(gimxSpecifiedServiceTypes) Invalid switch: %lu\n", (unsigned long) value->which) ;
        return (errno) ;
    }
    return (0) ;
}

#endif	/* IDL_MODULE_CosTradingRepos */

/*!*****************************************************************************
    gimxDCESecurityMechanismInfo() - decode/encode/erase a CORBA DCESecurityMechanismInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_DCE_CIOPSecurity) || IDL_MODULE_DCE_CIOPSecurity

errno_t  gimxDCESecurityMechanismInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        DCESecurityMechanismInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DCESecurityMechanismInfo  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, authorization_service))) ;
    CHECK (comxSequence (channel, NULL_OR (value, components), (ComxFunc) gimxTaggedComponent, sizeof (TaggedComponent))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_DCE_CIOPSecurity */

/*!*****************************************************************************
    gimxCOSLCSKeys() - decode/encode/erase a sequence of CORBA Key structures.
*******************************************************************************/

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService

errno_t  gimxCOSLCSKeys (

#    if PROTOTYPES
        ComxChannel  channel,
        COSLCSKeys  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        COSLCSKeys  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxName, sizeof (Name))) ;
}

#endif	/* IDL_MODULE_LifeCycleService */

/*!*****************************************************************************
    gimxPropertyList() - decode/encode/erase a sequence of CORBA NameValuePair structures.
*******************************************************************************/

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService

errno_t  gimxPropertyList (

#    if PROTOTYPES
        ComxChannel  channel,
        PropertyList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PropertyList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNameValuePair, sizeof (NameValuePair))) ;
}

#endif	/* IDL_MODULE_LifeCycleService */

/*!*****************************************************************************
    gimxNameComponents() - decode/encode/erase a sequence of CORBA NameComponent structures.
*******************************************************************************/

#if !defined(IDL_MODULE_LifeCycleService) || IDL_MODULE_LifeCycleService

errno_t  gimxNameComponents (

#    if PROTOTYPES
        ComxChannel  channel,
        NameComponents  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NameComponents  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNameComponent, sizeof (NameComponent))) ;
}

#endif	/* IDL_MODULE_LifeCycleService */

/*!*****************************************************************************
    gimxExtensibleFamily() - decode/encode/erase a CORBA ExtensibleFamily structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxExtensibleFamily (

#    if PROTOTYPES
        ComxChannel  channel,
        ExtensibleFamily  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExtensibleFamily  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, family_definer))) ;
    CHECK (comxUShort (channel, NULL_OR (value, family))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSecurityMechandName() - decode/encode/erase a CORBA SecurityMechandName structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSecurityMechandName (

#    if PROTOTYPES
        ComxChannel  channel,
        SecurityMechandName  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SecurityMechandName  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, mech_type))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxMechanismTypeList() - decode/encode/erase a sequence of CORBA MechanismType structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxMechanismTypeList (

#    if PROTOTYPES
        ComxChannel  channel,
        MechanismTypeList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MechanismTypeList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxString, sizeof (char *))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSecurityMechandNameList() - decode/encode/erase a sequence of CORBA SecurityMechandName structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSecurityMechandNameList (

#    if PROTOTYPES
        ComxChannel  channel,
        SecurityMechandNameList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SecurityMechandNameList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxSecurityMechandName, sizeof (SecurityMechandName))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAttributeType() - decode/encode/erase a CORBA AttributeType structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAttributeType (

#    if PROTOTYPES
        ComxChannel  channel,
        AttributeType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AttributeType  *value ;
#    endif

{
    CHECK (gimxExtensibleFamily (channel, NULL_OR (value, attribute_family))) ;
    CHECK (comxULong (channel, NULL_OR (value, attribute_type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAttributeTypeList() - decode/encode/erase a sequence of CORBA AttributeType structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAttributeTypeList (

#    if PROTOTYPES
        ComxChannel  channel,
        AttributeTypeList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AttributeTypeList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAttributeType, sizeof (AttributeType))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSecAttribute() - decode/encode/erase a CORBA SecAttribute structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSecAttribute (

#    if PROTOTYPES
        ComxChannel  channel,
        SecAttribute  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SecAttribute  *value ;
#    endif

{
    CHECK (gimxAttributeType (channel, NULL_OR (value, attribute_type))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, defining_authority))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAttributeList() - decode/encode/erase a sequence of CORBA SecAttribute structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAttributeList (

#    if PROTOTYPES
        ComxChannel  channel,
        AttributeList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AttributeList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxSecAttribute, sizeof (SecAttribute))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAuthenticationStatus() - decode/encode/erase a CORBA AuthenticationStatus enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAuthenticationStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        AuthenticationStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuthenticationStatus  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (AuthenticationStatus) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAssociationStatus() - decode/encode/erase a CORBA AssociationStatus enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAssociationStatus (

#    if PROTOTYPES
        ComxChannel  channel,
        AssociationStatus  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AssociationStatus  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (AssociationStatus) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxCredentialType() - decode/encode/erase a CORBA CredentialType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxCredentialType (

#    if PROTOTYPES
        ComxChannel  channel,
        CredentialType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CredentialType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (CredentialType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxRight() - decode/encode/erase a CORBA Right structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxRight (

#    if PROTOTYPES
        ComxChannel  channel,
        Right  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Right  *value ;
#    endif

{
    CHECK (gimxExtensibleFamily (channel, NULL_OR (value, rights_family))) ;
    CHECK (comxString (channel, NULL_OR (value, right))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxRightsList() - decode/encode/erase a sequence of CORBA Right structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxRightsList (

#    if PROTOTYPES
        ComxChannel  channel,
        RightsList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RightsList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxRight, sizeof (Right))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxRightsCombinator() - decode/encode/erase a CORBA RightsCombinator enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxRightsCombinator (

#    if PROTOTYPES
        ComxChannel  channel,
        RightsCombinator  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RightsCombinator  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (RightsCombinator) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxDelegationState() - decode/encode/erase a CORBA DelegationState enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxDelegationState (

#    if PROTOTYPES
        ComxChannel  channel,
        DelegationState  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DelegationState  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (DelegationState) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSecurityFeature() - decode/encode/erase a CORBA SecurityFeature enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSecurityFeature (

#    if PROTOTYPES
        ComxChannel  channel,
        SecurityFeature  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SecurityFeature  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (SecurityFeature) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSecurityFeatureValue() - decode/encode/erase a CORBA SecurityFeatureValue structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSecurityFeatureValue (

#    if PROTOTYPES
        ComxChannel  channel,
        SecurityFeatureValue  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SecurityFeatureValue  *value ;
#    endif

{
    CHECK (gimxSecurityFeature (channel, NULL_OR (value, feature))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSecurityFeatureValueList() - decode/encode/erase a sequence of CORBA SecurityFeatureValue structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSecurityFeatureValueList (

#    if PROTOTYPES
        ComxChannel  channel,
        SecurityFeatureValueList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SecurityFeatureValueList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxSecurityFeatureValue, sizeof (SecurityFeatureValue))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxQOP() - decode/encode/erase a CORBA QOP enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxQOP (

#    if PROTOTYPES
        ComxChannel  channel,
        QOP  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        QOP  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (QOP) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxRequiresSupports() - decode/encode/erase a CORBA RequiresSupports enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxRequiresSupports (

#    if PROTOTYPES
        ComxChannel  channel,
        RequiresSupports  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequiresSupports  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (RequiresSupports) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxCommunicationDirection() - decode/encode/erase a CORBA CommunicationDirection enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxCommunicationDirection (

#    if PROTOTYPES
        ComxChannel  channel,
        CommunicationDirection  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CommunicationDirection  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (CommunicationDirection) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxOptionsDirectionPair() - decode/encode/erase a CORBA OptionsDirectionPair structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxOptionsDirectionPair (

#    if PROTOTYPES
        ComxChannel  channel,
        OptionsDirectionPair  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OptionsDirectionPair  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, options))) ;
    CHECK (gimxCommunicationDirection (channel, NULL_OR (value, direction))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxOptionsDirectionPairList() - decode/encode/erase a sequence of CORBA OptionsDirectionPair structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxOptionsDirectionPairList (

#    if PROTOTYPES
        ComxChannel  channel,
        OptionsDirectionPairList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        OptionsDirectionPairList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxOptionsDirectionPair, sizeof (OptionsDirectionPair))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxDelegationMode() - decode/encode/erase a CORBA DelegationMode enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxDelegationMode (

#    if PROTOTYPES
        ComxChannel  channel,
        DelegationMode  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DelegationMode  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (DelegationMode) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxMechandOptions() - decode/encode/erase a CORBA MechandOptions structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxMechandOptions (

#    if PROTOTYPES
        ComxChannel  channel,
        MechandOptions  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MechandOptions  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, mechanism_type))) ;
    CHECK (comxUShort (channel, NULL_OR (value, options_supported))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxMechandOptionsList() - decode/encode/erase a sequence of CORBA MechandOptions structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxMechandOptionsList (

#    if PROTOTYPES
        ComxChannel  channel,
        MechandOptionsList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MechandOptionsList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxMechandOptions, sizeof (MechandOptions))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAuditEventType() - decode/encode/erase a CORBA AuditEventType structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAuditEventType (

#    if PROTOTYPES
        ComxChannel  channel,
        AuditEventType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuditEventType  *value ;
#    endif

{
    CHECK (gimxExtensibleFamily (channel, NULL_OR (value, event_family))) ;
    CHECK (comxUShort (channel, NULL_OR (value, event_type))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxAuditEventTypeList() - decode/encode/erase a sequence of CORBA AuditEventType structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxAuditEventTypeList (

#    if PROTOTYPES
        ComxChannel  channel,
        AuditEventTypeList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuditEventTypeList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAuditEventType, sizeof (AuditEventType))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSelectorValue() - decode/encode/erase a CORBA SelectorValue structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSelectorValue (

#    if PROTOTYPES
        ComxChannel  channel,
        SelectorValue  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SelectorValue  *value ;
#    endif

{
    CHECK (comxULong (channel, NULL_OR (value, selector))) ;
    CHECK (gimxAny (channel, NULL_OR (value, value))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxSelectorValueList() - decode/encode/erase a sequence of CORBA SelectorValue structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security

errno_t  gimxSelectorValueList (

#    if PROTOTYPES
        ComxChannel  channel,
        SelectorValueList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SelectorValueList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxSelectorValue, sizeof (SelectorValue))) ;
}

#endif	/* IDL_MODULE_Security */

/*!*****************************************************************************
    gimxCredentialsList() - decode/encode/erase a sequence of CORBA Credentials structures.
*******************************************************************************/

#if !defined(IDL_MODULE_SecurityLevel2) || IDL_MODULE_SecurityLevel2

errno_t  gimxCredentialsList (

#    if PROTOTYPES
        ComxChannel  channel,
        CredentialsList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CredentialsList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_SecurityLevel2 */

/*!*****************************************************************************
    gimxEvidenceType() - decode/encode/erase a CORBA EvidenceType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxEvidenceType (

#    if PROTOTYPES
        ComxChannel  channel,
        EvidenceType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EvidenceType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (EvidenceType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxNRVerificationResult() - decode/encode/erase a CORBA NRVerificationResult enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxNRVerificationResult (

#    if PROTOTYPES
        ComxChannel  channel,
        NRVerificationResult  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NRVerificationResult  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (NRVerificationResult) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxNRPolicyFeatures() - decode/encode/erase a CORBA NRPolicyFeatures structure.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxNRPolicyFeatures (

#    if PROTOTYPES
        ComxChannel  channel,
        NRPolicyFeatures  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NRPolicyFeatures  *value ;
#    endif

{
    CHECK (gimxExtensibleFamily (channel, NULL_OR (value, policy_id))) ;
    CHECK (comxULong (channel, NULL_OR (value, policy_version))) ;
    CHECK (comxString (channel, NULL_OR (value, mechanism))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxNRPolicyFeaturesList() - decode/encode/erase a sequence of CORBA NRPolicyFeatures structures.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxNRPolicyFeaturesList (

#    if PROTOTYPES
        ComxChannel  channel,
        NRPolicyFeaturesList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        NRPolicyFeaturesList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxNRPolicyFeatures, sizeof (NRPolicyFeatures))) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxRequestFeatures() - decode/encode/erase a CORBA RequestFeatures structure.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxRequestFeatures (

#    if PROTOTYPES
        ComxChannel  channel,
        RequestFeatures  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RequestFeatures  *value ;
#    endif

{
    CHECK (gimxNRPolicyFeatures (channel, NULL_OR (value, requested_policy))) ;
    CHECK (gimxEvidenceType (channel, NULL_OR (value, requested_evidence))) ;
    CHECK (comxString (channel, NULL_OR (value, requested_evidence_generators))) ;
    CHECK (comxString (channel, NULL_OR (value, requested_evidence_recipients))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, include_this_token_in_evidence))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxEvidenceDescriptor() - decode/encode/erase a CORBA EvidenceDescriptor structure.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxEvidenceDescriptor (

#    if PROTOTYPES
        ComxChannel  channel,
        EvidenceDescriptor  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EvidenceDescriptor  *value ;
#    endif

{
    CHECK (gimxEvidenceType (channel, NULL_OR (value, evidence_type))) ;
    CHECK (comxULong (channel, NULL_OR (value, evidence_validity_duration))) ;
    CHECK (comxBoolean (channel, NULL_OR (value, must_use_trusted_time))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxEvidenceDescriptorList() - decode/encode/erase a sequence of CORBA EvidenceDescriptor structures.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxEvidenceDescriptorList (

#    if PROTOTYPES
        ComxChannel  channel,
        EvidenceDescriptorList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        EvidenceDescriptorList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxEvidenceDescriptor, sizeof (EvidenceDescriptor))) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxAuthorityDescriptor() - decode/encode/erase a CORBA AuthorityDescriptor structure.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxAuthorityDescriptor (

#    if PROTOTYPES
        ComxChannel  channel,
        AuthorityDescriptor  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuthorityDescriptor  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, authority_name))) ;
    CHECK (comxString (channel, NULL_OR (value, authority_role))) ;
    CHECK (comxLong (channel, NULL_OR (value, last_revocation_check_offset))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxAuthorityDescriptorList() - decode/encode/erase a sequence of CORBA AuthorityDescriptor structures.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxAuthorityDescriptorList (

#    if PROTOTYPES
        ComxChannel  channel,
        AuthorityDescriptorList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        AuthorityDescriptorList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAuthorityDescriptor, sizeof (AuthorityDescriptor))) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxMechanismDescriptor() - decode/encode/erase a CORBA MechanismDescriptor structure.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxMechanismDescriptor (

#    if PROTOTYPES
        ComxChannel  channel,
        MechanismDescriptor  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MechanismDescriptor  *value ;
#    endif

{
    CHECK (comxString (channel, NULL_OR (value, mech_type))) ;
    CHECK (gimxAuthorityDescriptorList (channel, NULL_OR (value, authority_list))) ;
    CHECK (comxLong (channel, NULL_OR (value, max_time_skew))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxMechanismDescriptorList() - decode/encode/erase a sequence of CORBA MechanismDescriptor structures.
*******************************************************************************/

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService

errno_t  gimxMechanismDescriptorList (

#    if PROTOTYPES
        ComxChannel  channel,
        MechanismDescriptorList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        MechanismDescriptorList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxMechanismDescriptor, sizeof (MechanismDescriptor))) ;
}

#endif	/* IDL_MODULE_NRService */

/*!*****************************************************************************
    gimxTargetAssociationOptions() - decode/encode/erase a CORBA TargetAssociationOptions structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxTargetAssociationOptions (

#    if PROTOTYPES
        ComxChannel  channel,
        TargetAssociationOptions  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        TargetAssociationOptions  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxGenericMechanismInfo() - decode/encode/erase a CORBA GenericMechanismInfo structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxGenericMechanismInfo (

#    if PROTOTYPES
        ComxChannel  channel,
        GenericMechanismInfo  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        GenericMechanismInfo  *value ;
#    endif

{
    CHECK (comxOctetSeq (channel, NULL_OR (value, security_mechanism_type))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, mech_specific_data))) ;
    CHECK (comxSequence (channel, NULL_OR (value, components), (ComxFunc) gimxTaggedComponent, sizeof (TaggedComponent))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxContextIdDefn() - decode/encode/erase a CORBA ContextIdDefn enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxContextIdDefn (

#    if PROTOTYPES
        ComxChannel  channel,
        ContextIdDefn  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContextIdDefn  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ContextIdDefn) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxContinueEstablishContext() - decode/encode/erase a CORBA ContinueEstablishContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxContinueEstablishContext (

#    if PROTOTYPES
        ComxChannel  channel,
        ContinueEstablishContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContinueEstablishContext  *value ;
#    endif

{
    CHECK (comxULongLong (channel, NULL_OR (value, client_context_id))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, continuation_context_token))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxDiscardContext() - decode/encode/erase a CORBA DiscardContext structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxDiscardContext (

#    if PROTOTYPES
        ComxChannel  channel,
        DiscardContext  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        DiscardContext  *value ;
#    endif

{
    CHECK (gimxContextIdDefn (channel, NULL_OR (value, message_context_id_defn))) ;
    CHECK (comxULongLong (channel, NULL_OR (value, message_context_id))) ;
    CHECK (comxOctetSeq (channel, NULL_OR (value, discard_context_token))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxSECIOPMessageError() - decode/encode/erase a CORBA SECIOPMessageError structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxSECIOPMessageError (

#    if PROTOTYPES
        ComxChannel  channel,
        SECIOPMessageError  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SECIOPMessageError  *value ;
#    endif

{
    CHECK (gimxContextIdDefn (channel, NULL_OR (value, message_context_id_defn))) ;
    CHECK (comxULongLong (channel, NULL_OR (value, message_context_id))) ;
    CHECK (comxLong (channel, NULL_OR (value, major_status))) ;
    CHECK (comxLong (channel, NULL_OR (value, minor_status))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxContextTokenType() - decode/encode/erase a CORBA ContextTokenType enumerated type.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxContextTokenType (

#    if PROTOTYPES
        ComxChannel  channel,
        ContextTokenType  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ContextTokenType  *value ;
#    endif

{
    unsigned  long  enumeration ;
    if (value != NULL)  enumeration = (unsigned long) *value ;
    CHECK (comxEnum (channel, &enumeration)) ;
    if (value != NULL)  *value = (ContextTokenType) enumeration ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxSequencingHeader() - decode/encode/erase a CORBA SequencingHeader structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxSequencingHeader (

#    if PROTOTYPES
        ComxChannel  channel,
        SequencingHeader  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SequencingHeader  *value ;
#    endif

{
    CHECK (comxOctet (channel, NULL_OR (value, control_state))) ;
    CHECK (comxULong (channel, NULL_OR (value, direct_sequence_number))) ;
    CHECK (comxULong (channel, NULL_OR (value, reverse_sequence_number))) ;
    CHECK (comxULong (channel, NULL_OR (value, reverse_window))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxCryptographicProfileList() - decode/encode/erase a sequence of CORBA CryptographicProfile structures.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxCryptographicProfileList (

#    if PROTOTYPES
        ComxChannel  channel,
        CryptographicProfileList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CryptographicProfileList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) comxUShort, sizeof (unsigned short))) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxSPKM_1() - decode/encode/erase a CORBA SPKM_1 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxSPKM_1 (

#    if PROTOTYPES
        ComxChannel  channel,
        SPKM_1  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SPKM_1  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxCryptographicProfileList (channel, NULL_OR (value, crypto_profile))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxSPKM_2() - decode/encode/erase a CORBA SPKM_2 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxSPKM_2 (

#    if PROTOTYPES
        ComxChannel  channel,
        SPKM_2  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SPKM_2  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxCryptographicProfileList (channel, NULL_OR (value, crypto_profile))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxKerberosV5() - decode/encode/erase a CORBA KerberosV5 structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxKerberosV5 (

#    if PROTOTYPES
        ComxChannel  channel,
        KerberosV5  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        KerberosV5  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxCryptographicProfileList (channel, NULL_OR (value, crypto_profile))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxCSI_ECMA_Secret() - decode/encode/erase a CORBA CSI_ECMA_Secret structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxCSI_ECMA_Secret (

#    if PROTOTYPES
        ComxChannel  channel,
        CSI_ECMA_Secret  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CSI_ECMA_Secret  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxCryptographicProfileList (channel, NULL_OR (value, crypto_profile))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxCSI_ECMA_Hybrid() - decode/encode/erase a CORBA CSI_ECMA_Hybrid structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxCSI_ECMA_Hybrid (

#    if PROTOTYPES
        ComxChannel  channel,
        CSI_ECMA_Hybrid  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CSI_ECMA_Hybrid  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxCryptographicProfileList (channel, NULL_OR (value, crypto_profile))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxCSI_ECMA_Public() - decode/encode/erase a CORBA CSI_ECMA_Public structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP

errno_t  gimxCSI_ECMA_Public (

#    if PROTOTYPES
        ComxChannel  channel,
        CSI_ECMA_Public  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        CSI_ECMA_Public  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (gimxCryptographicProfileList (channel, NULL_OR (value, crypto_profile))) ;
    CHECK (comxString (channel, NULL_OR (value, security_name))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SECIOP */

/*!*****************************************************************************
    gimxSSL() - decode/encode/erase a CORBA SSL structure.
*******************************************************************************/

#if !defined(IDL_MODULE_SSLIOP) || IDL_MODULE_SSLIOP

errno_t  gimxSSL (

#    if PROTOTYPES
        ComxChannel  channel,
        SSL  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        SSL  *value ;
#    endif

{
    CHECK (comxUShort (channel, NULL_OR (value, target_supports))) ;
    CHECK (comxUShort (channel, NULL_OR (value, target_requires))) ;
    CHECK (comxUShort (channel, NULL_OR (value, port))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_SSLIOP */

/*!*****************************************************************************
    gimxParameter() - decode/encode/erase a CORBA Parameter structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Dynamic) || IDL_MODULE_Dynamic

errno_t  gimxParameter (

#    if PROTOTYPES
        ComxChannel  channel,
        Parameter  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        Parameter  *value ;
#    endif

{
    CHECK (gimxAny (channel, NULL_OR (value, argument))) ;
    CHECK (gimxParameterMode (channel, NULL_OR (value, mode))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Dynamic */

/*!*****************************************************************************
    gimxExceptionList() - decode/encode/erase a sequence of CORBA TypeCode structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Dynamic) || IDL_MODULE_Dynamic

errno_t  gimxExceptionList (

#    if PROTOTYPES
        ComxChannel  channel,
        ExceptionList  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        ExceptionList  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxIOR, sizeof (IOR))) ;
}

#endif	/* IDL_MODULE_Dynamic */

/*!*****************************************************************************
    gimxPriorityRange() - decode/encode/erase a CORBA PriorityRange structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging

errno_t  gimxPriorityRange (

#    if PROTOTYPES
        ComxChannel  channel,
        PriorityRange  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PriorityRange  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, min))) ;
    CHECK (comxShort (channel, NULL_OR (value, max))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Messaging */

/*!*****************************************************************************
    gimxRoutingTypeRange() - decode/encode/erase a CORBA RoutingTypeRange structure.
*******************************************************************************/

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging

errno_t  gimxRoutingTypeRange (

#    if PROTOTYPES
        ComxChannel  channel,
        RoutingTypeRange  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        RoutingTypeRange  *value ;
#    endif

{
    CHECK (comxShort (channel, NULL_OR (value, min))) ;
    CHECK (comxShort (channel, NULL_OR (value, max))) ;
    return (0) ;
}

#endif	/* IDL_MODULE_Messaging */

/*!*****************************************************************************
    gimxPolicyValueSeq() - decode/encode/erase a sequence of CORBA PolicyValue structures.
*******************************************************************************/

#if !defined(IDL_MODULE_Messaging) || IDL_MODULE_Messaging

errno_t  gimxPolicyValueSeq (

#    if PROTOTYPES
        ComxChannel  channel,
        PolicyValueSeq  *value)
#    else
        channel, value)

        ComxChannel  channel ;
        PolicyValueSeq  *value ;
#    endif

{
    return (comxSequence (channel, value, (ComxFunc) gimxAny, sizeof (Any))) ;
}

#endif	/* IDL_MODULE_Messaging */

/*******************************************************************************
    Lookup Tables - for converting named constants to numbers and vice-versa;
        see the coliToName() and coliToNumber() functions.
*******************************************************************************/

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    const  ColiMap  ActionRequiredLUT[] = {
      { (long) Continue, "Continue" },
      { (long) Terminate, "Terminate" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosLicensingManager */

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    const  ColiMap  AnswerLUT[] = {
      { (long) yes, "yes" },
      { (long) no, "no" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosLicensingManager */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  AssociationStatusLUT[] = {
      { (long) SecAssocSuccess, "SecAssocSuccess" },
      { (long) SecAssocFailure, "SecAssocFailure" },
      { (long) SecAssocContinue, "SecAssocContinue" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  AttributeModeLUT[] = {
      { (long) ATTR_NORMAL, "ATTR_NORMAL" },
      { (long) ATTR_READONLY, "ATTR_READONLY" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  AuthenticationStatusLUT[] = {
      { (long) SecAuthSuccess, "SecAuthSuccess" },
      { (long) SecAuthFailure, "SecAuthFailure" },
      { (long) SecAuthContinue, "SecAuthContinue" },
      { (long) SecAuthExpired, "SecAuthExpired" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    const  ColiMap  BindingTypeLUT[] = {
      { (long) nobject, "nobject" },
      { (long) ncontext, "ncontext" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosNaming */

#if !defined(IDL_MODULE_CosLicensingManager) || IDL_MODULE_CosLicensingManager
    const  ColiMap  ChallengeProtocolLUT[] = {
      { (long) default_protocol, "default_protocol" },
      { (long) producer_defined, "producer_defined" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosLicensingManager */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  CommunicationDirectionLUT[] = {
      { (long) SecDirectionBoth, "SecDirectionBoth" },
      { (long) SecDirectionRequest, "SecDirectionRequest" },
      { (long) SecDirectionReply, "SecDirectionReply" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    const  ColiMap  ComparisonTypeLUT[] = {
      { (long) IntervalC, "IntervalC" },
      { (long) MidC, "MidC" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTime */

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    const  ColiMap  ContextIdDefnLUT[] = {
      { (long) CIDClient, "CIDClient" },
      { (long) CIDPeer, "CIDPeer" },
      { (long) CIDSender, "CIDSender" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_SECIOP */

#if !defined(IDL_MODULE_SECIOP) || IDL_MODULE_SECIOP
    const  ColiMap  ContextTokenTypeLUT[] = {
      { (long) SecTokenTypeWrap, "SecTokenTypeWrap" },
      { (long) SecTokenTypeMIC, "SecTokenTypeMIC" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_SECIOP */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  CredentialTypeLUT[] = {
      { (long) SecInvocationCredentials, "SecInvocationCredentials" },
      { (long) SecNRCredentials, "SecNRCredentials" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  DefinitionKindLUT[] = {
      { (long) dk_none, "dk_none" },
      { (long) dk_all, "dk_all" },
      { (long) dk_Attribute, "dk_Attribute" },
      { (long) dk_Constant, "dk_Constant" },
      { (long) dk_Exception, "dk_Exception" },
      { (long) dk_Interface, "dk_Interface" },
      { (long) dk_Module, "dk_Module" },
      { (long) dk_Operation, "dk_Operation" },
      { (long) dk_Typedef, "dk_Typedef" },
      { (long) dk_Alias, "dk_Alias" },
      { (long) dk_Struct, "dk_Struct" },
      { (long) dk_Union, "dk_Union" },
      { (long) dk_Enum, "dk_Enum" },
      { (long) dk_Primitive, "dk_Primitive" },
      { (long) dk_String, "dk_String" },
      { (long) dk_Sequence, "dk_Sequence" },
      { (long) dk_Array, "dk_Array" },
      { (long) dk_Repository, "dk_Repository" },
      { (long) dk_Wstring, "dk_Wstring" },
      { (long) dk_Fixed, "dk_Fixed" },
      { (long) dk_Value, "dk_Value" },
      { (long) dk_ValueBox, "dk_ValueBox" },
      { (long) dk_ValueMember, "dk_ValueMember" },
      { (long) dk_Native, "dk_Native" },
      { (long) dk_AbstractInterface, "dk_AbstractInterface" },
      { (long) dk_LocalInterface, "dk_LocalInterface" },
      { (long) dk_Component, "dk_Component" },
      { (long) dk_Home, "dk_Home" },
      { (long) dk_Factory, "dk_Factory" },
      { (long) dk_Finder, "dk_Finder" },
      { (long) dk_Emits, "dk_Emits" },
      { (long) dk_Publishes, "dk_Publishes" },
      { (long) dk_Consumes, "dk_Consumes" },
      { (long) dk_Provides, "dk_Provides" },
      { (long) dk_Uses, "dk_Uses" },
      { (long) dk_Event, "dk_Event" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  DelegationModeLUT[] = {
      { (long) SecDelModeNoDelegation, "SecDelModeNoDelegation" },
      { (long) SecDelModeSimpleDelegation, "SecDelModeSimpleDelegation" },
      { (long) SecDelModeCompositeDelegation, "SecDelModeCompositeDelegation" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  DelegationStateLUT[] = {
      { (long) SecInitiator, "SecInitiator" },
      { (long) SecDelegate, "SecDelegate" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  EXCEPTcompletion_statusLUT[] = {
      { (long) COMPLETED_YES, "COMPLETED_YES" },
      { (long) COMPLETED_NO, "COMPLETED_NO" },
      { (long) COMPLETED_MAYBE, "COMPLETED_MAYBE" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    const  ColiMap  ElementInvalidReasonLUT[] = {
      { (long) element_type_invalid, "element_type_invalid" },
      { (long) positioning_property_invalid, "positioning_property_invalid" },
      { (long) element_exists, "element_exists" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosCollection */

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    const  ColiMap  EventStatusLUT[] = {
      { (long) ESTimeSet, "ESTimeSet" },
      { (long) ESTimeCleared, "ESTimeCleared" },
      { (long) ESTriggered, "ESTriggered" },
      { (long) ESFailedTrigger, "ESFailedTrigger" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTimerEvent */

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    const  ColiMap  EvidenceTypeLUT[] = {
      { (long) SecProofofCreation, "SecProofofCreation" },
      { (long) SecProofofReceipt, "SecProofofReceipt" },
      { (long) SecProofofApproval, "SecProofofApproval" },
      { (long) SecProofofRetrieval, "SecProofofRetrieval" },
      { (long) SecProofofOrigin, "SecProofofOrigin" },
      { (long) SecProofofDelivery, "SecProofofDelivery" },
      { (long) SecNoEvidence, "SecNoEvidence" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_NRService */

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    const  ColiMap  ExceptionReasonLUT[] = {
      { (long) invalid_property_name, "invalid_property_name" },
      { (long) conflicting_property, "conflicting_property" },
      { (long) property_not_found, "property_not_found" },
      { (long) unsupported_type_code, "unsupported_type_code" },
      { (long) unsupported_property, "unsupported_property" },
      { (long) unsupported_mode, "unsupported_mode" },
      { (long) fixed_property, "fixed_property" },
      { (long) read_only_property, "read_only_property" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosPropertyService */

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    const  ColiMap  FollowOptionLUT[] = {
      { (long) local_only, "local_only" },
      { (long) if_no_local, "if_no_local" },
      { (long) always, "always" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTrading */

#if !defined(IDL_MODULE_CosTrading) || IDL_MODULE_CosTrading
    const  ColiMap  HowManyPropsLUT[] = {
      { (long) props_none, "props_none" },
      { (long) props_some, "props_some" },
      { (long) props_all, "props_all" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTrading */

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    const  ColiMap  IteratorInvalidReasonLUT[] = {
      { (long) is_invalid, "is_invalid" },
      { (long) is_not_for_collection, "is_not_for_collection" },
      { (long) is_const, "is_const" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosCollection */

#if !defined(IDL_MODULE_CosTradingRepos) || IDL_MODULE_CosTradingRepos
    const  ColiMap  ListOptionLUT[] = {
      { (long) all, "all" },
      { (long) since, "since" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTradingRepos */

#if !defined(IDL_MODULE_Unknown) || IDL_MODULE_Unknown
    const  ColiMap  LocateStatusType_1_2LUT[] = {
      { (long) UNKNOWN_OBJECT, "UNKNOWN_OBJECT" },
      { (long) OBJECT_HERE, "OBJECT_HERE" },
      { (long) OBJECT_FORWARD, "OBJECT_FORWARD" },
      { (long) OBJECT_FORWARD_PERM, "OBJECT_FORWARD_PERM" },
      { (long) LOC_SYSTEM_EXCEPTION, "LOC_SYSTEM_EXCEPTION" },
      { (long) LOC_NEEDS_ADDRESSING_MODE, "LOC_NEEDS_ADDRESSING_MODE" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Unknown */

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    const  ColiMap  LowerBoundStyleLUT[] = {
      { (long) equal_lo, "equal_lo" },
      { (long) greater, "greater" },
      { (long) greater_or_equal, "greater_or_equal" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosCollection */

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    const  ColiMap  ModeLUT[] = {
      { (long) depthFirst, "depthFirst" },
      { (long) breadthFirst, "breadthFirst" },
      { (long) bestFirst, "bestFirst" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosGraphs */

#if !defined(IDL_MODULE_Unknown) || IDL_MODULE_Unknown
    const  ColiMap  MsgType_1_1LUT[] = {
      { (long) Request, "Request" },
      { (long) 0, "0" },
      { (long) Reply, "Reply" },
      { (long) CancelRequest, "CancelRequest" },
      { (long) LocateRequest, "LocateRequest" },
      { (long) LocateReply, "LocateReply" },
      { (long) CloseConnection, "CloseConnection" },
      { (long) MessageError, "MessageError" },
      { (long) Fragment, "Fragment" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Unknown */

#if !defined(IDL_MODULE_NRService) || IDL_MODULE_NRService
    const  ColiMap  NRVerificationResultLUT[] = {
      { (long) SecNRInvalid, "SecNRInvalid" },
      { (long) SecNRValid, "SecNRValid" },
      { (long) SecNRConditionallyValid, "SecNRConditionallyValid" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_NRService */

#if !defined(IDL_MODULE_CosNaming) || IDL_MODULE_CosNaming
    const  ColiMap  NotFoundReasonLUT[] = {
      { (long) missing_node, "missing_node" },
      { (long) not_context, "not_context" },
      { (long) not_object, "not_object" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosNaming */

#if !defined(IDL_MODULE_CosCompoundLifeCycle) || IDL_MODULE_CosCompoundLifeCycle
    const  ColiMap  OperationLUT[] = {
      { (long) Copy, "Copy" },
      { (long) Move, "Move" },
      { (long) Remove, "Remove" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosCompoundLifeCycle */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  OperationModeLUT[] = {
      { (long) OP_NORMAL, "OP_NORMAL" },
      { (long) OP_ONEWAY, "OP_ONEWAY" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    const  ColiMap  OverlapTypeLUT[] = {
      { (long) OTContainer, "OTContainer" },
      { (long) OTContained, "OTContained" },
      { (long) OTOverlap, "OTOverlap" },
      { (long) OTNoOverlap, "OTNoOverlap" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTime */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  ParameterModeLUT[] = {
      { (long) PARAM_IN, "PARAM_IN" },
      { (long) PARAM_OUT, "PARAM_OUT" },
      { (long) PARAM_INOUT, "PARAM_INOUT" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  PrimitiveKindLUT[] = {
      { (long) pk_null, "pk_null" },
      { (long) pk_void, "pk_void" },
      { (long) pk_short, "pk_short" },
      { (long) pk_long, "pk_long" },
      { (long) pk_ushort, "pk_ushort" },
      { (long) pk_ulong, "pk_ulong" },
      { (long) pk_float, "pk_float" },
      { (long) pk_double, "pk_double" },
      { (long) pk_boolean, "pk_boolean" },
      { (long) pk_char, "pk_char" },
      { (long) pk_octet, "pk_octet" },
      { (long) pk_any, "pk_any" },
      { (long) pk_TypeCode, "pk_TypeCode" },
      { (long) pk_Principal, "pk_Principal" },
      { (long) pk_string, "pk_string" },
      { (long) pk_objref, "pk_objref" },
      { (long) pk_longlong, "pk_longlong" },
      { (long) pk_ulonglong, "pk_ulonglong" },
      { (long) pk_longdouble, "pk_longdouble" },
      { (long) pk_wchar, "pk_wchar" },
      { (long) pk_wstring, "pk_wstring" },
      { (long) pk_value_base, "pk_value_base" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_CosGraphs) || IDL_MODULE_CosGraphs
    const  ColiMap  PropagationValueLUT[] = {
      { (long) deep, "deep" },
      { (long) shallow, "shallow" },
      { (long) none, "none" },
      { (long) inhibit, "inhibit" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosGraphs */

#if !defined(IDL_MODULE_CosPropertyService) || IDL_MODULE_CosPropertyService
    const  ColiMap  PropertyModeTypeLUT[] = {
      { (long) normal, "normal" },
      { (long) read_only, "read_only" },
      { (long) fixed_normal, "fixed_normal" },
      { (long) fixed_readonly, "fixed_readonly" },
      { (long) undefined, "undefined" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosPropertyService */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  QOPLUT[] = {
      { (long) SecQOPNoProtection, "SecQOPNoProtection" },
      { (long) SecQOPIntegrity, "SecQOPIntegrity" },
      { (long) SecQOPConfidentiality, "SecQOPConfidentiality" },
      { (long) SecQOPIntegrityAndConfidentiality, "SecQOPIntegrityAndConfidentiality" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CosQuery) || IDL_MODULE_CosQuery
    const  ColiMap  QueryStatusLUT[] = {
      { (long) complete, "complete" },
      { (long) incomplete, "incomplete" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosQuery */

#if !defined(IDL_MODULE_Unknown) || IDL_MODULE_Unknown
    const  ColiMap  ReplyStatusType_1_2LUT[] = {
      { (long) NO_EXCEPTION, "NO_EXCEPTION" },
      { (long) USER_EXCEPTION, "USER_EXCEPTION" },
      { (long) SYSTEM_EXCEPTION, "SYSTEM_EXCEPTION" },
      { (long) LOCATION_FORWARD, "LOCATION_FORWARD" },
      { (long) LOCATION_FORWARD_PERM, "LOCATION_FORWARD_PERM" },
      { (long) NEEDS_ADDRESSING_MODE, "NEEDS_ADDRESSING_MODE" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Unknown */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  RequiresSupportsLUT[] = {
      { (long) SecRequires, "SecRequires" },
      { (long) SecSupports, "SecSupports" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  RightsCombinatorLUT[] = {
      { (long) SecAllRights, "SecAllRights" },
      { (long) SecAnyRight, "SecAnyRight" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_Security) || IDL_MODULE_Security
    const  ColiMap  SecurityFeatureLUT[] = {
      { (long) SecNoDelegation, "SecNoDelegation" },
      { (long) SecSimpleDelegation, "SecSimpleDelegation" },
      { (long) SecCompositeDelegation, "SecCompositeDelegation" },
      { (long) SecNoProtection, "SecNoProtection" },
      { (long) SecIntegrity, "SecIntegrity" },
      { (long) SecConfidentiality, "SecConfidentiality" },
      { (long) SecIntegrityAndConfidentiality, "SecIntegrityAndConfidentiality" },
      { (long) SecDetectReplay, "SecDetectReplay" },
      { (long) SecDetectMisordering, "SecDetectMisordering" },
      { (long) SecEstablishTrustInTarget, "SecEstablishTrustInTarget" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_Security */

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    const  ColiMap  StatusLUT[] = {
      { (long) StatusActive, "StatusActive" },
      { (long) StatusMarkedRollback, "StatusMarkedRollback" },
      { (long) StatusPrepared, "StatusPrepared" },
      { (long) StatusCommitted, "StatusCommitted" },
      { (long) StatusRolledBack, "StatusRolledBack" },
      { (long) StatusUnknown, "StatusUnknown" },
      { (long) StatusNoTransaction, "StatusNoTransaction" },
      { (long) StatusPreparing, "StatusPreparing" },
      { (long) StatusCommitting, "StatusCommitting" },
      { (long) StatusRollingBack, "StatusRollingBack" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTransactions */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  TCKindLUT[] = {
      { (long) tk_null, "tk_null" },
      { (long) tk_void, "tk_void" },
      { (long) tk_short, "tk_short" },
      { (long) tk_long, "tk_long" },
      { (long) tk_ushort, "tk_ushort" },
      { (long) tk_ulong, "tk_ulong" },
      { (long) tk_float, "tk_float" },
      { (long) tk_double, "tk_double" },
      { (long) tk_boolean, "tk_boolean" },
      { (long) tk_char, "tk_char" },
      { (long) tk_octet, "tk_octet" },
      { (long) tk_any, "tk_any" },
      { (long) tk_TypeCode, "tk_TypeCode" },
      { (long) tk_Principal, "tk_Principal" },
      { (long) tk_objref, "tk_objref" },
      { (long) tk_struct, "tk_struct" },
      { (long) tk_union, "tk_union" },
      { (long) tk_enum, "tk_enum" },
      { (long) tk_string, "tk_string" },
      { (long) tk_sequence, "tk_sequence" },
      { (long) tk_array, "tk_array" },
      { (long) tk_alias, "tk_alias" },
      { (long) tk_except, "tk_except" },
      { (long) tk_longlong, "tk_longlong" },
      { (long) tk_ulonglong, "tk_ulonglong" },
      { (long) tk_longdouble, "tk_longdouble" },
      { (long) tk_wchar, "tk_wchar" },
      { (long) tk_wstring, "tk_wstring" },
      { (long) tk_fixed, "tk_fixed" },
      { (long) tk_value, "tk_value" },
      { (long) tk_value_box, "tk_value_box" },
      { (long) tk_native, "tk_native" },
      { (long) tk_abstract_interface, "tk_abstract_interface" },
      { (long) tk_local_interface, "tk_local_interface" },
      { (long) tk_component, "tk_component" },
      { (long) tk_home, "tk_home" },
      { (long) tk_event, "tk_event" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_CosTime) || IDL_MODULE_CosTime
    const  ColiMap  TimeComparisonLUT[] = {
      { (long) TCEqualTo, "TCEqualTo" },
      { (long) TCLessThan, "TCLessThan" },
      { (long) TCGreaterThan, "TCGreaterThan" },
      { (long) TCIndeterminate, "TCIndeterminate" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTime */

#if !defined(IDL_MODULE_CosTimerEvent) || IDL_MODULE_CosTimerEvent
    const  ColiMap  TimeTypeLUT[] = {
      { (long) TTAbsolute, "TTAbsolute" },
      { (long) TTRelative, "TTRelative" },
      { (long) TTPeriodic, "TTPeriodic" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTimerEvent */

#if !defined(IDL_MODULE_CosCollection) || IDL_MODULE_CosCollection
    const  ColiMap  UpperBoundStyleLUT[] = {
      { (long) equal_up, "equal_up" },
      { (long) less, "less" },
      { (long) less_or_equal, "less_or_equal" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosCollection */

#if !defined(IDL_MODULE_CosQueryCollection) || IDL_MODULE_CosQueryCollection
    const  ColiMap  ValueTypeLUT[] = {
      { (long) TypeBoolean, "TypeBoolean" },
      { (long) TypeChar, "TypeChar" },
      { (long) TypeOctet, "TypeOctet" },
      { (long) TypeShort, "TypeShort" },
      { (long) TypeUShort, "TypeUShort" },
      { (long) TypeLong, "TypeLong" },
      { (long) TypeULong, "TypeULong" },
      { (long) TypeFloat, "TypeFloat" },
      { (long) TypeDouble, "TypeDouble" },
      { (long) TypeString, "TypeString" },
      { (long) TypeObject, "TypeObject" },
      { (long) TypeAny, "TypeAny" },
      { (long) TypeSmallInt, "TypeSmallInt" },
      { (long) TypeInteger, "TypeInteger" },
      { (long) TypeReal, "TypeReal" },
      { (long) TypeDoublePrecision, "TypeDoublePrecision" },
      { (long) TypeCharacter, "TypeCharacter" },
      { (long) TypeDecimal, "TypeDecimal" },
      { (long) TypeNumeric, "TypeNumeric" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosQueryCollection */

#if !defined(IDL_MODULE_CosTransactions) || IDL_MODULE_CosTransactions
    const  ColiMap  VoteLUT[] = {
      { (long) VoteCommit, "VoteCommit" },
      { (long) VoteRollback, "VoteRollback" },
      { (long) VoteReadOnly, "VoteReadOnly" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosTransactions */

#if !defined(IDL_MODULE_CORBA) || IDL_MODULE_CORBA
    const  ColiMap  exception_typeLUT[] = {
      { (long) EXCEPT_NO_EXCEPTION, "EXCEPT_NO_EXCEPTION" },
      { (long) EXCEPT_USER_EXCEPTION, "EXCEPT_USER_EXCEPTION" },
      { (long) EXCEPT_SYSTEM_EXCEPTION, "EXCEPT_SYSTEM_EXCEPTION" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CORBA */

#if !defined(IDL_MODULE_CosConcurrencyControl) || IDL_MODULE_CosConcurrencyControl
    const  ColiMap  lock_modeLUT[] = {
      { (long) Read, "Read" },
      { (long) Write, "Write" },
      { (long) Upgrade, "Upgrade" },
      { (long) Intention_read, "Intention_read" },
      { (long) Intention_write, "Intention_write" },
      { 0L, NULL }
    } ;
#endif	/* IDL_MODULE_CosConcurrencyControl */
