#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Test for the CPUID driver')
    child.expect_exact('This test is reading out the CPUID of the platforms CPU')
    child.expect(r'CPUID_LEN: (\d+)\r\n')
    cpuid_len = int(child.match.group(1))
    child.expect(r'{ \"data\": \[( \d*,)* \d* \] }')
    assert child.match.group(0).count(',') == cpuid_len - 1
    child.expect(r'{ \"result\": \"Success\" }')


if __name__ == "__main__":
    sys.exit(run(testfunc))
