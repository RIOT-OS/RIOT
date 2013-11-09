#!/bin/sh

COMMAND=${1}
COUNT=${2}

DEFCOUNT="2"
DEFBRNAME="tapbr0"
DEFTAPNAME="tap"

if [ -z "${USER}" ]; then
    echo 'need to export $USER'
    exit 1
fi
if [ -z "${COMMAND}" ]; then
    echo "usage: $(basename $0) <create [count]|delete>"
    exit 1
fi

if [ -z "${BRNAME}" ]; then
    BRNAME="${DEFBRNAME}"
fi
if [ -z "${TAPNAME}" ]; then
    TAPNAME="${DEFTAPNAME}"
fi

if [ "${COMMAND}" = 'create' ]; then
    if [ -z "${COUNT}" ]; then
        COUNT="${DEFCOUNT}"
    fi

    echo "creating ${BRNAME} ..."
    sudo brctl addbr ${BRNAME} || exit 1
    sudo -s sh -c "echo 1  > /proc/sys/net/ipv6/conf/${BRNAME}/disable_ipv6" || exit 1
    sudo ip link set ${BRNAME} up || exit 1

    for N in $(seq 0 "$((COUNT - 1))"); do
        echo "creating ${TAPNAME}${N} ..."
        sudo ip tuntap add dev ${TAPNAME}${N} mode tap user ${USER} || exit 1
        sudo -s sh -c "echo 1 > /proc/sys/net/ipv6/conf/${TAPNAME}${N}/disable_ipv6" || exit 1
        sudo brctl addif ${BRNAME} ${TAPNAME}${N} || exit 1
        sudo ip link set ${TAPNAME}${N} up || exit 1
    done

elif [ "${COMMAND}" = 'delete' ]; then
    for IF in $(ls /sys/class/net/${BRNAME}/brif); do
        echo "deleting ${IF} ..."
        sudo ip link delete "${IF}"
    done
    echo "deleting ${BRNAME} ..."
    sudo ip link set ${BRNAME} down
    sudo brctl delbr ${BRNAME}

else
    echo 'unknown command'
    exit 1
fi

exit 0
