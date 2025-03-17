#!/usr/bin/env bash

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#           2018 Kaspar Schleiser <kaspar@schleiser.de>
#           2018 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# If we don't find this scripts directory, does it really make sense to exit? So
# shellcheck disable=SC2164
SCRIPTDIR="$(cd "$(dirname "$0")"; pwd)"
RIOTBASE="$(cd "${SCRIPTDIR}"/../../..; pwd)"
EXCLUDE_SIMPLE_FILE="${SCRIPTDIR}/exclude_simple"
EXCLUDE_PATTERN_FILE="${SCRIPTDIR}/exclude_patterns"
GENERIC_EXCLUDE_PATTERN_FILE="${SCRIPTDIR}/generic_exclude_patterns"

if [[ -z ${DOCUMENTATION_FORMAT+x} ]]; then
export DOCUMENTATION_FORMAT=check
fi

. "${RIOTBASE}"/dist/tools/ci/github_annotate.sh

github_annotate_setup

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    CERROR="\e[1;31m"
    CWARN="\033[1;33m"
    CRESET="\e[0m"
else
    CERROR=
    CWARN=
    CRESET=
fi
DOXY_OUTPUT=$(make -C "${RIOTBASE}" doc 2>&1| grep -Fvf "${EXCLUDE_SIMPLE_FILE}" )
DOXY_OUTPUT=$(echo "${DOXY_OUTPUT}" | grep -Evf "${EXCLUDE_PATTERN_FILE}" | grep -Evf "${GENERIC_EXCLUDE_PATTERN_FILE}")
DOXY_ERRCODE=$?
RESULT=0

if [ "${DOXY_ERRCODE}" -ne 0 ] ; then
    echo "'make doc' exited with non-zero code (${DOXY_ERRCODE})"
    echo "${DOXY_OUTPUT}"
    RESULT=2
else
    ERRORS=$(echo "${DOXY_OUTPUT}" | grep '.*warning' | sed "s#${PWD}/\([^:]*\)#\1#g")
    if [ -n "${ERRORS}" ] ; then
        if github_annotate_is_on; then
            # We want to see backslashes here to escape e.g. newlines in
            # github_annotate_error
            # shellcheck disable=SC2162
            echo "${ERRORS}" | grep "^.\+:[0-9]\+: warning:" | while read error_line
            do
                FILENAME=$(echo "${error_line}" | cut -d: -f1)
                LINENR=$(echo "${error_line}" | cut -d: -f2)
                DETAILS=$(echo "${error_line}" | cut -d: -f4- |
                          sed -e 's/^[ \t]*//' -e 's/[ \t]*$//')
                github_annotate_error "${FILENAME}" "${LINENR}" "${DETAILS}"
            done
            # show errors not matching the pattern above
            echo "${ERRORS}" | grep -v "^.\+:[0-9]\+: warning:"
        else
            echo -e "${CERROR}ERROR: Doxygen generates the following warnings:${CRESET}"
            echo "${ERRORS}"
        fi
        RESULT=2
    fi
fi

exclude_filter() {
    grep -v -e vendor -e examples -e tests -e "\<dist/tools\>"
}

# Check groups are correctly defined (e.g. no undefined groups and no group
# defined multiple times)
ALL_RAW_DEFGROUP=$(git grep -n '@defgroup' -- '*.h' '*.hpp' '*.txt' '*/doc.md' '*.doc.md' 'makefiles/pseudomodules.inc.mk' 'sys/net/gnrc/routing/ipv6_auto_subnets/gnrc_ipv6_auto_subnets.c'| exclude_filter)
ALL_RAW_INGROUP=$(git grep -n '@ingroup' -- '*.h' '*.hpp' '*.txt' '*/doc.md' '*.doc.md' 'makefiles/pseudomodules.inc.mk' 'sys/net/gnrc/routing/ipv6_auto_subnets/gnrc_ipv6_auto_subnets.c'| exclude_filter)
DEFINED_GROUPS=$(echo "${ALL_RAW_DEFGROUP}" | \
                    grep -oE '@defgroup[ ]+[^ ]+' | \
                    grep -oE '[^ ]+$' | \
                    sort)
DEFINED_GROUPS_UNIQUE=$(echo "${DEFINED_GROUPS}" | sort -u)

# Check for undefined groups
UNDEFINED_GROUPS=$( \
    for group in $(echo "${ALL_RAW_INGROUP}" | \
                    grep -oE '[^ ]+$' | sort -u); \
    do \
        echo "${DEFINED_GROUPS_UNIQUE}" | grep -xq "${group}" || echo "${group}"; \
    done \
    )

if [ -n "${UNDEFINED_GROUPS}" ]
then
    COUNT=$(echo "${UNDEFINED_GROUPS}" | wc -l)
    echo -ne "\n\n${CERROR}ERROR${CRESET} "
    echo -ne "There are ${CWARN}${COUNT}${CRESET} undefined Doxygen groups"
    if github_annotate_is_on; then
        echo ""
    else
        echo ":"
    fi
    for group in ${UNDEFINED_GROUPS};
    do
        INGROUPS=$(echo "${ALL_RAW_INGROUP}" | grep "\<${group}\>$" | sort -u)
        if github_annotate_is_on; then
            # We want to see backslashes here to escape e.g. newlines in
            # github_annotate_error
            # shellcheck disable=SC2162
            echo "${INGROUPS}" | while read ingroup;
            do
                github_annotate_error \
                    "$(echo "${ingroup}" | awk -F: '{ print $1,$2 }')" \
                    "Undefined doxygen group '${group}'"
            done
        else
            echo -e "\n${CWARN}${group}${CRESET} found in:";
            echo "${INGROUPS}" | awk -F: '{ print "\t" $1 }';
        fi
    done
    RESULT=2
fi

# Check for groups defined multiple times:
MULTIPLE_DEFINED_GROUPS=$(echo "${DEFINED_GROUPS}" | uniq -d)

if [ -n "${MULTIPLE_DEFINED_GROUPS}" ]
then
    COUNT=$(echo "${MULTIPLE_DEFINED_GROUPS}" | wc -l)
    echo -ne "\n\n${CERROR}ERROR${CRESET} "
    echo -ne "There are ${CWARN}${COUNT}${CRESET} Doxygen groups defined multiple times"
    if github_annotate_is_on; then
        echo ""
    else
        echo ":"
    fi
    for group in ${MULTIPLE_DEFINED_GROUPS};
    do
        DEFGROUPS=$(echo "${ALL_RAW_DEFGROUP}" |
            awk -F@ '{ split($2, end, " "); printf("%s%s\n",$1,end[2]) }' |
            grep "\<${group}\>$" | sort -u)
        DEFGROUPFILES=$(echo "${DEFGROUPS}" | awk -F: '{ print "\t" $1 }')
        if github_annotate_is_on; then
            # We want to see backslashes here to escape e.g. newlines in
            # github_annotate_error
            # shellcheck disable=SC2162
            echo "${DEFGROUPS}" | while read defgroup;
            do
                github_annotate_error "$(echo "${defgroup}" | awk -F: '{ print $1,$2 }')" \
                    "Multiple doxygen group definitions of '${group}' in\n${DEFGROUPFILES}"
            done
        else
            echo -e "\n${CWARN}${group}${CRESET} defined in:";
            echo "${DEFGROUPFILES}"
        fi
    done
    RESULT=2
fi
github_annotate_teardown
exit ${RESULT}
