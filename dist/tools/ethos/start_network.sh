#!/bin/sh

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
    kill $UHCPD_PID
}

start_uhcpd() {
    ${UHCPD} ${TAP} ${PREFIX} > /dev/null &
    UHCPD_PID=$!
}

PORT=$1
TAP=$2
PREFIX=$3
UHCPD=../uhcpd/bin/uhcpd

[ -z "$PORT" -o -z "$TAP" -o -z "$PREFIX" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix>"
    exit 1
}

trap "cleanup" EXIT

create_tap && start_uhcpd && ./ethos $TAP $PORT
