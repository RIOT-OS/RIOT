#!/usr/bin/env sh
#
# Copyright (C) 2021 Benjamin Valentin <benjamin.valentin@ml-pa.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

if nproc > /dev/null 2>&1; then
    MAKE_ARGS="-j$(nproc)"
fi
APP_DIR=$(pwd)

if tput colors > /dev/null 2>&1 && [ "$(tput colors)" -ge 8 ]; then
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

if [ "$1" = "--no-docker" ]; then
    LOCAL_MAKE_ARGS="${MAKE_ARGS}"
    shift 1
else
    # Use a standardized build within Docker and with minimal output
    export DOCKER_MAKE_ARGS="${MAKE_ARGS}"
    export BUILD_IN_DOCKER=1
fi

export RIOT_CI_BUILD=1

BOARDS=

rm "${APP_DIR}/Makefile.ci" 2>/dev/null
touch "${APP_DIR}/Makefile.ci"
TMPFILE="$(mktemp)"

for BOARD in $(EXTERNAL_BOARD_DIRS="" make  --no-print-directory info-boards-supported -C "${APP_DIR}"); do
    printf "${CNORMAL}%-40s${CRESET}" "${BOARD}"
    # disabling warning about globbing and word splitting for LOCAL_MAKE_ARGS,
    # as this is exactly what we want here
    # shellcheck disable=SC2086
    if ! make BOARD="${BOARD}" ${LOCAL_MAKE_ARGS} clean all -C "${APP_DIR}" > "$TMPFILE" 2>&1; then
        if grep -e overflowed \
                -e "not within region" \
                -e "wraps around address space" \
                -e "overlaps section" \
                -e "does not fit in ROM" \
                "$TMPFILE" > /dev/null; then
            printf "${CBIG}%s${CRESET}\n" "too big"
            BOARDS="${BOARDS} ${BOARD}"
        elif grep -e "not whitelisted" \
                  -e "unsatisfied feature requirements" \
                  -e "Some feature requirements are blacklisted:" \
                  -e "not supported.  Stop." \
                  -e "let the build continue on expected errors by setting CONTINUE_ON_EXPECTED_ERRORS=1" \
                  "$TMPFILE" > /dev/null; then
            printf "${CWARN}%s${CRESET}\n" "not supported"
        else
            printf "${CERROR}%s${CRESET}\n" "build failed"
            cat "$TMPFILE"
        fi
    else
        if grep -e "skipping link step" "$TMPFILE" > /dev/null; then
            printf "${CSKIP}%s${CRESET}\n" "skipped"
        else
            printf "${COK}%s${CRESET}\n" "OK"
        fi
    fi
done

rm "${APP_DIR}/Makefile.ci"
make -f "$(dirname "$0")"/Makefile.for_sh DIR="${APP_DIR}" ADD_BOARDS="${BOARDS}" Makefile.ci > /dev/null
