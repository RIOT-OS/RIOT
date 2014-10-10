#!/bin/sh

# customizable
CHECKROOT=$(dirname "${0}")

# prepare
ROOT=$(git rev-parse --show-toplevel)
EXIT_CODE=0
BRANCH="${1}"
DIFFFILTER="${2}"

# set default diff-filter
if [ -z "${DIFFFILTER}" ]; then
    DIFFFILTER="ACMR"
fi

# select files to check
if [ -z "${BRANCH}" ]; then
    FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E '\.h$')"
else
    FILES="$(git diff --diff-filter=${DIFFFILTER} --name-only ${BRANCH} | grep -E '\.h$')"
fi

# check files
for FILE in ${FILES}; do
    if head -100 "${ROOT}/${FILE}" \
        | sed -e 's/$/ /' \
        | tr -d '\r\n' \
        | sed -e 's/  */ /g' \
        | grep -v -q '#ifdef __cplusplus extern "C" { #endif'; \
    then
        EXIT_CODE=1
        echo "file does not have a C++ compatible header: '${FILE}'"
    fi
done

exit ${EXIT_CODE}
