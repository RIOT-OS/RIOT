#!/bin/bash

# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.

# Author: Joakim Nohlgård <joakim.nohlgard@eistec.se>

# Convert a GCC command line to Eclipse settings for import in
# Project->Properties->C/C++ General->Paths and Symbols
#
# Tested on:
#  Eclipse IDE for C/C++ Developers
#
#  Version: Luna Service Release 2 (4.4.2)
#  Build id: 20150219-0600

ECHO="printf %s\n"

if [ $# -ne 0 ]; then
    ${ECHO} "Usage: $0"
    ${ECHO} "Read GCC command line arguments from stdin, output Eclipse XML to stdout"
    ${ECHO} "Note: does not handle spaces inside macros and include paths at all."
    exit 2
fi

XML_INSTRUCTIONS='
<!--
Instrucions:

In Eclipse:
1. Open the project properties, menu Project->Properties
2. Select C/C++ General->Paths and Symbols
2a. (optional) Click Restore Defaults to delete any existing macros and include paths
3. Click Import Settings...
4. Select this file and press Finish
5. Rebuild C/C++ index, menu Project->C/C++ Index->Rebuild

-->
'
XML_HEADER='<?xml version="1.0" encoding="UTF-8"?><cdtprojectproperties>'$'\n''<!-- Automatically generated from make eclipsesym -->'
XML_FOOTER='</cdtprojectproperties>'
XML_MACRO_SECTION_BEGIN='<section name="org.eclipse.cdt.internal.ui.wizards.settingswizards.Macros">'
XML_INCLUDE_SECTION_BEGIN='<section name="org.eclipse.cdt.internal.ui.wizards.settingswizards.IncludePaths">'
XML_SECTION_END='</section>'
XML_MACRO_TEMPLATE='"<macro><name>"$1"</name><value>"$2"</value></macro>"'
XML_INCLUDE_TEMPLATE='"<includepath workspace_path=\"true\">"$0"</includepath>"'
LANGUAGES=( 'GNU C' 'GNU C++' 'Assembly' 'C Source File' 'C++ Source File' 'Assembly Source File' )
ECLIPSE_PROJECT_NAME='RIOT'

GCCCOMMANDLINE=$(cat)

# Find all includes
INCLUDES=$(${ECHO} "${GCCCOMMANDLINE}" | sed -e 's/ /\n/g' | egrep '^-I' | cut -c3-)

# Parse and rewrite to project relative paths
INCLUDES_REL=""
for p in ${INCLUDES}; do
    #ABSPATH=$(readlink -m "$p")
    RELPATH=$(readlink -m "$p" | sed -e "s,^${RIOTBASE},/${ECLIPSE_PROJECT_NAME}/,")
    INCLUDES_REL=${INCLUDES_REL}$'\n'${RELPATH}
done

# Grab macro definitions
MACROS=$(${ECHO} "${GCCCOMMANDLINE}" | sed -e 's/ /\n/g' | egrep '^-D' | cut -c3-)

# Output
${ECHO} "${XML_HEADER}"
${ECHO} "${XML_INSTRUCTIONS}"
${ECHO} "${XML_INCLUDE_SECTION_BEGIN}"
for l in "${LANGUAGES[@]}"; do
    ${ECHO} "<language name=\"${l}\">"
    ${ECHO} "${INCLUDES_REL}" | awk \
        "{ if (\$0 != \"\") { print ${XML_INCLUDE_TEMPLATE}; } }" | \
        sed -e 's,/[/]*,/,g'
    ${ECHO} '</language>'
done
${ECHO} "${XML_SECTION_END}"
${ECHO} "${XML_MACRO_SECTION_BEGIN}"
for l in "${LANGUAGES[@]}"; do
    ${ECHO} "<language name=\"${l}\">"
    ${ECHO} "${MACROS}" | awk -F= \
        "{ if (\$2 == \"\") { \$2 = \"1\" } print ${XML_MACRO_TEMPLATE}; }" | \
        sed -e 's/\\"/"/g'
    ${ECHO} '</language>'
done
${ECHO} "${XML_SECTION_END}"
${ECHO} "${XML_FOOTER}"
