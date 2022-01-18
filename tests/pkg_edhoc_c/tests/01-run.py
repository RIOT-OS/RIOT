#!/usr/bin/env python3

# Copyright (C) 2021 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from testrunner import run

# Default COAP port on which the edhoc responder is running
COAP_PORT = int(os.getenv("COAP_PORT", "5683"))

LAKE_WG_EDHOC_TV_34900_MSG1 = \
    "0x0d 0x00 0x58 0x20 0x8d 0x3e 0xf5 0x6d\n" + \
    "0x1b 0x75 0x0a 0x43 0x51 0xd6 0x8a 0xc2\n" + \
    "0x50 0xa0 0xe8 0x83 0x79 0x0e 0xfc 0x80\n" + \
    "0xa5 0x38 0xa4 0x44 0xee 0x9e 0x2b 0x57\n" + \
    "0xe2 0x44 0x1a 0x7c 0x21"

LAKE_WG_EDHOC_TV_34900_MSG2 = \
    "0x58 0x20 0x52 0xfb 0xa0 0xbd 0xc8 0xd9\n" + \
    "0x53 0xdd 0x86 0xce 0x1a 0xb2 0xfd 0x7c\n" + \
    "0x05 0xa4 0x65 0x8c 0x7c 0x30 0xaf 0xdb\n" + \
    "0xfc 0x33 0x01 0x04 0x70 0x69 0x45 0x1b\n" + \
    "0xaf 0x35 0x37 0x4a 0xa3 0xf1 0xbd 0x5d\n" + \
    "0x02 0x8d 0x19 0xcf 0x3c 0x99"

LAKE_WG_EDHOC_TV_34900_MSG3 = \
    "0x37 0x52 0xd5 0x53 0x5f 0x31 0x47 0xe8\n" + \
    "0x5f 0x1c 0xfa 0xcd 0x9e 0x78 0xab 0xf9\n" + \
    "0xe0 0xa8 0x1b 0xbf"

LAKE_WG_EDHOC_TV_34900_TH4 = \
    "0x7c 0xcf 0xde 0xdc 0x2c 0x10 0xca 0x03\n" + \
    "0x56 0xe9 0x57 0xb9 0xf6 0xa5 0x92 0xe0\n" + \
    "0xfa 0x74 0xdb 0x2a 0xb5 0x4f 0x59 0x24\n" + \
    "0x40 0x96 0xf9 0xa2 0xac 0x56 0xd2 0x07"

LAKE_WG_EDHOC_TV_34900_OSCORE_SECRET = \
    "0x5b 0xb2 0xae 0xe2 0x5b 0x16 0x0e 0x7c\n" + \
    "0x6d 0x26 0x12 0xb0 0xa6 0x01 0x09 0x16"

LAKE_WG_EDHOC_TV_34900_OSCORE_SALT = \
    "0x8e 0x44 0x92 0x10 0xe0 0x3b 0xc2 0x9d"


def get_ipv6_addr_and_netif(child):
    child.expect_exact('>')
    child.sendline('ifconfig')
    # Get device local address
    child.expect(r"Iface\s+(?P<netif>\d+)\s+")
    netif = child.match.group("netif")
    # Get device local address
    child.expect(
        r"inet6\s+addr:\s+(?P<lladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)"
        "  scope: link  VAL"
    )
    addr = child.match.group("lladdr").lower()
    return addr, netif


def testfunc(child):
    addr, netif = get_ipv6_addr_and_netif(child)
    child.sendline("init handshake {}%{} {}".format(addr, netif, COAP_PORT))
    child.expect_exact("[initiator]: sending msg1 (37 bytes):")
    for line in LAKE_WG_EDHOC_TV_34900_MSG1.split('\n'):
        child.expect_exact(line)
    child.expect_exact("[responder]: received an EDHOC message (len 37):")
    child.expect_exact("[responder]: sending msg2 (46 bytes):")
    for line in LAKE_WG_EDHOC_TV_34900_MSG2.split('\n'):
        child.expect_exact(line)
    child.expect_exact("[initiator]: received a message (46 bytes):")
    child.expect_exact("[initiator]: sending msg3 (20 bytes)")
    for line in LAKE_WG_EDHOC_TV_34900_MSG3.split('\n'):
        child.expect_exact(line)
    child.expect_exact("[responder]: finalize exchange")
    child.expect_exact("[responder]: handshake successfully completed")
    child.expect_exact("[initiator]: handshake successfully completed")
    child.expect_exact("[initiator]: Transcript hash 4 (32 bytes):")
    for line in LAKE_WG_EDHOC_TV_34900_TH4.split('\n'):
        child.expect_exact(line)
    child.sendline("init oscore")
    child.expect_exact("OSCORE secret:")
    for line in LAKE_WG_EDHOC_TV_34900_OSCORE_SECRET.split('\n'):
        child.expect_exact(line)
    child.expect_exact("OSCORE salt:")
    child.expect_exact(LAKE_WG_EDHOC_TV_34900_OSCORE_SALT)
    child.sendline("resp oscore")
    child.expect_exact("OSCORE secret:")
    for line in LAKE_WG_EDHOC_TV_34900_OSCORE_SECRET.split('\n'):
        child.expect_exact(line)
    child.expect_exact("OSCORE salt:")
    child.expect_exact(LAKE_WG_EDHOC_TV_34900_OSCORE_SALT)


if __name__ == "__main__":
    sys.exit(run(testfunc))
