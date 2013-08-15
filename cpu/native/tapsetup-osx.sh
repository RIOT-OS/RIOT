#!/bin/sh

COMMAND=${1}
COUNT=${2}

DEFCOUNT="2"
DEFBRNAME="bridge1234"

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

if [ "${COMMAND}" = 'create' ]; then
    if [ -z "${COUNT}" ]; then
        COUNT="${DEFCOUNT}"
    fi

    sudo ifconfig ${BRNAME} create || exit 1
    echo "upping ${BRNAME}"
    sudo ifconfig ${BRNAME} up || exit 1

    for N in $(seq 0 "$((COUNT - 1))"); do
        sudo chown ${USER} /dev/tap${N} || exit 1
        echo "start RIOT instance for tap${N} now and hit enter"
        read
        sudo ifconfig ${BRNAME} addm tap${N} || exit 1
        sudo ifconfig tap${N} up
    done

elif [ "${COMMAND}" = 'delete' ]; then
    sudo ifconfig ${BRNAME} destroy

else
    echo 'unknown command'
    exit 1
fi

exit 0
