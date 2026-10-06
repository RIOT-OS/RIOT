#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Start.")
    child.expect(r'\+ bitarithm_msb: \d+ iterations per second')
    child.expect(r'\+ bitarithm_lsb: \d+ iterations per second')
    child.expect(r'\+ bitarithm_bits_set: \d+ iterations per second')
    child.expect(r'\+ bitarithm_test_and_clear: \d+ iterations per second')
    child.expect_exact("Done.")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=30))
