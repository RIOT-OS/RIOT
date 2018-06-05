#!/usr/bin/env bash
#
# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

FLAKE8_CMD="python3 -m flake8"

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR=$'\033[1;31m'
    CRESET=$'\033[0m'
else
    CERROR=
    CRESET=
fi

: "${RIOTTOOLS:=${PWD}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh

FILEREGEX='(\.py$|pyterm$)'
FILES=$(FILEREGEX=${FILEREGEX} changed_files)

if [ -z "${FILES}" ]
then
    exit 0
fi

${FLAKE8_CMD} --version &> /dev/null || {
    printf "%s%s: cannot execute \"%s\"!%s\n" "${CERROR}" "$0" "${FLAKE8_CMD}" "${CRESET}"
    exit 1
}

ERRORS=$(${FLAKE8_CMD} --config="${RIOTTOOLS}"/flake8/flake8.cfg ${FILES})

if [ -n "${ERRORS}" ]
then
    printf "%sThere are style issues in the following Python scripts:%s\n\n" "${CERROR}" "${CRESET}"
    printf "%s\n" "${ERRORS}"
    exit 1
else
    exit 0
fi
