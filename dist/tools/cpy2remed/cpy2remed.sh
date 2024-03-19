#!/bin/sh

# Copyright 2022 Krzysztof Cabaj <kcabaj@gmail.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

#cpy2remed - copy to removable media
#$1 contains generated hexfile
#$2 contains directory name for this particular nucleo board which could be
#   set in the board makefile.include using variable DIR_NAME_AT_REMED

HEXFILE="$1"
DEV_DIR="$2"

REMED_MOUNT_PATH="${REMED_MOUNT_PATH:-/media/$USER/$DEV_DIR/}"

if [ -d "$REMED_MOUNT_PATH" ]
then
    echo "Directory $REMED_MOUNT_PATH exist."
    echo "Copying image file ..."
    if cp "$HEXFILE" "$REMED_MOUNT_PATH"; then
        echo "Copying ended with success."
    else
        echo "Copying ended with ERROR!"
    fi

else
    #recently produced Nucleo boards use NOD_xxxx while older NODE_xxxx

    #substiute in path NODE_ to NOD_
    #NEW_REMED_MOUNT_PATH="${REMED_MOUNT_PATH/NODE_/NOD_}"
    NEW_REMED_MOUNT_PATH=$(echo "$REMED_MOUNT_PATH" | sed -e "s|NODE_|NOD_|g")
    if [ -d "$NEW_REMED_MOUNT_PATH" ]
    then
        echo "$NEW_REMED_MOUNT_PATH directory exist."
        echo "Copying image file ..."
    if cp "$HEXFILE" "$NEW_REMED_MOUNT_PATH"; then
        echo "Copying ended with success."
    else
        echo "Copying ended with ERROR!"
    fi

    else
        echo "Check if Nucleo board is connected!"
        echo "If board uses for removable media different name than NODE_xxxxx or NOD_xxxx"
        echo "add DIR_NAME_AT_REMED value to board's Makefile.include."
        echo "More details in RIOT PR #18057."
    fi
fi
