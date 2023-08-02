#!/usr/bin/env sh
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.

SCRIPTDIR="$(cd "$(dirname "$0")" || exit 1; pwd)"
"$SCRIPTDIR"/ttys.py --format path
