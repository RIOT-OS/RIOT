#!/usr/bin/env bash
#
# Copyright (C) 2015 Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

CI_BASE_BRANCH=${CI_BASE_BRANCH:-master}
if [ -z "${JSON_OUTPUT}" ]; then
    export JSON_OUTPUT=0
fi

function print_result {
    if [ ${JSON_OUTPUT} -ne 0 ]; then
        echo -en "\"status\":$1,"
    else
        local RED="\033[0;31m"
        local GREEN="\033[0;32m"
        local NO_COLOUR="\033[0m"

        if (( "$1" == 0 )); then
            echo -e "${GREEN}✓$NO_COLOUR"
        else
            echo -e "${RED}x$NO_COLOUR"
        fi
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
    if [ ${JSON_OUTPUT} -ne 0 ]; then
        echo -ne "{\"command\":\"$@\","
    else
        echo -n "Running '$@' "
    fi
    OUT=$($@ 2>&1)
    NEW_RESULT=$?

    print_result $NEW_RESULT
    set_result $NEW_RESULT

    # Indent command output so that its easily discernable from the rest
    if [ -n "$OUT" ]; then
        if [ ${JSON_OUTPUT} -ne 0 ]; then
            echo -ne '"output":"'
            # Using printf to avoid problems if the command output begins with a -
            (printf "%s\n" "$OUT" | while IFS= read -r line; do printf '%s\\n' "$line" | sed 's/"/\\"/g'; done)
            echo -ne '"'
        else
            echo "Command output:"
            echo ""
            # Using printf to avoid problems if the command output begins with a -
            (printf "%s\n" "$OUT" | while IFS= read -r line; do printf "\t%s\n" "$line"; done)
            echo ""
        fi
    else if [ ${JSON_OUTPUT} -ne 0 ]; then
        echo -ne '"output":""'
    fi
    fi
    if [ ${JSON_OUTPUT} -ne 0 ]; then
        echo -ne "}"
    fi
}

if [[ $BUILDTEST_MCU_GROUP ]]
then

    if [ "$BUILDTEST_MCU_GROUP" == "static-tests" ]
    then
        RESULT=0
        RECALL="$1"

        if [ "$RECALL" != "recall" ]; then
            if git diff ${CI_BASE_BRANCH} HEAD -- "$0" &> /dev/null 2>&1; then
                git rebase ${CI_BASE_BRANCH} > /dev/null || git rebase --abort > /dev/null 2>&1
                "$0" "recall"
                exit $?
            fi
        fi

        trap "RESULT=1" ERR

        git rebase ${CI_BASE_BRANCH} > /dev/null 2>&1
        if (( $? != 0 )); then
            git rebase --abort > /dev/null 2>&1
            echo "Rebase failed, aborting..."
            exit 1
        fi

        if [ $RESULT -ne 0 ]; then
            exit $RESULT
        fi

        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne "["
        run ./dist/tools/ci/print_toolchain_versions.sh
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","

        run ./dist/tools/whitespacecheck/check.sh ${CI_BASE_BRANCH}
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","
        run ./dist/tools/licenses/check.sh ${CI_BASE_BRANCH} --diff-filter=MR --error-exitcode=0
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","
        run ./dist/tools/licenses/check.sh ${CI_BASE_BRANCH} --diff-filter=AC
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","
        run ./dist/tools/doccheck/check.sh ${CI_BASE_BRANCH}
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","
        run ./dist/tools/externc/check.sh ${CI_BASE_BRANCH}
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","

        # TODO:
        #   Remove all but `${CI_BASE_BRANCH}` parameters to cppcheck (and remove second
        #   invocation) once all warnings of cppcheck have been taken care of
        #   in ${CI_BASE_BRANCH}.
        run ./dist/tools/cppcheck/check.sh ${CI_BASE_BRANCH} --diff-filter=MR --error-exitcode=0
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","
        run ./dist/tools/cppcheck/check.sh ${CI_BASE_BRANCH} --diff-filter=AC
        [ ${JSON_OUTPUT} -ne 0 ] && echo -ne ","
        run ./dist/tools/pr_check/pr_check.sh ${CI_BASE_BRANCH}
        [ ${JSON_OUTPUT} -ne 0 ] && echo "]"
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
        # TODO:
        #   Reenable once https://github.com/RIOT-OS/RIOT/issues/2300 is
        #   resolved:
        #   - make -C ./tests/unittests all test BOARD=qemu-i386 || exit
    fi

    BASE_BRANCH="${CI_BASE_BRANCH}"
    ./dist/tools/compile_test/compile_test.py $BASE_BRANCH
    set_result $?
fi

exit $RESULT
