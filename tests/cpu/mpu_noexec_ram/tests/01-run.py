#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Sören Tempel <tempel@uni-bremen.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("MEM MANAGE HANDLER\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))
