#!/usr/bin/env bash

# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.

exit_code=1

SCRIPTDIR="$(cd "$(dirname "$0")" || exit 1; pwd)"
for serial in "$@"; do
    if "$SCRIPTDIR"/ttys.py --format path --serial "$serial"; then
        exit_code=0
    fi
done

exit $exit_code
