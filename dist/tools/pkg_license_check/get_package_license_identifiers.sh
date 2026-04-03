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
    readarray -d '' MAKEFILES < <(find  "$(dirname "$(readlink -f "$0")")/../../../" -name Makefile -print0)
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
    LEGAL_STATUS=$(grep -E '^ *PKG_LEGAL_STATUS *[:\?]?=' -- "${pkg_makefile}" | get_variable_value)
    LICENCE=$(grep -E '^ *PKG_LICENSE *[:\?]?=' -- "${pkg_makefile}" | get_variable_value)

    NUM_LEGAL_STATUS=$(echo "${LEGAL_STATUS}" | wc -l)
    if [[ "${NUM_LEGAL_STATUS}" -ne 1 ]]
    then
        echo "error: \"PKG_LEGAL_STATUS\" must be defined on a single line for a package. Found ${NUM_LEGAL_STATUS} declarations in \"${pkg_makefile}\"." >&2
        exit 1
    fi

    case ${LEGAL_STATUS} in
        public-domain | licensed)
            ;;
        *)
            echo "error: unknown PKG_LEGAL_STATUS=${LEGAL_STATUS} in \"${pkg_makefile}\"." >&2
            exit 1
            ;;
    esac

    NUM_LICENSE=$(echo "${LICENCE}" | wc -l)
    if [[ "${NUM_LICENSE}" -ne 1 ]]
    then
        echo "error: \"PKG_LICENSE\" must be defined on a single line for a package. Found ${NUM_LICENSE} declarations in \"${pkg_makefile}\"." >&2
        exit 1
    fi

    echo -e "$(basename "$(dirname "${pkg_makefile}")")\t${LEGAL_STATUS}\t${LICENCE}"
done
