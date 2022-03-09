#!/bin/sh

#cpy2remed - copy to removable media
#$1 contains generated hexfile
#$2 contains directory name for this particular nucleo board which could be
#   set in the board makefile.include using variable DIR_NAME_AT_REMED

HEXFILE="$1"
DEV_DIR="$2"

REMED_MOUNT_PATH="${REMED_MOUNT_PATH:-/media/$USER/$DEV_DIR/}"

cp "$HEXFILE" "$REMED_MOUNT_PATH"
