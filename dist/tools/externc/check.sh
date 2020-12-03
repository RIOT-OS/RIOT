#!/bin/sh

# Copyright 2017 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
# Copyright 2014 DangNhat Pham-Huu <51002279@hcmut.edu.vn>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh

# prepare
ROOT=$(git rev-parse --show-toplevel)
EXIT_CODE=0

FILES=$(FILEREGEX='\.h$' changed_files)

# check files
for FILE in ${FILES}; do
    if cat "${ROOT}/${FILE}" \
        | sed -e 's/$/ /' \
        | tr -d '\r\n' \
        | sed -e 's/  */ /g' \
        | grep -v -q '#ifdef __cplusplus extern "C" {'; \
    then
        EXIT_CODE=1
        echo "file does not have a C++ compatible header: '${FILE}'"
    fi
done

exit ${EXIT_CODE}
