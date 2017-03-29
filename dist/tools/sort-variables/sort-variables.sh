#!/usr/bin/env bash
#
# Copyright (C) 2017 OTA keys S.A.
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# Author: Vincent Dupont <vincent@otakeys.com>
#

SEARCH_PATH=$1
VAR=$2
TMPFILE=$(mktemp)

if [ $# -lt 2 ]; then
    echo "Usage: sort-variables.sh <path> <variable>"
    echo "Sort the <variable> alphabetically, and cut lines to 80 characters"
    echo "It will search is <path> and its subdirectories"
    echo "A variable is defined as a string beginning a line, it's then assumed that a '=' will follow"
    exit 1
fi

SCRIPT_PATH=$(dirname $0)

echo "Looking for " $VAR

FILES=$(git grep -e "^${VAR}" | awk -F ':' '{print $1}')
echo $FILES

for FILE in $FILES; do
    sed -e "/$VAR/{:a" -e '/\\$/N; s/\\\n//; ta}' ${FILE} > ${TMPFILE}
    LIST=$(grep ${VAR} ${TMPFILE} | awk -F '=' '{print $2}' | awk '{for (i=NF; i>=1; i--) { print  $i }}' | sort | uniq)


    awk -vLIST="$LIST" -vVAR="$VAR" -F '=' -f ${SCRIPT_PATH}/build_file.awk ${TMPFILE} > $FILE

    rm ${TMPFILE}
done
