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

EXIT_CODE=0

filter() {
    if [ $QUIET -eq 0 ]; then
        cat
    else
        grep '^---' | cut -f 2 -d ' '
    fi
}

_headercheck() {
    OUT="$(${RIOTTOOLS}/headerguards/headerguards.py ${FILES} | filter)"

    if [ -n "$OUT" ]; then
        EXIT_CODE=1
        echo "$OUT"
    fi
}

: ${FILES:=$(FILEREGEX='\.h$' changed_files)}

if [ -z "${FILES}" ]; then
    exit
fi

: ${QUIET:=0}

if [ -z "$*" ]; then
    _headercheck
fi

exit $EXIT_CODE
