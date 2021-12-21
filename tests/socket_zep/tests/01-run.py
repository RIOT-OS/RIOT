#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import socket
from testrunner import run


IEEE802154_FRAME_LEN_MAX = 127
ZEP_DATA_HEADER_SIZE = 32
FCS_LEN = 2
RCVBUF_LEN = IEEE802154_FRAME_LEN_MAX + ZEP_DATA_HEADER_SIZE + FCS_LEN
zep_params = {
        "local_addr": "127.0.0.1",
        "local_port": 12345,
        "remote_addr": "127.0.0.1",
        "remote_port": 17754,
    }
s = None


def testfunc(child):
    child.expect_exact("Socket ZEP device driver test")
    child.expect(r"Initializing socket ZEP with " +
                 r"\(local: \[(?P<local_addr>[.0-9a-f]+)\]:(?P<local_port>\d+), " +
                 r"remote: \[(?P<remote_addr>[.0-9a-f]+)\]:(?P<remote_port>\d+)\)")
    assert(child.match.group('local_addr') == zep_params['local_addr'])
    assert(int(child.match.group('local_port')) == zep_params['local_port'])
    assert(child.match.group('remote_addr') == zep_params['remote_addr'])
    assert(int(child.match.group('remote_port')) == zep_params['remote_port'])
    child.expect(r"\(Hwaddrs: (?P<short_addr>[0-9a-f]{4}), (?P<long_addr>[0-9a-f]{16})\)")
    child.expect_exact("Send 'Hello\\0World\\0'")
    data, addr = s.recvfrom(RCVBUF_LEN)
    assert(len(data) == (ZEP_DATA_HEADER_SIZE + len("Hello\0World\0") + FCS_LEN))
    assert(b"Hello\0World\0" == data[ZEP_DATA_HEADER_SIZE:-2])
    child.expect_exact("Waiting for an incoming message (use `make test`)")
    s.sendto(b"\x45\x58\x02\x01\x1a\x44\xe0\x01\xff\xdb\xde\xa6\x1a\x00\x8b" +
             b"\xfd\xae\x60\xd3\x21\xf1\x00\x00\x00\x00\x00\x00\x00\x00\x00" +
             b"\x00\x22\x41\xdc\x02\x23\x00\x38\x30\x00\x0a\x50\x45\x5a\x00" +
             b"\x5b\x45\x00\x0a\x50\x45\x5a\x00Hello World\x3a\xf2",
             ("127.0.0.1", zep_params['local_port']))
    child.expect(r"RSSI: \d+, LQI: \d+, Data:")
    child.expect_exact(r"00000000  41  DC  02  23  00  38  30  00  0A  50  45  5A  00  5B  45  00")
    child.expect_exact(r"00000010  0A  50  45  5A  00  48  65  6C  6C  6F  20  57  6F  72  6C  64")


if __name__ == "__main__":
    os.environ['TERMFLAGS'] = "--eui64=00:5a:45:50:0a:00:30:38 -z [%s]:%d,[%s]:%d" % (
            zep_params['local_addr'], zep_params['local_port'],
            zep_params['remote_addr'], zep_params['remote_port'])
    s = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
    s.bind(("127.0.0.1", zep_params['remote_port']))
    res = run(testfunc, timeout=1, echo=True, traceback=True)
    s.close()
    if (res == 0):
        print("Run tests successful")
    else:
        print("Run tests failed")
    sys.exit(res)
