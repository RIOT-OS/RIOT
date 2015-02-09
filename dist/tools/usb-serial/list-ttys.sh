#!/bin/sh

# Copyright (C) 2015 Eistec AB
# Copyright (C) 2015 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser General
# Public License v2.1. See the file LICENSE in the top level directory for more
# details.

if [ ! -d /sys/bus/usb/devices ]; then
    echo "$(basename $0): /sys/bus/usb/devices not a directory (/sys is not mounted?)" >&2
    exit 1
fi

# iterate over usb-tty devices:
for basedev in $(find /sys/bus/usb/devices/ -regex "/sys/bus/usb/devices/[0-9]+[^:/]*" -maxdepth 2 -follow 2>/dev/null); do
    ttydirs=$(find ${basedev} -regex "${basedev}/[^/]*:.*" -mindepth 2 -maxdepth 3 -name tty -follow 2>/dev/null)
    if [ -z "${ttydirs}" ]; then
        continue
    fi
    # See if the device has any tty devices assigned to it.
    ttys=$(find ${ttydirs} -maxdepth 1 -mindepth 1 -printf '%f, ' | sed -e 's/, $/\n/' 2>/dev/null)
    if [ -z "${ttys}" ]; then
        continue
    fi
    # Get all info
    parent=$(echo ${basedev} | sed -e 's%\(/sys/bus/usb/devices/[^/]*\)/.*%\1%')
    serial=$(cat "${parent}/serial" 2>/dev/null)
    manuf=$(cat "${parent}/manufacturer" 2>/dev/null)
    product=$(cat "${parent}/product" 2>/dev/null)
    echo "${parent}: ${manuf} ${product} serial: '${serial}', tty(s): ${ttys}"
done
