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

# reset output dir
rm -fr "${OUTPUT}"
mkdir -p "${OUTPUT}"
for LICENSE in ${LICENSES}; do
    echo -n '' > "${OUTPUT}/${LICENSE}"
done

# categorize files
for FILE in $(git ls-tree -r --full-tree --name-only HEAD | grep -E '\.[sSc]$'); do
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
        EXIT_CODE=1
    fi
done

exit ${EXIT_CODE}
