#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import time
from testrunner import run


PRECISION = 0.05  # 5%
MAX_HELLOS = 5


def testfunc(child):
    child.expect_exact('Testing the tick conversion')
    child.expect_exact('All ok')

    child.expect_exact('Initializing the RTT driver')
    child.expect(r'This test will now display \'Hello\' every (\d+) seconds')
    period = int(child.match.group(1))
    child.expect(r'RTT now: \d+')
    child.expect(r'Setting initial alarm to now \+ {} s \(\d+\)'
                 .format(period))
    start = time.time()
    child.expect_exact('Done setting up the RTT, wait for many Hellos')
    for _ in range(MAX_HELLOS):
        child.expect_exact('Hello\r\n', timeout=period + 1)

    # Verify timings
    elapsed = time.time() - start
    assert elapsed > (MAX_HELLOS * period * (1 - PRECISION))
    assert elapsed < (MAX_HELLOS * period * (1 + PRECISION))


if __name__ == "__main__":
    sys.exit(run(testfunc))
