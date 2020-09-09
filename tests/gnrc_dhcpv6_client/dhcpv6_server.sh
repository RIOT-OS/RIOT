#! /bin/bash
#
# random_port.sh
# Copyright (C) 2018 Martine Lenders <m.lenders@fu-berlin.de>
#
# Distributed under terms of the MIT license.
#

if [ "$EUID" -ne 0 ]; then
  echo -e "\033[31;1mRequire root since kea uses some PID and Lock files in /var\033[0m" >&2
  exit 1
fi

if ! command -v kea-dhcp6; then
  echo -e "\033[31;1mCommand kea-dhcp6 required\033[0m" >&2
  exit 1
fi

_dhcpv6_server() {
    sleep 1 # sleep to let TAP become active
    sudo kea-dhcp6 -p $1 -c $2
}

# no need to kill from external, kea handles double instances gracefully
_dhcpv6_server $1 $2 &
