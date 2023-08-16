#!/usr/bin/env bash

# Copyright 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

CODESPELL_CMD="codespell"

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR=$'\033[1;31m'
    CRESET=$'\033[0m'
else
    CERROR=
    CRESET=
fi

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"

REL_SCRIPT_DIR=$(realpath --relative-to=${RIOTBASE} $(cd $(dirname $0); pwd))
REL_SCRIPT_PATH=${REL_SCRIPT_DIR}/$(basename "$0")

cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh

FILEREGEX='\.([CcHh]|[ch]pp|sh|py|md|txt)$'
FILES=$(FILEREGEX=${FILEREGEX} changed_files)

if [ -z "${FILES}" ]; then
    exit 0
fi

${CODESPELL_CMD} --version &> /dev/null || {
    printf "%s%s: cannot execute \"%s\"!%s\n" "${CERROR}" "$0" "${CODESPELL_CMD}" "${CRESET}"
    exit 1
}

CODESPELL_OPTS="-q 2"  # Disable "WARNING: Binary file"
CODESPELL_OPTS+=" --check-hidden"
CODESPELL_OPTS+=" --ignore-words ${RIOTTOOLS}/codespell/ignored_words.txt"
CODESPELL_OPTS+=" -c"   # Enable colored output

if [ "${CODESPELL_INTERACTIVE}" = "1" ]; then
    # interactive mode
    CODESPELL_OPTS+=" -w -i3"
    exec ${CODESPELL_CMD} ${CODESPELL_OPTS} ${FILES}
    # (exits shell)
fi

# non-interactive mode
. "${RIOTTOOLS}"/ci/github_annotate.sh

github_annotate_setup

# Filter-out all false positive raising "disabled due to" messages.
ERRORS=$(${CODESPELL_CMD} ${CODESPELL_OPTS} ${FILES} | grep -ve "disabled due to")

EXIT_CODE=0

if [ -n "${ERRORS}" ]
then
    IGNORED_WORDS="${REL_SCRIPT_DIR}/ignored_words.txt"
    if github_annotate_is_on; then
        echo "${ERRORS}" | sed -r "s/[[:cntrl:]]\[[0-9]{1,3}m//g" | \
            while read error; do
            FILENAME=$(echo "$error" | cut -d: -f1)
            LINENUM=$(echo "$error" | cut -d: -f2)
            ERROR=$(echo "$error" | cut -d: -f3- |
                    sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')
            INTERACTIVE_CALL="CODESPELL_INTERACTIVE=1"
            INTERACTIVE_CALL="${INTERACTIVE_CALL} BASE_BRANCH=${BASE_BRANCH}"
            INTERACTIVE_CALL="${INTERACTIVE_CALL} ./${REL_SCRIPT_PATH}"

            ANNOTATION="There is a typo: ${ERROR}\n"
            ANNOTATION="${ANNOTATION}\nIf this is a false positive,"
            ANNOTATION="${ANNOTATION} add it to ${IGNORED_WORDS}."
            ANNOTATION="${ANNOTATION}\n\nYou can fix this interactively by"
            ANNOTATION="${ANNOTATION} calling\n\n"
            ANNOTATION="${ANNOTATION}     ${INTERACTIVE_CALL}"
            github_annotate_error "${FILENAME}" "${LINENUM}" "${ANNOTATION}"
        done
    else
        printf "%sThere are typos in the following files:%s\n\n" "${CERROR}" "${CRESET}"
        printf "%s\n" "${ERRORS}"
        printf "If those are false positives, add them to %s\n" "${IGNORED_WORDS}"
    fi
    EXIT_CODE=1
fi

github_annotate_teardown
exit ${EXIT_CODE}
