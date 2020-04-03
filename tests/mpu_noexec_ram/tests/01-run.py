#!/usr/bin/env python3

# Copyright (C) 2020 Sören Tempel <tempel@uni-bremen.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("MEM MANAGE HANDLER\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))
