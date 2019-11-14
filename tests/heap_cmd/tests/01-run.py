#!/usr/bin/env python3

# Copyright (C) 2019 Gunar Schorcht <gunar@schorcht.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # check startup message
    child.expect('Shell-based test application for heap functions.')
    child.sendline('heap')
    ret = child.expect([r'heap: \d+ \(used \d+, free \d+\) \[bytes\]', 'heap statistics are not supported'])
    if ret == 1:
        return
    child.sendline('malloc 100')
    child.expect('allocated 0x')
    addr = child.readline()
    addr = addr[:-2]
    child.expect_exact('> ')
    child.sendline('heap')
    child.expect(r'heap: \d+ \(used \d+, free \d+\) \[bytes\]')
    child.sendline('free 0x' + addr)
    child.expect('freed 0x' + addr)
    child.expect_exact('>')
    child.sendline('heap')
    child.expect(r'heap: \d+ \(used \d+, free \d+\) \[bytes\]')


if __name__ == "__main__":
    sys.exit(run(testfunc))
