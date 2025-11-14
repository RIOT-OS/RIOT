#!/usr/bin/env python3

#  Copyright (C) 2025 Technische Universität Dresden
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>

import sys
from testrunner import run


def testfunc(child):
    child.expect("TEST PASSED", timeout=2)


if __name__ == "__main__":
    sys.exit(run(testfunc))
