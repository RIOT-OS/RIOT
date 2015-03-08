#!/bin/bash
#
# Copyright (C) 2015 Kushal Singh <kushal.spiderman.singh@gmail.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

BRANCH=${1}
ENDREGEX='^M$'

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
    FILES="$(git ls-tree -r --full-tree --name-only HEAD)"
else
    FILES="$(git diff ${DIFFFILTER} --name-only ${BRANCH})"
fi

if [ -z "${FILES}" ]; then
    exit
fi

grep -E -R -l ${ENDREGEX} ${FILES} > /dev/null

if [ $? -eq 0 ]
then
    echo "ERROR: carriage return error found"
    exit 1
else
    exit 0
fi
