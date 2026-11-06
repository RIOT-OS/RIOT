#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024-2026 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import json
import pexpect
from testrunner import run


def verify_sample_rate(data: dict):
    """Verify the sample rate"""
    expect_rate = data["configured_sample_rate_in_Hz"]
    meas_rate = data["measured_sample_rate_in_Hz"]
    error = abs(meas_rate - expect_rate)

    tol = max([50, 0.02 * expect_rate])
    assert error < tol, (
        f"configured sample rate is {expect_rate} Hz, "
        f"got {meas_rate} Hz, "
        f"{error} is out of tolerance {tol} Hz."
    )


def sample_rate_check_until_start(child: pexpect.spawn):
    """Expect JSON sample-rate objects until the PASS message."""
    at_least_one_sample_rate = False
    while True:
        idx = child.expect([r'\{[^\n]*\}\r\n', r'PASS\r\n'])
        if idx == 1:
            assert at_least_one_sample_rate, (
                "no sample rate was reported before recording started."
            )
            break
        data = json.loads(child.match.group(0))
        verify_sample_rate(data)
        at_least_one_sample_rate = True


def check_recording_for_a_change(child: pexpect.spawn):
    child.expect("RECORDING STOP")
    child.expect(r'\{[^\n]*\}\r\n')
    data = json.loads(child.match.group(0))
    samples = data["samples"]
    for i in range(1, len(samples)):
        if samples[i] != samples[i - 1]:
            break
    else:
        raise AssertionError(
            f"samples are not changing and fixed at {samples[0]}."
            "That seems a little bit strange... look into it!"
        )


def testfunc(child: pexpect.spawn):
    check_recording_for_a_change(child)
    sample_rate_check_until_start(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))
