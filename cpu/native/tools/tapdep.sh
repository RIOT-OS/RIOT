#!/bin/sh

DEFBRIDGE="tapbr0"
DEFTAPBASE="tap"

if [ ${EUID} -eq 0 -o "$(ps -p ${PPID} -o comm=)" = "sudo" -o "$(ps -p ${PPID} -o comm=)" = "su" ]; then
    echo "Error: ${0} must not be run as root"
    exit 1
fi
if [ -z "${USER}" ]; then
    echo 'need to export $USER'
    exit 1
fi

if [ -z "${BRIDGE}" ]; then
    BRIDGE="${DEFBRIDGE}"
fi
if [ -z "${TAPBASE}" ]; then
    TAPBASE="${DEFTAPBASE}"
fi

if [ ! -e /sys/class/net/${BRIDGE} ]; then
    # ${BRIDGE} does not exist yet, create it ...
    sudo brctl addbr ${BRIDGE} || exit 1
    sudo -s sh -c "echo 1  > /proc/sys/net/ipv6/conf/${BRIDGE}/disable_ipv6" || exit 1
    sudo ip link set ${BRIDGE} up || exit 1
fi

if [ -z "${PORT}" ]; then

    # try to find an existing unused interface ..
    for IF in $(ls /sys/class/net/${BRIDGE}/brif/); do
        if [ "$(cat /sys/class/net/${BRIDGE}/brif/${IF}/state)" = 0 ]; then
            PORT=${IF}
            break
        fi
    done

    # .. or create a new one
    if [ -z "${PORT}" ]; then
        COUNT=$(ls /sys/class/net/${BRIDGE}/brif/ | sort -n | wc -l)
        PORT=${TAPBASE}${COUNT}
    fi

fi

if [ ! -e /sys/class/net/${BRIDGE}/brif/${PORT} ]; then
    # ${PORT} does not exist, create it
    sudo ip tuntap add dev ${PORT} mode tap user ${USER} || exit 1
    sudo -s sh -c "echo 1 > /proc/sys/net/ipv6/conf/${PORT}/disable_ipv6" || exit 1
    sudo brctl addif ${BRIDGE} ${PORT} || exit 1
    sudo ip link set ${PORT} up || exit 1
fi

echo ${PORT}
