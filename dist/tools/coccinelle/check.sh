#!/usr/bin/env bash

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
    if [ $COCCINELLE_QUIET -eq 0 ]; then
        cat
    else
        grep '^---' | cut -f 2 -d ' '
    fi
}

indent() {
    sed 's/^/    /g'
}

_annotate_diff() {
    if [ -n "$1" -a -n "$2" -a -n "$3" ]; then
        MSG="Coccinelle proposes the following patch:\n\n$3"
        if [ $COCCINELLE_WARNONLY -eq 0 ]; then
            IFS="${OLD_IFS}" github_annotate_error "$1" "$2" "${MSG}"
        else
            IFS="${OLD_IFS}" github_annotate_warning "$1" "$2" "${MSG}"
        fi
    fi
}

coccinelle_checkone() {
    : ${COCCINELLE_WARNONLY:=0}
    OUT="$(spatch --very-quiet \
        --macro-file-builtins ${RIOTTOOLS}/coccinelle/include/riot-standard.h \
        --sp-file $patch ${FILES} | filter)"

    if [ -n "$OUT" ]; then
        if [ $COCCINELLE_QUIET -eq 1 ]; then
            echo "$patch:"
            echo "$OUT" | indent
            if [ $COCCINELLE_WARNONLY -eq 0 ]; then
                EXIT_CODE=1
            fi
        else
            if github_annotate_is_on; then
                DIFF=""
                DIFFFILE=""
                DIFFLINE=""
                echo "$OUT" | {
                    # see https://stackoverflow.com/a/30064493/11921757 for why we
                    # use a sub shell here
                    OLD_IFS="${IFS}"    # store old separator to later restore it
                    IFS=''  # keep leading and trailing spaces
                    while read -r line; do
                        # parse beginning of new diff
                        if echo "$line" | grep -q '^--- .\+$'; then
                            _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                            DIFF="$line"
                            DIFFFILE=$(echo "$line" | sed 's/^--- \(.\+\)$/\1/g')
                            DIFFLINE=""
                        # we are in a diff currently
                        elif [ -n "$DIFF" ]; then
                            # replace coccinelle temp file name with real
                            # filename to make for an easier copy-pastable
                            # patch
                            line=$(echo "$line" | sed "s#+++ .*#+++ $DIFFFILE#")
                            # grep first line number of diff including offset to
                            # comment _under_ diff
                            if echo "$line" | \
                               grep -q "@@ -[0-9]\+\(,[0-9]\+\)\? +[0-9]\+\(,[0-9]\+\)\? @@"
                               then
                               # treat hunk as new diff so it is at the corresponding line
                               if [ -n "${DIFFLINE}" ]; then
                                   _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                                   DIFF="--- $DIFFFILE\n+++ $DIFFFILE"
                               fi
                               DIFFLINE="$(echo "$line" | sed 's/@@ -\([0-9]\+\).*$/\1/')"
                               # Parse
                               # @@ -<DIFFLINE>,<DIFFOFFSET> ...
                               DIFFOFFSET="$(echo "$line" |
                                   sed 's/@@ -[0-9]\+\(,\([0-9]\)\+\)\?.*$/\2/')"
                               if [ -n "$DIFFOFFSET" ]; then
                                   # if there is a DIFFOFFSET, add it to
                                   # DIFFLINE. DIFFLINE starts at 1, so we
                                   # need to subtract 1 to not overshoot.
                                   DIFFLINE=$(( DIFFLINE + DIFFOFFSET - 1 ))
                               fi
                            fi
                            DIFF="$DIFF\n${line//\\/\\\\}"
                        fi
                    done
                    _annotate_diff "$DIFFFILE" "$DIFFLINE" "$DIFF"
                }
            else
                echo "$OUT"
            fi
        fi
    fi
}

coccinelle_checkall() {
    local dir="$1"
    local warn="${2:-0}"

    [ -d "$dir" ] || {
        echo "$0: coccinelle_checkall(): $dir doesn't exist!"
        exit 1
    }

    for patch in $dir/*; do
        coccinelle_checkone $patch
    done
}

: ${FILES:=$(FILEREGEX='\.c$' changed_files)}

if [ -z "${FILES}" ]; then
    exit
fi

: ${COCCINELLE_QUIET:=0}

github_annotate_setup

if [ -z "$*" ]; then
    coccinelle_checkall "${RIOTTOOLS}"/coccinelle/force

    COCCINELLE_WARNONLY=1 coccinelle_checkall "${RIOTTOOLS}"/coccinelle/warn
else
    for patch in "$@"; do
        coccinelle_checkone "$patch"
    done
fi

github_annotate_teardown

exit $EXIT_CODE
