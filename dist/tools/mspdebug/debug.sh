#!/usr/bin/env bash

MSPDEBUG="$1"
MSPDEBUG_PROGRAMMER="$2"
PROTOCOL="$3"
MSPDEBUG_TTY="$4"
DEBUG_ADAPTER_ID="$5"
GDBPORT="$6"
ELFFILE="$7"
PREFIX="$8"
RIOTBASE="$9"

# The setsid command is needed so that Ctrl+C in GDB doesn't kill mspdebug
: "${SETSID:=setsid}"

if "${PREFIX}-gdb" -v > /dev/null 2>&1; then
    GDB="${PREFIX}-gdb"
elif gdb-multiarch -v > /dev/null 2>&1; then
    GDB=gdb-multiarch
else
    echo "Couldn't find GDB (tried ${PREFIX}-gdb and gdb-multiarch). Check \$PATH."
    exit 1
fi

if [ -z "${MSPDEBUG_PROGRAMMER}" ]; then
    echo "MSPDEBUG_PROGRAMMER unset, cannot use mspdebug"
fi

sleep 2
args=()
if [ "JTAG" = "${PROTOCOL}" ]; then
    args+=("-j")
elif [ "Spy-Bi-Wire" != "${PROTOCOL}" ]; then
    echo "Debugger protocol \"${PROTOCOL}\" not supported, use JTAG or Spy-Bi-Wire"
    exit 1
fi
args+=("${MSPDEBUG_PROGRAMMER}")
if [ -n "${DEBUG_ADAPTER_ID}" ]; then
    args+=("s" "{$DEBUG_ADAPTER_ID}")
fi
if [ -n "${MSPDEBUG_TTY}" ]; then
    args+=("-d" "${MSPDEBUG_TTY}")
fi
args+=("gdb ${GDBPORT}")

${SETSID} -w "${MSPDEBUG}" "${args[@]}" > /dev/null 2>&1 &
echo "Please wait..."

sleep 3
exec "$GDB" \
    -ex "target remote localhost:${GDBPORT}" \
    -ex "monitor reset halt" "$ELFFILE" \
    -ex "set substitute-path /data/riotbuild/riotbase $RIOTBASE"
