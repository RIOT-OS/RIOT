# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pytest

import riotctrl_shell.gnrc

from .common import init_ctrl


def test_ping6():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCICMPv6Echo(rc)
    res = si.ping6("::1", interval=100)
    # mock just returns last input
    assert "ping" in res
    assert " ::1 " in res
    assert " -i 100 " in res


def test_ping6_parser_success1():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("""
12 bytes from ::1: icmp_seq=0 ttl=64 time=0.435 ms
12 bytes from ::1: icmp_seq=1 ttl=64 time=0.433 ms
12 bytes from ::1: icmp_seq=2 ttl=64 time=0.432 ms

--- ::1 PING statistics ---
3 packets transmitted, 3 packets received, 0% packet loss
round-trip min/avg/max = 0.432/0.433/0.435 ms""")
    assert ping_res
    assert "rtts" in ping_res
    assert "avg" in ping_res["rtts"]


def test_ping6_parser_success2():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("""
12 bytes from ::1: icmp_seq=0 ttl=64
12 bytes from ::1: icmp_seq=1 ttl=64
12 bytes from ::1: icmp_seq=1 ttl=64 (DUP)

--- ::1 PING statistics ---
2 packets transmitted, 3 packets received, 1 duplicates, 0% packet loss
round-trip min/avg/max = 0.432/0.433/0.435 ms""")
    assert ping_res
    assert "rtts" in ping_res
    assert "avg" in ping_res["rtts"]
    assert len(ping_res["replies"]) == 3
    assert ping_res["replies"][2]["dup"]


def test_ping6_parser_empty():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("")
    assert not ping_res


def test_ping6_parser_missing_rtts():
    parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()
    ping_res = parser.parse("""
12 bytes from ::1: icmp_seq=0 ttl=64
12 bytes from ::1: icmp_seq=1 ttl=64
12 bytes from ::1: icmp_seq=2 ttl=64

--- ::1 PING statistics ---
3 packets transmitted, 3 packets received, 0% packet loss""")
    assert ping_res
    assert "rtts" not in ping_res
    assert len(ping_res["replies"]) == 3


@pytest.mark.parametrize(
    "cmd,args,expected",
    [("neigh", None, "nib neigh"),
     ("route", ("show",), "nib route show"),
     ("snafu", ("foobar", "hello", "world"), "nib snafu foobar hello world")]
)
def test_nib_cmd(cmd, args, expected):
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_cmd(cmd, args)
    # mock just returns last input
    assert res == expected


@pytest.mark.parametrize(
    "method,iface,expected",
    [("nib_neigh_show", None, "nib neigh show"),
     ("nib_neigh_show", 5, "nib neigh show 5"),
     ("nib_prefix_show", None, "nib prefix show"),
     ("nib_prefix_show", 5, "nib prefix show 5"),
     ("nib_route_show", None, "nib route show"),
     ("nib_route_show", 5, "nib route show 5")]
)
def test_nib_show(method, iface, expected):
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = getattr(si, method)(iface)
    # mock just returns last input
    assert res == expected


def test_nib_abr_show():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_abr_show()
    # mock just returns last input
    assert res == "nib abr show"


@pytest.mark.parametrize(
    "method,iface,arg,expected",
    [("nib_neigh_del", 42, "dead:beef::1", "nib neigh del 42 dead:beef::1"),
     ("nib_prefix_del", 42, "dead:beef::/64",
      "nib prefix del 42 dead:beef::/64"),
     ("nib_route_del", 42, "::", "nib route del 42 ::")]
)
def test_nib_del(method, iface, arg, expected):
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = getattr(si, method)(iface, arg)
    # mock just returns last input
    assert res == expected


@pytest.mark.parametrize(
    "method,iface,arg",
    [("nib_neigh_del", 42, "not an address"),
     ("nib_prefix_del", 42, "not an address"),
     ("nib_route_del", 42, "not an address")]
)
def test_nib_del_error(method, iface, arg):
    rc = init_ctrl(output="This is representing some really unformatted error")
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    with pytest.raises(RuntimeError):
        getattr(si, method)(iface, arg)


def test_nib_abr_del():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_abr_del("c0ff::ee")
    # mock just returns last input
    assert res == "nib abr del c0ff::ee"


def test_nib_abr_del_err():
    rc = init_ctrl(output="This is representing some really unformatted error")
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    with pytest.raises(RuntimeError):
        si.nib_abr_del("c0ff::ee")


@pytest.mark.parametrize(
    "l2addr,expected",
    [(None, "nib neigh add 12 affe::1"),
     ("ab:cd:ef", "nib neigh add 12 affe::1 ab:cd:ef")]
)
def test_nib_neigh_add(l2addr, expected):
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_neigh_add(12, "affe::1", l2addr)
    assert res == expected


@pytest.mark.parametrize(
    "valid_sec,pref_sec,expected",
    [(None, None, "nib prefix add 12 dead:c0ff:ee::/45"),
     (120, None, "nib prefix add 12 dead:c0ff:ee::/45 120"),
     (120, 60, "nib prefix add 12 dead:c0ff:ee::/45 120 60")]
)
def test_nib_prefix_add(valid_sec, pref_sec, expected):
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_prefix_add(12, "dead:c0ff:ee::/45", valid_sec, pref_sec)
    assert res == expected


@pytest.mark.parametrize(
    "valid_sec,pref_sec",
    [(None, None), (120, None), (120, 60)]
)
def test_nib_prefix_add_runtime_error(valid_sec, pref_sec):
    rc = init_ctrl(output="This is representing some really unformatted error")
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    with pytest.raises(RuntimeError):
        si.nib_prefix_add(12, "dead:c0ff:ee::/45", valid_sec, pref_sec)


def test_nib_prefix_add_value_error():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    with pytest.raises(ValueError):
        si.nib_prefix_add(12, "dead:c0ff:ee::/45", pref_sec=60)


@pytest.mark.parametrize(
    "route,ltime_sec,expected",
    [("default", None, "nib route add 12 default fe80::1"),
     ("dead:c0ff:ee::/45", None, "nib route add 12 dead:c0ff:ee::/45 fe80::1"),
     ("default", 60, "nib route add 12 default fe80::1 60"),
     ("dead:c0ff:ee::/45", 120,
      "nib route add 12 dead:c0ff:ee::/45 fe80::1 120")]
)
def test_nib_route_add(route, ltime_sec, expected):
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_route_add(12, route, "fe80::1", ltime_sec)
    assert res == expected


def test_nib_abr_add():
    rc = init_ctrl()
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    res = si.nib_abr_add("dead:c0ff:ee::1")
    assert res == "nib abr add dead:c0ff:ee::1"


def test_nib_abr_add_error():
    rc = init_ctrl(output="This is representing some really unformatted error")
    si = riotctrl_shell.gnrc.GNRCIPv6NIB(rc)
    with pytest.raises(RuntimeError):
        si.nib_abr_add("dead:c0ff:ee::1")


@pytest.mark.parametrize(
    "parser",
    [riotctrl_shell.gnrc.GNRCIPv6NIBNeighShowParser,
     riotctrl_shell.gnrc.GNRCIPv6NIBPrefixShowParser,
     riotctrl_shell.gnrc.GNRCIPv6NIBRouteShowParser,
     riotctrl_shell.gnrc.GNRCIPv6NIBABRShowParser]
)
def test_nib_parsers_empty(parser):
    """
    Most NIB parser tests were already done in doctests.
    Just ensure with this test, that empty input generates an empty list
    """
    res = parser().parse("")
    assert res == []


@pytest.mark.parametrize(
    "parser",
    [riotctrl_shell.gnrc.GNRCIPv6NIBNeighShowParser,
     riotctrl_shell.gnrc.GNRCIPv6NIBPrefixShowParser,
     riotctrl_shell.gnrc.GNRCIPv6NIBRouteShowParser,
     riotctrl_shell.gnrc.GNRCIPv6NIBABRShowParser]
)
def test_nib_parsers_garbage(parser):
    """
    Most NIB parser tests were already done in doctests.
    Just ensure with this test, that garbage input generates an empty list
    """
    res = parser().parse("""
        Lorem ipsum dolor sit amet,
        consectetur adipiscing elit,
        sed do eiusmod tempor incididunt ut labore
        et dolore magna aliqua.""")
    assert res == []


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
