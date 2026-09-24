#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2020 Inria
# SPDX-FileCopyrightText: 2020 Freie Universität Berlin
# SPDX-FileCopyrightText: 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
# SPDX-License-Identifier: LGPL-2.1-only

# shellcheck source=dist/tools/ci/github_annotate.sh
. "$(dirname "${0}")"/github_annotate.sh

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

declare -A TEST_PATHS

TEST_PATHS[whitespacecheck]=""
TEST_PATHS[licenses]="*.c *.h *.s *.S *.cpp"
TEST_PATHS[features]="features.yaml makefiles/features_existing.inc.mk"
TEST_PATHS[doccheck]="doc/* *.c *.h *.hpp *.md *.mdx *.txt makefiles/pseudomodules.inc.mk"
TEST_PATHS[externc]="*.h"
TEST_PATHS[vera]="*.c *.h *.C *.H *.cpp *.hpp"
TEST_PATHS[coccinelle]="*.c"
TEST_PATHS[flake8]="*.py *pyterm"
TEST_PATHS[headerguards]="*.h"
TEST_PATHS[buildsystem]=""
TEST_PATHS[feature_resolution]="features.yaml *Makefile* makefiles/* boards/* cpu/*"
TEST_PATHS[boards_supported]="*Makefile* makefiles/* boards/* cpu/*"
TEST_PATHS[board_doc]="boards/*"
TEST_PATHS[codespell]="*.c *.h *.C *.H *.cpp *.hpp *.sh *.py *.md *.txt"
TEST_PATHS[cargo]="*Cargo.toml *.rs"
TEST_PATHS[examples_readme]="examples/*"
TEST_PATHS[examples_in_readme]="examples/*"
TEST_PATHS[code_in_guides]="doc/guides/* examples/*"
TEST_PATHS[uncrustify]="*.c *.h"
TEST_PATHS[shellcheck]="*.sh"

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

function is_path_changed {
    local base

    [ -z "${BASE_BRANCH}" ] && return 0

    base=$(git merge-base "${BASE_BRANCH}" HEAD 2>/dev/null) || return 0
    [ -n "${base}" ] || return 0

    [ -n "$(git diff --name-only "${base}" -- "$@")" ]
}

function should_run {
    local name="$1"
    local -a paths

    if [ -n "${STATIC_TESTS}" ]; then
        [ "${STATIC_TESTS}" = "all" ] && return 0
        printf '%s\n' "${STATIC_TESTS}" | tr ' ' '\n' | grep -qx "${name}"
        return
    fi

    read -ra paths <<< "${TEST_PATHS[${name}]}"
    is_path_changed "${paths[@]}"
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

should_run whitespacecheck && run ./dist/tools/whitespacecheck/check.sh "${BASE_BRANCH}"
if should_run licenses; then
    DIFFFILTER="MR" ERROR_EXIT_CODE=0 run ./dist/tools/licenses/check.sh
    DIFFFILTER="AC" run ./dist/tools/licenses/check.sh
fi
should_run features && run ./dist/tools/ci/check_features_existing_inc_mk_is_up_to_date.sh
should_run doccheck && run ./dist/tools/doccheck/check.sh
should_run externc && run ./dist/tools/externc/check.sh
# broken configuration produces many false positives
# TODO: fix config and re-enable
# run ./dist/tools/cppcheck/check.sh
should_run vera && run ./dist/tools/vera++/check.sh
should_run coccinelle && run ./dist/tools/coccinelle/check.sh
should_run flake8 && run ./dist/tools/flake8/check.sh
should_run headerguards && run ./dist/tools/headerguards/check.sh
should_run buildsystem && run ./dist/tools/buildsystem_sanity_check/check.sh
should_run feature_resolution && run ./dist/tools/feature_resolution/check.sh
should_run boards_supported && run ./dist/tools/boards_supported/check.sh
should_run board_doc && run ./dist/tools/board_doc_check/check.sh
should_run codespell && run ./dist/tools/codespell/check.sh
should_run cargo && run ./dist/tools/cargo-checks/check.sh
should_run examples_readme && run ./dist/tools/examples_check/check_has_readme.sh
should_run examples_in_readme && run ./dist/tools/examples_check/check_in_readme.sh
should_run code_in_guides && run ./dist/tools/code_in_guides_check/check_for_code.sh
if should_run uncrustify; then
    if [ -z "${GITHUB_RUN_ID}" ]; then
        run ./dist/tools/uncrustify/uncrustify.sh --check
    else
        run ./dist/tools/uncrustify/uncrustify.sh
    fi
fi
# clang-format is only advisory for now: remove the ERROR_EXIT_CODE (and add
# clang-format to DEPS above) once all CI workers ship clang-format >= 17
ERROR_EXIT_CODE=0 run ./dist/tools/clang_format/check.sh
should_run shellcheck && ERROR_EXIT_CODE=0 run ./dist/tools/shellcheck/check.sh

exit "$RESULT"
