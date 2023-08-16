#!/usr/bin/env bash

ZEP_DISPATCH_DIR="$(cd "$(dirname "$0")" && pwd -P)"
UHCPD="$(cd "${ZEP_DISPATCH_DIR}/../uhcpd/bin" && pwd -P)/uhcpd"
DHCPD="$(cd "${ZEP_DISPATCH_DIR}/../dhcpv6-pd_ia/" && pwd -P)/dhcpv6-pd_ia.py"
ZEP_DISPATCH="${ZEP_DISPATCH_DIR}/bin/zep_dispatch"

TAP_GLB="fdea:dbee:f::1/64"

NOSUDO="sudo -u ${SUDO_USER}"

create_tap() {
    ip tuntap add "${TAP}" mode tap user "${SUDO_USER}"
    sysctl -w net.ipv6.conf."${TAP}".forwarding=1
    sysctl -w net.ipv6.conf."${TAP}".accept_ra=0
    ip link set "${TAP}" up
    ip a a fe80::1/64 dev "${TAP}"
    ip a a ${TAP_GLB} dev "${TAP}"
    ip route add "${PREFIX}" via fe80::2 dev "${TAP}"
}

remove_tap() {
    ip tuntap del "${TAP}" mode tap
}

cleanup() {
    echo "Cleaning up..."
    remove_tap
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
    ${UHCPD} "${TAP}" "${PREFIX}" > /dev/null &
    UHCPD_PID=$!
}

start_dhcpd() {
    DHCPD_PIDFILE=$(mktemp)
    ${DHCPD} -d -p "${DHCPD_PIDFILE}" "${TAP}" "${PREFIX}" 2> /dev/null
}

start_zep_dispatch() {
    ${ZEP_DISPATCH} :: "${ZEP_PORT_BASE}" > /dev/null &
    ZEP_DISPATCH_PID=$!
}

if [ "$1" = "-d" ] || [ "$1" = "--use-dhcpv6" ]; then
    USE_DHCPV6=1
    shift 1
else
    USE_DHCPV6=0
fi

if [ "$1" = "-z" ] || [ "$1" = "--use-zep-dispatch" ]; then
    USE_ZEP_DISPATCH=1
    ZEP_PORT_BASE=$2
    shift 2
else
    USE_ZEP_DISPATCH=0
fi

ELFFILE=$1
PREFIX=$2
shift 2

# tap will be the last argument
for TAP in "$@"; do :; done

[[ -z "${ELFFILE}" || -z "${PREFIX}" || -z "${TAP}" ]] && {
    echo "usage: $0 [-d|--use-dhcp] [-z|--use-zep <port>] " \
         "<elffile> <prefix> [elf args]"
    exit 1
}

trap "cleanup" INT QUIT TERM EXIT

create_tap

if [ ${USE_ZEP_DISPATCH} -eq 1 ]; then
    start_zep_dispatch
fi

if [ ${USE_DHCPV6} -eq 1 ]; then
    start_dhcpd
else
    start_uhcpd
fi

${NOSUDO} "${ELFFILE}" "${TAP}" "$@"
