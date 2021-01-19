#!/bin/sh

# Copyright 2017 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh
. "${RIOTTOOLS}"/ci/github_annotate.sh

EXIT_CODE=0

filter() {
    if [ $QUIET -eq 0 ]; then
        cat
    else
        grep '^---' | cut -f 2 -d ' '
    fi
}

_annotate_diff() {
    if [ -n "$1" -a -n "$2" -a -n "$3" ]; then
        IFS="${OLD_IFS}" github_annotate_error "$1" "$2" "Wrong header guard format:\n\n$3"
    fi
}

_headercheck() {
    OUT=$(${RIOTTOOLS}/headerguards/headerguards.py ${FILES} 2>&1 | filter)
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
                    if echo "$line" | grep -q '^.*: no / broken header guard$'; then
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
                    elif echo "$line" | grep -q '^--- .\+$'; then
                        _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                        DIFF="$line"
                        DIFFFILE=$(echo "$line" | sed 's/^--- \(.\+\)$/\1/g')
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
                           DIFFLINE="$(echo "$line" | sed 's/@@ -\([0-9]\+\).*$/\1/')"
                        fi
                        DIFF="$DIFF\n$(echo "${line}"| sed 's/\\/\\\\/g' )"
                    fi
                done
                _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
            }
        else
            echo "$OUT"
        fi
    fi
}

: ${FILES:=$(FILEREGEX='\.h$' changed_files)}

if [ -z "${FILES}" ]; then
    exit
fi

github_annotate_setup

: ${QUIET:=0}

if [ -z "$*" ]; then
    _headercheck
fi

github_annotate_teardown

exit $EXIT_CODE
