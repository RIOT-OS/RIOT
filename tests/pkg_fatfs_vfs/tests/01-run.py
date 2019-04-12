#!/usr/bin/env python3

# Copyright (C) 2017 HAW-Hamburg.de
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

def testfunc(child):

    child.expect(u"Tests for FatFs over VFS - test results will be printed in "
                 "the format test_name:result\r\n")

    child.expect([u"[^\n]*:\[OK\]\r\n",
                  u"Test end.\r\n",
                  u".[^\n]*:\[FAILED\]\r\n",
                  u".*\r\n"])

if __name__ == "__main__":
    sys.exit(run(testfunc))
