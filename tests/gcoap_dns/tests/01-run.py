#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import unittest

from scapy.all import DNS, DNSQR

from testrunner import check_unittests
from testrunner.unittest import PexpectTestCase


class TestGCoAPDNS(PexpectTestCase):
    LOGFILE = sys.stdout

    def has_dns_cache(self):
        self.spawn.sendline("has_dns_cache")
        res = self.spawn.expect(["DNS cache does not exist", "DNS cache exists"])
        return bool(res)

    def test_embedded_unittests(self):
        self.spawn.sendline("unittests")
        check_unittests(self.spawn)

    def test_uri_cmd(self):
        self.spawn.sendline("uri")
        self.spawn.expect_exact("usage: uri -d")
        self.spawn.expect_exact("       uri <uri>")
        self.spawn.sendline("uri https://example.org")
        self.spawn.expect_exact("Invalid argument")
        self.spawn.sendline("uri coap://example.org")
        self.spawn.expect_exact("Successfully added URI coap://example.org")
        self.spawn.sendline("uri")
        self.spawn.expect_exact("coap://example.org")
        self.spawn.sendline("uri -d")
        self.spawn.expect_exact("Successfully reset URI")
        self.spawn.sendline("uri")
        self.spawn.expect_exact("usage: uri -d")
        self.spawn.expect_exact("       uri <uri>")

    def test_creds_cmd(self):
        self.spawn.sendline("creds")
        self.spawn.expect_exact("usage: creds -d <cred_tag>")
        self.spawn.expect_exact("       creds <cred_tag> <psk_id> <psk>")
        self.spawn.sendline("creds -d")
        self.spawn.expect_exact("usage: creds -d <cred_tag>")
        self.spawn.sendline("creds -d 48970")
        self.spawn.expect_exact(
            "Successfully removed credentials with tag 48970"
        )
        self.spawn.sendline("creds 0 ni8Oozoh eiyit3iF")
        self.spawn.expect_exact("usage: creds -d <cred_tag>")
        self.spawn.expect_exact("       creds <cred_tag> <psk_id> <psk>")
        self.spawn.sendline(
            "creds 35528 Jizaef3aiX4wah0ein3yaeth9Roobieyu ohk7iZae"
        )
        self.spawn.expect_exact("PSK ID too long (max. 32 bytes allowed)")
        self.spawn.sendline(
            "creds 13998 Eiquahd8 aegheT8aeng7Oht0aecha3lohWail4iej"
        )
        self.spawn.expect_exact("PSK too long (max. 32 bytes allowed)")
        for i in range(4):
            self.spawn.sendline(
                "creds {} eWiu5Doh pahzu5Ie".format(52411 + i)
            )
            res = self.spawn.expect(
                [
                    "Successfully added creds: {}, "
                    "eWiu5Doh, pahzu5Ie".format(52411 + i),
                    # DTLS is not compiled in
                    "Operation not supported",
                    "Not supported",
                ]
            )
            if res > 0:
                return
        self.spawn.sendline("creds 8132 AhXah6gu ohDahpi9")
        self.spawn.expect(["Cannot allocate memory", "Not enough space"])
        for i in range(4):
            self.spawn.sendline("creds -d {}".format(52411 + i))
            self.spawn.expect_exact(
                "Successfully removed credentials with tag {}"
                .format(52411 + i)
            )

    def test_proxy_cmd(self):
        self.spawn.sendline("proxy")
        self.spawn.expect_exact("usage: proxy [<proxy URI>|-]")
        self.spawn.sendline("proxy coap://[2001:db8::1]/")
        self.spawn.expect(["Operation not supported", "Not supported"])
        self.spawn.sendline("proxy -")
        self.spawn.expect_exact("Successfully reset proxy URI")

    def test_resp_cmd(self):
        self._set_resp(2, 2, "abcdef0123456789abcdef0123456789abcdef")
        self._set_resp(4, 2, "abcdef")
        self._set_resp(0, 0)
        self._set_resp(2, 1, DNS())
        self._set_resp(
            2,
            1,
            DNS(
                qr=1,
                qd=[DNSQR(qname="example.org", qtype="AAAA")],
                ancount=1,  # ancount needs to be set since `an` is already encoded
                an=(
                    # already encoding
                    # [DNSRR(ttl=300, type="AAAA", rdata="2001:db8::1")]
                    # to make older scapy version on Murdock happy
                    b"\x00\x00\x1c\x00\x01\x00\x00\x01,\x00\x10 \x01\r\xb8\x00\x00"
                    b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                ),
            ),
        )
        self._set_resp(0, 0)

    def test_doc_success(self):
        self._set_resp(
            2,
            1,
            DNS(
                qr=1,
                qd=[DNSQR(qname="example.org", qtype="AAAA")],
                ancount=1,  # ancount needs to be set since `an` is already encoded
                an=(
                    # already encoding
                    # [DNSRR(ttl=300, type="AAAA", rdata="2001:db8::1")]
                    # to make older scapy version on Murdock happy
                    b"\x00\x00\x1c\x00\x01\x00\x00\x01,\x00\x10 \x01\r\xb8\x00\x00"
                    b"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01"
                ),
            ),
        )
        self.spawn.sendline("uri coap://[::1]")
        self.spawn.expect_exact("Successfully added URI coap://[::1]")
        self.spawn.sendline("query example.org")
        self.spawn.expect_exact(
            "Hostname example.org resolves to 2001:db8::1 (IPv6)"
        )
        self.spawn.sendline("query example.org inet6")
        self.spawn.expect_exact(
            "Hostname example.org resolves to 2001:db8::1 (IPv6)"
        )
        self.spawn.sendline("query example.org inet")
        self.spawn.expect_exact("Bad message")
        self._set_resp(
            2,
            1,
            DNS(
                qr=1,
                qd=[DNSQR(qname="example.org", qtype="A")],
                ancount=1,  # ancount needs to be set since `an` is already encoded
                an=(
                    # already encoding
                    # [DNSRR(ttl=300, type="A", rdata="192.0.0.1")]
                    # to make older scapy version on Murdock happy
                    b"\x00\x00\x01\x00\x01\x00\x00\x01,\x00\x04\xc0\x00\x00\x01"
                ),
            ),
        )
        self.spawn.sendline("query example.org inet")
        self.spawn.expect_exact(
            "Hostname example.org resolves to 192.0.0.1 (IPv4)"
        )
        if self.has_dns_cache():
            self.spawn.sendline("query example.org inet6")
            self.spawn.expect_exact(
                "Hostname example.org resolves to 2001:db8::1 (IPv6)"
            )
        else:
            self.spawn.sendline("query example.org inet6")
            self.spawn.expect_exact("Bad message")

    def _expect_od_dump_of(self, hexbytes):
        for i in range((len(hexbytes) // 32) + 1):
            rang = hexbytes[(i * 32) : ((i * 32) + 32)]  # noqa: E203
            od_bytes = "".join(
                [
                    "  {}{}".format(a.upper(), b.upper())
                    for a, b in zip(rang[::2], rang[1::2])
                ]
            )
            self.spawn.expect_exact("{:08x}{}".format(i * 16, od_bytes))

    def _check_resp(self, resp_dns):
        if resp_dns is None:
            resp_hex = ""
        elif isinstance(resp_dns, str):
            resp_hex = resp_dns
        else:
            self.assertIsInstance(resp_dns, DNS)
            resp_hex = bytes(resp_dns).hex()
        self.spawn.sendline("resp")
        self._expect_od_dump_of(resp_hex)

    def _set_resp(self, resp_class, resp_detail, resp_dns=None):
        if resp_dns is None:
            resp_hex = ""
        elif isinstance(resp_dns, str):
            resp_hex = resp_dns
        else:
            self.assertIsInstance(resp_dns, DNS)
            resp_hex = bytes(resp_dns).hex()
        self.assertEqual(0, resp_class & ~0x7)
        self.assertEqual(0, resp_detail & ~0x1F)
        if resp_hex and resp_class == 2:
            MAX = 50
            self.spawn.sendline(
                "resp {} {}".format(
                    (resp_class << 5) | resp_detail,
                    resp_hex[:MAX]
                )
            )
            self.spawn.expect_exact(
                "Successfully set response with code "
                "{:d}.{:02d}".format(resp_class, resp_detail)
            )
            self._expect_od_dump_of(resp_hex[:MAX])
            while resp_hex[MAX:]:
                resp_hex = resp_hex[MAX:]
                self.spawn.sendline("resp -c {}".format(resp_hex[:MAX]))
                self.spawn.expect_exact("Successfully continued response")
                self._expect_od_dump_of(resp_hex[:MAX])
            self._check_resp(resp_dns)
        else:
            self.spawn.sendline(
                "resp {}".format((resp_class << 5) | resp_detail)
            )
            self.spawn.expect_exact(
                "Successfully set response code "
                "{:d}.{:02d}".format(resp_class, resp_detail)
            )


if __name__ == "__main__":
    unittest.main(verbosity=2)
