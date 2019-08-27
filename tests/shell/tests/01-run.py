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
    ('help', EXPECTED_HELP),
    ('reboot', ('test_shell.'))
)

PROMPT = '> '

ON_NATIVE = os.environ['BOARD'] == 'native'


def check_cmd(child, cmd, expected):
    child.expect(PROMPT)
    child.sendline(cmd)
    for line in expected:
        child.expect_exact(line)


def testfunc(child):
    # Avoid sending en extra empty line on native.
    if ON_NATIVE:
        child.crlf = '\n'

    # check startup message
    child.expect('test_shell.\r\n')
    child.sendline('')

    child.sendline('bufsize')
    child.expect('([0-9]+)\r\n')

    bufsize = int(child.match[1])

    child.expect(PROMPT)

    # check a long line
    longline = "_"*bufsize + "verylong"
    if ON_NATIVE:
        child.sendline(longline)
    else:
        # this is dirty hack to work around a bug in the uart (#10634)
        for c in longline:
            child.write(c)
            child.flush()
        child.sendline()
        child.sendline()

    child.expect('shell: maximum line length exceeded')

    # test cancelling a line
    child.expect(PROMPT)
    child.sendline('garbage1234'+CONTROL_C)
    garbage_expected = 'garbage1234\r\r\n> '
    garbage_received = child.read(len(garbage_expected))
    assert garbage_expected == garbage_received

    # loop other defined commands and expected output
    for cmd, expected in CMDS:
        check_cmd(child, cmd, expected)


if __name__ == "__main__":
    sys.exit(run(testfunc))
