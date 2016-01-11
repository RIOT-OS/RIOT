#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import autotest
from autotest.default import DefaultBuildStrategy, DefaultTestStrategy

BOARDS = [autotest.Board("native")]

TEST_CASES = [
        ('bitarithm_timings', BOARDS, DefaultBuildStrategy, None),
        ('bloom_bytes', BOARDS, DefaultBuildStrategy, DefaultTestStrategy),
        ('coap', BOARDS, DefaultBuildStrategy, None),
        ('fmt_print', BOARDS, DefaultBuildStrategy, DefaultTestStrategy),
    ]

def run_tests(tests):
    for test in tests:
        tc = autotest.TestCase(test[0],
                               build_strategy=test[2](test[1]) if test[2] else None,
                               test_strategy=test[3](test[1]) if test[3] else None)
        tc.execute()

if __name__ == '__main__':
    run_tests(TEST_CASES)
