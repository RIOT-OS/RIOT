#!/usr/bin/env bash
#
# Run a simple tests to check various inputs to compile_like_murdock.py in an
# attempt to prevent regressions as features get added.
# This can be invoked manually and also via github actions.


readonly COMPILE_TEST_DIR="$(readlink -f "$(dirname "$0")/..")"


# sets the global accumulative RESULT variable to keep record of errors during
# tests.
function accumulate_errors {
    if (( $1 != 0 )); then
        RESULT=$1
    fi
}

# Run a test passed in $1. It will print the corresponding name and result and
# update the global state of the tests.
function run_test {
    echo -n "Running test: \"$*\" "
    if OUT=$($1 2>&1); then
        echo -e "\033[0;32m✓\033[0m"
    else
        accumulate_errors $?
        echo -e "\033[0;31mx\033[0m"
        1>&2 echo "$OUT"
    fi
}

# Call compile_like_murdock.py with default arguments.
function call_with_defaults {
    ${COMPILE_TEST_DIR}/compile_like_murdock.py --dry-run
}

# Call compile_like_murdock.py with specific board and app arguments.
function call_with_specific_board_app {
    ${COMPILE_TEST_DIR}/compile_like_murdock.py \
    --boards native \
    --apps tests/sys/shell/ \
    --dry-run
}

RESULT=0
run_test call_with_defaults
run_test call_with_specific_board_app

exit $(( RESULT ))
