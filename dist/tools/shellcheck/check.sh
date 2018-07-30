#!/usr/bin/env bash
#
# Copyright (C) 2018 Freie Universität Berlin
#                    Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

SHELLCHECK_CMD="$(command -v shellcheck)"
export SHELLCHECK_OPTS="-e SC1090"

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR=$'\e[1;31m'
    CRESET=$'\e[0m'
else
    CERROR=
    CRESET=
fi

: "${RIOTTOOLS:=${PWD}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh

FILES=$(FILEREGEX='(.*\.sh$)' changed_files)

if [ -z "${FILES}" ]
then
    exit 0
fi

${SHELLCHECK_CMD} --version &> /dev/null || {
    printf "%sError: Shellcheck command is missing%s\n" "${CERROR}" "${CRESET}"
    exit 1
}

ERRORS=$("${SHELLCHECK_CMD}" --format=gcc ${FILES})

if [ -n "${ERRORS}" ]
then
    printf "%s There are issues in the following shell scripts %s\n" "${CERROR}" "${CRESET}"
    printf "%s\n" "${ERRORS}"
    exit 1
else
    exit 0
fi
