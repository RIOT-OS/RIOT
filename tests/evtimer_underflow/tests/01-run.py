#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from __future__ import print_function
import os
import sys

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
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc, echo=False))
