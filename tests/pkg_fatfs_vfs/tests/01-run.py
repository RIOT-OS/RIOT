#!/usr/bin/env python3

# Copyright (C) 2017 HAW-Hamburg.de
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


class TestFailed(Exception):
    pass


def testfunc(child):

    child.expect_exact("Tests for FatFs over VFS - test results will be printed"
                       " in the format test_name:result\r\n")

    while True:
        res = child.expect([r"[^\n]*:\[OK\]\r\n",
                            r"Test end.\r\n",
                            r".[^\n]*:\[FAILED\]\r\n",
                            r".*\r\n"])
        if res > 1:
            raise TestFailed(child.after.split(':', 1)[0] + " test failed!")
        elif res == 1:
            break


if __name__ == "__main__":
    sys.exit(run(testfunc))
