#!/bin/sh

# $1 contains generated elf file - $FLASHFILE from makefile

ELFFILE="$1"
UF2FILE="${UF2FILE:-${ELFFILE%%.elf}.uf2}"
ELF2UF2_MOUNT_PATH="${ELF2UF2_MOUNT_PATH:-/media/$USER/RPI-RP2}"

"$(dirname "$0")"/elf2uf2 "$ELFFILE" "$UF2FILE"

cp "$UF2FILE" "$ELF2UF2_MOUNT_PATH"
