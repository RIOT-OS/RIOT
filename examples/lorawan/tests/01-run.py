#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
