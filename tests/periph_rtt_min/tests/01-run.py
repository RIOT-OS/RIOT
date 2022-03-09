#!/usr/bin/env python3

# Copyright (C) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys

from testrunner import run


def testfunc(child):
    child.expect(r"Evaluate RTT_MIN_OFFSET over (\d+) samples")

    exp_samples = int(child.match.group(1))
    test_end = r'RTT_MIN_OFFSET for [a-zA-Z\-\_0-9]+ over {samples} ' \
               r'samples: \d+'.format(samples=exp_samples)
    test_ongoing = r'Sample \d+'
    while child.expect([test_end, test_ongoing]):
        pass


if __name__ == "__main__":
    sys.exit(run(testfunc))
