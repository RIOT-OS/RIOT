#!/bin/sh

ETHOS_DIR="$(dirname $(readlink -f $0))"/../../../dist/tools/ethos

create_tap() {
    ip tuntap add ${TAP} mode tap user ${USER}
    ip link set ${TAP} up
    sysctl -w net.ipv6.conf.${TAP}.forwarding=1
    sysctl -w net.ipv6.conf.${TAP}.accept_ra=0
    ip a a ${NETPREFIX}::1/64 dev ${TAP}
    ip r d ${NETPREFIX}::/64 dev ${TAP}
    ip r a ${NETPREFIX}::2 dev ${TAP}
    ip r a ${NETPREFIX}::/64 via ${NETPREFIX}::2 dev ${TAP} metric 1
}

create_mcast_route() {
    ip -6 route add ${IPMCAST} dev ${TAP} table local
}

remove_tap() {
    ip tuntap del ${TAP} mode tap
}

cleanup() {
    echo "Cleaning up..."
    remove_tap
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
NETPREFIX=$(echo $3 |  awk -v delimeter="::" '{split($0,a,delimeter)} END{print a[1]}')
IPMCAST=$4

UHCPD="$(readlink -f "${ETHOS_DIR}/../uhcpd/bin")/uhcpd"

[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" -o -z "${IPMCAST}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> <ip-mcast>"
    exit 1
}

trap "cleanup" INT QUIT TERM EXIT


create_tap  && create_mcast_route && start_uhcpd && "${ETHOS_DIR}/ethos" ${TAP} ${PORT}
