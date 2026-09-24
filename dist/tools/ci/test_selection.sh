# shellcheck shell=bash # this script is only sourced, so no shebang

# SPDX-FileCopyrightText: 2026 AnnsAnn <git@annsann.eu>
# SPDX-License-Identifier: LGPL-2.1-only

# path patterns that trigger each test, empty runs unconditionally.
# all_* groups exist for reuse in entries; they also work as should_run names.
declare -A TEST_PATHS

# Reusable path groups for tests
TEST_PATHS[always]=""
TEST_PATHS[all_c_headers]="*.h *.H *.hpp"
TEST_PATHS[all_c_sources]="*.c *.s *.S *.cpp *.C"
TEST_PATHS[all_shell_files]="*.sh"
TEST_PATHS[all_python_files]="*.py *pyterm"
TEST_PATHS[all_rust_files]="*Cargo.toml *.rs"
TEST_PATHS[all_text_files]="*.md *.mdx *.txt"
TEST_PATHS[all_make_files]="*Makefile* makefiles/*"
TEST_PATHS[all_doc_files]="doc/* ${TEST_PATHS[all_text_files]}"
TEST_PATHS[all_example_files]="examples/*"
TEST_PATHS[all_c_files]="${TEST_PATHS[all_c_headers]} ${TEST_PATHS[all_c_sources]}"
TEST_PATHS[all_build_files]="features.yaml ${TEST_PATHS[all_make_files]} boards/* cpu/*"

# Test specific path patterns, e.g. if multiple checks are needed.
# Tests matching a single group use the group name directly.
TEST_PATHS[features]="features.yaml makefiles/features_existing.inc.mk"
TEST_PATHS[doccheck]="${TEST_PATHS[all_doc_files]} ${TEST_PATHS[all_c_files]} makefiles/pseudomodules.inc.mk"
TEST_PATHS[board_doc]="boards/*"
TEST_PATHS[codespell]="${TEST_PATHS[all_c_files]} ${TEST_PATHS[all_shell_files]} ${TEST_PATHS[all_python_files]} ${TEST_PATHS[all_text_files]}"
TEST_PATHS[code_in_guides]="doc/guides/* ${TEST_PATHS[all_example_files]}"

# true if any given path changed vs BASE_BRANCH; true if base unknown
function is_path_changed {
    local base

    [ -z "${BASE_BRANCH}" ] && return 0

    base=$(git merge-base "${BASE_BRANCH}" HEAD 2>/dev/null) || return 0
    [ -n "${base}" ] || return 0

    [ -n "$(git diff --name-only "${base}" -- "$@")" ]
}

# without command: true if the test should run.
# with command: run() it if so; name "always" runs unconditionally.
# parameters:
#   $1: name of the test (key in TEST_PATHS)
#   $@: command to run if the test should run
function should_run {
    local name="$1"
    local skip=1
    shift

    if [ -n "${STATIC_TESTS}" ]; then
        # explicit selection: STATIC_TESTS is "all" or space-separated names
        if [ "${STATIC_TESTS}" = "all" ] || \
           printf '%s\n' "${STATIC_TESTS}" | tr ' ' '\n' | grep -qx "${name}"; then
            skip=0
        fi
    elif [ -z "${TEST_PATHS[${name}]}" ]; then
        skip=0
    else
        local -a paths
        read -ra paths <<< "${TEST_PATHS[${name}]}"
        is_path_changed "${paths[@]}" && skip=0
    fi

    if [ $# -eq 0 ]; then
        return "${skip}"
    fi

    [ "${skip}" -eq 0 ] && run "$@"
}
