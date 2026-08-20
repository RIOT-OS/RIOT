# shellcheck shell=bash # this script is only sourced, so no shebang

# SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2014 Ludwig Knüpfer <ludwig.knuepfer@fu-berlin.de>
# SPDX-FileCopyrightText: 2014 Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
# SPDX-License-Identifier: LGPL-2.1-only

changed_files() {
    : "${FILEREGEX:='\.([CcHh]|[ch]pp)$'}"
    : "${EXCLUDE:='^(.+/vendor/|dist/tools/coccinelle/include|dist/tools/fixdep/fixdep.c|dist/tools/lpc2k_pgm/src)'}"
    : "${DIFFFILTER:=ACMR}"

    DIFFFILTER="--diff-filter=${DIFFFILTER}"

    # select either all or only touched-in-branch files, filter through FILEREGEX
    if [ -z "${BASE_BRANCH}" ]; then
        FILES="$(git ls-tree -r --full-tree --name-only HEAD | grep -E "${FILEREGEX}")"
    else
        FILES="$(git diff "${DIFFFILTER}" --name-only "${BASE_BRANCH}" | grep -E "${FILEREGEX}")"
    fi

    # filter out negatives
    echo "${FILES}" | grep -v -E "${EXCLUDE}"
}
