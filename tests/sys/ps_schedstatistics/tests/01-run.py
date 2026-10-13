#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run

PS_EXPECTED = (
    (r'\tpid | name                 | state    Q | pri | stack  \( used\) | '
     r'base addr  | current     | runtime  | switches'),
    (r'\t  - | isr_stack            | -        - |   - | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+'),
    (r'\t  1 | idle                 | pending  Q |  15 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t  2 | main                 | running  Q |   7 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t  3 | thread               | bl rx    _ |   6 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t  4 | thread               | bl rx    _ |   6 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t  5 | thread               | bl rx    _ |   6 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t  6 | thread               | bl mutex _ |   6 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t  7 | thread               | bl rx    _ |   6 | \d+  \( -?\d+\) | '
     r'0x\d+ | 0x\d+  | \d+\.\d+% |      \d+'),
    (r'\t    | SUM                  |            |     | \d+  \(\d+\)')
)


def _check_startup(child):
    for i in range(5):
        child.expect_exact('Creating thread #{}, next={}'
                           .format(i, (i + 1) % 5))


def _check_help(child):
    child.sendline('')
    child.expect_exact('>')
    child.sendline('help')
    child.expect_exact('Command              Description')
    child.expect_exact('---------------------------------------')
    child.expect_exact('ps                   Prints information about '
                       'running threads.')
    child.expect_exact('reboot               Reboot the node')


def _check_ps(child):
    child.sendline('ps')
    for line in PS_EXPECTED:
        child.expect(line)
    # Wait for all lines of the ps output to be displayed
    child.expect_exact('>')


def testfunc(child):
    _check_startup(child)
    _check_help(child)
    _check_ps(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))
