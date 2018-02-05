#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from __future__ import print_function
import os
import sys

ACCEPTED_ERROR = 20


def testfunc(child):
    child.expect(r"Testing generic evtimer \(start time = (\d+) ms\)")
    timer_offset = int(child.match.group(1))
    child.expect(r"Are the reception times of all (\d+) msgs close to the supposed values?")
    numof = int(child.match.group(1))

    for i in range(numof):
        child.expect(r'At \s*(\d+) ms received msg %i: "supposed to be (\d+)"' % i)
        # check if output is correct
        exp = int(child.match.group(2)) + timer_offset
        assert(int(child.match.group(1)) in range(exp - ACCEPTED_ERROR, exp + ACCEPTED_ERROR + 1))
        print(".", end="", flush=True)
    print("")
    print("All tests successful")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc, echo=False))
