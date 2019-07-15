#!/bin/sh

create_tap() {
    ip tuntap add ${TAP} mode tap user ${_USER}
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
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ${UHCPD} ${TAP} ${PREFIX}
}

TAP=$1
PREFIX=$2
_USER=$3
: ${UHCPD:="$(readlink -f $(dirname $0)"/../uhcpd/bin")/uhcpd"}

[ -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <tap-device> <prefix> [<user>]"
    exit 1
}

if [ -z "$_USER" ]; then
    if [ -n "$SUDO_USER" ] ; then
        _USER=$SUDO_USER
    else
        _USER=$USER
    fi
fi

trap "cleanup" INT QUIT TERM EXIT


create_tap && start_uhcpd
