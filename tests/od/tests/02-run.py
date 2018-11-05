#!/usr/bin/env python3

# Copyright (C) 2017 Hamburg University of Applied Sciences
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("od_hex_dump(short_str, sizeof(short_str), OD_WIDTH_DEFAULT)")
    child.expect_exact("00000000  41  42  00                                                      AB.")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), OD_WIDTH_DEFAULT)")
    child.expect_exact("00000000  FF  2C  61  FF  2E  62  63  64  65  66  67  68  69  6A  6B  6C  .,a..bcdefghijkl")
    child.expect_exact("00000010  6D  6E  6F  70  00                                              mnop.")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), 4)")
    child.expect_exact("00000000  FF  2C  61  FF  .,a.")
    child.expect_exact("00000004  2E  62  63  64  .bcd")
    child.expect_exact("00000008  65  66  67  68  efgh")
    child.expect_exact("0000000C  69  6A  6B  6C  ijkl")
    child.expect_exact("00000010  6D  6E  6F  70  mnop")
    child.expect_exact("00000014  00              .")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), 3)")
    child.expect_exact("00000000  FF  2C  61  .,a")
    child.expect_exact("00000003  FF  2E  62  ..b")
    child.expect_exact("00000006  63  64  65  cde")
    child.expect_exact("00000009  66  67  68  fgh")
    child.expect_exact("0000000C  69  6A  6B  ijk")
    child.expect_exact("0000000F  6C  6D  6E  lmn")
    child.expect_exact("00000012  6F  70  00  op.")
    child.expect_exact("od_hex_dump(long_str, sizeof(long_str), 8)")
    child.expect_exact("00000000  FF  2C  61  FF  2E  62  63  64  .,a..bcd")
    child.expect_exact("00000008  65  66  67  68  69  6A  6B  6C  efghijkl")
    child.expect_exact("00000010  6D  6E  6F  70  00              mnop.")

    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1))
