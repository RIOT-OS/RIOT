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
. "${RIOTTOOLS}"/ci/github_annotate.sh

FILES=$(FILEREGEX='(.*\.sh$)' changed_files)

if [ -z "${FILES}" ]
then
    exit 0
fi

${SHELLCHECK_CMD} --version &> /dev/null || {
    printf "%sError: Shellcheck command is missing%s\n" "${CERROR}" "${CRESET}"
    exit 1
}

github_annotate_setup

# shellcheck disable=SC2086
# FILES is supposed to be split, so don't quote it
ERRORS="$("${SHELLCHECK_CMD}" --format=gcc ${FILES})"

EXIT_CODE=0

if [ -n "${ERRORS}" ]
then
    if github_annotate_is_on; then
        echo "${ERRORS}" | while read -r error; do
            FILENAME=$(echo "${error}" | cut -d: -f1)
            LINENUM=$(echo "${error}" | cut -d: -f2)
            SEVERITY=$(echo "${error}" | cut -d: -f4)
            DETAILS=$(echo "${error}" | cut -d: -f5- |
                      sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')
            if echo "${SEVERITY}" | grep -q 'error'; then
                github_annotate_error "${FILENAME}" "${LINENUM}" "${DETAILS}"
            else
                github_annotate_warning "${FILENAME}" "${LINENUM}" "${DETAILS}"
            fi
        done
    else
        printf "%s There are issues in the following shell scripts %s\n" \
            "${CERROR}" "${CRESET}"
        printf "%s\n" "${ERRORS}"
    fi
    if [ -z "${ERROR_EXIT_CODE}" ]; then
        EXIT_CODE=1
    else
        EXIT_CODE="${ERROR_EXIT_CODE}"
    fi
fi

github_annotate_teardown

exit "${EXIT_CODE}"
