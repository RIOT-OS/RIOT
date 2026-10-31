#!/bin/sh
#
# SPDX-FileCopyrightText: 2025 ML!PA Consulting GmbH
# SPDX-License-Identifier: LGPL-2.1-only

SCRIPT_DIR="$(dirname $(readlink -f $0))"
RIOTBASE=${RIOTBASE:-${SCRIPT_DIR}/../../..}

init_iface() {
  sudo sysctl -w net.ipv6.conf.${IFACE}.forwarding=1
  sudo sysctl -w net.ipv6.conf.${IFACE}.accept_ra=0
  sudo ip a a fe80::${PREFIX_IID}/64 dev ${IFACE}
  sudo ip a a ${PREFIX}${PREFIX_IID}/128 dev ${IFACE}
  sudo ip link set ${IFACE} up
}

deinit_iface() {
  sudo ip a d fe80::${PREFIX_IID}/64 dev ${IFACE}
  sudo ip a d ${PREFIX}${PREFIX_IID}/128 dev ${IFACE}
}

cleanup() {
  echo "Cleaning up..."
  deinit_iface
  trap "" INT QUIT TERM EXIT
}

start_radvd() {
  sudo sysctl net.ipv6.conf."${IFACE}".accept_ra=2
  sudo sysctl net.ipv6.conf."${IFACE}".accept_ra_rt_info_max_plen=64
  sudo ${RADVD} -c ${IFACE} ${PREFIX}/${PREFIX_LEN}
}

USE_RADVD=0
POSITIONAL_ARGS=

while [ $# -gt 0 ]; do
  case $1 in
    -h|--help)
      echo "Environment variables:"
      echo ""
      echo "Usage: $0 [-r|--radvd] <PREFIX_WITH_LENGTH>"
      echo "          [-i|--iface] <INTERFACE_NAME>"
      exit 0
      ;;
    -r|--radvd)
      USE_RADVD=1
      ARG_PREFIX="$2"
      shift # past argument
      shift # past value
      ;;
    -i|--iface)
      ARG_IFACE="$2"
      shift # past argument
      shift # past value
      ;;
    *)
      POSITIONAL_ARGS="$POSITIONAL_ARGS $1"
      shift # past argument
      ;;
  esac
done

eval "set -- $POSITIONAL_ARGS"

IFACE="${1:-tap0}"
PREFIX_IID="1"

if [ ! -z ${ARG_IFACE} ]; then
  IFACE=${ARG_IFACE}
else
  IFACE="tap0"
fi

if [ ! -z ${ARG_PREFIX} ]; then
  PREFIX=$(echo "$ARG_PREFIX" | cut -d'/' -f1)
  PREFIX_LEN=$(echo "$ARG_PREFIX" | cut -d'/' -f2)
else
  PREFIX="fd00:dead:beef::"
  PREFIX_LEN="64"
fi

trap "cleanup" INT QUIT TERM EXIT

init_iface

if [ ${USE_RADVD} -eq 1 ]; then
  RADVD=${RIOTBASE}/dist/tools/radvd/radvd.sh
  start_radvd
fi

# calls TERMPROG with TERMFLAGS
${POSITIONAL_ARGS}
