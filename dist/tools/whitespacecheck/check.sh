#!/bin/bash

# Copyright 2015 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

. "$(dirname "$0")/../ci/github_annotate.sh"

IGNORE=$(awk '{ printf ":!%s ", $0 }' "$(dirname "$0")/ignore_list.txt")

# If no branch but an option is given, unset BRANCH.
# Otherwise, consume this parameter.
BRANCH="${1}"
if echo "${BRANCH}" | grep -q '^-'; then
    if [ "$(git rev-parse --abbrev-ref HEAD)" != "master" ]; then
        BRANCH="master"
    else
        BRANCH=""
    fi
else
    if [ -n "${BRANCH}" ]; then
        shift 1
    fi
fi

# sets
# - LOG to tee output into for later parsing
# - LOGFILE to parse GitHub annotations into
github_annotate_setup

# select files to check
if [ -z "${BRANCH}" ]; then
    BRANCH=$(git rev-list HEAD | tail -n 1)
fi

git -c core.whitespace="tab-in-indent,tabwidth=4" \
    diff --check "$(git merge-base "${BRANCH}" HEAD)" -- *.[ch] ${IGNORE} \
            | ${LOG}
RESULT=${PIPESTATUS[0]}

# Git regards any trailing white space except `\n` as an error so `\r` is
# checked here, too
git -c core.whitespace="trailing-space" \
    diff --check "$(git merge-base "${BRANCH}" HEAD)" -- . ${IGNORE} \
            | ${LOG}

TRAILING_RESULT=${PIPESTATUS[0]}

github_annotate_parse_log_default

github_annotate_teardown

if [ ${TRAILING_RESULT} -ne 0 ] || [ ${RESULT} -ne 0 ]
then
    echo "ERROR: This change introduces new whitespace errors"
    exit 1
else
    exit 0
fi
