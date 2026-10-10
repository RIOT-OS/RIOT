#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    for _ in range(4):
        child.expect_exact('Logging value %d and string %s')


if __name__ == "__main__":
    sys.exit(run(testfunc))
