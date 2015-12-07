#!/usr/bin/env bash
# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.

# Drone has not yet implemented Travis Matrix Build feature,
# see https://github.com/drone/drone/issues/6

function exec_build_func {
    group=$1
    shift
    if [ -z "$1" ]; then
        fileargs=$@
    fi

    echo "Begin group ${group}"
    # spawn new directory for the group
    mkdir -p "build_tmp/${group}"
    rsync -a --exclude="bin" --exclude=build_tmp . "build_tmp/${group}"
    cd "build_tmp/${group}"
    # For now, just ride on the Travis build scripts
    BUILDTEST_MCU_GROUP=${group} ./dist/tools/travis-scripts/build_and_test.sh "${fileargs}"
    RES=$?
    # clean temporary build directory for the group
    cd - 2>&1 >/dev/null
    rm -rf "build_tmp/${group}"
    echo "Result (${group}): ${RES}"
}

FAILURES=0
SUMMARY_MESSAGE="Summary of test groups run:\n"

# Travis compatibility defines
export TRAVIS_PULL_REQUEST="${CI_PULL_REQUEST}"
export TRAVIS_BRANCH="${CI_BRANCH}"
export TRAVIS_BUILD_NUMBER="${CI_BUILD_NUMBER}"
export TRAVIS_COMMIT="${DRONE_COMMIT}"
export TRAVIS_BUILD_DIR="${DRONE_BUILD_DIR}"

export -f exec_build_func

MYTMPDIR=$(mktemp -d)
trap 'rm -rf "$MYTMPDIR"' EXIT

# Check for PRs and labels
FULL_CHECK=true
if [ -z "$FORCE_FULL_CHECK" ]; then
    if [ "$CI_PULL_REQUEST" != false ] && ! [ -z "$CI_PULL_REQUEST" ]; then
        # Pull request
        # Check for labels
        . ./dist/tools/pr_check/check_labels.sh
        check_gh_label "Ready for CI build"
        if [ $? -ne 0 ]; then
            # CI test label not set -> run only static tests
            FULL_CHECK=false
        fi
    fi
fi

if $FULL_CHECK; then
    echo "Executing all tests"
    echo "NOTE: output is refreshed only when a test group finishes! Be patient!"
    # Execute all groups in parallel (-k ensures correct ordering of the
    # output)
    parallel -k exec_build_func {} "$@"  ::: static-tests avr8 msp430 x86 arm7 \
                                             cortex_m0_2 cortex_m0_1 \
                                             cortex_m3_1 cortex_m3_2 \
                                             cortex_m4_1 cortex_m4_2 \
    |& tee -a "$MYTMPDIR/output.log"
else
    echo "PR not ready for CI build. Only static-tests will be executed!"
    ((FAILURES++))
    exec_build_func static-tests "$@" |& tee -a "$MYTMPDIR/output.log"
fi

# Check the log for failures
RESULTS=$(cat "$MYTMPDIR/output.log" | grep "Result.*:" | cut -d ":" -f2 \
        | tr "\n" " ")

for retval in $RESULTS;do
    if [ $retval -ne "0" ];then
        ((FAILURES++))
    fi
done

SUMMARY=$(cat "$MYTMPDIR/output.log" | grep -e "^Result.*:" \
        | awk -F '[()]' '{print $(NF-1) $(NF-0)}')
SUMMARY_MESSAGE="${SUMMARY_MESSAGE}${SUMMARY}\n"

echo -n -e "${SUMMARY_MESSAGE}"

if [ ${FAILURES} -eq 0 ]; then
    echo "Build and test: All successful"
else
    echo "Build and test: ${FAILURES} failures in total"
fi

exit ${FAILURES}
