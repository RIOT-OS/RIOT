#!/bin/bash
#
# Copyright (C) 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

set -e

if [[ $BUILDTEST_MCU_GROUP ]]
then
    if [ "$BUILDTEST_MCU_GROUP" == "static-tests" ]
    then
        make -s -C ./examples/default info-concurrency
        git rebase riot/master || git rebase --abort

        ./dist/tools/whitespacecheck/check.sh master

        ./dist/tools/licenses/check.sh master --diff-filter=MR --error-exitcode=0
        ./dist/tools/licenses/check.sh master --diff-filter=AC

        ./dist/tools/doccheck/check.sh master

        ./dist/tools/externc/check.sh master

        # TODO:
        #   Remove all but `master` parameters to cppcheck (and remove second
        #   invocation) once all warnings of cppcheck have been taken care of
        #   in master.
        ./dist/tools/cppcheck/check.sh master --diff-filter=MR --error-exitcode=0
        ./dist/tools/cppcheck/check.sh master --diff-filter=AC
        ./dist/tools/pr_check/pr_check.sh riot/master
        exit 0
    fi
    if [ "$BUILDTEST_MCU_GROUP" == "x86" ]
    then

        make -C ./tests/unittests all test BOARD=native || exit
        # TODO:
        #   Reenable once https://github.com/RIOT-OS/RIOT/issues/2300 is
        #   resolved:
        #   - make -C ./tests/unittests all test BOARD=qemu-i386 || exit
    fi
    ./dist/tools/compile_test/compile_test.py
fi
