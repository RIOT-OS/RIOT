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
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh

FILEREGEX='\.([CcHh]|[ch]pp|sh|py|md|txt)$'
EXCLUDE='^(.+/vendor/)'
FILES=$(FILEREGEX=${FILEREGEX} EXCLUDE=${EXCLUDE} changed_files)

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

# Filter-out all false positive raising "disabled due to" messages.
ERRORS=$(${CODESPELL_CMD} ${CODESPELL_OPTS} ${FILES} | grep -ve "disabled due to")

if [ -n "${ERRORS}" ]
then
    printf "%sThere are typos in the following files:%s\n\n" "${CERROR}" "${CRESET}"
    printf "%s\n" "${ERRORS}"
    printf "If those are false positives, add them to %s\n" "${RIOTTOOLS}/codespell/ignored_words.txt"
    exit 1
else
    exit 0
fi
