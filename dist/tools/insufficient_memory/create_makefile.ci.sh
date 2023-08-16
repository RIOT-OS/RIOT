#!/usr/bin/env bash
#
# Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

MAKE_ARGS="-j4"
APP_DIR=$(pwd)

if tput colors &> /dev/null && [ "$(tput colors)" -ge 8 ]; then
    COK="\e[1;32m"
    CBIG="\e[1;34m"
    CNORMAL="\e[1m"
    CSKIP="\e[1;36m"
    CERROR="\e[1;31m"
    CWARN="\e[1;33m"
    CRESET="\e[0m"
else
    COK=
    CBIG=
    CNORMAL=
    CSKIP=
    CERROR=
    CWARN=
    CRESET=
fi

if [ "$1" == "--no-docker" ]; then
    LOCAL_MAKE_ARGS=${MAKE_ARGS}
    shift 1
else
    # Use a standardized build within Docker and with minimal output
    export DOCKER_MAKE_ARGS=${MAKE_ARGS}
    export BUILD_IN_DOCKER=1
fi

export RIOT_CI_BUILD=1

BOARDS=

rm "${APP_DIR}/Makefile.ci" 2>/dev/null
touch "${APP_DIR}/Makefile.ci"

for BOARD in $(make  --no-print-directory info-boards-supported -C "${APP_DIR}"); do
    printf "${CNORMAL}%-40s${CRESET}" "${BOARD}"
    output=$(make BOARD="${BOARD}" ${LOCAL_MAKE_ARGS} clean all -C "${APP_DIR}" 2>&1)
    if [ "$?" != 0 ]; then
        if echo "${output}" | grep -e overflowed -e "not within region" > /dev/null; then
            printf "${CBIG}%s${CRESET}\n" "too big"
            BOARDS="${BOARDS} ${BOARD}"
        elif echo "${output}" | grep -e "not whitelisted" -e "unsatisfied feature requirements" > /dev/null; then
            printf "${CWARN}%s${CRESET}\n" "not supported"
        else
            printf "${CERROR}%s${CRESET}\n" "build failed"
        fi
    else
        if echo "${output}" | grep -e "skipping link step" > /dev/null; then
            printf "${CSKIP}%s${CRESET}\n" "skipped"
        else
            printf "${COK}%s${CRESET}\n" "OK"
        fi
    fi
done

rm "${APP_DIR}/Makefile.ci"
make -f "$(dirname "$0")"/Makefile.for_sh DIR="${APP_DIR}" BOARD="${BOARDS}" Makefile.ci > /dev/null
