#!/bin/sh

DOSE_DIR="$(dirname "$(readlink -f $0)")"
USE_WRITE=0

create_tap() {
    ip tuntap add ${TAP} mode tap user ${USER}
    sysctl -w net.ipv6.conf.${TAP}.forwarding=${USE_WRITE}
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=1
    ip link set "${TAP}" up
}

remove_tap() {
    ip tuntap del "${TAP}" mode tap
}

cleanup() {
    echo "Cleaning up..."
    remove_tap
    trap "" INT QUIT TERM EXIT
}

if [ "$1" = "-w" ] || [ "$1" = "--write" ]; then
    USE_WRITE=1
    shift 1
fi

PORT=$1
TAP=$2

BAUDRATE=115200
START_DOSE=0

[ -z "${PORT}" ] || [ -z "${TAP}" ] && {
    echo "usage: $0 [-w|--write]" \
         "<serial-port> <tap-device>" \
         "[baudrate]"
    exit 1
}

shift 2

[ ! -z "$1" ] && {
    BAUDRATE=$1
}

if [ ${USE_WRITE} -eq 1 ]; then
    ARGS="-w"
fi

trap "cleanup" INT QUIT TERM EXIT

create_tap

[ ${START_DOSE} -eq 0 ] && "${DOSE_DIR}/dose" ${ARGS} -b ${BAUDRATE} ${TAP} ${PORT}
