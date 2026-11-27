#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    assert child.expect_exact(["SUCCESS", "FAILURE"]) == 0


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))
