#!/usr/bin/env python3

# Copyright (C) 2022 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

CSP_SERVER_PORT = 8
CSP_TEST_MESSAGE = "hello RIOT"


def testfunc(child):
    child.sendline(f"csp server start {CSP_SERVER_PORT}")
    child.expect(f"started server on port {CSP_SERVER_PORT}")
    child.sendline("csp")
    child.expect(r"addr:\s+(?P<addr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)\s+")
    addr = child.match.group("addr").lower()
    child.sendline(f"csp send {addr} {CSP_SERVER_PORT} '{CSP_TEST_MESSAGE}'")
    child.expect_exact(
        f"Packet {len(CSP_TEST_MESSAGE) + 1 } bytes received on port "
        f"{CSP_SERVER_PORT}: {CSP_TEST_MESSAGE}"
    )


if __name__ == "__main__":
    sys.exit(run(testfunc))
