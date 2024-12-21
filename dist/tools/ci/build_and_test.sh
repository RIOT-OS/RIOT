#!/usr/bin/env bash
#
# Copyright (C) 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

CI_BASE_BRANCH=${CI_BASE_BRANCH:-master}

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

        exec ./dist/tools/ci/static_tests.sh
    fi

    if [ "$BUILDTEST_MCU_GROUP" == "host" ]; then
        make -C dist/tools
        exit $?
    fi

    if [ "$BUILDTEST_MCU_GROUP" == "x86" ]
    then
        make -C ./tests/unittests all test BOARD=native32 TERMPROG='gdb -batch -ex r -ex bt $(ELF)' || exit
        set_result $?
    fi

    if [ "$BUILDTEST_MCU_GROUP" == "x86_64" ]
    then
        make -C ./tests/unittests all test BOARD=native TERMPROG='gdb -batch -ex r -ex bt $(ELF)' || exit
        set_result $?
    fi

    ./dist/tools/compile_test/compile_test.py $BASE_BRANCH
    set_result $?
fi

exit $RESULT
