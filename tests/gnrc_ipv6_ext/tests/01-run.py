#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re
import os
import sys
import subprocess

from scapy.all import Ether, IPv6, UDP, \
                      IPv6ExtHdrHopByHop, IPv6ExtHdrDestOpt, \
                      IPv6ExtHdrFragment, IPv6ExtHdrRouting, \
                      ICMPv6ParamProblem, \
                      sendp, srp1
from testrunner import run


EXT_HDR_NH = {
    IPv6ExtHdrHopByHop: 0,
    IPv6ExtHdrRouting: 43,
    IPv6ExtHdrFragment: 44,
    # IPSec headers currently unsupported by scapy
    IPv6ExtHdrDestOpt: 60,
    # Mobility header currently unsupported by scapy
  }


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


def test_empty_duplicate_hop_by_hop_opt(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending two empty hop-by-hop-option header
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop() / IPv6ExtHdrHopByHop() / UDP() / "\x03\x04",
             iface=iface, timeout=1, verbose=0)
    # should return parameter problem message
    assert(p is not None)
    assert(ICMPv6ParamProblem in p)
    assert(p[ICMPv6ParamProblem].code == 1)     # unrecognized next header
    assert(p[ICMPv6ParamProblem].ptr >= 40)     # after IPv6 header
    pktbuf_empty(child)


def test_empty_non_first_hop_by_hop_opt(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending empty hop-by-hop-option header after destination option
    # header
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrDestOpt() / IPv6ExtHdrHopByHop() / UDP() / "\x05\x06",
             iface=iface, timeout=1, verbose=0)
    # should return parameter problem message
    assert(p is not None)
    assert(ICMPv6ParamProblem in p)
    assert(p[ICMPv6ParamProblem].code == 1)     # unrecognized next header
    assert(p[ICMPv6ParamProblem].ptr >= 40)     # after IPv6 header
    pktbuf_empty(child)


def test_empty_duplicate_non_first_hop_by_hop_opt(child, iface, hw_dst, ll_dst,
                                                  ll_src):
    # Try sending empty hop-by-hop-option header after destination option
    # header and another hop-by-hop-option header
    p = srp1(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
             IPv6ExtHdrHopByHop() / IPv6ExtHdrDestOpt() / IPv6ExtHdrHopByHop() /
             UDP() / "\x07\x08",
             iface=iface, timeout=1, verbose=0)
    # should return parameter problem message
    assert(p is not None)
    assert(ICMPv6ParamProblem in p)
    assert(p[ICMPv6ParamProblem].code == 1)     # unrecognized next header
    assert(p[ICMPv6ParamProblem].ptr >= 48)     # after IPv6 header and HopByHopOpt
    pktbuf_empty(child)


def test_empty_routing_header_wo_register(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending an empty routing header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrRouting() / UDP(), iface=iface, verbose=0)
    pktbuf_empty(child)


def test_empty_routing_header_w_register(child, iface, hw_dst, ll_dst, ll_src):
    # Register to routing header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrRouting])
    # Try sending an empty routing header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrRouting() / UDP() / "\x01\x02", iface=iface, verbose=0)
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = 17 (UDP), len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  11  00  00  00  00  00  00  00")
    child.expect(r"~~ SNIP  1 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrRouting]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_fragment_header_wo_register(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending an empty fragment header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment() / UDP(), iface=iface, verbose=0)
    pktbuf_empty(child)


def test_empty_fragment_header_w_register(child, iface, hw_dst, ll_dst, ll_src):
    # Register to fragment header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrFragment])
    # Try sending an empty fragment header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment() / UDP() / "\x01\x02", iface=iface, verbose=0)
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = 17 (UDP), reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000000  11  00  00  00  00  00  00  00")
    child.expect(r"~~ SNIP  1 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrFragment]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_dest_opt_wo_register(child, iface, hw_dst, ll_dst, ll_src):
    # Try sending an empty Destination-Option header
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrDestOpt() / UDP(), iface=iface, verbose=0)
    pktbuf_empty(child)


def test_empty_dest_opt_w_register(child, iface, hw_dst, ll_dst, ll_src):
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


def test_empty_mixed1_w_hop_opt_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrHopByHop])
    # Try sending a packet with a number of extension headers in recommended
    # order: https://tools.ietf.org/html/rfc8200#section-4.1
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrDestOpt() / IPv6ExtHdrRouting() /
          IPv6ExtHdrFragment() / IPv6ExtHdrDestOpt() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Hop-by-hop option with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, PadN option (0x01) of length 0x04
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00  "
                 r"{:02X}  00  01  04  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrDestOpt],
                        EXT_HDR_NH[IPv6ExtHdrRouting]
                    ))
    # NH = IPv6ExtHdrFragment, len = 0x00, routing type = 0, segments left = 0
    # NH = IPv6ExtHdrDestOpt, reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000010  {:02X}  00  00  00  00  00  00  00  "
                 r"{:02X}  00  00  00  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrFragment],
                        EXT_HDR_NH[IPv6ExtHdrDestOpt]
                    ))
    # NH = 17 (UDP), len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000020  11  00  01  04  00  00  00  00")
    # IPv6 header
    child.expect(r"~~ SNIP  1 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed1_w_rt_hdr_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to routing header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrRouting])
    # Try sending a packet with a number of extension headers in recommended
    # order: https://tools.ietf.org/html/rfc8200#section-4.1
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrDestOpt() / IPv6ExtHdrRouting() /
          IPv6ExtHdrFragment() / IPv6ExtHdrDestOpt() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Routing header with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrFragment, len = 0x00, routing type = 0, segments left = 0
    # NH = IPv6ExtHdrDestOpt, reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00  "
                 r"{:02X}  00  00  00  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrFragment],
                        EXT_HDR_NH[IPv6ExtHdrDestOpt]
                    ))
    # NH = 17 (UDP), len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000010  11  00  01  04  00  00  00  00")
    # Destination option 1
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrRouting]))
    # Hop-by-hop option
    child.expect(r"~~ SNIP  2 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # IPv6 header
    child.expect(r"~~ SNIP  3 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed1_w_frag_hdr_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to fragment header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrFragment])
    # Try sending a packet with a number of extension headers in recommended
    # order: https://tools.ietf.org/html/rfc8200#section-4.1
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrDestOpt() / IPv6ExtHdrRouting() /
          IPv6ExtHdrFragment() / IPv6ExtHdrDestOpt() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Routing header with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00  "
                 "11  00  01  04  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrFragment, len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrFragment]))
    # Destination option 1
    child.expect(r"~~ SNIP  2 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrRouting]))
    # Hop-by-hop option
    child.expect(r"~~ SNIP  3 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # IPv6 header
    child.expect(r"~~ SNIP  4 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed1_w_dest_opt_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to destination-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrDestOpt])
    # Try sending a packet with a number of extension headers in recommended
    # order: https://tools.ietf.org/html/rfc8200#section-4.1
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrDestOpt() / IPv6ExtHdrRouting() /
          IPv6ExtHdrFragment() / IPv6ExtHdrDestOpt() / UDP() / "\x01\x02",
          iface=iface, verbose=0)

    # IPv6ExtHdrDestOpt is two times in the packet so pktdump will it print two
    # times
    # 1st print parsed up to the first IPv6ExtHdrDestOpt
    # Destination option 1 with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    # NH = IPv6ExtHdrFragment, len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00  "
                 r"{:02X}  00  00  00  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrRouting],
                        EXT_HDR_NH[IPv6ExtHdrFragment]
                    ))
    # NH = IPv6ExtHdrDestOpt, reserved = 0x00, fragment offset = 0, res = 0, M = 0
    # NH = 17 (UDP), len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000010  {:02X}  00  00  00  00  00  00  00  "
                 r"11  00  01  04  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrDestOpt]
                    ))
    # Hop-by-hop option
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # IPv6 header
    child.expect(r"~~ SNIP  2 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))

    # 2nd print parsed up to the second IPv6ExtHdrHopByHop
    # Destination option 2 with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = 17 (UDP), len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  11  00  01  04  00  00  00  00")
    # Fragment header
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # Routing header
    child.expect(r"~~ SNIP  2 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrFragment, len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrFragment]))
    # Destination option 1
    child.expect(r"~~ SNIP  3 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrRouting]))
    # Hop-by-hop option
    child.expect(r"~~ SNIP  4 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # IPv6 header
    child.expect(r"~~ SNIP  5 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed2_w_hop_opt_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to hop-by-hop-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrHopByHop])
    # Try sending a packet with a number of extension headers in not recommended
    # (but legal) order
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrRouting() / IPv6ExtHdrDestOpt() /
          IPv6ExtHdrFragment() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Hop-by-hop option with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    # NH = IPv6ExtHdrDestOpt, len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00  "
                 r"{:02X}  00  00  00  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrRouting],
                        EXT_HDR_NH[IPv6ExtHdrDestOpt]
                    ))
    # NH = IPv6ExtHdrFragment, len = 0x00, PadN option (0x01) of length 0x04
    # NH = 17 (UDP), reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000010  {:02X}  00  01  04  00  00  00  00  "
                 r"11  00  00  00  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrFragment]
                    ))
    # IPv6 header
    child.expect(r"~~ SNIP  1 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed2_w_rt_hdr_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to routing header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrRouting])
    # Try sending a packet with a number of extension headers in not recommended
    # (but legal) order
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrRouting() / IPv6ExtHdrDestOpt() /
          IPv6ExtHdrFragment() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Routing header with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, routing type = 0, segments left = 0
    # NH = IPv6ExtHdrFragment, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00  "
                 r"{:02X}  00  01  04  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrDestOpt],
                        EXT_HDR_NH[IPv6ExtHdrFragment]
                    ))
    # NH = 17 (UDP), reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000010  11  00  00  00  00  00  00  00")
    # Hop-by-hop-option
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrRouting]))
    # IPv6 header
    child.expect(r"~~ SNIP  2 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed2_w_frag_hdr_registered(child, iface, hw_dst, ll_dst,
                                            ll_src):
    # Register to fragment header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrFragment])
    # Try sending a packet with a number of extension headers in not recommended
    # (but legal) order
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrRouting() / IPv6ExtHdrDestOpt() /
          IPv6ExtHdrFragment() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Fragment header with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = 17 (UDP), reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000000  11  00  00  00  00  00  00  00")
    # Destination option
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrFragment, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrFragment]))
    # Routing header
    child.expect(r"~~ SNIP  2 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrDestOpt, len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # Hop-by-hop-option
    child.expect(r"~~ SNIP  3 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrRouting]))
    # IPv6 header
    child.expect(r"~~ SNIP  4 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
    pktbuf_empty(child)
    unregister(child)


def test_empty_mixed2_w_dest_opt_registered(child, iface, hw_dst, ll_dst, ll_src):
    # Register to destination-option header
    register_protnum(child, EXT_HDR_NH[IPv6ExtHdrDestOpt])
    # Try sending a packet with a number of extension headers in not recommended
    # (but legal) order
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrHopByHop() / IPv6ExtHdrRouting() / IPv6ExtHdrDestOpt() /
          IPv6ExtHdrFragment() / UDP() / "\x01\x02",
          iface=iface, verbose=0)
    # Destination option with payload
    child.expect(r"~~ SNIP  0 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len = int(child.match.group(1))
    # NH = IPv6ExtHdrFragment, len = 0x00, PadN option (0x01) of length 0x04
    # NH = 17 (UDP), reserved = 0x00, fragment offset = 0, res = 0, M = 0
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00  "
                 r"11  00  00  00  00  00  00  00".format(
                        EXT_HDR_NH[IPv6ExtHdrFragment]))
    # Routing header
    child.expect(r"~~ SNIP  1 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))

    # NH = IPv6ExtHdrDestOpt, len = 0x00, routing type = 0, segments left = 0
    child.expect(r"00000000  {:02X}  00  00  00  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrDestOpt]))
    # Hop-by-hop-option
    child.expect(r"~~ SNIP  2 - size:\s+(\d+) byte, type: NETTYPE_\w+ \(\d+\)")
    ipv6_payload_len += int(child.match.group(1))
    # NH = IPv6ExtHdrRouting, len = 0x00, PadN option (0x01) of length 0x04
    child.expect(r"00000000  {:02X}  00  01  04  00  00  00  00".format(
                 EXT_HDR_NH[IPv6ExtHdrRouting]))
    # IPv6 header
    child.expect(r"~~ SNIP  3 - size:\s+40 byte, type: NETTYPE_IPV6 \(\d+\)")
    child.expect_exact(r"length: {}  next header: {}".format(
            ipv6_payload_len, EXT_HDR_NH[IPv6ExtHdrHopByHop]
        ))
    child.expect_exact(r"destination address: {}".format(ll_dst))
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
    run(test_empty_routing_header_wo_register)
    run(test_empty_routing_header_w_register)
    run(test_empty_fragment_header_wo_register)
    run(test_empty_fragment_header_w_register)
    run(test_empty_dest_opt_wo_register)
    run(test_empty_dest_opt_w_register)
    # check various registrations with recommended order to validate parsing
    # (recommended order, see https://tools.ietf.org/html/rfc8200#section-4.1)
    run(test_empty_mixed1_w_hop_opt_registered)
    run(test_empty_mixed1_w_rt_hdr_registered)
    run(test_empty_mixed1_w_frag_hdr_registered)
    run(test_empty_mixed1_w_dest_opt_registered)
    # other orders SHOULD also be parsed (and since checking the order is more
    # complicated we are able to do that)
    run(test_empty_mixed2_w_hop_opt_registered)
    run(test_empty_mixed2_w_rt_hdr_registered)
    run(test_empty_mixed2_w_frag_hdr_registered)
    run(test_empty_mixed2_w_dest_opt_registered)
    print("SUCCESS")


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=1, echo=False))
