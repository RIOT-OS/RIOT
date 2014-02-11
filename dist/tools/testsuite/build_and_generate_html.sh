#!/bin/bash

test -f ~/.buildbot && . ~/.buildbot

export REVISION=${1}
export ROOT=${ROOT:-.}

${ROOT}/tools/testsuite/svn_test_revision.sh ${REVISION}
${ROOT}/tools/testsuite/generate_html.sh
