#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#               2022 Christian Amsüss <chrysn@fsfe.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


EXPECTED_HELP = (
    'Command              Description',
    '---------------------------------------',
    'bufsize              Get the shell\'s buffer size',
    'start_test           starts a test',
    'end_test             ends a test',
    'echo                 prints the input command',
    'empty                print nothing on command',
    'hello_world          Print a greeting',
    'xfa_test1            xfa test command 1',
    'xfa_test2            xfa test command 2',
)

PROMPT = '> '

CMDS = (
    ('start_test', '[TEST_START]'),

    # test default commands
    ('help', EXPECTED_HELP),

    ('end_test', '[TEST_END]'),
)

CMDS_REGEX = {'ps.rs'}


def check_cmd(child, cmd, expected):
    regex = cmd in CMDS_REGEX
    child.expect(PROMPT)
    child.sendline(cmd)
    for line in expected:
        if regex:
            child.expect(line)
        else:
            child.expect_exact(line)


def testfunc(child):
    # loop other defined commands and expected output
    for cmd, expected in CMDS:
        check_cmd(child, cmd, expected)


if __name__ == "__main__":
    sys.exit(run(testfunc))
