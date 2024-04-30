#!/usr/bin/env bash
#
# Copyright (C) 2020 HAW Hamburg
#               2020 Kaspar Schleiser <kaspar@schleiser.de>
#               2020 Inria
#               2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# This script performs some tests on the genconfig.py script. It uses as main
# Kconfig tree file 'Kconfig.test'. It runs the script under different
# conditions by injecting different configuration files and checking the return
# values.

KCONFIGLIB_DIR="$(readlink -f "$(dirname "$0")/..")"
TESTS_DIR=${KCONFIGLIB_DIR}/tests
GENCONFIG=${KCONFIGLIB_DIR}/genconfig.py

KCONFIG_FILE="${TESTS_DIR}/Kconfig.test"
CONFIG_FOO_Y="${TESTS_DIR}/foo_y.config"
CONFIG_BAR_Y="${TESTS_DIR}/bar_y.config"
CONFIG_BAZ_Y="${TESTS_DIR}/baz_y.config"
CONFIG_OPT_2_Y="${TESTS_DIR}/opt_2_y.config"

# prints a character representing the result:
#   - a green tick when $1 is 0
#   - a red cross when $1 is 1
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

# sets the global accumulative RESULT variable to keep record of errors during
# tests.
function set_result {
    NEW_RESULT=$1

    if (( NEW_RESULT != 0))
    then
        RESULT=$NEW_RESULT
    fi
}

# runs a test passed in $1. It will print the corresponding name and result and
# update the global state of the tests.
function run_test {
    echo -n "Running test: \"$*\" "
    $1
    res=$?

    print_result $res
    set_result $res
}

# TEST
# checks that the used files are present.
function check_files_exist {
    if ! test -f "${KCONFIG_FILE}"
    then
        1>&2 echo -n "Missing Kconfig file ${KCONFIG_FILE}"
        return 1
    fi
    if ! test -f "${CONFIG_FOO_Y}"
    then
        1>&2 echo -n "Missing configuration file ${CONFIG_FOO_Y}"
        return 1
    fi
    if ! test -f "${CONFIG_BAR_Y}"
    then
        1>&2 echo -n "Missing configuration file ${CONFIG_BAR_Y}"
        return 1
    fi
    if ! test -f "${CONFIG_BAZ_Y}"
    then
        1>&2 echo -n "Missing configuration file ${CONFIG_BAZ_Y}"
        return 1
    fi
    if ! test -f "${CONFIG_OPT_2_Y}"
    then
        1>&2 echo -n "Missing configuration file ${CONFIG_BAZ_Y}"
        return 1
    fi
}

# TEST
# verifies that when the dependencies for the APPLICATION symbol are present
# the symbol gets the 'y' value and the script succeeds.
function application_with_deps_should_succeed {
    if  OUT=$(${GENCONFIG} --kconfig-filename "${KCONFIG_FILE}" \
            --config-sources "${CONFIG_FOO_Y}" 2>&1)
    then
        return 0
    else
        1>&2 echo -n "$OUT"
        return 1
    fi
}

# TEST
# verifies that when a module that the user tried to enable can't be enabled
# (i.e. has missing dependencies) the script fails.
function missing_module_should_fail {
    if OUT=$(${GENCONFIG} --kconfig-filename "${KCONFIG_FILE}" \
           --config-sources "${CONFIG_BAR_Y}" "${CONFIG_FOO_Y}" 2>&1)
    then
        1>&2 echo -n "$OUT"
        return 1
    else
        return 0
    fi
}

# TEST
# verifies that when a module that the user tried to enable has its dependencies
# active, the script succeeds.
function module_with_deps_should_succeed {
    if OUT=$(${GENCONFIG} --kconfig-filename "${KCONFIG_FILE}" \
          --config-sources "${CONFIG_BAZ_Y}" "${CONFIG_BAR_Y}" "${CONFIG_FOO_Y}" 2>&1)
    then
        return 0
    else
        1>&2 echo -n "$OUT"
        return 1
    fi
}

# TEST
# verifies that when a choice option that the user tried to enable can't be
# enabled (i.e. has missing dependencies) the script fails.
function missing_choice_should_fail {
    if OUT=$(${GENCONFIG} --kconfig-filename "${KCONFIG_FILE}" \
          --config-sources "${CONFIG_FOO_Y}" "${CONFIG_OPT_2_Y}" 2>&1)
    then
        1>&2 echo -n "$OUT"
        return 1
    else
        return 0
    fi
}

# TEST
# verifies that when a choice option that the user tried to enable has its
# dependencies active, the script succeeds.
function choice_with_deps_should_succeed {
    if OUT=$(${GENCONFIG} --kconfig-filename "${KCONFIG_FILE}" \
          --config-sources "${CONFIG_FOO_Y}" "${CONFIG_BAZ_Y}" \
          "${CONFIG_OPT_2_Y}" 2>&1)
    then
        return 0
    else
        1>&2 echo -n "$OUT"
        return 1
    fi
}

run_test check_files_exist
run_test application_with_deps_should_succeed
run_test missing_module_should_fail
run_test module_with_deps_should_succeed
run_test missing_choice_should_fail
run_test choice_with_deps_should_succeed

exit $(( RESULT ))
