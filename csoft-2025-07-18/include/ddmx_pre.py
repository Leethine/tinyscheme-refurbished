# $Id: ddmx_pre.py,v 1.3 2021/09/15 04:15:43 alex Exp $
#


global gAliasMap, gMarshalMap, gModulePath, gRenameMap, gTypeModuleMap


#*******************************************************************************
#
#    declareForwardReferences () - output needed forward references.
#
#    My idl2h.py translator skips "local" and "valuetype" declarations.
#    The "dds_dlrl.idl" file has forward declarations for "valuetype"
#    ObjectRoot and some "local" interfaces.  My generated "ddmx_idl.h"
#    header file also needs forward references for them or it won't
#    compile.  I use IORs for the interfaces, as usual, and IOR for
#    the "valuetype" too.
#
#*******************************************************************************

def declareForwardReferences (name):

    print ()
    print ("/* Forward references for needed types. */")
    print ()
    print ("typedef  IOR  ObjectHome ;	/* \"local interface\" */")
    print ("typedef  IOR  ObjectListener ;	/* \"local interface\" */")
    print ("typedef  IOR  Selection ;	/* \"local interface\" */")
    print ("typedef  IOR  CacheBase ;	/* \"local interface\" */")
    print ("typedef  IOR  CacheAccess ;	/* \"local interface\" */")
    print ("typedef  IOR  CacheListener ;	/* \"local interface\" */")
    print ("typedef  IOR  Cache ;	/* \"local interface\" */")
    print ("typedef  IOR  Contract ;	/* \"local interface\" */")

    gTypeModuleMap["ObjectHome"] = gModulePath
    gMarshalMap["ObjectHome"] = "#define"
    addAlias ("ObjectHome", "IOR")

    gTypeModuleMap["ObjectListener"] = gModulePath
    gMarshalMap["ObjectListener"] = "#define"
    addAlias ("ObjectListener", "IOR")

    gTypeModuleMap["Selection"] = gModulePath
    gMarshalMap["Selection"] = "#define"
    addAlias ("Selection", "IOR")

    gTypeModuleMap["CacheBase"] = gModulePath
    gMarshalMap["CacheBase"] = "#define"
    addAlias ("CacheBase", "IOR")

    gTypeModuleMap["CacheAccess"] = gModulePath
    gMarshalMap["CacheAccess"] = "#define"
    addAlias ("CacheAccess", "IOR")

    gTypeModuleMap["CacheListener"] = gModulePath
    gMarshalMap["CacheListener"] = "#define"
    addAlias ("CacheListener", "IOR")

    gTypeModuleMap["Cache"] = gModulePath
    gMarshalMap["Cache"] = "#define"
    addAlias ("Cache", "IOR")

    gTypeModuleMap["Contract"] = gModulePath
    gMarshalMap["Contract"] = "#define"
    addAlias ("Contract", "IOR")

    gHookMap["DDS::ObjectHomeSeq"] = ""

    return ""


#    Add a prefix to the DCPSState enumeration values; "REGISTERED"
#    conflicts with a #define in Windows' "nb30.h" header file.

gRenameMap["DCPSState::INITIAL"]	= "DCPS_INITIAL"
gRenameMap["DCPSState::REGISTERED"]	= "DCPS_REGISTERED"
gRenameMap["DCPSState::ENABLED"]	= "DCPS_ENABLED"


#    Marshaling function names are too long for VMS linker.

operating_system_is_VMS = False
if operating_system_is_VMS:
    gRenameMap["DCPS::OfferedIncompatibleQosStatus"]	= "OfferedIncompatibleQosSt"
    gRenameMap["DCPS::RequestedDeadlineMissedStatus"]	= "RequestedDeadlineMissedSt"
    gRenameMap["DCPS::RequestedIncompatibleQosStatus"]	= "RequestedIncompatibleQosSt"
    gRenameMap["DCPS::DestinationOrderQosPolicyKind"]	= "DestinationOrderQosPolKind"
    gRenameMap["DCPS::PresentationQosPolicyAccessScopeKind"]	= "PresentationQosPolicyKind"


#    Hook to generate forward references when ObjectRootSeq is declared.

gHookMap["DDS::ObjectHomeSeq"] = declareForwardReferences

