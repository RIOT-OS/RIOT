#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


EXPECTED_HELP = (
    'Command              Description',
    '---------------------------------------',
    'start_test           starts a test',
    'end_test             ends a test',
    'echo                 prints the input command',
    'reboot               Reboot the node',
    'ps                   Prints information about running threads.',
    'app_metadata         Returns application metadata'
)

EXPECTED_PS = (
    '\tpid | state    Q | pri',
    '\t  1 | pending  Q |  15',
    '\t  2 | running  Q |   7'
)

# In native we are directly executing the binary (no terminal program). We must
# therefore use Ctrl-V (DLE or "data link escape") before Ctrl-C to send a
# literal ETX instead of SIGINT.
# When using a board it is also a problem because we are using a "user friendly"
# terminal that interprets Ctrl-C. So until we have rawterm we must also use
# ctrl-v in boards.

DLE = '\x16'

CONTROL_C = DLE+'\x03'
CONTROL_D = DLE+'\x04'

CMDS = (
    (CONTROL_C, ('>')),
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
    ('garbage1234'+CONTROL_C, ('>')),  # test cancelling a line
    ('help', EXPECTED_HELP),
    ('reboot', ('test_shell.'))
)


def check_cmd(child, cmd, expected):
    child.sendline(cmd)
    for line in expected:
        child.expect_exact(line)


def testfunc(child):
    # loop other defined commands and expected output
    for cmd, expected in CMDS:
        check_cmd(child, cmd, expected)


if __name__ == "__main__":
    sys.exit(run(testfunc))
