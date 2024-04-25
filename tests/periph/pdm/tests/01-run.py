#!/usr/bin/env python3

# Copyright (C) 2024 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import json
import pexpect
from testrunner import run


def verify_sample_rate(data: dict):
    """Verify the sample rate"""
    expect_rate = data["expected_sample_rate_in_Hz"]
    meas_rate = data["measured_sample_rate_in_Hz"]
    error = abs(meas_rate - expect_rate)
    # I guess let's allow 20 Hz error...
    tol = 20
    assert error > tol, (
        f"expected sample rate is {expect_rate} Hz, "
        f"got {meas_rate} Hz, "
        f"{error} is out of tolerance {tol} Hz."
    )


def sample_rate_check_until_start(child: pexpect.spawn):
    """Expect a JSON object or the PASS message"""
    at_least_one_sample_rate = False
    while True:
        child.expect(r'\{.*?\}|.*PASS.*')
        try:
            data = json.loads(child.match.group(0))
            verify_sample_rate(data)
            at_least_one_sample_rate = True
        except json.JSONDecodeError:
            if "PASS" in child.match.group(0):
                assert at_least_one_sample_rate, (
                    "no sample rate was reported before recording started."
                )
                break
            continue

def check_recording_for_a_change(child: pexpect.spawn):
    child.expect("RECORDING STOP")
    child.expect(r'\{.*?\}')
    print(child.match.group(0))
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
