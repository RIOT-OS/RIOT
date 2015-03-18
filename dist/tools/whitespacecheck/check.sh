#!/bin/sh

# Copyright 2015 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
    BRANCH=$(git rev-list HEAD | tail -n 1)
fi

git diff --check $(git merge-base ${BRANCH} HEAD)
if [ $? -gt 0 ]
then
    echo "ERROR: This change introduces new whitespace errors"
    exit 1
else
    exit 0
fi
