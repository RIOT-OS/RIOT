#!/usr/bin/env python3

# Copyright (C) 2020 Benjamin Valentin <benpicco@beuth-hochschule.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import time
from testrunner import run


def testfunc(child):
    child.expect_exact('TEST START')
    start = time.time()
    child.expect_exact('TEST SUCCEEDED')
    end = time.time()
    # test should run 10 cycles with 25ms each
    assert (end - start) > 0.25
    assert (end - start) < 0.40


if __name__ == "__main__":
    sys.exit(run(testfunc))
