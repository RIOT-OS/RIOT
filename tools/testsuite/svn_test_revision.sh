#!/bin/bash

. ~/.buildbot

REPO=${REPO:-https://svn.mi.fu-berlin.de/msb/FeuerWare/trunk}
LOGDIR=${LOGDIR:-${HOME}/buildlogs}

REVISION=${1:-HEAD}

LOGBASE="`date +%F-%Hh-%Mm-%Ss`-r${REVISION}-test.log"

LOGFILE="${LOGDIR}/${LOGBASE}"
TMPDIR=`mktemp -d /dev/shm/svn_test_repo.XXXXXXXXXX`

TOOLROOT=${TOOLROOT:-.}

if [ ! -d ${TMPDIR} ]; then exit 1; fi

touch ${LOGFILE}.lock

{
    echo "${0} runnning checkout/build/test cycle."
    echo "Repo: ${REPO}"
    echo "Revision: ${REVISION}" 
    echo "Temporary directory: ${TMPDIR}..."
    echo "Logfilename: ${LOGBASE}"

    cd ${TMPDIR} &&

    echo "Checking out..." &&
    svn co -q -r ${REVISION} ${REPO} &&
    echo "Done." &&

    cd ${TMPDIR}/trunk/board/msba2/tools && make &&
    cd ${TMPDIR}/trunk &&
    bash ${TOOLROOT}/tools/testsuite/run_tests.sh

    echo
    echo "Test run completed."
} 2>&1 | tee ${LOGFILE}

rm -f ${LOGFILE}.lock
rm -f ${LOGFILE}.parsed

rm -rf ${TMPDIR}

