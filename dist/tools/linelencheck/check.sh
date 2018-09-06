#!/usr/bin/env bash
#
# Copyright (C) 2018 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

: "${WARNING_LIMIT:=80}"
: "${ERROR_LIMIT:=100}"
: "${LINEREGEX:='.*'}"

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR="\033[1;31m"
    CWARN="\033[1;33m"
    CRESET="\033[0m"
else
    CERROR=
    CWARN=
    CRESET=
fi

check_line_length () {
    awk "!/$LINEREGEX/ {next} \
        length(\$0) > $WARNING_LIMIT && length(\$0) < $ERROR_LIMIT \
            {print \"${CWARN}warning${CRESET}:\" \"$1:\" NR \": \" \
            \"Line longer than $WARNING_LIMIT characters\"} \
        length(\$0) > $ERROR_LIMIT \
            {print \"${CERROR}error${CRESET}:\" \"$1:\" NR \": \" \
            \"Line longer than $ERROR_LIMIT characters\";err=1} \
        END{exit err}" \
        "$1"
}

check_line_length "$1"
