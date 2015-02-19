#!/bin/sh
#
# Unified OpenOCD script for RIOT
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables. An OpenOCD
# configuration file must be present in a the boards dist folder.
#
# Global environment variables used:
# OPENOCD:             OpenOCD command name, default: "openocd"
# OPENOCD_CONFIG:      OpenOCD configuration file name,
#                      default: "${RIOTBOARD}/${BOARD}/dist/openocd.cfg"
# OPENOCD_EXTRA_INIT:  extra parameters to pass on the OpenOCD command line
#                      before any initialization commands, default: (empty)
#
# The script supports the following actions:
#
# flash:        flash a given hexfile to the target.
#               hexfile is expected in ihex format and is pointed to
#               by HEXFILE environment variable
#
#               options:
#               HEXFILE: path to the hexfile that is flashed
#
# debug:        starts OpenOCD as GDB server in the background and
#               connects to the server with the GDB client specified by
#               the board (DBG environment variable)
#
#               options:
#               GDB_PORT:       port opened for GDB connections
#               TCL_PORT:       port opened for TCL connections
#               TELNET_PORT:    port opened for telnet connections
#               TUI:            if TUI!=null, the -tui option will be used
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
#
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>
# @author       Joakim Gebart <joakim.gebart@eistec.se>

# default GDB port
_GDB_PORT=3333
# default telnet port
_TELNET_PORT=4444
# default TCL port
_TCL_PORT=6333
# default path to OpenOCD configuration file
_OPENOCD_CONFIG=${RIOTBOARD}/${BOARD}/dist/openocd.cfg
# default OpenOCD command
_OPENOCD=openocd

#
# a couple of tests for certain configuration options
#
test_config() {
    if [ -z "${OPENOCD}" ]; then
        OPENOCD=${_OPENOCD}
    fi
    if [ -z "${OPENOCD_CONFIG}" ]; then
        OPENOCD_CONFIG=${_OPENOCD_CONFIG}
    fi
    if [ ! -f "${OPENOCD_CONFIG}" ]; then
        echo "Error: Unable to locate OpenOCD configuration file"
        echo "       (${OPENOCD_CONFIG})"
        exit 1
    fi
}

test_hexfile() {
    if [ ! -f "${HEXFILE}" ]; then
        echo "Error: Unable to locate HEXFILE"
        echo "       (${HEXFILE})"
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

test_ports() {
    if [ -z "${GDB_PORT}" ]; then
        GDB_PORT=${_GDB_PORT}
    fi
    if [ -z "${TELNET_PORT}" ]; then
        TELNET_PORT=${_TELNET_PORT}
    fi
    if [ -z "${TCL_PORT}" ]; then
        TCL_PORT=${_TCL_PORT}
    fi
}

test_tui() {
    if [ -n "${TUI}" ]; then
        TUI=-tui
    fi
}

#
# now comes the actual actions
#
do_flash() {
    test_config
    test_hexfile
    # flash device
    ${OPENOCD} -f "${OPENOCD_CONFIG}" \
            ${OPENOCD_EXTRA_INIT} \
            -c "tcl_port 0" \
            -c "telnet_port 0" \
            -c "gdb_port 0" \
            -c "init" \
            -c "targets" \
            -c "reset halt" \
            -c "program ${HEXFILE} verify" \
            -c "reset run" \
            -c "shutdown"
}

do_debug() {
    test_config
    test_elffile
    test_ports
    test_tui
    # start OpenOCD as GDB server
    ${OPENOCD} -f "${OPENOCD_CONFIG}" \
            ${OPENOCD_EXTRA_INIT} \
            -c "tcl_port ${TCL_PORT}" \
            -c "telnet_port ${TELNET_PORT}" \
            -c "gdb_port ${GDB_PORT}" \
            -c "init" \
            -c "targets" \
            -c "reset halt" \
            -l /dev/null &
    # save PID for terminating the server afterwards
    OCD_PID=$?
    # connect to the GDB server
    ${DBG} ${TUI} -ex "tar ext :${GDB_PORT}" ${ELFFILE}
    # clean up
    kill ${OCD_PID}
}

do_debugserver() {
    test_config
    test_ports
    # start OpenOCD as GDB server
    ${OPENOCD} -f "${OPENOCD_CONFIG}" \
            ${OPENOCD_EXTRA_INIT} \
            -c "tcl_port ${TCL_PORT}" \
            -c "telnet_port ${TELNET_PORT}" \
            -c "gdb_port ${GDB_PORT}" \
            -c "init" \
            -c "targets" \
            -c "reset halt"
}

do_reset() {
    test_config
    # start OpenOCD and invoke board reset
    ${OPENOCD} -f "${OPENOCD_CONFIG}" \
            ${OPENOCD_EXTRA_INIT} \
            -c "tcl_port 0" \
            -c "telnet_port 0" \
            -c "gdb_port 0" \
            -c "init" \
            -c "reset run" \
            -c "shutdown"
}

#
# parameter dispatching
#
case "$1" in
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
    ;;
esac
