#!/usr/bin/env python3

# Copyright (C) 2017 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import time

HOST_JITTER = 0.1
TEST_INTERVAL_LOWER = 1 - HOST_JITTER
TEST_INTERVAL_UPPER = 1 + HOST_JITTER


class InvalidTimeout(Exception):
    pass


def testfunc(child):
    error = 0

    child.expect_exact(u"[START]")
    child.expect(u"\r\n")
    child.expect(u"now=(([0-9]*[.])?[0-9]+) .+ us\r\n")
    start = time.time()
    try:
        while 1:
            child.expect(u"now=(([0-9]*[.])?[0-9]+) .+ us\r\n")
            end = time.time()
            delta = end - start
            start = time.time()
            timestamp = float(child.match.group(1))
            if (delta < TEST_INTERVAL_LOWER) or (delta > TEST_INTERVAL_UPPER):
                error = error + 1
                print("{}: Invalid timebetween messages, expected {} < {} < {}"
                      .format(timestamp, TEST_INTERVAL_LOWER, delta, TEST_INTERVAL_UPPER))
            # if error > 20:
            #    raise InvalidTimeout("20 times Invalid time between messages")
    except InvalidTimeout as e:
        print(e)
        sys.exit(1)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
