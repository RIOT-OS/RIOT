#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"\[(SUCCESS|FAILED)\]", timeout=30)
    assert child.match.group(1) == "SUCCESS", "test failed, see output above"


if __name__ == "__main__":
    sys.exit(run(testfunc))
