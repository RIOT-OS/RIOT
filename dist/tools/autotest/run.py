#!/usr/bin/env python
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

def run_test(name, boards, build_strategy = None, test_strategy = None):
    tc = autotest.TestCase(name, build_strategy(boards) if build_strategy else None,
                           test_strategy(boards) if test_strategy else None)
    tc.execute()

def run_tests(tests):
    for test in tests:
        run_test(*test)

if __name__ == '__main__':
    run_tests(TEST_CASES)
