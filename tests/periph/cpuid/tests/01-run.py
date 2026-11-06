#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('Test for the CPUID driver')
    child.expect_exact('This test is reading out the CPUID of the platforms CPU')
    child.expect(r'CPUID_LEN: (\d+)\r\n')
    cpuid_len = int(child.match.group(1))
    child.expect(r'CPUID:( 0x[0-9a-fA-F]{2})+\s*\r\n')
    assert child.match.group(0).count(' 0x') == cpuid_len


if __name__ == "__main__":
    sys.exit(run(testfunc))
