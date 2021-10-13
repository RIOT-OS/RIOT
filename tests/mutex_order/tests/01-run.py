#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
# Copyright (C) 2016 Oliver Hahm <oliver.hahm@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

NUM_THREADS = 5


def testfunc(child):

    # First collect the thread info how they are created
    # A number of lines with:
    #  T4 (prio 6): waiting on condition variable now
    thread_prios = {}
    for nr in range(NUM_THREADS):
        child.expect(r"T(\d+) \(prio (\d+)\): trying to lock mutex now")
        thread_id = int(child.match.group(1))
        thread_prio = int(child.match.group(2))
        thread_prios[thread_id] = thread_prio

    last_prio = -1
    for _ in range(len(thread_prios)):
        child.expect(r"T(\d+) \(prio (\d+)\): locked mutex now")
        thread_id = int(child.match.group(1))
        thread_prio = int(child.match.group(2))
        assert thread_prios[thread_id] == thread_prio
        assert thread_prio > last_prio
        last_prio = thread_prio


if __name__ == "__main__":
    sys.exit(run(testfunc))
