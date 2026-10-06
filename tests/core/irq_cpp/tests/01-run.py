#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Jens Wetterich <jens@wetterich-net.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Suite irq completed: SUCCESS")
    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=True, traceback=True))
