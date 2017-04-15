#!/bin/bash
#
# Copyright (C) 2015 Lucas Jenß <lucas@x3ro.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

# Change to RIOT root
cd "$(dirname "$0")/../../"

function dep {
    command -v $1 2>&1 1>/dev/null
    if (( $? != 0 )); then
        echo "Dependency not met: $1"
        exit 1
    fi
}

# Make sure all required commands are available
dep cppcheck
dep pcregrep

BUILDTEST_MCU_GROUP=static-tests ./dist/tools/ci/build_and_test.sh
