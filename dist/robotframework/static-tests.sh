#!/bin/sh

# Copyright 2018 Sebastian Meiling <s@mlng.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

: "${RIOTBASE:=$(cd $(dirname $0)/../../; pwd)}"
: "${RFBASE:=${RIOTBASE}/dist/robotframework/}"

export RIOTBASE

[ -z "$RFOUTPUT" ] && {
    RFOUTPUT=${RIOTBASE}/build/robot/static-tests/
    echo "RFOUTPUT not set, default to \"${RFOUTPUT}\"."
}

[ -z "$BASE_BRANCH" ] && {
    BASE_BRANCH="origin/master"
    echo "BASE_BRANCH not set, default to \"${BASE_BRANCH}\"."
}

export BASE_BRANCH

cd ${RFBASE}

python3 -m robot.run --noncritical warn-if-failed -P "${RFBASE}/lib:${RFBASE}/res" -d ${RFOUTPUT} static-tests.robot
