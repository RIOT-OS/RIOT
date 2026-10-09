#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect("test_prng_sha256prng_seed1_u32:SUCCESS\r\n")
    child.expect("test_prng_sha256prng_seed2_u8:SUCCESS\r\n")


if __name__ == "__main__":
    sys.exit(run(testfunc))
