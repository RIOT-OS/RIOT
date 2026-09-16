#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys

import pexpect

from testrunner import run


def inject(child, byts):
    if len(byts) < 16:
        child.sendline(f"inject {bytes(byts).hex()}")
    else:
        for i in range(0, len(byts), 16):
            child.sendline(f"preload {bytes(byts[i:i+16]).hex()}")
            for j in range(0, i + 16, 16):
                child.expect_exact(
                    f"{j:08X}" + "".join(
                        f"  {b:02X}" for b in byts[j:j+16]
                    )
                )
        child.sendline("inject")


def testfunc(child):
    # Old, static fragmentation tests
    # ===============================

    child.sendline("send_fragments")

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
    child.expect_exact("Stateless destination address compression: 128 bits inline")

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
    child.expect_exact("destination address: fd01::ff:fe00:1")

    # UDP
    child.expect_exact("PKTDUMP: data received:")
    child.expect_exact("~~ SNIP  0 - size: 108 byte, type: NETTYPE_UDP (5)")
    child.expect_exact("   src-port: 61616  dst-port: 61616")
    child.expect_exact("   length: 108  cksum: 0x242f")
    child.expect_exact("~~ SNIP  1 - size:  40 byte, type: NETTYPE_IPV6 (2)")
    child.expect_exact("traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)")
    child.expect_exact("flow label: 0x00000")
    child.expect_exact("length: 108  next header: 17  hop limit: 255")
    child.expect_exact("source address: fe80::ff:fe00:2")
    child.expect_exact("destination address: fd01::ff:fe00:1")

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
    child.expect_exact("~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)")
    child.expect_exact("   src-port: 61616  dst-port: 61616")
    child.expect_exact("   length: 108  cksum: 0x242f")
    child.expect_exact("~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)")
    child.expect_exact("traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)")
    child.expect_exact("flow label: 0x00000")
    child.expect_exact("length: 108  next header: 17  hop limit: 255")
    child.expect_exact("source address: fe80::ff:fe00:2")
    child.expect_exact("destination address: fd01::ff:fe00:1")
    child.expect_exact("IPv6 packets received in app: 1 (queue capacity: 4)")

    child.sendline("pktbuf")
    pktbuf_malloc = child.expect(
        [
            r"packet buffer: first byte: .*, last byte: .* \(size: (\d+)\)",
            "pktbuf: no stat output for gnrc_pktbuf_malloc",
        ]
    )
    pktbuf_malloc = bool(int(pktbuf_malloc))

    if not pktbuf_malloc:
        child.expect(
            rf"~ unused: .* \(next: .*, size: {child.match.group(1)}\) ~"
        )

    # Test IPHC
    # =========

    for byts, expect_lines, name in [
        # First two bytes (16 bits) are the IPHC dispatch
        #   0                                       1
        #   0   1   2   3   4   5   6   7   8   9   0   1   2   3   4   5
        # +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
        # | 0 | 1 | 1 |  TF   |NH | HLIM  |CID|SAC|  SAM  | M |DAC|  DAM  |
        # +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
        #
        # see also https://datatracker.ietf.org/doc/html/rfc6282#section-3.1.1
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC+NHC"
        ),
        (
            [   # IPHC dispatch, second byte missing
                0b01111111,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   1 byte, type: NETTYPE_SIXLOWPAN (1)",
                "Malformed IPHC dispatch (second byte missing)",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC, second byte missing"
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   TC: ECN + DSCP + FL in-line
                0b01100111, 0b00110011,
                (0x2 << 6) | 0x2d,          # ECN = 0x2, DSCP = 0x2d
                0x02, 0x34, 0x56,           # FL = 0x23456
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0xad (ECN: 0x2, DSCP: 0x2d)",
                "flow label: 0x23456",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "ECN + DSCP + FL in-line"
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   TC: ECN + DSCP + FL in-line, BYTE MISSING
                0b01100111, 0b00110011,
                (0x2 << 6) | 0x2d,          # ECN = 0x2, DSCP = 0x2d
                0x02, 0x34,                 # truncated FL
            ],
            [
                "PKTDUMP: data received:",
                "TF: ECN + DSCP + Flow Label (4 bytes)",
                "00000000  AD  02  34",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "ECN + DSCP + FL in-line, byte missing"
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   TC: ECN + FL in-line, DSCP elided
                0b01101111, 0b00110011,
                (0x2 << 6) | 0x07,          # ECN = 0x2, FL MSBs = 0x7
                0x89, 0xab,                 # FL LSBs = 0x89ab
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x80 (ECN: 0x2, DSCP: 0x00)",
                "flow label: 0x789ab",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "ECN+FL inline, DSCP elided",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   TC: ECN + FL in-line, DSCP elided, BYTE MISSING
                0b01101111, 0b00110011,
                (0x2 << 6) | 0x07,          # ECN = 0x2, FL MSBs = 0x7
                0x89,                       # truncated FL
            ],
            [
                "PKTDUMP: data received:",
                "TF: ECN + Flow Label (3 bytes)",
                "00000000  87  89",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "ECN+FL inline, DSCP elided, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   TC: ECN + DSCP (Traffic Class) in-line, flow label elided
                0b01110111, 0b00110011,
                (0x3 << 6) | 0x13,          # ECN = 0x3, DSCP = 0x13
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0xd3 (ECN: 0x3, DSCP: 0x13)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "ECN+DSCP inline, flow label elided",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   TC: ECN + DSCP (Traffic Class) in-line, flow label elided,
                #     BYTE MISSING
                0b01110111, 0b00110011,
                # In-line Traffic Class missing
            ],
            [
                "PKTDUMP: data received:",
                "TF: ECN + DSCP (1 bytes)",
                "00000000~~ SNIP  1",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "ECN+DSCP inline, flow label elided, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH=1, BYTE MISSING
                0b01111011, 0b00110011,
                59,  # NH = 59 (no next header)
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 0  next header: 59  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "NH=1",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH=1, BYTE MISSING
                0b01111011, 0b00110011,
                # no in-line NH
            ],
            [
                "PKTDUMP: data received:",
                "NH: inline",
                "00000000~~ SNIP  1",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "NH=1, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   HLIM=1
                0b01111100, 0b00110011,
                46,                     # HLIM=46
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 46",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "HLIM=1",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   HLIM=1, BYTE MISSING
                0b01111100, 0b00110011,
                # no in-line HLIM
            ],
            [
                "PKTDUMP: data received:",
                "HLIM: inline",
                "00000000~~ SNIP  1",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "HLIM=1, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   CID=1, with full stateless compression
                0b01111101, 0b10110011,
                0x45,                   # SCI = 0x4, DCI = 0x5
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "SCI: 0x4, DCI: 0x5",
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "CID=1, full stateless compression",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   CID=1, with full stateful compression, context not provided
                0b01111101, 0b11110111,
                0x45,                   # SCI = 0x4, DCI = 0x5
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "SCI: 0x4, DCI: 0x5",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "CID=1, full stateful compression, w/o context provided",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   CID=1, with full stateful compression
                0b01111101, 0b11110111,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x26, 0x76,
            ],
            [
                "PKTDUMP: data received:",
                "SCI: 0x1, DCI: 0x1",
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2676",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fd01::ff:fe00:2",
                "destination address: fd01::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "CID=1, full stateful compression",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   CID=1, but CID extension missing
                0b01111101, 0b10110011,
                # no CID extension
            ],
            [
                "PKTDUMP: data received:",
                "CID bit set but no CID extension provided (header malformed)",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "CID=1, with CID extension missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=0, SAM=00 (full address carried inline)
                0b01111101, 0b00000011,
                # in-line part of address
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x12,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x58, 0x61,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x5861",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: 2001:db8::abcd:ef12",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "SAC=0, SAM=00",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=0, SAM=00 (full address carried inline),
                #     BYTE MISSING
                0b01111101, 0b00000011,
                # truncated in-line part of address
                0x20, 0x01, 0x0d, 0xb8, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless source address compression: 128 bits inline",
                "00000000  20  01  0D  B8  00  00  00  00  "
                "00  00  00  00  AB  CD  EF",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "SAC=0, SAM=00, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=0, SAM=01 (64 bits carried inline)
                0b01111101, 0b00010011,
                # in-line part of address
                0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x12,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x87, 0x99,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x8799",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::abcd:ef12",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "SAC=0, SAM=01",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=0, SAM=01 (64 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b00010011,
                # truncated in-line part of address
                0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless source address compression: 64 bits inline",
                "00000000  00  00  00  00  AB  CD  EF",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "SAC=0, SAM=01, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=0, SAM=10 (16 bits carried inline)
                0b01111101, 0b00100011,
                # in-line part of address
                0x12, 0xef,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x10, 0x8b,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x108b",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:12ef",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "SAC=0, SAM=10",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=0, SAM=10 (16 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b00100011,
                # truncated in-line part of address
                0x12,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless source address compression: 16 bits inline",
                "00000000  12",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "SAC=0, SAM=10, byte missing",
        ),
        # SAC=0, SAM=11 already tested above (full source elision)
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=1, SAM=00 (unspecified address = ::)
                0b01111101, 0b01000011,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x20, 0xfb,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x20fb",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: ::",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "SAC=1, SAM=00",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=1, SAM=01 (context prefix, 64 bits carried inline)
                0b01111101, 0b11010011,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # in-line part of address
                0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x12,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x89, 0x18,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x8918",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fd01::abcd:ef12",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "SAC=1, SAM=01",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=1, SAM=01 (context prefix, 64 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b11010011,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # truncated in-line part of address
                0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful source address compression: 64 bits inline",
                "SCI: 0x1, DCI: 0x1",
                "00000000  00  00  00  00  AB  CD  EF",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "SAC=1, SAM=01, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=1, SAM=10 (context prefix, 16 bits carried inline)
                0b01111101, 0b11100011,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # in-line part of address
                0xcb, 0x68,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x59, 0x90,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x5990",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fd01::ff:fe00:cb68",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "SAC=1, SAM=10",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   SAC=1, SAM=10 (context prefix, 16 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b11100011,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # truncated in-line part of address
                0xcb,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful source address compression: 16 bits inline",
                "SCI: 0x1, DCI: 0x1",
                "00000000  CB",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "SAC=1, SAM=10, byte missing",
        ),
        # SAC=1, SAM=11 already tested above (full source elision)
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=0, DAM=00 (full address carried inline)
                0b01111101, 0b00110000,
                # in-line part of address
                0xfd, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fd01::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=0, DAC=0, DAM=00",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=0, DAM=00 (full address carried inline),
                #     BYTE MISSING
                0b01111101, 0b00110000,
                # truncated in-line part of address
                0xfd, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination address compression: 128 bits inline",
                "00000000  FD  01  00  00  00  00  00  "
                "00  00  00  00  FF  FE  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=0, DAC=0, DAM=00, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=0, DAM=01 (64 bits carried inline)
                0b01111101, 0b00110001,
                # in-line part of address
                0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=0, DAC=0, DAM=01",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=0, DAM=01 (64 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b00110001,
                # truncated in-line part of address
                0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination address compression: 64 bits inline",
                "00000000  00  00  00  FF  FE  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=0, DAC=0, DAM=01, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=0, DAM=10 (16 bits carried inline)
                0b01111101, 0b00110010,
                # in-line part of address
                0x00, 0x01,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x23, 0x78,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x2378",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=0, DAC=0, DAM=10",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=0, DAM=10 (16 bits carried inline), BYTE MISSING
                0b01111101, 0b00110010,
                # truncated in-line part of address
                0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination address compression: 16 bits inline",
                "00000000  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=0, DAC=0, DAM=10, byte missing",
        ),
        # M=0, DAC=0, DAM=11 already tested above (full destination elision)
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=1, DAM=00 (reserved => should fail)
                0b01111101, 0b00110100,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x20, 0xfb,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful destination address compression: reserved",
                "00000000  F3  00  20  FB",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=0, DAC=1, DAM=00",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=1, DAM=01 (context prefix, 64 bits carried inline)
                0b01111101, 0b10110101,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # in-line part of address
                0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x01,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fd01::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=0, DAC=1, DAM=01",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=1, DAM=01 (context prefix, 64 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b10110101,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # truncated in-line part of address
                0x00, 0x00, 0x00, 0xff, 0xfe, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful destination address compression: 64 bits inline",
                "SCI: 0x1, DCI: 0x1",
                "00000000  00  00  00  FF  FE  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=0, DAC=1, DAM=01, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=1, DAM=10 (context prefix, 16 bits carried inline)
                0b01111101, 0b10110110,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # in-line part of address
                0x00, 0x01,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fd01::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=0, DAC=1, DAM=10",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   M=0, DAC=1, DAM=10 (context prefix, 16 bits carried inline),
                #     BYTE MISSING
                0b01111101, 0b10110110,
                0x11,                   # SCI = 0x1, DCI = 0x1
                # truncated in-line part of address
                0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful destination address compression: 16 bits inline",
                "SCI: 0x1, DCI: 0x1",
                "00000000  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=0, DAC=1, DAM=10, byte missing",
        ),
        # M=0, DAC=1, DAM=11 already tested with vector above
        # (full source elision)
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=00 (full all-nodes address carried inline)
                0b01111011, 0b00111000,
                59,                     # Next Header: 59 (0x3B) (no next header)
                # in-line part of address
                0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 0  next header: 59  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: ff02::1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=00",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=00 (full all-nodes address carried inline),
                #     BYTE MISSING
                0b01111011, 0b00111000,
                59,                     # Next Header: 59 (0x3B) (no next header)
                # truncated in-line part of address
                0xff, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination multicast address compression:",
                "    128 bits carried inline",
                "00000000  3B  FF  02  00  00  00  00  00  "
                "00  00  00  00  00  00  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=00, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=01 (48 bits of all-nodes address carried inline)
                0b01111011, 0b00111001,
                59,                     # Next Header: 59 (0x3B) (no next header)
                # in-line part of address
                0x02, 0x00, 0x00, 0x00, 0x00, 0x01,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 0  next header: 59  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: ff02::1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=01",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=01 (48 bits of all-nodes address carried inline),
                #     BYTE MISSING
                0b01111011, 0b00111001,
                59,                     # Next Header: 59 (0x3B) (no next header)
                # truncated in-line part of address
                0x02, 0x00, 0x00, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination multicast address compression:",
                "    48 bits carried inline",
                "00000000  3B  02  00  00  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=01, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=10 (32 bits of all-nodes address carried inline)
                0b01111011, 0b00111010,
                59,                     # Next Header: 59 (0x3B) (no next header)
                # in-line part of address
                0x02, 0x00, 0x00, 0x01,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 0  next header: 59  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: ff02::1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=10",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=10 (32 bits of all-nodes address carried inline),
                #     BYTE MISSING
                0b01111011, 0b00111010,
                59,                     # Next Header: 59 (0x3B) (no next header)
                0x02, 0x00, 0x00,       # truncated in-line part of address
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination multicast address compression:",
                "    32 bits carried inline",
                "00000000  3B  02  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=10, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=11 (8 bits of all-nodes address carried inline)
                0b01111011, 0b00111011,
                59,                     # Next Header: 59 (0x3B) (no next header)
                0x01,                   # in-line part of address
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 0  next header: 59  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: ff02::1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=11",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=0, DAM=11 (8 bits of all-nodes address carried inline),
                #     BYTE MISSING
                0b01111011, 0b00111011,
                59,                     # Next Header: 59 (0x3B) (no next header)
                # in-line part of address missing...
            ],
            [
                "PKTDUMP: data received:",
                "Stateless destination multicast address compression:",
                "    8 bits carried inline",
                "00000000  3B",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=0, DAM=11, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=1, DAM=00 (48 bits of unicast-prefix multicast address
                #   [RFC3306 and RFC3956] carried inline
                0b01111011, 0b10111100,
                0x11,                   # SCI = 0x1, DCI = 0x1
                59,                     # Next Header: 59 (0x3B) (no next header)
                # in-line part of address
                0x02, 0x34, 0x56, 0x78, 0xab, 0xcd,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 0  next header: 59  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: ff02:3440:fd01::5678:abcd",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "M=1, DAC=1, DAM=00",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=1, DAM=00 (48 bits of unicast-prefix multicast address
                #   [RFC3306 and RFC3956] carried inline), BYTE MISSING
                0b01111011, 0b10111100,
                0x11,                   # SCI = 0x1, DCI = 0x1
                59,                     # Next Header: 59 (0x3B) (no next header)
                # truncated in-line part of address
                0x02, 0x34, 0x56, 0x78, 0xab,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful destination multicast address compression:",
                "    48 bits carried inline (Unicast-Prefix-based)",
                "SCI: 0x1, DCI: 0x1",
                "00000000  3B  02  34  56  78  AB",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=1, DAM=00, byte missing",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=1, DAM=10 (reserved => should fail)
                0b01111011, 0b10111110,
                0x11,                   # SCI = 0x1, DCI = 0x1
                59,                     # Next Header: 59 (0x3B) (no next header)
                # and some payload for good measure
                0x02, 0x34, 0x56, 0x78, 0xab, 0xcd, 0xef, 0x13,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful destination multicast address compression:",
                "    reserved",
                "SCI: 0x1, DCI: 0x1",
                "00000000  3B  02  34  56  78  AB  CD  EF  13",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=1, DAM=10",
        ),
        (
            [   # IPHC dispatch, all possible fields elided, except
                #   NH: 59,
                #   M=1, DAC=1, DAM=11 (reserved => should fail)
                0b01111011, 0b10111111,
                0x11,                   # SCI = 0x1, DCI = 0x1
                59,                     # Next Header: 59 (0x3B) (no next header)
                # and some payload for good measure
                0x02, 0x34, 0x56, 0x78, 0xab, 0xcd, 0xef, 0x14,
            ],
            [
                "PKTDUMP: data received:",
                "Stateful destination multicast address compression:",
                "    reserved",
                "SCI: 0x1, DCI: 0x1",
                "00000000  3B  02  34  56  78  AB  CD  EF  14",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "M=1, DAC=1, DAM=11",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x01,           # one byte hop-by-hop options header
                0x00,           # A Pad1 option
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = UDP (17 = 0x11),
                # 1 Pad1 option (00), 1 PadN option of length 5 (01 03 00 00 00)
                "00000000  11  00  00  01  03  00  00  00",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 16  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + Hop-by-hop extension header NHC (Pad1)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x01,           # one byte hop-by-hop options header
                # options missing
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  01",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + truncated hop-by-hop extension header NHC (Pad1)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x05,           # 5 bytes hop-by-hop options header
                0x01, 0x03, 0x00, 0x00, 0x00,  # A 5-byte PadN option
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = UDP (17 = 0x11),
                # 1 PadN option of length 5 (01 03 00 00 00), 1 Pad1 option (00)
                "00000000  11  00  01  03  00  00  00  00",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 16  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + Hop-by-hop extension header NHC (PadN, N-1)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x05,           # 5 bytes hop-by-hop options header
                0x01, 0x03, 0x00, 0x00,  # truncated 5-byte PadN option
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  05  01  03  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + truncated hop-by-hop extension header NHC (PadN, N-1)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x04,           # 4 bytes hop-by-hop options header
                0x01, 0x03, 0x00, 0x00,     # A 4-byte PadN option
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = UDP (17 = 0x11),
                # 1 PadN option of length 4 (01 03 00 00),
                # 1 PadN option of length 2 (01 00)
                "00000000  11  00  01  03  00  00  01  00",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 16  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + Hop-by-hop extension header NHC (PadN, N-2)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x04,           # 4 bytes hop-by-hop options header
                0x01, 0x03, 0x00,  # truncated 4-byte PadN option
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  04  01  03  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + truncated hop-by-hop extension header NHC (PadN, N-2)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = UDP (17 = 0x11),
                # 1 PadN option of length 4 (01 04 00 00 00 00),
                "00000000  11  00  01  04  00  00  00  00",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 16  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + Hop-by-hop extension header NHC (PadN, N=8)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00,     # truncated 6-byte PadN option
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  06  01  04  00  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + Truncated hop-by-hop extension header NHC (PadN, N=8)",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # Destination option dispatch, with another NHC following
                0b11100111,
                0x06,           # 6 bytes dst opt header
                0x1e, 0x00, 0x00, 0x00, 0x00, 0x00,     # Experimental dst opt header
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Destination header with next header = UDP (17 = 0x11),
                # Experimental option (0x1E),
                "00000000  11  00  1E  00  00  00  00  00",
                "~~ SNIP  2 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = Destination option (0x3c),
                # 1 PadN option of length 4 (01 04 00 00 00 00),
                "00000000  3C  00  01  04  00  00  00  00",
                "~~ SNIP  3 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 24  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + >1 extension headers NHC",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with another NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # Destination option dispatch, with another NHC following
                0b11100111,
                0x06,           # 6 bytes dst opt header
                # Truncated experimental dst opt header
                0x1e, 0x00, 0x00, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  06  01  04  00  00  00  00  "
                "E7  06  1E  00  00  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + >1 extension headers NHC, truncated",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with no NHC following
                0b11100000,
                59,             # NH = 59 (no next header)
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  48 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "00000000  3B  00  01  04  00  00  00  00",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + extension header + no next header",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with no NHC following
                0b11100000,
                59,             # NH = 59 (no next header)
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00,     # truncated 6-byte PadN option
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E0  3B  06  01  04  00  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + extension header + no next header, truncated",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with no NHC following
                0b11100000,
                0x11,           # NH = 17 (UDP)
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # uncompressed UDP header
                # (src-port 61616 (0xf0b0), dst-port 61616, length: 8, checksum)
                0xf0, 0xb0, 0xf0, 0xb0, 0x00, 0x08, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = UDP (17 = 0x11),
                # 1 PadN option of length 4 (01 04 00 00 00 00),
                "00000000  11  00  01  04  00  00  00  00",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 16  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + extension header + non-NHC next header",
        ),
        # truncating the test above would test gnrc_udp instead of gnrc_sixlowpan,
        # so skip
        (
            [   # IPHC dispatch, all possible fields elided (hop limit 255)
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # IPv6 header dispatch
                0b11101110,
                # IPHC dispatch, all possible fields elided (hop limit 1)
                0b01111101, 0b00110011,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61616  dst-port: 61616",
                "   length: 8  cksum: 0x24f7",
                "~~ SNIP  1 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 1",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "~~ SNIP  2 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                # Hop-by-hop header with next header = UDP (17 = 0x11),
                # 1 PadN option of length 4 (01 04 00 00 00 00),
                "00000000  29  00  01  04  00  00  00  00",
                "~~ SNIP  3 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 56  next header: 0  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                # outer header
                "IPv6 packets received in app: 1 (queue capacity: 4)",
                # inner header
                "IPv6 packets received in app: 2 (queue capacity: 4)",
            ],
            "Minimal IPHC inside extension header + minimal IPHC",
        ),
        (
            [   # IPHC dispatch, all possible fields elided (hop limit 255)
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # IPv6 header dispatch
                0b11101111,
                # IPHC dispatch, all possible fields elided (hop limit 1)
                0b01111101, 0b00110011,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  06  01  04  00  00  00  "
                "00  EF  7D  33  F3  00  24  F7",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC inside extension header + minimal IPHC, NH != 0",
        ),
        (
            [   # IPHC dispatch, all possible fields elided (hop limit 255)
                0b01111111, 0b00110011,
                # Hop-by-hop option dispatch, with NHC following
                0b11100001,
                0x06,           # 6 bytes hop-by-hop options header
                0x01, 0x04, 0x00, 0x00, 0x00, 0x00,     # A 6-byte PadN option
                # IPv6 header dispatch
                0b11101110,
                # truncated IPHC dispatch
                0b01111101,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E1  06  01  04  00  00  00  00  EE  7D",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC inside extension header + truncated minimal IPHC",
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # truncated UDP NHC
                0b11110111,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  F7",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + truncated NHC (missing ports)"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # truncated UDP NHC
                0b11110011, 0x00, 0x23
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  F3  00  23",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + truncated NHC (missing parts of checksum)"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # UDP NHC with full inline ports and 16-bit checksum
                0b11110000, 0xbc, 0x9d, 0xf0, 0xb0, 0x57, 0x8b,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 48285  dst-port: 61616",
                "   length: 8  cksum: 0x578b",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + UDP NHC with full ports"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # truncated UDP NHC
                0b11110100, 0xbc, 0x9d, 0xf0
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  F4  BC  9D  F0",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + UDP NHC with full ports, truncated"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # UDP NHC with compressed destination port, full source port
                # and 16-bit checksum
                0b11110001, 0xbc, 0x9e, 0xb0, 0x57, 0x8a,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 48286  dst-port: 61616",
                "   length: 8  cksum: 0x578a",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + UDP NHC with compressed destination port"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # truncated UDP NHC
                0b11110101, 0xbc, 0x9e,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  F5  BC  9E",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + UDP NHC with compressed destination port, truncated"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # UDP NHC with compressed source port, full destination port
                # and 16-bit checksum
                0b11110010, 0x9f, 0xf0, 0xb0, 0x23, 0x89,
            ],
            [
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:   0 byte, type: NETTYPE_UNDEF (0)",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_UDP (5)",
                "   src-port: 61599  dst-port: 61616",
                "   length: 8  cksum: 0x2389",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 8  next header: 17  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
            ],
            "Minimal IPHC + UDP NHC with compressed source port"
        ),
        (
            [   # IPHC dispatch, all possible fields elided
                0b01111111, 0b00110011,
                # truncated UDP NHC
                0b11110110, 0x9f, 0xf0,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  F6  9F  F0",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC + UDP NHC with compressed source port, truncated"
        ),
        (
            # generated by Microsoft Copilot
            [
                # Minimal IPHC header:
                # TF=11, NH=1, HLIM=11, SAC=0, SAM=11,
                # M=0, DAC=0, DAM=11
                0b01111111, 0b00110011,

                # Hop-by-Hop IPv6 Extension NHC:
                # EID=HOPOPT, NH=1 (no following NHC)
                0b11100001,

                # Compressed extension-header body length
                0xff,

                # 255-byte compressed body
                *([0x00] * 255),
            ],
            [
                "PKTDUMP: data received:",
                "00000000  E1  FF  00  00  00  00  00  00  "
                "00  00  00  00  00  00  00  00",
            ] +
            [
                f"{i:08X}" + "".join(
                    "  00" for _ in range(i, i + 16 if i + 16 < 255 + 2 else 255 + 2)
                ) for i in range(16, 255 + 2, 16)
            ] +
            [
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Hop-by-hop NHC with 255-byte body",
        ),
        (
            # generated by Microsoft Copilot
            [
                # IPHC dispatch, all possible fields elided:
                #   TF=11 (elided), NH=1 (NHC follows), HLIM=11 (elided)
                0b01111111,
                #   SAC=0, SAM=11 (elided), M=0, DAC=0, DAM=11 (elided)
                0b00110011,
                # NOTHING FOLLOWS — packet ends exactly where the NHC
                # dispatch byte would be expected
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Minimal IPHC, NH=1, but packet ends right after IPHC header",
        ),
        (
            # generated by Microsoft Copilot
            [
                # Minimal IPHC:
                # NH=1, HLIM=255, stateless link-local addresses elided
                0x7f, 0x33,

                # Fragment-header NHC:
                # EID=Fragment, NH bit clear => inline next-header follows
                0xe4,
                0x29,                   # Next Header = IPv6
                0x06,                   # compressed extension-header length

                # Fragment header payload; offset = 1, therefore non-initial fragment
                0x00, 0x00, 0x08, 0x00, 0x00, 0x00,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E4  29  06  00  00  08  00  00  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Non-initial Fragment header falsely claiming IPv6 payload",
        ),
        (
            # generated by Microsoft Copilot
            [
                # Minimal IPHC header:
                # TF=11, NH=1, HLIM=11, SAC=0, SAM=11,
                # M=0, DAC=0, DAM=11
                0b01111111, 0b00110011,

                # Reserved IPv6 extension-header NHC EID.
                # 0xea has EID=5 and NH=0, so an inline next-header and
                # extension-header length byte follow.
                0xea,
                0x11,       # inline next header: UDP
                0x00,       # compressed extension-header body length
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  EA  11  00",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Reserved IPv6 extension-header NHC must make progress",
        ),
        (
            # Partly generated by Copilot
            [   # IPHC dispatch, all possible fields elided (hop limit 255)
                0b01111111, 0b00110011,
                # Fragment header dispatch, with uncompressed NH following
                0b11100100,
                41,           # NH = IPv6
                0x06,         # compressed Fragment body length: 6 bytes
                # Fragment body:
                # offset_flags = 0x0008 => non-zero fragment offset (offset = 1)
                # identification = 0x00000000
                0x00, 0x08, 0x00, 0x00, 0x00, 0x00,
                # Fragment payload: 40 bytes, deliberately resembling an IPv6 header
                *([0xaa] * 40),
            ],
            [
                # 6LoWPAN parses this correctly to an IPv6 packet, but it should be
                # discarded by the IPv6 layer
                "PKTDUMP: data received:",
                "~~ SNIP  0 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "illegal version field: 10",
                "traffic class: 0xaa (ECN: 0x2, DSCP: 0x2a)",
                "flow label: 0xaaaaa",
                "length: 43690  next header: 170  hop limit: 170",
                "source address: aaaa:aaaa:aaaa:aaaa:aaaa:aaaa:aaaa:aaaa",
                "destination address: aaaa:aaaa:aaaa:aaaa:aaaa:aaaa:aaaa:aaaa",
                "~~ SNIP  1 - size:   8 byte, type: NETTYPE_IPV6_EXT (3)",
                "00000000  29  00  00  08  00  00  00  00",
                "~~ SNIP  2 - size:  40 byte, type: NETTYPE_IPV6 (2)",
                "traffic class: 0x00 (ECN: 0x0, DSCP: 0x00)",
                "flow label: 0x00000",
                "length: 48  next header: 44  hop limit: 255",
                "source address: fe80::ff:fe00:2",
                "destination address: fe80::ff:fe00:1",
                "IPv6 packets received in app: 1 (queue capacity: 4)",
                "IPv6 packets received in app: 2 (queue capacity: 4)",
            ],
            "Non-initial Fragment header with IPv6-looking payload",
        ),
        (
            [   # IPHC dispatch, all possible fields elided (hop limit 255)
                0b01111111, 0b00110011,

                # Fragment-header NHC:
                # EID=Fragment, NH=1: Illegal NHC
                0b11100101,
                0x06,                   # compressed Fragment body length: 6 bytes

                # Fragment body:
                # offset_flags = 0x0008 => non-zero fragment offset (offset = 1)
                # identification = 0x00000000
                0x00, 0x08, 0x00, 0x00, 0x00, 0x00,

                # IPv6 header dispatch
                0b11101110,
                # IPHC dispatch, all possible fields elided (hop limit 1)
                0b01111101, 0b00110011,
                # UDP NHC with inline port 4-bit suffixes and 16-bit checksum
                0b11110011, 0x00, 0x24, 0xf7,
            ],
            [
                "PKTDUMP: data received:",
                "NH: LOWPAN_NHC",
                "00000000  E5  06  00  08  00  00  00  00  EE  7D  33  F3  00  24  F7",
                "No IPv6 packets received (queue capacity: 4)",
            ],
            "Non-initial Fragment header with illegal NHC after fragment header",
        ),
    ]:
        print(f"\033[1;34m=== Testing {name} ===\033[00m")
        inject(child, byts)
        for line in expect_lines:
            try:
                child.expect_exact(line)
            except pexpect.TIMEOUT:
                print(
                    f"\033[1;31m{name} test failed on expect \"{line}\"!\033[00m",
                    file=sys.stderr,
                )
                raise

    if not pktbuf_malloc:
        child.sendline("pktbuf")
        child.expect(
            r"packet buffer: first byte: .*, last byte: .* \(size: (\d+)\)"
        )
        child.expect(
            rf"~ unused: .* \(next: .*, size: {child.match.group(1)}\) ~"
        )


if __name__ == "__main__":
    sys.exit(run(testfunc))
