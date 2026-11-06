#!/bin/sh
#
# SPDX-FileCopyrightText: 2026 Evgeniy Kuznetsov <evvykuznetsov@edu.hse.ru>
# SPDX-License-Identifier: LGPL-2.1-only

: "${RIOTBASE:=$(cd "$(dirname "$0")"/../../../ || exit; pwd)}"
: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"

# not running shellcheck with -x in the CI --> disable SC1091
# shellcheck disable=SC1091
. "${RIOTTOOLS}"/ci/github_annotate.sh

github_annotate_setup

EXIT_CODE=0

for board_dir in "${RIOTBASE}"/boards/*/; do
    board=$(basename "${board_dir}")

    [ "${board}" = "common" ] && continue

    if [ ! -f "${board_dir}/doc.md" ]; then
        MSG="boards/${board}/doc.md is missing"
        if github_annotate_is_on; then
            github_annotate_error_no_file "${MSG}"
        else
            echo "ERROR: ${MSG}"
        fi
        EXIT_CODE=1
    fi
done

github_annotate_teardown

exit "${EXIT_CODE}"
