# $Id: comx_post.py,v 1.2 2021/09/15 04:14:08 alex Exp $
#

global gCDRMap
gCDRMap = dict ()
gCDRMap["boolean"]		= "bool"
gCDRMap["char"]			= "char"
gCDRMap["double"]		= "double"
gCDRMap["float"]		= "float"
gCDRMap["long"]			= "long"
gCDRMap["long  double"]		= "LONGDOUBLE"
gCDRMap["long  long"]		= "LONGLONG"
gCDRMap["octet"]		= "octet"
gCDRMap["short"]		= "short"
gCDRMap["string"]		= "char  *"
gCDRMap["unsigned  long"]	= "unsigned  long"
gCDRMap["unsigned  long  long"]	= "ULONGLONG"
gCDRMap["unsigned  short"]	= "unsigned  short"
gCDRMap["wchar"]		= "wchar_t"
gCDRMap["wstring"]		= "wchar_t  *"

global gAliasMap
gAliasMap = dict ()
	# Sequences of basic CORBA types.
gAliasMap["sequence<boolean>"]			= "BooleanSeq"
gAliasMap["sequence<char>"]			= "CharSeq"
gAliasMap["sequence<double>"]			= "DoubleSeq"
gAliasMap["sequence<float>"]			= "FloatSeq"
gAliasMap["sequence<long>"]			= "LongSeq"
gAliasMap["sequence<long  double>"]		= "LongDoubleSeq"
gAliasMap["sequence<long  long>"]		= "LongLongSeq"
gAliasMap["sequence<octet>"]			= "OctetSeq"
gAliasMap["sequence<short>"]			= "ShortSeq"
gAliasMap["sequence<string>"]			= "StringSeq"
gAliasMap["sequence<unsigned  long>"]		= "ULongSeq"
gAliasMap["sequence<unsigned  long  long>"]	= "ULongLongSeq"
gAliasMap["sequence<unsigned  short>"]		= "UShortSeq"
gAliasMap["sequence<wchar>"]			= "WCharSeq"
gAliasMap["sequence<wstring>"]			= "WStringSeq"
	# Sequences of equivalent types.
gAliasMap["sequence<bool>"]			= "BooleanSeq"
gAliasMap["sequence<char  *>"]			= "StringSeq"
gAliasMap["sequence<wchar_t>"]			= "WCharSeq"
gAliasMap["sequence<wchar_t  *>"]		= "WStringSeq"

global gTypeModuleMap
gTypeModuleMap = dict ()
gTypeModuleMap["boolean"]		= "CORBA__CDR"
gTypeModuleMap["char"]			= "CORBA__CDR"
gTypeModuleMap["double"]		= "CORBA__CDR"
gTypeModuleMap["float"]			= "CORBA__CDR"
gTypeModuleMap["long"]			= "CORBA__CDR"
gTypeModuleMap["long  double"]		= "CORBA__CDR"
gTypeModuleMap["LONGDOUBLE"]		= "CORBA__CDR"
gTypeModuleMap["long  long"]		= "CORBA__CDR"
gTypeModuleMap["LONGLONG"]		= "CORBA__CDR"
gTypeModuleMap["octet"]			= "CORBA__CDR"
gTypeModuleMap["short"]			= "CORBA__CDR"
gTypeModuleMap["string"]		= "CORBA__CDR"
gTypeModuleMap["unsigned  long"]	= "CORBA__CDR"
gTypeModuleMap["unsigned  long  long"]	= "CORBA__CDR"
gTypeModuleMap["ULONGLONG"]		= "CORBA__CDR"
gTypeModuleMap["unsigned  short"]	= "CORBA__CDR"
gTypeModuleMap["wchar"]			= "CORBA__CDR"
gTypeModuleMap["wstring"]		= "CORBA__CDR"
gTypeModuleMap["Version"]		= "CORBA__CDR"
gTypeModuleMap["BooleanSeq"]		= "CORBA__CDR"
gTypeModuleMap["CharSeq"]		= "CORBA__CDR"
gTypeModuleMap["DoubleSeq"]		= "CORBA__CDR"
gTypeModuleMap["FloatSeq"]		= "CORBA__CDR"
gTypeModuleMap["LongSeq"]		= "CORBA__CDR"
gTypeModuleMap["LongDoubleSeq"]		= "CORBA__CDR"
gTypeModuleMap["LongLongSeq"]		= "CORBA__CDR"
gTypeModuleMap["OctetSeq"]		= "CORBA__CDR"
gTypeModuleMap["ShortSeq"]		= "CORBA__CDR"
gTypeModuleMap["StringSeq"]		= "CORBA__CDR"
gTypeModuleMap["ULongSeq"]		= "CORBA__CDR"
gTypeModuleMap["ULongLongSeq"]		= "CORBA__CDR"
gTypeModuleMap["UShortSeq"]		= "CORBA__CDR"
gTypeModuleMap["WCharSeq"]		= "CORBA__CDR"
gTypeModuleMap["WStringSeq"]		= "CORBA__CDR"

# List of basic CORBA data types implemented in the COMX utilities.
# Python scripts that include this file should append the types
# they implement to this list.

global gImplementedTypes
gImplementedTypes = set (sorted (gTypeModuleMap.keys (), key=str.upper))

global gMarshalMap
gMarshalMap = dict ()
	# Basic CORBA types.
gMarshalMap["bool"]			= "comxBoolean"
gMarshalMap["char"]			= "comxChar"
gMarshalMap["double"]			= "comxDouble"
gMarshalMap["float"]			= "comxFloat"
gMarshalMap["long"]			= "comxLong"
gMarshalMap["long  double"]		= "comxLongDouble"
gMarshalMap["LONGDOUBLE"]		= "comxLongDouble"
gMarshalMap["LONGLONG"]			= "comxLongLong"
gMarshalMap["long  long"]		= "comxLongLong"
gMarshalMap["octet"]			= "comxOctet"
gMarshalMap["short"]			= "comxShort"
gMarshalMap["char  *"]			= "comxString"
gMarshalMap["unsigned  long"]		= "comxULong"
gMarshalMap["ULONGLONG"]		= "comxULongLong"
gMarshalMap["unsigned  long  long"]	= "comxULongLong"
gMarshalMap["unsigned  short"]		= "comxUShort"
gMarshalMap["Version"]			= "comxVersion"
gMarshalMap["wchar_t"]			= "comxWChar"
gMarshalMap["wchar_t  *"]		= "comxWString"
	# Sequences of basic types.
gMarshalMap["BooleanSeq"]		= "comxBooleanSeq"
gMarshalMap["CharSeq"]			= "comxCharSeq"
gMarshalMap["DoubleSeq"]		= "comxDoubleSeq"
gMarshalMap["FloatSeq"]			= "comxFloatSeq"
gMarshalMap["LongSeq"]			= "comxLongSeq"
gMarshalMap["LongDoubleSeq"]		= "comxLongDoubleSeq"
gMarshalMap["LongLongSeq"]		= "comxLongLongSeq"
gMarshalMap["OctetSeq"]			= "comxOctetSeq"
gMarshalMap["ShortSeq"]			= "comxShortSeq"
gMarshalMap["StringSeq"]		= "comxStringSeq"
gMarshalMap["ULongSeq"]			= "comxULongSeq"
gMarshalMap["ULongLongSeq"]		= "comxULongLongSeq"
gMarshalMap["UShortSeq"]		= "comxUShortSeq"
gMarshalMap["WCharSeq"]			= "comxWCharSeq"
gMarshalMap["WStringSeq"]		= "comxWStringSeq"
