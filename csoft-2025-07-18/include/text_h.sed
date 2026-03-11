# $Id$
#*******************************************************************************
#
#    Convert Text to an Array of C Strings
#
# NOTE that the backslashes ('\') MUST be substituted BEFORE the quotation
# marks ('"'); the substitution for the quotation mark is \" and those
# backslashes are not to be doubled.
#
#*******************************************************************************

/^#/d
s|\\|\\\\|g
s|"|\\"|g
s|\t|\\t|g
s|\f|\\f|g
s|^|TEXT_H_LINE ("|
s|$|")|
