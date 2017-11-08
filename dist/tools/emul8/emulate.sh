#!/bin/sh

#
# Unified Emul8 script for RIOT
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables.
#
# It will start the Emul8 emulator, providing it with several environment
# variables:
# $image_file          Full path to the image file (see $IMAGE_FILE below)
#
# Global environment variables used:
# EMUL8:               Emul8 command name, default: "emul8"
# EMUL8_CONFIG:        Emul8 configuration file name,
#                      default: "${RIOTBOARD}/${BOARD}/dist/board.emul8"
# EMUL8_BIN_CONFIG:    Emul8 intermediate configuration file name,
#                      default: "${BINDIR}/board.emul8"
#
# @author       Bas Stottelaar <basstottelaar@gmail.com>

# Default path to Emul8 configuration file
: ${EMUL8_CONFIG:=${RIOTBOARD}/${BOARD}/dist/board.emul8}
# Default path to Emul8 intermediate configuration file
: ${EMUL8_BIN_CONFIG:=${BINDIR}/board.emul8}
# Default emul8 command
: ${EMUL8:=emul8}
# Image file used for emulation
# Default is to use $ELFFILE
: ${IMAGE_FILE:=${ELFFILE}}

#
# config test section.
#
test_config() {
    if [ ! -f "${EMUL8_CONFIG}" ]; then
        echo "Error: Unable to locate Emul8 board file"
        echo "       (${EMUL8_CONFIG})"
        exit 1
    fi
}

#
# helper section.
#
write_config() {
    echo "\$image_file = '${IMAGE_FILE}'" > "${EMUL8_BIN_CONFIG}"
    echo "include @${EMUL8_CONFIG}" >> "${EMUL8_BIN_CONFIG}"
}

#
# now comes the actual actions
#
do_write() {
    test_config
    write_config
    echo "Script written to '${EMUL8_BIN_CONFIG}'"
}

do_emulate() {
    test_config
    write_config
    sh -c "${EMUL8} '${EMUL8_BIN_CONFIG}'"
}

#
# parameter dispatching
#
ACTION="$1"

case "${ACTION}" in
  write)
    echo "### Writing emulation script ###"
    do_write
    ;;
  emulate)
    echo "### Starting emulation ###"
    do_emulate
    ;;
  *)
    echo "Usage: $0 {write|emulate}"
    exit 2
    ;;
esac
