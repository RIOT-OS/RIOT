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

coccinelle_checkone() {
    OUT="$(spatch --very-quiet \
        --macro-file-builtins ${RIOTTOOLS}/coccinelle/include/riot-standard.h \
        --sp-file $patch ${FILES} | filter)"

    if [ -n "$OUT" ]; then
        if [ $COCCINELLE_QUIET -eq 1 ]; then
            echo "$patch:"
            echo "$OUT" | indent
            if [ COCCINELLE_WARNONLY -eq 0 ]; then
                EXIT_CODE=1
            fi
        else
            echo "$OUT"
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

if [ -z "$*" ]; then
    coccinelle_checkall "${RIOTTOOLS}"/coccinelle/force

    COCCINELLE_WARNONLY=1 coccinelle_checkall "${RIOTTOOLS}"/coccinelle/warn
else
    for patch in "$@"; do
        coccinelle_checkone "$patch"
    done
fi

exit $EXIT_CODE
