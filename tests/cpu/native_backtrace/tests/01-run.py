#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"BACKTRACE_SIZE: (\d+)\r\n")
    trace_size = int(child.match.group(1))
    child.expect("backtrace_print:")
    for i in range(trace_size):
        child.expect(r"0x[0-9a-f]+")
    child.expect("backtrace_print_symbols:")
    for i in range(trace_size):
        child.expect(r".*")

    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=True, traceback=True))
