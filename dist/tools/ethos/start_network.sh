#!/bin/sh

ETHOS_DIR="$(cd $(dirname $0); pwd)"
ETHOS="${ETHOS_DIR}/ethos"
[ ! -f ${ETHOS} ] && {
    echo "missing binary (${ETHOS})"
    exit 1
}

UHCPD_DIR="$(cd ${ETHOS_DIR}; cd ../uhcpd; pwd)"
UHCPD="${UHCPD_DIR}/bin/uhcpd"
[ ! -f ${UHCPD} ] && {
    echo "missing binary (${UHCPD})"
    exit 1
}

create_tap() {
    ip tuntap add ${TAP} mode tap user ${USER}
    sysctl -w net.ipv6.conf.${TAP}.forwarding=1
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=0
    ip link set ${TAP} up
    ip a a fe80::1/64 dev ${TAP}
    ip a a fd00:dead:beef::1/128 dev lo
    ip route add ${PREFIX} via fe80::2 dev ${TAP}
}

remove_tap() {
    ip tuntap del ${TAP} mode tap
}

cleanup() {
    echo "Cleaning up..."
    remove_tap
    ip a d fd00:dead:beef::1/128 dev lo
    kill ${UHCPD_PID}
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ${UHCPD} ${TAP} ${PREFIX} > /dev/null &
    UHCPD_PID=$!
}

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> [baudrate]"
    exit 1
}

[ ! -z $4 ] && {
    BAUDRATE=$4
}

trap "cleanup" INT QUIT TERM EXIT


create_tap && start_uhcpd && "${ETHOS_DIR}/ethos" ${TAP} ${PORT} ${BAUDRATE}
