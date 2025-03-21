#! /usr/bin/env bash

# Copyright 2025 Technische Universität Hamburg <christopher.buechse@tuhh.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# The UF2 bootloader as well as UF2 compatible bootloaders provide a
# file called "INFO_UF2.TXT" in their mass storage filesystem. This gives
# information about the version of the bootloader as well as the installed
# SoftDevice for the Adafruit nRF52 Bootloader.
# The aim of this script is to check the currently installed SoftDevice
# for existence and match the version numbers to avoid any unwanted behavior.

: "${RIOTBASE:=$(cd "$(dirname "$0")"/../../../ || exit; pwd)}"
: "${RIOTTOOLS:=${RIOTBASE}/dist/tools}"

# The uf2conv script lists all currently mounted devices with UF2 bootloader
MOUNTPOINT=$("${RIOTTOOLS}"/uf2/uf2conv.py --list | cut -d ' ' -f1)

if [ -z "${MOUNTPOINT}" ]; then
    echo "No device with UF2 bootloader found!"
    exit 1
fi

INFO_FILE="${MOUNTPOINT}/INFO_UF2.TXT"

if [ ! -f "${INFO_FILE}" ]; then
    echo "No INFO_FILE.TXT found in UF2 Mass Storage Device!"
    exit 1
fi

# The format of the SoftDevice info line is "SoftDevice: S140 6.1.1" or
# "SoftDevice: not found"
SOFTDEVICE=$(grep "^SoftDevice" "${INFO_FILE}" | cut -d ' ' -f2- | tr -d '\r')

if [ "${SOFTDEVICE}" != "not found" ] && [ "${UF2_SOFTDEV}" = "DROP" ]; then
    echo -e "\033[0;31mYou have the UF2_SOFTDEV=DROP option set. This will"\
    "override any currently installed SoftDevice on your board."
    echo -e "Press enter to continue or CTRL-C to abort.\033[0m"
    read -r
fi

if [ "${SOFTDEVICE}" = "not found" ] && [ "${UF2_SOFTDEV}" != "DROP" ]; then
    echo -e "\033[0;31mNo SoftDevice present on the device, but the compilation is set" \
    "to expect a SoftDevice."
    echo -e "For more information see:" \
    "https://doc.riot-os.org/group__boards__common__adafruit-nrf52-bootloader.html#ada-nrf52-update"\
    "\033[0m"
    exit 1
fi

# Extract the actual SoftDevice version and compare it to the requested one
ACT_SOFTDEV_VER=$(echo "${SOFTDEVICE}" | cut -d ' ' -f2 | tr -d '.')
REQ_SOFTDEV_VER=${UF2_SOFTDEV//SD/}

if [ "${ACT_SOFTDEV_VER}" != "${REQ_SOFTDEV_VER}" ] && [ "${UF2_SOFTDEV}" != "DROP" ]; then
    echo -e "\033[0;31mThe SoftDevice version set in the board definitions"\
    "(${REQ_SOFTDEV_VER}) does not match the version present on the device"\
    "(${ACT_SOFTDEV_VER}). Please review the board settings.\033[0m"
    exit 1
fi

if [ "${SOFTDEVICE}" != "not found" ]; then
    echo "SoftDevice version ${SOFTDEVICE} found."
fi

exit 0
