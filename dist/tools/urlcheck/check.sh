#!/bin/sh

# Copyright 2020 Gunar Schorcht <gunar@schorcht.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

: "${RIOTBASE:=$(cd $(dirname $0)/../../../; pwd)}"
cd $RIOTBASE

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
. "${RIOTTOOLS}"/ci/changed_files.sh

FILES=""
while [ $# -gt 0 ]; do
    FILES="${FILES} $1"
    shift
done

FILEREGEX='\.([Hh]|[Hh]pp|txt|md)$'
EXCLUDE='^(.+/vendor/|dist/tools/coccinelle/include|boards/common/msba2/tools/src)'

if [ -z "${FILES}" ]; then
    FILES=$(FILEREGEX=${FILEREGEX} EXCLUDE=${EXCLUDE} changed_files)
else
    FILES=`echo "${FILES}" | grep -E ${FILEREGEX} | grep -vE ${EXCLUDE}`
fi

if [ -z "${FILES}" ]; then
    exit
fi

for file in ${FILES}; do
    echo "Checking ${file} ..."
    URL_LINES=`cat ${file} | grep -noP "(?=https|http)[^ )\]\"]+"`
    for url_line in $URL_LINES; do
        url=`echo "$url_line" | grep -oP "(?=https|http)[^ )\]\"]+"`
        line=`echo "$url_line" | cut -d ':' -f1`
        wget --spider --tries=1 --quiet $url
        if [ $? != 0 ]; then
            echo "Invalid URL in ${file}:${line} ${url}"
        fi
    done
done
