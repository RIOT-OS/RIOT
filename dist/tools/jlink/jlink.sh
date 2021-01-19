#!/bin/sh
#
# Unified Segger J-Link script for RIOT
#
# This script is supposed to be called from RIOTs build system,
# as it depends on certain environment variables.
#
# The minimum supported version of J-Link is V6.74.
#
# Global environment variables used:
# JLINK:            J-Link Commander command name, default: "JLinkExe"
# JLINK_SERVER:     J-Link GDB Server command name, default: "JLinkGDBServer"
# JLINK_DEVICE:     Device identifier used by J-Link
# JLINK_SERIAL:     Device serial used by J-Link
# JLINK_IF:         Interface used by J-Link, default: "SWD"
# JLINK_SPEED:      Interface clock speed to use (in kHz), default "2000"
# FLASH_ADDR:       Starting address of the target's flash memory, default: "0"
# IMAGE_OFFSET:     Offset from the targets flash memory, for flashing the
#                   image
# JLINK_PRE_FLASH:  Additional J-Link commands to execute before flashing
# JLINK_POST_FLASH: Additional J-Link commands to execute after flashing
#
# The script supports the following actions:
#
# flash:        flash <binfile>
#
#               flash given binary format file to the target.
#
#               options:
#               <binfile>:      path to the binary file that is flashed
#
# debug:        debug <elffile>
#
#               starts J-Link as GDB server in the background and
#               connects to the server with the GDB client specified by
#               the board (DBG environment variable)
#
#               options:
#               <elffile>:      path to the ELF file to debug
#               GDB_PORT:       port opened for GDB connections
#               TELNET_PORT:    port opened for telnet connections
#               DBG:            debugger client command, default: 'gdb -q'
#               TUI:            if TUI!=null, the -tui option will be used
#
# debug-server: starts J-Link as GDB server, but does not connect to
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
# term-rtt:     opens a serial terminal using J-Link RTT (Real-Time Transfer)
#
#
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>

# Set IMAGE_OFFSET to zero by default.
: ${IMAGE_OFFSET:=0}
# Allow overwriting the reset commands.
: ${JLINK_RESET_FILE:=${RIOTTOOLS}/jlink/reset.seg}

# default GDB port
_GDB_PORT=3333
# default telnet port
_TELNET_PORT=4444
# default J-Link command names, interface and speed
_JLINK=JLinkExe
_JLINK_SERVER=JLinkGDBServer
_JLINK_IF=SWD
_JLINK_SPEED=2000
# default terminal frontend
_JLINK_TERMPROG=${RIOTTOOLS}/pyterm/pyterm
_JLINK_TERMFLAGS="-ts 19021 ${PYTERMFLAGS}"

#
# a couple of tests for certain configuration options
#
test_config() {
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

test_binfile() {
    if [ ! -f "${BINFILE}" ]; then
        echo "Error: Unable to locate BINFILE"
        echo "       (${BINFILE})"
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
        JLINK_SERIAL="-selectemubysn '${JLINK_SERIAL}'"
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

test_version() {
    JLINK_MINIMUM_VERSION="6.74"
    # Adding '-nogui 1' will simply return 'Unknown command line option -nogui'
    # on older versions, JLINK_VERSION will still be parsed correctly.
    JLINK_VERSION=$(echo q | "${JLINK}" -nogui 1 2> /dev/null | grep "^DLL version*" | grep -oE "[0-9]+\.[0-9]+")

    if [ $? -ne 0 ]; then
        echo "Error: J-Link appears not to be installed on your PATH"
        exit 1
    fi

    "$RIOTTOOLS"/has_minimal_version/has_minimal_version.sh "$JLINK_VERSION" "$JLINK_MINIMUM_VERSION" 2> /dev/null

    if [ $? -ne 0 ]; then
        echo "Error: J-Link V$JLINK_MINIMUM_VERSION is required, but V${JLINK_VERSION} is installed"
        exit 1
    fi
}

#
# now comes the actual actions
#
do_flash() {
    BINFILE=$1
    test_config
    test_version
    test_serial
    test_binfile
    # clear any existing contents in burn file
    /bin/echo -n "" > ${BINDIR}/burn.seg
    # create temporary burn file
    if [ ! -z "${JLINK_PRE_FLASH}" ]; then
        printf "${JLINK_PRE_FLASH}\n" >> ${BINDIR}/burn.seg
    fi
    # address to flash is hex formatted, as required by JLink
    ADDR_TO_FLASH=$(printf "0x%08x\n" "$((${FLASH_ADDR} + ${IMAGE_OFFSET}))")
    echo "loadbin ${BINFILE} ${ADDR_TO_FLASH}" >> ${BINDIR}/burn.seg
    if [ ! -z "${JLINK_POST_FLASH}" ]; then
        printf "${JLINK_POST_FLASH}\n" >> ${BINDIR}/burn.seg
    fi
    cat ${JLINK_RESET_FILE} >> ${BINDIR}/burn.seg
    # flash device
    sh -c "${JLINK} ${JLINK_SERIAL} \
                    -nogui 1 \
                    -exitonerror 1 \
                    -device '${JLINK_DEVICE}' \
                    -speed '${JLINK_SPEED}' \
                    -if '${JLINK_IF}' \
                    -jtagconf -1,-1 \
                    -commandfile '${BINDIR}/burn.seg'"
}

do_debug() {
    ELFFILE=$1
    test_config
    test_version
    test_serial
    test_elffile
    test_ports
    test_tui
    test_dbg
    # start the J-Link GDB server
    sh -c "${JLINK_SERVER} ${JLINK_SERIAL_SERVER} \
                           -nogui \
                           -silent \
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
    test_config
    test_version
    test_ports
    test_serial
    # start the J-Link GDB server
    sh -c "${JLINK_SERVER} ${JLINK_SERIAL_SERVER} \
                           -nogui \
                           -device '${JLINK_DEVICE}' \
                           -speed '${JLINK_SPEED}' \
                           -if '${JLINK_IF}' \
                           -port '${GDB_PORT}' \
                           -telnetport '${TELNET_PORT}'"
}

do_reset() {
    test_config
    test_version
    test_serial
    # reset the board
    sh -c "${JLINK} ${JLINK_SERIAL} \
                    -nogui 1 \
                    -exitonerror 1 \
                    -device '${JLINK_DEVICE}' \
                    -speed '${JLINK_SPEED}' \
                    -if '${JLINK_IF}' \
                    -jtagconf -1,-1 \
                    -commandfile '${JLINK_RESET_FILE}'"
}

do_term() {
    test_config
    test_version
    test_serial
    test_term

    # temporary file that save the J-Link Commander pid
    JLINK_PIDFILE=$(mktemp -t "jilnk_pid.XXXXXXXXXX")
    # will be called by trap
    cleanup() {
        if [ -f $JLINK_PIDFILE ]; then
            JLINK_PID="$(cat ${JLINK_PIDFILE})"
            kill ${JLINK_PID}
            rm -r "${JLINK_PIDFILE}"
        fi
        exit 0
    }
    # cleanup after script terminates
    trap "cleanup ${JLINK_PIDFILE}" EXIT INT

    # start J-link as RTT server
    sh -c "${JLINK} ${JLINK_SERIAL} \
            -nogui 1 \
            -exitonerror 1 \
            -device '${JLINK_DEVICE}' \
            -speed '${JLINK_SPEED}' \
            -if '${JLINK_IF}' \
            -jtagconf -1,-1 \
            -commandfile '${RIOTTOOLS}/jlink/term.seg' >/dev/null & \
            echo  \$! > $JLINK_PIDFILE" &
    sleep 1

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
    echo "### Flashing at base address ${FLASH_ADDR} with offset ${IMAGE_OFFSET} ###"
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
  term-rtt)
    echo "### Starting RTT terminal ###"
    do_term
    ;;
  *)
    echo "Usage: $0 {flash|debug|debug-server|reset|term-rtt}"
    echo "          flash <binfile>"
    echo "          debug <elffile>"
    ;;
esac
