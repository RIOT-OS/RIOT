#!/usr/bin/env bash

# Copyright 2018 Jose Ignacio Alamos <jose.alamos@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE=$(git rev-parse --show-toplevel)
CURDIR=$(cd "$(dirname "$0")" && pwd)
: "${QUIET:=0}"
: "${WARNING:=1}"

. "$RIOTBASE"/dist/tools/ci/changed_files.sh
FILEREGEX='\.([CcHh]|[ch]pp)$'
EXCLUDE='^(.+/vendor/)'
FILES=$(FILEREGEX=${FILEREGEX} EXCLUDE=${EXCLUDE} changed_files)

if [ -z "${FILES}" ]; then
    exit 0
fi

PARAMS=""
if [ $QUIET -eq 1 ]; then
    _QUIET="--quiet"
fi

VERA_CMD="vera++ --root $CURDIR --exclusions $CURDIR/exclude $_QUIET"

if [ $WARNING -ne 0 ]; then
    echo "$FILES" | $VERA_CMD --profile riot \
        --parameters $CURDIR/profiles/riot_params.txt -w | sed 's/^/warning: /g'
fi

echo "$FILES" | $VERA_CMD --profile riot_force \
    --parameters $CURDIR/profiles/riot_force_params.txt -w
