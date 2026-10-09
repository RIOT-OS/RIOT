#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("This test tests re-setting of an already active timer.")
    child.expect_exact("It should print three times \"now=<value>\", with "
                       "values approximately 100ms (100000us) apart.")
    child.expect(r"now=\d+")
    child.expect(r"now=\d+")
    child.expect(r"now=\d+")
    child.expect_exact("Test completed!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
