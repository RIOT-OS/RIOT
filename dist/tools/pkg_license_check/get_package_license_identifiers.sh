#!/bin/bash

# SPDX-FileCopyrightText: Copyright (C) 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: LGPL-2.1-only

# Print list of unique license identifiers, as declared by package Makefiles.

set -euo pipefail

get_variable_value() {
    # Get the text following the =, then strip leading white spaces and following
    # white spaces and comment. Not strictly how make does it but close enough.
    cut -d = -f2 | \
        sed -e 's/^[ \t]*//' -e 's/\([ \t]*#.*\)?[ \t]*$//' -e '/^$/d'
}

if [[ $# -eq 0 ]]
then
    # Unfortunately, packages exist in a few places within RIOT, so we first want to grep
    # for places where licenses are set. This should happen in Makefile files only.
    RIOT_BASE="$(dirname "$(readlink -f "$0")")/../../../"
    readarray -d '' MAKEFILES < <(find  "$RIOT_BASE" -name Makefile -print0)
    FILES=()
    for fil in "${MAKEFILES[@]}"
    do
        if [[ "$( grep -hcE '^ *PKG_LICENSE *[:\?]?=' "${fil}" )" -gt 0 ]]
        then
            FILES[${#FILES[@]}]="${fil}"
        fi
    done
else
    FILES=("$@")
fi

for pkg_makefile in "${FILES[@]}"
do
    LICENCE=$(grep -E '^ *PKG_LICENSE *[:\?]?=' -- "${pkg_makefile}" | get_variable_value)

    NUM_LICENSE=$(echo "${LICENCE}" | wc -l)
    if [[ "${NUM_LICENSE}" -ne 1 ]]
    then
        echo "error: \"PKG_LICENSE\" must be defined on a single line for a package." >&2
        echo "error: Found ${NUM_LICENSE} declarations in \"${pkg_makefile}\"." >&2
        exit 1
    fi

    echo -e "$(basename "$(dirname "${pkg_makefile}")")\t${LICENCE}"
done
