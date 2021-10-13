#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from __future__ import print_function
import sys
from testrunner import run

ACCEPTED_ERROR = 2


def testfunc(child):
    child.expect(r"Testing generic evtimer")
    child.expect(r"Are the reception times of all (\d+) msgs close to the supposed values?")
    numof = int(child.match.group(1))

    for i in range(numof):
        child.expect(r'At \s*(\d+) ms received msg %i: "#\d+ supposed to be (\d+)"' % i)
        # check if output is correct
        expected = int(child.match.group(2))
        actual = int(child.match.group(1))
        assert(actual in range(expected - ACCEPTED_ERROR, expected + ACCEPTED_ERROR + 1))
        print(".", end="", flush=True)
    print("")
    print("All tests successful")


if __name__ == "__main__":
    sys.exit(run(testfunc))
