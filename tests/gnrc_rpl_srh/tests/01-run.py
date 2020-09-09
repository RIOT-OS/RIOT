#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import random
import re
import sys
import subprocess

from scapy.all import Ether, IPv6, UDP, \
                      IPv6ExtHdrHopByHop, IPv6ExtHdrDestOpt, \
                      IPv6ExtHdrFragment, IPv6ExtHdrRouting, \
                      ICMPv6ParamProblem, ICMPv6TimeExceeded, \
                      sendp, srp1, AsyncSniffer
from testrunner import run, check_unittests


EXT_HDR_NH = {
    IPv6ExtHdrHopByHop: 0,
    IPv6ExtHdrRouting: 43,
    IPv6ExtHdrFragment: 44,
    # IPSec headers currently unsupported by scapy
    IPv6ExtHdrDestOpt: 60,
    # Mobility header currently unsupported by scapy
  }


class Sniffer(object):
    def __init__(self, iface, *args, **kwargs):
        self.iface = iface
        self.sniffer = None
        self.stop_filter = None

    def start_sniff(self, stop_filter):
        assert self.sniffer is None
        self.stop_filter = stop_filter
        self.sniffer = AsyncSniffer(
            iface=self.iface,
            stop_filter=stop_filter,
        )
        self.sniffer.start()

    def wait_for_sniff_results(self, timeout=5):
        assert self.sniffer is not None
        self.sniffer.join(timeout=timeout)
        sniffer = self.sniffer
        self.sniffer = None
        if sniffer.results is None:
            return []
        return [p for p in sniffer.results
                # filter out packets only belonging to stop_filter if
                # it existed
                if sniffer.kwargs.get("stop_filter") is None or
                sniffer.kwargs["stop_filter"](p)]


sniffer = None


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


def get_host_hwaddr(tap):
    res = check_and_search_output(
            ["ip", "addr", "show", "dev", tap, "scope", "link"],
            r"link[^\s]+\s+(?P<hwaddr>[0-9A-Fa-f:]+)",
            "hwaddr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host hardware address on interface {}".format(tap)
            )
    else:
        return res


def pktbuf_empty(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    first_byte = child.match.group("first_byte")
    size = child.match.group("size")
    child.expect(
            r"~ unused: {} \(next: (\(nil\)|0), size: {}\) ~".format(
                first_byte, size))


def register_protnum(child):
    child.sendline("ip reg")
    child.expect("Registered to protocol number 59")


def unregister(child):
    child.sendline("ip unreg")
    child.expect(r"Unregistered from protocol number \d")


def get_first_interface(child):
    child.sendline("ifconfig")
    child.expect(r"Iface\s+(\d+)\s")
    return int(child.match.group(1))


def add_ipv6_address(child, iface, ipv6_addr):
    child.sendline("ifconfig {} add {}".format(iface, ipv6_addr))
    child.expect(r"success: added [a-f0-9:]+/\d+ to interface \d+")


def del_ipv6_address(child, iface, ipv6_addr):
    child.sendline("ifconfig {} del {}".format(iface, ipv6_addr))
    child.expect(r"success: removed [a-f0-9:]+ to interface \d+")


def add_neighbor(child, iface, ipv6_addr, hw_addr):
    child.sendline("nib neigh add {} {} {}".format(iface, ipv6_addr, hw_addr))
    child.sendline("nib neigh")
    child.expect(r"{} dev #{} lladdr {}".format(ipv6_addr.lower(), iface,
                                                hw_addr.upper()))


def del_neighbor(child, iface, ipv6_addr):
    child.sendline("nib neigh del {} {}".format(iface, ipv6_addr))


def test_wrong_type(child, iface, hw_dst, ll_dst, ll_src):
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrRouting(type=255, segleft=1, addresses=["abcd::1"]),
             iface=iface, timeout=1, verbose=0)
    assert(p is not None)
    assert(ICMPv6ParamProblem in p)
    assert(p[ICMPv6ParamProblem].code == 0)     # erroneous header field encountered
    assert(p[ICMPv6ParamProblem].ptr == 42)     # routing header type field
    pktbuf_empty(child)


def test_seg_left_gt_len_addresses(child, iface, hw_dst, ll_dst, ll_src):
    # send routing header with no (0) addresses but segleft set to a value
    # larger than 0
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrRouting(type=3, segleft=18, addresses=[]),
             iface=iface, timeout=1, verbose=0)
    assert(p is not None)
    assert(ICMPv6ParamProblem in p)
    assert(p[ICMPv6ParamProblem].code == 0)     # erroneous header field encountered
    assert(p[ICMPv6ParamProblem].ptr == 43)     # segleft field
    pktbuf_empty(child)


def test_multicast_dst(child, iface, hw_dst, ll_dst, ll_src):
    # sniffing for ICMPv6 parameter problem message
    sniffer.start_sniff(lambda p: p.haslayer(ICMPv6ParamProblem) or
                        (p.haslayer(UDP) and (p[IPv6].dst != "ff02::1")))
    # send routing header with multicast destination
    sendp(Ether(dst=hw_dst) / IPv6(dst="ff02::1", src=ll_src) /
          IPv6ExtHdrRouting(type=3, segleft=1, addresses=["abcd::1"]) /
          UDP(dport=2606), iface=iface, verbose=0)
    ps = sniffer.wait_for_sniff_results()
    p = [p for p in ps if (ICMPv6ParamProblem in p) or
                          ((UDP in p) and (p[UDP].dport == 2606) and
                           (p[IPv6].dst != "ff02::1"))]
    # packet should be discarded silently:
    # see https://tools.ietf.org/html/rfc6554#section-4.2
    assert(len(p) == 0)
    pktbuf_empty(child)


def test_multicast_addr(child, iface, hw_dst, ll_dst, ll_src):
    # sniffing for ICMPv6 parameter problem message
    sniffer.start_sniff(lambda p: p.haslayer(ICMPv6ParamProblem) or
                        (p.haslayer(UDP) and (p[IPv6].dst != ll_dst)))
    # Send routing header with multicast address in its destinations
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrRouting(type=3, segleft=1, addresses=["abcd::1"]) /
          UDP(dport=2606), iface=iface, verbose=0)
    ps = sniffer.wait_for_sniff_results()
    p = [p for p in ps if (ICMPv6ParamProblem in p) or
                          ((UDP in p) and (p[UDP].dport == 2606) and
                           (p[IPv6].dst != ll_dst))]
    # packet should be discarded silently:
    # see https://tools.ietf.org/html/rfc6554#section-4.2
    assert(len(p) == 0)
    pktbuf_empty(child)


def test_multiple_addrs_of_mine_uncomp(child, iface, hw_dst, ll_dst, ll_src):
    dummy = "affe::1"
    # add dummy IPv6 address
    dst_iface = get_first_interface(child)
    add_ipv6_address(child, dst_iface, dummy)
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrRouting(type=3, segleft=3, addresses=[ll_dst, ll_src,
                                                             dummy]),
             iface=iface, timeout=1, verbose=0)
    assert(p is not None)
    assert(ICMPv6ParamProblem in p)
    assert(p[ICMPv6ParamProblem].code == 0)             # erroneous header field encountered
    assert(p[ICMPv6ParamProblem].ptr == 40+8+(2 * 16))  # dummy in routing header
    pktbuf_empty(child)
    del_ipv6_address(child, dst_iface, dummy)


def test_forward_uncomp(child, iface, hw_dst, ll_dst, ll_src):
    dummy = "affe::1"
    hl = random.randint(2, 255)
    # sniffing for packets to dummy
    sniffer.start_sniff(lambda p: p[Ether].src == hw_dst)
    # add dummy IPv6 address
    dst_iface = get_first_interface(child)
    hw_src = get_host_hwaddr(iface)
    add_neighbor(child, dst_iface, dummy, hw_src)
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src, hlim=hl) /
          IPv6ExtHdrRouting(type=3, segleft=1, addresses=[dummy]),
          iface=iface, verbose=0)
    ps = sniffer.wait_for_sniff_results()
    p = [p for p in ps if p[Ether].src == hw_dst]
    assert(len(p) > 0)
    p = p[0]
    assert(IPv6 in p)
    assert(IPv6ExtHdrRouting in p)
    assert(p[IPv6].src == ll_src)
    assert(p[IPv6].dst == dummy)
    assert(p[IPv6].hlim == (hl - 1))
    assert(p[IPv6ExtHdrRouting].type == 3)
    assert(p[IPv6ExtHdrRouting].segleft == 0)
    pktbuf_empty(child)
    del_neighbor(child, dst_iface, dummy)


def test_forward_uncomp_not_first_ext_hdr(child, iface, hw_dst, ll_dst, ll_src):
    dummy = "affe::1"
    hl = random.randint(2, 255)
    # sniffing for packets to dummy
    sniffer.start_sniff(lambda p: p[Ether].src == hw_dst)
    # add dummy IPv6 address
    dst_iface = get_first_interface(child)
    hw_src = get_host_hwaddr(iface)
    add_neighbor(child, dst_iface, dummy, hw_src)
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src, hlim=hl) /
          IPv6ExtHdrHopByHop() /
          IPv6ExtHdrRouting(type=3, segleft=1, addresses=[dummy]),
          iface=iface, verbose=0)
    ps = sniffer.wait_for_sniff_results()
    p = [p for p in ps if p[Ether].src == hw_dst]
    assert(len(p) > 0)
    p = p[0]
    assert(IPv6 in p)
    assert(IPv6ExtHdrRouting in p)
    assert(p[IPv6].src == ll_src)
    assert(p[IPv6].dst == dummy)
    assert(p[IPv6].hlim == (hl - 1))
    assert(p[IPv6ExtHdrRouting].type == 3)
    assert(p[IPv6ExtHdrRouting].segleft == 0)
    pktbuf_empty(child)
    del_neighbor(child, dst_iface, dummy)


def test_seq_left_0(child, iface, hw_dst, ll_dst, ll_src):
    register_protnum(child)
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrRouting(type=3, segleft=0), iface=iface, verbose=0)
    # we are the target, so the packet should be dumped
    # empty snip
    child.expect(r"~~ SNIP  0 - size:\s+0 byte, type: NETTYPE_UNDEF \(\d+\)")
    ipv6_payload_len = 0
    # parsed routing header
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = 59 (IPV6_NONXT), len = 0x00, routing type = 3, segments left = 0
    child.expect(r"00000000  3B  00  03  00  00  00  00  00")
    # IPv6 header
    child.expect(r"~~ SNIP  2 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrRouting]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_time_exc(child, iface, hw_dst, ll_dst, ll_src):
    dummy = "affe::1"
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, hlim=1, src=ll_src) /
             IPv6ExtHdrRouting(type=3, segleft=1, addresses=[dummy]),
             iface=iface, timeout=1, verbose=0)
    assert(p is not None)
    assert(ICMPv6TimeExceeded in p)
    assert(p[ICMPv6TimeExceeded].code == 0)
    pktbuf_empty(child)


def testfunc(child):
    global sniffer
    tap = get_bridge(os.environ["TAP"])
    child.sendline("unittests")
    # wait for and check result of unittests
    print("." * check_unittests(child), end="", flush=True)
    lladdr_src = get_host_lladdr(tap)
    child.sendline("ifconfig")
    child.expect(r"HWaddr: (?P<hwaddr>[A-Fa-f:0-9]+)\s")
    hwaddr_dst = child.match.group("hwaddr").lower()
    child.expect(r"(?P<lladdr>fe80::[A-Fa-f:0-9]+)\s")
    lladdr_dst = child.match.group("lladdr").lower()
    sniffer = Sniffer(tap)

    def run(func):
        if child.logfile == sys.stdout:
            func(child, tap, hwaddr_dst, lladdr_dst, lladdr_src)
        else:
            try:
                func(child, tap, hwaddr_dst, lladdr_dst, lladdr_src)
                print(".", end="", flush=True)
            except Exception as e:
                print("FAILED")
                raise e

    run(test_wrong_type)
    run(test_seg_left_gt_len_addresses)
    run(test_multicast_dst)
    run(test_multicast_addr)
    run(test_multiple_addrs_of_mine_uncomp)
    run(test_forward_uncomp)
    run(test_forward_uncomp_not_first_ext_hdr)
    # compressed tests hard to implement with scapy and also covered in
    # unittests
    run(test_seq_left_0)
    run(test_time_exc)
    print("SUCCESS")


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=1, echo=False))
