#! /bin/sh
#
# check_config.sh
# Copyright (C) 2021 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.
#

ip link show dev "${IFACE}" > /dev/null
RESULT=$?

if [ $RESULT -ne 0 ]; then
    echo "You may be able to create \"${IFACE}\" by using e.g." \
         "\`${RIOTTOOLS#${RIOTBASE}/}/tapsetup/tapsetup\`."
fi

exit $RESULT
