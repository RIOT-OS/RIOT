#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Found: bar.foo -> xyz")


if __name__ == "__main__":
    sys.exit(run(testfunc))
