#!/bin/bash

test -f ~/.buildbot && . ~/.buildbot

export REVISION=${1}
export ROOT=${ROOT:-.}

${ROOT}/testsuite/svn_test_revision.sh ${REVISION}
${ROOT}/testsuite/generate_html.sh

