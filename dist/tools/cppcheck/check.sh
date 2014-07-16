#!/bin/sh

BRANCH=${1}
FILEREGEX='\.([sScHh]|cpp)$'

if echo "${BRANCH}" | grep -q '^-'; then
    BRANCH=""
else
    shift 1
fi

if [ -z "${BRANCH}" ]; then
    FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E ${FILEREGEX})"
else
    FILES="$(git diff --diff-filter=ACMR --name-only ${BRANCH} | grep -E ${FILEREGEX})"
fi

if [ -z "${FILES}" ]; then
    exit
fi

cppcheck --std=c99 --enable=style --force \
    --error-exitcode=2 --inline-suppr --quiet -j 8 \
    --template "{file}({line}): {severity} ({id}): {message}" \
    ${@} ${FILES}
