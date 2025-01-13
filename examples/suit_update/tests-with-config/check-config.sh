#! /bin/sh
#
# check_config.sh
# Copyright (C) 2021 Martine Lenders <mail@martine-lenders.eu>
# Copyright (C) 2022 Inria
#
# Distributed under terms of the MIT license.
#

ip link show dev "${IFACE}" > /dev/null
RESULT=$?

if [ $RESULT -eq 0 ]; then
    if [ "${BOARD}" = "native" ] || [ "${BOARD}" = "native32" ]; then
        IFACE_IPV6_ADDR=$(ip -6 addr show dev "${IFACE}"| grep inet6 | \
                          awk -F '[ \t]+|/' '{print $3}' | grep -v ^::1 | \
                          grep -v ^fe80)
        if [ -n "${IFACE_IPV6_ADDR}" ]; then
            exit 0
        fi
    else
        exit 0
    fi
fi

if [ "${BOARD}" = "native" ] || [ "${BOARD}" = "native32" ]; then
    echo "You may be able to create \"${IFACE}\" by using e.g." \
         "\`${RIOTTOOLS#${RIOTBASE}/}/tapsetup/tapsetup\`."
    echo "You can add a routable IPV6 address by using e.g." \
         "sudo ip address add 2001:db8::1/64 dev ${IFACE}"
else
    echo "You may setup \"${IFACE}\" by using e.g." \
         "\`${RIOTTOOLS#${RIOTBASE}/}/ethos/setup_network.sh ${IFACE} 2001:db8::/64\`"
fi

exit 1
