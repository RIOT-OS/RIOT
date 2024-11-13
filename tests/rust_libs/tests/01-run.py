#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#               2022 Christian Amsüss <chrysn@fsfe.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import json
import sys
from testrunner import run


EXPECTED_CMDS = {
        'bufsize': 'Get the shell\'s buffer size',
        'start_test': 'starts a test',
        'end_test': 'ends a test',
        'echo': 'prints the input command',
        'empty': 'print nothing on command',
        'periodic': 'periodically print command',
        'hello_world': 'Print a greeting',
        'xfa_test1': 'xfa test command 1',
        'xfa_test2': 'xfa test command 2',
}

PROMPT = '> '

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


def check_cmd_list(child):
    child.expect(PROMPT)
    child.sendline('help_json')
    child.expect(r"(\{[^\n\r]*\})\r\n")
    cmdlist = json.loads(child.match.group(1))["cmds"]
    cmds = set(EXPECTED_CMDS)
    for item in cmdlist:
        assert item['cmd'] in EXPECTED_CMDS, f"command {item['cmd']} not expected"
        assert item['cmd'] in cmds, f"command {item['cmd']} listed twice"
        assert item['desc'] == EXPECTED_CMDS[item['cmd']], f"description of {item['cmd']} not expected"
        cmds.remove(item['cmd'])

    assert len(cmds) == 0, f"commands {cmds} missing"


def testfunc(child):
    # loop other defined commands and expected output
    check_cmd(child, 'start_test', '[TEST_START]')
    check_cmd_list(child)
    check_cmd(child, 'end_test', '[TEST_END]')


if __name__ == "__main__":
    sys.exit(run(testfunc))
