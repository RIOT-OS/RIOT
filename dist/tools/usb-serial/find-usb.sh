#!/usr/bin/env bash

#
# Copyright (C) 2018 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.
#

# Find the USB hardware address linked to logical address (dev/ttyusb*)

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
        # split results into array

        if [ $# -lt 1 ]; then
            # No arguments given, return all usb hardware addresses
            uper=$(echo ${parent} | tr -d '/sys/bus/usb/devices/' | \
                   tr '.' ',' | tr '-' ':')
            echo "${uper}"
            status=0
            continue
        fi
        # else: Match any of the given tty
        for s in "${@}"; do
            echo "/dev/${tty}" | egrep -e "${s}" -q
            if [ $? -eq 0 ]; then
                # return usb hardware address
                # echo "${parent}"
                uper=$(echo ${parent} | tr -d '/sys/bus/usb/devices/' | \
                       tr '.' ',' | tr '-' ':')
                echo "${uper}"
                # convert_syntax "${parent}"
                status=0
            fi
        done
    done
done

exit $status;
