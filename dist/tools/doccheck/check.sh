#!/bin/sh

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

TMP_ERR=$(mktemp)
TMP_WARN=$(mktemp)
TMP_DOC=$(mktemp)
ERR_DIFFFILTER="--diff-filter=AC"
WARN_DIFFFILTER="--diff-filter=MR"

# If no branch but an option is given, unset BRANCH.
# Otherwise, consume this parameter.
BRANCH="${1}"
if echo "${BRANCH}" | grep -q '^-'; then
    if [ $(git rev-parse --abbrev-ref HEAD) != "master" ]; then
        BRANCH="master"
    else
        BRANCH=""
    fi
else
    if [ -n "${BRANCH}" ]; then
        shift 1
    fi
fi


# select files to check
if [ -z "${BRANCH}" ]; then
    git ls-tree -r --full-tree --name-only HEAD | grep -E '\.([sSch]|cpp)$' | sort | uniq > ${TMP_WARN}
else
    git diff ${ERR_DIFFFILTER} --name-only $(git merge-base ${BRANCH} HEAD) | grep -E '\.([sSchp]|cpp)$' | sort | uniq > ${TMP_ERR}
    git diff ${WARN_DIFFFILTER} --name-only $(git merge-base ${BRANCH} HEAD) | grep -E '\.([sSchp]|cpp)$' | sort | uniq > ${TMP_WARN}
fi

make doc 2>&1 | grep '.*warning' | sed "s#.*${PWD}/\([^:]*\).*#\1#" | sort | uniq > ${TMP_DOC}

WARNINGS=$(comm -1 -2 ${TMP_WARN} ${TMP_DOC})
ERRORS=$(comm -1 -2 ${TMP_ERR} ${TMP_DOC})

rm ${TMP_ERR} ${TMP_WARN} ${TMP_DOC}
    
if [ -n "${WARNINGS}" ]
then
echo "WARNING: The following modified files generate doxygen warnings:"
echo "${WARNINGS}"
fi

if [ -n "${ERRORS}" ]
then
    echo "ERROR: The following new files generate doxygen warnings:"
    echo "${ERRORS}"
    exit 2
else
    exit 0
fi
