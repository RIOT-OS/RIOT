#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.udp with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import time
import unittest

import riotctrl.shell

import riotctrl_shell.od
import riotctrl_shell.udp
import riotctrl_shell.sys

from testrunner.spawn import flush_child

import riotctrl_shell_tests.testcase_base


class MainInteraction(riotctrl.shell.ShellInteraction):
    PREPARE_CHUNKSIZE = 16

    @riotctrl.shell.ShellInteraction.check_term
    def preload_payload(self, byts, timeout=-1, async_=False):
        for i in range(0, len(byts), self.PREPARE_CHUNKSIZE):
            res = self.cmd(
                f"preload_payload {byts[i:i+self.PREPARE_CHUNKSIZE].hex()}",
                timeout=timeout,
                async_=async_,
            )
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res

    @riotctrl.shell.ShellInteraction.check_term
    def reset_payload(self, timeout=-1, async_=False):
        res = self.cmd(
            "reset_payload",
            timeout=timeout,
            async_=async_,
        )
        return res

    @riotctrl.shell.ShellInteraction.check_term
    def payload_remote(self, remote, timeout=-1, async_=False):
        res = self.cmd(
            f"payload_remote {remote}",
            timeout=timeout,
            async_=async_,
        )
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res

    @riotctrl.shell.ShellInteraction.check_term
    def trigger_receive(self, count=1, delay_us=1000000, timeout=-1, async_=False):
        res = self.cmd(
            f"trigger_receive {count} {delay_us}", timeout=timeout, async_=async_
        )
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res


class TestSelf(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = MainInteraction(cls.ctrl)

    def test_prepare_payload(self):
        od_parser = riotctrl_shell.od.OdParser()

        res = self.shell.preload_payload(bytes([0x01, 0x02, 0x03, 0x04]))
        preloaded_payload = od_parser.parse(res)
        assert bytes([0x01, 0x02, 0x03, 0x04]) == preloaded_payload

        res = self.shell.preload_payload(bytes([0x05, 0x06, 0x07]))
        preloaded_payload = od_parser.parse(res)
        assert bytes([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07]) == preloaded_payload

        res = self.shell.preload_payload(bytes(i for i in range(8, 65)))
        preloaded_payload = od_parser.parse(res)
        assert (
            bytes([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07])
            + bytes(i for i in range(8, 65))
        ) == preloaded_payload

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.preload_payload(bytes([0x41]))
        assert (
            'error: "41" does not fit into remaining buffer (buffer size: 64).'
            in str(ctx.exception)
        )

    def test_payload_remote(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.payload_remote("this_is_no_legal_str")
        assert "error: unable to parse address-port-pair" in str(ctx.exception)
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.payload_remote("[192.0.2.42]:5683")
        assert "error: unable to parse address-port-pair" in str(ctx.exception)
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.payload_remote("[2001:::db8:1]:5683")
        assert "error: unable to parse address-port-pair" in str(ctx.exception)
        res = self.shell.payload_remote("[2001:db8::1]:5683")
        assert "success: set payload remote to [2001:db8::1%5]:5683" in res
        res = self.shell.payload_remote("[2001:db8::1]:52213")
        assert "success: set payload remote to [2001:db8::1%5]:52213" in res

    def test_trigger_receive(self):
        # we just want to cover the error case, the functionality is test on-the-go
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.trigger_receive("not_an_integer")
        assert "usage:" in str(ctx.exception)


class TestUDP(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = riotctrl_shell.udp.UDPClient(cls.ctrl)

    def setUp(self):
        super().setUp()
        self.od_parser = riotctrl_shell.od.OdParser()
        self.udp_parser = riotctrl_shell.udp.UDPClientSendParser()

    def test_udp_client_errors(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.udp_client_send("foobar", 5683, "Hello World!")
        assert "Error: unable to parse destination address" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.udp_client_send("2001:db8::1", "foobar", "Hello World!")
        assert "Error: unable to parse destination address" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.udp_client_send("2001:db8::1", 0, "Hello World!")
        assert "Error: no port or illegal port value provided" in str(ctx.exception)

    def test_udp_client_send_text(self):
        res = self.shell.udp_client_send("2001:db8::1", 5683, "Hello World!")
        stats = self.udp_parser.parse(res)
        byts = self.od_parser.parse(res)
        assert byts == b"Hello World!"
        assert len(stats) == 1
        assert stats[0]["payload_len"] == len("Hello World!")
        assert stats[0]["dst"] == "2001:db8::1"
        assert stats[0]["dport"] == 5683

    def test_udp_client_send_text_like_hex(self):
        res = self.shell.udp_client_send("2001:db8::1", 5683, "abcd001122")
        stats = self.udp_parser.parse(res)
        byts = self.od_parser.parse(res)
        assert byts == b"abcd001122"
        assert len(stats) == 1
        assert stats[0]["payload_len"] == len("abcd001122")
        assert stats[0]["dst"] == "2001:db8::1"
        assert stats[0]["dport"] == 5683

    def test_udp_client_send_bytes(self):
        res = self.shell.udp_client_send("2001:db8::1", 5683, b"\x00\x11\x22\x33\x44")
        stats = self.udp_parser.parse(res)
        byts = self.od_parser.parse(res)
        assert byts == b"\x00\x11\x22\x33\x44"
        assert len(stats) == 1
        assert stats[0]["payload_len"] == len(b"\x00\x11\x22\x33\x44")
        assert stats[0]["dst"] == "2001:db8::1"
        assert stats[0]["dport"] == 5683

    def test_udp_client_only_count(self):
        count = 3
        start = time.time()
        res = self.shell.udp_client_send("2001:db8::1", 5683, "abcd", count)
        stop = time.time()
        stats = self.udp_parser.parse(res)
        byts = self.od_parser.parse(res, multiple=True)
        assert len(byts) == count
        assert len(stats) == count
        for i in range(count):
            assert byts[i] == b"abcd"
            assert stats[i]["payload_len"] == len(b"abcd")
            assert stats[i]["dst"] == "2001:db8::1"
            assert stats[i]["dport"] == 5683
        assert (stop - start) >= count - 1

    def test_udp_client_count_and_time(self):
        count = 5
        timeout = 100_000
        start = time.time()
        res = self.shell.udp_client_send("2001:db8::1", 5683, "abcd", count, timeout)
        stop = time.time()
        stats = self.udp_parser.parse(res)
        byts = self.od_parser.parse(res, multiple=True)
        assert len(byts) == count
        assert len(stats) == count
        for i in range(count):
            assert byts[i] == b"abcd"
            assert stats[i]["payload_len"] == len(b"abcd")
            assert stats[i]["dst"] == "2001:db8::1"
            assert stats[i]["dport"] == 5683
        assert (stop - start) >= (count - 1) * timeout / 1_000_000


class UDPServerTestShell(
    riotctrl_shell.sys.Reboot,
    riotctrl_shell.udp.UDPServer,
    MainInteraction,
):
    pass


class TestUDPServer(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = UDPServerTestShell(cls.ctrl)

    def setUp(self):
        self.shell.reboot()
        flush_child(self.ctrl.term)
        super().setUp()

    def test_udp_server_start_invalid_port(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.udp_server_start("foobar")
        assert "Error: invalid port specified" in str(ctx.exception)

        with self.assertRaises(RuntimeError) as ctx:
            self.shell.udp_server_start("foobar")
        assert "Error: invalid port specified" in str(ctx.exception)

    def _test_udp_server_start(self):
        res = self.shell.udp_server_start(5683)
        assert "Success: started UDP server on port 5683" in res

    def test_udp_server_start(self):
        self._test_udp_server_start()

    def test_udp_server_start_double(self):
        self._test_udp_server_start()
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.udp_server_start(5688)
        assert "Error: server already running on port 5683" in str(ctx.exception)

    def test_udp_server_stop(self):
        res = self.shell.udp_server_stop()
        # does not throw exception which is intentional for idempotency
        assert "Error: server was not running" in res

        self._test_udp_server_start()

        res = self.shell.udp_server_stop()
        assert "Success: stopped UDP server" in res

    def test_udp_server_check_output(self):
        self._test_udp_server_start()

        self.shell.preload_payload(b"abcdef")
        self.shell.payload_remote("[2001:db8::1]:12345")

        self.shell.trigger_receive()
        lost_perc = self.shell.udp_server_check_output()
        assert lost_perc == 0

        self.shell.reset_payload()
        self.shell.preload_payload(b"0123456789")
        self.shell.payload_remote("[2001:db8::2]:12349")

        self.shell.trigger_receive(count=2)
        lost_perc = self.shell.udp_server_check_output(2)
        assert lost_perc == 0

        self.shell.reset_payload()
        self.shell.preload_payload(b"xyzab")
        self.shell.payload_remote("[2001:db8::3]:12298")

        self.shell.trigger_receive(count=10, delay_us=100_000)
        lost_perc = self.shell.udp_server_check_output(10, 100_000)
        assert lost_perc == 0

        self.shell.reset_payload()
        self.shell.preload_payload(b"hello")
        self.shell.payload_remote("[2001:db8::4]:12336")

        self.shell.trigger_receive(count=1, delay_us=1)
        lost_perc = self.shell.udp_server_check_output(1, 0)
        assert lost_perc == 0

        self.shell.reset_payload()
        self.shell.preload_payload(b"let's emulate loss")
        self.shell.payload_remote("[2001:db8::5]:12362")

        self.shell.trigger_receive(count=7, delay_us=100000)
        lost_perc = self.shell.udp_server_check_output(10, 100000)
        assert lost_perc == (10 - 7) * 10


if __name__ == "__main__":
    unittest.main()
