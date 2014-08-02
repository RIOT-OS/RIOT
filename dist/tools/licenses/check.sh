#!/bin/sh

# customizable
CHECKROOT=$(dirname "${0}")
LICENSEDIR="${CHECKROOT}/patterns"
OUTPUT="${CHECKROOT}/out"
UNKNOWN="${OUTPUT}/unknown"
TMP="${CHECKROOT}/.tmp"

# prepare
ROOT=$(git rev-parse --show-toplevel)
LICENSES=$(ls "${LICENSEDIR}")
EXIT_CODE=0
BRANCH="${1}"

# reset output dir
rm -fr "${OUTPUT}"
mkdir -p "${OUTPUT}"
for LICENSE in ${LICENSES}; do
    echo -n '' > "${OUTPUT}/${LICENSE}"
done

# select files to check
if [ -z "${BRANCH}" ]; then
    FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E '\.[sSch]$')"
else
    FILES="$(git diff --diff-filter=ACMR --name-only ${BRANCH} | grep -E '\.[sSch]$')"
fi

# categorize files
for FILE in ${FILES}; do
    FAIL=1
    head -100 "${ROOT}/${FILE}" | sed -e 's/[\/\*\t]/ /g' -e 's/$/ /' | tr -d '\r\n' | sed -e 's/  */ /g' > "${TMP}"
    for LICENSE in ${LICENSES}; do
        if pcregrep -q -f "${LICENSEDIR}/${LICENSE}" "${TMP}"; then
            echo "${FILE}" >> "${OUTPUT}/${LICENSE}"
            FAIL=0
            break
        fi
    done
    if [ ${FAIL} = 1 ]; then
        echo "${FILE}" >> "${UNKNOWN}"
        echo "file has an unknown license header: '${FILE}'"
        EXIT_CODE=1
    fi
done

exit ${EXIT_CODE}
