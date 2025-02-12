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
        if os.environ.get('BOARD') in ['native', 'native32']:
            child.expect_exact('type: 0x0000, content: 0 ((nil))')
        else:
            child.expect(r'type: 0x0000, content: 0 \((0x)?0+\)')
    else:
        child.expect_exact(f'type: 0x{counter:04x}, content: {counter} (0x{counter:x})')


def testfunc(child):
    expect_none(child)
    expect_none(child)
    expect_some(child, 8, 8, 0)
    expect_some(child, 8, 4, 4)
    expect_some(child, 8, 8, 4)
    child.expect_exact('DONE')


if __name__ == "__main__":
    sys.exit(run(testfunc))
