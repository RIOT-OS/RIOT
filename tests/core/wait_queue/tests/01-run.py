#!/usr/bin/env python3

# Copyright (C) 2025 Mihai Renea <mihai.renea@ml-pa.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Author: Mihai Renea <mihai.renea@ml-pa.com>

import sys
from testrunner import run


def testfunc(child):
    child.expect("Test successful!")


if __name__ == "__main__":
    sys.exit(run(testfunc))
