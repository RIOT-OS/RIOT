#!/usr/bin/env bash

#
# Copyright (C) 2015 Eistec AB
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.
#

# Find all USB to serial devices

# default error status code
status=1

# iterate over usb-tty devices:
for basedev in $(find /sys/bus/usb/devices/ -regex "/sys/bus/usb/devices/[0-9]+[^:/]*" -maxdepth 2 -follow 2>/dev/null); do
    ttydirs=$(find ${basedev} -regex "${basedev}/[^/]*:.*" -mindepth 2 -maxdepth 3 -name tty -follow 2>/dev/null)
    if [ -z "${ttydirs}" ]; then
        continue
    fi
    # See if the device has any tty devices assigned to it
    for tty in $(find ${ttydirs} -maxdepth 1 -mindepth 1 -printf '%f\n' 2>/dev/null); do
        parent=$(echo ${basedev} | sed -e 's%\(/sys/bus/usb/devices/[^/]*\)/.*%\1%')
        serial=$(cat "${parent}/serial" 2>/dev/null)
        # split results into array

        if [ $# -lt 1 ]; then
            # No arguments given, return all ttys
            echo "/dev/${tty}"
            status=0
            continue
        fi
        # else: Match any of the given serials
        for s in "${@}"; do
            echo "${serial}" | egrep -e "${s}" -q
            if [ $? -eq 0 ]; then
                # return tty
                echo "/dev/${tty}"
                status=0
            fi
        done
    done
done

exit $status;
