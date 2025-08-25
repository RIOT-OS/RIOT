#!/usr/bin/env python3

#  Copyright (C) 2021 Freie Universität Berlin
#  Copyright (C) 2024 TU Dresden
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Julian Holzwarth <julian.holzwarth@fu-berlin.de>
# @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>

import os
import sys
from testrunner import run

PRINT_MAX = int(os.getenv('CONFIG_MSG_QUEUE_PRINT_MAX'))


def expect_none(child):
    child.expect("No messages or no message queue")


def expect_some(child, size, avail, range_start):
    child.expect(r"Message queue of thread \d+\r\n")
    child.expect_exact(f'size: {size} (avail: {avail})')

    expect_less = avail > PRINT_MAX

    if expect_less:
        range_end = range_start + PRINT_MAX
    else:
        range_end = range_start + avail

    for counter in range(range_start, range_end):
        expect_content(child, counter)

    if expect_less:
        child.expect('...')


def expect_content(child, counter):
    if counter == 0:
        child.expect(r'type: 0x0000, content: 0 \(\(nil\)|(0x)?0+\)')
    else:
        child.expect(r'type: (0x[a-fA-F0-9]+), content: (\d+) \(((0x)?[a-fA-Z0-9]+)\)')
        assert int(child.match.group(1), 16) == counter, f"Expected {counter:x} as type, got child.match.group(1)"
        assert int(child.match.group(2)) == counter, f"Expected {counter} as content, got child.match.group(2)"
        assert int(child.match.group(3), 16) == counter, f"Expected {counter:x} as ptr, got child.match.group(3)"


def testfunc(child):
    expect_none(child)
    expect_none(child)
    expect_some(child, 8, 8, 0)
    expect_some(child, 8, 4, 4)
    expect_some(child, 8, 8, 4)
    child.expect_exact('DONE')


if __name__ == "__main__":
    sys.exit(run(testfunc))
