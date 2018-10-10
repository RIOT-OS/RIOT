#!/bin/sh

# Copyright 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

MSG_MAX_LENGTH=50
MSG_STRETCH_LENGTH=72

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

# select HEAD, if no branch is given
if [ -z "${BRANCH}" ]; then
    BRANCH=$(git rev-list HEAD | tail -n 1)
fi

ERROR="$(git log \
    --no-merges --pretty=format:'%s' $(git merge-base ${BRANCH} HEAD)..HEAD | \
    while read msg; do
        msg_length=$(echo "${msg}" | awk '{print length($0)}')

        if [ ${msg_length} -gt ${MSG_MAX_LENGTH} ]; then
            if [ ${msg_length} -gt ${MSG_STRETCH_LENGTH} ]; then
                MSG="Error: Commit message is longer than ${MSG_STRETCH_LENGTH} characters:"
                echo "error"
            else
                MSG="Warning: Commit message is longer than ${MSG_MAX_LENGTH}"
                MSG="${MSG} (but < ${MSG_STRETCH_LENGTH}) characters:"
            fi
            echo "${MSG}" >&2
            echo "    \"${msg}\"" >&2
        fi
    done)"

if [ -n "${ERROR}" ]; then
    exit 1
fi
