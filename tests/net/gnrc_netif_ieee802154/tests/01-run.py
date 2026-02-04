#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import socket
from testrunner import run
from pexpect import EOF, TIMEOUT
from scapy.all import ZEP2, Dot15d4Data, Dot15d4FCS, raw

ZEP_V2_VERSION = 2
ZEP_V2_TYPE_DATA = 1


def get_random_free_port() -> int:
    # we let the OS determine a free port and then return it
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind(("127.0.0.1", 0))
        # quickly reuse the port after the return
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        return s.getsockname()[1]


local_port = get_random_free_port()
remote_port = get_random_free_port()


def testfunc(child):
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as s:
        s.bind(("", local_port))
        child.expect(r"l2_addr: ([0-9A-F:]+)\r\n")
        dst = int(child.match.group(1).replace(':', ''), base=16)

        # first send valid packet to check if communication is set up correctly
        # generated with udp send <link-local> 61616 0 in tests/net/gnrc_udp
        payload = b"\x7e\x33\xf3\x00"
        payload = (
            Dot15d4FCS(fcf_srcaddrmode=2, fcf_panidcompress=True,
                       fcf_frametype="Data") /
            Dot15d4Data(dest_addr=dst, dest_panid=0x23, src_addr=dst-1) /
            payload
        )
        packet = raw(
            ZEP2(ver=ZEP_V2_VERSION, type=ZEP_V2_TYPE_DATA, channel=26,
                 length=len(payload)) / payload
        )
        s.sendto(packet, ("localhost", remote_port))
        child.expect("PKTDUMP: data received:")
        child.expect("00000000  7E  33  F3  00")
        child.expect("~~ PKT    -  2 snips, total size:  16 byte")
        payload = (
            Dot15d4FCS(fcf_srcaddrmode=2, fcf_panidcompress=True,
                       fcf_frametype="Data") /
            # just append destination PAN ID and address
            bytes([0x23, 0x00, dst & 0xff, dst >> 8])
        )
        packet = raw(
            ZEP2(ver=ZEP_V2_VERSION, type=ZEP_V2_TYPE_DATA, channel=26,
                 length=len(payload)) / payload
        )
        s.sendto(packet, ("localhost", remote_port))
        res = child.expect([TIMEOUT, EOF, "PKTDUMP: data received:"])
        # we actually want the timeout here. The application either
        # receives a bogus packet or crashes in an error case case
        assert res == 0


if __name__ == "__main__":
    os.environ['TERMFLAGS'] = '-z "0.0.0.0:%d,localhost:%d"' % (remote_port, local_port)
    sys.exit(run(testfunc, timeout=1))
