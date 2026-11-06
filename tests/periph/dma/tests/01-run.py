#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('DMA is working')


if __name__ == "__main__":
    sys.exit(run(testfunc))
