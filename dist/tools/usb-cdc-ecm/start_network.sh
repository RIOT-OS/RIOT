#!/usr/bin/env bash

USB_CDC_ECM_DIR="$(dirname "$(readlink -f "$0")")"

INTERFACE_CHECK_COUNTER=5  # 5 attempts to find usb interface

find_interface() {
    INTERFACE=$(ls -A /sys/bus/usb/drivers/cdc_ether/*/net/ 2>/dev/null)
    INTERFACE_CHECK=$(echo -n "${INTERFACE}" | head -c1 | wc -c)
    if [ "${INTERFACE_CHECK}" -eq 0 ] && [ ${INTERFACE_CHECK_COUNTER} != 0 ]; then
        # We want to have multiple opportunities to find the USB interface
        # as sometimes it can take a few seconds for it to enumerate after
        # the device has been flashed.
        sleep 1
        ((INTERFACE_CHECK_COUNTER=INTERFACE_CHECK_COUNTER-1))
        find_interface
    fi
    INTERFACE=${INTERFACE%/}
}

echo "Waiting for network interface."
find_interface

if [ "${INTERFACE_CHECK}" -eq 0 ]; then
    echo "Unable to find network interface"
    exit 1
else
    echo "Found interface: ${INTERFACE}"
fi

setup_interface() {
    sysctl -w net.ipv6.conf."${INTERFACE}".forwarding=1
    sysctl -w net.ipv6.conf."${INTERFACE}".accept_ra=0
    ip link set "${INTERFACE}" up
    ip a a fe80::1/64 dev "${INTERFACE}"
    ip a a fd00:dead:beef::1/128 dev lo
    ip route add "${PREFIX}" via fe80::2 dev "${INTERFACE}"
}

cleanup_interface() {
    ip a d fe80::1/64 dev "${INTERFACE}"
    ip a d fd00:dead:beef::1/128 dev lo
    ip route del "${PREFIX}" via fe80::2 dev "${INTERFACE}"
}

cleanup() {
    echo "Cleaning up..."
    cleanup_interface
    if [ -n "${UHCPD_PID}" ]; then
        kill "${UHCPD_PID}"
    fi
    if [ -n "${DHCPD_PIDFILE}" ]; then
        kill "$(cat "${DHCPD_PIDFILE}")"
        rm "${DHCPD_PIDFILE}"
    fi
    trap "" INT QUIT TERM EXIT
}

start_uhcpd() {
    ${UHCPD} "${INTERFACE}" "${PREFIX}" > /dev/null &
    UHCPD_PID=$!
}

start_dhcpd() {
    DHCPD_PIDFILE=$(mktemp)
    ${DHCPD} -d -p "${DHCPD_PIDFILE}" "${INTERFACE}" "${PREFIX}" 2> /dev/null
}

if [ "$1" = "-d" ] || [ "$1" = "--use-dhcpv6" ]; then
    USE_DHCPV6=1
    shift 1
else
    USE_DHCPV6=0
fi

PREFIX=$1
[ -z "${PREFIX}" ] && {
    echo "usage: $0 [-d|--use-dhcpv6] <prefix> [<serial-port>]"
    exit 1
}

if [ -n "$2" ]; then
    PORT=$2
fi

trap "cleanup" INT QUIT TERM EXIT

setup_interface

if [ ${USE_DHCPV6} -eq 1 ]; then
    DHCPD="$(readlink -f "${USB_CDC_ECM_DIR}/../dhcpv6-pd_ia/")/dhcpv6-pd_ia.py"
    start_dhcpd
else
    UHCPD="$(readlink -f "${USB_CDC_ECM_DIR}/../uhcpd/bin")/uhcpd"
    start_uhcpd
fi

if [ -z "${PORT}" ]; then
    echo "Network enabled over CDC-ECM"
    echo "Press Return to stop"
    read -r
else
    "${USB_CDC_ECM_DIR}/../pyterm/pyterm" -p "${PORT}"
fi
