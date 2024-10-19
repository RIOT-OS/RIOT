#!/bin/sh

SLIPTTY_DIR="$(cd "$(dirname "$0")" && pwd -P)"
UHCPD="$(cd "${SLIPTTY_DIR}/../uhcpd/bin" && pwd -P)/uhcpd"
DHCPD="$(cd "${SLIPTTY_DIR}/../dhcpv6-pd_ia/" && pwd -P)/dhcpv6-pd_ia.py"
RADVD="$(cd "${SLIPTTY_DIR}/../radvd/" && pwd -P)/radvd.sh"
TUN=sl0
TUN_GLB="fd00:dead:beef::1/128"
UHCPD_PID=
CREATED_IFACE=0
START_SLIP=1

SUDO=${SUDO:-sudo}

unsupported_platform() {
    echo "unsupported platform" >&2
    echo "(currently supported \`uname -s\` 'Darvin' and 'Linux')" >&2
}

case "$(uname -s)" in
    Darvin)
        PLATFORM="OSX";;
    Linux)
        PLATFORM="Linux";;
    *)
        unsupported_platform
        exit 1
        ;;
esac

create_tun() {
    case "${PLATFORM}" in
        Linux)
            if ! ip link show ${TUN} 2>&1 > /dev/null; then
                ${SUDO} ip tuntap add ${TUN} mode tun user ${USER} || exit 1
                CREATED_IFACE=1
            fi
            ${SUDO} sysctl -w net.ipv6.conf.all.forwarding=1
            echo "Activated forwarding for all interfaces." >&2
            echo "Deactivate with" >&2
            echo "    ${SUDO} sysctl -w net.ipv6.conf.all.forwarding=0" >&2
            echo "when not desired without this script" >&2
            ${SUDO} sysctl -w net.ipv6.conf.${TUN}.accept_ra=2
            ${SUDO} sysctl -w net.ipv6.conf."${TUN}".accept_ra_rt_info_max_plen=64
            ${SUDO} ip link set ${TUN} up || exit 1
            ${SUDO} ip address add fe80::1/64 dev ${TUN}
            ${SUDO} ip address add ${TUN_GLB} dev ${TUN}
            ${SUDO} ip neigh add fe80::2 dev ${TUN}
            ${SUDO} ip route add ${PREFIX} via fe80::2 dev ${TUN}
            ;;
        OSX)
            # TUN interface in OSX needs to be called tunX
            TUN="tun$(echo "${TUN}" | grep -o '[0-9]\+$')"
            ${SUDO} chown ${USER} /dev/${TUN} || exit 1
            ${SUDO} sysctl -w net.inet6.ip6.forwarding=1
            echo "Activated forwarding for all interfaces." >&2
            echo "Deactivate with" >&2
            echo "    ${SUDO} sysctl -w net.inet6.ip6.forwarding=0" >&2
            echo "when not desired without this script" >&2
            echo "start RIOT instance for ${TUN} now and hit enter"
            read _
            ${SUDO} ifconfig ${TUN} up || exit 1
            ${SUDO} ifconfig ${TUN} inet6 fe80::1 prefixlen 64
            TUN_GLB_ADDR=$(echo "${TUN_GLB}" | cut -d/ -f1)
            TUN_GLB_LEN=$(echo "${TUN_GLB}" | cut -d/ -f2)
            if [ -z "${TUN_GLB_ADDR}" ] || [ -z "${TUN_GLB_LEN}" ]; then
                echo "TUN global address (-g) must be of format " \
                     "<addr>/<prefix_len>" >&2
                exit 1
            fi
            ${SUDO} ifconfig ${TUN} inet6 fe80::1 prefixlen 64
            ${SUDO} ifconfig ${TUN} inet6 ${TUN_GLB_ADDR} prefixlen \
                ${TUN_GLB_LEN}
            ${SUDO} route -n add -interface ${TUN} -inet6 -prefixlen 64 \
                    ${PREFIX} fe80::2 || exit 1
            ;;
    esac
    return 0
}

remove_tun() {
    case "${PLATFORM}" in
        Linux)
            ${SUDO} ip tuntap del ${TUN} mode tun
            ;;
        OSX)
            ${SUDO} route delete -inet6 ${PREFIX} -prefixlen 64 fe80::2
            ;;
    esac
}

cleanup() {
    if [ 1 -eq "${CREATED_IFACE}" ]; then
        remove_tun
    fi
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
    "${UHCPD}" "${TUN}" "${PREFIX}" > /dev/null &
    UHCPD_PID=$!
}

start_dhcpd() {
    DHCPD_PIDFILE=$(mktemp)
    ${DHCPD} -d -p ${DHCPD_PIDFILE} ${TUN} ${PREFIX} 2> /dev/null
}

start_radvd() {
    "${RADVD}" -c "${TUN}" "${PREFIX}"
}

usage() {
    echo "usage: $1 [-I <sl0>] [-d] [-r] [-e] [-g <addr>/<prefix_len>] <prefix> serial [baudrate]"
    echo "usage: $1 [-I <sl0>] [-d] [-r] [-e] [-g <addr>/<prefix_len>] <prefix> tcp:host [port]"
}

trap "cleanup" INT QUIT TERM EXIT

SLIP_ONLY=0
USE_DHCPV6=0
USE_RADVD=0

while getopts derI:g:h opt; do
    case ${opt} in
        d)  USE_DHCPV6=1; shift 1;;
        e)  SLIP_ONLY=1; shift 1;;
        r)  USE_RADVD=1; shift 1;;
        I)  TUN=${OPTARG}; shift 2;;
        g)  TUN_GLB=${OPTARG}; shift 2;;
        h)  usage $0; exit 0;;
    esac
done

PREFIX="$1"
if [ -z "$PREFIX" ]; then
    usage $0
    exit 1
fi
shift

create_tun && \
if [ ${SLIP_ONLY} -ne 1 ]; then
    if [ ${USE_DHCPV6} -eq 1 ]; then
        start_dhcpd
        START_SLIP=$?
    elif [ ${USE_RADVD} -eq 1 ]; then
        start_radvd
        START_SLIP=$?
    else
        start_uhcpd
        START_SLIP=$?
    fi
else
    START_SLIP=0
fi

[ ${START_SLIP} -eq 0 ] && "${SLIPTTY_DIR}"/sliptty -I "${TUN}" "$@"
