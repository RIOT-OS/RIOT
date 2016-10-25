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
    which $1 2>&1 1>/dev/null
    if (( $? != 0 )); then
        echo "Dependency not met: $1"
        exit 1
    fi
}

function abort {
    echo "$(tput setaf 1)$1$(tput sgr0)"
    exit 1
}

function request_confirmation {
    read -p "$(tput setaf 4)$1 (y/n) $(tput sgr0)"
    [ "$REPLY" == "y" ] || abort "Aborted!"
}

# Make sure all required commands are available
dep cppcheck
dep pcregrep

RIOT_REMOTE_COUNT="$(git remote | grep "^riot$" | wc -l)"
if (( "$RIOT_REMOTE_COUNT" != 1 )); then
    echo "The static test setup expect a remote called 'riot', pointing to the"
    echo "central repository. This remote currently does not exist."
    request_confirmation "Do you wish to create it?"

    git remote add riot https://github.com/RIOT-OS/RIOT.git
    git fetch riot
fi

BUILDTEST_MCU_GROUP=static-tests ./dist/tools/travis-scripts/build_and_test.sh
