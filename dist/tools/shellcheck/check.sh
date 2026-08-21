#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2018 Freie Universität Berlin
# SPDX-FileCopyrightText: 2018 Inria
# SPDX-License-Identifier: LGPL-2.1-only

SHELLCHECK_CMD="$(command -v shellcheck)"
export SHELLCHECK_OPTS="-x" # allow shellcheck to follow sourced files

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR=$'\e[1;31m'
    CRESET=$'\e[0m'
else
    CERROR=
    CRESET=
fi

: "${RIOTBASE:=$(cd "$(dirname "$0")"/../../../ || exit 1; pwd)}"
: "${RIOTTOOLS:=${PWD}/dist/tools}"

# shellcheck source=dist/tools/ci/changed_files.sh
. "${RIOTTOOLS}"/ci/changed_files.sh
# shellcheck source=dist/tools/ci/github_annotate.sh
. "${RIOTTOOLS}"/ci/github_annotate.sh

# RIOTBASE should be the starting point for all sourced file paths
SHELLCHECK_OPTS=${SHELLCHECK_OPTS}" --source-path=${RIOTBASE}"

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
