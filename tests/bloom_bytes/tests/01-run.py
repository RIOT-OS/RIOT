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
    child.expect_exact("Testing Bloom filter.")
    child.expect_exact("m: 4096 k: 8")
    child.expect("adding 512 elements took \d+ms")
    child.expect("checking 10000 elements took \d+ms")
    child.expect("\d+ elements probably in the filter.")
    child.expect("\d+ elements not in the filter.")
    child.expect(".+ false positive rate.")
    child.expect_exact("All done!")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
