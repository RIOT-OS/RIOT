# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import logging
import riotctrl_shell.gnrc

from .common import init_ctrl


logger = logging.getLogger(__name__)


def test_ping6():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCICMPv6Echo(rc)
    res = si.ping6("::1", interval=100)
    # mock just returns last input
    assert "ping6" in res
    assert " ::1 " in res
    assert " -i 100 " in res


def test_ping6_parser_success():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("""
12 bytes from ::1: icmp_seq=0 ttl=64 time=0.435 ms
12 bytes from ::1: icmp_seq=1 ttl=64 time=0.433 ms
12 bytes from ::1: icmp_seq=2 ttl=64 time=0.432 ms

--- ::1 PING statistics ---
3 packets transmitted, 3 packets received, 0% packet loss
round-trip min/avg/max = 0.432/0.433/0.435 ms""")
    assert ping_res is not None
    assert "rtts" in ping_res
    assert "avg" in ping_res["rtts"]


def test_ping6_parser_empty():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("")
    assert ping_res is None


def test_ping6_parser_missing_rtts():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("""
12 bytes from ::1: icmp_seq=0 ttl=64 time=0.553 ms
12 bytes from ::1: icmp_seq=1 ttl=64 time=0.496 ms
12 bytes from ::1: icmp_seq=2 ttl=64 time=0.496 ms

--- ::1 PING statistics ---
3 packets transmitted, 3 packets received, 0% packet loss""")
    assert ping_res is None


def test_pktbuf():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCPktbufStats(rc)
    res = si.pktbuf_stats()
    # mock just returns last input
    assert res == "pktbuf"


def test_pktbuf_parser_success_empty():
    parser = riotctrl_shell.gnrc.GNRCPktbufStatsParser()
    pktbuf_res = parser.parse("""
packet buffer: first byte: 0x5660dce0, last byte: 0x5660fce0 (size: 8192)
  position of last byte used: 1792
~ unused: 0x5660dce0 (next: (nil), size: 8192) ~""")
    assert pktbuf_res is not None
    assert pktbuf_res["first_byte"] > 0
    assert "start" in pktbuf_res["first_unused"]
    assert pktbuf_res.is_empty()


def test_pktbuf_parser_success_not_empty():
    parser = riotctrl_shell.gnrc.GNRCPktbufStatsParser()
    pktbuf_res = parser.parse("""
packet buffer: first byte: 0x5660dce0, last byte: 0x5660fce0 (size: 8192)
  position of last byte used: 1792
~ unused: 0x5660de00 (next: (nil), size: 7904) ~""")
    assert pktbuf_res is not None
    assert pktbuf_res["first_byte"] > 0
    assert "start" in pktbuf_res["first_unused"]
    assert not pktbuf_res.is_empty()


def test_pktbuf_parser_empty():
    parser = riotctrl_shell.gnrc.GNRCPktbufStatsParser()
    pktbuf_res = parser.parse("")
    assert pktbuf_res is None


def test_pktbuf_parser_2nd_header_not_found():
    parser = riotctrl_shell.gnrc.GNRCPktbufStatsParser()
    pktbuf_res = parser.parse(
        "packet buffer: first byte: 0x5668ace0, last byte: 0x5668cce0 "
        "(size: 8192)"
    )
    assert pktbuf_res is None
