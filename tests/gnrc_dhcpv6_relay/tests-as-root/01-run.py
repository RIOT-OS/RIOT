#!/usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import re
import subprocess
import time
import unittest

from scapy.all import Ether, IPv6, ICMPv6DestUnreach, UDP, DHCP6, \
    DHCP6_Solicit, DHCP6_Advertise, DHCP6_Request, DHCP6_Confirm, \
    DHCP6_Renew, DHCP6_Rebind, DHCP6_Reply, DHCP6_Release, DHCP6_Decline, \
    DHCP6_Reconf, DHCP6_InfoRequest, DHCP6_RelayForward, DHCP6_RelayReply, \
    DHCP6OptIfaceId, DHCP6OptRelayMsg, DHCP6OptClientId, DHCP6OptUnknown, \
    raw, sendp, AsyncSniffer
from testrunner.unittest import PexpectTestCase


class TestDHCPv6RelayAgent(PexpectTestCase):
    CLIENT_PORT = 546
    SERVER_PORT = 547
    BOARD = os.environ["BOARD"]
    CONFIG_DHCPV6_RELAY_BUFLEN = int(os.environ["CONFIG_DHCPV6_RELAY_BUFLEN"])
    CONFIG_DHCPV6_RELAY_HOP_LIMIT = \
        int(os.environ["CONFIG_DHCPV6_RELAY_HOP_LIMIT"])
    LOGFILE = None
    tap = os.environ["TAP"]

    @classmethod
    def setUpClass(cls):
        super().setUpClass()
        cls._get_bridge()
        cls._get_tap_lladdr()
        if cls.BOARD == "native":
            cls.pre_sniffer_wait = cls.TIMEOUT / 20
            cls.post_sniffer_wait = cls.TIMEOUT / 20
        else:
            cls.pre_sniffer_wait = cls.TIMEOUT / 20
            cls.post_sniffer_wait = cls.TIMEOUT / 20
        cls._get_node_ifaceid()
        cls.spawn.sendline('ifconfig')
        # check if node joined All_DHCP_Relay_Agents_and_Servers
        cls.spawn.expect_exact('inet6 group: ff02::1:2')

    def setUp(self):
        self.test_trid = 0xc0ffee

    @staticmethod
    def _check_and_search_output(cmd, pattern, res_group, *args, **kwargs):
        output = subprocess.check_output(cmd, *args, **kwargs).decode()
        for line in output.splitlines():
            match = re.search(pattern, line)
            if match is not None:
                return match.group(res_group)
        return None

    @classmethod
    def _get_bridge(cls):
        res = cls._check_and_search_output(
                ["bridge", "link"],
                r"{}.+master\s+(?P<master>[^\s]+)".format(cls.tap),
                "master"
            )
        cls.tap = cls.tap if res is None else res

    @classmethod
    def _get_tap_lladdr(cls):
        res = cls._check_and_search_output(
                ["ip", "addr", "show", "dev", cls.tap, "scope", "link"],
                r"inet6\s+(?P<lladdr>[0-9A-Fa-f:]+)/\d+",
                "lladdr"
            )
        if res is None:
            raise AssertionError(
                "Can't find host link-local address on interface {}".format(
                    cls.tap
                )
            )
        cls.tap_lladdr = res

    @classmethod
    def _get_node_ifaceid(cls):
        pkt = DHCP6_Solicit()
        result = cls.send_and_exp_pkts(pkt, DHCP6_RelayForward)
        assert len(result) > 0
        cls.node_ifaceid = result[0][DHCP6OptIfaceId].ifaceid

    @staticmethod
    def _contains_dhcp(pkt):
        # just using DHCP6 in pkt does not work
        def expand(pkt):
            yield pkt
            while pkt.payload:
                pkt = pkt.payload
                yield pkt
        return any(isinstance(layer,
                              (DHCP6, DHCP6_RelayForward, DHCP6_RelayReply))
                   for layer in expand(pkt))

    @classmethod
    def _udp(cls, client=True):
        if client:
            return UDP(sport=cls.CLIENT_PORT, dport=cls.SERVER_PORT)
        else:
            return UDP(sport=cls.SERVER_PORT, dport=cls.SERVER_PORT)

    @staticmethod
    def _lower_headers(ipv6_src=None):
        return Ether(dst="33:33:00:01:00:02") / \
            IPv6(src=ipv6_src, dst="ff02::1:2")

    @classmethod
    def _sendp(cls, pkt, client=True, ipv6_src=None):
        sendp(cls._lower_headers(ipv6_src=ipv6_src) / cls._udp(client=client) /
              pkt, iface=cls.tap, verbose=0 if cls.LOGFILE is None else 2)

    @classmethod
    def send_and_exp_pkts(cls, send_pkt, exp_type, client=True):
        sniffer = AsyncSniffer(iface=cls.tap)
        sniffer.start()
        time.sleep(cls.pre_sniffer_wait)
        cls._sendp(send_pkt, client=client)
        time.sleep(cls.post_sniffer_wait)
        return [pkt for pkt in sniffer.stop() if exp_type in pkt and
                # filter out sent packet
                (UDP not in pkt or
                 raw(pkt[UDP].payload) != raw(send_pkt)) and
                # filter out ICMPv6 since error notifications can also contain
                # the sent packets and we are not interested in error
                # notifications
                ICMPv6DestUnreach not in pkt]

    @classmethod
    def send_and_exp_any_dhcp6(cls, send_pkt, client=True, ipv6_src=None):
        sniffer = AsyncSniffer(iface=cls.tap)
        sniffer.start()
        time.sleep(cls.pre_sniffer_wait)
        cls._sendp(send_pkt, client=client, ipv6_src=ipv6_src)
        time.sleep(cls.post_sniffer_wait)
        return [pkt for pkt in sniffer.stop() if cls._contains_dhcp(pkt) and
                # filter out sent packet
                (UDP not in pkt or
                 raw(pkt[UDP].payload) != raw(send_pkt)) and
                # filter out ICMPv6 since error notifications can also contain
                # the sent packets and we are not interested in error
                # notifications
                ICMPv6DestUnreach not in pkt]

    def assert_node_responsive(self):
        self.spawn.sendline("")
        self.spawn.expect(r"> ")

    def assert_legal_relay_forward(self, pkt, exp_hopcount):
        self.assertEqual(pkt[UDP].dport, self.SERVER_PORT)
        self.assertEqual(pkt[DHCP6_RelayForward].hopcount, exp_hopcount)
        # either linkaddr is set or there is an Interface-ID option in
        # the message
        if pkt[DHCP6_RelayForward].linkaddr == "::":
            self.assertIn(DHCP6OptIfaceId, pkt)
            # assure ifaceid for later tests
            self.assertEqual(pkt[DHCP6OptIfaceId].ifaceid,
                             self.node_ifaceid)
        self.assertIn(DHCP6OptRelayMsg, pkt)

    def assert_pkt_ignored(self, pkt, client_pkt=True, ipv6_src=None):
        result = self.send_and_exp_any_dhcp6(pkt, client=client_pkt,
                                             ipv6_src=ipv6_src)
        self.assert_empty(result)
        self.assert_node_responsive()

    def assert_len(self, collection, exp_len):
        assert len(collection) == exp_len, \
            f'{collection} is not of length {exp_len}'

    def assert_empty(self, collection):
        assert len(collection) == 0, f'{collection} is not empty'

    def assert_not_empty(self, collection):
        assert len(collection) > 0, f'{collection} is empty'

    def assert_relayed_reply_pkt_w_trid(self, pkt, exp_rpkt):
        self.assertNotIn(DHCP6_RelayReply, pkt)
        self.assertNotIn(DHCP6OptIfaceId, pkt)
        self.assertNotIn(DHCP6OptRelayMsg, pkt)
        self.assertIn(exp_rpkt, pkt)
        self.assertEqual(pkt[exp_rpkt].trid, self.test_trid)

    def test_dhcpv6_client_msgs(self):
        for msg_type in [DHCP6_Solicit, DHCP6_Request, DHCP6_Confirm,
                         DHCP6_Renew, DHCP6_Rebind, DHCP6_Release,
                         DHCP6_Decline, DHCP6_InfoRequest]:
            pkt = msg_type(trid=self.test_trid)
            result = self.send_and_exp_pkts(pkt, DHCP6_RelayForward)
            self.assert_len(result, 1)
            pkt = result[0]
            self.assert_legal_relay_forward(pkt, 0)
            self.assertIn(msg_type, pkt[DHCP6OptRelayMsg].message)
            self.assertEqual(pkt[DHCP6OptRelayMsg][msg_type].trid,
                             self.test_trid)
            self.assert_node_responsive()

    def test_dhcpv6_server_msgs(self):
        for msg_type in [DHCP6_Advertise, DHCP6_Reply, DHCP6_Reconf]:
            pkt = msg_type(trid=self.test_trid)
            self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_client_msg_too_long(self):
        pkt = DHCP6_Solicit() / \
            DHCP6OptUnknown(optcode=1,
                            optlen=self.CONFIG_DHCPV6_RELAY_BUFLEN) / \
            (b"x" * self.CONFIG_DHCPV6_RELAY_BUFLEN)
        self.assert_pkt_ignored(pkt)

    def test_dhcpv6_client_msg_too_small(self):
        for dhcp6_len in range(len(DHCP6())):
            with self.subTest(dhcp6_len=dhcp6_len):
                pkt = (b"\x01" * dhcp6_len)
                self.assert_pkt_ignored(pkt)

    def test_dhcpv6_client_msg_from_unspec(self):
        self.assert_pkt_ignored(DHCP6_Confirm(), ipv6_src="::")

    def test_dhcpv6_client_msg_too_long_for_fwd(self):
        buflen = self.CONFIG_DHCPV6_RELAY_BUFLEN
        buflen -= len(DHCP6_Solicit())        # remove SOLICIT header
        buflen -= len(DHCP6OptUnknown())      # remove option header of SOLICIT
        buflen -= len(DHCP6_RelayForward())   # remove RELAY-FORWARD header
        # remove Interface-ID option of RELAY-FORWARD
        buflen -= len(DHCP6OptIfaceId(ifaceid=self.node_ifaceid))
        # remove Relay-Message option header of RELAY-FORWARD
        buflen -= len(DHCP6OptUnknown(optlen=0))

        pkt = DHCP6_Solicit() / \
            DHCP6OptUnknown(optcode=1, optlen=buflen) / (b"x" * buflen)
        result = self.send_and_exp_pkts(pkt, DHCP6_RelayForward)
        # should just fit
        self.assert_len(result, 1)
        self.assert_legal_relay_forward(result[0], 0)
        self.assert_node_responsive()
        buflen += 1
        pkt = DHCP6_Solicit() / \
            DHCP6OptUnknown(optcode=1, optlen=buflen) / (b"x" * buflen)
        # SOLICIT should be too long
        self.assert_pkt_ignored(pkt)

    def _test_dhcpv6_relay_forward(self, pkt, hopcount, exp_peeraddr,
                                   exp_ifaceid):
        result = self.send_and_exp_pkts(pkt, DHCP6_RelayForward, client=False)
        self.assert_len(result, 1)
        pkt = result[0]
        self.assert_legal_relay_forward(pkt, hopcount + 1)
        rpkt = pkt[DHCP6OptRelayMsg].message[DHCP6_RelayForward]
        self.assertEqual(rpkt.peeraddr, exp_peeraddr)
        self.assertEqual(rpkt[DHCP6OptIfaceId].ifaceid, exp_ifaceid)
        self.assert_node_responsive()
        return rpkt

    def test_dhcpv6_relay_forward(self):
        hopcount = self.CONFIG_DHCPV6_RELAY_HOP_LIMIT - 1
        peeraddr = "fe80::f00:1337"
        ifaceid = b"ab"
        self.assertGreater(hopcount, 0)
        pkt = DHCP6_RelayForward(peeraddr=peeraddr, hopcount=hopcount) / \
            DHCP6OptIfaceId(ifaceid=ifaceid) / \
            DHCP6OptRelayMsg(message=DHCP6_Rebind(trid=self.test_trid))
        rpkt = self._test_dhcpv6_relay_forward(pkt, hopcount, peeraddr,
                                               ifaceid)
        self.assertIn(DHCP6_Rebind, rpkt[DHCP6OptRelayMsg].message)
        self.assertEqual(rpkt[DHCP6OptRelayMsg].message[DHCP6_Rebind].trid,
                         self.test_trid)

    def test_dhcpv6_relay_forward_options_reversed(self):
        hopcount = self.CONFIG_DHCPV6_RELAY_HOP_LIMIT - 1
        peeraddr = "fe80::f00:1337"
        ifaceid = b"ab"
        self.assertGreater(hopcount, 0)
        pkt = DHCP6_RelayForward(peeraddr=peeraddr, hopcount=hopcount) / \
            DHCP6OptRelayMsg(message=DHCP6_Rebind(trid=self.test_trid)) / \
            DHCP6OptIfaceId(ifaceid=ifaceid)
        rpkt = self._test_dhcpv6_relay_forward(pkt, hopcount, peeraddr,
                                               ifaceid)
        self.assertIn(DHCP6_Rebind, rpkt[DHCP6OptRelayMsg].message)
        self.assertEqual(rpkt[DHCP6OptRelayMsg].message[DHCP6_Rebind].trid,
                         self.test_trid)

    def test_dhcpv6_relay_forward_hop_limit_exceeded(self):
        test_trid = 0xc0ffee
        hoplimit = self.CONFIG_DHCPV6_RELAY_HOP_LIMIT
        pkt = DHCP6_RelayForward(hopcount=hoplimit + 1,
                                 peeraddr="fe80::f00:1337") / \
            DHCP6OptIfaceId(ifaceid=b"ab") / \
            DHCP6OptRelayMsg(message=DHCP6_Rebind(trid=test_trid))
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_forward_too_small(self):
        for i in range(len(DHCP6()), len(DHCP6_RelayForward())):
            pkt = (chr(12) + ("\0" * (i - 1))).encode()
            self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_forward_from_unspec(self):
        hopcount = self.CONFIG_DHCPV6_RELAY_HOP_LIMIT - 1
        peeraddr = "fe80::f00:1337"
        ifaceid = b"ab"
        self.assertGreater(hopcount, 0)
        pkt = DHCP6_RelayForward(peeraddr=peeraddr, hopcount=hopcount) / \
            DHCP6OptIfaceId(ifaceid=ifaceid) / \
            DHCP6OptRelayMsg(message=DHCP6_Rebind(trid=self.test_trid))
        self.assert_pkt_ignored(pkt, ipv6_src="::")

    def _test_dhcpv6_simple_relay_reply(self, pkt, exp_rpkt):
        result = self.send_and_exp_pkts(pkt, exp_rpkt, client=False)
        self.assert_len(result, 1)
        pkt = result[0]
        self.assert_relayed_reply_pkt_w_trid(pkt, exp_rpkt)
        self.assert_node_responsive()

    def test_dhcpv6_simple_relay_reply(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptRelayMsg(message=DHCP6_Advertise(trid=self.test_trid))
        self._test_dhcpv6_simple_relay_reply(pkt, DHCP6_Advertise)

    def test_dhcpv6_simple_relay_reply_options_reversed(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid)) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid)
        self._test_dhcpv6_simple_relay_reply(pkt, DHCP6_Reply)

    def test_dhcpv6_simple_relay_reply_foreign_option(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptClientId() / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid))
        result = self.send_and_exp_pkts(pkt, DHCP6_Reply, client=False)
        self.assert_len(result, 1)
        pkt = result[0]
        self.assert_relayed_reply_pkt_w_trid(pkt, DHCP6_Reply)
        self.assert_node_responsive()

    def test_dhcpv6_simple_relay_reply_foreign_option_w_bogus_optlen(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid)) / \
            DHCP6OptClientId(optlen=32)
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_nested_relay_reply(self):
        peeraddr = "fe80::f00:affe"
        ifaceid = b"abcd"
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptRelayMsg(
                message=DHCP6_RelayReply(peeraddr=peeraddr) /
                    # noqa: E131 (easier to read with this indentation)
                    DHCP6OptIfaceId(ifaceid=ifaceid) /
                    DHCP6OptRelayMsg(
                        message=DHCP6_Reconf(trid=self.test_trid)
                    )
            )
        result = self.send_and_exp_pkts(pkt, DHCP6_Reconf, client=False)
        self.assert_len(result, 1)
        pkt = result[0]
        self.assertIn(DHCP6_RelayReply, pkt)
        self.assertNotIn(DHCP6_RelayReply, pkt[DHCP6_RelayReply].message)
        self.assertEqual(pkt[DHCP6_RelayReply].peeraddr, peeraddr)
        self.assertIn(DHCP6OptIfaceId, pkt)
        self.assertEqual(pkt[DHCP6OptIfaceId].ifaceid, ifaceid)
        self.assertIn(DHCP6_Reconf, pkt[DHCP6_RelayReply].message)
        self.assertEqual(pkt[DHCP6_Reconf].trid, self.test_trid)
        self.assert_node_responsive()

    def test_dhcpv6_relay_reply_too_small(self):
        for dhcp6_len in range(len(DHCP6()), len(DHCP6_RelayReply())):
            with self.subTest(dhcp6_len=dhcp6_len):
                pkt = (chr(13) + ("\0" * (dhcp6_len - 1))).encode()
                self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_reply_unexpeted_ifaceid_len(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid="hello!") / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid))
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_reply_unexpected_ifaceid(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=b"ab") / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid))
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_reply_unexpected_peeraddr(self):
        pkt = DHCP6_RelayReply(peeraddr="fe80::abcd:f00:1337") / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid))
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_reply_invalid_optlen(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptRelayMsg(optlen=32,
                             message=DHCP6_Reply(trid=self.test_trid))
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_reply_no_ifaceid_no_linkaddr(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptRelayMsg(message=DHCP6_Reply(trid=self.test_trid))
        self.assert_pkt_ignored(pkt, client_pkt=False)

    def test_dhcpv6_relay_reply_empty(self):
        pkt = DHCP6_RelayReply(peeraddr=self.tap_lladdr) / \
            DHCP6OptIfaceId(ifaceid=self.node_ifaceid) / \
            DHCP6OptRelayMsg()
        self.assert_pkt_ignored(pkt, client_pkt=False)


if __name__ == "__main__":
    unittest.main()
