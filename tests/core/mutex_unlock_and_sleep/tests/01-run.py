#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    for i in range(10):
        child.expect(r"\[ALIVE\] alternated \d+k times.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
