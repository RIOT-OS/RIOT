#!/bin/sh

# $1 contains generated elf file - $FLASHFILE from makefile

ELFFILE="$1"
UF2FILE="${UF2FILE:-${ELFFILE%%.elf}.uf2}"

echo "[INFO] Converting $ELFFILE to $UF2FILE"
"$(dirname "$0")"/picotool uf2 convert "$ELFFILE" "$UF2FILE"

echo "[INFO] Loading $UF2FILE onto Pico"
"$(dirname "$0")"/picotool load "$UF2FILE"

echo "[INFO] Verifying $UF2FILE on Pico"
"$(dirname "$0")"/picotool verify "$UF2FILE"
