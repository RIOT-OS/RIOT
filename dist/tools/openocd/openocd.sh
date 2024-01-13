#!/usr/bin/env bash
#
# Unified OpenOCD script for RIOT
#
# This script is supposed to be called from RIOTs make system,
# as it depends on certain environment variables. An OpenOCD
# configuration file must be present in a the boards dist folder
# or be given as "board/[...].cfg" to use an OpenOCD shipped configuration.
#
# Any extra command line arguments after the command name are passed on the
# openocd command line after the configuration file name but before any other
# initialization commands.
#
# Global environment variables used:
# OPENOCD:             OpenOCD command name, default: "openocd"
#                      Care must be taken when specifying an OpenOCD version in
#                      its build directory, as it does not look up its own
#                      configuration files relative to the executable -- the
#                      scripts directory needs to be passed in like this:
#                      `OPENOCD="~/openocd/src/openocd -s ~/openocd/tcl"`.
# OPENOCD_CONFIG:      OpenOCD configuration file name,
#                      default: "${BOARDSDIR}/${BOARD}/dist/openocd.cfg"
# OPENOCD_SERVER_ADDRESS: OpenOCD server bind address, default: "localhost"
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
# debug:        debug <elfile>
#               starts OpenOCD as GDB server in the background and
#               connects to the server with the GDB client specified by
#               the board
#
#               options:
#               <elffile>:      path to the file to debug, must be in a format
#                               recognized by GDB (preferably ELF, it will not
#                               work with .bin, .hex or .s19 because they lack
#                               symbol information)
#               GDB_PORT:       port opened for GDB connections
#               TCL_PORT:       port opened for TCL connections
#               TELNET_PORT:    port opened for telnet connections
#               DBG:            debugger client command, default: 'gdb -q'
#               TUI:            if TUI!=null, the -tui option will be used
# debug-client: debug-client <elffile>
#               connects to a running debug-server
#               GDB_PORT:       port opened for GDB connections
#               DBG:            debugger client command, default: 'gdb -q'
#               TUI:            if TUI!=null, the -tui option will be used
#
# debugr:       debug <elfile>
#               debug given file on the target but flash it first directly
#               in RAM.
#
# flashr:       flash <image_file>
#               flash given file to the target but directly in RAM.
#
#               See 'flash' command above for options
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
# term-rtt:     opens a serial terminal using RTT (Real-Time Transfer)
#
#               <options>
#               RTT_PORT:       port opened for RTT connection
#
# @author       Hauke Peteresen <hauke.petersen@fu-berlin.de>
# @author       Joakim Nohlgård <joakim.nohlgard@eistec.se>

# Default GDB port, set to 0 to disable, required != 0 for debug and debug-server targets
: ${GDB_PORT:=3333}
# Default GDB port core offset
: ${GDB_PORT_CORE_OFFSET:=0}
# Default telnet port, set to 0 to disable
: ${TELNET_PORT:=4444}
# Default TCL port, set to 0 to disable
: ${TCL_PORT:=6333}
# Default RTT port
: ${RTT_PORT:=9999}
# Default OpenOCD command
: ${OPENOCD:=openocd}
# Extra board initialization commands to pass to OpenOCD
: ${OPENOCD_EXTRA_INIT:=}
# Debugger interface initialization commands to pass to OpenOCD
: ${OPENOCD_ADAPTER_INIT:=}
# If set to 1 'reset_config' will use 'connect_assert_srst' before 'flash' or 'reset.
: ${OPENOCD_RESET_USE_CONNECT_ASSERT_SRST:=}
# Default bind address for OpenOCD
: ${OPENOCD_SERVER_ADDRESS:=localhost}
# The setsid command is needed so that Ctrl+C in GDB doesn't kill OpenOCD
: ${SETSID:=setsid}
# GDB command, usually a separate command for each platform (e.g. arm-none-eabi-gdb)
: ${GDB:=gdb}
# Debugger client command, can be used to wrap GDB in a front-end
: ${DBG:=${GDB}}
# Default debugger flags,
: ${DBG_DEFAULT_FLAGS:=-q -ex \"tar ext ${OPENOCD_SERVER_ADDRESS}:$(( GDB_PORT + GDB_PORT_CORE_OFFSET ))\"}
# Extra debugger flags, added by the user
: ${DBG_EXTRA_FLAGS:=}
# Debugger flags, will be passed to sh -c, remember to escape any quotation signs.
# Use ${DBG_DEFAULT_FLAGS} to insert the default flags anywhere in the string
: ${DBG_FLAGS:=${DBG_DEFAULT_FLAGS} ${DBG_EXTRA_FLAGS}}
# Initial target state when using debug, by default a 'halt' request is sent to
# the target when starting a debug session. 'reset halt' can also be used
# depending on the type of target.
: ${OPENOCD_DBG_START_CMD:=-c 'halt'}
# Extra commands to add when using debug
: ${OPENOCD_DBG_EXTRA_CMD:=}
# command used to reset the board
: ${OPENOCD_CMD_RESET_RUN:="-c 'reset run'"}
# command used to halt the board
: ${OPENOCD_CMD_RESET_HALT:="-c 'reset halt'"}
# Select core on multi-core processors.
: ${OPENOCD_CORE:=}
# Set to any value to skip verifying after flashing.
: ${OPENOCD_SKIP_VERIFY:=}
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

# default terminal frontend
_OPENOCD_TERMPROG=${RIOTTOOLS}/pyterm/pyterm
_OPENOCD_TERMFLAGS="-ts ${OPENOCD_SERVER_ADDRESS}:${RTT_PORT} ${PYTERMFLAGS}"

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

# Handle OPENOCD_RESET_USE_CONNECT_ASSERT_SRST
if [ "${OPENOCD_RESET_USE_CONNECT_ASSERT_SRST}" = "1" ]; then
  OPENOCD_EXTRA_RESET_INIT+="-c 'reset_config connect_assert_srst'"
fi

#
# a couple of tests for certain configuration options
#
test_config() {
    if [ ! -f "${OPENOCD_CONFIG}" ] && [[ ! "${OPENOCD_CONFIG}" == board/* ]] ; then
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

_has_bin_extension() {
    # The regex need to be without quotes
    local firmware=$1
    [[ "${firmware}" =~ ^.*\.bin$ ]]
}

# Return 0 if given file should be considered a binary
_is_binfile() {
    local firmware="$1"
    local firmware_type="$2"
    [[ "${firmware_type}" = "bin" ]] || { \
        [[ -z "${firmware_type}" ]] && _has_bin_extension "${firmware}"; }
}

_flash_list() {
    # Openocd output for 'flash list' is either
    # ....
    # {name nrf51 base 0 size 0 bus_width 1 chip_width 1} {name nrf51 base 268439552 size 0 bus_width 1 chip_width 1}
    # ....
    # or for newer openocd versions (v0.12.0 or higher)
    # ...
    # {name nrf51.flash driver nrf51 base 0 size 0 bus_width 1 chip_width 1 target nrf51.cpu} {name nrf51.uicr ...}
    # ...
    # Before printing the flash list, try to init and probe the board
    # to get the actual address.
    # Some openocd configuration put an address of 0 and rely on probing to
    # find the real flash address like 0x08000000
    #
    # If it does not work, fallback to only query the configured value
    #
    # Probing can fail when the board is in a non flashable state or
    # maybe probing would need a different init procedure.
    # At least, currently fallback to returning the configured value

    # Probe the real value
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_RESET_INIT} \
            -c 'init' \
            -c 'targets' \
            ${OPENOCD_CMD_RESET_HALT} \
            -c 'flash probe 0' \
            -c 'flash list' \
            -c 'shutdown'" 2>&1 && return

    # Fallback to return the value stored in openocd
    echo "WARN: Failed to probe board flash." >&2
    echo "WARN: Falling back to using the openocd configuration value." >&2
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            -c 'flash list' \
            -c 'shutdown'" 2>&1
}

# Print flash address for 'bank_num' num defaults to 0
# _flash_address  [bank_num:0]
_flash_address() {
    _flash_list | "${RIOTTOOLS}/openocd/openocd_flashinfo.py" --idx "${1:-0}"
}

do_flashr() {
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

    # In case of binary file, IMAGE_OFFSET should include the flash base address
    # This allows flashing normal binary files without env configuration
    if _is_binfile "${IMAGE_FILE}" "${IMAGE_TYPE}"; then
        # hardwritten to use the first bank
        FLASH_ADDR=$(_flash_address 0)
        echo "Binfile detected, adding ROM base address: ${FLASH_ADDR}"
        IMAGE_TYPE=bin
        IMAGE_OFFSET=$(printf "0x%08x\n" "$((${IMAGE_OFFSET} + ${FLASH_ADDR}))")
    fi

    if [ "${IMAGE_OFFSET}" != "0" ]; then
        echo "Flashing with IMAGE_OFFSET: ${IMAGE_OFFSET}"
    fi

    # flash device
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            ${OPENOCD_EXTRA_RESET_INIT} \
            -c 'tcl_port 0' \
            -c 'telnet_port 0' \
            -c 'gdb_port 0' \
            -c 'init' \
            -c 'targets ${OPENOCD_CORE}' \
            -c 'reset' \
            -c 'halt' \
            -c 'load_image \"${IMAGE_FILE}\" ' \
            -c 'resume ${START_ADDR}' \
            -c 'shutdown'" &&
    echo "'Done flashing"
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

    # In case of binary file, IMAGE_OFFSET should include the flash base address
    # This allows flashing normal binary files without env configuration
    if _is_binfile "${IMAGE_FILE}" "${IMAGE_TYPE}"; then
        # hardwritten to use the first bank
        FLASH_ADDR=$(_flash_address 0)
        echo "Binfile detected, adding ROM base address: ${FLASH_ADDR}"
        IMAGE_TYPE=bin
        IMAGE_OFFSET=$(printf "0x%08x\n" "$((${IMAGE_OFFSET} + ${FLASH_ADDR}))")
    fi

    if [ -z "${OPENOCD_SKIP_VERIFY}" ]; then
        OPENOCD_VERIFY="-c 'verify_image \"${IMAGE_FILE}\" ${IMAGE_OFFSET}'"
    fi

    if [ "${IMAGE_OFFSET}" != "0" ]; then
        echo "Flashing with IMAGE_OFFSET: ${IMAGE_OFFSET}"
    fi

    # flash device
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            ${OPENOCD_EXTRA_RESET_INIT} \
            -c 'tcl_port 0' \
            -c 'telnet_port 0' \
            -c 'gdb_port 0' \
            -c 'init' \
            -c 'targets' \
            ${OPENOCD_CMD_RESET_HALT} \
            ${OPENOCD_PRE_FLASH_CMDS} \
            -c 'flash write_image erase \"${IMAGE_FILE}\" ${IMAGE_OFFSET} ${IMAGE_TYPE}' \
            ${OPENOCD_PRE_VERIFY_CMDS} \
            ${OPENOCD_VERIFY} \
            -c 'reset run' \
            -c 'shutdown'" &&
    echo 'Done flashing'
}

do_debugclient() {
    ELFFILE=$1
    test_elffile
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

do_debug() {
    ELFFILE=$1
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
            -c 'bindto ${OPENOCD_SERVER_ADDRESS}' \
            -c 'tcl_port ${TCL_PORT}' \
            -c 'telnet_port ${TELNET_PORT}' \
            -c 'gdb_port ${GDB_PORT}' \
            -c 'init' \
            ${OPENOCD_DBG_EXTRA_CMD} \
            -c 'targets ${OPENOCD_CORE}' \
            ${OPENOCD_DBG_START_CMD} \
            -l /dev/null & \
            echo \$! > $OCD_PIDFILE" &
    do_debugclient ${ELFFILE}
}

do_debugserver() {
    test_config
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
    # start OpenOCD as GDB server
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            -c 'bindto ${OPENOCD_SERVER_ADDRESS}' \
            -c 'tcl_port ${TCL_PORT}' \
            -c 'telnet_port ${TELNET_PORT}' \
            -c 'gdb_port ${GDB_PORT}' \
            -c 'init' \
            ${OPENOCD_DBG_EXTRA_CMD} \
            -c 'targets' \
            -c 'halt' & \
            echo \$! > $OCD_PIDFILE ; \
            wait \$(cat $OCD_PIDFILE)" &

    while read -r line; do
        echo "Exit with Ctrl+D"
    done
}

do_reset() {
    test_config
    # start OpenOCD and invoke board reset
    sh -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            ${OPENOCD_EXTRA_RESET_INIT} \
            -c 'tcl_port 0' \
            -c 'telnet_port 0' \
            -c 'gdb_port 0' \
            -c 'init' \
            ${OPENOCD_CMD_RESET_RUN} \
            -c 'shutdown'"
}

do_term() {
    test_config

    # temporary file that save the OpenOCD pid
    OPENOCD_PIDFILE=$(mktemp -t "openocd_pid.XXXXXXXXXX")
    # will be called by trap
    cleanup() {
        if [ -f $OPENOCD_PIDFILE ]; then
            OPENOCD_PID="$(cat ${OPENOCD_PIDFILE})"
            kill ${OPENOCD_PID}
            rm -r "${OPENOCD_PIDFILE}"
        fi
        exit 0
    }
    # cleanup after script terminates
    trap "cleanup ${OPENOCD_PIDFILE}" EXIT INT

    set -x
    echo test
    # start OpenOCD as RTT server for channel 0
    sh -x -c "${OPENOCD} \
            ${OPENOCD_ADAPTER_INIT} \
            -f '${OPENOCD_CONFIG}' \
            ${OPENOCD_EXTRA_INIT} \
            -c 'bindto ${OPENOCD_SERVER_ADDRESS}' \
            -c 'tcl_port 0' \
            -c 'telnet_port 0' \
            -c 'gdb_port 3333' \
            -c init \
            -c 'rtt setup '${RAM_START_ADDR}' '${RAM_LEN}' \"SEGGER RTT\"' \
            -c 'rtt start' \
            -c 'rtt server start '${RTT_PORT}' 0' \
            >/dev/null & \
            echo  \$! > $OPENOCD_PIDFILE" &
    sleep 1

    ${OPENOCD_TERMPROG:-${_OPENOCD_TERMPROG}} ${OPENOCD_TERMFLAGS:-${_OPENOCD_TERMFLAGS}}
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
  flashr)
    START_ADDR=$(objdump -f $1 | sed '/^$/d' | tail -1 | grep -o "0x[0-9a-fA-F].*")
    echo "### Flashing target RAM ###"
        do_flashr "$@"
    ;;
  debugr)
    START_ADDR=$(objdump -f $1 | sed '/^$/d' | tail -1 | grep -o "0x[0-9a-fA-F].*")
    echo "Start address: $START_ADDR"
    DBG_FLAGS="$DBG_FLAGS \
        -ex 'load $1' \
        "
        do_debug "$@"
    ;;
  debug)
    echo "### Starting Debugging ###"
    do_debug "$@"
    ;;
  debug-client)
    echo "### Attaching to GDB Server ###"
    do_debugclient "$@"
    ;;
  debug-server)
    echo "### Starting GDB Server ###"
    do_debugserver
    ;;
  reset)
    echo "### Resetting Target ###"
    do_reset
    ;;
  term-rtt)
    echo "### Starting RTT terminal ###"
    do_term
    ;;
  *)
    echo "Usage: $0 {flash|debug|debug-server|reset}"
    echo "          flash <flashfile>"
    echo "          debug <elffile>"
    exit 2
    ;;
esac
