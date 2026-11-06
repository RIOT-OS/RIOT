#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.od with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import unittest

import riotctrl.shell

import riotctrl_shell.od

import riotctrl_shell_tests.testcase_base


class Od(riotctrl.shell.ShellInteraction):
    FILL_CHUNKSIZE = 16

    @riotctrl.shell.ShellInteraction.check_term
    def app_cmd(self, cmd, timeout=-1, async_=False):
        res = self.cmd(cmd, timeout=timeout, async_=async_)
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res

    def od(self, offset=None, width=None, timeout=-1, async_=False):
        cmd = "od"
        if offset is not None:
            cmd += f" {offset:d}"
        elif width is not None:
            cmd += " 0"
        if width is not None:
            cmd += f" {width:d}"
        return self.app_cmd(cmd, timeout=timeout, async_=async_)

    def fill_buf(self, byts, timeout=-1, async_=False):
        res = ""
        for i in range(0, len(byts), self.FILL_CHUNKSIZE):
            res += self.app_cmd(
                f"fillbuf {byts[i:i+self.FILL_CHUNKSIZE].hex()}",
                timeout=timeout,
                async_=async_,
            )
        return res

    def reset_buf(self, timeout=-1, async_=False):
        return self.app_cmd("resetbuf", timeout=timeout, async_=async_)


class TestBase(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = Od(cls.ctrl)

    def setUp(self):
        super().setUp()
        self.parser = riotctrl_shell.od.OdParser()

    def tearDown(self):
        self.shell.reset_buf()


class TestSelf(TestBase):
    def test_od_usage(self):
        with self.assertRaises(RuntimeError):
            self.shell.app_cmd("od help")

        with self.assertRaises(RuntimeError):
            self.shell.app_cmd("od 0 foobar")

    def test_fill_buf_overflow(self):
        # fill buffer up to buffer size of `_od_buf` in main.c
        # (see also `OD_BUF_SIZE`)
        self.shell.fill_buf(bytes(i for i in range(0, 64)))
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.fill_buf(bytes([0x41]))
        assert (
            'error: "41" does not fit into remaining buffer (buffer size: 64).'
            in str(ctx.exception)
        )

    def test_fill_buf_non_hex(self):
        with self.assertRaises(RuntimeError) as ctx:
            self.shell.app_cmd("fillbuf testtesttest")
        assert (
            'error: Cannot parse "testtesttest" as hex string.'
            in str(ctx.exception)
        )

    def test_fill_reset(self):
        res = self.shell.od()
        byts = self.parser.parse(res)
        assert byts == b""
        self.shell.fill_buf(bytes(i for i in range(0, 64)))
        res = self.shell.od()
        byts = self.parser.parse(res)
        assert byts == bytes(i for i in range(0, 64))
        self.shell.reset_buf()
        res = self.shell.od()
        byts = self.parser.parse(res)
        assert byts == b""


class TestOdParser(TestBase):
    def test_od_parse_base(self):
        # confirm that only with offset output bytes are parsed
        res = self.shell.od()
        byts = self.parser.parse(res)
        assert byts == b""

        self.shell.fill_buf(bytes(range(0, 5)))
        res = self.shell.od()
        byts = self.parser.parse(res)
        assert byts == bytes(range(0, 5))

        self.shell.fill_buf(bytes(range(0, 16)))
        res = self.shell.od()
        byts = self.parser.parse(res)
        assert byts == bytes(range(0, 5)) + bytes(range(0, 16))

    def test_od_parse_width5(self):
        self.shell.fill_buf(bytes(range(0, 5)))
        res = self.shell.od(width=5)
        byts = self.parser.parse(res)
        assert byts == bytes(range(0, 5))

        self.shell.fill_buf(bytes(range(0, 16)))
        res = self.shell.od(width=5)
        byts = self.parser.parse(res)
        assert byts == bytes(range(0, 5)) + bytes(range(0, 16))

    def test_od_parse_width18(self):
        self.shell.fill_buf(bytes(range(0, 5)))
        res = self.shell.od(width=18)
        byts = self.parser.parse(res)
        assert byts == bytes(range(0, 5))

        self.shell.fill_buf(bytes(range(0, 36)))
        res = self.shell.od(width=18)
        byts = self.parser.parse(res)
        assert byts == bytes(range(0, 5)) + bytes(range(0, 36))

    def test_od_parse_offset(self):
        self.shell.fill_buf(bytes(range(0, 27)))
        res = self.shell.od(offset=0xc0ffee)
        with self.assertRaises(AssertionError) as ctx:
            self.parser.parse(res)
        assert f"Offset 0x{0xc0ffee:08x} != 0x00000000" in str(ctx.exception)
        byts = self.parser.parse(res, offset=0xc0ffee)
        assert byts == bytes(range(0, 27))

    def test_od_parse_offset_width(self):
        self.shell.fill_buf(bytes(range(0, 19)))
        res = self.shell.od(offset=0xabe, width=8)
        with self.assertRaises(AssertionError) as ctx:
            self.parser.parse(res)
        assert f"Offset 0x{0xabe:08x} != 0x00000000" in str(ctx.exception)
        byts = self.parser.parse(res, offset=0xabe)
        assert byts == bytes(range(0, 19))


if __name__ == "__main__":
    unittest.main()
