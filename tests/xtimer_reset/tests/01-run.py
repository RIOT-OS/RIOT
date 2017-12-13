#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact("This test tests re-setting of an already active timer.")
    child.expect_exact("It should print three times \"now=<value>\", with "
                       "values approximately 100ms (100000us) apart.")
    child.expect(u"now=\d+")
    child.expect(u"now=\d+")
    child.expect(u"now=\d+")
    child.expect_exact("Test completed!")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
