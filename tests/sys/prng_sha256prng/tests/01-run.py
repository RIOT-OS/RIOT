#!/usr/bin/env python3

# Copyright (C) 2020 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect("test_prng_sha256prng_seed1_u32:SUCCESS\r\n")
    child.expect("test_prng_sha256prng_seed2_u8:SUCCESS\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
