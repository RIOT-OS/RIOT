#!/bin/sh

# Copyright 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# customizable
CHECKROOT=$(dirname "${0}")
LICENSEDIR="${CHECKROOT}/patterns"
OUTPUT="${CHECKROOT}/out"
UNKNOWN="${OUTPUT}/unknown"
TMP="${CHECKROOT}/.tmp"

# prepare
ROOT=$(git rev-parse --show-toplevel)
LICENSES=$(ls "${LICENSEDIR}")
EXIT_CODE=0
ERROR_EXIT_CODE="1"

# reset output dir
rm -fr "${OUTPUT}"
mkdir -p "${OUTPUT}"
for LICENSE in ${LICENSES}; do
    echo -n '' > "${OUTPUT}/${LICENSE}"
done

# If no branch but an option is given, unset BRANCH.
# Otherwise, consume this parameter.
BRANCH="${1}"
if echo "${BRANCH}" | grep -q '^-'; then
    BRANCH=""
else
    if [ -n "${BRANCH}" ]; then
        shift 1
    fi
fi

# If the --diff-filter option is given, consume this parameter.
# Set the default DIFFFILTER option otherwise.
DIFFFILTER="${1}"
if echo "${DIFFFILTER}" | grep -q '^--diff-filter='; then
    shift 1
else
    DIFFFILTER="--diff-filter=ACMR"
fi

# If the --error-exitcode option is given, consume this parameter
# and overwrite the default ERROR_EXIT_CODE.
if echo "${1}" | grep -q '^--error-exitcode='; then
    ERROR_EXIT_CODE=$(echo ${1} | sed -e 's/--error-exitcode=//')
    shift 1
fi

# select files to check
if [ -z "${BRANCH}" ]; then
    FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E '\.([sSch]|cpp)$')"
else
    FILES="$(git diff ${DIFFFILTER} --name-only ${BRANCH} | grep -E '\.([sSchp]|cpp)$')"
fi

# categorize files
for FILE in ${FILES}; do
    FAIL=1
    head -100 "${ROOT}/${FILE}" | sed -e 's/[\/\*\t]/ /g' -e 's/$/ /' | tr -d '\r\n' | sed -e 's/  */ /g' > "${TMP}"
    for LICENSE in ${LICENSES}; do
        if pcregrep -q -f "${LICENSEDIR}/${LICENSE}" "${TMP}"; then
            echo "${FILE}" >> "${OUTPUT}/${LICENSE}"
            FAIL=0
            break
        fi
    done
    if [ ${FAIL} = 1 ]; then
        echo "${FILE}" >> "${UNKNOWN}"
        echo "file has an unknown license header: '${FILE}'"
        EXIT_CODE=${ERROR_EXIT_CODE}
    fi
done

exit ${EXIT_CODE}
