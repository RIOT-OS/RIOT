#!/usr/bin/env bash

LOG=${1}

if [ "xx${PARSELOG_ALWAYS}xx"=!"xxyxx" ]; then
    if [ -f ${LOG}.parsed ]; then
        cat ${LOG}.parsed
        exit 0
    fi
fi

{
#BUILD_FAIL=`grep '\[BUILD FAILED\]' ${LOG} | wc -l`
NUM_FAIL=`grep '\[.* FAILED\]' ${LOG} | wc -l`
NUM_OK=`grep '\[TEST SUCCESSFUL\]' ${LOG} | wc -l`

REVISION=`awk '/^Revision:/ { print $2; exit}' $LOG`
REPO=`awk '/^Repo:/ { print $2; exit}' $LOG`


if [ -f ${LOG}.lock ]; then
    BUILD_STATUS="build in progress..."
elif (($NUM_FAIL==0)); then
    BUILD_STATUS="OK"
else
    BUILD_STATUS="broken!"
fi

echo "<h2>Revision: ${REVISION} Status: ${BUILD_STATUS}</h2>"

svn log --incremental -v -l 1 ${REPO}@${REVISION} | sed -e 's/$/<br>/'

echo "------------------------------------------------------------------------<br>"

if (($NUM_FAIL==0)); then
    true
else
echo "<br> Failed builds/tests: <br>"
    grep '\[.* FAILED\]' ${LOG} | uniq | sed -e 's/$/<br>/'
fi
} | tee ${1}.parsed
