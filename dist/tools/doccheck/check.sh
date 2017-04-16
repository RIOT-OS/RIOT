#!/bin/sh

# Copyright 2014 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

ERRORS=$(make doc 2>&1 | grep '.*warning' | sed "s#.*${PWD}/\([^:]*\).*#\1#" | sort | uniq)

if [ -n "${ERRORS}" ]
then
    echo "ERROR: The following files generate doxygen warnings:"
    echo "${ERRORS}"
    exit 2
else
    exit 0
fi
