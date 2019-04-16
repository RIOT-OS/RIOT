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
