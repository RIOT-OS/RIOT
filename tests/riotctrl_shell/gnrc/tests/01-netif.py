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
    riotctrl_shell.netif.TXTSnd,
):
    pass


class TestNetifGNRC(riotctrl_shell_tests.testcase_base.TestBase):
    ETH_SET_EXP = {
        "addr": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "02:00:00:12:34:58",
            "exp_legal": (
                re.compile(
                    r"success: set \(short\) address on interface (?P<iface>\d+) "
                    "to 02:00:00:12:34:58"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["hwaddr"] == exp["legal_value"],
        },
        "addr_long": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "02:00:00:ff:fe:12:34:58",
            "exp_legal": RuntimeError("error: unable to set long address"),
        },
        "addr_short": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "02:00:00:12:34:59",
            "exp_legal": (
                re.compile(
                    r"success: set \(short\) address on interface (?P<iface>\d+) "
                    "to 02:00:00:12:34:59"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["hwaddr"] == exp["legal_value"],
        },
        "cca_threshold": {
            "legal_value": 12,
            "exp_legal": RuntimeError("error: unable to set CCA threshold [in dBm]"),
        },
        "freq": {
            "legal_value": 3612286262,
            "exp_legal": RuntimeError("error: unable to set frequency [in Hz]"),
        },
        "channel": {
            "legal_value": 19,
            "exp_legal": RuntimeError("error: unable to set channel"),
        },
        "chan": {
            "legal_value": 20,
            "exp_legal": RuntimeError("error: unable to set channel"),
        },
        "csma_retries": {
            "legal_value": 5,
            "exp_legal": RuntimeError("error: unable to set CSMA retries"),
        },
        "hop_limit": {
            "legal_value": 42,
            "exp_legal": (
                re.compile(r"success: set hop limit on interface (?P<iface>\d+) to 42"),
            ),
            "netif_changed": lambda netif, exp: netif["hl"] == exp["legal_value"],
        },
        "hl": {
            "legal_value": 23,
            "exp_legal": (
                re.compile(r"success: set hop limit on interface (?P<iface>\d+) to 23"),
            ),
            "netif_changed": lambda netif, exp: netif["hl"] == exp["legal_value"],
        },
        "key": {
            "illegal_value": "foobar",
            "exp_illegal": ("error: invalid key size",),
            "legal_value": "01020304050607",
            "exp_legal": RuntimeError("error: unable to set encryption key"),
        },
        "mtu": {
            "legal_value": 1244,
            "exp_legal": (
                re.compile(r"success: set MTU on interface (?P<iface>\d+) to 1244"),
            ),
            "netif_changed": lambda netif, exp: netif["mtu"] == exp["legal_value"],
        },
        "nid": {
            "legal_value": 0x09A,
            "exp_legal": RuntimeError("error: unable to set network identifier"),
        },
        "pan": {
            "legal_value": 0x09B,
            "exp_legal": RuntimeError("error: unable to set network identifier"),
        },
        "pan_id": {
            "legal_value": 0x09B,
            "exp_legal": RuntimeError("error: unable to set network identifier"),
        },
        "appkey": {
            "illegal_value": "foobar",
            "exp_illegal": ("error: unable to parse key",),
            "legal_value": "21b06a8a164b4604f195c6925f4656e1",
            "exp_legal": (
                re.compile(
                    r"success: set AppKey on interface (?P<iface>\d+) to "
                    "21b06a8a164b4604f195c6925f4656e1"
                ),
            ),
        },
        "appskey": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "cd:7b:5e:02:44:86:cd:a1",
            "exp_legal": RuntimeError("error: unable to set AppSKey"),
        },
        "appeui": {
            "illegal_value": "foobar",
            "exp_illegal": ("error: unable to parse key",),
            "legal_value": "020000fffe12345a",
            "exp_legal": (
                re.compile(
                    r"success: set AppEUI on interface (?P<iface>\d+) to 020000fffe12345a"
                ),
            ),
        },
        "nwkskey": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "cd:7b:5e:02:44:86:cd:a2",
            "exp_legal": RuntimeError("error: unable to set NwkSKey"),
        },
        "deveui": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "02:00:00:ff:fe:12:34:60",
            "exp_legal": RuntimeError("error: unable to set long address"),
        },
        "dr": {
            "legal_value": 5,
            "exp_legal": RuntimeError("error: unable to set datarate"),
        },
        "rx2_dr": {
            "legal_value": 3,
            "exp_legal": RuntimeError("error: unable to set RX2 datarate"),
        },
        "power": {
            "legal_value": -2013,
            "exp_legal": RuntimeError("error: unable to set TX power [in dBm]"),
        },
        "retrans": {
            "legal_value": 4,
            "exp_legal": RuntimeError("error: unable to set max. retransmissions"),
        },
        "src_len": {
            "legal_value": 6,
            "exp_legal": RuntimeError("error: unable to set source address length"),
        },
        "state": {
            "illegal_value": "uaeuaeuie",
            "exp_illegal": help,
            "legal_value": "off",
            "exp_legal": RuntimeError("error: unable to set state to OFF"),
        },
    }
    IEEE802154_SET_EXP = {
        "addr": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "34:58",
            "exp_legal": (
                re.compile(
                    r"success: set \(short\) address on interface (?P<iface>\d+) to .*"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["hwaddr"] == exp["legal_value"],
        },
        "addr_long": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "02:00:00:ff:fe:12:34:58",
            "exp_legal": (
                re.compile(
                    r"success: set long address on interface (?P<iface>\d+) to .*"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["long_hwaddr"]
            == "02:00:00:FF:FE:12:34:58",
        },
        "addr_short": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "34:59",
            "exp_legal": (
                re.compile(
                    r"success: set \(short\) address on interface (?P<iface>\d+) to .*"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["hwaddr"] == exp["legal_value"],
        },
        "cca_threshold": {
            "legal_value": 63,
            "exp_legal": (
                re.compile(
                    r"success: set CCA threshold \[in dBm\] on interface (?P<iface>\d+) to 63"
                ),
            ),
        },
        "freq": {
            "legal_value": 3612286262,
            "exp_legal": (
                re.compile(
                    r"success: set frequency \[in Hz\] on interface (?P<iface>\d+) to 3612286262"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["frequency"]
            == f"{exp['legal_value']}Hz",
        },
        "channel": {
            "legal_value": 28111,
            "exp_legal": RuntimeError(r"error: unable to set channel"),
        },
        "chan": {
            "legal_value": 52,
            "exp_legal": (
                re.compile(r"success: set channel on interface (?P<iface>\d+) to 52"),
            ),
            "netif_changed": lambda netif, exp: netif["channel"] == exp["legal_value"],
        },
        "csma_retries": {
            "legal_value": 9,
            "exp_legal": (
                re.compile(
                    r"success: set CSMA retries on interface (?P<iface>\d+) to 9"
                ),
            ),
        },
        "hop_limit": {
            "legal_value": 70,
            "exp_legal": (
                re.compile(r"success: set hop limit on interface (?P<iface>\d+) to 70"),
            ),
        },
        "hl": {
            "legal_value": 133,
            "exp_legal": (
                re.compile(
                    r"success: set hop limit on interface (?P<iface>\d+) to 133"
                ),
            ),
        },
        "key": {
            "illegal_value": "foobar",
            "exp_illegal": ("error: invalid key size",),
            "legal_value": "01020304050607",
            "exp_legal": (
                "Notice: setting 56 bit key.",
                re.compile(
                    r"success: set encryption key on interface (?P<iface>\d+) to",
                ),
                "01020304050607",
            ),
        },
        "mtu": {
            "legal_value": 1040,
            "exp_legal": (
                re.compile(r"success: set MTU on interface (?P<iface>\d+) to 1040"),
            ),
            "netif_changed": lambda netif, exp: netif["mtu"] == exp["legal_value"],
        },
        "nid": {
            "legal_value": 0xABCD,
            "exp_legal": (
                re.compile(
                    rf"success: set network identifier on interface (?P<iface>\d+) to {0xabcd}"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["nid"] == exp["legal_value"],
        },
        "pan": {
            "legal_value": "ABCE",
            "exp_legal": (
                re.compile(
                    r"success: set network identifier on interface (?P<iface>\d+) to 0xabce"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["nid"]
            == int(exp["legal_value"], base=16),
        },
        "pan_id": {
            "legal_value": "C0FFEE",
            "exp_legal": RuntimeError("error: unable to parse value."),
        },
        "appkey": {
            "illegal_value": "foobar",
            "exp_illegal": ("error: unable to parse key",),
            "legal_value": "21b06a8a164b4604f195c6925f4656e1",
            "exp_legal": (
                re.compile(
                    r"success: set AppKey on interface (?P<iface>\d+) to "
                    "21b06a8a164b4604f195c6925f4656e1"
                ),
            ),
        },
        "appskey": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "cd:7b:5e:02:44:86:cd:a1",
            "exp_legal": RuntimeError("error: unable to set AppSKey"),
        },
        "appeui": {
            "illegal_value": "foobar",
            "exp_illegal": ("error: unable to parse key",),
            "legal_value": "020000fffe12345a",
            "exp_legal": (
                re.compile(
                    r"success: set AppEUI on interface (?P<iface>\d+) to 020000fffe12345a"
                ),
            ),
        },
        "nwkskey": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "cd:7b:5e:02:44:86:cd:a2",
            "exp_legal": RuntimeError("error: unable to set NwkSKey"),
        },
        "deveui": {
            "illegal_value": "foobar",
            "exp_illegal": (
                "error: unable to parse address",
                "Must be of format [0-9a-fA-F]{2}(:[0-9a-fA-F]{2})*",
                "(hex pairs delimited by colons)",
            ),
            "legal_value": "020000fffe123460",
            "exp_legal": (
                re.compile(
                    r"success: set long address on interface (?P<iface>\d+) to 020000fffe123460"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["long_hwaddr"]
            == "02:00:00:FF:FE:12:34:60",
        },
        "dr": {
            "legal_value": 5,
            "exp_legal": RuntimeError("error: unable to set datarate"),
        },
        "rx2_dr": {
            "legal_value": 3,
            "exp_legal": RuntimeError("error: unable to set RX2 datarate"),
        },
        "power": {
            "legal_value": -2501,
            "exp_legal": (
                re.compile(
                    r"success: set TX power \[in dBm\] on interface (?P<iface>\d+) to -2501"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["tx_power"]
            == f"{exp['legal_value']}dBm",
        },
        "retrans": {
            "legal_value": 10,
            "exp_legal": (
                re.compile(
                    r"success: set max. retransmissions on interface (?P<iface>\d+) to 10"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["max_retrans"]
            == exp["legal_value"],
        },
        "src_len": {
            "legal_value": 2,
            "exp_legal": (
                re.compile(
                    r"success: set source address length on interface (?P<iface>\d+) to 2"
                ),
            ),
            "netif_changed": lambda netif, exp: netif["source_address_length"]
            == exp["legal_value"],
        },
        "state": {
            "illegal_value": "foobar",
            "exp_illegal": help,
            "legal_value": "reset",
            "exp_legal": (
                re.compile(r"success: set state of interface (?P<iface>\d+) to RESET"),
            ),
            "netif_changed": lambda netif, exp: netif["state"]
            == exp["legal_value"].upper(),
        },
    }
    ETH_FLAG_EXP = {
        "6lo": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "abr": {
            "enable": "success: set option",
            "enable_check": lambda netif: "ABR" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "ABR" not in netif["flags"],
        },
        "ack_req": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "gts": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "pan_coord": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "autoack": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "autocca": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "csma": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "encrypt": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "mac_no_sleep": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "fwd": {
            "enable": "success: set option",
            "disable": "success: unset option",
        },
        "iphc": {
            "enable": "success: set option",
            "enable_check": lambda netif: "IPHC" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "IPHC" not in netif["flags"],
        },
        "preload": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "promisc": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "phy_busy": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "raw": {
            "enable": "success: set option",
            "disable": "success: unset option",
        },
        "rtr_adv": {
            "enable": "success: set option",
            "enable_check": lambda netif: "RTR_ADV" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "RTR_ADV" not in netif["flags"],
        },
        "iq_invert": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "rx_single": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "chan_hop": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "checksum": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "otaa": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "link_check": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
    }
    IEEE802154_FLAG_EXP = {
        "6lo": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "abr": {
            "enable": "success: set option",
            "enable_check": lambda netif: "ABR" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "ABR" not in netif["flags"],
        },
        "ack_req": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "gts": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "pan_coord": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "autoack": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "autocca": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "csma": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "encrypt": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "mac_no_sleep": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "fwd": {
            "enable": "success: set option",
            "disable": "success: unset option",
        },
        "iphc": {
            "enable": "success: set option",
            "enable_check": lambda netif: "IPHC" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "IPHC" not in netif["flags"],
        },
        "preload": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "promisc": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "phy_busy": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "raw": {
            "enable": "success: set option",
            "disable": "success: unset option",
        },
        "rtr_adv": {
            "enable": "success: set option",
            "enable_check": lambda netif: "RTR_ADV" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "RTR_ADV" not in netif["flags"],
        },
        "iq_invert": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "rx_single": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "chan_hop": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "checksum": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
        "otaa": {
            "enable": "success: set option",
            "enable_check": lambda netif: "OTAA" in netif["flags"],
            "disable": "success: unset option",
            "disable_check": lambda netif: "OTAA" not in netif["flags"],
        },
        "link_check": {
            "enable": RuntimeError("error: unable to set option"),
            "disable": RuntimeError("error: unable to set option"),
        },
    }

    @classmethod
    def init_shell_interaction(cls):
        cls.shell = IfconfigTestShell(cls.ctrl)

    def setUp(self):
        res = self.shell.reboot()
        # in case some board decides to put some "> " into its boot message...
        res = self.consume_remaining_help_text(res)
        c_eth_pid = re.compile(r"Started Ethernet interface on PID (\d+)\.")
        c_802154_pid = re.compile(r"Started IEEE 802\.15\.4 interface on PID (\d+)\.")
        self.eth_pid = None
        self.ieee802154_pid = None
        for line in res.splitlines():
            if match := c_eth_pid.match(line):
                self.eth_pid = int(match[1])
            if match := c_802154_pid.match(line):
                self.ieee802154_pid = int(match[1])
        assert self.eth_pid is not None, "Could not identify Ethernet PID"
        assert self.ieee802154_pid is not None, "Could not identify Ethernet PID"
        self.set_exp = {
            self.eth_pid: self.ETH_SET_EXP,
            self.ieee802154_pid: self.IEEE802154_SET_EXP,
        }
        self.flag_exp = {
            self.eth_pid: self.ETH_FLAG_EXP,
            self.ieee802154_pid: self.IEEE802154_FLAG_EXP,
        }
        super().setUp()
        self.parser = riotctrl_shell.netif.IfconfigListParser()

    def parse_ifconfig_list(self, pid):
        return self.parser.parse(self.shell.ifconfig_list(pid))

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

    def parse_set_options_from_help_text(self, help_text, netif):
        start_c = re.compile(r"\s*<key> may be one of the following")
        key_help_c = re.compile(r'\s+\* "([^"]+)" - .*')

        parse_options = False
        options = list()
        for line in help_text.splitlines():
            if start_c.match(line):
                parse_options = True
                continue
            if parse_options:
                if match := key_help_c.match(line):
                    options.append(match[1])
        # check for duplicates
        assert len(options) == len(set(options))
        assert len(options) == len(self.set_exp[netif])
        return options

    def parse_flags_from_help_text(self, help_text, netif):
        flags_c = re.compile(r"usage: ifconfig <if_id> \[-\]\{([^}]+)\}")

        flags = []
        for line in help_text.splitlines():
            if match := flags_c.match(line):
                flags = match[1].split("|")
        # check for duplicates
        assert len(flags) == len(set(flags))
        assert len(flags) == len(self.flag_exp[netif])
        return flags

    def test_ifconfig_list(self):
        res = self.shell.ifconfig_list()
        netifs = self.parser.parse(res)
        # Ethernet interface and IEEE 802.15.4
        assert len(netifs) == 2
        assert str(self.eth_pid) in netifs
        # use MTU to identify Ethernet interface
        assert netifs[str(self.eth_pid)]["mtu"] == 1500
        assert str(self.ieee802154_pid) in netifs
        # use MTU to identify 6LoWPAN interface
        assert netifs[str(self.ieee802154_pid)]["mtu"] == 1280

        res = self.shell.ifconfig_list("foobar")

        assert "error: invalid interface given" in res

        netifs = self.parse_ifconfig_list(self.eth_pid)
        assert len(netifs) == 1
        assert str(self.eth_pid) in netifs

        netifs = self.parse_ifconfig_list(self.ieee802154_pid)
        assert len(netifs) == 1
        assert str(self.ieee802154_pid) in netifs

    def test_ifconfig_cmd_wo_netif(self):
        with self.assertRaises(ValueError) as ctx:
            self.shell.ifconfig_cmd(args="up")

        assert str(ctx.exception) == "netif required when args are provided"

    def test_ifconfig_help(self):
        res = self.shell.ifconfig_help(self.eth_pid)
        res = self.consume_remaining_help_text(res)
        assert "usage: ifconfig help" in res
        # Check a non-exhaustive list of help texts.
        # Might be extended if something is amiss.
        assert (
            "usage: ifconfig <if_id> add [anycast|multicast|unicast] "
            "<ipv6_addr>[/prefix_len]" in res
        )
        assert "usage: ifconfig <if_id> del <ipv6_addr>" in res
        assert "usage: ifconfig <if_id> l2filter {add|del} <addr>" in res

    def _test_ifconfig_set_help_text(self, help_start):
        res = self.consume_remaining_help_text(help_start)
        assert "usage: ifconfig <if_id> set" in res

    def _test_ifconfig_set_illegal_value(self, netif, option):
        if "exp_illegal" in self.set_exp[netif][option]:
            exp_illegal = self.set_exp[netif][option]["exp_illegal"]
            # either tuple or the help function
            assert isinstance(exp_illegal, tuple) or callable(exp_illegal)
            illegal_value = self.set_exp[netif][option]["illegal_value"]
            with self.assertRaises(RuntimeError) as ctx:
                self.shell.ifconfig_set(netif, option, illegal_value)
            if exp_illegal == help:
                # this option is advertised through help, but not recognized
                # by the actual command
                self._test_ifconfig_set_help_text(str(ctx.exception))
            else:
                for exp in exp_illegal:
                    if isinstance(exp, re.Pattern):
                        # exp is a `re.compile()`
                        match = exp.search(
                            str(ctx.exception)
                        )
                        assert match, f"'{ctx.exception.strip()}' does not match '{exp}'"
                        if "iface" in match.groupdict():
                            assert int(match["iface"]) == netif
                    else:
                        # exp is a plain string
                        assert exp in str(ctx.exception)

    def _test_ifconfig_set_legal_value(self, netif, option):
        exp_legal = self.set_exp[netif][option]["exp_legal"]
        if isinstance(exp_legal, Exception) or callable(exp_legal):
            # legal value, but causes an exception
            exc = RuntimeError if callable(exp_legal) else type(exp_legal)
            with self.assertRaises(exc) as ctx:
                self.shell.ifconfig_set(
                    netif,
                    option,
                    self.set_exp[netif][option]["legal_value"],
                )
            if exp_legal is help:
                # this option is advertise through help, but not recognized
                # by the actual command
                self._test_ifconfig_set_help_text(str(ctx.exception))
            else:
                assert str(exp_legal) in str(ctx.exception)
        else:
            # legal value provides result text
            # either tuple or the help function
            assert isinstance(exp_legal, tuple)
            legal_value = self.set_exp[netif][option]["legal_value"]
            res = self.shell.ifconfig_set(netif, option, legal_value)
            for exp in exp_legal:
                if isinstance(exp, re.Pattern):
                    # exp is a `re.compile()`
                    match = exp.search(res)
                    assert match, f"'{res.strip()}' does not match '{exp}'"
                    if "iface" in match.groupdict():
                        assert int(match["iface"]) == netif
                else:
                    # exp is a plain string
                    assert exp in res

            if "netif_changed" in self.set_exp[netif][option]:
                netif_obj = self.parse_ifconfig_list(netif)[str(netif)]
                assert self.set_exp[netif][option]["netif_changed"](
                    netif_obj, self.set_exp[netif][option]
                ), f"{option} not as expected in {netif_obj}"

    def _test_ifconfig_set(self, netif):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_set(netif, "foobar30460", "testtestest")
        res = self.consume_remaining_help_text(str(ctx.exception))

        options = self.parse_set_options_from_help_text(res, netif)
        self.shell.ifconfig_list(netif)
        for option in options:
            self._test_ifconfig_set_illegal_value(netif, option)
            self._test_ifconfig_set_legal_value(netif, option)

    def test_ifconfig_set_invalid_iface(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_set("foobar", "addr", "02:00:00:22:34:60")
        assert "error: invalid interface given" in str(ctx.exception)

    def test_ifconfig_set_eth(self):
        self._test_ifconfig_set(self.eth_pid)

    def test_ifconfig_set_radio(self):
        self._test_ifconfig_set(self.ieee802154_pid)

    def test_ifconfig_set_encrypt_key(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_set(self.ieee802154_pid, "key", "0200")
        assert "error: invalid key size" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_set(self.ieee802154_pid, "key", "xx020304050607")
        assert "error: unable to parse key" in str(ctx.exception)

        # key lengths 24, 32, and 64 too long for shell buffer
        for key_len in [7, 8, 16]:
            key = bytes(range(key_len)).hex()
            res = self.shell.ifconfig_set(self.ieee802154_pid, "key", key)
            assert f"Notice: setting {key_len * 8} bit key." in res
            assert (
                f"success: set encryption key on interface {self.ieee802154_pid} to "
                in res
            )
            passed_success = False
            key_found = False
            # search the key in the output and compare it to `key`
            for line in res.splitlines():
                # first search success output, so that we do not accidentally
                # parse the echoed shell command
                if re.search("success:", line):
                    passed_success = True
                    # stay on the line in case output changes to without linebreak
                if passed_success:
                    if match := re.search(rf"[a-fA-F0-9]{{{len(key)}}}", line):
                        assert match[0] == key
                        key_found = True
            assert key_found

    def test_ifconfig_set_state(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_set(self.ieee802154_pid, "state", "foobar")
        res = self.consume_remaining_help_text(str(ctx.exception))
        exp = r"usage: ifconfig <if_id> set state \{([^}]+)\}"
        assert (
            match := re.search(exp, res)
        ) is not None, f"'{res.strip()}' does not match '{exp}'"
        states = match[1].split("|")
        assert len(states) > 2

        for state in states:
            res = self.shell.ifconfig_set(self.ieee802154_pid, "state", state)
            assert (
                f"success: set state of interface {self.ieee802154_pid} to {state.upper()}"
            ) in res
            netif = self.parse_ifconfig_list(self.ieee802154_pid)[
                str(self.ieee802154_pid)
            ]
            assert netif["state"] == state.upper()

    def test_ifconfig_up_down(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_up("foobar")
        assert "error: invalid interface given" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_down("foobar")
        assert "error: invalid interface given" in str(ctx.exception)

        netifs = self.parse_ifconfig_list(self.eth_pid)
        assert netifs[str(self.eth_pid)]["link"] == "down"

        self.shell.ifconfig_up(self.eth_pid)

        netifs = self.parse_ifconfig_list(self.eth_pid)
        assert netifs[str(self.eth_pid)]["link"] == "up"

        self.shell.ifconfig_down(self.eth_pid)

        netifs = self.parse_ifconfig_list(self.eth_pid)
        assert netifs[str(self.eth_pid)]["link"] == "down"

        with self.assertRaises(RuntimeError) as ctx:
            # we left NETOPT_LINK deliberately unimplemented
            # for the IEEE 802.15.4 device in netdev_mock.c
            self.shell.ifconfig_up(self.ieee802154_pid)

        assert "error: unable to set link up" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            # we left NETOPT_LINK deliberately unimplemented
            # for the IEEE 802.15.4 device in netdev_mock.c
            self.shell.ifconfig_down(self.ieee802154_pid)

        assert "error: unable to set link down" in str(ctx.exception)

    def _test_ifconfig_add(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_add("foobar", "fe80::1")
        assert "error: invalid interface given" in str(ctx.exception)

        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert "ipv6_addrs" not in netif
        assert "ipv6_groups" not in netif
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_add(self.eth_pid, "fe80::::1")
        assert "error: unable to parse IPv6 address." in str(ctx.exception)

        res = self.shell.ifconfig_add(self.eth_pid, "fe80::1")
        assert f"success: added fe80::1/64 to interface {self.eth_pid}" in res
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert "fe80::1" in [a["addr"] for a in netif["ipv6_addrs"]]
        for addr in netif["ipv6_addrs"]:
            if addr["addr"] == "fe80::1":
                assert "anycast" not in addr
        assert "ipv6_groups" in netif

        res = self.shell.ifconfig_add(self.eth_pid, "fe80::c001/32", anycast=True)
        assert f"success: added fe80::c001/32 to interface {self.eth_pid}" in res
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert "fe80::c001" in [a["addr"] for a in netif["ipv6_addrs"]]
        for addr in netif["ipv6_addrs"]:
            if addr["addr"] == "fe80::c001":
                assert addr["anycast"]
        assert "ipv6_groups" in netif

    def test_ifconfig_add(self):
        self._test_ifconfig_add()

    def test_ifconfig_del(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_del("foobar", "fe80::1")
        assert "error: invalid interface given" in str(ctx.exception)

        self._test_ifconfig_add()

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_del(self.eth_pid, "fe80::::1")
        assert "error: unable to parse IPv6 address." in str(ctx.exception)

        # check preconditions
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert "fe80::1" in [a["addr"] for a in netif["ipv6_addrs"]]
        assert "fe80::c001" in [a["addr"] for a in netif["ipv6_addrs"]]
        assert "ipv6_groups" in netif

        self.shell.ifconfig_del(self.eth_pid, "fe80::1")
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert "fe80::1" not in [a["addr"] for a in netif["ipv6_addrs"]]
        assert "ipv6_groups" in netif

        self.shell.ifconfig_del(self.eth_pid, "fe80::c001")
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert "ipv6_addrs" not in netif
        assert "ipv6_groups" not in netif

    def _test_ifconfig_flag_enable(self, netif, flag, enable=True):
        key = "enable" if enable else "disable"
        enable_exp = self.flag_exp[netif][flag][key]
        if isinstance(enable_exp, Exception):
            with self.assertRaises(type(enable_exp)) as ctx:
                self.shell.ifconfig_flag(netif, flag, enable=enable)
            assert str(enable_exp) in str(ctx.exception)
        else:
            res = self.shell.ifconfig_flag(netif, flag, enable=enable)
            assert enable_exp in res
            if f"{key}_check" in self.flag_exp[netif][flag]:
                netif_obj = self.parse_ifconfig_list(netif)[str(netif)]
                assert self.flag_exp[netif][flag][f"{key}_check"](
                    netif_obj
                ), f"{flag} not {key}d in {netif_obj}"

    def _test_ifconfig_flag(self, netif):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_flag("foobar", "6lo")
        assert "error: invalid interface given" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_flag(netif, "foobar", enable=False)
        res = self.consume_remaining_help_text(str(ctx.exception))
        assert "usage: ifconfig <if_id>" in res

        flags = self.parse_flags_from_help_text(res, netif)

        for flag in flags:
            self._test_ifconfig_flag_enable(netif, flag, enable=True)
            self._test_ifconfig_flag_enable(netif, flag, enable=False)

    def test_ifconfig_flag_eth(self):
        self._test_ifconfig_flag(self.eth_pid)

    def test_ifconfig_flag_radio(self):
        self._test_ifconfig_flag(self.ieee802154_pid)

    def _test_ifconfig_l2filter_add(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_l2filter_add("foobar", "02:00:00:22:34:60")
        assert "error: invalid interface given" in str(ctx.exception)

        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert netif["whitelist"] == []

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_l2filter_add(self.eth_pid, "foobar")
        assert "given address is invalid" in str(ctx.exception)

        res = self.shell.ifconfig_l2filter_add(self.eth_pid, "02:00:00:22:34:60")
        assert "successfully added address to filter" in res
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert netif["whitelist"] == ["02:00:00:22:34:60"]

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_l2filter_add(
                self.ieee802154_pid, "02:00:00:ff:fe:22:34:60"
            )
        assert "unable to add link layer address to filter" in str(ctx.exception)

    def test_ifconfig_l2filter_add(self):
        self._test_ifconfig_l2filter_add()

    def test_ifconfig_l2filter_del(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_l2filter_del("foobar", "02:00:00:22:34:60")
        assert "error: invalid interface given" in str(ctx.exception)

        self._test_ifconfig_l2filter_add()

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_l2filter_del(self.eth_pid, "foobar")
        assert "given address is invalid" in str(ctx.exception)

        # check precondition
        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert netif["whitelist"] == ["02:00:00:22:34:60"]

        res = self.shell.ifconfig_l2filter_del(self.eth_pid, "02:00:00:22:34:60")
        assert "successfully removed address to filter" in res

        netif = self.parse_ifconfig_list(self.eth_pid)[str(self.eth_pid)]
        assert netif["whitelist"] == []

    def _test_ifconfig_stats(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_stats("foobar", "l2")
        assert "error: invalid interface given" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_stats(self.eth_pid, "foobar")
        assert "Module foobar doesn't exist or does not provide statistics" in (
            str(ctx.exception)
        )

        res = self.shell.ifconfig_stats(self.eth_pid, "l2")
        parser = riotctrl_shell.netif.IfconfigStatsParser()
        stats = parser.parse(res)
        assert stats["Layer 2"]["tx"]["packets"] == 0
        assert stats["Layer 2"]["tx"]["bytes"] == 0

        res = self.shell.ifconfig_stats(self.eth_pid, "ipv6")
        stats = parser.parse(res)
        assert stats["IPv6"]["tx"]["packets"] == 0
        assert stats["IPv6"]["tx"]["bytes"] == 0

        res = self.shell.netif_txtsnd(
            self.eth_pid, "ab:cd:ef:8c:ae:12", '"Hello World!"'
        )
        od_parser = riotctrl_shell.od.OdParser()
        assert b"Hello World!" in od_parser.parse(res)

        res = self.shell.ifconfig_stats(self.eth_pid, "l2")
        stats = parser.parse(res)
        assert stats["Layer 2"]["tx"]["packets"] > 0
        assert stats["Layer 2"]["tx"]["bytes"] > 0

        res = self.shell.ifconfig_stats(self.eth_pid, "ipv6")
        stats = parser.parse(res)
        assert stats["IPv6"]["tx"]["packets"] == 0
        assert stats["IPv6"]["tx"]["bytes"] == 0

    def test_ifconfig_stats(self):
        self._test_ifconfig_stats()

    def test_ifconfig_stats_reset(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_stats_reset("foobar", "l2")
        assert "error: invalid interface given" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.ifconfig_stats_reset(self.eth_pid, "foobar")
        assert "Module foobar doesn't exist or does not provide statistics" in (
            str(ctx.exception)
        )

        self._test_ifconfig_stats()

        # check preconditions
        res = self.shell.ifconfig_stats(self.eth_pid, "l2")
        parser = riotctrl_shell.netif.IfconfigStatsParser()
        stats = parser.parse(res)
        assert stats["Layer 2"]["tx"]["packets"] > 0
        assert stats["Layer 2"]["tx"]["bytes"] > 0

        res = self.shell.ifconfig_stats(self.eth_pid, "ipv6")
        stats = parser.parse(res)
        assert stats["IPv6"]["tx"]["packets"] == 0
        assert stats["IPv6"]["tx"]["bytes"] == 0

        self.shell.ifconfig_stats_reset(self.eth_pid, "l2")

        res = self.shell.ifconfig_stats(self.eth_pid, "l2")
        stats = parser.parse(res)
        assert stats["Layer 2"]["tx"]["packets"] == 0
        assert stats["Layer 2"]["tx"]["bytes"] == 0

        res = self.shell.ifconfig_stats(self.eth_pid, "ipv6")
        stats = parser.parse(res)
        assert stats["IPv6"]["tx"]["packets"] == 0
        assert stats["IPv6"]["tx"]["bytes"] == 0

        self.shell.ifconfig_stats_reset(self.eth_pid, "ipv6")

        res = self.shell.ifconfig_stats(self.eth_pid, "l2")
        stats = parser.parse(res)
        assert stats["Layer 2"]["tx"]["packets"] == 0
        assert stats["Layer 2"]["tx"]["bytes"] == 0

        res = self.shell.ifconfig_stats(self.eth_pid, "ipv6")
        stats = parser.parse(res)
        assert stats["IPv6"]["tx"]["packets"] == 0
        assert stats["IPv6"]["tx"]["bytes"] == 0

    def test_netif_txtsnd(self):
        with self.assertRaises(RuntimeError) as ctx:
            res = self.shell.netif_txtsnd("xyz", "foobar", "abcdef")
        assert "error: invalid interface given" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            res = self.shell.netif_txtsnd(self.eth_pid, "foobar", "abcdef")
        assert "error: invalid address given" in str(ctx.exception)

        res = self.shell.netif_txtsnd(self.eth_pid, "ab:cd:ef:8c:ae:12", "abcdef")
        od_parser = riotctrl_shell.od.OdParser()
        assert f"Sending on interface {self.eth_pid}" in res
        assert b"abcdef" in od_parser.parse(res)

        res = self.shell.netif_txtsnd(self.eth_pid, "bcast", '"testing broadcast"')
        assert b"testing broadcast" in od_parser.parse(res)
        # ff:ff:ff:ff:ff:ff is the Ethernet broadcast address
        assert f"Sending on interface {self.eth_pid}" in res
        assert (b"\xff" * 6) in od_parser.parse(res)

        res = self.shell.netif_txtsnd(self.ieee802154_pid, "bcast", '"testing WPAN broadcast"')
        assert f"Sending on interface {self.ieee802154_pid}" in res
        assert b"testing WPAN broadcast" in od_parser.parse(res)
        # ff:ff is the IEEE 802.15.4 broadcast address used by RIOT
        assert (b"\xff" * 2) in od_parser.parse(res)


if __name__ == "__main__":
    unittest.main()
