# $Id: Makefile,v 1.3 2021/09/15 22:09:02 alex Exp alex $
#

IDL_DIR = ../idl

IDL_CPPFLAGS = \
	-D_COMPONENT_REPOSITORY_ \
	-DGIOP_1_2 \
	-DNO_ESCAPED_IDENTIFIERS \
	-D_PRE_3_0_COMPILER_ \
	-I$(IDL_DIR) \
	-I$(IDL_DIR)/CORBA_IDL \
	-I$(IDL_DIR)/CORBAservices \
	-I$(IDL_DIR)/Interop \
	-I$(IDL_DIR)/Misc

#    CORBA IDL.

CORBA_IDL = gimx_all.idl
CORBA_IDL_H = gimx_idl.h.new

#    Data Acquisition from Industrial Systems (DAIS) IDL.

DAIS_IDL = damx_all.idl
DAIS_IDL_H = damx_idl.h.new

#    Data Distribution Service for Real-Time Systems (DDS) IDL.

DDS_IDL = $(IDL_DIR)/DDS/dds_dlrl.idl
DDS_IDL_H = ddmx_idl.h.new

#    Enhanced View of Time Service (EVOT) IDL.

EVOT_IDL = $(IDL_DIR)/EnhancedViewOfTime/CosClockService.idl
EVOT_IDL_H = vtmx_idl.h.new

#    Laboratory Equipment Control Interface Specification (LECIS) IDL.

LECIS_IDL = $(IDL_DIR)/LECIS/LECIS.idl
LECIS_IDL_H = lemx_idl.h.new

IDL2H = ./idl2h.py

REX_HELP = rex_util_help

REX_HELP_H = $(REX_HELP).h.new

RM = rm -f

all::	$(CORBA_IDL_H) $(DAIS_IDL_H) $(DDS_IDL_H)
all::	$(EVOT_IDL_H) $(LECIS_IDL_H) $(REX_HELP_H)

$(CORBA_IDL_H): $(CORBA_IDL) $(IDL2H)
	-$(RM) $@
	$(IDL2H) --cppCommand "gcc -E -nostdinc $(IDL_CPPFLAGS)" --include --preDefs comx_post.py --preDefs gimx_pre.py --postDefs gimx_post.py --prefix gimx $(CORBA_IDL) > $@

$(DAIS_IDL_H): $(DAIS_IDL) $(IDL2H)
	-$(RM) $@
	$(IDL2H) --cppCommand "gcc -E -nostdinc $(IDL_CPPFLAGS) -I$(IDL_DIR)/DAIS" --include --preDefs gimx_post.py --preDefs damx_pre.py --postDefs damx_post.py --prefix damx $(DAIS_IDL) > $@

$(DDS_IDL_H): $(DDS_IDL) $(IDL2H)
	-$(RM) $@
	$(IDL2H) --cppCommand "gcc -E -nostdinc $(IDL_CPPFLAGS) -I$(IDL_DIR)/DDS" --include --preDefs gimx_post.py --preDefs ddmx_pre.py --postDefs ddmx_post.py --prefix ddmx $(DDS_IDL) > $@

$(EVOT_IDL_H): $(EVOT_IDL) $(IDL2H)
	-$(RM) $@
	$(IDL2H) --cppCommand "gcc -E -nostdinc $(IDL_CPPFLAGS) -I$(IDL_DIR)/EnhancedViewOfTime" --include --preDefs gimx_post.py --preDefs vtmx_pre.py --postDefs vtmx_post.py --prefix vtmx $(EVOT_IDL) > $@

$(LECIS_IDL_H): $(LECIS_IDL) $(IDL2H)
	-$(RM) $@
	$(IDL2H) --cppCommand "gcc -E -nostdinc $(IDL_CPPFLAGS)" --include --preDefs gimx_post.py --postDefs lemx_post.py --prefix lemx $(LECIS_IDL) > $@

# Run MAKE with an argument, "make rex_util_help.h.new", to generate a header
# file with each line of the REX_UTIL help text converted to a C string.  The
# generated header has a ".new" extension and is intended to replace the body
# of the version-controlled ".h" file when changes to the help text are made.
#
# NOTE that the backslashes ('\') MUST be substituted BEFORE the quotation
# marks ('"'); the substitution for the quotation mark is \" and those
# backslashes are not to be doubled.

$(REX_HELP_H):	$(REX_HELP).txt
	$(RM) $@
	sed	-e '/^#/d'			\
		-e 's|\\|\\\\|g'		\
		-e 's|"|\\"|g'			\
		-e 's|\t|\\t|g'			\
		-e 's|\f|\\f|g'			\
		-e 's|^|REX_HELP_LINE ("|'	\
		-e 's|$$|")|'			\
		$(REX_HELP).txt > $@

clean::
	-$(RM) $(CORBA_IDL_H) $(DAIS_IDL_H) $(DDS_IDL_H)
	-$(RM) $(EVOT_IDL_H) $(LECIS_IDL_H) $(REX_HELP_H)
