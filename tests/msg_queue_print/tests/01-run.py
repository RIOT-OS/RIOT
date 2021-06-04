#!/usr/bin/env python3

#  Copyright (C) 2021 Freie Universität Berlin,
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>

import os
import sys
from testrunner import run


def testfunc(child):
    child.expect("No message queue")
    child.expect(r"Message queue of thread \d+\r\n")
    child.expect_exact('size: 8 (avail: 0)')
    child.expect(r"Message queue of thread \d+\r\n")
    child.expect_exact('size: 8 (avail: 8)')
    if os.environ.get('BOARD') == 'native':
        child.expect_exact('type: 0x0000, content: 0 ((nil))')
    else:
        child.expect(r'type: 0x0000, content: 0 \((0x)?0+\)')
    child.expect_exact('type: 0x0001, content: 1 (0x1)')
    child.expect_exact('type: 0x0002, content: 2 (0x2)')
    child.expect_exact('type: 0x0003, content: 3 (0x3)')
    child.expect_exact('type: 0x0004, content: 4 (0x4)')
    child.expect_exact('type: 0x0005, content: 5 (0x5)')
    child.expect_exact('type: 0x0006, content: 6 (0x6)')
    child.expect_exact('type: 0x0007, content: 7 (0x7)')
    child.expect_exact('DONE')


if __name__ == "__main__":
    sys.exit(run(testfunc))
