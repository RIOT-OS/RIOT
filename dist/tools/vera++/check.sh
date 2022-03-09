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
. "$RIOTBASE"/dist/tools/ci/github_annotate.sh
# tests/pkg_utensor/models/deep_mlp_weight.hpp is an auto-generated file
# with lots of commas so T009 takes very long. Since it is auto-generated, just
# exclude it.
EXCLUDE='^(.+/vendor/|dist/tools/coccinelle/include|dist/tools/fixdep/fixdep.c|dist/tools/lpc2k_pgm/src|tests/pkg_utensor/external_modules/models)'
FILES=$(changed_files)

if [ -z "${FILES}" ]; then
    exit 0
fi

PARAMS=""
if [ $QUIET -eq 1 ]; then
    _QUIET="--quiet"
fi

VERA_CMD="vera++ --root $CURDIR --exclusions $CURDIR/exclude $_QUIET"

# sets
# - LOG to tee output into for later parsing
# - LOGFILE to parse GitHub annotations into
github_annotate_setup

RESULT=0    # remove if script should error

echo "$FILES" | $VERA_CMD --profile riot \
    --parameters $CURDIR/profiles/riot_params.txt -w 2>&1 | ${LOG}
# Uncomment when the script should error
# RESULT=${PIPESTATUS[0]}

if github_annotate_is_on; then
    _annotate() {
        FILENAME=$(echo "${line}" | cut -d: -f1)
        LINENUM=$(echo "${line}" | cut -d: -f2)
        DETAILS=$(echo "${line}" | cut -d: -f3- |
                  sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')

        # parse if warning (errors don't have a label)
        if echo "$DETAILS" | grep -q "^warning: "; then
            DETAILS=$(echo "$DETAILS" | sed 's/^warning: //')
            github_annotate_warning "$FILENAME" "$LINENUM" "$DETAILS"
        else
            # errors might start with "error:" (but don't have to)
            DETAILS=$(echo "$DETAILS" | sed 's/^error: //')
            github_annotate_error "$FILENAME" "$LINENUM" "$DETAILS"
        fi
    }

    github_annotate_parse_log_default _annotate
fi

github_annotate_teardown

exit ${RESULT}
