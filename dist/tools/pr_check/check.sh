#!/usr/bin/env bash
#
# Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"

EXIT_CODE=0

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
    echo -e "${CERROR}Pull request needs squashing:${CRESET}" 1>&2
    echo -e "${SQUASH_COMMITS}"
    EXIT_CODE=1
fi

exit ${EXIT_CODE}
