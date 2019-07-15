#!/bin/sh

ETHOS_DIR="$(dirname $(readlink -f $0))"

cleanup() {
    [ -n "${SETUP_NETWORK_PID}" ] && kill -TERM ${SETUP_NETWORK_PID}
    trap "" INT QUIT TERM EXIT
}

setup_network() {
    ${ETHOS_DIR}/setup_network.sh $1 $2 &
    SETUP_NETWORK_PID=$!
}

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200
UHCPD="$(readlink -f "${ETHOS_DIR}/../uhcpd/bin")/uhcpd"

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> [baudrate]"
    exit 1
}

[ ! -z $4 ] && {
    BAUDRATE=$4
}

trap "cleanup" INT QUIT TERM EXIT

setup_network ${TAP} ${PREFIX} && "${ETHOS_DIR}/ethos" ${TAP} ${PORT} ${BAUDRATE}
