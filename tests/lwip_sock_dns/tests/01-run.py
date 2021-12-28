#!/usr/bin/env python3

# Copyright (C) 2022 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from time import sleep
from testrunner import run


def _test_dns_requests(child):
    child.sendline('dns request 0 example.com')
    child.expect_exact('example.com resolves to 2606:2800:220:1:248:1893:25c8:1946')

    child.sendline('dns request 4 example.com')
    child.expect_exact('example.com resolves to 93.184.216.34')

    child.sendline('dns request 6 example.com')
    child.expect_exact('example.com resolves to 2606:2800:220:1:248:1893:25c8:1946')


def test_no_dns_server(child):
    child.sendline('dns server')
    child.expect_exact('DNS server: -')


def test_invalid_ipv4(child):
    child.sendline('dns server 4 1.2.3.4.5')
    child.expect_exact('invalid IPv4 address')


def test_invalid_ipv6(child):
    child.sendline('dns server 6 1:2:3:4:5:6:7:8:9')
    child.expect_exact('invalid IPv6 address')


def test_ipv4_dns_server(child):
    child.sendline('dns server 4 8.8.8.8')
    child.sendline('dns server')
    child.expect_exact('DNS server: 8.8.8.8:53')
    _test_dns_requests(child)


def test_ipv6_dns_server(child):
    child.sendline('dns server 6 2001:4860:4860::8888')
    child.sendline('dns server')
    child.expect_exact('DNS server: [2001:4860:4860::8888]:53')
    _test_dns_requests(child)


def testfunc(child):
    # wait until connection has been established
    sleep(5)

    test_no_dns_server(child)
    test_invalid_ipv4(child)
    test_invalid_ipv6(child)
    test_ipv4_dns_server(child)
    test_ipv6_dns_server(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))
