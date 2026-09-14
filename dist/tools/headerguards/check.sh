#!/usr/bin/env bash

# SPDX-FileCopyrightText: 2017 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

: "${RIOTBASE:=$(cd "$(dirname "$0")/../../../" || exit 1; pwd)}"
cd "$RIOTBASE" || exit 1

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"

# shellcheck source=dist/tools/ci/changed_files.sh
. "${RIOTTOOLS}"/ci/changed_files.sh
# shellcheck source=dist/tools/ci/github_annotate.sh
. "${RIOTTOOLS}"/ci/github_annotate.sh

EXIT_CODE=0

filter() {
    if [ "$QUIET" -eq 0 ]; then
        cat
    else
        grep '^---' | cut -f 2 -d ' '
    fi
}

_annotate_diff() {
    if [ -n "$1" ] && [ -n "$2" ] && [ -n "$3" ]; then
        IFS="${OLD_IFS}" github_annotate_error "$1" "$2" "Wrong header guard format:\n\n$3"
    fi
}

_headercheck() {
    if [ "${#FILES_ARR[@]}" -eq 0 ] || [ -z "${FILES_ARR[0]}" ]; then
        return 0
    fi

    OUT=$("${RIOTTOOLS}"/headerguards/headerguards.py "${FILES_ARR[@]}" 2>&1 | filter)
    if [ -n "$OUT" ]; then
        EXIT_CODE=1
        if github_annotate_is_on; then
            DIFF=""
            DIFFFILE=""
            DIFFLINE=""
            echo "$OUT" | {
                # see https://stackoverflow.com/a/30064493/11921757 for why we
                # use a sub shell here
                OLD_IFS="$IFS"      # store old separator to later restore it
                IFS=''  # keep leading and trailing spaces
                while read -r line; do
                    # file has no or broken header guard
                    if [[ "$line" == *": no / broken header guard" ]]; then
                        # this output comes outside of a diff, so reset diff parser
                        _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                        DIFF=""
                        DIFFFILE=""
                        DIFFLINE=""
                        # annotate broken header guard
                        FILE=$(echo "$line" | cut -d: -f1 | xargs echo)
                        MESSAGE=$(echo "$line" | cut -d: -f2 | xargs echo)
                        github_annotate_error "$FILE" 0 "$MESSAGE"
                    # parse beginning of new diff
                    elif [[ "$line" == "--- "* ]]; then
                        _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                        DIFF="$line"
                        DIFFFILE="${line#--- }"
                        DIFFLINE=""
                    # we are in a diff currently
                    elif [ -n "$DIFF" ]; then
                        # grep first line number of diff
                        if echo "$line" | \
                           grep -q "@@ -[0-9]\+\(,[0-9]\+\)\? +[0-9]\+\(,[0-9]\+\)\? @@"
                           then
                           # treat hunk as new diff so it is at the corresponding line
                           if [ -n "${DIFFLINE}" ]; then
                               _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                               DIFF="--- $DIFFFILE\n+++ $DIFFFILE"
                           fi
                           DIFFLINE="${line#@@ -}"
                           DIFFLINE="${DIFFLINE%%[!0-9]*}"
                           # Parse
                           # @@ -<DIFFLINE>,<DIFFOFFSET> ...
                           # shellcheck disable=SC2001 # cond. expr. can't easily be done without sed
                           DIFFOFFSET="$(echo "$line" |
                               sed 's/@@ -[0-9]\+\(,\([0-9]\)\+\)\?.*$/\2/')"
                           if [ -n "$DIFFOFFSET" ]; then
                               # if there is a DIFFOFFSET, add it to
                               # DIFFLINE. DIFFLINE starts at 1, so we
                               # need to subtract 1 to not overshoot.
                               DIFFLINE=$(( DIFFLINE + DIFFOFFSET - 1 ))
                           fi
                        fi
                        # replace all \ by \\
                        DIFF="$DIFF\n${line//\\/\\\\}"
                    fi
                done
                _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
            }
        else
            echo "$OUT"
        fi
    fi
}

# create an array from the changed files list
: "${FILES:=$(FILEREGEX='\.h$' changed_files)}"
mapfile -t FILES_ARR <<< "${FILES}"

if [ -z "${FILES}" ]; then
    exit
fi

github_annotate_setup

: "${QUIET:=0}"

if [ -z "$*" ]; then
    _headercheck
fi

github_annotate_teardown

exit "$EXIT_CODE"
