#! /bin/sh

# SPDX-FileCopyrightText: 2018 Martine Lenders <m.lenders@fu-berlin.de>
# SPDX-License-Identifier: MIT

ip link show dev "${IFACE}" > /dev/null
RESULT=$?

if [ $RESULT -ne 0 ]; then
    echo "You may be able to create \"${IFACE}\" by using e.g." \
         "\`${RIOTTOOLS#${RIOTBASE}/}/tapsetup/tapsetup\`."
fi

exit $RESULT
