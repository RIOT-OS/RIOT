# shellcheck shell=bash # this script is only sourced, so no shebang

# SPDX-FileCopyrightText: 2026 AnnsAnn <git@annsann.eu>
# SPDX-License-Identifier: LGPL-2.1-only

# path patterns that trigger each test
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

# true if any given path changed vs BASE_BRANCH; true if base unknown
function is_path_changed {
    local base

    [ -z "${BASE_BRANCH}" ] && return 0

    base=$(git merge-base "${BASE_BRANCH}" HEAD 2>/dev/null) || return 0
    [ -n "${base}" ] || return 0

    [ -n "$(git diff --name-only "${base}" -- "$@")" ]
}

# without command: true if the test should run.
# with command: run() it if so; name "true" runs unconditionally.
function should_run {
    local name="$1"
    local skip=1
    shift

    if [ "${name}" = "true" ]; then
        skip=0
    elif [ -n "${STATIC_TESTS}" ]; then
        # explicit selection: STATIC_TESTS is "all" or space-separated names
        if [ "${STATIC_TESTS}" = "all" ] || \
           printf '%s\n' "${STATIC_TESTS}" | tr ' ' '\n' | grep -qx "${name}"; then
            skip=0
        fi
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
