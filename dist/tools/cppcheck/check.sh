#!/bin/sh

# Copyright 2014 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
# Copyright 2014 Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Don't show warnings about unusedStructMembers by default
DEFAULT_SUPPRESSIONS="${1}"
if echo "${DEFAULT_SUPPRESSIONS}" | grep -q '^--show-unused-struct'; then
    DEFAULT_SUPPRESSIONS=""
    shift 1
else
    DEFAULT_SUPPRESSIONS=--suppress="unusedStructMember"
fi

BRANCH=${1}
FILEREGEX='\.([sScHh]|cpp)$'
EXCLUDE='^(cpu/saml21/include/atmel/)'

# If no branch but an option is given, unset BRANCH.
# Otherwise, consume this parameter.
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

# select either all or only touched-in-branch files, filter through FILEREGEX
if [ -z "${BRANCH}" ]; then
    FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E ${FILEREGEX})"
else
    FILES="$(git diff ${DIFFFILTER} --name-only ${BRANCH} | grep -E ${FILEREGEX})"
fi

# filter out negatives
FILES=$(echo "${FILES}" | grep -v -E ${EXCLUDE})

if [ -z "${FILES}" ]; then
    exit
fi

# TODO: switch back to 8 jobs when/if cppcheck issue is resolved
cppcheck --std=c99 --enable=style --force --error-exitcode=2 --quiet -j 1 \
         --template "{file}:{line}: {severity} ({id}): {message}"         \
         --inline-suppr ${DEFAULT_SUPPRESSIONS} ${@} ${FILES}
