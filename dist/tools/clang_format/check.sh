#!/usr/bin/env bash

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

CF="$(command -v clang-format)"

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
    DIFFHUNK="$3"

    # for pure insertions the hunk starts at line 0, which is not a valid
    # annotation target
    if [ "${DIFFLINE}" -eq 0 ]; then
        DIFFLINE=1
    fi
    github_annotate_warning "${DIFFFILE}" "${DIFFLINE}" \
        "clang-format proposes the following patch:

${DIFFHUNK}"
}

EXIT_CODE=0

# shellcheck disable=SC2086
# FILES is supposed to be split, so don't quote it
for F in ${FILES}; do
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
            # the pipe spawns a subshell, so the final annotation is also
            # within it (same pattern as uncrustify.sh uses)
            diff -u "${RIOTBASE}/${F}" "${FORMATTED}" | {
                DIFFLINE=
                DIFFHUNK=
                while read -r LINE; do
                    case "${LINE}" in
                        "@@"*)
                            if [ -n "${DIFFLINE}" ]; then
                                _annotate_hunk "${F}" "${DIFFLINE}" \
                                               "${DIFFHUNK}"
                            fi
                            DIFFLINE="$(printf '%s\n' "${LINE}" | \
                                       sed -e 's/^@@ -\([0-9]*\).*/\1/')"
                            DIFFHUNK="${LINE}"
                            ;;
                        *)
                            if [ -n "${DIFFLINE}" ]; then
                                DIFFHUNK="${DIFFHUNK}
${LINE}"
                            fi
                            ;;
                    esac
                done
                if [ -n "${DIFFLINE}" ]; then
                    _annotate_hunk "${F}" "${DIFFLINE}" "${DIFFHUNK}"
                fi
            }
        fi
    fi
done

github_annotate_teardown

exit "${EXIT_CODE}"
