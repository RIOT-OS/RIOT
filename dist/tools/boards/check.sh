#!/bin/sh
#
# Copyright (c) 2017 Kaspar Schleiser <kaspar@schleiser.de>
#
# This script checks whether the board groups defined in boards/groups.mk
# contain and only contain boards that actually exist in the tree.

: ${RIOTBASE:=$(pwd)}

export RIOTBASE

BOARD_LIST="$(make --no-print-directory -C${RIOTBASE}/dist/tools/boards list-boards)"
BOARD_FOLDERS="$(find ${RIOTBASE}/boards/* -maxdepth 0 -type d \! -name "common" -exec basename {} \;)"

if [ "$(echo ${BOARD_LIST} | sort)" != "$(echo ${BOARD_FOLDERS} | sort)" ]; then
    echo "$0: board list mismatch!"

    LIST_MISSING="$(echo "$BOARD_FOLDERS" | grep -vwF "${BOARD_LIST}")"
    FOLDER_MISSING="$(echo "$BOARD_LIST" | grep -vwF "${BOARD_FOLDERS}")"

    [ -n "$LIST_MISSING" ] && {
        echo "Boards missing in boards/group.mk:"
        echo "$LIST_MISSING"
    }

    [ -n "$FOLDER_MISSING" ] && {
        echo "Boards listed in boards/group.mk without board folder:"
        echo "$FOLDER_MISSING"
    }

    exit 1
fi
true
