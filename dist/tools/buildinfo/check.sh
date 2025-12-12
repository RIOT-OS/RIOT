#!/usr/bin/env bash
#
# SPDX-License-Identifier: LGPL-2.1-only
# SPDX-FileCopyrightText: 2025 Marian Buschsieweke

# This tests checks the code base for files that may have a missing
# `#include "buildinfo/{app,board,cpu,riotver}.h"`

# local is not POSIX standard, but supported by even busybox ash
# shellcheck disable=SC3043

set -eu

: "${RIOTBASE:="$(cd "$(dirname "$0")/../../../" || exit; pwd)"}"

: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"
: "${RIOTMAKE:=${RIOTBASE}/makefiles}"

# not running shellcheck with -x in the CI --> disable SC1091
# shellcheck disable=SC1091
. "${RIOTTOOLS}"/ci/github_annotate.sh

SCRIPT_PATH=dist/tools/buildinfo/check.sh

check_for_missing_include() {
    local header="$1"
    local file="$2"
    local offender="$3"
    local error=0

    # handle two false positives with use of RIOT_APPLICATION and RIOT_VERSION
    # in a comment
    case "$file" in
    cpu/esp32/include/sdkconfig.h)
        return 0
        ;;
    examples/advanced/xipfs/main.c)
        return 0
        ;;
    esac

    local found=1
    grep "$offender" "$file" | grep -Eq '^[\t ]*\#[\t el]*if' || found=0

    found=1
    grep -qw "buildinfo/$header\.h" "$file" || found=0
    if [ "$found" -eq 0 ]; then
        local line
        line="$(grep -n "$offender" "$file" | head -n1 | grep -oE '^[0-9]*')"
        github_annotate_error "$file" "$line" "$offender used without #include \"buildinfo/$header.h\""
        echo "$file:$line: Use of $offender needs \`#include \"buildinfo/$header.h\"\`"
        error=1
    fi

    return $error
}

list_files_containing_macro() {
    git -C "$RIOTBASE" grep -lwF "$1"
}

check_missing_app() {
    local error=0
    for file in $(list_files_containing_macro RIOT_APPLICATION); do
        case "$file" in
        *.c)
            check_for_missing_include app "$file" RIOT_APPLICATION || error=1
            ;;
        *.h)
            check_for_missing_include app "$file" RIOT_APPLICATION || error=1
            ;;
        esac
    done

    return $error
}

check_missing_board() {
    local error=0
    for pattern in RIOT_BOARD BOARD_NATIVE; do
        for file in $(list_files_containing_macro "$pattern"); do
            case "$file" in
            *.c)
                check_for_missing_include board "$file" "$pattern" || error=1
                ;;
            *.h)
                check_for_missing_include board "$file" "$pattern" || error=1
                ;;
            esac
        done
    done

    return $error
}

check_missing_cpu() {
    local error=0
    local CPU_PATTERNS="
        RIOT_CPU
        CPU_ARM7TDMI_GBA
        CPU_ATMEGA1284P
        CPU_ATMEGA128RFA1
        CPU_ATMEGA2560
        CPU_ATMEGA256RFR2
        CPU_ATMEGA328P
        CPU_ATMEGA32U4
        CPU_ATMEGA8
        CPU_ATXMEGA
        CPU_CC2538
        CPU_CC26X0_CC13X0
        CPU_CC26X2_CC13X2
        CPU_EFM32
        CPU_ESP32
        CPU_ESP8266
        CPU_FE310
        CPU_GD32V
        CPU_KINETIS
        CPU_LM4F120
        CPU_LPC1768
        CPU_LPC23XX
        CPU_MSP430
        CPU_NATIVE
        CPU_NRF51
        CPU_NRF52
        CPU_NRF53
        CPU_NRF9160
        CPU_QN908X
        CPU_RP2350
        CPU_RPX0XX
        CPU_SAM3
        CPU_SAM4S
        CPU_SAMD21
        CPU_SAMD5X
        CPU_SAML1X
        CPU_SAML21
        CPU_STM32
        "

    for pattern in $CPU_PATTERNS; do
        for file in $(list_files_containing_macro "$pattern"); do
            case "$file" in
            *.c)
                check_for_missing_include cpu "$file" "$pattern" || error=1
                ;;
            *.h)
                check_for_missing_include cpu "$file" "$pattern" || error=1
                ;;
            esac
        done
    done

    return $error
}

check_missing_riotver() {
    local error=0
    for pattern in RIOT_VERSION RIOT_VERSION_CODE; do
        for file in $(list_files_containing_macro "$pattern"); do
            case "$file" in
            *.c)
                check_for_missing_include riotver "$file" "$pattern" || error=1
                ;;
            *.h)
                check_for_missing_include riotver "$file" "$pattern" || error=1
                ;;
            esac
        done
    done

    return $error
}

error=0
check_missing_app || error=1
check_missing_board || error=1
check_missing_cpu || error=1
check_missing_riotver || error=1
exit $error
