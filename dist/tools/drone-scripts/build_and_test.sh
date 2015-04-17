#!/bin/bash
# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.

# For now, just ride on the Travis build scripts

# Drone has not yet implemented Travis' Matrix Build feature,
# see https://github.com/drone/drone/issues/6
FAILURES=0
SUMMARY_MESSAGE="Summary of test groups run:\n"

# Travis compatibility defines
export TRAVIS_PULL_REQUEST="${CI_PULL_REQUEST}"
export TRAVIS_BRANCH="${CI_BRANCH}"
export TRAVIS_BUILD_NUMBER="${CI_BUILD_NUMBER}"
export TRAVIS_COMMIT="${DRONE_COMMIT}"
export TRAVIS_BUILD_DIR="${DRONE_BUILD_DIR}"

for group in static-tests \
             avr8         \
             msp430       \
             x86          \
             arm7         \
             cortex_m0    \
             cortex_m3_1  \
             cortex_m3_2  \
             cortex_m4    \
    ;
do
    echo "Begin group ${group}"
    BUILDTEST_MCU_GROUP=${group} ./dist/tools/travis-scripts/build_and_test.sh "$@"
    RES=$?
    echo "Result (${group}): ${RES}"
    if [ ${RES} -ne 0 ]; then
        FAILURES=$((${FAILURES} + 1))
    fi
    SUMMARY_MESSAGE="${SUMMARY_MESSAGE}${group}: ${RES}\n"
done

echo -n -e "${SUMMARY_MESSAGE}"

if [ ${FAILURES} -eq 0 ]; then
    echo "Build and test: All successful"
else
    echo "Build and test: ${FAILURES} failures in total"
fi

exit ${FAILURES}
