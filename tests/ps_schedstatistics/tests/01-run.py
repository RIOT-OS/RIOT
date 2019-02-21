#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

PS_EXPECTED = (
    ('\tpid | name                 | state    Q | pri | stack  ( used) | '
     'base addr  | current     | runtime  | switches'),
    ('\t  - | isr_stack            | -        - |   - | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+'),
    ('\t  1 | idle                 | pending  Q |  15 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t  2 | main                 | running  Q |   7 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t  3 | thread               | bl rx    _ |   6 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t  4 | thread               | bl rx    _ |   6 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t  5 | thread               | bl rx    _ |   6 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t  6 | thread               | bl mutex _ |   6 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t  7 | thread               | bl rx    _ |   6 | \d+  ( -?\d+) | '
     '0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    ('\t    | SUM                  |            |     | \d+  (\d+)')
)


def _check_startup(child):
    for i in range(5):
        child.expect_exact('Creating thread #{}, next={}'
                           .format(i, (i + 1) % 5))


def _check_help(child):
    child.sendline('')
    child.expect('>')
    child.sendline('help')
    child.expect_exact('Command              Description')
    child.expect_exact('---------------------------------------')
    child.expect_exact('reboot               Reboot the node')
    child.expect_exact('ps                   Prints information about '
                       'running threads.')


def _check_ps(child):
    child.sendline('ps')
    for line in PS_EXPECTED:
        child.expect(line)


def testfunc(child):
    _check_startup(child)
    _check_help(child)
    _check_ps(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))
