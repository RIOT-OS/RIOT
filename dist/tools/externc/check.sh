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
. "${RIOTTOOLS}"/ci/github_annotate.sh

github_annotate_setup

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
        ANNOTATION="File does not have a C++ compatible header"
        EXIT_CODE=1
        echo "${ANNOTATION}: '${FILE}'"
        ANNOTATION="${ANNOTATION}.\nPlease add:\n\n"
        ANNOTATION="${ANNOTATION}"'```C\n'
        ANNOTATION="${ANNOTATION}#ifdef __cplusplus\n"
        ANNOTATION="${ANNOTATION}extern \"C\" {\n"
        ANNOTATION="${ANNOTATION}#endif\n\n"
        ANNOTATION="${ANNOTATION}/* your file content */\n\n"
        ANNOTATION="${ANNOTATION}"'#ifdef __cplusplus\n'
        ANNOTATION="${ANNOTATION}}\n"
        ANNOTATION="${ANNOTATION}"'#endif\n'
        ANNOTATION="${ANNOTATION}"'```\n\n'
        ANNOTATION="${ANNOTATION}after your header \`#include\`s"
        github_annotate_error "${FILE}" 0 "${ANNOTATION}"
    fi
done

github_annotate_teardown

exit ${EXIT_CODE}
