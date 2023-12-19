#!/usr/bin/env bash

ZEP_DISPATCH_DIR="$(cd "$(dirname "$0")" && pwd -P)"
UHCPD="$(cd "${ZEP_DISPATCH_DIR}/../uhcpd/bin" && pwd -P)/uhcpd"
DHCPD="$(cd "${ZEP_DISPATCH_DIR}/../dhcpv6-pd_ia/" && pwd -P)/dhcpv6-pd_ia.py"
RADVD="$(cd "${ZEP_DISPATCH_DIR}/../radvd/" && pwd -P)/radvd.sh"
ZEP_DISPATCH="${ZEP_DISPATCH_DIR}/bin/zep_dispatch"

NOSUDO="sudo -u ${SUDO_USER}"

create_tap() {
    ip tuntap add "${TAP}" mode tap user "${SUDO_USER}"
    sysctl -w net.ipv6.conf."${TAP}".forwarding=1
    sysctl -w net.ipv6.conf."${TAP}".accept_ra=0
    ip link set "${TAP}" up
    ip a a fe80::1/64 dev "${TAP}"
    ip a a ${HOST_IP} dev "${TAP}"
}

remove_tap() {
    ip tuntap del "${TAP}" mode tap
}

cleanup() {
    echo "Cleaning up..."
    if [ "${CREATE_TAP}" -eq 1 ]; then
        remove_tap
    fi
    if [ -n "${UHCPD_PID}" ]; then
        kill "${UHCPD_PID}"
    fi
    if [ -n "${ZEP_DISPATCH_PID}" ]; then
        kill "${ZEP_DISPATCH_PID}"
    fi
    if [ -n "${DHCPD_PIDFILE}" ]; then
        kill "$(cat "${DHCPD_PIDFILE}")"
        rm "${DHCPD_PIDFILE}"
    fi
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ip route add "${PREFIX}" via fe80::2 dev "${TAP}"
    ${UHCPD} "${TAP}" "${PREFIX}" > /dev/null &
    UHCPD_PID=$!
}

start_dhcpd() {
    ip route add "${PREFIX}" via fe80::2 dev "${TAP}"
    DHCPD_PIDFILE=$(mktemp)
    ${DHCPD} -d -p "${DHCPD_PIDFILE}" "${TAP}" "${PREFIX}" 2> /dev/null
}

start_radvd() {
    ADDR=$(echo "${PREFIX}" | sed -e 's/::\//::1\//')
    ip a a "${ADDR}" dev "${TAP}"
    sysctl net.ipv6.conf."${TAP}".accept_ra=2
    sysctl net.ipv6.conf."${TAP}".accept_ra_rt_info_max_plen=64
    ${RADVD} -c "${TAP}" "${PREFIX}"
}

start_zep_dispatch() {
    ${ZEP_DISPATCH} ${ZEP_DISPATCH_FLAGS} :: "${ZEP_PORT_BASE}" > /dev/null &
    ZEP_DISPATCH_PID=$!
}

if [ "$1" = "-c" ] || [ "$1" = "--create-tap" ]; then
    CREATE_TAP=1
    shift 1
else
    CREATE_TAP=0
fi

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

if [ "$1" = "-z" ] || [ "$1" = "--use-zep-dispatch" ]; then
    USE_ZEP_DISPATCH=1
    ZEP_PORT_BASE=$2
    shift 2
else
    USE_ZEP_DISPATCH=0
fi

if [ "$1" = "-t" ] || [ "$1" = "--topology" ]; then
    ZEP_DISPATCH_FLAGS+="-t $2 "
    shift 2
fi

if [ "$1" = "-w" ] || [ "$1" = "--monitor" ]; then
    modprobe mac802154_hwsim
    ZEP_DISPATCH_FLAGS+="-w wpan0 "
    shift 1
fi

if [ "$1" = "-i" ] || [ "$1" = "--host-ip" ]; then
    HOST_IP=$2
    shift 2
else
    HOST_IP="fd00:dead:beef::1/128"
fi

ELFFILE=$1
PREFIX=$2
shift 2

# tap will be the last argument
for TAP in "$@"; do :; done

[[ -z "${ELFFILE}" || -z "${PREFIX}" || -z "${TAP}" ]] && {
    echo "usage: $0 [-c|--create-tap] [-d|--use-dhcpv6] [-r|--use-radvd] [-z|--use-zep-dispatch <port>] [-t|--topology <xxx>] [-w|--monitor] [-i|--host-ip <ip>] " \
         "<elffile> <prefix> [elf args]"
    exit 1
}

trap "cleanup" INT QUIT TERM EXIT

if [ ${CREATE_TAP} -eq 1 ]; then
    create_tap
fi

if [ ${USE_ZEP_DISPATCH} -eq 1 ]; then
    start_zep_dispatch
fi

if [ ${USE_DHCPV6} -eq 1 ]; then
    start_dhcpd
elif [ ${USE_RADVD} -eq 1 ]; then
    start_radvd
else
    start_uhcpd
fi

${NOSUDO} "${ELFFILE}" "${TAP}" "$@"
