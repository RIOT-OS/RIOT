#!/bin/sh

# $1 contains generated elf file - $FLASHFILE from makefile

ELFFILE="$1"
UF2FILE="${UF2FILE:-${ELFFILE%%.elf}.uf2}"

udev_warning='[WARNING] Picotool udev rules are not set yet. You won`t be able to '\
'run picotool without sudo. See: '\
'https://github.com/raspberrypi/picotool/tree/master?tab=readme-ov-file#linux--macos\n'\
'The udev rule file can be found in '$(dirname "$0")'/source/udev'

if ! [ -e /etc/udev/rules.d/99-picotool.rules ]
then
  printf '\033[1;31m%b\033[0m\n' "$udev_warning"
fi

echo "[INFO] Converting $ELFFILE to $UF2FILE"
"$(dirname "$0")"/picotool uf2 convert "$ELFFILE" "$UF2FILE"

# -v flag -> Verify
# -x flag -> eXecute after loading
echo "[INFO] Loading $UF2FILE onto Pico"
"$(dirname "$0")"/picotool load -x -v "$UF2FILE"
