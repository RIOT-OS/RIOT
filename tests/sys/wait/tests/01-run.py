#!/usr/bin/env python3

#  Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
import time
from testrunner import run


def testfunc(child):
    child.expect("Test Application for sys/wait")
    child.expect(r"Waiting: (\d+) s")
    start = time.time()
    expected = int(child.match.group(1))
    child.expect("Done")
    elapsed = time.time() - start
    assert elapsed > expected

    child.expect(r"Waiting: (\d+) ms")
    start = time.time()
    expected = int(child.match.group(1))
    child.expect("Done")
    elapsed = time.time() - start
    assert elapsed > expected / 1000

    child.expect(r"Waiting: (\d+) us")
    start = time.time()
    expected = int(child.match.group(1))
    child.expect("Done")
    elapsed = time.time() - start
    assert elapsed > expected / 1000000


if __name__ == "__main__":
    sys.exit(run(testfunc))
