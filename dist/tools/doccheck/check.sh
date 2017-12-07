#!/bin/sh

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

RIOTBASE=$(readlink -f "$(dirname $(realpath $0))/../../..")

ERRORS=$(make -C "${RIOTBASE}" doc 2>&1 | \
            grep '.*warning' | \
            sed "s#${PWD}/\([^:]*\)#\1#g")

if [ -n "${ERRORS}" ]
then
    echo "ERROR: Doxygen generates the following warnings:"
    echo "${ERRORS}"
    exit 2
else
    exit 0
fi
