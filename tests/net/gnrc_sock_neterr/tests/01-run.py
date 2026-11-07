#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"SUCCESS: error code EHOSTUNREACH \((\d+) == (\d+)\)")
    assert child.match.group(1) == child.match.group(2)
    child.expect(r"SUCCESS: error code ENETUNREACH \((\d+) == (\d+)\)")
    assert child.match.group(1) == child.match.group(2)


if __name__ == "__main__":
    sys.exit(run(testfunc))
