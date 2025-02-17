#!/bin/bash

PREFIX=2001:db8::/60

SCRIPTPATH=$(dirname $(realpath "$0"))

RIOTBASE=$SCRIPTPATH/../..
RIOTTOOLS=$RIOTBASE/dist/tools
TOPOLOGY=$SCRIPTPATH/tapology.txt

setup() {
    echo "creating tap interfaces"
    i=0
    sed '/^#/d' "$1" | while read -r level num; do
        # we actually want to execute the output here.
        # shellcheck disable=SC2091
        $(printf -- "sudo %s/tapsetup/tapsetup -b br%s -t tap_%s -c %s\n" "$RIOTTOOLS" "$i" "$level" "$num") > /dev/null;
        i=$((i+1))
    done

    # add address to br0 so this can be tested by pinging the host system
    sudo ip addr add ${PREFIX/::\//::1\/} dev br0

    # start radvd with a large prefix
    sudo "$RIOTTOOLS"/radvd/radvd.sh -c br0 $PREFIX
}

teardown() {
    echo "deleting tap interfaces"
    i=0
    sed '/^#/d' "$1" | while read -r level num; do
        $(printf -- "sudo %s/tapsetup/tapsetup -b br%s -t tap_%s -d\n" "$RIOTTOOLS" "$i" "$level") > /dev/null;
        i=$((i+1))
    done

    # stop radvd
    sudo "$RIOTTOOLS"/radvd/radvd.sh -d
}

if [ $# -gt 1 ]; then
    TOPOLOGY=$2
fi

if [ ! -f "$TOPOLOGY" ]; then
    echo "no such file: $TOPOLOGY"
    exit 1
fi

if [ $# -gt 0 ]; then
    case $1 in
    -c)
        ;;
    -d)
        teardown "$TOPOLOGY"
        exit
        ;;
    *)
        echo "usage: $0 [-c <topology>] [-d topology]"
        exit 1
        ;;
    esac
fi

setup "$TOPOLOGY"
