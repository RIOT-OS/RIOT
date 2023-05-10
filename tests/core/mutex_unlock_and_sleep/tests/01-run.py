#!/usr/bin/env python3

# Copyright (C) 2017 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    for i in range(10):
        child.expect(r"\[ALIVE\] alternated \d+k times.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
