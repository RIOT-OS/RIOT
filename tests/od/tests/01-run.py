#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("od_hex_dump(short_str, sizeof(short_str), OD_WIDTH_DEFAULT)")
    child.expect_exact("00000000  41  42  00")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), OD_WIDTH_DEFAULT)")
    child.expect_exact("00000000  FF  2C  61  FF  2E  62  63  64  65  66  67  68  69  6A  6B  6C")
    child.expect_exact("00000010  6D  6E  6F  70  00")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), 4)")
    child.expect_exact("00000000  FF  2C  61  FF")
    child.expect_exact("00000004  2E  62  63  64")
    child.expect_exact("00000008  65  66  67  68")
    child.expect_exact("0000000C  69  6A  6B  6C")
    child.expect_exact("00000010  6D  6E  6F  70")
    child.expect_exact("00000014  00")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), 3)")
    child.expect_exact("00000000  FF  2C  61")
    child.expect_exact("00000003  FF  2E  62")
    child.expect_exact("00000006  63  64  65")
    child.expect_exact("00000009  66  67  68")
    child.expect_exact("0000000C  69  6A  6B")
    child.expect_exact("0000000F  6C  6D  6E")
    child.expect_exact("00000012  6F  70  00")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), 8)")
    child.expect_exact("00000000  FF  2C  61  FF  2E  62  63  64")
    child.expect_exact("00000008  65  66  67  68  69  6A  6B  6C")
    child.expect_exact("00000010  6D  6E  6F  70  00")

    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1))
