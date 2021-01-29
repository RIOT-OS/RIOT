#!/usr/bin/env bash

if [ -z $2 ]; then
    echo "usage: $0 <old_board> <new_board>"
    echo ""
    echo "Add a new board with the same MCU as an old board."
    exit 1
fi

BOARD_OLD=$1
BOARD_NEW=$2
RIOTBASE=$(dirname $0)/../../..

find $RIOTBASE -name Makefile.ci | while read FILE; do
    if grep $BOARD_OLD $FILE > /dev/null; then
        make -f $(dirname $0)/Makefile.for_sh DIR=$(dirname $FILE) BOARD=${BOARD_NEW} Makefile.ci > /dev/null
    fi
done
