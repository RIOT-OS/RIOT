#!/bin/sh

######################################################################
# Linux functions
######################################################################

Linux_bridge_default () {
    echo tapbr0
}

Linux_bridge_exists () {
    if [ -e /sys/class/net/${BRIDGE} ]; then
        return 0
    else
        return 1
    fi
}

Linux_bridge_create () {
    sudo brctl addbr ${BRIDGE} || exit 1
    sudo -s sh -c "echo 1  > /proc/sys/net/ipv6/conf/${BRIDGE}/disable_ipv6" || exit 1
    sudo ip link set ${BRIDGE} up || exit 1
}

Linux_port_define () {
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
}

Linux_port_create () {
    sudo ip tuntap add dev ${PORT} mode tap user ${USER} || exit 1
    sudo -s sh -c "echo 1 > /proc/sys/net/ipv6/conf/${PORT}/disable_ipv6" || exit 1
    sudo brctl addif ${BRIDGE} ${PORT} || exit 1
    sudo ip link set ${PORT} up || exit 1
}

Linux_port_exists () {
    if [ -e /sys/class/net/${BRIDGE}/brif/${PORT} ]; then
        return 0
    else
        return 1
    fi
}

######################################################################
# OSX functions
######################################################################

Darwin_default_bridge () {
    echo bridge1234
}

Darwin_bridge_create () {
    sudo ifconfig ${BRIDGE} create || exit 1
    echo "upping ${BRIDGE}"
    sudo ifconfig ${BRIDGE} up || exit 1
}

Darwin_port_create () {
    sudo chown ${USER} /dev/${PORT} || exit 1
    echo "start RIOT instance for ${PORT} now and hit enter"
    read
    sudo ifconfig ${BRIDGE} addm ${PORT} || exit 1
    sudo ifconfig ${PORT} up
}

##
# THE SCRIPT
##

OS=$(uname -s)

DEFBRIDGE=$(${OS}_default_bridge)
DEFTAPBASE="tap"

# make sure this is not called as root
if [ ${EUID} -eq 0 -o "$(ps -p ${PPID} -o comm=)" = "sudo" -o "$(ps -p ${PPID} -o comm=)" = "su" ]; then
    echo "Error: ${0} must not be run as root"
    exit 1
fi
if [ -z "${USER}" ]; then
    echo 'need to export $USER'
    exit 1
fi

# set defaults
if [ -z "${BRIDGE}" ]; then
    BRIDGE="${DEFBRIDGE}"
fi
if [ -z "${TAPBASE}" ]; then
    TAPBASE="${DEFTAPBASE}"
fi

if ! ${OS}_bridge_exists; then
    ${OS}_bridge_create
fi

if [ -z "${PORT}" ]; then
    ${OS}_port_define
fi

if ! ${OS}_port_exists; then
    ${OS}_port_create
fi

echo ${PORT}
