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

RIOTBASE=$(readlink -f "$(dirname $(realpath $0))/../../..")

CHECK_PATHS="${RIOTBASE}/tests \
             ${RIOTBASE}/dist/tools/testrunner"

# Skip module level import not at top of file because of testrunner import
ERRORS=$(flake8 ${CHECK_PATHS} --max-line-length=119 | sort)

if [ -n "${ERRORS}" ]
then
    echo -e "${CERROR}There are style issues in the following Python scripts:${CRESET}"
    echo "${ERRORS}"
    exit 2
else
    exit 0
fi
