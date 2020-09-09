#!/usr/bin/env bash
#
# Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
#               2020 Inria
#               2020 Freie Universität Berlin
#               2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

function print_result {
    local RED="\033[0;31m"
    local GREEN="\033[0;32m"
    local NO_COLOUR="\033[0m"

    if (( "$1" == 0 )); then
        echo -e "${GREEN}✓$NO_COLOUR"
    else
        echo -e "${RED}x$NO_COLOUR"
    fi
}

set_result() {
    NEW_RESULT=$1

    if (( NEW_RESULT != 0))
    then
        RESULT=$NEW_RESULT
    fi
}

function run {
    echo -n "Running \"$*\" "
    OUT=$("$@" 2>&1)
    NEW_RESULT=$?

    print_result $NEW_RESULT
    set_result $NEW_RESULT

    # Indent command output so that its easily discernible from the rest
    if [ -n "$OUT" ]; then
        echo "Command output:"
        echo ""
        # Using printf to avoid problems if the command output begins with a -
        (printf "%s\n" "$OUT" | while IFS= read -r line; do printf "\t%s\n" "$line"; done)
        echo ""
    fi
}

RESULT=0

if [ -n "${CI_BASE_COMMIT}" ]; then
    # on Murdock, there's no base branch in the checkout folder.
    # Thus, tag it here.
    echo "-- tagging ${CI_BASE_BRANCH} HEAD commit (${CI_BASE_COMMIT})"
    git tag "${CI_BASE_BRANCH}" "${CI_BASE_COMMIT}"
fi

CI_BASE_BRANCH=${CI_BASE_BRANCH:-master}

export BASE_BRANCH="${CI_BASE_BRANCH}"

run ./dist/tools/commit-msg/check.sh "${BASE_BRANCH}"
run ./dist/tools/whitespacecheck/check.sh "${BASE_BRANCH}"
DIFFFILTER="MR" ERROR_EXIT_CODE=0 run ./dist/tools/licenses/check.sh
DIFFFILTER="AC" run ./dist/tools/licenses/check.sh
run ./dist/tools/doccheck/check.sh
run ./dist/tools/externc/check.sh
run ./dist/tools/cppcheck/check.sh
run ./dist/tools/vera++/check.sh
run ./dist/tools/pr_check/pr_check.sh "${BASE_BRANCH}"
run ./dist/tools/coccinelle/check.sh
run ./dist/tools/flake8/check.sh
run ./dist/tools/headerguards/check.sh
run ./dist/tools/buildsystem_sanity_check/check.sh
run ./dist/tools/codespell/check.sh
run ./dist/tools/uncrustify/uncrustify.sh --check

exit $RESULT
