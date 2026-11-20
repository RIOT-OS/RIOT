#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.gnrc with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import re
import unittest

import riotctrl_shell.gnrc
import riotctrl_shell.netif
import riotctrl_shell.sys
from testrunner import flush_child

import riotctrl_shell_tests.testcase_base


class EchoShellInteraction(
    riotctrl_shell.gnrc.GNRCICMPv6Echo,
    riotctrl_shell.netif.Ifconfig,
    riotctrl_shell.sys.Reboot,
):
    pass


class TestGNRCICMPv6Echo(riotctrl_shell_tests.testcase_base.TestBase):
    TEST_HOST = "example.com"
    TEST_IP = "::1"

    @classmethod
    def init_shell_interaction(cls):
        cls.shell = EchoShellInteraction(cls.ctrl)

    def setUp(self):
        self.shell.reboot()
        # in case some board decides to put some "> " into its boot message...
        flush_child(self.ctrl.term)
        super().setUp()
        self.parser = riotctrl_shell.gnrc.GNRCICMPv6EchoParser()

    def check_rtts(self, ping):
        assert ping["rtts"]["avg"] > 0
        assert ping["rtts"]["max"] > 0
        assert ping["rtts"]["min"] > 0

    def check_stats(self, ping):
        assert ping["stats"]["tx"] > 0
        assert ping["stats"]["rx"] > 0

    def check_replies(self, ping, count=3, exp_src=TEST_IP, exp_ttl=64):
        assert len(ping["replies"]) == count
        for i, reply in enumerate(ping["replies"]):
            assert reply["source"] == exp_src
            assert reply["seq"] == i
            assert reply["rtt"] >= 0
            assert reply["ttl"] == exp_ttl

    def _test_ping6_count_2(self, hostname, exp_src=TEST_IP):
        count = 2
        res = self.shell.ping6(hostname, count=count)
        ping = self.parser.parse(res)

        self.check_rtts(ping)
        self.check_stats(ping)
        self.check_replies(ping, count=count, exp_src=exp_src)

    def test_ping6_hostname(self):
        self._test_ping6_count_2(self.TEST_HOST)

    def test_ping6_ip_addr(self):
        self._test_ping6_count_2(self.TEST_IP)

    def test_ping6_if_addr(self):
        res = self.shell.ifconfig_list()
        if_parser = riotctrl_shell.netif.IfconfigListParser()
        netifs = if_parser.parse(res)
        # just take first key of interfaces
        netif = list(netifs)[0]

        ip_addr = "fe80::1"
        self.shell.ifconfig_add(netif, ip_addr)

        self._test_ping6_count_2(f"{ip_addr}%{netif}", ip_addr)

    def test_ping6_count_timeout(self):
        count = 26
        interval = 45

        res = self.shell.ping6(self.TEST_HOST, count=count, interval=interval)
        ping = self.parser.parse(res)

        self.check_rtts(ping)
        self.check_stats(ping)
        self.check_replies(ping, count=count)

    def test_ping6_hoplimit(self):
        res = self.shell.ping6(self.TEST_IP, hop_limit=24)
        ping = self.parser.parse(res)

        self.check_rtts(ping)
        self.check_stats(ping)
        self.check_replies(ping)

    def test_ping6_packet_size(self):
        res = self.shell.ping6(self.TEST_IP, packet_size=1024)
        ping = self.parser.parse(res)

        self.check_rtts(ping)
        self.check_stats(ping)
        self.check_replies(ping)


class NIBShellInteraction(
    riotctrl_shell.gnrc.GNRCIPv6NIB,
    riotctrl_shell.netif.Ifconfig,
    riotctrl_shell.sys.Reboot,
):
    pass


class TestGNRCIPv6NIB(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = NIBShellInteraction(cls.ctrl)

    def setUp(self):
        self.shell.reboot()
        # in case some board decides to put some "> " into its boot message...
        flush_child(self.ctrl.term)
        super().setUp()
        self.netif_parser = riotctrl_shell.netif.IfconfigListParser()
        self.neigh_parser = riotctrl_shell.gnrc.GNRCIPv6NIBNeighShowParser()
        self.prefix_parser = riotctrl_shell.gnrc.GNRCIPv6NIBPrefixShowParser()
        self.route_parser = riotctrl_shell.gnrc.GNRCIPv6NIBRouteShowParser()
        self.abr_parser = riotctrl_shell.gnrc.GNRCIPv6NIBABRShowParser()

    def parse_ifconfig_list(self):
        netifs = self.netif_parser.parse(self.shell.ifconfig_list())
        assert netifs
        return netifs

    def show_neighbors(self, netif):
        res = self.shell.nib_neigh_show(netif)
        return self.neigh_parser.parse(res)

    def show_prefixes(self, netif):
        res = self.shell.nib_prefix_show(netif)
        return self.prefix_parser.parse(res)

    def show_routes(self, netif):
        res = self.shell.nib_route_show(netif)
        return self.route_parser.parse(res)

    def show_abrs(self):
        res = self.shell.nib_abr_show()
        return self.abr_parser.parse(res)

    def test_nib_neigh_show(self):
        res = self.shell.nib_neigh_show()
        neighbors = self.neigh_parser.parse(res)
        assert not neighbors

        netif = list(self.parse_ifconfig_list())[0]
        res = self.shell.nib_neigh_show(netif)
        neighbors = self.neigh_parser.parse(res)
        assert not neighbors

    def _test_nib_neigh_add(self):
        with self.assertRaises(RuntimeError):
            self.shell.nib_neigh_add("foobar", "fe80::1")
        # flush help text out
        flush_child(self.ctrl.term)

        netif = list(self.parse_ifconfig_list())[0]
        with self.assertRaises(RuntimeError):
            self.shell.nib_neigh_add(netif, "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        self.shell.nib_neigh_add(netif, "fe80::1")
        neighbors = self.show_neighbors(netif)
        assert len(neighbors) == 1
        assert neighbors[0]["ipv6_addr"] == "fe80::1"
        assert neighbors[0]["iface"] == int(netif)

        with self.assertRaises(RuntimeError):
            self.shell.nib_neigh_add(netif, "fe80::1", "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        self.shell.nib_neigh_add(netif, "fe80::2", "00:01:02:03:04:05")
        neighbors = self.show_neighbors(netif)
        assert len(neighbors) == 2
        assert neighbors[0]["ipv6_addr"] == "fe80::1"
        assert neighbors[0]["iface"] == int(netif)
        assert neighbors[1]["ipv6_addr"] == "fe80::2"
        assert neighbors[1]["l2addr"] == "00:01:02:03:04:05"
        assert neighbors[1]["iface"] == int(netif)

        return netif, neighbors

    def test_nib_neigh_add(self):
        self._test_nib_neigh_add()

    def test_nib_neigh_del(self):
        netif, added_neighbors = self._test_nib_neigh_add()

        with self.assertRaises(RuntimeError):
            self.shell.nib_neigh_del("foobar", "fe80::1")
        # flush help text out
        flush_child(self.ctrl.term)

        with self.assertRaises(RuntimeError):
            self.shell.nib_neigh_del(netif, "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        # check precondition
        neighbors = self.show_neighbors(netif)
        assert len(neighbors) == 2
        assert neighbors[0]["ipv6_addr"] == added_neighbors[0]["ipv6_addr"]
        assert neighbors[0]["iface"] == added_neighbors[0]["iface"]
        assert neighbors[1]["ipv6_addr"] == added_neighbors[1]["ipv6_addr"]
        assert neighbors[1]["l2addr"] == added_neighbors[1]["l2addr"]
        assert neighbors[1]["iface"] == added_neighbors[1]["iface"]

        self.shell.nib_neigh_del(
            added_neighbors[0]["iface"], added_neighbors[0]["ipv6_addr"]
        )
        neighbors = self.show_neighbors(netif)
        assert len(neighbors) == 1
        assert neighbors[0]["ipv6_addr"] == added_neighbors[1]["ipv6_addr"]
        assert neighbors[0]["l2addr"] == added_neighbors[1]["l2addr"]
        assert neighbors[0]["iface"] == added_neighbors[1]["iface"]

        self.shell.nib_neigh_del(
            added_neighbors[1]["iface"], added_neighbors[1]["ipv6_addr"]
        )
        neighbors = self.show_neighbors(netif)
        assert len(neighbors) == 0

    def test_nib_prefix_show(self):
        res = self.shell.nib_prefix_show()
        prefixes = self.prefix_parser.parse(res)
        assert not prefixes

        netif = list(self.parse_ifconfig_list())[0]
        res = self.shell.nib_prefix_show(netif)
        prefixes = self.prefix_parser.parse(res)
        assert not prefixes

    def _test_nib_prefix_add(self):
        with self.assertRaises(RuntimeError):
            self.shell.nib_prefix_add("foobar", "2001:db8:1::1")
        # flush help text out
        flush_child(self.ctrl.term)

        netif = list(self.parse_ifconfig_list())[0]
        with self.assertRaises(RuntimeError):
            self.shell.nib_prefix_add(netif, "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        self.shell.nib_prefix_add(netif, "2001:db8:1::1")
        prefixes = self.show_prefixes(netif)
        assert len(prefixes) == 1
        assert prefixes[0]["prefix"] == "2001:db8:1::1"
        assert prefixes[0]["prefix_len"] == 128
        assert prefixes[0]["iface"] == int(netif)

        self.shell.nib_prefix_add(netif, "2001:db8:2::/47", 2000, 1000)
        prefixes = self.show_prefixes(netif)
        assert len(prefixes) == 2
        assert prefixes[0]["prefix"] == "2001:db8:1::1"
        assert prefixes[0]["prefix_len"] == 128
        assert prefixes[0]["iface"] == int(netif)
        assert prefixes[1]["prefix"] == "2001:db8:2::"
        assert prefixes[1]["prefix_len"] == 47
        assert prefixes[1]["iface"] == int(netif)
        assert 0 < prefixes[1]["valid_sec"] < 2000
        assert 0 < prefixes[1]["pref_sec"] < 1000

        return netif, prefixes

    def test_nib_prefix_add(self):
        self._test_nib_prefix_add()

    def test_nib_prefix_del(self):
        netif, added_prefixes = self._test_nib_prefix_add()

        with self.assertRaises(RuntimeError):
            self.shell.nib_prefix_del("foobar", "2001:db8:1::1")
        # flush help text out
        flush_child(self.ctrl.term)

        with self.assertRaises(RuntimeError):
            self.shell.nib_prefix_del(netif, "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        # check precondition
        prefixes = self.show_prefixes(netif)
        assert len(prefixes) == 2
        assert prefixes[0]["prefix"] == added_prefixes[0]["prefix"]
        assert prefixes[0]["prefix_len"] == added_prefixes[0]["prefix_len"]
        assert prefixes[0]["iface"] == added_prefixes[0]["iface"]
        assert prefixes[1]["prefix"] == added_prefixes[1]["prefix"]
        assert prefixes[1]["prefix_len"] == added_prefixes[1]["prefix_len"]
        assert prefixes[1]["iface"] == added_prefixes[1]["iface"]
        assert 0 < prefixes[1]["valid_sec"] <= added_prefixes[1]["valid_sec"]
        assert 0 < prefixes[1]["pref_sec"] <= added_prefixes[1]["pref_sec"]

        self.shell.nib_prefix_del(netif, "2001:db8:1::1")
        prefixes = self.show_prefixes(netif)
        assert len(prefixes) == 1
        assert prefixes[0]["prefix"] == added_prefixes[1]["prefix"]
        assert prefixes[0]["prefix_len"] == added_prefixes[1]["prefix_len"]
        assert prefixes[0]["iface"] == added_prefixes[1]["iface"]
        assert 0 < prefixes[0]["valid_sec"] <= added_prefixes[1]["valid_sec"]
        assert 0 < prefixes[0]["pref_sec"] <= added_prefixes[1]["pref_sec"]

        self.shell.nib_prefix_del(netif, "2001:db8:2::/47")
        prefixes = self.show_prefixes(netif)
        assert len(prefixes) == 0

    def test_nib_route_show(self):
        res = self.shell.nib_route_show()
        routes = self.route_parser.parse(res)
        assert not routes

        netif = list(self.parse_ifconfig_list())[0]
        res = self.shell.nib_route_show(netif)
        routes = self.route_parser.parse(res)
        assert not routes

    def _test_nib_route_add(self, default_route="default"):
        with self.assertRaises(RuntimeError):
            self.shell.nib_route_add("foobar", "2001:db8:1::1", "fe80::1")
        # flush help text out
        flush_child(self.ctrl.term)

        netif = list(self.parse_ifconfig_list())[0]
        with self.assertRaises(RuntimeError):
            self.shell.nib_route_add(netif, "foobar", "fe80::1")
        # flush help text out
        flush_child(self.ctrl.term)

        with self.assertRaises(RuntimeError):
            self.shell.nib_route_add(netif, "2001:db8:1::1", "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        self.shell.nib_route_add(netif, default_route, "fe80::1")
        routes = self.show_routes(netif)
        assert len(routes) == 1
        assert routes[0]["iface"] == int(netif)
        assert routes[0]["route"]["default"]
        assert routes[0]["route"]["primary"]
        assert routes[0]["next_hop"] == "fe80::1"

        self.shell.nib_route_add(netif, "2001:db8:2::/47", "fe80::2", 3600)
        routes = self.show_routes(netif)
        assert len(routes) == 2
        assert routes[0]["iface"] == int(netif)
        assert routes[0]["route"]["prefix"] == "2001:db8:2::"
        assert routes[0]["route"]["prefix_len"] == 47
        assert routes[0]["next_hop"] == "fe80::2"
        assert routes[1]["iface"] == int(netif)
        assert routes[1]["route"]["default"]
        assert routes[1]["route"]["primary"]
        assert routes[1]["next_hop"] == "fe80::1"

        self.shell.nib_prefix_add(netif, "2001:db8:1::/64")
        routes = self.show_routes(netif)
        assert len(routes) == 3
        assert routes[0]["iface"] == int(netif)
        assert routes[0]["route"]["prefix"] == "2001:db8:2::"
        assert routes[0]["route"]["prefix_len"] == 47
        assert routes[0]["next_hop"] == "fe80::2"
        assert routes[1]["iface"] == int(netif)
        assert routes[1]["route"]["prefix"] == "2001:db8:1::"
        assert routes[1]["route"]["prefix_len"] == 64
        assert "next_hop" not in routes[1]
        assert routes[2]["iface"] == int(netif)
        assert routes[2]["route"]["default"]
        assert routes[2]["route"]["primary"]
        assert routes[2]["next_hop"] == "fe80::1"

        return netif, routes

    def test_nib_route_add(self):
        self._test_nib_route_add()

    def test_nib_route_del(self):
        netif, added_routes = self._test_nib_route_add(default_route="::")

        with self.assertRaises(RuntimeError):
            self.shell.nib_route_del(netif, "foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        routes = self.show_routes(netif)
        assert len(routes) == 3
        assert routes[0]["iface"] == added_routes[0]["iface"]
        assert routes[0]["route"] == added_routes[0]["route"]
        assert routes[0]["next_hop"] == added_routes[0]["next_hop"]
        assert routes[1]["iface"] == added_routes[1]["iface"]
        assert routes[1]["route"] == added_routes[1]["route"]
        assert routes[2]["iface"] == added_routes[2]["iface"]
        assert routes[2]["route"] == added_routes[2]["route"]
        assert routes[2]["next_hop"] == added_routes[2]["next_hop"]

        self.shell.nib_prefix_del(netif, "2001:db8:1::/64")
        routes = self.show_routes(netif)
        assert len(routes) == 2
        assert routes[0]["iface"] == added_routes[0]["iface"]
        assert routes[0]["route"] == added_routes[0]["route"]
        assert routes[0]["next_hop"] == added_routes[0]["next_hop"]
        assert routes[1]["iface"] == added_routes[2]["iface"]
        assert routes[1]["route"] == added_routes[2]["route"]
        assert routes[1]["next_hop"] == added_routes[2]["next_hop"]

        self.shell.nib_route_del(netif, "::")
        routes = self.show_routes(netif)
        assert len(routes) == 1
        assert routes[0]["iface"] == added_routes[0]["iface"]
        assert routes[0]["route"] == added_routes[0]["route"]
        assert routes[0]["next_hop"] == added_routes[0]["next_hop"]

        self.shell.nib_route_del(netif, "2001:db8:2::/47")
        routes = self.show_routes(netif)
        assert len(routes) == 0

    def test_nib_abr_show(self):
        res = self.shell.nib_abr_show()
        routes = self.abr_parser.parse(res)
        assert not routes

    def _test_nib_abr_add(self):
        with self.assertRaises(RuntimeError):
            self.shell.nib_abr_add("foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        with self.assertRaises(RuntimeError):
            self.shell.nib_abr_add("2001:db8::affe")
        # flush help text out
        flush_child(self.ctrl.term)

        netif = list(
            k for k, v in self.parse_ifconfig_list().items() if "6LO" in v["flags"]
        )[0]
        self.shell.ifconfig_add(netif, "2001:db8::affe")
        self.shell.nib_abr_add("2001:db8::affe")
        abrs = self.show_abrs()
        assert len(abrs) == 1
        assert abrs[0]["addr"] == "2001:db8::affe"
        assert abrs[0]["version"] == 0
        assert abrs[0]["valid_min"] > 0

        return abrs

    def test_nib_abr_add(self):
        self._test_nib_abr_add()

    def test_nib_abr_del(self):
        added_abrs = self._test_nib_abr_add()

        with self.assertRaises(RuntimeError):
            self.shell.nib_abr_del("foobar")
        # flush help text out
        flush_child(self.ctrl.term)

        abrs = self.show_abrs()
        assert len(abrs) == 1
        assert abrs[0]["addr"] == added_abrs[0]["addr"]
        assert abrs[0]["version"] == added_abrs[0]["version"]
        assert abrs[0]["valid_min"] <= added_abrs[0]["valid_min"]

        self.shell.nib_abr_del(added_abrs[0]["addr"])
        abrs = self.show_abrs()
        assert len(abrs) == 0


class TestGNRCPktbufStats(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = riotctrl_shell.gnrc.GNRCPktbufStats(cls.ctrl)

    def allocate_pkt(self, byts):
        res = self.shell.cmd(f"pktbuf_add {byts.hex()}")
        cmp = re.compile(
            r"success: allocated packet at (0x(?P<addr>[0-9A-Fa-f]+)|\(nil\))"
        )
        addr = None
        for line in res.splitlines():
            if match := cmp.match(line):
                if "nil" in match["addr"]:
                    addr = 0
                else:
                    addr = int(match["addr"], base=16)
        assert addr is not None and addr > 0
        return addr

    def test_pktbuf_stats(self):
        res = self.shell.pktbuf_stats()
        parser = riotctrl_shell.gnrc.GNRCPktbufStatsParser()
        stats = parser.parse(res)
        assert stats.is_empty()
        assert stats.fullest_capacity() == 0

        byts1 = bytes(range(16))
        addr1 = self.allocate_pkt(byts1)
        res = self.shell.pktbuf_stats()
        stats = parser.parse(res)
        assert not stats.is_empty()
        assert stats.fullest_capacity() > len(byts1) / stats["size"]

        byts2 = bytes(range(17))
        addr2 = self.allocate_pkt(byts2)
        res = self.shell.pktbuf_stats()
        stats = parser.parse(res)
        assert not stats.is_empty()
        assert stats.fullest_capacity() > (len(byts1) + len(byts2)) / stats["size"]

        byts3 = bytes(range(16))
        addr3 = self.allocate_pkt(byts3)
        res = self.shell.pktbuf_stats()
        stats = parser.parse(res)
        assert not stats.is_empty()
        assert (
            stats.fullest_capacity()
            > (len(byts1) + len(byts2) + len(byts3)) / stats["size"]
        )

        # test for whole in pktbuf
        res = self.shell.cmd(f"pktbuf_remove {addr2:x}")
        assert "success:" in res and f"{addr2:x}" in res

        res = self.shell.pktbuf_stats()
        stats = parser.parse(res)
        assert not stats.is_empty()
        assert (
            stats.fullest_capacity()
            > (len(byts1) + len(byts2) + len(byts3)) / stats["size"]
        )

        # test for first chunk not being at the start
        res = self.shell.cmd(f"pktbuf_remove {addr1:x}")
        assert "success:" in res and f"{addr1:x}" in res

        res = self.shell.pktbuf_stats()
        stats = parser.parse(res)
        assert not stats.is_empty()
        assert (
            stats.fullest_capacity()
            > (len(byts1) + len(byts2) + len(byts3)) / stats["size"]
        )

        res = self.shell.cmd(f"pktbuf_remove {addr3:x}")
        assert "success:" in res and f"{addr3:x}" in res

        res = self.shell.pktbuf_stats()
        stats = parser.parse(res)
        assert stats.is_empty()
        assert (
            stats.fullest_capacity()
            > (len(byts1) + len(byts2) + len(byts3)) / stats["size"]
        )


if __name__ == "__main__":
    unittest.main()
