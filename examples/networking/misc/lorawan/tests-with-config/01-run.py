#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import time

from testrunner import run


def testfunc(child):
    child.expect_exact("LoRaWAN Class A low-power application")
    child.expect_exact("Starting join procedure")
    child.expect_exact("Join procedure succeeded")

    for _ in range(0, 5):
        child.expect_exact("Sending: This is RIOT!")
        time.sleep(20)

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc))
