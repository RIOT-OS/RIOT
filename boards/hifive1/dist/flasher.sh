#!/bin/sh
#
# Former Unified OpenOCD script for RIOT..
# This has been modified to not do "reset" or "reset run" commands while
# flashing since the HiFive1 does not support them.
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables. An OpenOCD
# configuration file must be present in a the boards dist folder.
#
# Any extra command line arguments after the command name are passed on the
# openocd command line after the configuration file name but before any other
# initialization commands.
#
# Global environment variables used:
# OPENOCD:             OpenOCD command name, default: "openocd"
# OPENOCD_CONFIG:      OpenOCD configuration file name,
#                      default: "${RIOTBOARD}/${BOARD}/dist/openocd.cfg"
#
# The script supports the following actions:
#
# flash:        flash <image_file>
#               flash given file to the target.
#
#               options:
#               <image_file>:   Filename of the file that will be flashed
#               OPENOCD_PRE_FLASH_CHECK_SCRIPT: a command to run before flashing to
#               verify the integrity of the image to be flashed. <image_file> is
#               passed as a command line argument to this command.
#
#               Flashing works with any file format recognized by OpenOCD
#               (elf, ihex, s19, bin).
#
#
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>
# @author       Joakim Nohlgård <joakim.nohlgard@eistec.se>

# Default OpenOCD command
: ${OPENOCD:=openocd}
# This is an optional offset to the base address that can be used to flash an
# image in a different location than it is linked at. This feature can be useful
# when flashing images for firmware swapping/remapping boot loaders.
# Default offset is 0, meaning the image will be flashed at the address that it
# was linked at.
: ${IMAGE_OFFSET:=0}
# Type of image, leave empty to let OpenOCD automatically detect the type from
# the file (default).
# Valid values: elf, hex, s19, bin (see OpenOCD manual for more information)
: ${IMAGE_TYPE:=}

#
# a couple of tests for certain configuration options
#
test_config() {
    if [ ! -f "${OPENOCD_CONFIG}" ]; then
        echo "Error: Unable to locate OpenOCD configuration file"
        echo "       (${OPENOCD_CONFIG})"
        exit 1
    fi
    echo "Using OpenOCD configuration file"
    echo "       (${OPENOCD_CONFIG})"
}

test_elffile() {
    if [ ! -f "${EXECUTABLE}" ]; then
        echo "Error: Unable to locate EXECUTABLE"
        echo "       (${EXECUTABLE})"
        exit 1
    fi
}

test_imagefile() {
    if [ ! -f "${IMAGE_FILE}" ]; then
        echo "Error: Unable to locate IMAGE_FILE"
        echo "       (${IMAGE_FILE})"
        exit 1
    fi
}

#
# now comes the actual actions
#
do_flash() {
    IMAGE_FILE=$1
    test_config
    test_imagefile
    if [ -n "${OPENOCD_PRE_FLASH_CHECK_SCRIPT}" ]; then
        sh -c "${OPENOCD_PRE_FLASH_CHECK_SCRIPT} '${IMAGE_FILE}'"
        RETVAL=$?
        if [ $RETVAL -ne 0 ]; then
            echo "pre-flash checks failed, status=$RETVAL"
            exit $RETVAL
        fi
    fi
    # flash device
    echo 'Doing flashing'

    # For some reason 'halt' seems to be more reliable then 'reset halt' at
    # getting the HiFive's attention.
    sh -c "${OPENOCD} -f '${OPENOCD_CONFIG}' \
            -c 'tcl_port disabled' \
            -c 'telnet_port disabled' \
            -c 'gdb_port disabled' \
            -c 'init' \
            -c 'halt' \
            -c 'flash protect 0 64 last off' \
            -c 'flash write_image erase \"${IMAGE_FILE}\" ${IMAGE_OFFSET} ${IMAGE_TYPE}' \
            -c 'verify_image \"${IMAGE_FILE}\"' \
            -c 'reset halt' \
            -c 'reg pc 0x20000000' \
            -c 'resume' \
            -c 'exit'" &&
    echo 'Done flashing'
}

#
# parameter dispatching
#
ACTION="$1"
shift # pop $1 from $@

case "${ACTION}" in
  flash)
    echo "### Flashing Target ###"
    do_flash "$@"
    ;;
  *)
    echo "Usage: $0 flash <flashfile>"
    exit 2
    ;;
esac
