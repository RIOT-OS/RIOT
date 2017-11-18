#!/usr/bin/env python3

# Copyright (C) 2017 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

def testfunc(child):
    child.expect_exact("Start.")
    child.expect('\+ bitarithm_msb: \d+ iterations per second')
    child.expect('\+ bitarithm_lsb: \d+ iterations per second')
    child.expect('\+ bitarithm_bits_set: \d+ iterations per second')
    child.expect_exact("Done.")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc, timeout=30))
