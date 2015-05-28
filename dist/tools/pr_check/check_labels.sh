#! /bin/bash
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
GITHUB_REPO="gebart/RIOT"

if which wget &> /dev/null; then
    GET="wget -O -"
elif which curl &> /dev/null; then
    GET="curl"
else
    echo "Script needs wget or curl" >&2
    exit 2
fi

# initialize label cache variable
LABELS_JSON=

if [ -z "${CI_PULL_REQUEST}" ]; then
    # Travis does not set the CI_PULL_REQUEST environment variable
    # Other PR systems (e.g. Drone, Strider, Codeship) usually set the
    # variable CI_PULL_REQUEST to the pull request number.
    if [ -n "${TRAVIS_PULL_REQUEST}" ]; then
        CI_PULL_REQUEST=${TRAVIS_PULL_REQUEST}
    fi
fi

check_gh_label() {
    if [ -z "${CI_PULL_REQUEST}" ]; then
        # No pull request number, this happens if we run the CI on a branch
        # which is not part of a PR, e.g. master, or if we run the CI scripts
        # from the command line.
        return 2
    fi
    LABEL="${1}"

    if [ -z "${LABELS_JSON}" ]; then
        LABELS_JSON=$(${GET} "${GITHUB_API_HOST}/repos/${GITHUB_REPO}/issues/${CI_PULL_REQUEST}/labels" 2> /dev/null)
        if [ "$?" -ne 0 ]; then
            # Reading labels failed
            return 3
        fi
    fi

    printf '%s\n' "${LABELS_JSON}" | grep -q "${LABEL}"
    return $?
}

# make sure we don't trigger test script failure because of the return status
# of the environment checks above.
true
