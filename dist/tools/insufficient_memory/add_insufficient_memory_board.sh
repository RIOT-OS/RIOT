#!/bin/bash
#
# Copyright (C) 2019 Benjamin Valentin <benjamin.valentin@ml-pa.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

if [ -z $1 ]; then
    echo "usage: $0 <board>"
    exit 1
fi

BOARD=$1
RIOTBASE=$(dirname $0)/../../..
PROJECTS+=$RIOTBASE/examples/*/Makefile
PROJECTS+=" "
PROJECTS+=$RIOTBASE/tests/*/Makefile

for i in $PROJECTS; do
    test=$(dirname $(realpath $i));
    if make BOARD=$BOARD -j -C $test 2>&1 >/dev/null | grep -e overflowed -e "not within region" > /dev/null; then
        echo $(basename $test) is too big for $BOARD
        make -f Makefile.for_sh -C $(dirname $0) DIR=$test BOARD=$BOARD Makefile.ci > /dev/null
    else
        echo $(basename $test) is OK
    fi
done
