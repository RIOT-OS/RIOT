#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2020 Inria
# SPDX-FileCopyrightText: 2020 Freie Universität Berlin
# SPDX-FileCopyrightText: 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
# SPDX-License-Identifier: LGPL-2.1-only

# shellcheck source=dist/tools/ci/github_annotate.sh
. "$(dirname "${0}")"/github_annotate.sh

# provides should_run(): only runs tests affected by the branch's changes
# also provides TEST_PATHS which you can append to add more should_run filters to check against
# e.g. if you introduce a test for *.foo files you'd want to add a new TEST_PATHS[any_foo_files]
# shellcheck source=dist/tools/ci/test_selection.sh
. "$(dirname "${0}")"/test_selection.sh

declare -A DEPS

DEPS["./dist/tools/licenses/check.sh"]="head"
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
        if ! command -v "${dep}" &>/dev/null; then
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

should_run always ./dist/tools/whitespacecheck/check.sh "${BASE_BRANCH}"
# licenses runs twice with different DIFFFILTER, needs explicit check
if should_run any_c_files; then
    DIFFFILTER="MR" ERROR_EXIT_CODE=0 run ./dist/tools/licenses/check.sh
    DIFFFILTER="AC" run ./dist/tools/licenses/check.sh
fi
should_run features ./dist/tools/ci/check_features_existing_inc_mk_is_up_to_date.sh
should_run doccheck ./dist/tools/doccheck/check.sh
should_run any_c_headers ./dist/tools/externc/check.sh
should_run any_c_files ./dist/tools/vera++/check.sh
should_run any_c_sources ./dist/tools/coccinelle/check.sh
should_run any_python_files ./dist/tools/flake8/check.sh
should_run any_c_headers ./dist/tools/headerguards/check.sh
should_run always ./dist/tools/buildsystem_sanity_check/check.sh
should_run any_build_files ./dist/tools/feature_resolution/check.sh
should_run any_build_files ./dist/tools/boards_supported/check.sh
should_run board_doc ./dist/tools/board_doc_check/check.sh
should_run codespell ./dist/tools/codespell/check.sh
should_run any_rust_files ./dist/tools/cargo-checks/check.sh
should_run any_example_files ./dist/tools/examples_check/check_has_readme.sh
should_run any_example_files ./dist/tools/examples_check/check_in_readme.sh
should_run code_in_guides ./dist/tools/code_in_guides_check/check_for_code.sh
should_run any_c_files ./dist/tools/uncrustify/uncrustify.sh
ERROR_EXIT_CODE=0 should_run any_c_files ./dist/tools/clang_format/check.sh
ERROR_EXIT_CODE=0 should_run any_shell_files ./dist/tools/shellcheck/check.sh

exit "$RESULT"
