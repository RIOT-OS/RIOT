#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


CPUID_LEN = int(os.getenv('CPUID_LEN') or 4)


def testfunc(child):
    child.expect_exact('Test for the CPUID driver')
    child.expect_exact('This test is reading out the CPUID of the platforms CPU')
    expected = 'CPUID:' + CPUID_LEN * r' 0x[0-9a-fA-F]{2}'
    child.expect(expected)


if __name__ == "__main__":
    sys.exit(run(testfunc))
