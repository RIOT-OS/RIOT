#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import re
import sys
import subprocess

from scapy.all import Ether, IPv6, ICMPv6ND_RA, ICMPv6NDOptRDNSS, sendp
from testrunner import run


def check_and_search_output(cmd, pattern, res_group, *args, **kwargs):
    output = subprocess.check_output(cmd, *args, **kwargs).decode("utf-8")
    for line in output.splitlines():
        m = re.search(pattern, line)
        if m is not None:
            return m.group(res_group)
    return None


def get_bridge(tap):
    res = check_and_search_output(
            ["bridge", "link"],
            r"{}.+master\s+(?P<master>[^\s]+)".format(tap),
            "master"
        )
    return tap if res is None else res


def get_host_lladdr(tap):
    res = check_and_search_output(
            ["ip", "addr", "show", "dev", tap, "scope", "link"],
            r"inet6\s+(?P<lladdr>[0-9A-Fa-f:]+)/\d+",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def dns_server(child):
    child.sendline("dns server")
    res = child.expect([r"DNS server: -",
                        r"DNS server: \[([0-9a-f:]+)%\d+\]:(\d+)"])
    if res > 0:
        return child.match.group(1), int(child.match.group(2))
    else:
        return None


DNS_ADDR = "2001:db8:affe::dead:beef"
DNS_PORT = 53


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])
    lladdr_src = get_host_lladdr(tap)
    child.sendline("ifconfig")
    child.expect(r"HWaddr: (?P<hwaddr>[A-Fa-f:0-9]+)\s")
    hwaddr_dst = child.match.group("hwaddr").lower()
    child.expect(r"(?P<lladdr>fe80::[A-Fa-f:0-9]+)\s")
    lladdr_dst = child.match.group("lladdr").lower()

    assert dns_server(child) is None
    sendp(Ether(dst=hwaddr_dst) / IPv6(dst=lladdr_dst, src=lladdr_src) /
          ICMPv6ND_RA() / ICMPv6NDOptRDNSS(dns=[DNS_ADDR]),
          iface=tap)
    assert dns_server(child) == (DNS_ADDR, DNS_PORT)


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=True))
