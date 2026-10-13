#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("result: 7")


if __name__ == "__main__":
    sys.exit(run(testfunc))
