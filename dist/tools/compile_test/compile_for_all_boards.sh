#!/bin/sh
# Compile an an application for all boards.
# Store the boards where it failed in `failed.txt`.

if [ -z $1 ]; then
    echo "Usage: $0 <app_dir>"
    exit 1
fi

OUT=$1/failed.txt
rm -f $OUT

BOARDS=$(make --no-print-directory -C $1 info-boards-supported)

for i in $BOARDS; do
    if make -C $1 BOARD=$i -j > /dev/null ; then
        echo $i ok
    else
        echo $i >> $OUT;
    fi
done

if [ -f $OUT ]; then
    echo "Failed on:"
    cat $OUT
fi
