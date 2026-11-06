#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2025 Technische Universität Dresden
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>

import sys
from testrunner import run


def testfunc(child):
    child.expect("TEST PASSED", timeout=2)


if __name__ == "__main__":
    sys.exit(run(testfunc))
