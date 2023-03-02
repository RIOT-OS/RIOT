#!/usr/bin/env python3

# Copyright (C) 2022 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import unittest

from scapy.all import load_contrib, IPv6, UDP
from scapy.contrib.coap import CoAP

from testrunner import check_unittests
from testrunner.unittest import PexpectTestCase


DEBUG_TESTS = bool(int(os.environ.get("DEBUG_TESTS", 0)))


class TestCompiledUnittests(PexpectTestCase):
    LOGFILE = sys.stdout if DEBUG_TESTS else None

    def test_compiled_unittests(self):
        self.spawn.sendline("unittests")
        check_unittests(self.spawn)


class InputMixin:
    MAX = 16

    def input(self, inp):
        if isinstance(inp, str):
            input_hex = inp
        else:
            input_hex = bytes(inp).hex()
        while input_hex:
            self.spawn.sendline("input add {}".format(input_hex[: self.MAX]))
            self.spawn.expect_exact("Successfully added to input buffer")
            self.expect_od_dump_of(input_hex[:self.MAX])
            input_hex = input_hex[self.MAX:]

    def input_reset(self):
        self.spawn.sendline("input reset")

    def expect_od_dump_of(self, byts):
        if isinstance(byts, str):
            hexbytes = byts
        else:
            hexbytes = bytes(byts).hex()
        for i in range((len(hexbytes) // 32) + 1):
            rang = hexbytes[(i * 32) : ((i * 32) + 32)]  # noqa: E203
            if not rang:
                # reached end directly at line break
                break
            od_bytes = "".join(
                [
                    "  {}{}".format(a.upper(), b.upper())
                    for a, b in zip(rang[::2], rang[1::2])
                ]
            )
            self.spawn.expect_exact("{:08x}{}".format(i * 16, od_bytes))


class TestSelfTest(PexpectTestCase, InputMixin):
    LOGFILE = sys.stdout if DEBUG_TESTS else None

    def tearDown(self):
        self.input_reset()
        self.spawn.expect_exact("Successfully reset input buffer.")
        self.spawn.sendline("output reset")
        self.spawn.expect_exact("Successfully reset output buffer.")
        self.spawn.sendline("compress init ::")
        self.spawn.sendline("set_ack 0 11111111")
        self.spawn.expect_exact("ACKs set to 0 ms delay with bitmap 11111111")
        self.spawn.sendline("timer reset")
        self.spawn.expect_exact("Reset all timers")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_input_add(self):
        self.spawn.sendline("input")
        self.spawn.expect_exact("Input buffer is empty.")
        self.spawn.sendline("input add 0123456789abcdef")
        self.spawn.expect_exact("Successfully added to input buffer")
        self.spawn.expect_exact("00000000  01  23  45  67  89  AB  CD  EF")
        self.spawn.sendline("input")
        self.spawn.expect_exact("00000000  01  23  45  67  89  AB  CD  EF")
        self.spawn.sendline("input add dead c0ffee 20b07 42")
        self.spawn.expect_exact("Successfully added to input buffer")
        self.spawn.expect_exact("00000000  DE  AD  C0  FF  EE  20  B0  74  20")
        self.spawn.sendline("input")
        self.spawn.expect_exact(
            "00000000  01  23  45  67  89  AB  CD  EF  DE  AD  C0  FF  EE  20  B0  74"
        )
        self.spawn.expect_exact("00000010  20")

    def test_input_add_full_buf_mod_128(self):
        for _ in range(0, 128, 16):
            self.spawn.sendline("input add {}".format("1a" * 16))
            self.spawn.expect_exact("Successfully added to input buffer")
            self.spawn.expect_exact(
                "00000000  {}".format("  ".join("1A" for _ in range(16)))
            )
        self.spawn.sendline("input")
        for offset in range(0, 128, 16):
            self.spawn.expect_exact(
                "000000{:02x}  {}".format(offset, "  ".join("1A" for _ in range(16)))
            )
        self.spawn.sendline("input add 2b")
        self.spawn.expect_exact("Too many bytes added to input buffer")

    def test_input_add_full_buf_not_mod_128(self):
        for offset in range(0, 126, 21):
            self.spawn.sendline("input add {}".format("1a" * 21))
            self.spawn.expect_exact("Successfully added to input buffer")
            self.spawn.expect_exact(
                "00000000  {}".format("  ".join("1A" for _ in range(16)))
            )
            self.spawn.expect_exact(
                "00000010  {}".format("  ".join("1A" for _ in range(5)))
            )
        self.spawn.sendline("input add {}".format("1a" * 5))
        self.spawn.expect_exact("Too many bytes added to input buffer")
        self.spawn.sendline("input")
        for offset in range(0, 128, 16):
            self.spawn.expect_exact(
                "000000{:02x}  {}".format(offset, "  ".join("1A" for _ in range(16)))
            )
        self.spawn.sendline("input add 2b")
        self.spawn.expect_exact("Too many bytes added to input buffer")

    def test_input_usage(self):
        self.spawn.sendline("input help")
        self.spawn.expect_exact("usage: input {reset|add <hex> ...}")
        self.spawn.sendline("input add")
        self.spawn.expect_exact("usage: input {reset|add <hex> ...}")
        self.spawn.sendline("input foobar test")
        self.spawn.expect_exact("usage: input {reset|add <hex> ...}")

    def test_input_reset(self):
        self.spawn.sendline("input")
        self.spawn.expect_exact("Input buffer is empty.")
        self.spawn.sendline("input reset")
        self.spawn.expect_exact("Successfully reset input buffer.")
        self.spawn.sendline("input")
        self.spawn.expect_exact("Input buffer is empty.")
        self.spawn.sendline("input add 0123456789abcdef")
        self.spawn.expect_exact("Successfully added to input buffer")
        self.spawn.expect_exact("00000000  01  23  45  67  89  AB  CD  EF")
        self.spawn.sendline("input")
        self.spawn.expect_exact("00000000  01  23  45  67  89  AB  CD  EF")
        self.spawn.sendline("input reset")
        self.spawn.expect_exact("Successfully reset input buffer.")
        self.spawn.sendline("input")
        self.spawn.expect_exact("Input buffer is empty.")

    def test_output_usage(self):
        self.spawn.sendline("output help")
        self.spawn.expect_exact("usage: output [{reset|copy}]")

    def test_output_show(self):
        self.spawn.sendline("output")
        self.spawn.expect_exact("Output buffer is empty")

    def test_output_copy(self):
        self.spawn.sendline("output copy")
        self.spawn.expect_exact("Successfully copied output buffer to input buffer.")

    def test_compress_init_parse_error(self):
        self.spawn.sendline("compress init foobar")
        self.spawn.expect_exact("Unable to parse source IPv6 address foobar.")
        self.spawn.expect_exact(
            "usage: compress {{up|down|bi} <device id>|init <source addr>}"
        )

    def test_compress_init_success(self):
        self.spawn.sendline("compress init 2001:db8::1")
        self.spawn.expect_exact(
            "Successfully initialized compressor with source IPv6 address 2001:db8::1"
        )

    def test_compress_usage(self):
        self.spawn.sendline("compress")
        self.spawn.expect_exact(
            "usage: compress {{up|down|bi} <device id>|init <source addr>}"
        )
        self.spawn.sendline("compress bi 0")
        self.spawn.expect_exact(
            "usage: compress {{up|down|bi} <device id>|init <source addr>}"
        )
        self.spawn.sendline("compress xyz test 1337")
        self.spawn.expect_exact(
            "usage: compress {{up|down|bi} <device id>|init <source addr>}"
        )
        self.spawn.sendline("compress xyz 1337")
        self.spawn.expect_exact(
            "usage: compress {{up|down|bi} <device id>|init <source addr>}"
        )

    def test_compress_no_input(self):
        self.spawn.sendline("compress up 1")
        self.spawn.expect_exact("No input buffer defined")

    def test_compress_wrong_device_id(self):
        pkt = (
            IPv6(src="2001:db8:1::1", dst="2001:db8:1::2", hlim=56)
            / UDP(sport=0x1F42, dport=0x1F42)
            / "the payload"
        )
        self.input(pkt)
        self.spawn.sendline("compress init {}".format(pkt[IPv6].src))
        self.spawn.sendline("compress down 4")
        self.spawn.expect_exact("Unable to compress (maybe wrong device ID 4?)")

    def test_compress_success(self):
        pkt = (
            IPv6(src="2001:db8:1::1", dst="2001:db8:1::2", hlim=56)
            / UDP(sport=0x1F42, dport=0x1F42)
            / "the payload"
        )
        self.input(pkt)
        self.spawn.sendline("compress init {}".format(pkt[IPv6].src))
        self.spawn.sendline("compress down 1")
        self.spawn.expect_exact("Used uncompressed rule 0/8 to generate")
        self.expect_od_dump_of(b"\0" + bytes(pkt))
        self.input_reset()
        pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.spawn.sendline("compress init {}".format(pkt[IPv6].src))
        self.input(pkt)
        self.spawn.sendline("compress up 1")
        self.spawn.expect_exact("Used rule 3/8 to compress to")
        exp_pkt = b"\x03\x30\x90"
        self.expect_od_dump_of(exp_pkt)
        self.spawn.sendline("output")
        self.expect_od_dump_of(exp_pkt)
        return exp_pkt

    def test_decompress_usage(self):
        self.spawn.sendline("decompress")
        self.spawn.expect_exact("usage: decompress {up|down|bi} <device id>")
        self.spawn.sendline("decompress bi 0")
        self.spawn.expect_exact("usage: decompress {up|down|bi} <device id>")
        self.spawn.sendline("decompress xyz test")
        self.spawn.expect_exact("usage: decompress {up|down|bi} <device id>")
        self.spawn.sendline("decompress xyz 123")
        self.spawn.expect_exact("usage: decompress {up|down|bi} <device id>")

    def test_decompress_success(self):
        pkt = IPv6(src="2001:db8:1::1", dst="2001:db8:1::2", hlim=56) / UDP(
            sport=0x1F42, dport=0x1F42
        ) / "payload"
        self.input(b"\0" + bytes(pkt))
        self.spawn.sendline("decompress down 1")
        self.expect_od_dump_of(pkt)
        pkt = IPv6(src="2001:db8:1::1", dst="2001:db8:1::2", hlim=56) / UDP(
            sport=0x1F42, dport=0x1F42
        )
        pkt = b"\x03\x30\x90"
        self.input_reset()
        self.input(pkt)
        self.spawn.sendline("decompress up 1")
        exp_pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.expect_od_dump_of(exp_pkt)

    def test_decompress_no_input(self):
        self.spawn.sendline("decompress up 1")
        self.spawn.expect_exact("No input buffer defined")

    def test_set_ack_usage(self):
        self.spawn.sendline("set_ack foobar")
        self.spawn.expect_exact("usage: set_ack <delay ms> <bitmap>")
        self.spawn.sendline("set_ack foobar 1test")
        self.spawn.expect_exact("usage: set_ack <delay ms> <bitmap>")
        self.spawn.sendline("set_ack 12 1test")
        self.spawn.expect_exact("usage: set_ack <delay ms> <bitmap>")

    def test_set_ack_too_large(self):
        self.spawn.sendline("set_ack 0 010001100")
        self.spawn.expect_exact("010001100 does not fit fragment window size 8")

    def test_set_ack_success(self):
        self.spawn.sendline("set_ack")
        self.spawn.expect_exact("ACKs set to 0 ms delay with bitmap 11111111")
        self.spawn.sendline("set_ack 10 01000110")
        self.spawn.expect_exact("ACKs set to 10 ms delay with bitmap 01000110")
        self.spawn.sendline("set_ack 5 00000000")
        self.spawn.expect_exact("ACKs set to 5 ms delay with bitmap 00000000")
        self.spawn.sendline("set_ack 0 11111111")
        self.spawn.expect_exact("ACKs set to 0 ms delay with bitmap 11111111")

    def test_timer_success(self):
        self.spawn.sendline("timer foobar")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_usage(self):
        self.spawn.sendline("fragment")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1 25")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1 25 5000")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1 25 5000 foobar")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1 25 dc no-ack")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1 mtu 5000 ack-always")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment 1 9000 5000 ack-on-error")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )
        self.spawn.sendline("fragment x 25 5000 not-fragmented")
        self.spawn.expect_exact(
            "usage: fragment <device id> <MTU> <duty cycle> "
            "{no-ack|ack-always|ack-on-error|not-fragmented}"
        )

    def test_fragment_init(self):
        self.spawn.sendline("fragment init")
        self.spawn.expect_exact("Fragmenter initialized")

    def test_fragment_success_no_frag(self):
        pkt = self.test_compress_success()
        self.spawn.sendline("output copy")
        self.spawn.expect_exact("Successfully copied output buffer to input buffer.")
        self.spawn.sendline("fragment init")
        self.spawn.sendline("fragment 1 25 5000 no-ack")
        self.spawn.expect_exact("TX Fragment 1 on dev 0x1")
        self.expect_od_dump_of(pkt)
        self.spawn.expect_exact("No fragmentation needed")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_success_no_ack(self):
        pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.input(bytes(pkt))
        self.spawn.sendline("fragment init")
        self.spawn.sendline("fragment 1 25 50 no-ack")
        self.spawn.expect_exact("TX Fragment 1 on dev 0x1")
        self.expect_od_dump_of(
            b"\x15\x30\x00\x00\x00\x00\x06\x08\xa0\x7f\x40\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.expect_exact("TX Fragment 2 on dev 0x1")
        self.expect_od_dump_of(
            b"\x15\x7f\xa0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            b"\x87\xd0\x47\xd0\x80\x03\x21\x13"
        )
        self.spawn.expect_exact("TX Fragment 3 on dev 0x1")
        self.expect_od_dump_of(b"\x15\x98\xa6\x43\xbf\xc8\x00\x00\x00\x00")
        self.spawn.expect_exact("TX End")
        self.spawn.expect_exact("Fragmented!")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_success_ack_always_all1(self):
        pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.input(pkt)
        self.spawn.sendline("fragment init")
        self.spawn.sendline("set_ack 0 11111111")
        self.spawn.sendline("fragment 1 25 50 ack-always")
        self.spawn.expect_exact("TX Fragment 1 on dev 0x1")
        self.expect_od_dump_of(
            b"\x17\x66\x00\x00\x00\x00\x00\xc1\x14\x0f\xe8\x00\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x40")
        self.spawn.expect_exact("TX Fragment 2 on dev 0x1")
        self.expect_od_dump_of(
            b"\x17\x51\xfe\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02"
            b"\x1f\x41\x1f\x42\x00\x0c\x84"
        )
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x40")
        self.spawn.expect_exact("TX Fragment 3 on dev 0x1")
        self.expect_od_dump_of(b"\x17\x73\x14\xc8\x77\xf4\xe4\x00\x00\x00\x00")
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x40")
        self.spawn.expect_exact("TX Fragment 4 on dev 0x1")
        self.expect_od_dump_of(b"\x17\x73\x14\xc8\x77\xf0")
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x40")
        self.spawn.expect_exact("TX End")
        self.spawn.expect_exact("Fragmented!")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_success_ack_always_001(self):
        pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.input(pkt)
        self.spawn.sendline("fragment init")
        self.spawn.sendline("set_ack 10 00111111")
        self.spawn.sendline("fragment 1 25 50 ack-always")
        self.spawn.expect_exact("TX Fragment 1 on dev 0x1")
        self.expect_od_dump_of(
            b"\x17\x66\x00\x00\x00\x00\x00\xc1\x14\x0f\xe8\x00\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x1f")
        self.spawn.expect_exact("TX Fragment 2 on dev 0x1")
        self.expect_od_dump_of(
            b"\x17\x51\xfe\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02"
            b"\x1f\x41\x1f\x42\x00\x0c\x84"
        )
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x5f")
        self.spawn.expect_exact("TX Fragment 3 on dev 0x1")
        self.expect_od_dump_of(b"\x17\x73\x14\xc8\x77\xf4\xe4\x00\x00\x00\x00")
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x40")
        self.spawn.expect_exact("TX Fragment 4 on dev 0x1")
        self.expect_od_dump_of(b"\x17\x73\x14\xc8\x77\xf0")
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x17\x40")
        self.spawn.expect_exact("TX End")
        self.spawn.expect_exact("Fragmented!")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_success_ack_on_error_all1(self):
        pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.input(pkt)
        self.spawn.sendline("fragment init")
        self.spawn.sendline("set_ack 0 11111111")
        self.spawn.sendline("fragment 1 25 50 ack-on-error")
        self.spawn.expect_exact("TX Fragment 1 on dev 0x1")
        self.expect_od_dump_of(
            b"\x16\x66\x00\x00\x00\x00\x00\xc1\x14\x0f\xe8\x00\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.expect_exact("TX Fragment 2 on dev 0x1")
        self.expect_od_dump_of(
            b"\x16\x51\xfe\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02"
            b"\x1f\x41\x1f\x42\x00\x0c\x84"
        )
        self.spawn.expect_exact("TX Fragment 3 on dev 0x1")
        self.expect_od_dump_of(b"\x16\x73\x14\xc8\x77\xf4\xe4\x00\x00\x00\x00")
        self.spawn.expect_exact("TX End")
        self.spawn.expect_exact("Fragmented")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_success_ack_on_error_10(self):
        pkt = (
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        self.input(pkt)
        self.spawn.sendline("fragment init")
        self.spawn.sendline("set_ack 0 10111111")
        self.spawn.sendline("fragment 1 25 50 ack-on-error")
        self.spawn.expect_exact("TX Fragment 1 on dev 0x")
        self.expect_od_dump_of(
            b"\x16\x66\x00\x00\x00\x00\x00\xc1\x14\x0f\xe8\x00\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.expect_exact("Simulate ACK")
        self.expect_od_dump_of(b"\x16\x5f")
        self.spawn.expect_exact("TX Fragment 2 on dev 0x")
        self.expect_od_dump_of(
            b"\x16\x51\xFE\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02"
            b"\x1F\x41\x1F\x42\x00\x0C\x84"
        )
        self.spawn.expect_exact("TX Fragment 3 on dev 0x")
        self.expect_od_dump_of(b"\x16\x73\x14\xc8\x77\xf4\xe4\x00\x00\x00\x00")
        self.spawn.expect_exact("TX End")
        self.spawn.expect_exact("Fragmented")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_fragment_no_input(self):
        self.spawn.sendline("compress init fe80::1")
        self.spawn.sendline("fragment init")
        self.spawn.sendline("fragment 1 25 5000 no-ack")
        self.spawn.expect_exact("No input buffer defined")

    def test_fragment_no_rule(self):
        pkt = (
            IPv6(
                src="2001:db8::1",
                dst="2001:db8::2",
                hlim=64,
            )
            / UDP(sport=0x1F40, dport=0x1F40)
            / CoAP(paymark=b"\xff")
            / (2 * "0123456789abcdef")
        )
        self.input(pkt)
        self.spawn.sendline("compress init {}".format(pkt[IPv6].src))
        self.spawn.sendline("fragment init")
        self.spawn.sendline("fragment 4 25 5000 ack-always")
        self.spawn.expect_exact(
            "No fragmentation rule known for mode ack-always on device 4"
        )
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_reassemble_usage(self):
        self.spawn.sendline("reassemble")
        self.spawn.expect_exact("usage: reassemble <device id> <timeout>")
        self.spawn.sendline("reassemble foobar")
        self.spawn.expect_exact("usage: reassemble <device id> <timeout>")
        self.spawn.sendline("reassemble test 5000")
        self.spawn.expect_exact("usage: reassemble <device id> <timeout>")
        self.spawn.sendline("reassemble 1 foobar")
        self.spawn.expect_exact("usage: reassemble <device id> <timeout>")

    def test_reassemble_no_input(self):
        self.spawn.sendline("reassemble init")
        self.spawn.sendline("reassemble 1 100")
        self.spawn.expect_exact("No input buffer defined")

    def test_reassemble_no_frag(self):
        pkt = b"\x03\x30\x90"
        self.input(pkt)
        self.spawn.sendline("reassemble init")
        self.spawn.sendline("reassemble 1 100")
        self.spawn.expect_exact("RX Unfragmented on dev 0x1")
        self.expect_od_dump_of(pkt)
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_reassemble_success_no_ack(self):
        self.input(
            b"\x15\x30\x00\x00\x00\x00\x06\x08\xa0\x7f\x40\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.sendline("reassemble init")
        self.spawn.sendline("reassemble 1 100")
        self.input_reset()
        self.input(
            b"\x15\x7f\xa0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
            b"\x87\xd0\x47\xd0\x80\x03\x21\x13"
        )
        self.spawn.sendline("reassemble 1 100")
        self.input_reset()
        self.input(b"\x15\x98\xa6\x43\xbf\xc8\x00\x00\x00\x00")
        self.spawn.sendline("reassemble 1 100")
        self.spawn.expect_exact("RX Reassembly complete on dev 0x1")
        self.expect_od_dump_of(
            bytes(
                IPv6(
                    src="fe80::1",
                    dst="fe80::2",
                )
                / UDP(sport=0x1F41, dport=0x1F42)
                / CoAP()
            )
        )
        free_timers = 0
        total_timers = 4
        while free_timers != total_timers:
            self.spawn.sendline("timer")
            self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
            free_timers = int(self.spawn.match.group(1))
            total_timers = int(self.spawn.match.group(2))

    def test_reassemble_success_ack_always(self):
        self.input(
            b"\x17\x66\x00\x00\x00\x00\x00\xc1\x14\x0f\xe8\x00\x00\x00\x00\x00\x00\x00"
            b"\x00\x00\x00\x00\x00\x00\x00"
        )
        self.spawn.sendline("reassemble init")
        self.spawn.sendline("reassemble 1 100")
        self.input_reset()
        self.input(
            b"\x17\x51\xfe\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02"
            b"\x1f\x41\x1f\x42\x00\x0c\x84"
        )
        self.spawn.sendline("reassemble 1 100")
        self.input_reset()
        self.input(b"\x17\x73\x14\xc8\x77\xf4\xe4\x00\x00\x00\x00")
        self.spawn.sendline("reassemble 1 100")
        self.spawn.expect_exact("Packet sent on dev 0x1")
        self.expect_od_dump_of(b"\x17\x40")
        self.expect_od_dump_of(
            IPv6(
                src="fe80::1",
                dst="fe80::2",
            )
            / UDP(sport=0x1F41, dport=0x1F42)
            / CoAP()
        )
        free_timers = 0
        total_timers = 4
        while free_timers != total_timers:
            self.spawn.sendline("timer")
            self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
            free_timers = int(self.spawn.match.group(1))
            total_timers = int(self.spawn.match.group(2))


class TestFragmentation(PexpectTestCase, InputMixin):
    LOGFILE = sys.stdout if DEBUG_TESTS else None

    def tearDown(self):
        self.input_reset()
        self.spawn.sendline("output reset")
        self.spawn.sendline("compress init ::")
        self.spawn.expect_exact("Successfully reset input buffer.")
        self.spawn.sendline("set_ack 0 11111111")
        self.spawn.expect_exact("ACKs set to 0 ms delay with bitmap 11111111")
        self.spawn.sendline("timer reset")
        self.spawn.expect_exact("Reset all timers")
        self.spawn.sendline("timer")
        self.spawn.expect(r"Free timers: (\d+) \(of (\d+)\)")
        self.assertEqual(int(self.spawn.match.group(1)), int(self.spawn.match.group(2)))

    def test_compress_rule_2_up(self):
        pkt = (
            IPv6(
                src="2001:db8:1::1",
                dst="2001:db8:2::2",
            )
            / UDP(sport=0x1389, dport=0x1388)
            / CoAP(
                type="ACK",
                tkl=4,
                code="2.05 Content",
                msg_id=0x23BC,
                token=b"TOK!",
                paymark=b"\xff",
            )
            / "payload"
        )
        self.spawn.sendline("compress init {}".format(pkt[IPv6].src))
        self.input(pkt)
        self.spawn.sendline("compress up 1")
        self.spawn.expect_exact("Used rule 2/8 to compress to")
        exp_pkt = [
            0x02,       # rule ID = 2
            # IP6_APPPRE mapping index = 1 (2b),
            # COAP_T = 2 [ACK] (2b)
            # COAP_MID LSB = 0xC (4b)
            (1 << 6) | (2 << 4) | (0xC),
            # Token
            ord("T"),
            ord("O"),
            ord("K"),
            ord("!"),
            # payload
            ord("p"),
            ord("a"),
            ord("y"),
            ord("l"),
            ord("o"),
            ord("a"),
            ord("d"),
        ]
        self.expect_od_dump_of(exp_pkt)

    def test_decompress_rule_2_up(self):
        exp_pkt = (
            IPv6(
                src="2001:db8:1::1",
                dst="2001:db8:2::2",
            )
            / UDP(sport=0x1389, dport=0x1388)
            / CoAP(
                type="ACK",
                tkl=4,
                code="2.05 Content",
                msg_id=0x23BC,
                token=b"TOK!",
                paymark=b"\xff",
            )
            / "payload"
        )
        self.spawn.sendline("decompress init {}".format(exp_pkt[IPv6].src))
        pkt = [
            # Rule ID: 2
            0x02,
            # IP6_APPPRE mapping index = 1 (2b),
            # COAP_T = 2 [ACK] (2b) (libSCHC bug, should be 2)
            # COAP_MID LSB = 0xC (4b)
            (1 << 6) | (2 << 4) | (0xC),
            # Token
            ord("T"),
            ord("O"),
            ord("K"),
            ord("!"),
            # payload
            ord("p"),
            ord("a"),
            ord("y"),
            ord("l"),
            ord("o"),
            ord("a"),
            ord("d"),
        ]
        self.input(pkt)
        self.spawn.sendline("decompress up 1")
        self.spawn.expect_exact("Decompressed to")
        self.expect_od_dump_of(exp_pkt)

    def test_compress_rule_2_down(self):
        pkt = (
            IPv6(
                src="2001:db8:2::2",
                dst="2001:db8:1::1",
                hlim=64,
            )
            / UDP(sport=0x1388, dport=0x1389)
            / CoAP(
                type="CON",
                tkl=4,
                code="GET",
                msg_id=0x23BC,
                token=b"TOK!",
                options=[("Uri-Path", "temp")],
            )
        )
        self.spawn.sendline("compress init {}".format(pkt[IPv6].src))
        self.input(pkt)
        self.spawn.sendline("compress down 1")
        self.spawn.expect_exact("Used rule 2/8 to compress to")
        exp_pkt = [
            # Rule ID: 2
            0x02,
            # Hop-Limit: 64 (8b)
            64,
            # IP6_APPPRE mapping index = 1 (2b),
            # COAP_T = 0 [CON] (2b)
            # COAP_MID[0] 4 msb = 0x23 >> 4
            (1 << 6) | (0 << 4) | (0x23 >> 4),
            # COAP_MID[0] 4 lsb = (0x23 << 4) & 0xff
            # COAP_MID[1] 4 msb = 0xBC >> 4
            ((0x23 << 4) & 0xFF) | (0xBC >> 4),
            # COAP_MID[1] 4 lsb = (0xBC << 4) & 0xff
            # Token[0] 4 msb = ord("T") >> 4
            ((0xBC << 4) & 0xFF) | (ord("T") >> 4),
            # Token[0] 4 lsb = ord("T") >> 4
            # Token[1] 4 msb = (ord("O") << 4) & 0xff
            ((ord("T") << 4) & 0xFF) | (ord("O") >> 4),
            # Token[1] 4 lsb = ord("O") >> 4
            # Token[2] 4 msb = (ord("K") << 4) & 0xff
            ((ord("O") << 4) & 0xFF) | (ord("K") >> 4),
            # Token[2] 4 lsb = ord("K") >> 4
            # Token[3] 4 msb = (ord("!") << 4) & 0xff
            ((ord("K") << 4) & 0xFF) | (ord("!") >> 4),
            # Token[2] 4 lsb = ord("K") >> 4
            # 4b padding
            ((ord("!") << 4) & 0xFF),
        ]
        self.expect_od_dump_of(exp_pkt)

    def test_decompress_rule_2_down(self):
        exp_pkt = (
            IPv6(
                src="2001:db8:2::2",
                dst="2001:db8:1::1",
                hlim=64,
            )
            / UDP(sport=0x1388, dport=0x1389)
            / CoAP(
                type="CON",
                tkl=4,
                code="GET",
                msg_id=0x23BC,
                token=b"TOK!",
                options=[("Uri-Path", "temp")],
            )
        )
        self.spawn.sendline("decompress init {}".format(exp_pkt[IPv6].src))
        pkt = [
            # Rule ID: 2
            0x02,
            # Hop-Limit: 64 (8b)
            64,
            # IP6_APPPRE mapping index = 1 (2b),
            # COAP_T = 0 [CON] (2b)
            # COAP_MID[0] 4 msb = 0x23 >> 4
            (1 << 6) | (0 << 4) | (0x23 >> 4),
            # COAP_MID[0] 4 lsb = (0x23 << 4) & 0xff
            # COAP_MID[1] 4 msb = 0xBC >> 4
            ((0x23 << 4) & 0xFF) | (0xBC >> 4),
            # COAP_MID[1] 4 lsb = (0xBC << 4) & 0xff
            # Token[0] 4 msb = ord("T") >> 4
            ((0xBC << 4) & 0xFF) | (ord("T") >> 4),
            # Token[0] 4 lsb = ord("T") >> 4
            # Token[1] 4 msb = (ord("O") << 4) & 0xff
            ((ord("T") << 4) & 0xFF) | (ord("O") >> 4),
            # Token[1] 4 lsb = ord("O") >> 4
            # Token[2] 4 msb = (ord("K") << 4) & 0xff
            ((ord("O") << 4) & 0xFF) | (ord("K") >> 4),
            # Token[2] 4 lsb = ord("K") >> 4
            # Token[3] 4 msb = (ord("!") << 4) & 0xff
            ((ord("K") << 4) & 0xFF) | (ord("!") >> 4),
            # Token[2] 4 lsb = ord("K") >> 4
            # 4b padding
            ((ord("!") << 4) & 0xFF),
        ]
        self.input(pkt)
        self.spawn.sendline("decompress down 1")
        self.spawn.expect_exact("Decompressed to")
        self.expect_od_dump_of(exp_pkt)


if __name__ == "__main__":
    load_contrib("coap")
    unittest.main(verbosity=2)
