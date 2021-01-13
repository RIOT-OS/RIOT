#!/usr/bin/env bash
#
# Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

. "$(dirname "$0")/../ci/github_annotate.sh"

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"

EXIT_CODE=0

github_annotate_setup

if tput colors &> /dev/null && [ $(tput colors) -ge 8 ]; then
    CERROR="\e[1;31m"
    CRESET="\e[0m"
else
    CERROR=
    CRESET=
fi

if [[ ${#} -eq 1 ]]; then
    RIOT_MASTER="${1}"
else
    RIOT_MASTER="master"
fi

keyword_filter() {
    grep -i \
        -e "^    [0-9a-f]\+ .\{0,2\}SQUASH" \
        -e "^    [0-9a-f]\+ .\{0,2\}FIX" \
        -e "^    [0-9a-f]\+ .\{0,2\}REMOVE *ME"
}

SQUASH_COMMITS="$(git log $(git merge-base HEAD "${RIOT_MASTER}")...HEAD --pretty=format:"    %h %s" | \
                  keyword_filter)"

if [ -n "${SQUASH_COMMITS}" ]; then
    if github_annotate_is_on; then
        echo "${SQUASH_COMMITS}" | while read commit; do
            ANNOTATION="Commit needs to be squashed: \"${commit}\""
            ANNOTATION="${ANNOTATION}\n\nPLEASE ONLY SQUASH WHEN ASKED BY A "
            ANNOTATION="${ANNOTATION}MAINTAINER!"
            ANNOTATION="${ANNOTATION}\nSee: "
            ANNOTATION="${ANNOTATION}https://github.com/RIOT-OS/RIOT/blob/master/CONTRIBUTING.md#squash-commits-after-review"
            github_annotate_error_no_file "${ANNOTATION}"
        done
    else
        echo -e "${CERROR}Pull request needs squashing:${CRESET}" 1>&2
        echo -e "${SQUASH_COMMITS}"
    fi
    EXIT_CODE=1
fi

github_annotate_teardown

github_annotate_report_last_run

exit ${EXIT_CODE}
