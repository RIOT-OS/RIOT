#!/usr/bin/env python3

# Copyright (C) 2018-2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re
import os
import sys
import subprocess
import threading

from pexpect import TIMEOUT
from scapy.all import Ether, IPv6, UDP, ICMPv6ParamProblem, \
                      IPv6ExtHdrHopByHop, IPv6ExtHdrDestOpt, \
                      Pad1, PadN, HBHOptUnknown, \
                      sendp, srp1, AsyncSniffer
from testrunner import run


EXT_HDR_NH = {
    IPv6ExtHdrHopByHop: 0,
    UDP: 17,
    IPv6ExtHdrDestOpt: 60,
}
TEST_OPTION_TYPES = {
    # See experimental options
    # https://www.iana.org/assignments/ipv6-parameters/ipv6-parameters.xhtml#ipv6-parameters-2
    "ACTION_SKIP": 0x1E,
    "ACTION_DISCARD": 0x5E,
    "ACTION_DISCARD_ERROR_MCAST": 0x9E,
    "ACTION_DISCARD_ERROR": 0xDE,
}
HW_MCAST = "33:33:00:00:00:01"
MCAST = "ff02::1"
RECV_TIMEOUT = 0.2


class StartCheckAsyncSniffer(AsyncSniffer):
    def _cb(self):
        self._started.set()

    def __init__(self, *args, **kwargs):
        kwargs["started_callback"] = self._cb
        self._started = threading.Event()
        self._started.clear()
        super().__init__(*args, **kwargs)

    def wait_for_started(self, timeout=None):
        self._started.wait(timeout)


def pktbuf_empty(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    first_byte = child.match.group("first_byte")
    size = child.match.group("size")
    child.expect(
            r"~ unused: {} \(next: (\(nil\)|0), size: {}\) ~".format(
                first_byte, size))


def register_protnum(child, protnum):
    child.sendline("ip reg %d" % protnum)
    child.expect("Registered to protocol number %d" % protnum)


def unregister(child):
    child.sendline("ip unreg")
    child.expect(r"Unregistered from protocol number \d")


def test_empty_hop_by_hop_opt_wo_register(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending an empty hop-by-hop-option header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / UDP(), iface=iface, verbose=0)
    pktbuf_empty(child)


def test_empty_hop_by_hop_opt_w_register(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrHopByHop])
    # Try sending an empty hop-by-hop-option header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / UDP() / "\x01\x02", iface=iface, verbose=0)
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = 17 (UDP), len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  11  00  01  04  00  00  00  00")
    child.expect(r"~~ SNIP  1 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_hop_by_hop_opt_large_hdr_len(child, iface, hw_dst, ll_dst,
                                            ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrHopByHop])
    # Try sending an empty hop-by-hop-option header with too big header length
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop(len=20) / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    pktbuf_empty(child)
    unregister(child)


def test_empty_duplicate_hop_by_hop_opt(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending two empty hop-by-hop-option header
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop() / IPv6ExtHdrHopByHop() / UDP() / "\x03\x04",
             iface=iface, timeout=RECV_TIMEOUT, verbose=0)
    # should return parameter problem message
    assert p is not None
    assert ICMPv6ParamProblem in p
    assert p[ICMPv6ParamProblem].code == 1      # unrecognized next header
    assert p[ICMPv6ParamProblem].ptr >= 40      # after IPv6 header
    pktbuf_empty(child)


def test_empty_non_first_hop_by_hop_opt(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending empty hop-by-hop-option header after destination option
    # header
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrDestOpt() / IPv6ExtHdrHopByHop() / UDP() / "\x05\x06",
             iface=iface, timeout=RECV_TIMEOUT, verbose=0)
    # should return parameter problem message
    assert p is not None
    assert ICMPv6ParamProblem in p
    assert p[ICMPv6ParamProblem].code == 1      # unrecognized next header
    assert p[ICMPv6ParamProblem].ptr >= 40      # after IPv6 header
    pktbuf_empty(child)


def test_empty_duplicate_non_first_hop_by_hop_opt(child, iface, hw_dst, ll_dst,
                                                  ll_src):
    # Try sending empty hop-by-hop-option header after destination option
    # header and another hop-by-hop-option header
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop() / IPv6ExtHdrDestOpt() / IPv6ExtHdrHopByHop() /
             UDP() / "\x07\x08",
             iface=iface, timeout=RECV_TIMEOUT, verbose=0)
    # should return parameter problem message
    assert p is not None
    assert ICMPv6ParamProblem in p
    assert p[ICMPv6ParamProblem].code == 1      # unrecognized next header
    assert p[ICMPv6ParamProblem].ptr >= 48      # after IPv6 header and HopByHopOpt
    pktbuf_empty(child)


def test_hop_by_hop_opt_only_one_pad1(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    # send malformed packet with only one Pad1 option
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          # autopad=0 already introduces one Pad1 option and doesn't work with
          # options parameter
          IPv6ExtHdrHopByHop(autopad=0) / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    res = child.expect(
        # 10 bytes == UDP header plus 2 byte payload
        [r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    # We expect the header parsing to be messed up
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_7_pad1(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          # autopad=0 already introduces one Pad1 option and doesn't work with
          # options parameter
          IPv6ExtHdrHopByHop(nh=EXT_HDR_NH[UDP], autopad=0) /
          Pad1() / Pad1() / Pad1() / Pad1() / Pad1() / Pad1() /
          UDP() / "\x01\x02",
          iface=iface, verbose=0)
    child.expect(
        r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)"
    )
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_broken_padn(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          # autopad=0 doesn't work with options parameter
          IPv6ExtHdrHopByHop(nh=EXT_HDR_NH[UDP], autopad=0) /
          PadN(optlen=7, optdata="\x11\x22\x33\x44\x55\x66\x77") /
          UDP() / "\x01\x02",
          iface=iface, verbose=0)
    res = child.expect(
        # 10 bytes == UDP header plus 2 byte payload
        [r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    # We expect the header parsing to be messed up
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_skip_unknown(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop(
                 options=HBHOptUnknown(
                     otype=TEST_OPTION_TYPES["ACTION_SKIP"], optlen=4,
                     optdata="\x11\x22\x33\x44"
                 )
             ) /
             UDP() / "\x01\x02",
             iface=iface, verbose=0, timeout=RECV_TIMEOUT)
    assert p is None
    child.expect(
        r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)"
    )
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_1(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop(
                 options=HBHOptUnknown(
                     otype=TEST_OPTION_TYPES["ACTION_DISCARD"], optlen=4,
                     optdata="\x11\x22\x33\x44"
                 )
             ) /
             UDP() / "\x01\x02",
             iface=iface, verbose=0, timeout=RECV_TIMEOUT)
    assert p is None
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_2(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop(
                 options=[
                     HBHOptUnknown(
                         otype=TEST_OPTION_TYPES["ACTION_SKIP"], optlen=6,
                         optdata="\x11\x22\x33\x44\x55\x66"
                     ),
                     HBHOptUnknown(
                         otype=TEST_OPTION_TYPES["ACTION_DISCARD"], optlen=4,
                         optdata="\x11\x22\x33\x44"
                     )
                 ]
             ) /
             UDP() / "\x01\x02",
             iface=iface, verbose=0, timeout=RECV_TIMEOUT)
    assert p is None
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_3(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop(
                 options=HBHOptUnknown(
                     otype=TEST_OPTION_TYPES["ACTION_DISCARD_ERROR_MCAST"],
                     optlen=4, optdata="\x11\x22\x33\x44"
                 )
             ) /
             UDP() / "\x01\x02",
             iface=iface, verbose=0, timeout=RECV_TIMEOUT)
    assert p is not None
    assert ICMPv6ParamProblem in p
    # unrecognized IPv6 option encountered
    assert p[ICMPv6ParamProblem].code == 2
    # first after IPv6 header + extension header => 40 + 2 = 42
    assert p[ICMPv6ParamProblem].ptr == 42
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_3_mcast(child, iface, hw_dst, ll_dst,
                                                ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    # sniff for parameter problem as with multicast srp1 does not work
    sniffer = StartCheckAsyncSniffer(iface=iface, count=1,
                                     filter="icmp6[0] == 4")
    sniffer.start()
    sniffer.wait_for_started()
    sendp(Ether(dst=HW_MCAST) / IPv6(dst=MCAST, src=ll_src) /
          IPv6ExtHdrHopByHop(
              options=HBHOptUnknown(
                  otype=TEST_OPTION_TYPES["ACTION_DISCARD_ERROR_MCAST"],
                  optlen=4, optdata="\x11\x22\x33\x44"
              )
          ) /
          UDP() / "\x01\x02",
          iface=iface, verbose=0)
    sniffer.join(RECV_TIMEOUT)
    ps = sniffer.results
    assert len(ps) == 1
    p = ps[0]
    assert ICMPv6ParamProblem in p
    # unrecognized IPv6 option encountered
    assert p[ICMPv6ParamProblem].code == 2
    # first after IPv6 header + extension header => 40 + 2 = 42
    assert p[ICMPv6ParamProblem].ptr == 42
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_4(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop(
                 options=[
                     HBHOptUnknown(
                         otype=TEST_OPTION_TYPES["ACTION_SKIP"], optlen=6,
                         optdata="\x11\x22\x33\x44\x55\x66"
                     ),
                     HBHOptUnknown(
                         otype=TEST_OPTION_TYPES["ACTION_DISCARD_ERROR_MCAST"],
                         optlen=4, optdata="\x11\x22\x33\x44"
                     )
                 ]
             ) /
             UDP() / "\x01\x02",
             iface=iface, verbose=0, timeout=RECV_TIMEOUT)
    assert p is not None
    assert ICMPv6ParamProblem in p
    # unrecognized IPv6 option encountered
    assert p[ICMPv6ParamProblem].code == 2
    # first after IPv6 header + extension header + skipped option
    # => 40 + 2 + 2 + 6 = 50
    assert p[ICMPv6ParamProblem].ptr == 50
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_5(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop(
                 options=HBHOptUnknown(
                     otype=TEST_OPTION_TYPES["ACTION_DISCARD_ERROR"],
                     optlen=4, optdata="\x11\x22\x33\x44"
                 )
             ) /
             UDP() / "\x01\x02",
             iface=iface, verbose=0, timeout=RECV_TIMEOUT)
    assert p is not None
    assert ICMPv6ParamProblem in p
    # unrecognized IPv6 option encountered
    assert p[ICMPv6ParamProblem].code == 2
    # first after IPv6 header + extension header => 40 + 2 = 42
    assert p[ICMPv6ParamProblem].ptr == 42
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_hop_by_hop_opt_discard_unknown_5_mcast(child, iface, hw_dst, ll_dst,
                                                ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    # sniff for parameter problem as with multicast srp1 does not work
    sniffer = StartCheckAsyncSniffer(iface=iface, count=1,
                                     filter="icmp6[0] == 4")
    sniffer.start()
    sniffer.wait_for_started()
    sendp(Ether(dst=HW_MCAST) / IPv6(dst=MCAST, src=ll_src) /
          IPv6ExtHdrHopByHop(
              options=HBHOptUnknown(
                  otype=TEST_OPTION_TYPES["ACTION_DISCARD_ERROR"],
                  optlen=4, optdata="\x11\x22\x33\x44"
              )
          ) /
          UDP() / "\x01\x02",
          iface=iface, verbose=0)
    sniffer.join(RECV_TIMEOUT)
    ps = sniffer.results
    assert ps is None
    res = child.expect(
        # the packet should be not received at all
        [r"PKTDUMP: data received:", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_empty_dst_opt_wo_register(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending an empty Destination-Option header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt() / UDP(), iface=iface, verbose=0)
    pktbuf_empty(child)


def test_empty_dst_opt_w_register(child, iface, hw_dst, ll_dst, ll_src):
    # Register to Destination-Option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrDestOpt])
    # Try sending an empty Destination-Option header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt() / UDP() / "\x01\x02", iface=iface, verbose=0)
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = 17 (UDP), len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  11  00  01  04  00  00  00  00")
    child.expect(r"~~ SNIP  1 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrDestOpt]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_dst_opt_large_hdr_len(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrDestOpt])
    # Try sending an empty hop-by-hop-option header with too big header length
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt(len=20) / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    pktbuf_empty(child)
    unregister(child)


def test_dst_opt_only_one_pad1(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    # send malformed packet with only one Pad1 option
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt(autopad=0) / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    res = child.expect(
        # 10 bytes == UDP header plus 2 byte payload
        [r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    # We expect the header parsing to be messed up
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


def test_dst_opt_7_pad1(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt(nh=EXT_HDR_NH[UDP], autopad=0) /
          Pad1() / Pad1() / Pad1() / Pad1() / Pad1() / Pad1() /
          UDP() / "\x01\x02",
          iface=iface, verbose=0)
    child.expect(
        r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)",
        timeout=RECV_TIMEOUT
    )
    pktbuf_empty(child)
    unregister(child)


def test_dst_opt_broken_padn(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[UDP])
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt(nh=EXT_HDR_NH[UDP], autopad=0) /
          PadN(optlen=7, optdata="\x11\x22\x33\x44\x55\x66\x77") /
          UDP() / "\x01\x02",
          iface=iface, verbose=0)
    res = child.expect(
        # 10 bytes == UDP header plus 2 byte payload
        [r"~~ SNIP  0 - size:  10 byte, type: NETTYPE_UNDEF \(\d+\)", TIMEOUT],
        timeout=RECV_TIMEOUT
    )
    # We expect the header parsing to be messed up
    assert res > 0
    pktbuf_empty(child)
    unregister(child)


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
            r"inet6 (?P<lladdr>[0-9A-Fa-f:]+)/64",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])

    lladdr_src = get_host_lladdr(tap)
    child.sendline("ifconfig")
    child.expect(r"HWaddr: (?P<hwaddr>[A-Fa-f:0-9]+)\s")
    hwaddr_dst = child.match.group("hwaddr").lower()
    child.expect(r"(?P<lladdr>fe80::[A-Fa-f:0-9]+)\s")
    lladdr_dst = child.match.group("lladdr").lower()

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

    run(test_empty_hop_by_hop_opt_wo_register)
    run(test_empty_hop_by_hop_opt_w_register)
    run(test_empty_duplicate_hop_by_hop_opt)
    run(test_empty_non_first_hop_by_hop_opt)
    run(test_empty_duplicate_non_first_hop_by_hop_opt)
    run(test_hop_by_hop_opt_only_one_pad1)
    run(test_hop_by_hop_opt_7_pad1)
    run(test_hop_by_hop_opt_broken_padn)
    run(test_hop_by_hop_opt_skip_unknown)
    run(test_hop_by_hop_opt_discard_unknown_1)
    run(test_hop_by_hop_opt_discard_unknown_2)
    run(test_hop_by_hop_opt_discard_unknown_3)
    run(test_hop_by_hop_opt_discard_unknown_3_mcast)
    run(test_hop_by_hop_opt_discard_unknown_4)
    run(test_hop_by_hop_opt_discard_unknown_5)
    run(test_hop_by_hop_opt_discard_unknown_5_mcast)
    run(test_empty_dst_opt_wo_register)
    run(test_empty_dst_opt_w_register)
    run(test_empty_dst_opt_large_hdr_len)
    run(test_dst_opt_only_one_pad1)
    run(test_dst_opt_7_pad1)
    run(test_dst_opt_broken_padn)
    print("SUCCESS")


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=1, echo=False))
