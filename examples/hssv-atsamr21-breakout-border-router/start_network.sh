#+-------------------------------------------------------------------------------------------------+
#| Host configuration script for the border router based on hssv-atsamr21-breakout with CDC-ECM.   |
#|                                                                                                 |
#| This is based on the start_network.sh script from ethos.                                        |
#| Modified by: Joksan Alvarado.                                                                   |
#+-------------------------------------------------------------------------------------------------+

#!/bin/sh

configure_border_interface() {
    sysctl -w net.ipv6.conf.${BORDER_IF}.forwarding=1
    sysctl -w net.ipv6.conf.${BORDER_IF}.accept_ra=0
    ip a a fe80::1/64 dev ${BORDER_IF}
    ip a a fd00:dead:beef::1/128 dev lo
    ip route add ${PREFIX} via fe80::2 dev ${BORDER_IF}
}

restore_border_interface() {
    sysctl -w net.ipv6.conf.${BORDER_IF}.forwarding=0
    sysctl -w net.ipv6.conf.${BORDER_IF}.accept_ra=1
    ip a d fe80::1/64 dev ${BORDER_IF}
    ip route del ${PREFIX} via fe80::2 dev ${BORDER_IF}
}

cleanup() {
    echo "Cleaning up..."
    restore_border_interface
    ip a d fd00:dead:beef::1/128 dev lo
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ${UHCPD} ${BORDER_IF} ${PREFIX}
}

BORDER_IF=$1
PREFIX=$2
UHCPD="$(readlink -f "../../dist/tools/uhcpd/bin")/uhcpd"

[ -z "${BORDER_IF}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <border-interface> <prefix>"
    exit 1
}

trap "cleanup" INT QUIT TERM EXIT


configure_border_interface && start_uhcpd
