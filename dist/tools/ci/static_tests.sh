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

. "$(dirname "${0}")"/github_annotate.sh

declare -A DEPS

DEPS["./dist/tools/licenses/check.sh"]="head pcregrep"
DEPS["./dist/tools/doccheck/check.sh"]="doxygen tput"
DEPS["./dist/tools/cppcheck/check.sh"]="cppcheck"
DEPS["./dist/tools/vera++/check.sh"]="vera++"
DEPS["./dist/tools/coccinelle/check.sh"]="spatch"
DEPS["./dist/tools/flake8/check.sh"]="python3 flake8"
DEPS["./dist/tools/codespell/check.sh"]="codespell"
DEPS["./dist/tools/uncrustify/uncrustify.sh"]="uncrustify"
DEPS["./dist/tools/shellcheck/shellcheck.sh"]="shellcheck"

if ! command -v git &>/dev/null; then
    echo -n "Required command 'git' for all static tests not found in PATH "
    print_warning
    set_result 1
    exit 1
fi

function print_warning {
    local YELLOW="\033[0;33m"
    local NO_COLOUR="\033[0m"

    echo -e "${YELLOW}•${NO_COLOUR}"
}

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
    for dep in ${DEPS["$1"]}; do
        if ! command -v ${dep} &>/dev/null; then
            echo -n "Required command '${dep}' for '$*' not found in PATH "
            print_warning
            set_result 1
            return 1
        fi
    done

    if [ -n "${GITHUB_RUN_ID}" ]; then
        echo -n "::group::$1 "
    else
        echo -n "Running \"$*\" "
    fi
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
    if [ -n "${GITHUB_RUN_ID}" ]; then
        github_annotate_report_last_run
        echo "::endgroup::"
    fi
}

RESULT=0

if [ -n "${CI_BASE_COMMIT}" ]; then
    # on Murdock, there's no base branch in the checkout folder.
    # Thus, tag it here.
    echo "-- tagging ${CI_BASE_BRANCH} HEAD commit (${CI_BASE_COMMIT})"
    git tag "${CI_BASE_BRANCH}" "${CI_BASE_COMMIT}"
fi

if [ -z "${GITHUB_RUN_ID}" ]; then
    # only default to master when not running in a GitHub action
    # (so GitHub can check release branches too)
    CI_BASE_BRANCH=${CI_BASE_BRANCH:-master}
fi

export BASE_BRANCH="${CI_BASE_BRANCH}"

run ./dist/tools/whitespacecheck/check.sh "${BASE_BRANCH}"
DIFFFILTER="MR" ERROR_EXIT_CODE=0 run ./dist/tools/licenses/check.sh
DIFFFILTER="AC" run ./dist/tools/licenses/check.sh
run ./dist/tools/ci/check_features_existing_inc_mk_is_up_to_date.sh
run ./dist/tools/doccheck/check.sh
run ./dist/tools/externc/check.sh
# broken configuration produces many false positives
# TODO: fix config and re-enable
# run ./dist/tools/cppcheck/check.sh
run ./dist/tools/vera++/check.sh
run ./dist/tools/coccinelle/check.sh
run ./dist/tools/flake8/check.sh
run ./dist/tools/headerguards/check.sh
run ./dist/tools/buildsystem_sanity_check/check.sh
run ./dist/tools/feature_resolution/check.sh
run ./dist/tools/boards_supported/check.sh
run ./dist/tools/codespell/check.sh
run ./dist/tools/cargo-checks/check.sh
run ./dist/tools/examples_check/check_has_readme.sh
run ./dist/tools/examples_check/check_in_readme.sh
if [ -z "${GITHUB_RUN_ID}" ]; then
    run ./dist/tools/uncrustify/uncrustify.sh --check
else
    run ./dist/tools/uncrustify/uncrustify.sh
fi
ERROR_EXIT_CODE=0 run ./dist/tools/shellcheck/check.sh

exit $RESULT
