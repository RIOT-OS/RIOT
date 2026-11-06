#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.cord_ep with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import unittest

import riotctrl.shell

import riotctrl_shell.cord_ep
import riotctrl_shell.od

from riotctrl_shell.sys import Reboot
from testrunner.spawn import flush_child

import riotctrl_shell_tests.testcase_base


class MainInteraction(riotctrl.shell.ShellInteraction):
    PREPARE_CHUNKSIZE = 16

    @riotctrl.shell.ShellInteraction.check_term
    def preload_response(self, byts, timeout=-1, async_=False):
        for i in range(0, len(byts), self.PREPARE_CHUNKSIZE):
            res = self.cmd(
                f"preload_response {byts[i:i+self.PREPARE_CHUNKSIZE].hex()}",
                timeout=timeout,
                async_=async_,
            )
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res

    @riotctrl.shell.ShellInteraction.check_term
    def response_remote(self, uri, timeout=-1, async_=False):
        res = self.cmd(
            f"response_remote {uri}",
            timeout=timeout,
            async_=async_,
        )
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res


class TestSelf(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = MainInteraction(cls.ctrl)

    def test_prepare_response(self):
        od_parser = riotctrl_shell.od.OdParser()

        res = self.shell.preload_response(bytes([0x01, 0x02, 0x03, 0x04]))
        preloaded_response = od_parser.parse(res)
        assert bytes([0x01, 0x02, 0x03, 0x04]) == preloaded_response

        res = self.shell.preload_response(bytes([0x05, 0x06, 0x07]))
        preloaded_response = od_parser.parse(res)
        assert bytes([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07]) == preloaded_response

        res = self.shell.preload_response(bytes(i for i in range(8, 65)))
        preloaded_response = od_parser.parse(res)
        assert (
            bytes([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07])
            + bytes(i for i in range(8, 65))
        ) == preloaded_response

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.preload_response(bytes([0x41]))
        assert (
            'error: "41" does not fit into remaining buffer (buffer size: 64).'
            in str(ctx.exception)
        )

    def test_response_remote(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.response_remote("this_is_no_uri")
        assert "error: Only coap schemes are supported" in str(ctx.exception)
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.response_remote("coap://192.0.2.42/")
        assert "error: Only IPv6 addresses are supported" in str(ctx.exception)
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.response_remote("coap://[2001:::db8:1]/")
        assert "error: IPv6 address malformed" in str(ctx.exception)
        res = self.shell.response_remote("coap://[2001:db8::1]/")
        assert "success: set response remote to [2001:db8::1%5]:5683" in res
        res = self.shell.response_remote("coap://[2001:db8::1]:52213/")
        assert "success: set response remote to [2001:db8::1%5]:52213" in res


class CordEpWithMain(MainInteraction, riotctrl_shell.cord_ep.CordEp, Reboot):
    pass


class TestCordEp(riotctrl_shell_tests.testcase_base.TestBase):
    # see also https://www.iana.org/assignments/core-parameters
    # for CoAP response values
    DISCOVER_RESPONSE = bytes(
        [
            # CoAP version 1 (0b01), ACK (0b10), Token length 2 (0x2),
            0b01100000 | 0x2,
            # 2.05 Content
            (0x2 << 5) | 5,
            0, 0,  # message ID (will be filled by sock_udp_sendv_aux in main.c)
            0, 0,  # Token (will be filled by sock_udp_sendv_aux in main.c)
            0b11000001,  # Opt Delta 12 (0b1100), Opt Length: 1 (0b0001)
            40,  # Content-Format (type 12): application/link-format (40)
            0xFF,  # End of options marker
        ]
    ) + b'</resourcedirectory/>;ct="40";rt="core.rd"'

    REGISTER_RESPONSE = bytes(
        [
            # CoAP version 1 (0b01), ACK (0b10), Token length 2 (0x2),
            0b01100000 | 0x2,
            # 2.01 Created
            (0x2 << 5) | 1,
            0, 0,  # message ID (will be filled by sock_udp_sendv_aux in main.c)
            0, 0,  # Token (will be filled by sock_udp_sendv_aux in main.c)
            0b10000011,  # Opt Delta 8 (0b1000), Opt Length: 3 (0b0011)
        ]
    ) + b"reg" + bytes(  # Loctation-Path (type 8): "reg"
        [
            0b00000001,  # Opt Delta 0 (0b0000), Opt Length: 1 (0b0001)
        ]
    ) + b"1" + bytes(  # Loctation-Path (type 8): "1"
        [
            0b00000000,  # Opt Delta 0 (0b0000), Opt Length: 1 (0b0000)
            # => Location-Path: /reg/1
        ]
    )

    REMOVE_RESPONSE = bytes(
        [
            # CoAP version 1 (0b01), ACK (0b10), Token length 2 (0x2),
            0b01100000 | 0x2,
            # 2.02 Deleted
            (0x2 << 5) | 2,
            0, 0,  # message ID (will be filled by sock_udp_sendv_aux in main.c)
            0, 0,  # Token (will be filled by sock_udp_sendv_aux in main.c)
        ]
    )

    UPDATE_RESPONSE = bytes(
        [
            # CoAP version 1 (0b01), ACK (0b10), Token length 2 (0x2),
            0b01100000 | 0x2,
            # 2.04 Changed
            (0x2 << 5) | 4,
            0, 0,  # message ID (will be filled by sock_udp_sendv_aux in main.c)
            0, 0,  # Token (will be filled by sock_udp_sendv_aux in main.c)
        ]
    )

    REMOTE_IP = "fe80::2"
    REMOTE_URI = f"coap://[{REMOTE_IP}]"
    REGIF = "/resourcedirectory/"
    COAP_PORT = 5683
    NETIF_ID = 5

    @classmethod
    def init_shell_interaction(cls):
        cls.shell = CordEpWithMain(cls.ctrl)

    def tearDown(self):
        res = self.shell.reboot()
        assert "Starting riotctrl_shell.cord_ep test" in res
        flush_child(self.ctrl.term)

    def preload_and_check_response(self, response):
        res = self.shell.response_remote(self.REMOTE_URI)
        assert (
            "success: set response remote to "
            f"[{self.REMOTE_IP}%{self.NETIF_ID}]:{self.COAP_PORT}" in res
        )
        res = self.shell.preload_response(response)
        od_parser = riotctrl_shell.od.OdParser()
        preloaded_response = od_parser.parse(res)
        assert preloaded_response == response

    def check_info_result(self, res):
        info_parser = riotctrl_shell.cord_ep.CordEpRegistrationInfoParser()
        info = info_parser.parse(res)

        assert info["rdaddr"] == f"{self.REMOTE_URI}:{self.COAP_PORT}"
        assert info["epname"].startswith("RIOT-")
        assert info["ltime"] > 0
        assert info["regif"] == self.REGIF
        assert info["location"] == "/reg/1/"
        return info

    def test_cord_ep_cmd_not_known(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.cord_ep_cmd("foobar45742", args=(1, 2, 3))
        assert "usage: cord_ep " in str(ctx.exception)

    def test_cord_ep_info_unregistered(self):
        res = self.shell.cord_ep_info()
        parser = riotctrl_shell.cord_ep.CordEpRegistrationInfoParser()
        info = parser.parse(res)
        # nt registered to any resource directory, so info is empty
        assert len(info) == 0

    def test_cord_ep_discover(self):
        self.preload_and_check_response(self.DISCOVER_RESPONSE)

        res = self.shell.cord_ep_discover(f"[{self.REMOTE_IP}]:{self.COAP_PORT}")
        assert f"the registration interface is '{self.REGIF}'" in res

    def test_cord_ep_register(self):
        self.preload_and_check_response(self.REGISTER_RESPONSE)

        res = self.shell.cord_ep_register(f"{self.REMOTE_URI}{self.REGIF}")
        self.check_info_result(res)

        # confirm that info returns the same
        res = self.shell.cord_ep_info()
        self.check_info_result(res)

    def test_cord_ep_remove(self):
        self.test_cord_ep_register()

        self.preload_and_check_response(self.REMOVE_RESPONSE)

        res = self.shell.cord_ep_info()
        info = self.check_info_result(res)
        assert len(info) > 0

        res = self.shell.cord_ep_remove()
        assert "node successfully removed from RD" in res

        res = self.shell.cord_ep_info()
        info_parser = riotctrl_shell.cord_ep.CordEpRegistrationInfoParser()
        info = info_parser.parse(res)
        assert len(info) == 0

    def test_cord_ep_update(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.cord_ep_update()
        assert "error: not associated with any RD" in str(ctx.exception)

        self.test_cord_ep_register()

        self.preload_and_check_response(self.UPDATE_RESPONSE)
        res = self.shell.cord_ep_update()
        assert "RD update successful" in res

        res = self.shell.cord_ep_info()
        self.check_info_result(res)


if __name__ == "__main__":
    unittest.main()
