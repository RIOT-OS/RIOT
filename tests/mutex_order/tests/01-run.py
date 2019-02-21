#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


thread_prio = {
        3:  6,
        4:  4,
        5:  0,
        6:  2,
        7:  1
        }


def testfunc(child):
    for k in thread_prio.keys():
        child.expect(u"T%i \(prio %i\): trying to lock mutex now" % (k, thread_prio[k]))

    last = -1
    for i in range(len(thread_prio)):
        child.expect(u"T\d+ \(prio (\d+)\): locked mutex now")
        assert(int(child.match.group(1)) > last)
        last = int(child.match.group(1))


if __name__ == "__main__":
    sys.exit(run(testfunc))
