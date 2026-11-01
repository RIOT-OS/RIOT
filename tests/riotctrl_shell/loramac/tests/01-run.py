#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.lorawan with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
"""

import logging
import os
import re
import sys
import unittest

import pexpect
from riotctrl.ctrl import RIOTCtrl

import riotctrl_shell.loramac

from testrunner.spawn import flush_child


class TestBase(unittest.TestCase):
    DEBUG = bool(int(os.environ.get("DEBUG_TEST", "0")))

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        flush_child(cls.ctrl.term)
        cls.shell = riotctrl_shell.loramac.Loramac(cls.ctrl)
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()

    def setUp(self):
        flush_child(self.ctrl.term)


class TestLoramac(TestBase):
    EXP_GET_OUTPUT = {
        "deveui": {
            "param": "DEVEUI",
            "convert_from": bytes.fromhex,
            "convert_to": lambda b: b.hex(),
            "init_value": b"\x00" * 8,
        },
        "appeui": {
            "param": "APPEUI",
            "convert_from": bytes.fromhex,
            "convert_to": lambda b: b.hex(),
            "init_value": b"\x00" * 8,
        },
        "appkey": {
            "param": "APPKEY",
            "convert_from": bytes.fromhex,
            "convert_to": lambda b: b.hex(),
            "init_value": b"\x00" * 16,
        },
        "appskey": {
            "param": "APPSKEY",
            "convert_from": bytes.fromhex,
            "convert_to": lambda b: b.hex(),
            "init_value": b"\x00" * 16,
        },
        "nwkskey": {
            "param": "NWKSKEY",
            "convert_from": bytes.fromhex,
            "convert_to": lambda b: b.hex(),
            "init_value": b"\x00" * 16,
        },
        "devaddr": {
            "param": "DEVADDR",
            "convert_from": bytes.fromhex,
            "convert_to": lambda b: b.hex(),
            "init_value": b"\x00" * 4,
        },
        "class": {
            "param": "Device class",
            "convert_from": str,
            "convert_to": str,
            "init_value": "A",
        },
        "dr": {
            "param": "DATARATE",
            "convert_from": int,
            "convert_to": str,
            "init_value": 0,
        },
        "adr": {
            "param": "ADR",
            "convert_from": lambda s: bool(["off", "on"].index(s)),
            "convert_to": lambda b: "on" if b else "off",
            "init_value": False,
        },
        "public": {
            "param": "Public network",
            "convert_from": lambda s: bool(["off", "on"].index(s)),
            "convert_to": lambda b: "on" if b else "off",
            "init_value": True,
        },
        "netid": {
            "param": "NetID",
            "convert_from": int,
            "convert_to": str,
            "init_value": 0,
        },
        "tx_power": {
            "param": "TX power index",
            "convert_from": int,
            "convert_to": str,
            "init_value": 0,
        },
        "rx2_freq": {
            "param": "RX2 freq",
            "convert_from": int,
            "convert_to": str,
            "init_value": 869525000,
        },
        "rx2_dr": {
            "param": "RX2 dr",
            "convert_from": int,
            "convert_to": str,
            "init_value": 0,
        },
        "ul_cnt": {
            "param": "Uplink Counter",
            "convert_from": int,
            "convert_to": str,
            "init_value": 0,
        },
        "ch_mask": {
            "param": "Channels mask",
            "convert_from": lambda x: int(x, base=16),
            "convert_to": lambda d: f"{d:04x}",
            "init_value": 7,
        },
    }

    def test_loramac_help(self):
        res = self.shell.loramac_help()
        assert "Usage: loramac " in res
        parser = riotctrl_shell.loramac.LoramacHelpParser()
        # ensure that EEPROM is mocked
        assert parser.has_eeprom(res)
        assert not parser.has_eeprom("just some string")

    def test_loramac_get(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.loramac_get("")
        help_text = str(ctx.exception)
        assert "Usage: loramac get " in help_text
        assert re.search("Usage: loramac get <(([^|>]+)[|>])+", help_text)
        keys = []
        for match in re.findall("([^<|>]+)[|>]", help_text):
            key = match
            keys.append(key)
            res = self.shell.loramac_get(key)
            exp_output = self.EXP_GET_OUTPUT[key]
            assert f"{exp_output['param']}: " in res
            match = re.search(rf"{exp_output['param']}: (\S+)\s", res)
            assert match
            value_str = match[1]
            value = exp_output["convert_from"](value_str)
            assert value == exp_output["init_value"]
        assert set(keys) == set(self.EXP_GET_OUTPUT.keys())

    def test_loramac_set(self):
        set_test_vector = {
            "deveui": {"value": "12345678a1a2a3a4"},
            "appeui": {"value": "abcdef0102030405"},
            "appkey": {"value": "000102030405060708090a0b0c0d0e0f"},
            "appskey": {"value": "101112131415161718191a1b1c1d1e1f"},
            "nwkskey": {"value": "202122232425262728292a2b2c2d2e2f"},
            "devaddr": {"value": "abcdef00"},
            "class": {"value": "B"},
            "dr": {"value": "15"},
            "adr": {"value": "on"},
            "public": {"value": "off"},
            "netid": {"value": "1337"},
            "tx_power": {"value": "5"},
            "rx2_freq": {"value": "53342345"},
            "rx2_dr": {"value": "9"},
            "ul_cnt": {"value": "64345648"},
            "ch_mask": {"value": "003a"},
        }
        assert set_test_vector.keys() == self.EXP_GET_OUTPUT.keys()
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.loramac_set("", "")
        help_text = str(ctx.exception)
        assert "Usage: loramac set " in help_text
        # Closing "> " of first parameter doc is swallowed by shell parser as prompt
        assert re.search(r"Usage: loramac set <(([^|>]+)\|?)+", help_text)
        # Go to actual prompt
        flush_child(self.ctrl.term)
        keys = []
        for match in re.findall(r"([^<|>]+)\|?", help_text):
            if "loramac set" in match:
                continue
            key = match
            keys.append(key)
            self.shell.loramac_set(key, set_test_vector[key]["value"])

            # try to get value
            exp_output = self.EXP_GET_OUTPUT[key]
            set_value = exp_output["convert_from"](set_test_vector[key]["value"])
            if key in ["dr", "netid", "rx2_dr"]:
                # these values will not be set in the mock
                set_value = exp_output["init_value"]
            res = self.shell.loramac_get(key)
            assert f"{exp_output['param']}: " in res
            match = re.search(rf"{exp_output['param']}: (\S+)\s", res)
            assert match
            value_str = match[1]
            value = exp_output["convert_from"](value_str)
            assert value == set_value
            self.shell.loramac_set(
                key, exp_output["convert_to"](exp_output["init_value"])
            )
        assert set(keys) == set(set_test_vector.keys())

    def test_loramac_join_help_text(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.loramac_join("")
        help_text = str(ctx.exception)
        assert re.search("Usage: loramac join <(([^|>]+)[|>])+", help_text)
        assert ["otaa", "abp"] == re.findall("([^<|>]+)[|>]", help_text)

    def hard_reset_node(self):
        self.tearDownClass()
        self.setUpClass()

    def test_loramac_join_abp(self):
        try:
            res = self.shell.loramac_join("abp", timeout=1)
            # ABP joins even if the device is a mock device
            assert "Join procedure succeeded!" in res
        finally:
            # reset from trying to join
            self.hard_reset_node()

    def test_loramac_join_otaa(self):
        try:
            with self.assertRaises(pexpect.TIMEOUT):
                # we do not actually join, so we expect a timeout
                self.shell.loramac_join("otaa", timeout=1)
        finally:
            # reset from trying to join
            self.hard_reset_node()

    def test_loramac_tx(self):
        # not joined so expect error
        with self.assertRaises(RuntimeError):
            self.shell.loramac_tx("testing")
        try:
            res = self.shell.loramac_join("abp", timeout=1)
            # ABP joins even if the device is a mock device
            assert "Join procedure succeeded!" in res
            with self.assertRaises(pexpect.TIMEOUT):
                # still a dummy device, so sending will timeout
                res = self.shell.loramac_tx("testing", cnf=True, port=43, timeout=1)
        finally:
            # reset from trying to join
            self.hard_reset_node()

    def test_loramac_eeprom_save(self):
        res = self.shell.loramac_eeprom_save()
        assert res

    def test_loramac_eeprom_erase(self):
        res = self.shell.loramac_eeprom_erase()
        assert res


class TestLoramacUpLinkCounterParser(TestBase):
    def test_link_counter_parser(self):
        test_value = "4319"
        self.shell.loramac_set("ul_cnt", test_value)
        res = self.shell.loramac_get("ul_cnt")
        parser = riotctrl_shell.loramac.LoramacUpLinkCounterParser()
        assert test_value == parser.uplink_count(res)

    def test_link_counter_parser_wrong_output(self):
        res = self.shell.loramac_get("deveui")
        parser = riotctrl_shell.loramac.LoramacUpLinkCounterParser()
        with self.assertRaises(RuntimeError):
            parser.uplink_count(res)


if __name__ == "__main__":
    unittest.main()
