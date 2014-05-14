#!/usr/bin/env sh

set -o nounset                              # Treat unset variables as an error

if [[ $QUIET == "1" ]]; then
    for i in `ls ../patches`; do
        patch -p1 -N -i ../patches/$i > /dev/null
    done
else
    for i in `ls ../patches`; do
        patch -p1 -N -i ../patches/$i
    done
fi
exit 0
