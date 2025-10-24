#!/usr/bin/env bash
#
# Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

# shellcheck source=/dev/null
. "$(dirname "$0")/../ci/github_annotate.sh"

: "${RIOTBASE:="$(cd "$(dirname "$0")"/../../../ || exit; pwd)"}"
cd "$RIOTBASE" || exit

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"

EXIT_CODE=0

# Keywords that should trigger the commit message check and prevent an accidental
# merge of something not meant to be merged.
# The pretty-print format of the commit messages is always the following:
# `a5e4f038b8 commit message`
# This has to be reflected in the RegEx matching pattern.
NOMERGE_KEYWORD_FILE="$(dirname "$0")/no_merge_keywords"

github_annotate_setup

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
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

SQUASH_COMMITS="$(git log "$(git merge-base HEAD "${RIOT_MASTER}")"...HEAD --pretty=format:"%h %s" | \
                grep -i -f "${NOMERGE_KEYWORD_FILE}")"

if [ -n "${SQUASH_COMMITS}" ]; then
    if github_annotate_is_on; then
        ANNOTATION=""
        while read -r commit; do
            ANNOTATION="${ANNOTATION}Commit needs to be squashed or contains a no-merge keyword: \"${commit}\"\n"
        done < <(echo "${SQUASH_COMMITS}")

        if [ -n "${ANNOTATION}" ]; then
            ANNOTATION="${ANNOTATION}\nPLEASE ONLY SQUASH WHEN ASKED BY A "
            ANNOTATION="${ANNOTATION}MAINTAINER!"
            ANNOTATION="${ANNOTATION}\nSee: "
            ANNOTATION="${ANNOTATION}https://github.com/RIOT-OS/RIOT/blob/master/CONTRIBUTING.md#squash-commits-after-review\n"
            github_annotate_error_no_file "${ANNOTATION}"
        fi
    else
        echo -e "${CERROR}Pull request needs squashing or contains no-merge keywords:${CRESET}" 1>&2
        while IFS= read -r line; do
            echo -e "    ${line}"
        done <<< "${SQUASH_COMMITS}"
    fi
    EXIT_CODE=1
fi

github_annotate_teardown

github_annotate_report_last_run

exit ${EXIT_CODE}
