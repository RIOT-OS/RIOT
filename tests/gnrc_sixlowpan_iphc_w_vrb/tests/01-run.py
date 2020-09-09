#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run, check_unittests


def testfunc(child):
    debug_enabled = child.expect([
        r"OK \(\d+ tests\)",
        r"6lo iphc: VRB present, trying to create entry for dst 2001:db8::2",
    ])
    if debug_enabled:
        # only import scapy dependency if DEBUG_ENABLE=1
        from scapy.all import SixLoWPAN, LoWPANFragmentationFirst, \
                              LoWPAN_IPHC, IPv6, ICMPv6EchoRequest

        # test_recv__success
        child.expect_exact("6lo iphc: found route, trying to forward")
        child.expect(
            r"6lo iphc: Do not know how to forward fragment from "
            r"\((?P<vrb_src>[a-fA-F0-9:]+), (?P<vrb_src_tag>\d+)\) to "
            r"\((?P<vrb_dst>[a-fA-F0-9:]+), (?P<vrb_dst_tag>\d+)\)"
        )
        vrb_src_tag = int(child.match.group("vrb_src_tag"))
        child.expect_exact("Original fragmentation header:")
        child.expect("(?P<hex>(  [A-Fa-f0-9]{2})+)")
        frag = SixLoWPAN(bytes.fromhex(child.match.group("hex")))
        assert LoWPANFragmentationFirst in frag
        assert frag[LoWPANFragmentationFirst].datagramTag == vrb_src_tag
        stream = ""
        child.expect_exact("IPHC headers + payload:")
        for _ in range(7):
            child.expect("(?P<hex>(  [A-Fa-f0-9]{2})+)")
            stream += child.match.group("hex")
        iphc = SixLoWPAN(bytes.fromhex(stream))
        assert LoWPAN_IPHC in iphc
        assert IPv6 in iphc     # The IPHC header after compression
        assert ICMPv6EchoRequest in iphc
        # check against fields of original fragment specified in main.c
        assert iphc[LoWPAN_IPHC].tf == 0x3
        assert iphc[LoWPAN_IPHC].nh == 0b0      # Inline (ICMPv6)
        assert iphc[LoWPAN_IPHC].hlim == 0b00   # Inline as it was decremented
        assert not iphc[LoWPAN_IPHC].cid
        assert iphc[LoWPAN_IPHC].sac == 0b0     # Stateless
        assert iphc[LoWPAN_IPHC].sam == 0b00    # Inline
        assert not iphc[LoWPAN_IPHC].m          # not multicast
        assert iphc[LoWPAN_IPHC].dac == 0b0     # Stateless
        assert iphc[LoWPAN_IPHC].dam == 0b00    # Inline
        assert iphc[IPv6].src == "2001:db8::1"
        assert iphc[IPv6].dst == "2001:db8::2"
        assert iphc[IPv6].hlim == (64 - 1)  # hop limit was decremented
        assert iphc[ICMPv6EchoRequest].code == 0
        assert iphc[ICMPv6EchoRequest].cksum == 0x8ea0
        assert iphc[ICMPv6EchoRequest].id == 0x238f
        assert iphc[ICMPv6EchoRequest].seq == 2
        assert iphc[ICMPv6EchoRequest].data.startswith(
            bytes.fromhex("9d4bb21c5353535353535353535353535353535353535353")
        )

        # test_recv__no_route
        child.expect_exact(
            "6lo iphc: VRB present, trying to create entry for dst 2001:db8::2"
        )
        child.expect_exact(
            "6lo iphc: no route found, reassemble datagram normally"
        )

        # test_recv__vrb_full
        child.expect_exact(
            "6lo iphc: VRB present, trying to create entry for dst 2001:db8::2"
        )
        child.expect_exact(
            "6lo iphc: no route found, reassemble datagram normally"
        )

        # test_recv__pkt_held
        child.expect_exact("6lo iphc: found route, trying to forward")
        child.expect(
            r"6lo iphc: Do not know how to forward fragment from "
            r"\((?P<vrb_src>[a-fA-F0-9:]+), (?P<vrb_src_tag>\d+)\) to "
            r"\((?P<vrb_dst>[a-fA-F0-9:]+), (?P<vrb_dst_tag>\d+)\)"
        )
        child.expect_exact("Original fragmentation header:")
        child.expect_exact("IPHC headers + payload:")
        assert check_unittests(child) >= 4


if __name__ == "__main__":
    sys.exit(run(testfunc))
