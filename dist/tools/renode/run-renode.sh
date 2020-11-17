#!/bin/sh

#
# Unified Renode script for RIOT
#
# This script is supposed to be called from RIOT's make system, as it depends
# on certain environment variables.
#
# The minimum supported version of Renode is V1.12.
#
# By default, it expects to find "dist/board.resc" in the board folder. This
# file is the entry point for emulation.
#
# It will start the Renode emulator, providing it with several environment
# variables:
# $BOARD               Selected board name
# $CPU                 Selected CPU name
# $ELFFILE             Full path to the image file
# $RIOTBASE            Full path to RIOT-OS directory
# $RIOTBOARD           Full path to board directory
# $RIOTCPU             Full path to CPU directory
#
# Global environment variables used:
# RENODE:              Renode command name, default: "renode"
# RENODE_BOARD_CONFIG: Renode configuration file name,
#                      default: "${BOARDSDIR}/${BOARD}/dist/board.resc"
# RENODE_BIN_CONFIG:   Renode intermediate configuration file name,
#                      default: "${BINDIR}/board.resc"
#
# @author       Bas Stottelaar <basstottelaar@gmail.com>

# Default path to Renode configuration file
: ${RENODE_BOARD_CONFIG:=${RIOTBOARD}/${BOARD}/dist/renode/board.resc}
# Default path to Renode intermediate configuration file
: ${RENODE_BIN_CONFIG:=${BINDIR}/renode.resc}
# Default Renode command
: ${RENODE:=renode}

#
# config test section.
#
test_config() {
    if [ ! -f "${RENODE_BOARD_CONFIG}" ]; then
        echo "Error: Unable to locate Renode board file"
        echo "       (${RENODE_BOARD_CONFIG})"
        exit 1
    fi
}

test_version() {
    RENODE_MINIMUM_VERSION="1.12"

    # Depends on https://github.com/renode/renode/issues/154
    #RENODE_VERSION=$(${RENODE} --version > /dev/null)
    RENODE_VERSION="1.12"

    if [ $? -ne 0 ]; then
        echo "Error: Renode appears not to be installed on your PATH"
        exit 1
    fi

    "$RIOTTOOLS"/has_minimal_version/has_minimal_version.sh "$RENODE_VERSION" "$RENODE_MINIMUM_VERSION" 2> /dev/null

    if [ $? -ne 0 ]; then
        echo "Error: Renode V$RENODE_MINIMUM_VERSION is required, but V${RENODE_VERSION} is installed"
        exit 1
    fi
}

#
# helper section.
#
write_config() {
    echo "# RIOT-OS variables for Renode emulation" > "${RENODE_BIN_CONFIG}"
    echo "\$BOARD = '${BOARD}'" >> "${RENODE_BIN_CONFIG}"
    echo "\$CPU = '${CPU}'" >> "${RENODE_BIN_CONFIG}"
    echo "\$ELFFILE = @${ELFFILE}" >> "${RENODE_BIN_CONFIG}"
    echo "\$RIOTBASE = @${RIOTBASE}" >> "${RENODE_BIN_CONFIG}"
    echo "\$RIOTBOARD = @${RIOTBOARD}" >> "${RENODE_BIN_CONFIG}"
    echo "\$RIOTCPU = @${RIOTCPU}" >> "${RENODE_BIN_CONFIG}"
    echo "include @${RENODE_BOARD_CONFIG}" >> "${RENODE_BIN_CONFIG}"
}

write_warning() {
  echo "Note: The emulation experience depends on several factors, such as "
  echo "the application, used peripherals and completeness of the "
  echo "implementation of the target in Renode. Check the documentation of "
  echo "Renode at https://renode.readthedocs.io/ for information."
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
    test_version
    write_config
    write_warning
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
