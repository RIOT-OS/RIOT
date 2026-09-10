#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
# SPDX-FileCopyrightText: 2026 Karl Fessel
# SPDX-License-Identifier: LGPL-2.1-only

SCRIPTREALDIR=$(realpath "$(dirname "${0}")")

: "${RIOTBASE:=$(cd "$(dirname "$0")"/../../../ || exit 1; pwd)}"
cd "$RIOTBASE" || exit 1

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
# shellcheck source=/dev/null
. "${RIOTTOOLS}"/ci/changed_files.sh

# customizable
CHECKROOT="${SCRIPTREALDIR}"
LICENSEDIR="${CHECKROOT}/patterns"
OUTPUT="${CHECKROOT}/out"
UNKNOWN="${OUTPUT}/unknown"
TMP="${CHECKROOT}/.tmp"

# Needed for compatibility with BSD sed
TAB_CHAR="$(printf '\t')"

# prepare
ROOT=$(git rev-parse --show-toplevel)
# put the SPDX patterns first to speed up the loop below
LICENSES=$(find "${LICENSEDIR}" -type f -iname 'spdx*' -printf '%f ')
LICENSES=${LICENSES}$(find "${LICENSEDIR}" -type f ! -iname 'spdx*' -printf '%f ')

EXIT_CODE=0

: "${ERROR_EXIT_CODE:=1}"

# reset output dir
rm -fr "${OUTPUT}"
mkdir -p "${OUTPUT}"

# prepare license patterns
declare -A PATTERNS
for LICENSE in ${LICENSES}; do
    echo -n '' > "${OUTPUT}/${LICENSE}"
    PATTERNS[${LICENSE}]="$(grep -v '^$' "${LICENSEDIR}/${LICENSE}" | \
        paste -sd' ' | sed -e 's/[[:space:]][[:space:]]*/ /g')"
done

FILES=$(FILEREGEX='\.([sSch]|cpp)$' changed_files)

# categorize files
for FILE in ${FILES}; do
    FAIL=1
    head -100 "${ROOT}/${FILE}" | sed -e 's/[\/\*'"${TAB_CHAR}"']/ /g' | \
        paste -sd' ' | sed -e 's/[[:space:]][[:space:]]*/ /g' > "${TMP}"
    for LICENSE in ${LICENSES}; do
        if grep -qE "${PATTERNS[${LICENSE}]}" "${TMP}"; then
            echo "${FILE}" >> "${OUTPUT}/${LICENSE}"
            FAIL=0
            break
        fi
    done
    if [ ${FAIL} = 1 ]; then
        echo "${FILE}" >> "${UNKNOWN}"
        echo "file has an unknown license header: '${FILE}'"
        EXIT_CODE=${ERROR_EXIT_CODE}
    fi
done

exit "${EXIT_CODE}"
