#! /usr/bin/env python3
#
# $Id: idl2h.py,v 1.10 2021/09/15 04:06:13 alex Exp alex $
#
#*******************************************************************************
#
#    IDL2H - scans one or more CORBA IDL files and generates header file
#        declarations for the structures, enumerations, and marshalling
#        functions defined in the IDL files.  The output is written to
#        standard output.
#
#*******************************************************************************


# For backwards compatibility with Python 2.7, disable 2.7's print statement.
from __future__ import print_function

import getopt, os, os.path, shlex, subprocess, sys, traceback


gAppendSeq = 0			# Rewrite sloppy sequential type names?
gAutoSequence = 0		# Automatically generate sequence types?
gPostDefs = ""			# Output file for post-processing definitions.
gPrefix = "mx"			# Prefix for marshaling function names.
gScanInterface = 1		# Scan interface declarations?
gUseAlias = 0			# Resolve data type aliases?

gCPPCommand = "gcc -E -D_COMPONENT_REPOSITORY_ -DGIOP_1_2 -DNO_ESCAPED_IDENTIFIERS"
gInclude = 0			# Process #include's in IDL?

gPipedFile = ""			# Name of file fed to CPP.
gFileName = ""			# Current file as reported by CPP.

gMaxSectionNameLength = 8	# Maximum section name length for GCC.

gLastPrint = ""
gModulePath = ""
gConstMap = {}			# Maps constants to defining module.
gCustomTypes = set ()		# Data types with custom marshaling functions.
gHookMap = {}			# Maps names to processing hooks.
gRenameMap = {}			# Maps types to equivalent types.

#    AliasMap - maps data types to shorter aliases.

gAliasMap = dict ()

#    CDR Map - maps the basic CORBA data types (plus a couple of others)
#        to the corresponding CoLi types.

gCDRMap = dict ()

#    EnumModuleMap - maps enumerated types to their defining modules.

gEnumModuleMap = dict ()

#    EnumValueMap - maps enumerated types to value prefix/suffix.
#        This dictionary provides a means of renaming all of an
#        enumerated type's values by mapping the type name to a
#        tag.  The tag is applied to each value name according
#        to the format of the tag:
#            "<suffix>"   - appends <suffix> to the value name.
#            "$<suffix>"  - does the same.
#            "^<prefix>"  - prepends <prefix> to the value name.
#        Function renumerate() (below) performs this translation.
#        I currently only use this to rename the values of
#        "CORBA::exception_type" in "CORBA_StandardExceptions.idl",
#        which is parsed for the "gimx_idl.h" header file.  Some
#        of the value names clash with enumerated values defined
#        elsewhere and my generated C code doesn't otherwise
#        distinguish the value names according to their module
#        scope.  In this case, I used a tag of "^EXCEPT_" to
#        add "EXCEPT_" to the beginning of each "exception_type"
#        value name.

gEnumValueMap = dict ()

#    NewEnums - is the set of new enumerated types defined in the
#        header file being generated.  This prevents lookup tables
#        being declared for predefined enumerated types.  NOTE that
#        predefinition files that are adding new types to this header
#        file (e.g., "gimx_pre.py") MUST add the new types to this set.

gNewEnums = set ()

#    MarshalMap - maps data types to their marshaling functions.

gMarshalMap = dict ()

#    TypeModuleMap - maps defined data types to their defining modules.

gTypeModuleMap = dict ()

#    ImplementedTypes - is, upon completion of the program, the set of
#        all data types implemented, from the primitive CDR types up
#        through the new types implemented in the header file being
#        generated.  Taking an application-specific IDL file as an
#        example, the ImplementedTypes set will contain all the types
#        defined in:
#
#            comx_util.h => gimx_util.h/gimx_idl.h => apmx_util.h/apmx_idl.h
#
#        where "apmx" is the prefix for the application's marshaling functions.

gImplementedTypes = set ()

#    Guards - is the set of the C Preprocessor macros used to include
#        or not include, by module, functions and lookup tables.  This
#        set is listed in the post-processing ".py" file for reference
#        purposes.  For example, a developer can pick out which guards
#        to "-D<guard>=0" in the CPP options to reduce object and
#        executable size.

gGuards = set ()

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


def declareSeq (type):
    global gAliasMap, gAutoSequence, gModulePath, gRenameMap, gTypeModuleMap
    if not gAutoSequence:  return
    typeSeq = type + "Seq"
    if modularize (typeSeq) in gRenameMap:
        typeSeq = gRenameMap[modularize (typeSeq)]
    if typeSeq in gTypeModuleMap:  return
    print ()
    print ("typedef  SEQUENCE ({}, {}) ;".format (type, typeSeq))
    gTypeModuleMap[typeSeq] = gModulePath
    alias = "sequence<" + type + ">"
    if alias in gAliasMap:  addAlias (typeSeq, alias)
    return


def firstLwr (name):
    return str.lower (name[0]) + name[1:]


def firstUpr (name):
    return str.upper (name[0]) + name[1:]


def getModule (modulePath):
    if modulePath is None:  modulePath = ""
    last = str.rfind (modulePath, "::")
    if last >= 0:
        return modulePath[last+2:]
    elif modulePath == "":
        return "CORBA"
    else:
        return modulePath


def getValue (stream):
    value = ""
    token = stream.get_token ()
    while (token != "") and (token != ";"):
        if token == ":":
            value = ""			# Remove namespace qualifier.
        elif value == "":
            value = token
        else:
            value = value + " " + token
        token = stream.get_token ()
    if token == ";":  stream.push_token (token)
    return value.replace ("< <", "<<")


def guardOf (module):
    if (module is None) or (module == ""):  module = gPrefix
    guard = "IDL_MODULE_" + module.replace (":", "_")
    gGuards.add (guard)
    return guard


def hookIt (name):
    global gHookMap
    if name not in gHookMap:  return ""
    hook = gHookMap[name]
    if hook == "ignore":  return hook
    return hook (name)


def modularize (name):
    global gModulePath
    if str.find (name, "::") >= 0:  return name
    if gModulePath == "":
        return name
    else:
        return getModule (gModulePath) + "::" + name


def renameOf (name):
    global gRenameMap
    if modularize (name) in gRenameMap:
        return gRenameMap[modularize (name)]
    elif name in gCDRMap:
        return gCDRMap[name]
    else:
        return name


def renumerate (name):
    global gEnumValueMap
    last = str.rfind (name, "::")
    if last >= 0:
        enumType = modularize (name[:last])
    else:
        enumType = ""
    if enumType in gEnumValueMap:
        tag = gEnumValueMap[enumType]
        if tag[0] == "^":
            return tag[1:] + getModule (name)
        elif tag[0] == "$":
            return getModule (name) + tag[1:]
        else:
            return getModule (name) + tag
    else:
        return renameOf (name)


def resolve (dataType, steps = 5):
    global gAliasMap
    while (steps > 0) and (dataType in gAliasMap):
        dataType = gAliasMap[dataType]
        steps = steps - 1
    return dataType


#    sequify() - converts our project-specific, random naming of sequences
#        to the CORBA convention (if gAppendSeq is true).  Depending on the
#        authors of the IDL modules, most sequence types had a "seq" prefix
#        or a "seq" suffix, with the case of the letters varying.  The "seq"
#        tag is removed and "Seq" is appended to the element type name.
#        This conversion is currently applied to struct fields, union fields,
#        and alias type names in simple typedef declarations.

def sequify (name):
    global gAppendSeq
    if not gAppendSeq:  return name
    if str.find (name, "sequence<") >= 0:  return name
    if str.upper (name[:3]) == "SEQ":
        dataType = name[3:]
    elif str.upper (name[-3:]) == "SEQ":
        dataType = name[:-3]
    else:
        return name
    if dataType in gTypeModuleMap:
        name = dataType + "Seq"
    else:
        alternate = dataType[0].lower() + dataType[1:]
        if alternate in gTypeModuleMap:
            dataType = alternate
        name = dataType + "Seq"
    return name


def skipTo (stream, target = ";"):
    token = stream.get_token ()
    while (token != "") and (token != target):
        if token == "{":  skipTo (stream, "}")
        token = stream.get_token ()
    return token


def unmodularize (name):
    return getModule (name)

#*******************************************************************************
#
#    getField(), parseField() - retrieve a field declaration (e.g., in a
#        structure or union) consisting of a data type and the field name.
#
#*******************************************************************************


def getField (stream, terminator = ";", unexpected = ["}"], steps = 1):

    global gAliasMap, gCDRMap

    names = []

#    Parse the field declaration.

    dataType, names, token = parseField (stream, terminator, unexpected)
    if token != terminator:  return (dataType, names, token)

#    Replace very long data types by their Windows counterparts.

    dataType = str.replace (dataType, "unsigned  long  long", "ULONGLONG")
    dataType = str.replace (dataType, "long  long", "LONGLONG")
    dataType = str.replace (dataType, "long  double", "LONGDOUBLE")

#    Perform renaming, etc. to the data type.  For a sequence type, first
#    apply the renaming and such to the data type being sequenced.

    if str.find (dataType, "sequence<") >= 0:
        dataType = dataType[9:-1]
        dataType = renameOf (dataType)
        dataType = unmodularize (dataType)
        if dataType in gCDRMap:  dataType = gCDRMap[dataType]
        alias = "sequence<" + dataType + ">"
        if alias in gAliasMap:
            dataType = gAliasMap[alias]
        else:
            if gUseAlias:  dataType = resolve (dataType, 10)
            dataType = "sequence<" + dataType + ">"

    dataType = renameOf (dataType)		# Rename type.
    dataType = unmodularize (dataType)		# Remove namespace qualifiers.
    if gUseAlias:
        dataType = resolve (dataType, 10)	# Resolve aliases.

#    Convert CORBA data types to the corresponding CoLi data types.

    if dataType in gCDRMap:  dataType = gCDRMap[dataType]

    return (dataType, names, token)


def parseField (stream, terminator = ";", unexpected = ["}"]):
    dataType = ""
    names = []
    name = ""
    token = stream.get_token ()
    while token != terminator:			# Name precedes teminator.
        if (token == ""):  break
        if (token == "#"):
            global idlMap
            idlMap[token].parse (stream, 0)
            token = stream.get_token ()
            continue
        if token in unexpected:  break
        if str.find (terminator, token) >= 0:  break
        if (token == ","):
            names.append (name)
        elif len (names) > 0:
            "Dummy Statement!  Multiple variables per line."
            # Data type known: drop down, save token as name, get next token.
        elif dataType == "":			# Data type precedes name.
            dataType = name
        elif str.find (":<[", dataType[-1]) >= 0:
            dataType = dataType + name
        elif str.find (":<>[]", name) >= 0:
            dataType = dataType + name
        else:
            dataType = dataType + "  "  + name
        name = token
        token = stream.get_token ()
    if name == "]":
        last = str.rfind (dataType, " ")
        if last < 0:
            name = dataType + "]"
            dataType = ""
        else:
            name = dataType[last+1:] + "]"
            dataType = dataType [:last-1]
    if name != ",":
        names.append (name)
    return (dataType, names, token)

#*******************************************************************************
#
#    Multi-line (Block) Declarations; e.g., modules, structures, unions, etc.
#    BLOCK is the base class that parses a basic block:
#
#        <keyword> <name> {
#            ... declarations ...
#        } ;
#
#*******************************************************************************

class  BLOCK:

    def __init__ (self, keyword, skip = ""):
        self.keyword = keyword
        self.skip = skip
        self.name = ""

    def parseHeader (self, stream, depth):
        self.name = stream.get_token ()		# Block name.
        if self.name == "":  return
        return stream.get_token ()		# Opening curly brace.

    def parseBody (self, stream, depth):
        global gFileName
        while 1:
            token = stream.get_token ()
            if token == "":  break
            if token == "}":  break		# Closing curly brace?
            if token in idlMap:
                idlMap[token].parse (stream, depth + 1)
            else:
                if self.skip == "":		# Unexpected field?
                    sys.stderr.write ("Unexpected {} at line {} in {}\n".format (token, stream.lineno, gFileName))
                else:				# Expected fields to skip.
                    if skipTo (stream, self.skip) == "":  break
        return token

    def parseTrailer (self, stream, depth):
        global gLastPrint
        gLastPrint = self.keyword
        return stream.get_token ()		# Following semi-colon.

    def parse (self, stream, depth):
        if self.parseHeader (stream, depth) == "":  return ""
        self.parseBody (stream, depth)
        self.parseTrailer (stream, depth)


class  ENUM (BLOCK):

    def parseHeader (self, stream, depth):
        token = BLOCK.parseHeader (self, stream, depth)
        if token == "":  return ""
        if hookIt (modularize (self.name)) == "ignore":
            stream.push_token (token)
            skipTo (stream, ";")
            return ""

    def parseBody (self, stream, depth):
        global gEnumModuleMap, gModulePath, gTypeModuleMap
        self.name = renameOf (self.name)
        if self.name in gTypeModuleMap:
            if (gTypeModuleMap[self.name] is not None) and \
               (gTypeModuleMap[self.name] != "None"):
                print ("\n/*===== \"{}\" already defined =====*/".format (self.name))
            return skipTo (stream, "}")
        gEnumModuleMap[self.name] = gModulePath
        gTypeModuleMap[self.name] = gModulePath
        gNewEnums.add (self.name)
        sys.stdout.write ("\ntypedef  enum  {} {{".format (self.name))
        while 1:
            token = stream.get_token ()
            if token == "":  break
            if (token == "#"):
                global idlMap
                idlMap[token].parse (stream, 0)
                continue
            if token == "}":  break		# Closing curly brace.
            if token == ",":			# Comma between enumerations.
                sys.stdout.write (",")
                continue
            enumeration = renumerate (self.name + "::" + token)
            if str.find (enumeration, "::") >= 0:  enumeration = token
            sys.stdout.write ("\n    {}".format (enumeration))
        sys.stdout.write ("\n}}  {} ;\n".format (self.name))
        return token

    def parseTrailer (self, stream, depth):
        token = BLOCK.parseTrailer (self, stream, depth)
        return token


class  INTERFACE (BLOCK):

    def parse (self, stream, depth):
        global gAliasMap, gLastPrint, gScanInterface
        self.name = stream.get_token ()
        if (self.name == ""):  return ""
        if self.name != "IOR":		# Avoid "interface IOR".
            self.name = renameOf (self.name)
            hookIt (self.name)
            if self.name not in gAliasMap:
                if gLastPrint != "interface":  print ()
                print ("typedef  IOR  {} ;".format (self.name))
                gTypeModuleMap[self.name] = gModulePath
                gMarshalMap[self.name] = "#define"
                addAlias (self.name, "IOR")
                gLastPrint = "interface"
        if gScanInterface:		# Process declarations inside interface?
            token = stream.get_token ()
            while token != "":		# Skip inheritance specification.
                if (token == "{") or (token == ";"):  break
                token = stream.get_token ()
            if token == "{":
                BLOCK.parseBody (self, stream, depth + 1)
            else:
                stream.push_token (token)
        return skipTo (stream, ";")


class  MODULE (BLOCK):

    def parseHeader (self, stream, depth):
        global gModulePath
        token = BLOCK.parseHeader (self, stream, depth)
        if token == "{":
            if gModulePath == "":
                gModulePath = self.name
            else:
                gModulePath = gModulePath + "::" + self.name
            print ()
            print ("/* Module: {} */".format (gModulePath))
            print ()
            print ("#ifndef {}".format (guardOf (gModulePath)))
            print ("#    define  {}  1".format (guardOf (gModulePath)))
            print ("#endif")
            hookIt (gModulePath + "::")
        return token

    def parseTrailer (self, stream, depth):
        global gModulePath
        token = BLOCK.parseTrailer (self, stream, depth)
        last = str.rfind (gModulePath, "::")
        if last < 0:  last = 0
        gModulePath = gModulePath[:last]
        return token


class  STRUCT (BLOCK):

    def parseHeader (self, stream, depth):
        global gModulePath, gTypeModuleMap
        token = BLOCK.parseHeader (self, stream, depth)
        if token == "":  return ""
        if hookIt (modularize (self.name)) == "ignore":
            stream.push_token (token)
            skipTo (stream, ";")
            return ""
        self.name = renameOf (self.name)
        if self.name in gTypeModuleMap:
            if (gTypeModuleMap[self.name] is not None) and \
               (gTypeModuleMap[self.name] != "None"):
                print ("\n/*===== \"{}\" already defined =====*/".format (self.name))
            if skipTo (stream, "}") != "":  stream.get_token ()
            return ""
        gTypeModuleMap[self.name] = gModulePath
        gMarshalMap[self.name] = gPrefix + firstUpr (self.name)
        print ()
        print ("typedef  struct  {} {{".format (self.name))
        return token

    def parseBody (self, stream, depth):
        global gUseAlias
        while 1:
            dataType, names, token = getField (stream, steps = 0)
            if token != ";":  break
            dataType = sequify (dataType)
            if gUseAlias:  dataType = resolve (dataType)
            for name in names:
                if str.find (dataType, "sequence<") >= 0:	# Sequence type?
                    dataType = dataType[9:-1]
                    print ("    SEQUENCE ({}, {}) ;".format (dataType, name))
                elif dataType[-1] == "*":
                    print ("    {}{} ;".format (dataType, name))
                else:
                    print ("    {}  {} ;".format (dataType, name))
        return token

    def parseTrailer (self, stream, depth):
        token = BLOCK.parseTrailer (self, stream, depth)
        print ("}}  {} ;".format (self.name))
        declareSeq (self.name)
        return token


class  UNION (BLOCK):

    def parseHeader (self, stream, depth):
        global gLastPrint, gModulePath, gTypeModuleMap
        gLastPrint = self.keyword
        self.name = stream.get_token ()		# Union name.
        if self.name == "":  return ""
        if hookIt (modularize (self.name)) == "ignore":
            skipTo (stream, ";")
            return ""
        token = stream.get_token ()		# "switch" keyword.
        if token != "switch":  return token
        token = stream.get_token ()		# Left parenthesis.
        if token != "(":  return token
        self.switch = stream.get_token ()	# Switch type.
        if self.switch == "":  return ""
        token = stream.get_token ()		# Right parenthesis.
        if token != ")":  return token
        self.name = renameOf (self.name)
        if self.name in gTypeModuleMap:
            if (gTypeModuleMap[self.name] is not None) and \
               (gTypeModuleMap[self.name] != "None"):
                print ("\n/*===== \"{}\" already defined =====*/".format (self.name))
            if skipTo (stream, "}") != "":  stream.get_token ()
            return ""
        gTypeModuleMap[self.name] = gModulePath
        gMarshalMap[self.name] = gPrefix + firstUpr (self.name)
        self.switch = renameOf (self.switch)
        print ()
        print ("typedef  struct  {} {{".format (self.name))
        print ("    {}  which ;".format (self.switch))
        print ("    union {")
        return stream.get_token ()		# Opening curly brace.

    def parseBody (self, stream, depth):
        while 1:
            dataType, names, token = getField (stream, ";", ["case", "default", "}"])
            if token == "case":
                value = stream.get_token ()	# Switch value.
                if value == "":  break
                token = stream.get_token ()	# Skip colon.
                if token != ":":  break
                enumeration = renumerate (self.switch + "::" + value)
                if str.find (enumeration, "::") >= 0:  enumeration = value
                print ("\t\t\t/* {} */".format (enumeration))
                continue
            elif token == "default":
                token = stream.get_token ()	# Skip colon.
                if token != ":":  break
                print ("\t\t\t/* <default> */")
                continue
            if token != ";":  break
            dataType = sequify (dataType)
            if gUseAlias:  dataType = resolve (dataType)
            for name in names:
                if str.find (dataType, "sequence<") >= 0:	# Sequence type?
                    dataType = dataType[9:-1]
                    print ("        SEQUENCE ({}, {}) ;".format (dataType, name))
                elif dataType[-1] == "*":
                    print ("        {}{} ;".format (dataType, name))
                else:
                    print ("        {}  {} ;".format (dataType, name))
        return token

    def parseTrailer (self, stream, depth):
        token = BLOCK.parseTrailer (self, stream, depth)
        print ("    }  data ;")
        print ("}}  {} ;".format (self.name))
        declareSeq (self.name)
        return token


class  VALUETYPE (BLOCK):

    def parse (self, stream, depth):
        global gAliasMap, gLastPrint, gScanInterface
        self.name = stream.get_token ()
        if (self.name == ""):  return ""
        self.name = renameOf (self.name)
        hookIt (self.name)
        if self.name not in gAliasMap:
            if gLastPrint != "valuetype":  print ()
            print ("typedef  _valuetype_  {} ;	/* \"valuetype\" */".format (self.name))
            gTypeModuleMap[self.name] = gModulePath
            gMarshalMap[self.name] = "#define"
            addAlias (self.name, "_valuetype_")
            gLastPrint = "valuetype"
        return skipTo (stream, ";")

#*******************************************************************************
#
#    Single-line Declarations; e.g., constants and typedef's.
#
#*******************************************************************************

class  SINGLE:

    def __init__ (self, keyword):
        self.keyword = keyword


class  POUND (SINGLE):

    def parse (self, stream, depth):
        global gFileName, gInclude, gPipedFile
        token = stream.get_token ()
        if token == "pragma":
            stream.instream.readline ()
        else:
            lineno = int (token)
            token = stream.get_token ()
            stream.instream.readline ()
            stream.lineno = lineno
            gFileName = token[1:-1]
            if gFileName == "":  gFileName = gPipedFile
            if lineno == 1:  print ("/*  {} : line {}  */".format (gFileName, lineno))
        return token


class  CONST (SINGLE):

    def parse (self, stream, depth):
        global gConstMap, gLastPrint, gModulePath
        dataType, names, token = getField (stream, "=")
        if token != "=":  return token
        name = names[0]
        value = getValue (stream)		# Get constant's value.
        if value == "":  return ""
        if dataType == "long":
            value = value + "L"
        elif dataType == "unsigned  long":
            value = value + "UL"
        if renameOf (name) in gConstMap:
            print ("\n/*===== {}::{} previously defined as {} in {} =====*/".format (gModulePath, name, renameOf (name), gConstMap[renameOf (name)]))
        else:
            name = renameOf (name)
            gConstMap[name] = gModulePath
            if gLastPrint != "const":  print ()
            gLastPrint = "const"
            print ("#define  {}  ({})".format (name, value))
        token = stream.get_token ()		# Following semi-colon.
        return token


class  TYPEDEF (SINGLE):

    def parse (self, stream, depth):
        global gAliasMap, gCDRMap, gLastPrint, gModulePath, gTypeModuleMap
        gLastPrint = "typedef"
        dataType, names, token = getField (stream, steps = 0)
        if token != ";":
            return token
        name = names[0]
        if hookIt (modularize (name)) == "ignore":
            return token
        name = renameOf (name)
        if (str.find (dataType, "sequence<") >= 0) or \
           (dataType[-3:] == "Seq"):
            name = sequify (name)
        if name in gTypeModuleMap:		# Already defined?
            print ("\n/*===== \"{}\" already defined =====*/".format (name))
        elif str.find (dataType, "sequence<") >= 0:
            print ()
            dataType = resolve (dataType)
            if dataType in gMarshalMap:
                gMarshalMap[name] = "#define"
                print ("typedef  {}  {} ;".format (dataType, name))
            else:
                dataType = dataType[9:-1]
                print ("typedef  SEQUENCE ({}, {}) ;".format (dataType, name))
            gTypeModuleMap[name] = gModulePath
        else:					# Non-sequence type.
            addAlias (name, dataType)
            gTypeModuleMap[name] = gModulePath
            dataType = resolve (dataType)
            if dataType in gMarshalMap:
                gMarshalMap[name] = "#define"
            print ()
            if dataType[-1] == "*":		# "<dataType>  *<name>"?
                print ("typedef  {}{} ;".format (dataType, name))
            else:				# "<dataType>  <name>"
                print ("typedef  {}  {} ;".format (dataType, name))
        return token


class  TYPEPREFIX (SINGLE):

    def parse (self, stream, depth):
        token = stream.get_token ()
        token = stream.get_token ()
        token = stream.get_token ()
        if token != ";":
            stream.push_token (token)
        return token


class  IGNORE (SINGLE):

    def parse (self, stream, depth):
        return self.keyword


class  SKIP (SINGLE):

    def __init__ (self, keyword, terminator = ";"):
        SINGLE.__init__ (self, keyword)
        self.terminator = terminator

    def parse (self, stream, depth):
        return skipTo (stream, self.terminator)

#*******************************************************************************
#
#    idlMap - maps IDL delcaration keywords to the objects used to parse the
#        declarations.
#
#*******************************************************************************

idlMap = {
    "abstract"		: SKIP ("abstract"),
    "const"		: CONST ("const"),
    "enum"		: ENUM ("enum"),
    "exception"		: SKIP ("exception"),
    "import"		: SKIP ("import"),
    "interface"		: INTERFACE ("interface", ";"),
    "local"		: SKIP ("local"),
    "module"		: MODULE ("module"),
    "native"		: SKIP ("native"),
    "struct"		: STRUCT ("struct"),
    "typedef"		: TYPEDEF ("typedef"),
    "typeprefix"	: TYPEPREFIX ("typeprefix"),
    "union"		: UNION ("union"),
    "valuetype"		: VALUETYPE ("valuetype"),
    "#"			: POUND ("#")
}

#*******************************************************************************
#*******************************************************************************
#
#    Main Program - parses each IDL file on the command line and writes
#        the generated C header file to standard output.
#
#*******************************************************************************
#*******************************************************************************

opts, args = getopt.getopt (sys.argv[1:], "",
                            ["appendSeq", "autoSequence",
                             "cppCommand=", "include",
                             "postDefs=", "preDefs=",
                             "prefix=", "useAlias",
                             "epoch"])

for option, argument in opts:
    if option == "--appendSeq":
        gAppendSeq = 1
    elif option == "--autoSequence":
        gAutoSequence = 1
    elif option == "--cppCommand":
        gCPPCommand = argument
    elif option == "--include":
        gInclude = 1
    elif option == "--preDefs":
        exec (open (argument).read ())
    elif option == "--postDefs":
        gPostDefs = argument
    elif option == "--prefix":
        gPrefix = argument
    elif option == "--useAlias":
        gUseAlias = 1
    elif option == "--epoch":
        gAppendSeq = 1
        gAutoSequence = 1
        gPrefix = "epmx"
        gUseAlias = 1

firstFile = 1

for file in args:
    gPipedFile = file
    gFileName = gPipedFile
    gLastPrint = ""
    if firstFile:
        firstFile = 0
    else:
        print ("\f")
    print ("/*******************************************************************************")
    print ("   ", gFileName)
    print ("*******************************************************************************/")
    directory, filename = os.path.split (gPipedFile)
    if hookIt (filename) == "ignore":  continue
    if gInclude:
        command = gCPPCommand + " - < " + gPipedFile
    else:
        command = "grep -v '#include' " + gPipedFile + " | " + gCPPCommand + " -"
    sys.stderr.write ("{}\n".format (command))
# For backwards compatibility with Python 2.7, use "universal_newlines=True"
# instead of Python 3's "text="True".  (Python 3 supports both.)
    pipe = subprocess.Popen (command, shell=True, universal_newlines=True, stdin=subprocess.PIPE, stdout=subprocess.PIPE, close_fds=True)
    stream = shlex.shlex (pipe.stdout)
    stream.commenters = ""
    while 1:
        token = stream.get_token ()
        if token == "":  break
        if token in idlMap:
            idlMap[token].parse (stream, 1)
        else:
            sys.stderr.write ("MAIN: Unexpected {} at line {} in {}\n".format (token, stream.lineno, gFileName))
            skipTo (stream, ";")

# Generate the alphabetically sorted, extern declarations of the enumerated
# type lookup-tables (LUTs) for a given enumerated type's values.

if len (gNewEnums) > 0:
    print ("\f")
    print ("/*******************************************************************************")
    print ("    Tables for mapping enumerated values to names and vice-versa;")
    print ("    see the coliToName() and coliToNumber() functions.")
    print ("*******************************************************************************/")
    print ()
    enumList = sorted (list (gNewEnums), key=str.upper)
    for enum in enumList:
        module = gEnumModuleMap[enum]
        if module is None:  module = "Unknown"
        print ("#if !defined({}) || {}".format (guardOf (module), guardOf (module)))
        print ("    extern  const  ColiMap  {}LUT[]  OCD (\"{}\") ;".format (enum, getModule (module)[:gMaxSectionNameLength]))
        print ("#endif")


uMap = {}

if len (gTypeModuleMap) > 0:
    print ("\f")
    print ("/*******************************************************************************")
    print ("    Public functions.")
    print ("*******************************************************************************/")
    print ()
    print ("/* Marshaling functions for the defined data types. */")
    typeList = sorted (gTypeModuleMap.keys (), key=str.upper)
    for type in typeList:
        if type in gImplementedTypes:  continue		# Skip implemented types.
        if type in gCustomTypes:			# Skip custom types.
            gImplementedTypes.add (type)
            continue
        gImplementedTypes.add (type)
        module = gTypeModuleMap[type]
        if module is None:  module = "Unknown"
        function = gPrefix + firstUpr (type)
        print ()
        print ("#if !defined({}) || {}".format (guardOf (module), guardOf (module)))
        if (type in gMarshalMap) and (gMarshalMap[type] == "#define"):
            print ("#    define  {}(channel,value)  {} ((channel), (value))".format (function, gMarshalMap[resolve (type)]))
        else:
            print ("    extern  errno_t  {} P_((ComxChannel channel,".format (function))
            numSpaces = len ("    extern  errno_t  ") + len (gPrefix)
            numSpaces = numSpaces + len (type) + len (" P_((")
            print ("{:{}}{} *value))".format ("", numSpaces, type))
            print ("        OCD (\"{}\") ;".format (getModule (module)[:gMaxSectionNameLength]))
        print ("#endif")

#*******************************************************************************
#
#    If a postprocessing definitions file is to be written, then do so.
#    This is a Python file that populates the various, global maps and
#    lists with their current contents at the time this run completes.
#    The file is useful as a *preprocessing* definitions file to be fed
#    into the translating of another IDL group that depends on the
#    definitions from this run.  For example:
#
#	Primitive
#	CDR Types	=>	Basic CORBA	=>	My IDL
#	comx_post.py		gimx_post.py		mymx_post.py
#
#    The contents of a postprocessing definitions file are cumulative,
#    so the "My IDL" translation only needs to read "gimx_post.py" and
#    not also "comx_post.py".  Another translation that depends on the
#    "My IDL" definitions only needs to read "mymx_post.py" to get the
#    COMX, GIMX, and MYMX definitions.
#
#*******************************************************************************

if gPostDefs != "":

    f = open (gPostDefs, "w")
    f.write ("# {} Definitions\n".format (str.upper (gPrefix)))

    f.write ("\n# Aliases\n\n")
    f.write ("global gAliasMap\n")
    f.write ("gAliasMap = dict ()\n")
    keyList = sorted (gAliasMap.keys (), key=str.upper)
    for key in keyList:
        f.write ("gAliasMap[\"{}\"] = \"{}\"\n".format (key, gAliasMap[key]))

    f.write ("\n# CDR Types\n\n")
    f.write ("global gCDRMap\n")
    f.write ("gCDRMap = dict ()\n")
    keyList = sorted (gCDRMap.keys (), key=str.upper)
    for key in keyList:
        f.write ("gCDRMap[\"{}\"] = \"{}\"\n".format (key, gCDRMap[key]))

    f.write ("\n# Custom Types\n\n")
    f.write ("global gCustomTypes\n")
    f.write ("gCustomTypes = set ()\n")
    keyList = sorted (list (gCustomTypes), key=str.upper)
    for key in keyList:
        f.write ("gCustomTypes.add (\"{}\")\n".format (key))

    f.write ("\n# C Preprocessor Guards\n#\n")
    keyList = sorted (list (gGuards), key=str.upper)
    for key in keyList:
        f.write ("#	{}\n".format (key))

    f.write ("\f\n\n# Enumerated Types\n\n")
    f.write ("global gEnumModuleMap\n")
    f.write ("gEnumModuleMap = dict ()\n")
    keyList = sorted (gEnumModuleMap.keys (), key=str.upper)
    for key in keyList:
        value = gEnumModuleMap[key]
        if value is None:
            f.write ("gEnumModuleMap[\"{}\"] = None\n".format (key))
        else:
            f.write ("gEnumModuleMap[\"{}\"] = \"{}\"\n".format (key, value))

    f.write ("\f\n\n# New Enumerated Types\n\n")
    f.write ("#global gNewEnums\n")
    f.write ("#gNewEnums = set ()\n")
    keyList = sorted (list (gNewEnums), key=str.upper)
    for key in keyList:
        f.write ("#gNewEnums.add (\"{}\")\n".format (key))

    f.write ("\f\n\n# Enumerated Value Prefixes/Suffixes\n\n")
    f.write ("global gEnumValueMap\n")
    f.write ("gEnumValueMap = dict ()\n")
    keyList = sorted (gEnumValueMap.keys (), key=str.upper)
    for key in keyList:
        f.write ("gEnumValueMap[\"{}\"] = \"{}\"\n".format (key, gEnumValueMap[key]))

    f.write ("\f\n\n# Rename Types\n\n")
    f.write ("global gRenameMap\n")
    f.write ("gRenameMap = dict ()\n")
    keyList = sorted (gRenameMap.keys (), key=str.upper)
    for key in keyList:
        f.write ("gRenameMap[\"{}\"] = \"{}\"\n".format (key, gRenameMap[key]))

    f.write ("\f\n\n# Defined Types\n\n")
    f.write ("global gTypeModuleMap\n")
    f.write ("gTypeModuleMap = dict ()\n")
    keyList = sorted (gTypeModuleMap.keys (), key=str.upper)
    for key in keyList:
        value = gTypeModuleMap[key]
        if value is None:
            f.write ("gTypeModuleMap[\"{}\"] = None\n".format (key))
        else:
            f.write ("gTypeModuleMap[\"{}\"] = \"{}\"\n".format (key, value))

    f.write ("\f\n\n# Implemented Types\n\n")
    f.write ("global gImplementedTypes\n")
    f.write ("gImplementedTypes = set ()\n")
    keyList = sorted (list (gImplementedTypes), key=str.upper)
    for key in keyList:
        f.write ("gImplementedTypes.add (\"{}\")\n".format (key))

    f.write ("\f\n\n# Marshaling Functions (implicit)\n\n")
    f.write ("global gMarshalMap\n")
    f.write ("gMarshalMap = dict ()\n")
    keyList = sorted (gMarshalMap.keys (), key=str.upper)
    for key in keyList:
        f.write ("gMarshalMap[\"{}\"] = \"{}\"\n".format (key, gMarshalMap[key]))

    gUnmarshaledTypes = list ()
    f.write ("\f\n\n# Marshaling Functions (explicit)\n\n")
    keyList = sorted (gTypeModuleMap.keys (), key=str.upper)
    for key in keyList:
        value = gTypeModuleMap[key]
        dataType = resolve (key)
        if dataType in gMarshalMap:
            f.write ("gMarshalMap[\"{}\"] = \"{}\"\n".format (key, gMarshalMap[dataType]))
        else:
            gUnmarshaledTypes.append (key)
#            f.write ("gMarshalMap[\"{}\"] = \"{}{}\"\n".format (key, gPrefix, firstUpr (key)))

    f.write ("\f\n\n# Unmarshaled Types (which are actually marshalled)\n\n")
    itemList = sorted (gUnmarshaledTypes)
    for item in itemList:
        f.write ("# {}\n".format (item))

    f.close ()
