#!/usr/bin/env python3

# Copyright (C) 2020 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('SHA-224 test #1: passed')
    child.expect_exact('SHA-224 test #2: passed')
    child.expect_exact('SHA-224 test #3: passed')
    child.expect_exact('SHA-256 test #1: passed')
    child.expect_exact('SHA-256 test #2: passed')
    child.expect_exact('SHA-256 test #3: passed')

    # If the adc noise module is not a single entropy source, the ENTROPY test
    # should succeed. Otherwise, if the adc noise module is the only entropy
    # source, ignore a test failure which might be introduced by improver ADC
    # configurations or properties. This still allows us to execute this test
    # regardless of configuration specifics.
    child.expect(r'adc_noise_single_entropy: (\d+)\r\n')
    adc_noise_single_entropy = int(child.match.group(1))
    if adc_noise_single_entropy == 0:
        child.expect_exact('ENTROPY test: passed')


if __name__ == "__main__":
    sys.exit(run(testfunc))
