#!/bin/sh

# Copyright 2017 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# customizable
DIRS="core drivers pkg sys"

# prepare
EXIT_CODE=0

# check files
if FILES=$(git grep -lE 'ifndef\ +(_.*_H$|.*_H_$)' ${DIRS})
then
    EXIT_CODE=1
    for LINE in ${FILES}
    do
        echo "files with invalid header guard (leading or trailing underscore): ${LINE}"
    done
fi

exit ${EXIT_CODE}
