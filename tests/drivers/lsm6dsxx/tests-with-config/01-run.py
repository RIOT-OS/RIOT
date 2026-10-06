#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect("Accelerometer x:")
    child.expect("Gyroscope x:")
    child.expect(r"Temperature \[in")


if __name__ == "__main__":
    sys.exit(run(testfunc))
