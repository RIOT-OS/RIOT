#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # 1st 6LoWPAN fragment
    child.expect_exact("PKTDUMP: data received:")
    child.expect_exact("~~ SNIP  0 - size:  74 byte, type: NETTYPE_SIXLOWPAN (1)")
    child.expect_exact("Fragmentation Header (first)")
    child.expect_exact("datagram size: 148")
    child.expect_exact("tag: 0x0001")
    child.expect_exact("IPHC dispatch")
    child.expect_exact("TF: traffic class and flow label elided")
    child.expect_exact("NH: LOWPAN_NHC")
    child.expect_exact("HLIM: 255")
    child.expect_exact("Stateless source address compression: elided (use L2 address)")
    child.expect_exact("Stateless destinaton address compression: 128 bits inline")

    # 2nd 6LoWPAN fragment
    child.expect_exact("PKTDUMP: data received:")
    child.expect_exact("~~ SNIP  0 - size:  57 byte, type: NETTYPE_SIXLOWPAN (1)")
    child.expect_exact("Fragmentation Header (subsequent)")
    child.expect_exact("datagram size: 148")
    child.expect_exact("tag: 0x0001")
    child.expect_exact("offset: 12")

    # IPv6
    child.expect_exact("PKTDUMP: data received:")
    child.expect_exact("~~ SNIP  0 - size: 148 byte, type: NETTYPE_IPV6 (2)")
    child.expect_exact("traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)")
    child.expect_exact("flow label: 0x00000")
    child.expect_exact("length: 108  next header: 17  hop limit: 255")
    child.expect_exact("source address: fe80::ff:fe00:2")
    child.expect_exact("destination address: fd01::1")

    # UDP
    child.expect_exact("PKTDUMP: data received:")
    child.expect_exact("~~ SNIP  0 - size: 108 byte, type: NETTYPE_UDP (4)")
    child.expect_exact("   src-port: 61616  dst-port: 61616")
    child.expect_exact("   length: 108  cksum: 0x232f")
    child.expect_exact("~~ SNIP  1 - size:  40 byte, type: NETTYPE_IPV6 (2)")
    child.expect_exact("traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)")
    child.expect_exact("flow label: 0x00000")
    child.expect_exact("length: 108  next header: 17  hop limit: 255")
    child.expect_exact("source address: fe80::ff:fe00:2")
    child.expect_exact("destination address: fd01::1")

    # UDP (port 61616)
    child.expect_exact("PKTDUMP: data received:")
    child.expect_exact("~~ SNIP  0 - size: 100 byte, type: NETTYPE_UNDEF (0)")
    child.expect_exact("00000000  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00")
    child.expect_exact("00000010  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00")
    child.expect_exact("00000020  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00")
    child.expect_exact("00000030  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00")
    child.expect_exact("00000040  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00")
    child.expect_exact("00000050  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00  00")
    child.expect_exact("00000060  00  00  00  00")
    child.expect_exact("~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (4)")
    child.expect_exact("   src-port: 61616  dst-port: 61616")
    child.expect_exact("   length: 108  cksum: 0x232f")
    child.expect_exact("~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)")
    child.expect_exact("traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)")
    child.expect_exact("flow label: 0x00000")
    child.expect_exact("length: 108  next header: 17  hop limit: 255")
    child.expect_exact("source address: fe80::ff:fe00:2")
    child.expect_exact("destination address: fd01::1")


if __name__ == "__main__":
    sys.exit(run(testfunc))
