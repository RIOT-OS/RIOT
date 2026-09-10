#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run
from pexpect import TIMEOUT


def testfunc(child):
    res = child.expect([TIMEOUT, "Unexpected payload. This test failed!"])
    # we actually want the timeout here. The application runs into an assertion
    # pretty quickly when failing and runs forever on success
    assert(res == 0)


if __name__ == "__main__":
    sys.exit(run(testfunc, echo=True, timeout=10))
