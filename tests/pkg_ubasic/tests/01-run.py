#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    for i in range(1, 6):
        child.expect(r"Running test #{}... done. Run time: [0-9.]* s".format(i))


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=120))
