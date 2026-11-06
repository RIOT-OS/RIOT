#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run
from pexpect import TIMEOUT


def testfunc(child):
    res = child.expect([TIMEOUT, "Message was not written"])
    # we actually want the timeout here. The application runs into an assertion
    # pretty quickly when failing and runs forever on success
    assert (res == 0)


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))
