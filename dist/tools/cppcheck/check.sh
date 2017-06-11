#!/bin/sh

# Copyright 2017 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
# Copyright 2014 Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

. ${RIOTBASE:+${RIOTBASE}/}dist/tools/ci/changed_files.sh

# Don't show warnings about unusedStructMembers by default
DEFAULT_SUPPRESSIONS="${1}"
if echo "${DEFAULT_SUPPRESSIONS}" | grep -q '^--show-unused-struct'; then
    DEFAULT_SUPPRESSIONS=""
    shift 1
else
    DEFAULT_SUPPRESSIONS=--suppress="unusedStructMember"
fi

FILES=$(changed_files)

if [ -z "${FILES}" ]; then
    exit
fi

# TODO: switch back to 8 jobs when/if cppcheck issue is resolved
cppcheck --std=c99 --enable=style --force --error-exitcode=2 --quiet -j 1 \
         --template "{file}:{line}: {severity} ({id}): {message}"         \
         --inline-suppr ${DEFAULT_SUPPRESSIONS} ${@} ${FILES}
