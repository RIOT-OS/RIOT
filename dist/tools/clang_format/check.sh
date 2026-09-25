#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2026 AnnsAnn <git@annsann.eu>
# SPDX-FileCopyrightText: 2026 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

# Check the formatting of all changed C/C++ files with clang-format and
# annotate the proposed patches in GitHub (if running as a GitHub action).
#
# Requires clang-format >= 17. If clang-format is not available or too old,
# the check is skipped with a warning

RIOTBASE="$(git rev-parse --show-toplevel)"

# shellcheck source=dist/tools/ci/changed_files.sh
. "${RIOTBASE}"/dist/tools/ci/changed_files.sh
# shellcheck source=dist/tools/ci/github_annotate.sh
. "${RIOTBASE}"/dist/tools/ci/github_annotate.sh

CF="$(compgen -c clang-format 2>/dev/null | sort -t- -k3 -n -r | head -n1)"
CF="$(command -v "${CF}" 2>/dev/null)"

if [ -z "${CF}" ]; then
    echo "clang-format is not available, skipping check."
    exit 0
fi

CF_MAJOR="$("${CF}" --version | sed -e 's/^[^0-9]*\([0-9]*\).*$/\1/')"

if [ "${CF_MAJOR}" -lt 17 ]; then
    echo "clang-format >= 17 required (found $("${CF}" --version)),"
    echo "skipping check."
    exit 0
fi

FILES="$(changed_files)"

if [ -z "${FILES}" ]; then
    exit 0
fi

github_annotate_setup

FORMATTED="$(mktemp)"
trap 'rm -f "${FORMATTED}"' EXIT

_annotate_hunk() {
    DIFFFILE="$1"
    DIFFLINE="$2"
    DIFFLEN="$3"
    DIFFHUNK="$4"
    DIFFENDL=$((DIFFLINE+DIFFLEN))
    ANOTATION_MESSAGE="${DIFFHUNK}"
    # for pure insertions the hunk starts at line 0, which is not a valid
    # annotation target
    if [ "${DIFFLINE}" -eq 0 ]; then
        DIFFLINE=1
    fi
    github_annotate "$ANOTATION_MESSAGE" warning \
    "file=${DIFFFILE},line=${DIFFLINE},endLine=${DIFFENDL},title=clang-format proposes\
the following patch"
}

EXIT_CODE=0

# create an array from the changed files list
mapfile -t FILES_ARR <<< "${FILES}"

for F in "${FILES_ARR[@]}"; do
    if ! "${CF}" --style=file "${RIOTBASE}/${F}" > "${FORMATTED}" 2>/dev/null; then
        echo "File ${F} could not be parsed by clang-format."
        EXIT_CODE="${ERROR_EXIT_CODE:-1}"
        if github_annotate_is_on; then
            github_annotate_error "${F}" 1 \
                "File could not be parsed by clang-format."
        fi
    elif ! cmp -s "${RIOTBASE}/${F}" "${FORMATTED}"; then
        echo "File ${F} needs to be clang-formatted."
        EXIT_CODE="${ERROR_EXIT_CODE:-1}"
        if github_annotate_is_on && [ -s "${FORMATTED}" ]; then
            DIFFLINE=
            DIFFHUNK=
            DIFFLEN=
            while IFS= read -r LINE; do
                case "${LINE}" in
                    @@*)
                        if [ -n "${DIFFLINE}" ]; then
                            _annotate_hunk "${F}" "${DIFFLINE}" "${DIFFLEN}" "${DIFFHUNK}"
                        fi
                        DIFFLINE="${LINE#@@ -}"
                        DIFFLINE="${DIFFLINE%%[!0-9]*}"
                        DIFFLEN="${LINE#@@ -*,}"
                        DIFFLEN="${DIFFLEN%%[!0-9]*}"
                        DIFFHUNK="${LINE}"
                        ;;
                    *)
                        if [ -n "${DIFFLINE}" ]; then
                            DIFFHUNK+=$'\n'"${LINE}"
                        fi
                        ;;
                esac
            done < <(diff -u "${RIOTBASE}/${F}" "${FORMATTED}")
            if [ -n "${DIFFLINE}" ]; then
                _annotate_hunk "${F}" "${DIFFLINE}" "${DIFFLEN}" "${DIFFHUNK}"
            fi
        fi
    fi
done

github_annotate_teardown

exit "${EXIT_CODE}"
