#!/bin/sh
#
# Unified OpenOCD script for RIOT
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
# flash:        flash a given ELF file to the target.
#
#               options:
#               IMAGE_FILE: Filename of the file that will be flashed
#               PRE_FLASH_CHECK_SCRIPT: a command to run before flashing to
#               verify the integrity of the image to be flashed. ELFFILE is
#               passed as a command line argument to this command.
#               Even though the file name variable is named ELFFILE, flashing
#               works with any file format recognized by OpenOCD (elf, ihex, s19, bin).
#
# debug:        starts OpenOCD as GDB server in the background and
#               connects to the server with the GDB client specified by
#               the board
#
#               options:
#               GDB_PORT:       port opened for GDB connections
#               TCL_PORT:       port opened for TCL connections
#               TELNET_PORT:    port opened for telnet connections
#               DBG:            debugger client command, default: 'gdb -q'
#               TUI:            if TUI!=null, the -tui option will be used
#               ELFFILE:        path to the file to debug, must be in a format
#                               recognized by GDB (preferably ELF, it will not
#                               work with .bin, .hex or .s19 because they lack
#                               symbol information)
#
# debug-server: starts OpenOCD as GDB server, but does not connect to
#               to it with any frontend. This might be useful when using
#               IDEs.
#
#               options:
#               GDB_PORT:       port opened for GDB connections
#               TCL_PORT:       port opened for TCL connections
#               TELNET_PORT:    port opened for telnet connections
#
# reset:        triggers a hardware reset of the target board
#
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>
# @author       Joakim Nohlgård <joakim.nohlgard@eistec.se>

# Default GDB port, set to 0 to disable, required != 0 for debug and debug-server targets
: ${GDB_PORT:=3333}
# Default telnet port, set to 0 to disable
: ${TELNET_PORT:=4444}
# Default TCL port, set to 0 to disable
: ${TCL_PORT:=6333}
# Default path to OpenOCD configuration file
: ${OPENOCD_CONFIG:=${RIOTBOARD}/${BOARD}/dist/openocd.cfg}
# Default OpenOCD command
: ${OPENOCD:=openocd}
# Extra board initialization commands to pass to OpenOCD
: ${OPENOCD_EXTRA_INIT:=}
# Debugger interface initialization commands to pass to OpenOCD
: ${OPENOCD_ADAPTER_INIT:=}
# The setsid command is needed so that Ctrl+C in GDB doesn't kill OpenOCD
: ${SETSID:=setsid}
# GDB command, usually a separate command for each platform (e.g. arm-none-eabi-gdb)
: ${GDB:=gdb}
# Debugger client command, can be used to wrap GDB in a front-end
: ${DBG:=${GDB}}
# Default debugger flags,
: ${DBG_DEFAULT_FLAGS:=-q -ex \"tar ext :${GDB_PORT}\"}
# Extra debugger flags, added by the user
: ${DBG_EXTRA_FLAGS:=}
# Debugger flags, will be passed to sh -c, remember to escape any quotation signs.
# Use ${DBG_DEFAULT_FLAGS} to insert the default flags anywhere in the string
: ${DBG_FLAGS:=${DBG_DEFAULT_FLAGS} ${DBG_EXTRA_FLAGS}}
# Initial target state when using debug, by default a 'halt' request is sent to
# the target when starting a debug session. 'reset halt' can also be used
# depending on the type of target.
: ${OPENOCD_DBG_START_CMD:=-c 'halt'}
# This is an optional offset to the base address that can be used to flash an
# image in a different location than it is linked at. This feature can be useful
# when flashing images for firmware swapping/remapping boot loaders.
# Default offset is 0, meaning the image will be flashed at the address that it
# was linked at.
: ${IMAGE_OFFSET:=0}
# Image file used for flashing. Must be in a format that OpenOCD can handle (ELF,
# Intel hex, S19, or raw binary)
# Default is to use $ELFFILE
: ${IMAGE_FILE:=${ELFFILE}}
# Type of image, leave empty to let OpenOCD automatically detect the type from
# the file (default).
# Valid values: elf, hex, s19, bin (see OpenOCD manual for more information)
: ${IMAGE_TYPE:=}

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
test_config() {
    if [ ! -f "${OPENOCD_CONFIG}" ]; then
        echo "Error: Unable to locate OpenOCD configuration file"
        echo "       (${OPENOCD_CONFIG})"
        exit 1
    fi
}

test_elffile() {
    if [ ! -f "${ELFFILE}" ]; then
        echo "Error: Unable to locate ELFFILE"
        echo "       (${ELFFILE})"
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
    test_config
    test_imagefile
    if [ -n "${PRE_FLASH_CHECK_SCRIPT}" ]; then
        sh -c "${PRE_FLASH_CHECK_SCRIPT} '${IMAGE_FILE}'"
        RETVAL=$?
        if [ $RETVAL -ne 0 ]; then
            echo "pre-flash checks failed, status=$RETVAL"
            exit $RETVAL
        fi
    fi
    # flash device
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            -c 'tcl_port 0' \
            -c 'telnet_port 0' \
            -c 'gdb_port 0' \
            -c 'init' \
            -c 'targets' \
            -c 'reset halt' \
            ${OPENOCD_PRE_FLASH_CMDS} \
            -c 'flash write_image erase \"${IMAGE_FILE}\" ${IMAGE_OFFSET} ${IMAGE_TYPE}' \
            ${OPENOCD_PRE_VERIFY_CMDS} \
            -c 'verify_image \"${IMAGE_FILE}\"' \
            -c 'reset run' \
            -c 'shutdown'" &&
    echo 'Done flashing'
}

do_debug() {
    test_config
    test_elffile
    # temporary file that saves OpenOCD pid
    OCD_PIDFILE=$(mktemp -t "openocd_pid.XXXXXXXXXX")
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
    # start OpenOCD as GDB server
    ${SETSID} sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            -c 'tcl_port ${TCL_PORT}' \
            -c 'telnet_port ${TELNET_PORT}' \
            -c 'gdb_port ${GDB_PORT}' \
            -c 'init' \
            -c 'targets' \
            ${OPENOCD_DBG_START_CMD} \
            -l /dev/null & \
            echo \$! > $OCD_PIDFILE" &
    # Export to be able to access these from the sh -c command lines, may be
    # useful when using a frontend for GDB
    export ELFFILE
    export GDB
    export GDB_PORT
    export DBG_FLAGS
    export DBG_DEFAULT_FLAGS
    export DBG_EXTRA_FLAGS
    # Start the debugger and connect to the GDB server
    sh -c "${DBG} ${DBG_FLAGS} ${ELFFILE}"
}

do_debugserver() {
    test_config
    # start OpenOCD as GDB server
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            -c 'tcl_port ${TCL_PORT}' \
            -c 'telnet_port ${TELNET_PORT}' \
            -c 'gdb_port ${GDB_PORT}' \
            -c 'init' \
            -c 'targets' \
            -c 'halt'"
}

do_reset() {
    test_config
    # start OpenOCD and invoke board reset
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            -c 'tcl_port 0' \
            -c 'telnet_port 0' \
            -c 'gdb_port 0' \
            -c 'init' \
            -c 'reset run' \
            -c 'shutdown'"
}

#
# parameter dispatching
#
ACTION="$1"

case "${ACTION}" in
  flash)
    echo "### Flashing Target ###"
    do_flash
    ;;
  debug)
    echo "### Starting Debugging ###"
    do_debug
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
