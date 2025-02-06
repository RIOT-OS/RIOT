#!/bin/sh

ETHOS_DIR="$(dirname $(readlink -f $0))"

create_tap() {
    ip tuntap add ${TAP} mode tap user ${USER}
    sysctl -w net.ipv6.conf.${TAP}.forwarding=1
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=0
    ip link set ${TAP} up
    ip a a fe80::1/64 dev ${TAP}
    ip a a ${HOST_IP} dev ${TAP}
}

remove_tap() {
    ip tuntap del ${TAP} mode tap
}

cleanup() {
    echo "Cleaning up..."
    remove_tap
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
    ip route add "${PREFIX}" via fe80::2 dev "${TAP}"
    ${UHCPD} ${TAP} ${PREFIX} > /dev/null &
    UHCPD_PID=$!
}

start_dhcpd() {
    ip route add "${PREFIX}" via fe80::2 dev "${TAP}"
    DHCPD_PIDFILE=$(mktemp)
    ${DHCPD} -d -p ${DHCPD_PIDFILE} ${TAP} ${PREFIX} 2> /dev/null
}

start_radvd() {
    ADDR=$(echo "${PREFIX}" | sed -e 's/::\//::1\//')
    ip a a "${ADDR}" dev "${TAP}"
    sysctl net.ipv6.conf."${TAP}".accept_ra=2
    sysctl net.ipv6.conf."${TAP}".accept_ra_rt_info_max_plen=64
    ${RADVD} -c "${TAP}" "${PREFIX}"
}

if [ "$1" = "-d" ] || [ "$1" = "--use-dhcpv6" ]; then
    USE_DHCPV6=1
    shift 1
else
    USE_DHCPV6=0
fi

if [ "$1" = "-r" ] || [ "$1" = "--use-radvd" ]; then
    USE_RADVD=1
    shift 1
else
    USE_RADVD=0
fi

if [ "$1" = "-e" ] || [ "$1" = "--ethos-only" ]; then
    ETHOS_ONLY=1
    shift 1
else
    ETHOS_ONLY=0
fi

if [ "$1" = "-i" ] || [ "$1" = "--host-ip" ]; then
    HOST_IP=$2
    shift 2
else
    HOST_IP="fd00:dead:beef::1/128"
fi

PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200
START_ETHOS=1

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 [-d|--use-dhcpv6] [-r|--use-radvd] [-e|--ethos-only] [-i|--host-ip <ip>]" \
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
    elif [ ${USE_RADVD} -eq 1 ]; then
        RADVD="$(readlink -f "${ETHOS_DIR}/../radvd/")/radvd.sh"
        start_radvd
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
