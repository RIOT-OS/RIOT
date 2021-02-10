#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import pexpect
import random
import sys
import time

from scapy.all import AsyncSniffer, sendp, Ether, IPv6, UDP
from scapy.all import DHCP6_Solicit, DHCP6_Advertise, DHCP6_Request, DHCP6_Reply
from scapy.all import DHCP6OptClientId, DHCP6OptServerId, DHCP6OptIA_PD, DHCP6OptUnknown
from scapy.all import DUID_LL, DHCP6OptIAPrefix
from testrunner import run


TIMEOUT = 1

MUD_OPTION_CODE = 112
MUD_TEST_URL = b'https://example.org'

# MUD URL option in DHCPv6 is not yet supported by scapy
DHCP6OptMUD = DHCP6OptUnknown


def get_upstream_netif(child):
    child.sendline("ifconfig")
    child.sendline("help")  # workaround to spot end of ifconfig output
    candidate = None
    while True:     # Search for an interface that does _not_ contain 6LO flag
        if candidate is None:
            child.expect(r"Iface\s+([^\s]+)\s+")
            candidate = child.match.group(1)
        res = child.expect([r"\b6LO\b", r"Iface\s+([^\s]+)\s+", "Command",
                            pexpect.TIMEOUT], timeout=.2)
        if res > 0:
            break
        candidate = None
    # wait for a line in "help"
    child.expect("reboot")
    return candidate


def get_downstream_netif(child):
    child.sendline("ifconfig")
    child.sendline("help")  # workaround to spot end of ifconfig output
    candidate = None
    while True:     # Search for an interface that does _not_ contain 6LO flag
        if candidate is None:
            child.expect(r"Iface\s+([^\s]+)\s+")
        candidate = child.match.group(1)
        res = child.expect([r"\b6LO\b", r"Iface\s+([^\s]+)\s+", "Command",
                            pexpect.TIMEOUT], timeout=.2)
        if res == 0:
            break
        elif res == 1:
            candidate = child.match.group(1)
        elif res == 2:
            break
        else:
            candidate = None
    # wait for a line in "help"
    child.expect("reboot")
    return candidate


def get_hwaddrs(child, netif):
    hwaddrs = []
    child.sendline("ifconfig {}".format(netif))
    child.expect(r"HWaddr:\s+(([A-Fa-f0-9]{2}:?)+)\s")
    hwaddrs.append(child.match.group(1).lower())
    if len(hwaddrs[0]) == 5:    # short address
        res = child.expect([pexpect.TIMEOUT,
                            r"Long HWaddr:\s+(([A-Fa-f0-9]{2}:?)+)\s"])
        if res > 0:
            hwaddrs.append(child.match.group(1).lower())
    return hwaddrs


def start_sniffer(iface, count=None, stop_filter=None):
    sniffer = AsyncSniffer(
        iface=iface,
        filter="udp and dst port 547",
        count=count,
        stop_filter=stop_filter,
    )
    sniffer.start()
    return sniffer


def wait_for_dhcpv6_pkt(iface, sniffer=None, timeout=5):
    if sniffer is None:
        sniffer = start_sniffer(iface, count=1)
    sniffer.join(timeout=timeout)
    if sniffer.results is None:
        raise TimeoutError("Sniffing for DHCPv6 traffic timed out")
    return [p for p in sniffer.results
            # filter out packets only belonging to stop_filter if it existed
            if sniffer.kwargs.get("stop_filter") is None or
            sniffer.kwargs["stop_filter"](p)][-1]


def build_reply_headers(pkt):
    src_ether = pkt[Ether].src
    src_ip = pkt[IPv6].src
    sport = pkt[UDP].sport
    dport = pkt[UDP].dport
    return Ether(dst=src_ether) / IPv6(dst=src_ip) / \
        UDP(sport=dport, dport=sport)


def testfunc(child):
    iface = os.environ["TAP"]

    pkt = wait_for_dhcpv6_pkt(iface)
    # the packet was a solicit
    assert DHCP6_Solicit in pkt
    # check if the sender is the upstream interface of the node
    upstream_netif = get_upstream_netif(child)
    print(upstream_netif)
    upstream_hwaddrs = get_hwaddrs(child, upstream_netif)
    assert DHCP6OptClientId in pkt and DUID_LL in pkt[DHCP6OptClientId].duid
    assert pkt[DHCP6OptClientId].duid[DUID_LL].lladdr in upstream_hwaddrs
    # and it is asking for a prefix delegation
    assert DHCP6OptIA_PD in pkt

    # reply to solicit with advertise and a prefix provided
    trid = pkt[DHCP6_Solicit].trid
    srv_duid = "aa:bb:cc:dd:ee:ff"
    cli_id = DHCP6OptClientId(duid=pkt[DHCP6OptClientId].duid)
    srv_id = DHCP6OptServerId(duid=DUID_LL(lladdr=srv_duid))
    prefix = "2001:db8:{:x}:{:x}::".format(
            random.randint(0, 0xffff),
            random.randint(0, 0xffff)
        )
    ia_pd = DHCP6OptIA_PD(T1=12000, T2=13000, iaid=pkt[DHCP6OptIA_PD].iaid,
                          iapdopt=[
                              DHCP6OptIAPrefix(preflft=14000, validlft=15000,
                                               prefix=prefix, plen=64)])
    # start sniffer to catch incoming request
    sniffer = start_sniffer(iface,
                            stop_filter=lambda pkt: DHCP6_Request in pkt)
    sendp(build_reply_headers(pkt) / DHCP6_Advertise(trid=trid) /
          ia_pd / cli_id / srv_id, iface=iface, verbose=False)

    # wait for request
    pkt = wait_for_dhcpv6_pkt(iface, sniffer)
    # the packet was indeed a request
    assert DHCP6_Request in pkt
    # and from the client
    assert DHCP6OptClientId in pkt and DUID_LL in pkt[DHCP6OptClientId].duid
    assert pkt[DHCP6OptClientId].duid[DUID_LL].lladdr in upstream_hwaddrs
    # and it is trying to talk to this server
    assert DHCP6OptServerId in pkt and DUID_LL in pkt[DHCP6OptServerId].duid
    assert pkt[DHCP6OptServerId].duid[DUID_LL].lladdr == srv_duid
    # and is still asking for a prefix delegation
    assert DHCP6OptIA_PD in pkt

    assert DHCP6OptMUD in pkt
    mud_option = pkt[DHCP6OptMUD]
    assert mud_option.optcode == 112
    assert mud_option.optlen == len(MUD_TEST_URL)
    assert mud_option.data == MUD_TEST_URL

    # reply to request with reply and a prefix provided
    trid = pkt[DHCP6_Request].trid
    sendp(build_reply_headers(pkt) / DHCP6_Reply(trid=trid) /
          ia_pd / cli_id / srv_id, iface=iface, verbose=False)
    time.sleep(1)

    # check if global address was configured
    child.sendline("ifconfig {}".format(get_downstream_netif(child)))
    # remove one trailing ':' from prefix just to be safe ;-)
    child.expect(r"inet6 addr:\s+{}[0-9a-fA-F:]+\s"
                 .format(prefix[:-1]))
    print("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT, echo=True))
