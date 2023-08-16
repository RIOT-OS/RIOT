#! /bin/sh
#
# release-stats.sh
# Copyright (C) 2017 Martine Lenders <mail@martine-lenders.eu>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# This script is intended to help release managers to collect statistics about
# a release for https://github.com/RIOT-OS/RIOT/wiki/release-statistics

RIOTBASE="$(realpath "$( cd "$(dirname "${0}")" && pwd )/../../..")"
RELEASE="${1}"

if [ $# -lt 1 ]; then
    echo "usage: ${0} <release> [<old> [<riotbase>]]" >&2
    exit 1
fi

if [ $# -gt 1 ]; then
    OLD="${2}"
else
    OLD="${RELEASE}-devel"
fi

if [ $# -gt 2 ]; then
    RIOTBASE="$(realpath "${3}")"
fi

FILES=$(git ls-tree --full-tree -r "${RELEASE}")
START=$(git -C "${RIOTBASE}" log -n 1 --pretty="format:%at" "${OLD}")
END=$(git -C "${RIOTBASE}" log -n 1 --pretty="format:%at" "${RELEASE}")

echo "    <tr>"
echo "        <td>${RELEASE}</td>"
echo "        <td>$(echo "${FILES}" | wc -l)</td>"
echo -n "        <td>$(echo "${FILES}"  | awk '{ print $3 }' | xargs -I'{}' git cat-file blob '{}'| wc -l) "
echo "($(echo "$FILES" | grep -i "[chs]$" | awk '{ print $3 }' | xargs -I'{}' git cat-file blob '{}' | wc -l))</td>"
echo "        <td>$(( (END - START) / (60 * 60 * 24) )) days</td>"
echo "        <td>$(git log --no-merges --pretty=oneline "${OLD}".."${RELEASE}" | wc -l)</td>"
echo "        <td>$(git diff --shortstat --no-renames "${OLD}".."${RELEASE}" | sed 's#^\s\+##')</td>"
echo "        <td>$(git shortlog --no-merges -n -s "${OLD}".."${RELEASE}" | head -n 5 | sed 's#\s\+# #g;s#^ ##;s#$#<br />#;2,$s#^#            #')</td>"
echo "    </tr>"
