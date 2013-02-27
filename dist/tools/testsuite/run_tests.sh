#!/bin/bash

TOOLROOT=${TOOLROOT:-.}

flash() {
    echo "Building ${1}..."
    jam -aq flash || ( echo "[BUILD FAILED] ${1}" && false ) 
}

run_tests() {
    TESTDIR=projects/${1}/tests
    flash ${PROJECT} || return
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
    export PROJECT=`basename $i`
    if [ -d projects/${PROJECT}/tests ]; then
        {
            echo "Testing project ${PROJECT}..."
            PORT="`sh ${TOOLROOT}/tools/lock_board.sh`"
            FLASHUTIL_SHELL="sh -c"

            echo "Using Target connecting to ${PORT}."

            export PORT FLASHUTIL_SHELL
            run_tests ${PROJECT}

            sh ${TOOLROOT}/tools/unlock_board.sh ${PORT}
        } 2>&1
    fi
done

