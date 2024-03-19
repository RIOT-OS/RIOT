#!/usr/bin/env python3

# Copyright (C) 2023 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


ERROR_MS = 50  # This is large enough to pass the test on IoT-LAB nrf52dk


def testfunc(child):
    child.expect(r"Testing gnrc_mac timeout module \(start time = (\d+) ms\)")
    start = int(child.match.group(1))
    child.expect(r"Set timeout_1, should be expired at (\d+) ms\)")
    timeout_1 = int(child.match.group(1))
    child.expect(r"Set timeout_2, should be expired at (\d+) ms\)")
    timeout_2 = int(child.match.group(1))
    child.expect(r"Set timeout_3, should be expired at (\d+) ms\)")
    timeout_3 = int(child.match.group(1))
    child.expect_exact("Are the reception times of all 3 msgs close to the supposed values?")

    child.expect_exact("If yes, the tests were successful")
    child.expect(
        r"At   (\d+) ms received msg 1: "
        r"timeout_1 \(set at {start} ms\) expired, supposed to be {timeout_1} ms\!"
        .format(start=start, timeout_1=timeout_1)
    )
    timeout_1_measured = int(child.match.group(1))
    assert timeout_1_measured - timeout_1 < ERROR_MS
    child.expect_exact(f"At   {timeout_1_measured} ms: timeout_1 is not running.")
    child.expect_exact(f"At   {timeout_1_measured} ms: timeout_2 is running.")
    child.expect_exact(f"At   {timeout_1_measured} ms: timeout_3 is running.")
    child.expect(
        r"At   (\d+) ms received msg 2: "
        r"timeout_2 \(set at {start} ms\) expired, supposed to be {timeout_2} ms\!"
        .format(start=start, timeout_2=timeout_2)
    )
    timeout_2_measured = int(child.match.group(1))
    assert timeout_2_measured - timeout_2 < ERROR_MS
    child.expect_exact(f"At   {timeout_2_measured} ms: timeout_1 is not running.")
    child.expect_exact(f"At   {timeout_2_measured} ms: timeout_2 is not running.")
    child.expect_exact(f"At   {timeout_2_measured} ms: timeout_3 is running.")
    child.expect(
        r"At   (\d+) ms received msg 3: "
        r"timeout_3 \(set at {start} ms\) expired, supposed to be {timeout_3} ms\!"
        .format(start=start, timeout_3=timeout_3)
    )
    timeout_3_measured = int(child.match.group(1))
    assert timeout_3_measured - timeout_3 < ERROR_MS
    child.expect_exact(f"At   {timeout_3_measured} ms: timeout_1 is not running.")
    child.expect_exact(f"At   {timeout_3_measured} ms: timeout_2 is not running.")
    child.expect_exact(f"At   {timeout_3_measured} ms: timeout_3 is not running.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
