#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import os
from testrunner import run


EXPECTED_HELP = (
    'Command              Description',
    '---------------------------------------',
    'start_test           starts a test',
    'end_test             ends a test',
    'echo                 prints the input command',
    'reboot               Reboot the node',
    'ps                   Prints information about running threads.'
)

EXPECTED_PS = (
    '\tpid | state    Q | pri',
    '\t  1 | pending  Q |  15',
    '\t  2 | running  Q |   7'
)

# In native we are directly executing the binary (no terminal program). We must
# therefore use Ctrl-V (DLE or "data link escape") before Ctrl-C to send a
# literal ETX instead of SIGINT.
# When using a board (with miniterm.py) it is not a problem.

if os.environ['BOARD'] == 'native':
    DLE = '\x16'
else:
    DLE = ''

CMDS = (
    ('start_test', ('[TEST_START]')),
    ('end_test', ('[TEST_END]')),
    ('\n', ('>')),
    ('123456789012345678901234567890123456789012345678901234567890',
        ('shell: command not found: '
         '123456789012345678901234567890123456789012345678901234567890')),
    ('unknown_command', ('shell: command not found: unknown_command')),
    ('help', EXPECTED_HELP),
    ('echo a string', ('\"echo\"\"a\"\"string\"')),
    ('ps', EXPECTED_PS),
    ('reboot', ('test_shell.')),
    (DLE+'\x03', ('shell exited (1)')),
    ('echo', ('"echo"')),
    (DLE+'\x04', ('shell exited (2)'))
)


def check_cmd(child, cmd, expected):
    child.sendline(cmd)
    for line in expected:
        child.expect_exact(line)


def testfunc(child):
    # check startup message
    child.expect('test_shell.')

    # loop other defined commands and expected output
    for cmd, expected in CMDS:
        check_cmd(child, cmd, expected)

    child.expect_exact("Shell call bad command 1=1")
    child.expect_exact('"echo""one""two""three"')
    child.expect_exact("Shell call good command 1=1")

    child.sendline('reboot')
    child.expect('test_shell.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
