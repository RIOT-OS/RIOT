#!/bin/sh
#
# PyOCD script for RIOT. Only versions >= 0.14.1 are supported.
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables.
#
# The script supports the following actions:
#
# flash:        flash <hex_file>
#               flash given file to the target.
#
#               options:
#               <hex_file>:   Filename of the hex file that will be flashed
#               PYOCD_FLASH_TARGET_TYPE: CPU target type (nrf52, nrf51, etc)
#
# debug:        debug <elfile>
#               starts pyocd-gdbserver as GDB server in the background and
#               connects to the server with the GDB client specified by
#               the board.
#
#               options:
#               <elffile>:      path to the file to debug, must be in a format
#                               recognized by GDB (preferably ELF, it will not
#                               work with .bin, .hex because they lack
#                               symbol information)
#               GDB_PORT:       port opened for GDB connections
#               TELNET_PORT:    port opened for telnet connections
#               DBG:            debugger client command, default: 'gdb -q'
#               TUI:            if TUI!=null, the -tui option will be used
#
# debug-server: starts pyocd-gdbserver as GDB server, but does not connect to
#               to it with any frontend. This might be useful when using
#               IDEs.
#
#               options:
#               GDB_PORT:       port opened for GDB connections
#               TELNET_PORT:    port opened for telnet connections
#
# reset:        triggers a hardware reset of the target board
#
# @author       Alexandre Abadie <alexandre.abadie@inria.fr>
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>
# @author       Joakim Nohlgård <joakim.nohlgard@eistec.se>

# Default GDB port, set to 0 to disable, required != 0 for debug and
# debug-server targets
: ${GDB_PORT:=3333}
# Default telnet port, set to 0 to disable.
: ${TELNET_PORT:=4444}
# Default PyOCD commands.
: ${PYOCD_CMD:=pyocd}
: ${PYOCD_FLASH:=${PYOCD_CMD} flash}
: ${PYOCD_GDBSERVER:=${PYOCD_CMD} gdbserver}
# Debugger interface initialization commands to pass to PyOCD
: ${PYOCD_ADAPTER_INIT:=}
# The setsid command is needed so that Ctrl+C in GDB doesn't kill PyOCD.
: ${SETSID:=setsid}
# GDB command, usually a separate command for each platform (e.g.
# arm-none-eabi-gdb).
: ${GDB:=gdb}
# Debugger client command, can be used to wrap GDB in a front-end.
: ${DBG:=${GDB}}
# Default debugger flags.
: ${DBG_DEFAULT_FLAGS:=-q -ex \"tar ext :${GDB_PORT}\"}
# Extra debugger flags, added by the user.
: ${DBG_EXTRA_FLAGS:=}
# Debugger flags, will be passed to sh -c, remember to escape any quotation
# signs. Use ${DBG_DEFAULT_FLAGS} to insert the default flags anywhere in the
# string.
: ${DBG_FLAGS:=${DBG_DEFAULT_FLAGS} ${DBG_EXTRA_FLAGS}}
# CPU Target type.
# Use `-t` followed by value. Example: -t nrf51
: ${PYOCD_FLASH_TARGET_TYPE:=}
# This is an optional offset to the base address that can be used to flash an
# image in a different location than it is linked at. This feature can be useful
# when flashing images for firmware swapping/remapping boot loaders.
# Default offset is 0, meaning the image will be flashed at the address that it
# was linked at.
: ${IMAGE_OFFSET:=0}

#
# Examples of alternative debugger configurations
#

# Using the GDB text UI:
# DBG_EXTRA_FLAGS=-tui make debug
# or to always use TUI, put in your .profile:
# export DBG_EXTRA_FLAGS=-tui

# Wrapping GDB inside DDD (https://www.gnu.org/software/ddd/)
# DBG=ddd DBG_FLAGS='--debugger "${GDB} ${DBG_DEFAULT_FLAGS}"' make debug
# Alternatively, to always use DDD, put the following in your .profile or similar:
# export DBG=ddd
# export DBG_FLAGS='--debugger "${GDB} ${DBG_DEFAULT_FLAGS}"'
# The single quotes are important on the line above, or it will not work.

#
# a couple of tests for certain configuration options
#
test_elffile() {
    if [ ! -f "${ELFFILE}" ]; then
        echo "Error: Unable to locate ELFFILE"
        echo "       (${ELFFILE})"
        exit 1
    fi
}

test_hexfile() {
    if [ ! -f "${HEX_FILE}" ]; then
        echo "Error: Unable to locate HEX_FILE"
        echo "       (${HEX_FILE})"
        exit 1
    fi
}

#
# now comes the actual actions
#
do_flash() {
    HEX_FILE=$1
    test_hexfile

    if [ "${IMAGE_OFFSET}" != "0" ]; then
        echo "Flashing with IMAGE_OFFSET: ${IMAGE_OFFSET}"
    fi

    # flash device
    sh -c "${PYOCD_FLASH} ${PYOCD_FLASH_TARGET_TYPE} ${PYOCD_ADAPTER_INIT} -a ${IMAGE_OFFSET} \"${HEX_FILE}\"" &&
    echo 'Done flashing'
}

do_debug() {
    ELFFILE=$1
    test_elffile
    # temporary file that saves PyOCD pid
    OCD_PIDFILE=$(mktemp -t "pyocd_pid.XXXXXXXXXX")
    # will be called by trap
    cleanup() {
        OCD_PID="$(cat $OCD_PIDFILE)"
        kill ${OCD_PID}
        rm -f "$OCD_PIDFILE"
        exit 0
    }
    # cleanup after script terminates
    trap "cleanup ${OCD_PIDFILE}" EXIT
    # don't trap on Ctrl+C, because GDB keeps running
    trap '' INT
    # start PyOCD as GDB server
    ${SETSID} sh -c "${PYOCD_GDBSERVER} \
            ${PYOCD_FLASH_TARGET_TYPE} \
            ${PYOCD_ADAPTER_INIT} \
            -p ${GDB_PORT} \
            -T ${TELNET_PORT} & \
            echo \$! > $OCD_PIDFILE" &
    # Export to be able to access these from the sh -c command lines, may be
    # useful when using a frontend for GDB
    export ELFFILE
    export GDB
    export GDB_PORT
    export DBG_FLAGS
    # Start the debugger and connect to the GDB server
    sh -c "${DBG} ${DBG_FLAGS} ${ELFFILE}"
}

do_debugserver() {
    # start PyOCD as GDB server
    sh -c "${PYOCD_GDBSERVER} \
            ${PYOCD_FLASH_TARGET_TYPE} \
            ${PYOCD_ADAPTER_INIT} \
            -p ${GDB_PORT} \
            -T ${TELNET_PORT}"
}

do_reset() {
    # start PyOCD and invoke board reset
    sh -c "${PYOCD_CMD} cmd -c reset ${PYOCD_FLASH_TARGET_TYPE} ${PYOCD_ADAPTER_INIT}"
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
  debug)
    echo "### Starting Debugging ###"
    do_debug "$@"
    ;;
  debug-server)
    echo "### Starting GDB Server ###"
    do_debugserver
    ;;
  reset)
    echo "### Resetting Target ###"
    do_reset
    ;;
  *)
    echo "Usage: $0 {flash|debug|debug-server|reset}"
    exit 2
    ;;
esac
