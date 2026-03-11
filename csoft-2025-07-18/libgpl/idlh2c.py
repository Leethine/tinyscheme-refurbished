#! /usr/bin/env python3
#
# $Id: idlh2c.py,v 1.6 2021/09/20 02:06:28 alex Exp alex $
#
#*******************************************************************************
#
#    IDLH2C - scans an IDL2H-generated header file and generates the
#        marshaling functions for the defined data types.  The output
#        is written to standard output.
#
#*******************************************************************************


# For backwards compatibility with Python 2.7, disable 2.7's print statement.
from __future__ import print_function

import getopt, os, os.path, shlex, sys, traceback


gAppendSeq = 0			# Rewrite sloppy sequential type names?
gAutoSequence = 0		# Automatically generate sequence types?
gPostDefs = ""			# Output file for post-processing definitions.
gPrefix = "mx"			# Prefix for marshaling function names.

gCustomTypes = []		# Data types with custom marshaling functions.
gEnumModuleMap = {}		# Maps enumerated types to defining module.
gEnumValueMap = {}		# Maps enumerated types to value prefix/suffix.
gHookMap = {}			# Maps names to processing hooks.
gLUT = {}			# Lookup tables for enumerated types.
gRenameMap = {}			# Maps types to equivalent types.

#    AliasMap - maps data types to shorter aliases.

gAliasMap = {}

#    CDR Map - maps the basic CORBA data types (plus a couple of others)
#        to the corresponding CoLi types.

gCDRMap = {}

#    MarshalMap - maps data types to their marshaling functions.

gMarshalMap = {}

#    TypeModuleMap - maps defined data types to their defining modules.

gTypeModuleMap = {}

#*******************************************************************************
#
#    Utility Functions.
#
#*******************************************************************************


def addAlias (this, that):
    global gAliasMap
    if this == that:
        sys.stderr.write ("addAlias:  this = \"{}\"  that = \"{}\"\n".format (this, that))
        return 0

	# Avoid circular definitions; i.e., "<this> => <that> => ... => <this>".
    numSteps = 0
    mapping = that
    while mapping in gAliasMap:
        mapping = gAliasMap[mapping]
        if mapping == this:  return 0
        numSteps = numSteps + 1
        if numSteps > 5:
            sys.stderr.write ("addAlias:  this = \"{}\"  that = \"{}\"  mapping = \"{}\"\n".format (this, that, mapping))
        if numSteps > 10:
            return 0
    gAliasMap[this] = that
    return 1


def  declareMacros ():
    print ("/*******************************************************************************")
    print ("    Convenience Macros (from comx_util.c).")
    print ("*******************************************************************************/")
    print ()
    print ("/* CHECK(status) - return ERRNO on non-zero status. */")
    print ()
    print ("#ifndef CHECK")
    print ("#    define  CHECK(status)			\\")
    print ("        if ((status))  return (errno) ;")
    print ("#endif")
    print ()
    print ("/* NULL_OR(pointer,field) - pass NULL if structure pointer is NULL; else")
    print ("   pass address of field in structure. */")
    print ()
    print ("#ifndef NULL_OR")
    print ("#    define  NULL_OR(pointer, field)		\\")
    print ("        (((pointer) == NULL) ? NULL : &(pointer)->field)")
    print ("#endif")
    print ()
    print ("/* RETURN_IF_NULL(pointer) - return EINVAL if pointer is NULL. */")
    print ()
    print ("#ifndef RETURN_IF_NULL")
    print ("#    define  RETURN_IF_NULL(pointer)		\\")
    print ("        if ((pointer) == NULL) {		\\")
    print ("            SET_ERRNO (EINVAL) ;		\\")
    print ("            return (errno) ;			\\")
    print ("        }")
    print ("#endif")
    return

def eatComment (stream):
    token = stream.get_token ()
    if token == "":  return ""
    if token != "*":
        stream.push_token (token)
        return "noncomment"
    last = ""
    while (1):
        token = stream.get_token ()
        if (last == "*") and (token == "/"):  break
        last = token
    return "comment"


def firstLwr (name):
    return str.lower (name[0]) + name[1:]


def firstUpr (name):
    return str.upper (name[0]) + name[1:]


def guardOf (module):
    if (module is None) or (module == ""):  module = gPrefix
    return "IDL_MODULE_" + module.replace (":", "_")


def resolve (dataType, steps = 10):
    global gAliasMap
    while (steps > 0) and (dataType in gAliasMap):
        dataType = gAliasMap[dataType]
        steps = steps - 1
    return dataType


def skipTo (stream, target = ";"):
    token = stream.get_token ()
    while (token != "") and (token != target):
        if token == "{":  skipTo (stream, "}")
        if token == "/":  eatComment (stream)
        token = stream.get_token ()
    return token


def squeeze (dataType):
    return str.replace (dataType, "  ", " ")

#*******************************************************************************
#*******************************************************************************


def doALIAS (stream, token):
    global gMarshalMap, gPrefix

    dataType = token
    name = ""
    token = stream.get_token ()
    while token != ";":				# Gather up declaration.
        if name != "":  dataType = dataType + "  " + name
        name = token
        token = stream.get_token ()

    dataType = resolve (dataType)

    addAlias (name, dataType)

    return token


def doENUM (stream):
    global gCustomTypes, gLUT, gMarshalMap, gPrefix

    name = stream.get_token ()

    values = []
    token = stream.get_token ()			# Opening curly brace.
    while token != "}":
        token = stream.get_token ()		# Enumerated value.
        while token == "/":
            if eatComment (stream) != "comment":  break
            token = stream.get_token ()
        values = values + [token]
        token = stream.get_token ()		# Comma or closing curly brace.

    gLUT[name] = values

    dataType = stream.get_token ()
    if dataType in gCustomTypes:		# Skip custom data types.
        return skipTo (stream, ";")

    module = gTypeModuleMap[dataType]
    if module is None:  module = "Unknown"

    function = gPrefix + firstUpr (dataType)

    print ("\f")
    print ("/*!*****************************************************************************")
    print ("    {}() - decode/encode/erase a CORBA {} enumerated type.".format (function, dataType))
    print ("*******************************************************************************/")
    print ()
    print ("#if !defined({}) || {}".format (guardOf (module), guardOf (module)))
    print ()
    print ("errno_t  {} (".format (function))
    print ()
    print ("#    if PROTOTYPES")
    print ("        ComxChannel  channel,")
    print ("        {}  *value)".format (dataType))
    print ("#    else")
    print ("        channel, value)")
    print ("")
    print ("        ComxChannel  channel ;")
    print ("        {}  *value ;".format (dataType))
    print ("#    endif")
    print ()
    print ("{")
    print ("    unsigned  long  enumeration ;")
    print ("    if (value != NULL)  enumeration = (unsigned long) *value ;")
    print ("    CHECK (comxEnum (channel, &enumeration)) ;")
    print ("    if (value != NULL)  *value = ({}) enumeration ;".format (dataType))
    print ("    return (0) ;")
    print ("}")
    print ()
    print ("#endif	/* {} */".format (guardOf (module)))

    return stream.get_token ()			# Semi-colon.


def doFIELD (stream, token, isUnion = 0):
    global gMarshalMap, gPrefix, gTypeModuleMap

    isArray = 0
    isSequence = 0

    if token == "SEQUENCE":
        isSequence = 1
        stream.get_token ()			# Left parenthesis.
        dataType = stream.get_token ()
        token = stream.get_token ()
        while token != ",":			# Gather up element type.
            dataType = dataType + "  " + token
            token = stream.get_token ()
        variable = stream.get_token ()
        stream.get_token ()			# Right parenthesis.
        stream.get_token ()			# Semi-colon.
    else:
        dataType = token
        variable = ""
        token = stream.get_token ()
        while token != ";":			# Gather up field.
            if token == "[":
                isArray = 1
                numElements = stream.get_token ()
                stream.get_token ()		# Right square bracket.
            else:
                if variable != "":  dataType = dataType + "  " + variable
                variable = token
            token = stream.get_token ()

    if isUnion:  variable = "data." + variable
    if (dataType in gTypeModuleMap) and \
       (gTypeModuleMap[dataType] == "CORBA__CDR"):
        marshal = gMarshalMap[dataType]		# CDR type: comx<Type>().
    elif (dataType in gMarshalMap) and \
         (gMarshalMap[dataType][0:7] != "#define"):
        marshal = gMarshalMap[dataType]		# Some type: <!prefix><Type>().
    else:
        marshal = gPrefix + firstUpr (dataType)	# Other: <prefix><Type>().
    dataType = squeeze (dataType)

    if isArray:
        invocation = "comxArray (channel, NULL_OR (value, {}), (ComxFunc) {}, sizeof ({}), {})".format (variable, marshal, dataType, numElements)
    elif isSequence:
        invocation = "comxSequence (channel, NULL_OR (value, {}), (ComxFunc) {}, sizeof ({}))".format (variable, marshal, dataType)
    elif marshal == "comxEnum":
        invocation = "{},{},{}".format (marshal, dataType, variable)
    else:
        invocation = "{} (channel, NULL_OR (value, {}))".format (marshal, variable)

    return invocation


def doSEQUENCE (stream):
    global gCustomTypes, gMarshalMap, gPrefix

    stream.get_token ()				# Left parenthesis.
    elementType = stream.get_token ()
    token = stream.get_token ()
    while token != ",":				# Gather up element type.
        elementType = elementType + "  " + token
        token = stream.get_token ()
    dataType = stream.get_token ()
    if dataType in gCustomTypes:		# Skip custom data types.
        return skipTo (stream, ";")
    stream.get_token ()				# Right parenthesis.

    module = gTypeModuleMap[dataType]
    if module is None:  module = "Unknown"

    print ("\f")
    print ("/*!*****************************************************************************")
    print ("    {}{}() - decode/encode/erase a sequence of CORBA {} structures.".format (gPrefix, firstUpr (dataType), elementType))
    print ("*******************************************************************************/")
    print ()
    print ("#if !defined({}) || {}".format (guardOf (module), guardOf (module)))
    print ()
    print ("errno_t  {}{} (".format (gPrefix, firstUpr (dataType)))
    print ()
    print ("#    if PROTOTYPES")
    print ("        ComxChannel  channel,")
    print ("        {}  *value)".format (dataType))
    print ("#    else")
    print ("        channel, value)")
    print ("")
    print ("        ComxChannel  channel ;")
    print ("        {}  *value ;".format (dataType))
    print ("#    endif")
    print ()
    print ("{")

    elementType = resolve (elementType)
    if elementType in gMarshalMap:
        marshal = gMarshalMap[elementType]
# The following elif statement generates comxSequence(comxEnum(elementType)),
# but the element type's marshaling function needs to be called instead of
# comxEnum() so that each element is converted to/from an unsigned long
# before/after calling comxEnum().
#    elif elementType in gEnumModuleMap:
#        marshal = "comxEnum"
    else:
        marshal = gPrefix + firstUpr (elementType)
    elementType = squeeze (elementType)

    print ("    return (comxSequence (channel, value, (ComxFunc) {}, sizeof ({}))) ;".format (marshal, elementType))
    print ("}")
    print ()
    print ("#endif	/* {} */".format (guardOf (module)))

    return stream.get_token ()			# Semi-colon.


def doSTRUCT (stream):
    global gCustomTypes, gPrefix

    dataType = stream.get_token ()
    if dataType in gCustomTypes:		# Skip custom data types.
        return skipTo (stream, ";")

    module = gTypeModuleMap[dataType]
    if module is None:  module = "Unknown"

    function = gPrefix + firstUpr (dataType)

    print ("\f")
    print ("/*!*****************************************************************************")
    print ("    {}{}() - decode/encode/erase a CORBA {} structure.".format (gPrefix, firstUpr (dataType), dataType))
    print ("*******************************************************************************/")
    print ()
    print ("#if !defined({}) || {}".format (guardOf (module), guardOf (module)))
    print ()
    print ("errno_t  {} (".format (function))
    print ()
    print ("#    if PROTOTYPES")
    print ("        ComxChannel  channel,")
    print ("        {}  *value)".format (dataType))
    print ("#    else")
    print ("        channel, value)")
    print ("")
    print ("        ComxChannel  channel ;")
    print ("        {}  *value ;".format (dataType))
    print ("#    endif")
    print ()
    print ("{")

    stream.get_token ()				# Opening curly brace.
    token = stream.get_token ()			# Data type.
    while token != "}":
        if token == "/":
            if eatComment (stream) != "comment":  break
        elif token == "union":
            doUNION (stream, function, dataType)
        else:
            invocation = doFIELD (stream, token)
            if invocation[:9] == "comxEnum,":
                invocationList = str.split (invocation, ",")
                marshal = invocationList[0]
                dataType = invocationList[1]
                variable = invocationList[2]
                print ("  { unsigned  long  enumeration ;")
                print ("    if (value != NULL)  enumeration = value->{} ;".format (variable))
                print ("    CHECK ({} (channel, &enumeration)) ;".format (marshal))
                print ("    if (value != NULL)  value->{} = ({}) enumeration ;".format (variable, dataType))
                print ("  }")
            else:
                print ("    CHECK (" + invocation + ") ;")
                dataType = ""
        token = stream.get_token ()

    print ("    return (0) ;")
    print ("}")
    print ()
    print ("#endif	/* {} */".format (guardOf (module)))

    stream.get_token ()				# Data type.

    return stream.get_token ()			# Semi-colon.


def doUNION (stream, parentFunction, switchType):

    stream.get_token ()				# Opening curly brace.

    print ("    switch (value->which) {")

    defaultSeen = 0
    token = stream.get_token ()			# Data type.
    while token != "}":
        if token == "/":
            stream.get_token ()			# "*"
            value = stream.get_token ()
            if value == "<":			# "<default>"?
                value = value + stream.get_token () + stream.get_token ()
            stream.get_token ()			# "*"
            stream.get_token ()			# "/"
            if value == "<default>":
                defaultSeen = 1
                print ("    default:")
            else:
                print ("    case {}:".format (value))
        else:
            invocation = doFIELD (stream, token, 1)
            if invocation[:9] == "comxEnum,":
                invocationList = str.split (invocation, ",")
                marshal = invocationList[0]
                dataType = invocationList[1]
                variable = invocationList[2]
                print ("      { unsigned  long  enumeration ;")
                print ("        if (value != NULL)  enumeration = value->{} ;".format (variable))
                print ("        CHECK ({} (channel, &enumeration)) ;".format (marshal))
                print ("        if (value != NULL)  value->{} = ({}) enumeration ;".format (variable, dataType))
                print ("      }")
            else:
                print ("        CHECK (" + invocation + ") ;")
            print ("        break ;")
        token = stream.get_token ()

    if not defaultSeen:
        print ("    default:")
        print ("        SET_ERRNO (EINVAL) ;")
        if switchType == "":  switchType = "switch"
        print ("        LGE \"({}) Invalid {}: %lu\\n\", (unsigned long) value->which) ;".format (parentFunction, switchType))
        print ("        return (errno) ;")

    print ("    }")

    stream.get_token ()				# Union's "data" field.

    return stream.get_token ()			# Semi-colon.

#*******************************************************************************
#*******************************************************************************
#
#    Main Program - parses each IDL2H-generated header file on the command
#        line and writes the generated C code to standard output.
#
#*******************************************************************************
#*******************************************************************************

opts, args = getopt.getopt (sys.argv[1:], "",
                            ["autoSequence", "preDefs=", "prefix=", "epoch"])

for option, argument in opts:
    if option == "--autoSequence":
        gAutoSequence = 1
    elif option == "--preDefs":
        exec (open (argument).read ())
    elif option == "--prefix":
        gPrefix = argument
    elif option == "--epoch":
        gAppendSeq = 1
        gAutoSequence = 1
        gPrefix = "epmx"

declareMacros ()

for file in args:
    f = open (file, 'r')
    stream = shlex.shlex (f)
    token = ""
    while 1:
        last = token
        token = stream.get_token ()
        if token == "":  break
        if token == "/":
            eatComment (stream)
            token = last
            continue
        if (last == "typedef") and (token == "enum"):
            token = doENUM (stream)
        elif (last == "typedef") and (token == "SEQUENCE"):
            token = doSEQUENCE (stream)
        elif (last == "typedef") and (token == "struct"):
            token = doSTRUCT (stream)
        elif last == "typedef":
            token = doALIAS (stream, token)
        elif token == "extern":
            token = skipTo (stream, ";")
        elif (token != "typedef") and (token != "\f"):
            print ("===== \"{}\"".format (token))

keyList = list (gLUT.keys ())
if len (keyList) > 0:
    print ("")
    print ("/*******************************************************************************")
    print ("    Lookup Tables - for converting named constants to numbers and vice-versa;")
    print ("        see the coliToName() and coliToNumber() functions.")
    print ("*******************************************************************************/")
    keyList.sort ()
    for key in keyList:
        module = gEnumModuleMap.get (key)
        if module is None:  module = "Unknown"
        print ()
        print ("#if !defined({}) || {}".format (guardOf (module), guardOf (module)))
        print ("    const  ColiMap  {}LUT[] = {{".format (key))
        valueList = gLUT[key]
        for value in valueList:
            print ("      {{ (long) {}, \"{}\" }},".format (value, value))
        print ("      { 0L, NULL }")
        print ("    } ;")
        print ("#endif	/* {} */".format (guardOf (module)))
