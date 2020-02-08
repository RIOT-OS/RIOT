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

PROMPT = '> '

CMDS = (
    ('start_test', '[TEST_START]'),
    (CONTROL_C, PROMPT),
    ('\n', PROMPT),
    ('123456789012345678901234567890123456789012345678901234567890',
     'shell: command not found: '
     '123456789012345678901234567890123456789012345678901234567890'),
    ('unknown_command', 'shell: command not found: unknown_command'),
    ('hello-willy\b\b\b\borld', 'shell: command not found: hello-world'),
    ('\b\b\b\becho', '\"echo\"'),
    ('help', EXPECTED_HELP),
    ('echo a string', '\"echo\"\"a\"\"string\"'),
    ('ps', EXPECTED_PS),
    ('help', EXPECTED_HELP),
    ('reboot', 'test_shell.'),
    ('end_test', '[TEST_END]'),
)

BOARD = os.environ['BOARD']


def print_error(message):
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    print(FAIL + message + ENDC)


def check_cmd(child, cmd, expected):
    child.expect(PROMPT)
    child.sendline(cmd)
    for line in expected:
        child.expect_exact(line)


def check_startup(child):
    child.sendline(CONTROL_C)
    child.expect_exact(PROMPT)


def check_and_get_bufsize(child):
    child.sendline('bufsize')
    child.expect('([0-9]+)\r\n')
    bufsize = int(child.match.group(1))

    return bufsize


def check_line_exceeded(child, longline):

    if BOARD == 'nrf52dk':
        # There is an issue with nrf52dk when the Virtual COM port is connected
        # and sending more than 64 bytes over UART. If no terminal is connected
        # to the Virtual COM and interfacing directly to the nrf52832 UART pins
        # the issue is not present. See issue #10639 on GitHub.
        print_error('test case "check_line_exceeded" broken for nrf52dk. SKIP')
        return

    child.sendline(longline)
    child.expect('shell: maximum line length exceeded')


def check_line_canceling(child):
    child.expect(PROMPT)
    child.sendline('garbage1234' + CONTROL_C)
    garbage_expected = 'garbage1234\r\r\n'
    garbage_received = child.read(len(garbage_expected))

    assert garbage_expected == garbage_received


def check_erase_long_line(child, longline):
    # FIXME: this only works on native, due to #10634 combined with socat
    # insisting in line-buffering the terminal.

    if BOARD == 'native':
        longline_erased = longline + "\b"*len(longline) + "echo"
        child.sendline(longline_erased)
        child.expect_exact('"echo"')


def testfunc(child):
    # avoid sending an extra empty line on native.
    if BOARD == 'native':
        child.crlf = '\n'

    check_startup(child)

    bufsize = check_and_get_bufsize(child)
    longline = "_"*bufsize + "verylong"

    check_line_exceeded(child, longline)

    check_line_canceling(child)

    check_erase_long_line(child, longline)

    # loop other defined commands and expected output
    for cmd, expected in CMDS:
        check_cmd(child, cmd, expected)


if __name__ == "__main__":
    sys.exit(run(testfunc))
