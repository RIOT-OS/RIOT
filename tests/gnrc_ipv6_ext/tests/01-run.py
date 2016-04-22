#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Takuo Yonezawa <Yonezawa-T2@mail.dnp.co.jp>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

def testfunc(child):
    index = child.expect_exact([
        "ipv6: Received (src = fd01::1, dst = fd01::2, next header = 0, length = 40)",
        "pkt->users: 0"
    ])

    if index == 1:
        # debug is disabled
        return

    child.expect_exact("ipv6: handle extension header (nh = 0)")
    child.expect_exact("ipv6: handle extension header (nh = 43)")
    child.expect_exact("ipv6: Received (src = fd01::1, dst = fd01::3, next header = 0, length = 40)")
    child.expect_exact("ipv6: handle extension header (nh = 0)")
    child.expect_exact("ipv6: handle extension header (nh = 43)")
    child.expect_exact("ipv6: Received (src = fd01::1, dst = fd01::2, next header = 0, length = 40)")
    child.expect_exact("ipv6: handle extension header (nh = 0)")
    child.expect_exact("ipv6: handle extension header (nh = 43)")
    child.expect_exact("ipv6: forward nh = 17 to other threads")
    child.expect_exact("pkt->users: 0")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
