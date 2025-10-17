#!/bin/sh

# Copyright 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# shellcheck source=/dev/null
. "$(dirname "$0")/../ci/github_annotate.sh"

github_annotate_setup

MSG_MAX_LENGTH=50
MSG_STRETCH_LENGTH=72

if tput colors 2> /dev/null > /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR1="\033[1;31m"
    CWARN1="\033[1;33m"
    CERROR2="\033[0;31m"
    CWARN2="\033[0;33m"
    CRESET="\033[0m"
else
    CERROR1=
    CWARN1=
    CERROR2=
    CWARN2=
    CRESET=
fi
if [ -n "${BASH_VERSION}" ]; then
    # workaround when included in bash to escape newlines and carriage returns
    # properly in _escape
    ECHO_ESC='echo -e'
else
    ECHO_ESC='echo'
fi

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

# select HEAD, if no branch is given
if [ -z "${BRANCH}" ]; then
    BRANCH=$(git rev-list HEAD | tail -n 1)
fi

ERROR="$(git log \
    --no-merges --pretty=format:'%s' "$(git merge-base "${BRANCH}" HEAD)"..HEAD | \
    while read -r msg; do
        msg_length="$(echo "${msg}" | awk '{print length($0)}')"

        if [ "${msg_length}" -gt "${MSG_MAX_LENGTH}" ]; then
            ERROR=0
            if [ "${msg_length}" -gt "${MSG_STRETCH_LENGTH}" ]; then
                MSG="Commit message is longer than ${MSG_STRETCH_LENGTH} characters:"
                ERROR=1
                echo "error"
            else
                MSG="Commit message is longer than ${MSG_MAX_LENGTH}"
                MSG="${MSG} (but < ${MSG_STRETCH_LENGTH}) characters:"
            fi
            if github_annotate_is_on; then
                if [ ${ERROR} -eq 1 ]; then
                    github_annotate_error_no_file "${MSG} \"${msg}\""
                else
                    github_annotate_warning_no_file "${MSG} \"${msg}\""
                fi
            else
                if [ ${ERROR} -eq 1 ]; then
                    ${ECHO_ESC} "${CERROR1}Error:${CERROR2} ${MSG}${CRESET}" >&2
                else
                    ${ECHO_ESC} "${CWARN1}Warning:${CWARN2} ${MSG}${CRESET}" >&2
                fi
                echo "    \"${msg}\"" >&2
            fi
        fi
    done)"

github_annotate_teardown

github_annotate_report_last_run

if [ -n "${ERROR}" ]; then
    exit 1
fi
