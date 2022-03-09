#!/usr/bin/env bash

# This script wraps IoT-LAB remote debugging tools so they can be used with
# debug and debug-server targets.

# @author       Alexandre Abadie <alexandre.abadie@inria.fr>

IOTLAB_NODE=$1
IOTLAB_NODE_LIST=$2
IOTLAB_AUTHORITY=$3
IOTLAB_EXP_ID=$4
ELFFILE=$5
SERVER_ONLY=$6

# GDB command, usually a separate command for each platform (e.g. arm-none-eabi-gdb)
: "${GDB:=gdb-multiarch}"
# Debugger client command, can be used to wrap GDB in a front-end
: "${DBG:=${GDB}}"
# Default GDB port
: "${GDB_PORT:=3333}"

# If run from the SSH frontend the remote is the iotlab node hostname. If run
# locally, the GDB server is exposed via an SSH tunnel, so the GDB remote is
# localhost in this case.
if [ ! -z "${IOT_LAB_FRONTEND_FQDN}" ]
then
    GDB_REMOTE=${IOTLAB_NODE}
    # Use a blocking interactive command to simulate the SSH tunnel
    REMOTE_ACCESS_COMMAND="tee &"
else
    GDB_REMOTE=localhost
    REMOTE_ACCESS_COMMAND="ssh -L 3333:${IOTLAB_NODE}:${GDB_PORT} -N ${IOTLAB_AUTHORITY} &"
fi

# Default debugger flags,
: "${DBG_FLAGS:=-q -ex \"target remote ${GDB_REMOTE}:${GDB_PORT}\"}"

# temporary file that contains the iotlab SSH tunnel pid
IOTLAB_DEBUG_PIDFILE=$(mktemp -t "iotlab_debug_pid.XXXXXXXXXX")
# will be called by trap
cleanup() {
    # shellcheck disable=SC2086
    iotlab-node --jmespath='keys(@)[0]' --format='lambda ret: exit(int(ret))' --debug-stop ${IOTLAB_EXP_ID} ${IOTLAB_NODE_LIST}
    # shellcheck disable=SC2046
    kill $(cat "${IOTLAB_DEBUG_PIDFILE}")
    rm -f "${IOTLAB_DEBUG_PIDFILE}"
    exit 0
}
# cleanup after script terminates
trap 'cleanup ${IOTLAB_DEBUG_PIDFILE}' EXIT

# Start the GDB server on the IoT-LAB node
# shellcheck disable=SC2086
iotlab-node --jmespath='keys(@)[0]' --format='lambda ret: exit(int(ret))' --debug-start ${IOTLAB_EXP_ID} ${IOTLAB_NODE_LIST}

# open SSH tunnel to GDB server running on the IoT-LAB node
sh -c "\
    ${REMOTE_ACCESS_COMMAND} \
    echo \$! > ${IOTLAB_DEBUG_PIDFILE}" &

if [ ! -z "${SERVER_ONLY}" ]
then
    # Block until CTRL-C is sent
    read -r -d ""
else
    # don't trap on Ctrl+C, because GDB keeps running
    trap '' INT
    # Start the debugger and connect to the GDB server
    sh -c "${DBG} ${DBG_FLAGS} ${ELFFILE}"
fi
