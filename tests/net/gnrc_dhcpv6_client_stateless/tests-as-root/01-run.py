#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#               2021 Jan Romann <jan.romann@uni-bremen.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import pexpect
import random
import sys
import time

from scapy.all import (
    DHCP6_Reply,
    DHCP6_InfoRequest,
    ICMPv6NDOptSrcLLAddr,
    ICMPv6NDOptMTU,
    sendp,
    Ether,
    IPv6,
    UDP,
    ICMPv6ND_RS,
    ICMPv6ND_RA,
    DHCP6OptClientId,
    DHCP6OptServerId,
    ICMPv6NDOptPrefixInfo,
    DUID_LL,
    Automaton,
    ATMT,
)
from testrunner import run

try:
    from scapy.all import DHCP6OptMudUrl

    mud_option_loaded = True
except ImportError:
    from scapy.all import DHCP6OptUnknown

    DHCP6OptMudUrl = DHCP6OptUnknown
    mud_option_loaded = False

TIMEOUT = 1

MUD_OPTION_CODE = 112
MUD_TEST_URL = b"https://example.org"


class StatelessDHCPv6Test(Automaton):
    """
        Scapy Automaton used for performing stateless DHCPv6 tests.
    """

    def parse_args(self, child, **kwargs):
        """
            Initializes the Automaton.

            Receives the TAP interface that is being
            used as a keyword argument (`iface`).
            Also generates a randomized prefix for
            SLAAC testing.
        """
        super().parse_args(**kwargs)
        self.child = child
        self.iface = kwargs["iface"]
        self.prefix = "2001:db8:{:x}:{:x}::".format(
            random.randint(0, 0xFFFF), random.randint(0, 0xFFFF)
        )

    @ATMT.state(initial=1)
    def WAITING_FOR_NDP_RS(self):
        """
            The initial state.

            The Automaton waits for an NDP Router Solication.
        """
        pass

    @ATMT.receive_condition(WAITING_FOR_NDP_RS, prio=1)
    def received_ICMP(self, pkt):
        """
            Checks if an incoming packet contains an NDP Router Solicitaion (RS).

            If an RS has been received, `on_NDP_RS` will be called with the packet
            as an argument and the Automaton's state will change to
            `WAITING_FOR_DHCP_IR`.
        """
        if ICMPv6ND_RS in pkt:
            raise self.WAITING_FOR_DHCP_IR().action_parameters(pkt)

    @ATMT.action(received_ICMP)
    def on_NDP_RS(self, pkt):
        """
            Called when an NDP Router Solicitation has been received.

            Calls `send_RA` to send an NDP Router Advertisement to all IPv6 nodes.
        """
        self.send_RA()

    @staticmethod
    def build_router_advertise_header():
        """
            Builds Ethernet and IPv6 headers for sending a packet to all IPv6 nodes.
        """
        return Ether() / IPv6(dst="ff02::1")

    def send_RA(self):
        """
            Composes and sends an NDP Router Advertisement (RA).

            The RA contains a prefix which will be used by the RIOT
            application for configuring a global IPv6 addresses using
            Stateless Address Autoconfiguration (SLAAC).
        """
        header = self.build_router_advertise_header()
        ra = ICMPv6ND_RA(M=0, O=1)
        src_ll_addr = ICMPv6NDOptSrcLLAddr(lladdr=header[Ether].src)
        mtu = ICMPv6NDOptMTU()
        prefix_info = ICMPv6NDOptPrefixInfo(prefix=self.prefix, prefixlen=64)
        sendp(
            header / ra / src_ll_addr / mtu / prefix_info,
            iface=self.iface,
            verbose=False,
        )

    @ATMT.state()
    def WAITING_FOR_DHCP_IR(self):
        """
            The second state.

            The Automaton waits for a DHCPv6 Information Request.
        """
        pass

    @ATMT.receive_condition(WAITING_FOR_DHCP_IR, prio=1)
    def received_DHCPv6(self, pkt):
        """
            Checks if an expected DHCPv6 Information Request (IR) was received.

            The method asserts that the expected options are contained in the IR,
            triggers `on_DHCPv6_IR` if this is the case, and lets the Automaton
            switch to the final state `END`.
        """
        if DHCP6_InfoRequest in pkt:
            hwaddrs = get_hwaddrs(self.child)

            assert DHCP6OptClientId in pkt and DUID_LL in pkt[DHCP6OptClientId].duid
            assert pkt[DHCP6OptClientId].duid[DUID_LL].lladdr in hwaddrs

            # The information-request contained a MUD URL option
            assert DHCP6OptMudUrl in pkt
            mud_option = pkt[DHCP6OptMudUrl]
            assert mud_option.optlen == len(MUD_TEST_URL)

            if mud_option_loaded:
                assert mud_option.mudstring == MUD_TEST_URL
            else:
                assert mud_option.optcode == MUD_OPTION_CODE
                assert mud_option.data == MUD_TEST_URL

            raise self.END().action_parameters(pkt)

    @ATMT.action(received_DHCPv6)
    def on_DHCPv6_IR(self, pkt):
        """
            Calls `send_DHCPv6_Reply` for sending a DHCPv6 Reply message.
        """
        self.send_DHCPv6_Reply(pkt)

    @staticmethod
    def build_reply_headers(pkt):
        """
            Constructs the Ethernet, IPv6, and UDP headers for the DHCPv6 Reply.

            Uses the received packet for inserting the correct addresses and ports.
        """
        src_ether = pkt[Ether].src
        src_ip = pkt[IPv6].src
        sport = pkt[UDP].sport
        dport = pkt[UDP].dport
        return Ether(dst=src_ether) / IPv6(dst=src_ip) / UDP(sport=dport, dport=sport)

    def send_DHCPv6_Reply(self, pkt):
        """
            Sends out the DHCPv6 Reply message.
        """
        header = self.build_reply_headers(pkt)
        trid = pkt[DHCP6_InfoRequest].trid
        srv_duid = header[Ether].src
        cli_id = DHCP6OptClientId(duid=pkt[DHCP6OptClientId].duid)
        srv_id = DHCP6OptServerId(duid=DUID_LL(lladdr=srv_duid))
        sendp(
            header / DHCP6_Reply(trid=trid) / cli_id / srv_id,
            iface=self.iface,
            verbose=False,
        )

    @ATMT.timeout(WAITING_FOR_NDP_RS, 10.0)
    @ATMT.timeout(WAITING_FOR_DHCP_IR, 10.0)
    def waiting_timeout(self):
        """
            Defines a timeout of 10 seconds for both the first and second state.
        """
        raise self.ERROR_TIMEOUT()

    @ATMT.state(final=1)
    def END(self):
        """
            The final state.

            Checks if the global IPv6 address has been configured correctly and
            terminates the test.
        """
        time.sleep(1)

        # check if global address was configured
        self.child.sendline("ifconfig")
        # remove one trailing ':' from prefix just to be safe ;-)
        self.child.expect(r"inet6 addr:\s+{}[0-9a-fA-F:]+\s".format(self.prefix[:-1]))
        print("SUCCESS")


def get_hwaddrs(child):
    """
        Extracts the RIOT device's MAC Address from the command line for assertions.
    """
    hwaddrs = []
    child.sendline("ifconfig")
    child.expect(r"HWaddr:\s+(([A-Fa-f0-9]{2}:?)+)\s")
    hwaddrs.append(child.match.group(1).lower())
    if len(hwaddrs[0]) == 5:  # short address
        res = child.expect([pexpect.TIMEOUT, r"Long HWaddr:\s+(([A-Fa-f0-9]{2}:?)+)\s"])
        if res > 0:
            hwaddrs.append(child.match.group(1).lower())
    return hwaddrs


def testfunc(child):
    """
        The test function that is called by the test runner.
    """
    iface = os.environ["TAP"]
    StatelessDHCPv6Test(child, iface=iface).run()


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT, echo=True))
