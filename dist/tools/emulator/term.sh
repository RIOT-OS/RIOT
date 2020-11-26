#!/usr/bin/env bash

# This script wraps an emulator (renode or qemu) and a terminal client
# in a single command and takes 6 arguments:
# 1. the type of emulator (renode or qemu)
# 2. the board to emulate,
# 3. the application directory of the the current application
# 4. the terminal client tool
# 5. the options of the terminal client
# 6. the serial port to connect to the terminal client to

# This script is supposed to be called from RIOTs make system when using the
# "term" or "cleanterm" targets.

# @author       Alexandre Abadie <alexandre.abadie@inria.fr>

EMULATOR=$1
BOARD=$2
APPDIR=$3
TERMPROG=$4
TERMFLAGS=$5
PORT=$6

# temporary file that contains the emulator pid
EMULATOR_PIDFILE=$(mktemp -t "emulator_pid.XXXXXXXXXX")
SOCAT_PIDFILE=$(mktemp -t "socat_pid.XXXXXXXXXX")

# will be called by trap
cleanup() {
    if [ ${EMULATOR} = "qemu" ]
    then
        echo "cleanup ${SOCAT_PIDFILE}"
        kill "$(cat ${SOCAT_PIDFILE})"
        rm -f "${SOCAT_PIDFILE}"
    fi
    echo "cleanup ${EMULATOR_PIDFILE}"
    kill "$(cat ${EMULATOR_PIDFILE})"
    rm -f "${EMULATOR_PIDFILE}"
    rm -f ${PORT}
    exit 0
}
# cleanup after script terminates
trap "cleanup terminal for ${EMULATOR} emulator" EXIT

# start emulator in background
sh -c "\
    EMULATOR_PIDFILE=${EMULATOR_PIDFILE} \
    EMULATE=1 \
    BOARD=${BOARD} \
    make -C ${APPDIR} emulate & \
    echo \$! > ${EMULATOR_PIDFILE}" &

# with qemu, start socat redirector in background
if [ ${EMULATOR} = "qemu" ]
then
    sleep 1
    sh -c "\
        socat pty,link=${PORT},raw,echo=0 TCP:localhost:5555 & \
        echo \$! > ${SOCAT_PIDFILE}" &
fi

# Start the terminal client after PORT is available. PORT can be a symlink.
while [ ! -L "${PORT}" ]
do
    sleep 1
done

sh -c "${TERMPROG} ${TERMFLAGS}"
