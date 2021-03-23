#!/usr/bin/env bash

# This script wraps an emulator GDB server (renode or qemu) and a debugger
# client in a single command and takes 4 arguments: the board to emulate,
# the application directory of the the current application, the elffile
# containing the firmware to debug, the debugger port and custom debugger client
# arguments

# This script is supposed to be called from RIOTs make system when using the
# "debug" target.

# @author       Alexandre Abadie <alexandre.abadie@inria.fr>

BOARD=$1
APPDIR=$2
ELFFILE=$3

# GDB command, usually a separate command for each platform (e.g. arm-none-eabi-gdb)
: ${GDB:=gdb-multiarch}
# Debugger client command, can be used to wrap GDB in a front-end
: ${DBG:=${GDB}}
# Default GDB port, set to 0 to disable, required != 0 for debug and debug-server targets
: ${GDB_PORT:=$4}
# Default debugger flags,
: ${DBG_DEFAULT_FLAGS:=-q -ex \"target remote :${GDB_PORT}\"}
# Custom extra debugger flags, depends on the emulator
: ${DBG_CUSTOM_FLAGS:=$5}
# Debugger flags, will be passed to sh -c, remember to escape any quotation signs.
# Use ${DBG_DEFAULT_FLAGS} to insert the default flags anywhere in the string
: ${DBG_FLAGS:=${DBG_DEFAULT_FLAGS} ${DBG_CUSTOM_FLAGS}}

# temporary file that contains the emulator pid
EMULATOR_PIDFILE=$(mktemp -t "emulator_pid.XXXXXXXXXX")
# will be called by trap
cleanup() {
    kill "$(cat ${EMULATOR_PIDFILE})"
    rm -f "${EMULATOR_PIDFILE}"
    exit 0
}
# cleanup after script terminates
trap "cleanup ${EMULATOR_PIDFILE}" EXIT
# don't trap on Ctrl+C, because GDB keeps running
trap '' INT

# start emulator GDB server
sh -c "\
    GDB_PORT=${GDB_PORT} \
    EMULATE=1 \
    EMULATOR_PIDFILE=${EMULATOR_PIDFILE} \
    BOARD=${BOARD} \
    make -C ${APPDIR} debug-server & \
    echo \$! > ${EMULATOR_PIDFILE}" &
# Start the debugger and connect to the GDB server
sh -c "${DBG} ${DBG_FLAGS} ${ELFFILE}"
