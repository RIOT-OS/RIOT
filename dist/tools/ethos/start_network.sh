#!/bin/sh

ETHOS_DIR="$(dirname $(readlink -f $0))"

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
    if [ -n "${UHCPD_PID}" ]; then
        kill ${UHCPD_PID}
    fi
    if [ -n "${DHCPD_PIDFILE}" ]; then
        kill "$(cat ${DHCPD_PIDFILE})"
        rm "${DHCPD_PIDFILE}"
    fi
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ${UHCPD} ${TAP} ${PREFIX} > /dev/null &
    UHCPD_PID=$!
}

start_dhcpd() {
    DHCPD_PIDFILE=$(mktemp)
    ${DHCPD} -d -p ${DHCPD_PIDFILE} ${TAP} ${PREFIX} 2> /dev/null
}

if [ "$1" = "-d" ] || [ "$1" = "--use-dhcpv6" ]; then
    USE_DHCPV6=1
    shift 1
else
    USE_DHCPV6=0
fi

if [ "$1" = "-e" ] || [ "$1" = "--ethos-only" ]; then
    ETHOS_ONLY=1
    shift 1
else
    ETHOS_ONLY=0
fi

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200
START_ETHOS=1

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 [-d|--use-dhcp] [-e|--ethos-only] " \
         "<serial-port> <tap-device> <prefix> " \
         "[baudrate]"
    exit 1
}

[ ! -z $4 ] && {
    BAUDRATE=$4
}

trap "cleanup" INT QUIT TERM EXIT


create_tap && \
if [ ${ETHOS_ONLY} -ne 1 ]; then
    if [ ${USE_DHCPV6} -eq 1 ]; then
        DHCPD="$(readlink -f "${ETHOS_DIR}/../dhcpv6-pd_ia/")/dhcpv6-pd_ia.py"
        start_dhcpd
        START_ETHOS=$?
    else
        UHCPD="$(readlink -f "${ETHOS_DIR}/../uhcpd/bin")/uhcpd"
        start_uhcpd
        START_ETHOS=$?
    fi
else
    START_ETHOS=0
fi

[ ${START_ETHOS} -eq 0 ] && "${ETHOS_DIR}/ethos" ${TAP} ${PORT} ${BAUDRATE}
