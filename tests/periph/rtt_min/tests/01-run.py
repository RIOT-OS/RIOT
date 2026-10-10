#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Inria
# SPDX-License-Identifier: LGPL-2.1-only

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
    test_result_ok = r'OK \d+ <= \d+'
    child.expect(test_result_ok)


if __name__ == "__main__":
    sys.exit(run(testfunc))
