#!/bin/bash

TOOLROOT=${TOOLROOT:-.}

flash() {
    echo "Building ${1}..."
    jam -aq flash || ( echo "[BUILD FAILED] ${1}" && false )
}

run_tests() {
    TESTDIR=projects/${1}/tests
    flash ${APPLICATION} || return
    for tst in `ls ${TESTDIR}/`; do
        echo "Project \"${1}\": Running test ${tst}..."
        $TESTDIR/$tst || (
            echo
            echo "[TEST FAILED] ${TESTDIR}/${tst}"
            ) || echo "[TEST SUCCESSFUL] ${TESTDIR}/${tst}"
    done
}

echo
echo "Running tests..."
echo

for i in projects/*; do
    export APPLICATION=`basename $i`
    if [ -d projects/${APPLICATION}/tests ]; then
        {
            echo "Testing project ${APPLICATION}..."
            PORT="`sh ${TOOLROOT}/tools/lock_board.sh`"
            FLASHUTIL_SHELL="sh -c"

            echo "Using Target connecting to ${PORT}."

            export PORT FLASHUTIL_SHELL
            run_tests ${APPLICATION}

            sh ${TOOLROOT}/tools/unlock_board.sh ${PORT}
        } 2>&1
    fi
done
