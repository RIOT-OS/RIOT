#!/usr/bin/env bash
#
# Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

# The following script part has been moved here from:
# ./dist/tools/pr_check/pr_check.sh

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

check_gh_label() {
    LABEL="${1}"

    echo "${LABELS_JSON}" | grep -q "${LABEL}"
    return $?
}
