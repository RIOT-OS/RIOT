#!/bin/sh

#
# Unified Renode script for RIOT
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables.
#
# It will start the Renode emulator, providing it with several environment
# variables:
# $image_file          Full path to the image file (see $IMAGE_FILE below)
#
# Global environment variables used:
# RENODE:              Renode command name, default: "renode"
# RENODE_CONFIG:       Renode configuration file name,
#                      default: "${RIOTBOARD}/${BOARD}/dist/board.resc"
# RENODE_BIN_CONFIG:   Renode intermediate configuration file name,
#                      default: "${BINDIR}/board.resc"
#
# @author       Bas Stottelaar <basstottelaar@gmail.com>

# Default path to Renode configuration file
: ${RENODE_CONFIG:=${RIOTBOARD}/${BOARD}/dist/board.resc}
# Default path to Renode intermediate configuration file
: ${RENODE_BIN_CONFIG:=${BINDIR}/board.resc}
# Default Renode command
: ${RENODE:=renode}
# Image file used for emulation
# Default is to use $ELFFILE
: ${IMAGE_FILE:=${ELFFILE}}

#
# config test section.
#
test_config() {
    if [ ! -f "${RENODE_CONFIG}" ]; then
        echo "Error: Unable to locate Renode board file"
        echo "       (${RENODE_CONFIG})"
        exit 1
    fi
}

#
# helper section.
#
write_config() {
    echo "\$image_file = '${IMAGE_FILE}'" > "${RENODE_BIN_CONFIG}"
    echo "include @${RENODE_CONFIG}" >> "${RENODE_BIN_CONFIG}"
}

#
# now comes the actual actions
#
do_write() {
    test_config
    write_config
    echo "Script written to '${RENODE_BIN_CONFIG}'"
}

do_start() {
    test_config
    write_config
    sh -c "${RENODE} '${RENODE_BIN_CONFIG}'"
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
  start)
    echo "### Starting Renode ###"
    do_start
    ;;
  *)
    echo "Usage: $0 {write|start}"
    exit 2
    ;;
esac
