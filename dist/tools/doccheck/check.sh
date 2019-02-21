#!/usr/bin/env bash

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#           2018 Kaspar Schleiser <kaspar@schleiser.de>
#           2018 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE="$(cd $(dirname $0)/../../..; pwd)"

if tput colors &> /dev/null && [ $(tput colors) -ge 8 ]; then
    CERROR="\e[1;31m"
    CWARN="\033[1;33m"
    CRESET="\e[0m"
else
    CERROR=
    CWARN=
    CRESET=
fi

DOXY_OUTPUT=$(make -C "${RIOTBASE}" doc 2>&1)
DOXY_ERRCODE=$?

if [ "${DOXY_ERRCODE}" -ne 0 ] ; then
    echo "'make doc' exited with non-zero code (${DOXY_ERRCODE})"
    echo "${DOXY_OUTPUT}"
    exit 2
else
    ERRORS=$(echo "${DOXY_OUTPUT}" | grep '.*warning' | sed "s#${PWD}/\([^:]*\)#\1#g")
    if [ -n "${ERRORS}" ] ; then
        echo -e "${CERROR}ERROR: Doxygen generates the following warnings:${CRESET}"
        echo "${ERRORS}"
        exit 2
    fi
fi

exclude_filter() {
    grep -v -e vendor -e examples -e tests -e "\<dist/tools\>"
}

# Check all groups are defined
DEFINED_GROUPS=$(git grep @defgroup -- '*.h' '*.c' '*.txt' | \
                    exclude_filter | \
                    grep -oE '@defgroup[ ]+[^ ]+' | \
                    grep -oE '[^ ]+$' | sort -u)

UNDEFINED_GROUPS=$( \
    for group in $(git grep '@ingroup' -- '*.h' '*.c' '*.txt' | \
                    exclude_filter | \
                    grep -oE '[^ ]+$' | sort -u); \
    do \
        echo "${DEFINED_GROUPS}" | grep -xq "${group}" || echo "${group}"; \
    done \
    )

ALL_RAW_INGROUP=$(git grep '@ingroup' -- '*.h' '*.c' '*.txt' | exclude_filter)

UNDEFINED_GROUPS_PRINT=$( \
    for group in ${UNDEFINED_GROUPS}; \
    do \
        echo -e "\n${CWARN}${group}${CRESET} found in:"; \
        echo "${ALL_RAW_INGROUP}" | grep "\<${group}\>$" | sort -u | \
                awk -F: '{ print "\t" $1 }'; \
    done \
    )

if [ -n "${UNDEFINED_GROUPS}" ]
then
    COUNT=$(echo "${UNDEFINED_GROUPS}" | wc -l)
    echo -ne "${CERROR}ERROR${CRESET} "
    echo -e "There are ${CWARN}${COUNT}${CRESET} undefined Doxygen groups:"
    echo "${UNDEFINED_GROUPS_PRINT}"
    exit 2
fi
