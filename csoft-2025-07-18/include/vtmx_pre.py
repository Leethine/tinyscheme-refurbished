# $Id: vtmx_pre.py,v 1.1 2021/09/13 02:44:38 alex Exp $
#


global gHookMap, gMarshalMap, gRenameMap, gTypeModuleMap


gRenameMap["ClockProperty::Unknown"] = "TIMESCALE_Unknown"
gRenameMap["ClockProperty::TAI"] = "TIMESCALE_TAI"
gRenameMap["ClockProperty::UT0"] = "TIMESCALE_UT0"
gRenameMap["ClockProperty::UT1"] = "TIMESCALE_UT1"
gRenameMap["ClockProperty::UTC"] = "TIMESCALE_UTC"
gRenameMap["ClockProperty::TT"] = "TIMESCALE_TT"
gRenameMap["ClockProperty::TDB"] = "TIMESCALE_TDB"
gRenameMap["ClockProperty::TCG"] = "TIMESCALE_TCG"
gRenameMap["ClockProperty::TCB"] = "TIMESCALE_TCB"
gRenameMap["ClockProperty::Sidereal"] = "TIMESCALE_Sidereal"
gRenameMap["ClockProperty::_Local"] = "TIMESCALE__Local"
gRenameMap["ClockProperty::GPS"] = "TIMESCALE_GPS"
gRenameMap["ClockProperty::Other"] = "TIMESCALE_Other"


#*******************************************************************************
#    Hook to replace "typedef" of "ClockProperty::CoordinationService".
#    In the IDL, the base type of "Clock" is used before "Clock" itself
#    is declared in the enclosing module.
#*******************************************************************************

def declareCoordinationSource (name):

    print ()
    print ("typedef  IOR  CoordinationSource ;")

    gTypeModuleMap["CoordinationSource"] = gModulePath
    gMarshalMap["CoordinationSource"] = "#define"
    addAlias ("CoordinationSource", "IOR")

    gHookMap["ClockProperty::CoordinationService"] = "ignore"

    return "ignore"

gHookMap["ClockProperty::CoordinationSource"] = declareCoordinationSource
