#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.netif with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import re
import unittest

import pexpect

import riotctrl_shell.netif
import riotctrl_shell.od
import riotctrl_shell.sys

import riotctrl_shell_tests.testcase_base


class IfconfigTestShell(
    riotctrl_shell.netif.Ifconfig,
    riotctrl_shell.sys.Reboot,
):
    pass


class TestNetifLwIP(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = IfconfigTestShell(cls.ctrl)

    def setUp(self):
        res = self.shell.reboot()
        # in case some board decides to put some "> " into its boot message...
        res = self.consume_remaining_help_text(res)
        c_eth_name = re.compile(r"Started Ethernet interface (.+)\.")
        c_802154_name = re.compile(r"Started IEEE 802\.15\.4 interface (.+)\.")
        self.eth_name = None
        self.ieee802154_name = None
        for line in res.splitlines():
            if match := c_eth_name.match(line):
                self.eth_name = match[1]
            if match := c_802154_name.match(line):
                self.ieee802154_name = match[1]
        assert self.eth_name is not None, "Could not identify Ethernet PID"
        assert self.ieee802154_name is not None, r"Could not identify IEEE 802\.15\.4 PID"
        super().setUp()
        self.parser = riotctrl_shell.netif.IfconfigListParser()

    def consume_remaining_help_text(self, help_text):
        # help text has a lot of `> ` (which causes a ShellInteraction command to return)
        # which we want to also collect
        help_text += "> "
        while True:
            try:
                self.ctrl.term.expect(r".+", timeout=0.1)
                help_text += self.ctrl.term.match[0]
            except pexpect.TIMEOUT:
                break
        return help_text[:-2]  # remove swallowed actual prompt "> "

    def test_ifconfig_list(self):
        res = self.shell.ifconfig_list()
        netifs = self.parser.parse(res)
        # Ethernet interface and IEEE 802.15.4
        assert len(netifs) == 2
        assert self.eth_name in netifs
        assert self.ieee802154_name in netifs
        # this is more of a lucky break that something named `inet6_addr`
        # is parsed. This should become `ipv6_addr` and more in line with
        # the GNRC output at some point.
        assert "inet6_addr" in netifs[self.eth_name]
        assert "inet6_addr" in netifs[self.ieee802154_name]

    def test_ifconfig_cmd_wo_netif(self):
        with self.assertRaises(ValueError) as ctx:
            self.shell.ifconfig_cmd(args="up")

        assert str(ctx.exception) == "netif required when args are provided"


if __name__ == "__main__":
    unittest.main()
