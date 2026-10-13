#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2024 Marian Buschsieweke
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Marian Buschsieweke <marian.buschsieweke@posteo.net>

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Testing snprintf() implementation...")
    child.expect_exact("Test succeeded")


if __name__ == "__main__":
    sys.exit(run(testfunc))
