#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

THREAD_PRIO = {
    3: 6,
    4: 4,
    5: 0,
    6: 2,
    7: 1
}


def testfunc(child):
    for k in THREAD_PRIO:
        child.expect("T%i \(prio %i\): trying to lock mutex now"
                     % (k, THREAD_PRIO[k]))

    last = -1
    for i in range(len(THREAD_PRIO)):
        child.expect("T\d+ \(prio (\d+)\): locked mutex now")
        assert(int(child.match.group(1)) > last)
        last = int(child.match.group(1))

if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))
