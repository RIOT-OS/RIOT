#! /bin/bash
#
# Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

EXIT_CODE=0
GITHUB_API_HOST="https://api.github.com"
GITHUB_REPO="RIOT-OS/RIOT"

if which wget &> /dev/null; then
    GET="wget -O -"
elif which curl &> /dev/null; then
    GET="curl"
else
    echo "Script needs wget or curl" >&2
    exit 2
fi

LABELS_JSON=$(${GET} "${GITHUB_API_HOST}/repos/${GITHUB_REPO}/issues/${TRAVIS_PULL_REQUEST}/labels" 2> /dev/null)

if tput colors &> /dev/null && [ $(tput colors) -ge 8 ]; then
    CERROR="\e[1;31m"
    CRESET="\e[0m"
else
    CERROR=
    CRESET=
fi

check_gh_label() {
    LABEL="${1}"

    echo "${LABELS_JSON}" | grep -q "${LABEL}"
    return $?
}

if [[ ${#} -eq 1 ]]; then
    RIOT_MASTER="${1}"
else
    RIOT_MASTER="master"
fi

SQUASH_COMMITS="$(git log $(git merge-base HEAD "${RIOT_MASTER}")...HEAD --pretty=format:"    %h %s" | \
                  grep -i -e "^    [0-9a-f]\{7\} .\{0,2\}SQUASH" -e "^    [0-9a-f]\{7\} .\{0,2\}FIX")"

if [ -n "${SQUASH_COMMITS}" ]; then
    echo -e "${CERROR}Pull request needs squashing:${CRESET}" 1>&2
    echo -e "${SQUASH_COMMITS}"
    EXIT_CODE=1
fi

if [ -n "$TRAVIS_PULL_REQUEST" ]; then
    if check_gh_label "NEEDS SQUASHING"; then
        echo -e "${CERROR}Pull request needs squashing according to its labels${CRESET}"
        EXIT_CODE=1
    fi

    if check_gh_label "Waiting For Other PR"; then
        echo -e "${CERROR}Pull request is waiting for another pull request according to its labels${CRESET}"
        EXIT_CODE=1
    fi
fi

exit ${EXIT_CODE}
