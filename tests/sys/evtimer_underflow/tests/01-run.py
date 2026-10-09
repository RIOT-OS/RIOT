#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run

how_many = 100


def testfunc(child):
    for i in range(how_many):
        for j in range(8):
            child.expect(r'received msg "%i"' % (j + 1))
        print(".", end="", flush=True)
    print("")
    print("Stopped after %i iterations, but should run forever." % how_many)
    print("=> All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc))
