#!/usr/bin/env bash
#
# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

if tput colors &> /dev/null && [ $(tput colors) -ge 8 ]; then
    CERROR="\e[1;31m"
    CRESET="\e[0m"
else
    CERROR=
    CRESET=
fi

DIST_TOOLS=${RIOTBASE:-.}/dist/tools

. ${DIST_TOOLS}/ci/changed_files.sh

FILES=$(FILEREGEX='(?=*.py$|pyterm$)' changed_files)

if [ -z "${FILES}" ]
then
    exit 0
fi

ERRORS=$(flake8 --config=${DIST_TOOLS}/flake8/flake8.cfg ${FILES})

if [ -n "${ERRORS}" ]
then
    printf "${CERROR}There are style issues in the following Python scripts:${CRESET}\n\n"
    printf "${ERRORS}\n"
    exit 1
else
    exit 0
fi
