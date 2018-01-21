#!/bin/sh

ETHOS_DIR="$(dirname $(readlink -f "$0"))"
ETHOS="${ETHOS_DIR}/ethos"
UHCPD="$(readlink -f "${ETHOS_DIR}/../uhcpd/bin")/uhcpd"
PORT=$1
TAP=$2
PREFIX=$3
BAUDRATE=115200
ETHOS_PID=
UHCPD_PID=

create_tap() {
    ip tuntap add "${TAP}" mode tap user "${USER}"
    sysctl -w "net.ipv6.conf.${TAP}.forwarding=1"
    sysctl -w "net.ipv6.conf.${TAP}.accept_ra=0"
    ip link set "${TAP}" up
    ip a a fe80::1/64 dev "${TAP}"
    ip a a fd00:dead:beef::1/128 dev lo
    ip route add "${PREFIX}" via fe80::2 dev "${TAP}"
}

remove_tap() {
    ip tuntap del ${TAP} mode tap
}

cleanup() {
    echo "Cleaning up..."
    ip a d fd00:dead:beef::1/128 dev lo
    kill -0 "${UHCPD_PID}" >/dev/null 2>&1 && kill -TERM "${UHCPD_PID}"
    kill -0 "${ETHOS_PID}" >/dev/null 2>&1 && kill -TERM "${ETHOS_PID}"
    remove_tap
    trap "" INT QUIT TERM EXIT
    RUNNING=0
}

start_uhcpd() {
    "${UHCPD}" "${TAP}" "${PREFIX}" > /dev/null &
    UHCPD_PID=$!
}


[ -z "${PORT}" -o -z "${TAP}" -o -z "${PREFIX}" ] && {
    echo "usage: $0 <serial-port> <tap-device> <prefix> [baudrate]"
    exit 1
}

[ -n "$4" ] && {
    BAUDRATE=$4
}

RUNNING=1
trap "cleanup" INT QUIT TERM EXIT

create_tap
start_uhcpd
if kill -0 "${UHCPD_PID}" >/dev/null 2>&1; then
    echo "uhcpd running, pid ${UHCPD_PID}"
else
    echo "uhcpd failed to start" >&2
    exit 1
fi
# Run ethos as a background job, but redirect the stdin of this script to the
# ethos process (using <&0).
# We need to do this to remember the ethos PID to be able to clean up if this
# script is terminated from the outside (for example when using kill -TERM $PID),
# or else ethos may remain running after the script has been terminated.
# Observed when running start_network.sh from within another script and the other
# script uses kill to tell this script to quit.
"${ETHOS}" "${TAP}" "${PORT}" "${BAUDRATE}" <&0 &
ETHOS_PID=$!
if kill -0 "${ETHOS_PID}" >/dev/null 2>&1; then
    echo "ethos running, pid ${ETHOS_PID}"
else
    echo "ethos failed to start" >&2
    exit 1
fi
# Wait until ethos has been terminated, or we receive a signal to terminate.
# Ethos will close if the user presses CTRL+D. We will receive a signal when the
# user presses CTRL+C, or when using kill <thispid> externally.
while [ "$RUNNING" = "1" ]; do
    wait "${ETHOS_PID}"
    STATUS=$?
    if [ $STATUS -gt 128 ]; then
        # wait caught some signal, continue until ethos has been terminated by
        # the cleanup function (which sets RUNNING=0).
        continue
    else
        # ethos terminated with the given exit code
        break
    fi
done
# Return the ethos exit code as the script exit code
exit $STATUS
