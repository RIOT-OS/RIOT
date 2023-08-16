#!/bin/bash

# Copyright 2017 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
# Copyright 2014 Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh
. "${RIOTTOOLS}"/ci/github_annotate.sh

# Don't show warnings about unusedStructMembers by default
DEFAULT_SUPPRESSIONS="${1}"
if echo "${DEFAULT_SUPPRESSIONS}" | grep -q '^--show-unused-struct'; then
    DEFAULT_SUPPRESSIONS=""
    shift 1
else
    DEFAULT_SUPPRESSIONS=--suppress="unusedStructMember"
fi

FILES=""
CPPCHECK_OPTIONS=""
IN_FILES_SECTION=false
while [ $# -gt 0 ]; do
    if [ "$1" = "--" ]; then
        IN_FILES_SECTION=true
        shift
        continue
    fi

    if [ "$IN_FILES_SECTION" = false ]; then
        CPPCHECK_OPTIONS="${CPPCHECK_OPTIONS} $1"
    else
        FILES="${FILES} $1"
    fi

    shift
done

if [ -z "${FILES}" ]; then
    FILES=$(changed_files)
fi

if [ -z "${FILES}" ]; then
    exit
fi

# sets
# - LOG to tee output into for later parsing
# - LOGFILE to parse GitHub annotations into
github_annotate_setup

# TODO: switch back to 8 jobs when/if cppcheck issue is resolved
cppcheck --std=c99 --enable=style --force --error-exitcode=2 --quiet -j 1 \
         --template "{file}:{line}: {severity} ({id}): {message}"         \
         --inline-suppr ${DEFAULT_SUPPRESSIONS} ${CPPCHECK_OPTIONS} ${@}  \
         ${FILES} 2>&1 | ${LOG} 1>&2

RESULT=${PIPESTATUS[0]}

if github_annotate_is_on; then
    grep "^.\+:[0-9]\+: [a-z]\+ (.*):" ${LOGFILE} | while read error_line; do
        FILE=$(echo "${error_line}" | cut -d: -f1)
        LINENUM=$(echo "${error_line}" | cut -d: -f2)
        SEVERITY=$(echo "${error_line}" | cut -d: -f3)
        ID=$(echo "${SEVERITY}" | sed 's/^.*(\(.*\))$/\1/g')
        DETAILS=$(echo "${error_line}" | cut -d: -f4- |
                  sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')
        DETAILS="${DETAILS}\n\n"
        DETAILS="${DETAILS}If you are sure this is a false positive, you can "
        DETAILS="${DETAILS}suppress this by adding the following comment above "
        DETAILS="${DETAILS}this line:\n\n"
        DETAILS="${DETAILS}    /* cppcheck-suppress ${ID}\n"
        DETAILS="${DETAILS}     * (reason: <your reason why you think this is "
        DETAILS="${DETAILS}a false positive>) */\n"

        # contributors get confused if CI errors (i.e. non-zero result), but outputs only warnings
        # => make all warnings errors on non-zero output. Otherwise, use severity level of output
        # to determine annotation type.
        if [ ${RESULT} -ne 0 ] || echo "${SEVERITY}" | grep -q "\<error\>"; then
            github_annotate_error "${FILE}" "${LINENUM}" "${DETAILS}"
        else
            github_annotate_warning "${FILE}" "${LINENUM}" "${DETAILS}"
        fi
    done
fi

github_annotate_teardown
exit ${RESULT}
