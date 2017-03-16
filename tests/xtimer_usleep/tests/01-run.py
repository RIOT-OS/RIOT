#!/usr/bin/env python

# Copyright (C) 2017 Francisco Acosta <francisco.acosta@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

from datetime import datetime

class InvalidTimeout(Exception):
    pass

def testfunc(child):
    exp_diff1 = 1000000
    exp_diff5 = 5000000
    exp_diff10 = 10000000
    child.expect(u"This test will print \"Slept for X sec...\" every 1, 5 and 10 seconds.\r\n")
    child.expect(u"\r\n")
    child.expect(u"<======== If using pyterm, this is the time when started.")
    child.expect(u"\r\n")
    m = 9
    while (m):
        n = 3
        while (n):
            if n == 3:
                exp_diff = exp_diff1
            if n == 2:
                exp_diff = exp_diff5
            elif n == 1:
                exp_diff = exp_diff10
            start = datetime.now()
            child.expect(u"Slept for \\d+ sec...", timeout=11)
            stop = datetime.now()
            diff = (stop - start)
            diff = (diff.seconds * 1000000) + diff.microseconds
            # fail within 5% of expected
            if diff > (exp_diff + (exp_diff1 * 0.05)) or \
               diff < (exp_diff - (exp_diff1 * 0.05)):
                raise InvalidTimeout("Invalid timeout %d (expected %d)" % (diff, exp_diff));
            else:
                print("Timed out correctly: %d (expected %d)" % (diff, exp_diff))
            n = n - 1
        m = m -1

    child.expect(u"Test end.", timeout=15)

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
