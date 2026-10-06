#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin,
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect("OK")
    child.expect("OK")
    child.expect("OK")
    child.expect("OK")
    child.expect("OK")
    child.expect("OK")
    child.expect("OK")


if __name__ == "__main__":
    sys.exit(run(testfunc))
