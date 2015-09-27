#!/bin/sh

# Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
# Copyright 2014 DangNhat Pham-Huu <51002279@hcmut.edu.vn>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# customizable
CHECKROOT=$(dirname "${0}")

# prepare
ROOT=$(git rev-parse --show-toplevel)
EXIT_CODE=0
BRANCH="${1}"
DIFFFILTER="${2}"

# set default diff-filter
if [ -z "${DIFFFILTER}" ]; then
    DIFFFILTER="ACMR"
fi

# select files to check
if [ -z "${BRANCH}" ]; then
    FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E '\.h$')"
else
    FILES="$(git diff --diff-filter=${DIFFFILTER} --name-only ${BRANCH} | grep -E '\.h$')"
fi

FILES=$(echo "${FILES}" | grep -v -E '^(dist/tools|boards/msba2-common/tools/|cpu/saml21/include/atmel/)')

# check files
for FILE in ${FILES}; do
    if cat "${ROOT}/${FILE}" \
        | sed -e 's/$/ /' \
        | tr -d '\r\n' \
        | sed -e 's/  */ /g' \
        | grep -v -q '#ifdef __cplusplus extern "C" { #endif'; \
    then
        EXIT_CODE=1
        echo "file does not have a C++ compatible header: '${FILE}'"
    fi
done

exit ${EXIT_CODE}
