#!/bin/sh
#
# Unified Segger JLink script for RIOT
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables. An
#
# Global environment variables used:
# JLINK:            JLink command name, default: "JLinkExe"
# JLINK_SERVER:     JLink GCB server command name, default: "JLinkGDBDerver"
# JLINK_DEVICE:     Device identifier used by JLink
# JLINK_SERIAL:     Device serial used by JLink
# JLINK_IF:         Interface used by JLink, default: "SWD"
# JLINK_SPEED:      Interface clock speed to use (in kHz), default "2000"
# FLASH_ADDR:       Starting address of the target's flash memory, default: "0"
# JLINK_PRE_FLASH:  Additional JLink commands to execute before flashing
# JLINK_POST_FLASH: Additional JLink commands to execute after flashing
#
# The script supports the following actions:
#
# flash:        flash a given hex file to the target.
#               hexfile is expected in ihex format and is pointed to
#               by BINFILE environment variable
#
#               options:
#               BINFILE: path to the binary file that is flashed
#
# debug:        starts JLink as GDB server in the background and
#               connects to the server with the GDB client specified by
#               the board (DBG environment variable)
#
#               options:
#               GDB_PORT:       port opened for GDB connections
#               TELNET_PORT:    port opened for telnet connections
#               DBG:            debugger client command, default: 'gdb -q'
#               TUI:            if TUI!=null, the -tui option will be used
#               ELFFILE:        path to the ELF file to debug
#
# debug-server: starts JLink as GDB server, but does not connect to
#               to it with any frontend. This might be useful when using
#               IDEs.
#
#               options:
#               GDB_PORT:       port opened for GDB connections
#               TELNET_PORT:    port opened for telnet connections
#
# reset:        triggers a hardware reset of the target board
#
#
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>

# default GDB port
_GDB_PORT=3333
# default telnet port
_TELNET_PORT=4444
# default JLink command, interface and speed
_JLINK=JLinkExe
_JLINK_SERVER=JLinkGDBServer
_JLINK_IF=SWD
_JLINK_SPEED=2000
# default terminal frontend
_JLINK_TERMPROG=${RIOTTOOLS}/pyterm/pyterm
_JLINK_TERMFLAGS="-ts 19021"

#
# a couple of tests for certain configuration options
#
test_config() {
    if [ -z "${HEXFILE}" ]; then
        echo "no hexfile"
    else
        echo "HEXFILE found"
    fi

    if [ -z "${JLINK}" ]; then
        JLINK=${_JLINK}
    fi
    if [ -z "${JLINK_SERVER}" ]; then
        JLINK_SERVER=${_JLINK_SERVER}
    fi
    if [ -z "${JLINK_IF}" ]; then
        JLINK_IF=${_JLINK_IF}
    fi
    if [ -z "${JLINK_SPEED}" ]; then
        JLINK_SPEED=${_JLINK_SPEED}
    fi
    if [ -z "${JLINK_DEVICE}" ]; then
        echo "Error: No target device defined in JLINK_DEVICE env var"
        exit 1
    fi
    if [ -z "${FLASH_ADDR}" ]; then
        echo "Error: No flash address defined in FLASH_ADDR env var"
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

test_ports() {
    if [ -z "${GDB_PORT}" ]; then
        GDB_PORT=${_GDB_PORT}
    fi
    if [ -z "${TELNET_PORT}" ]; then
        TELNET_PORT=${_TELNET_PORT}
    fi
}

test_elffile() {
    if [ ! -f "${ELFFILE}" ]; then
        echo "Error: Unable to locate ELFFILE"
        echo "       (${ELFFILE})"
        exit 1
    fi
}

test_tui() {
    if [ -n "${TUI}" ]; then
        TUI=-tui
    fi
}

test_serial() {
    if [ -n "${JLINK_SERIAL}" ]; then
        JLINK_SERIAL_SERVER="-select usb='${JLINK_SERIAL}'"
        JLINK_SERIAL="-SelectEmuBySN '${JLINK_SERIAL}'"
    fi
}

test_dbg() {
    if [ -z "${DBG}" ]; then
        DBG="${GDB}"
    fi
}

test_term() {
    if [ -z "${JLINK_TERMPROG}" ]; then
        JLINK_TERMPROG="${_JLINK_TERMPROG}"
    fi
    if [ -z "${JLINK_TERMFLAGS}" ]; then
        JLINK_TERMFLAGS="${_JLINK_TERMFLAGS}"
    fi
}

#
# now comes the actual actions
#
do_flash() {
    test_config
    test_serial
    test_hexfile
    # clear any existing contents in burn file
    /bin/echo -n "" > ${BINDIR}/burn.seg
    # create temporary burn file
    if [ ! -z "${JLINK_PRE_FLASH}" ]; then
        printf "${JLINK_PRE_FLASH}\n" >> ${BINDIR}/burn.seg
    fi
    echo "loadbin ${HEXFILE} ${FLASH_ADDR}" >> ${BINDIR}/burn.seg
    if [ ! -z "${JLINK_POST_FLASH}" ]; then
        printf "${JLINK_POST_FLASH}\n" >> ${BINDIR}/burn.seg
    fi
    cat ${RIOTTOOLS}/jlink/reset.seg >> ${BINDIR}/burn.seg
    # flash device
    sh -c "${JLINK} ${JLINK_SERIAL} \
                    -device '${JLINK_DEVICE}' \
                    -speed '${JLINK_SPEED}' \
                    -if '${JLINK_IF}' \
                    -jtagconf -1,-1 \
                    -commandfile '${BINDIR}/burn.seg'"
}

do_debug() {
    test_config
    test_serial
    test_elffile
    test_ports
    test_tui
    test_dbg
    # start the JLink GDB server
    sh -c "${JLINK_SERVER} ${JLINK_SERIAL_SERVER} \
                           -device '${JLINK_DEVICE}' \
                           -speed '${JLINK_SPEED}' \
                           -if '${JLINK_IF}' \
                           -port '${GDB_PORT}' \
                           -telnetport '${TELNET_PORT}'" &
    # save PID for terminating the server afterwards
    DBG_PID=$?
    # connect to the GDB server
    ${DBG} -q ${TUI} -ex "tar ext :${GDB_PORT}" ${ELFFILE}
    # clean up
    kill ${DBG_PID}
}

do_debugserver() {
    test_ports
    test_config
    test_serial
    # start the JLink GDB server
    sh -c "${JLINK_SERVER} ${JLINK_SERIAL_SERVER} \
                           -device '${JLINK_DEVICE}' \
                           -speed '${JLINK_SPEED}' \
                           -if '${JLINK_IF}' \
                           -port '${GDB_PORT}' \
                           -telnetport '${TELNET_PORT}'"
}

do_reset() {
    test_config
    test_serial
    # reset the board
    sh -c "${JLINK} ${JLINK_SERIAL} \
                    -device '${JLINK_DEVICE}' \
                    -speed '${JLINK_SPEED}' \
                    -if '${JLINK_IF}' \
                    -jtagconf -1,-1 \
                    -commandfile '${RIOTTOOLS}/jlink/reset.seg'"
}

do_term() {
    test_config
    test_serial
    test_term

    # temporary file that save the JLink pid
    JLINK_PIDFILE=$(mktemp -t "jilnk_pid.XXXXXXXXXX")
    # will be called by trap
    cleanup() {
        JLINK_PID="$(cat ${JLINK_PIDFILE})"
        kill ${JLINK_PID}
        rm -r "${JLINK_PIDFILE}"
        exit 0
    }
    # cleanup after script terminates
    trap "cleanup ${JLINK_PIDFILE}" EXIT
    # don't trapon Ctrl+C, because JLink keeps running
    trap '' INT
    # start Jlink as RTT server
    setsid sh -c "${JLINK} ${JLINK_SERIAL} \
            -device '${JLINK_DEVICE}' \
            -speed '${JLINK_SPEED}' \
            -if '${JLINK_IF}' \
            -jtagconf -1,-1 \
            -commandfile '${RIOTTOOLS}/jlink/term.seg' & \
            echo  \$! > $JLINK_PIDFILE" &

    sh -c "${JLINK_TERMPROG} ${JLINK_TERMFLAGS}"
}

#
# parameter dispatching
#
ACTION="$1"
shift # pop $1 from $@

case "${ACTION}" in
  flash)
    echo "### Flashing Target ###"
    echo "### Flashing at address ${FLASH_ADDR} ###"
    do_flash "$@"
    ;;
  debug)
    echo "### Starting Debugging ###"
    do_debug "$@"
    ;;
  debug-server)
    echo "### Starting GDB Server ###"
    do_debugserver "$@"
    ;;
  reset)
    echo "### Resetting Target ###"
    do_reset "$@"
    ;;
  term_rtt)
    echo "### Starting RTT terminal ###"
    do_term
    ;;
  *)
    echo "Usage: $0 {flash|debug|debug-server|reset}"
    ;;
esac
