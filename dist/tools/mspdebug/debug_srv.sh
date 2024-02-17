#!/usr/bin/env bash

MSPDEBUG="$1"
MSPDEBUG_PROGRAMMER="$2"
PROTOCOL="$3"
MSPDEBUG_TTY="$4"
DEBUG_ADAPTER_ID="$5"
DEBUG_TARGET_ID="$6"
GDBPORT="$7"

if [ -z "${MSPDEBUG_PROGRAMMER}" ]; then
    echo "MSPDEBUG_PROGRAMMER unset, cannot use mspdebug"
fi

args=()
args+=("--expect-id" "$DEBUG_TARGET_ID")
if [ "JTAG" = "${PROTOCOL}" ]; then
    args+=("-j")
else
    if [ "Spy-Bi-Wire" != "${PROTOCOL}" ]; then
        echo "Debugger protocol \"${PROTOCOL}\" not supported, use JTAG or Spy-Bi-Wire"
        exit 1
    fi
fi
args+=("${MSPDEBUG_PROGRAMMER}")
if [ -n "${DEBUG_ADAPTER_ID}" ]; then
    args+=("s" "{$DEBUG_ADAPTER_ID}")
fi
if [ -n "${MSPDEBUG_TTY}" ]; then
    args+=("-d" "${MSPDEBUG_TTY}")
fi
args+=("gdb ${GDBPORT}")

"${MSPDEBUG}" "${args[@]}"
