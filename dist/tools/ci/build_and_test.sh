#!/usr/bin/env bash
#
# Copyright (C) 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

CI_BASE_BRANCH=${CI_BASE_BRANCH:-master}

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

RESULT=0
set_result() {
    NEW_RESULT=$1

    if (( $NEW_RESULT != 0))
    then
        RESULT=$NEW_RESULT
    fi
}

function run {
    echo -n "Running '$@' "
    OUT=$($@ 2>&1)
    NEW_RESULT=$?

    print_result $NEW_RESULT
    set_result $NEW_RESULT

    # Indent command output so that its easily discernable from the rest
    if [ -n "$OUT" ]; then
        echo "Command output:"
        echo ""
        # Using printf to avoid problems if the command output begins with a -
        (printf "%s\n" "$OUT" | while IFS= read -r line; do printf "\t%s\n" "$line"; done)
        echo ""
    fi
}

if [[ $BUILDTEST_MCU_GROUP ]]
then
    export BASE_BRANCH="${CI_BASE_BRANCH}"

    if [ "$BUILDTEST_MCU_GROUP" == "static-tests" ]
    then
        RESULT=0
        RECALL="$1"

        if [ "$RECALL" != "recall" ]; then
            if git diff ${CI_BASE_BRANCH} HEAD -- "$0" &> /dev/null; then
                git rebase ${CI_BASE_BRANCH} || git rebase --abort

                "$0" "recall"
                exit $?
            fi
        fi

        trap "RESULT=1" ERR

        git rebase ${CI_BASE_BRANCH}
        if (( $? != 0 )); then
            git rebase --abort > /dev/null 2>&1
            echo "Rebase failed, aborting..."
            exit 1
        fi

        if [ $RESULT -ne 0 ]; then
            exit $RESULT
        fi

        run ./dist/tools/ci/print_toolchain_versions.sh

        run ./dist/tools/commit-msg/check.sh ${CI_BASE_BRANCH}
        run ./dist/tools/whitespacecheck/check.sh ${CI_BASE_BRANCH}
        DIFFFILTER="MR" ERROR_EXIT_CODE=0 run ./dist/tools/licenses/check.sh
        DIFFFILTER="AC" run ./dist/tools/licenses/check.sh
        run ./dist/tools/doccheck/check.sh
        run ./dist/tools/externc/check.sh
        run ./dist/tools/cppcheck/check.sh
        run ./dist/tools/pr_check/pr_check.sh ${CI_BASE_BRANCH}
        run ./dist/tools/coccinelle/check.sh
        run ./dist/tools/flake8/check.sh
        run ./dist/tools/headerguards/check.sh
        exit $RESULT
    fi

    if [ "$BUILDTEST_MCU_GROUP" == "host" ]; then
        make -C dist/tools
        exit $?
    fi

    if [ "$BUILDTEST_MCU_GROUP" == "x86" ]
    then
        make -C ./tests/unittests all-debug test BOARD=native TERMPROG='gdb -batch -ex r -ex bt $(ELF)' || exit
        set_result $?
    fi


    ./dist/tools/compile_test/compile_test.py $BASE_BRANCH
    set_result $?
fi

exit $RESULT
