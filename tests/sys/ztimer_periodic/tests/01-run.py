#!/usr/bin/env python3

# Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Test successful.")


if __name__ == "__main__":
    sys.exit(run(testfunc))
