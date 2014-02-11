#!/bin/bash

ROOT=${ROOT:-.}
TOOLROOT=${TOOLROOT:-${ROOT}}
HTMLDIR=${TOOLROOT}/tools/testsuite/html

test -f ~/.buildbot && . ~/.buildbot

OUTFILE=${HTMLFILE:-${HOME}/riot.html}
LOGDIR=${BUILDLOGS:-${HOME}/buildlogs}

{
cat ${HTMLDIR}/header

for i in `ls -t $LOGDIR/*.log`; do
    ${TOOLROOT}/tools/testsuite/parse_buildlog.sh $i
    if [ "x${HTTPROOT}x"!="xx" ]; then
        echo "<br>"
        echo "Full build log: <a href=\"${HTTPROOT}/`basename ${i}`\"> click here </a>."
    fi

done

cat ${HTMLDIR}/footer
} > ${OUTFILE}
