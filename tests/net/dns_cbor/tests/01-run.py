#! /usr/bin/env python3
#
# Copyright (C) 2023 TU Dresden
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import ipaddress
import os
import sys
import unittest

import cbor2

from testrunner.unittest import PexpectTestCase

__author__ = "Martine S. Lenders <martine.lenders@tu-dresden.de>"


DEBUG = int(os.environ.get("TEST_DEBUG", 0))


class TestSelf(PexpectTestCase):
    LOGFILE = sys.stdout if DEBUG else None

    def tearDown(self):
        self.spawn.sendline("reset")

    def test_load(self):
        self.spawn.sendline("load")
        self.spawn.expect_exact("usage: load <hex bytes>")

        self.spawn.sendline("load abCdef")
        self.spawn.expect_exact('Successfully loaded "abCdef" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF")
        self.spawn.sendline("load 12345")
        self.spawn.expect_exact('Successfully loaded "12345" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF  12  34  50")
        self.spawn.sendline("load 6789")
        self.spawn.expect_exact('Successfully loaded "6789" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF  12  34  50  67  89")
        self.spawn.sendline("load 00")
        self.spawn.expect_exact('Successfully loaded "00" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF  12  34  50  67  89  00")

        self.spawn.sendline("load 1234xyz")
        self.spawn.expect_exact("Invalid character 'x' in \"1234xyz\"")
        self.spawn.sendline('load "abc-"')
        self.spawn.expect_exact("Invalid character '-' in \"abc-\"")
        self.spawn.sendline("load ef.")
        self.spawn.expect_exact("Invalid character '.' in \"ef.\"")

        self.spawn.sendline("load fedc")
        self.spawn.expect_exact('Successfully loaded "fedc" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF  12  34  50  67  89  00  FE  DC")

        res = 0
        while res == 0:
            self.spawn.sendline("load 0")
            res = self.spawn.expect_exact(
                ["Successfully loaded", "Buffer would overflow"]
            )
        self.assertEqual(res, 1)

    def test_reset(self):
        self.spawn.sendline("reset")
        self.spawn.sendline("load abcdef")
        self.spawn.expect_exact('Successfully loaded "abcdef" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF")
        self.spawn.sendline("load abcdef")
        self.spawn.expect_exact('Successfully loaded "abcdef" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF  AB  CD  EF")
        self.spawn.sendline("reset")
        self.spawn.sendline("load abcdef")
        self.spawn.expect_exact('Successfully loaded "abcdef" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF")

    def test_set_status(self):
        self.spawn.sendline("set_status")
        self.spawn.expect_exact("usage: set_status <domain name> [<type> [<class>]]")

        self.spawn.sendline("set_status test.com")
        self.spawn.expect_exact("Successfully set status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact('    domain_name: "test.com"')
        self.spawn.expect_exact("    type: 0")
        self.spawn.expect_exact("    class: 0")

        self.spawn.sendline("set_status test.org 1234")
        self.spawn.expect_exact("Successfully set status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact('    domain_name: "test.org"')
        self.spawn.expect_exact("    type: 1234")
        self.spawn.expect_exact("    class: 0")

        self.spawn.sendline("set_status example.org 1234 7654")
        self.spawn.expect_exact("Successfully set status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact('    domain_name: "example.org"')
        self.spawn.expect_exact("    type: 1234")
        self.spawn.expect_exact("    class: 7654")

    def test_dump(self):
        self.spawn.sendline("load abcdef")
        self.spawn.expect_exact('Successfully loaded "abcdef" into buffer:')
        self.spawn.expect_exact("00000000  AB  CD  EF")

        self.spawn.sendline("set_status example.de 4321 5678")
        self.spawn.expect_exact("Successfully set status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact('    domain_name: "example.de"')
        self.spawn.expect_exact("    type: 4321")
        self.spawn.expect_exact("    class: 5678")

        self.spawn.sendline("dump")
        self.spawn.expect_exact("Buffer:")
        self.spawn.expect_exact("00000000  AB  CD  EF")
        self.spawn.expect_exact("Status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact('    domain_name: "example.de"')
        self.spawn.expect_exact("    type: 4321")
        self.spawn.expect_exact("    class: 5678")
        self.spawn.expect_exact("END")

        self.spawn.sendline("reset")
        self.spawn.sendline("dump")
        self.spawn.expect_exact("Buffer: empty")
        self.spawn.expect_exact("Status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact('    domain_name: ""')
        self.spawn.expect_exact("    type: 0")
        self.spawn.expect_exact("    class: 0")
        self.spawn.expect_exact("END")

    def test_compose(self):
        self.spawn.sendline("compose")
        self.spawn.expect_exact("usage: compose <domain name> <type> [<class>]")
        self.spawn.sendline("compose domain.org")
        self.spawn.expect_exact("usage: compose <domain name> <type> [<class>]")
        self.spawn.sendline(f"compose domain.org {0x1 << 16}")
        self.spawn.expect_exact(f"Type {0x1 << 16} too large (must be 16-bit number)")
        self.spawn.sendline(f"compose domain.org {0xffff} {0x1 << 16}")
        self.spawn.expect_exact(f"Class {0x1 << 16} too large (must be 16-bit number)")
        self.spawn.sendline(
            f"compose this..is.a.very.long.domain.name {0xffff} {0xffff}"
        )
        self.spawn.expect_exact(
            'Domain name "this..is.a.very.long.domain.name" is longer than '
            "31 characters"
        )
        self.spawn.sendline(f"compose this.is.a.very.long.domain.name {0xffff}")
        self.spawn.expect_exact("Success:")
        self.spawn.sendline(
            f"compose this.is.a.very.long.domain.name {0xffff} {0xffff}"
        )
        self.spawn.expect_exact("Success:")
        # Don't care about actual result yet

    def test_error_codes(self):
        self.spawn.sendline("error_codes")
        self.spawn.expect(r'EBADMSG \d+ "[^"]+"')
        self.spawn.expect(r'EINVAL \d+ "[^"]+"')
        self.spawn.expect(r'ENOBUFS \d+ "[^"]+"')
        self.spawn.expect(r'ENOMEM \d+ "[^"]+"')
        self.spawn.expect(r'ENOSPC \d+ "[^"]+"')
        self.spawn.expect_exact("END_ERROR")


class HasErrorCodes(PexpectTestCase):
    def setUp(self):
        if not hasattr(self, "error_codes") or not self.error_codes:
            self.error_codes = {}
            res = 0
            self.spawn.sendline("error_codes")
            while res == 0:
                res = self.spawn.expect([r'([A-Z_]+) (\d+) "([^"]+)"', "END_ERROR"])
                if res == 0:
                    self.error_codes[self.spawn.match[1]] = {
                        "value": int(self.spawn.match[2]),
                        "str": self.spawn.match[3],
                    }

    def expect_error(self, errno):
        self.spawn.expect_exact(
            f"Error {self.error_codes[errno]['value']} "
            f"\"{self.error_codes[errno]['str']}\""
        )


class TestCompose(HasErrorCodes):
    LOGFILE = sys.stdout if DEBUG else None

    def compose(self, domain_name, typ=28, cls=1):
        self.spawn.sendline(f'compose "{domain_name}" {typ} {cls}')

    def dump(self):
        self.spawn.sendline("dump")

    def expect_cbor(self, obj):
        cbor = cbor2.dumps(obj)
        for i in range(0, len(cbor), 16):
            self.spawn.expect_exact(
                f"{i:08X}  {'  '.join(f'{c:02X}' for c in cbor[i:i + 16])}"
            )

    def expect_status(self, obj):
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact(f"  domain_name: \"{obj['question']['domain_name']}\"")
        self.spawn.expect_exact(f"  type: {obj['question']['type']}")
        self.spawn.expect_exact(f"  class: {obj['question']['class']}")

    def tearDown(self):
        self.spawn.sendline("reset")

    def test_empty_AAAA(self):
        self.compose("")
        self.expect_cbor([[]])
        self.dump()
        self.expect_status({"question": {"domain_name": "", "type": 28, "class": 1}})

    def test_empty_A(self):
        self.compose("", 1)
        self.expect_cbor([[1]])
        self.dump()
        self.expect_status({"question": {"domain_name": "", "type": 1, "class": 1}})

    def test_example_org_AAAA(self):
        self.compose("example.org")
        self.expect_cbor([["example", "org"]])
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_A(self):
        self.compose("example.org", 1)
        self.expect_cbor([["example", "org", 1]])
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 1, "class": 1}}
        )

    def test_example_org_CNAME(self):
        self.compose("example.org", 5)
        self.expect_cbor([["example", "org", 5]])
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_maximum_length(self):
        self.compose("this_is_a_very_long_domain_name", 0xFFFF, 0xFFFF)
        self.expect_cbor([["this_is_a_very_long_domain_name", 0xFFFF, 0xFFFF]])


class TestParse(HasErrorCodes):
    LOGFILE = sys.stdout if DEBUG else None

    def expect_buffer(self, byts):
        for i in range(0, len(byts), 16):
            self.spawn.expect_exact(
                f"{i:08X}  {'  '.join(f'{c:02X}' for c in byts[i:i + 16])}"
            )

    def expect_ttl(self, ttl):
        self.spawn.expect_exact(f"TTL: {ttl}")

    def expect_status(self, obj):
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact(f"  domain_name: \"{obj['question']['domain_name']}\"")
        self.spawn.expect_exact(f"  type: {obj['question']['type']}")
        self.spawn.expect_exact(f"  class: {obj['question']['class']}")

    def set_buffer(self, byts):
        load_size = 16
        for i in range(0, len(byts), load_size):
            cbor_hex = byts[i : i + load_size].hex()  # noqa: E203
            self.spawn.sendline(f"load {cbor_hex}")
            self.spawn.expect_exact(f'Successfully loaded "{cbor_hex}" into buffer')
            self.expect_buffer(byts[: i + load_size])

    def set_cbor(self, obj):
        self.set_buffer(cbor2.dumps(obj))

    def set_status(self, domain_name, typ=28, cls=1):
        self.spawn.sendline(f'set_status "{domain_name}" {typ} {cls}')
        self.spawn.expect_exact("Successfully set status:")
        self.spawn.expect_exact("  question:")
        self.spawn.expect_exact(f'    domain_name: "{domain_name}"')
        self.spawn.expect_exact(f"    type: {typ}")
        self.spawn.expect_exact(f"    class: {cls}")

    def parse(self):
        self.spawn.sendline("parse")

    def dump(self):
        self.spawn.sendline("dump")

    def tearDown(self):
        self.spawn.sendline("reset")

    def test_empty_response(self):
        self.set_status("")
        self.parse()
        self.expect_error("EINVAL")

    def test_empty_response_map(self):
        self.set_cbor({})
        self.set_status("")
        self.parse()
        self.expect_error("EINVAL")

    def test_empty_response_array(self):
        self.set_cbor([])
        self.set_status("")
        self.parse()
        self.expect_error("EINVAL")

    def test_empty_response_answer(self):
        self.set_cbor([[]])
        self.set_status("")
        self.parse()
        self.expect_error("EBADMSG")

    def test_with_empty_question_and_answer(self):
        self.set_cbor([[], []])
        self.set_status("")
        self.parse()
        self.expect_error("EBADMSG")

    def test_with_empty_question_and_answer_and_additional(self):
        self.set_cbor([[], [], []])
        self.set_status("")
        self.parse()
        self.expect_error("EBADMSG")

    def test_example_org_A_w_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        self.set_cbor([["example", "org", 1], [[1234, ipv4_addr]]])
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(ipv4_addr)
        self.expect_ttl(1234)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 1, "class": 1}}
        )

    def test_incomplete_name_in_question(self):
        self.set_buffer(b"\x82\x83gexamp")
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_example_org_CNAME_w_question(self):
        self.set_cbor(
            [["example", "org", 5], [[4321, "beispiel", cbor2.CBORTag(7, 1)]]]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x08beispiel\x03org\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_w_question_w_name(self):
        self.set_cbor(
            [
                ["example", "org", 5],
                [["example", "org", 4321, "beispiel", cbor2.CBORTag(7, 1)]],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x08beispiel\x03org\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_w_question_wrong_class(self):
        self.set_cbor(
            [
                ["example", "org", 5],
                [
                    [4321, 5, 12, "ejamplo", "es"],
                    [4321, 5, "beispiel", cbor2.CBORTag(7, 1)],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x08beispiel\x03org\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_w_question_wrong_name(self):
        self.set_cbor(
            [
                ["example", "org", 5],
                [
                    ["ejamplo", "org", 4321, 5, "ejamplo", "es"],
                    [4321, 5, "beispiel", cbor2.CBORTag(7, 1)],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x08beispiel\x03org\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_w_question_wrong_length(self):
        self.set_cbor(
            [
                ["example", "org", 5],
                [
                    ["example", "com", 4321, 5, "ejamplo", "es"],
                    [4321, 5, "beispiel", cbor2.CBORTag(7, 1)],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x08beispiel\x03org\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_w_question_wrong_name_same_prefix(self):
        self.set_cbor(
            [
                ["example", "org", 5],
                [
                    ["example", "org", "es", 4321, 5, "ejamplo", "es"],
                    [4321, 5, "beispiel", cbor2.CBORTag(7, 1)],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x08beispiel\x03org\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_empty_w_question(self):
        self.set_cbor([["example", "org", 5], [[4321]]])
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_example_org_CNAME_empty_tstrs_w_question(self):
        self.set_cbor([["example", "org", 5], [[4321, "", ""]]])
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"\x00")
        self.expect_ttl(4321)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 5, "class": 1}}
        )

    def test_stuff_after_rdata_name(self):
        self.set_cbor(
            [["example", "org", 5], [[4321, "beispiel", cbor2.CBORTag(7, 1), 23]]]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_error("EINVAL")

    def test_too_long_name(self):
        self.set_cbor(
            [
                ["example", "org", 5],
                [
                    [
                        3600,
                        5,
                        "this",
                        "is",
                        "a",
                        "very",
                        "long",
                        "name",
                        "and",
                        "there",
                        "is",
                        "more",
                    ]
                ],
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_error("ENOSPC")

    def test_too_long_name_for_rdata(self):
        self.set_cbor(
            [["example", "org", 5], [[3600, 5, "this", "is", "a", "long", "name"]]]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_error("ENOBUFS")

    def test_example_org_foreign_record_w_question(self):
        self.set_cbor([["example", "org", 64, 15], [[1234, b"012345"]]])
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(b"012345")
        self.expect_ttl(1234)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 64, "class": 15}}
        )

    def test_wrong_rtype_type_in_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        self.set_cbor([["example", "org", b"\x01"], [[1234, ipv4_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_too_large_type_in_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        self.set_cbor([["example", "org", 1 << 16], [[1234, ipv4_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_too_large_class_in_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        self.set_cbor([["example", "org", 1, 1 << 16], [[1234, ipv4_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_example_org_AAAA_w_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([["example", "org"], [[3600, ipv6_addr]]])
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_CNAME_w_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [7200, 5, "beispiel", "org"],
                    [3600, ipv6_addr],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_invalid_type_after_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([["example", "org"], ipv6_addr])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_type_in_rr(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([["example", "org"], [[ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_rdata_type(self):
        self.set_cbor([["example", "org"], [[3600, {}]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_type_in_answer_section(self):
        self.set_cbor([["example", "org"], [1234]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_too_large_rdata(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([["example", "org"], [[3600, ipv6_addr + b"\0"]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("ENOBUFS")

    def test_invalid_tag_rdata(self):
        self.set_cbor([["example", "org"], [[3600, 5, cbor2.CBORTag(16, 5)]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_example_org_AAAA_wo_question_empty_rdata(self):
        self.set_cbor([[[3600, b""]]])
        self.set_status("example.org")
        self.parse()
        self.spawn.expect_exact("Success: no data")
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_wo_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[[3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_wo_question_w_flags(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([0x8642, [[3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_too_large_flags(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([0x18642, [[3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_no_response_flag_set(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([0x7642, [[3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_example_org_AAAA_and_A_wo_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[[7200, 1, ipv4_addr], [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_diff_class_wo_question(self):
        ipv6_addr1 = ipaddress.ip_address("2001:db8::1").packed
        ipv6_addr2 = ipaddress.ip_address("2001:db8::2").packed
        self.set_cbor(
            [[[7200, 28, 12, ipv6_addr1], [3600, ipv6_addr2]]]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr2)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_same_name_prefix_wo_question(self):
        ipv6_addr1 = ipaddress.ip_address("2001:db8::1").packed
        ipv6_addr2 = ipaddress.ip_address("2001:db8::2").packed
        self.set_cbor(
            [[["example", "org", "de", 7200, ipv6_addr1], [3600, ipv6_addr2]]]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr2)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_same_length_name_wo_question(self):
        ipv6_addr1 = ipaddress.ip_address("2001:db8::1").packed
        ipv6_addr2 = ipaddress.ip_address("2001:db8::2").packed
        self.set_cbor(
            [[["example", "com", 7200, ipv6_addr1], [3600, ipv6_addr2]]]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr2)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_foreign_class_rr_wo_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[[7200, 64, 15, ipv4_addr], [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_CNAME_tstr_wo_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    [7200, 5, "beispiel", "org"],
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_CNAME_tstr_and_tag_wo_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [7200, 5, "beispiel", cbor2.CBORTag(7, 1)],
                    [3600, ipv6_addr],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_CNAME_tag_wo_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [7200, 5, cbor2.CBORTag(7, 0)],
                    [3600, ipv6_addr],
                ],
            ]
        )
        self.set_status("test", 234, 124)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_tstr_rdata_wo_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[[7200, 5, "example", "com"], [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_and_raw_rr_wo_question(self):
        ipv4_addr = ipaddress.ip_address("198.51.100.1").packed
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    b"\x07example\x03org\x00\x00\x01\x00\x01\x00\x00\x1c\x20\x00\x04"
                    + ipv4_addr,
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_wo_question_w_empty_opt(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[cbor2.CBORTag(141, []), [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_wo_question_w_empty_opt_wo_opts(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[cbor2.CBORTag(141, [[]]), [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_wo_question_full_opt(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    cbor2.CBORTag(
                        141, [1024, [12, b"abcd", 34, b"efgh"], 0x8000, 123, 1]
                    ),
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_example_org_AAAA_empty_question_w_empty_opt(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[], [cbor2.CBORTag(141, [[]]), [3600, ipv6_addr]]])
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(3600)
        self.dump()
        self.expect_status({"question": {"domain_name": "", "type": 28, "class": 1}})

    def test_opt_not_array(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[cbor2.CBORTag(141, b""), [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_not_opt_tag(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[], [cbor2.CBORTag(112, [b""]), [3600, ipv6_addr]]])
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_unexpected_type_in_opt(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[], [cbor2.CBORTag(141, [b""]), [3600, ipv6_addr]]])
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_opt_num_type(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[cbor2.CBORTag(141, [[b""]]), [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_opt_value_missing(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[cbor2.CBORTag(141, [[12]]), [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_opt_val_type(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([[cbor2.CBORTag(141, [[12, 34]]), [3600, ipv6_addr]]])
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_opt_non_uint_flags(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    cbor2.CBORTag(141, [1024, [12, b"abcd", 34, b"efgh"], -0x8000]),
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_opt_non_uint_rcode(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    cbor2.CBORTag(
                        141, [1024, [12, b"abcd", 34, b"efgh"], 0x8000, -123]
                    ),
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_opt_non_uint_version(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    cbor2.CBORTag(
                        141, [1024, [12, b"abcd", 34, b"efgh"], 0x8000, 123, -1]
                    ),
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_opt_too_many_uints_in_tail(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    cbor2.CBORTag(
                        141, [1024, [12, b"abcd", 34, b"efgh"], 0x8000, 123, 1, 2]
                    ),
                    [3600, ipv6_addr],
                ]
            ]
        )
        self.set_status("example.org")
        self.parse()
        self.expect_error("EINVAL")

    def test_name_ref_tag_in_question(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor([["example", "org", cbor2.CBORTag(7, 1)], [[3600, ipv6_addr]]])
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_name_ref(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [cbor2.CBORTag(7, 5), 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_invalid_name_ref_type(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [cbor2.CBORTag(7, b"\x05"), 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_name_ref_tag_in_answer(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [cbor2.CBORTag(7, 1), 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(6000)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_recursive_name_ref(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    ["www", "example", cbor2.CBORTag(7, 1), 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 3), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(6000)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_wrong_tag_for_name_ref(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    ["www", "example", cbor2.CBORTag(16, 1), 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 3), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_endless_recursion(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    ["www", "example", cbor2.CBORTag(7, 3), 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 3), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("EINVAL")

    def test_nearly_drain_name_ref(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", 3600, ipv6_addr],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_buffer(ipv6_addr)
        self.expect_ttl(6000)
        self.dump()
        self.expect_status(
            {"question": {"domain_name": "example.org", "type": 28, "class": 1}}
        )

    def test_drain_name_ref(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["www", "example", "org"],
                [
                    ["a", "b", "c", "d", "e", "f", "g", "h", "i", "j", 360, ipv6_addr],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("www.example.org", 1, 23)
        self.parse()
        self.expect_error("ENOMEM")

    def test_name_too_long_on_comp_border(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    [
                        "this",
                        "is",
                        "a",
                        "very",
                        "long",
                        "domain",
                        "name",
                        "and",
                        "there",
                        "is",
                        "more",
                        3600,
                        ipv6_addr,
                    ],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("ENOSPC")

    def test_name_too_long_on_name_ref(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                ["example", "org"],
                [
                    [
                        "this",
                        "is",
                        "a",
                        "very",
                        "long",
                        "domain",
                        "name",
                        cbor2.CBORTag(7, 1),
                        3600,
                        ipv6_addr,
                    ],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("ENOSPC")

    def test_name_too_long_within_comp(self):
        ipv6_addr = ipaddress.ip_address("2001:db8::1").packed
        self.set_cbor(
            [
                [
                    [
                        "these",
                        "are",
                        "very",
                        "long",
                        "domain",
                        "names",
                        "and",
                        "there",
                        "are",
                        "more",
                        3600,
                        ipv6_addr,
                    ],
                    [cbor2.CBORTag(7, 0), 6000, ipv6_addr],
                ],
            ]
        )
        self.set_status("example.org", 1, 23)
        self.parse()
        self.expect_error("ENOSPC")


if __name__ == "__main__":
    unittest.main(verbosity=2)
