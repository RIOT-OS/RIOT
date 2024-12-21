#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import json
import os
import sys
from testrunner import run


# This is the minimum subset of commands expected to be available on all
# boards. The test will still pass if additional commands are present, as
# `shell_cmds_default` may pull in board specific commands.
EXPECTED_CMDS = {
    'bufsize': 'Get the shell\'s buffer size',
    'start_test': 'starts a test',
    'end_test': 'ends a test',
    'echo': 'prints the input command',
    'empty': 'print nothing on command',
    'periodic': 'periodically print command',
    'app_metadata': 'Returns application metadata',
    'pm': 'interact with layered PM subsystem',
    'ps': 'Prints information about running threads.',
    'reboot': 'Reboot the node',
    'version': 'Prints current RIOT_VERSION',
    'xfa_test1': 'xfa test command 1',
    'xfa_test2': 'xfa test command 2',
}

EXPECTED_PS = (
    '\tpid | state    Q | pri',
    r'\t  \d | running  Q |   7'
)

RIOT_TERMINAL = os.environ.get('RIOT_TERMINAL')
CLEANTERMS = {"socat"}

TESTRUNNER_SHELL_SKIP_REBOOT = bool(int(os.environ.get('TESTRUNNER_SHELL_SKIP_REBOOT') or 0))

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

    # test empty line input
    ('\n', PROMPT),

    # test simple word separation
    ('echo a string', '"echo""a""string"'),
    ('echo   multiple   spaces   between   argv', '"echo""multiple""spaces""between""argv"'),
    ('echo \t tabs\t\t processed \t\tlike\t \t\tspaces', '"echo""tabs""processed""like""spaces"'),

    # test unknown commands
    ('unknown_command', 'shell: command not found: unknown_command'),

    # test leading/trailing BLANK
    ('     echo leading spaces', '"echo""leading""spaces"'),
    ('\t\t\t\t\techo leading tabs', '"echo""leading""tabs"'),
    ('echo trailing spaces     ', '"echo""trailing""spaces"'),
    ('echo trailing tabs\t\t\t\t\t', '"echo""trailing""tabs"'),

    # test backspace
    ('hello-willy\b\b\b\borld', 'shell: command not found: hello-world'),
    ('\b\b\b\becho', '"echo"'),

    # test escaping
    ('echo \\\'', '"echo""\'"'),
    ('echo \\"', '"echo""""'),
    ('echo escaped\\ space', '"echo""escaped space"'),
    ('echo escape within \'\\s\\i\\n\\g\\l\\e\\q\\u\\o\\t\\e\'', '"echo""escape""within""singlequote"'),
    ('echo escape within "\\d\\o\\u\\b\\l\\e\\q\\u\\o\\t\\e"', '"echo""escape""within""doublequote"'),
    ("""echo "t\\e st" "\\"" '\\'' a\ b""", '"echo""te st"""""\'""a b"'),  # noqa: W605

    # test correct quoting
    ('echo "hello"world', '"echo""helloworld"'),
    ('echo hel"lowo"rld', '"echo""helloworld"'),
    ('echo hello"world"', '"echo""helloworld"'),
    ('echo quoted space " "', '"echo""quoted""space"" "'),
    ('echo abc"def\'ghijk"lmn', '"echo""abcdef\'ghijklmn"'),
    ('echo abc\'def"ghijk\'lmn', '"echo""abcdef"ghijklmn"'),
    ('echo "\'" \'"\'', '"echo""\'""""'),

    # test incorrect quoting
    ('echo a\\', 'shell: incorrect quoting'),
    ('echo "', 'shell: incorrect quoting'),
    ('echo \'', 'shell: incorrect quoting'),
    ('echo abcdef"ghijklmn', 'shell: incorrect quoting'),
    ('echo abcdef\'ghijklmn', 'shell: incorrect quoting'),

    # test default commands
    ('ps', EXPECTED_PS),

    # test commands added to shell_commands_xfa
    ('xfa_test1', '[XFA TEST 1 OK]'),
    ('xfa_test2', '[XFA TEST 2 OK]'),

    # test reboot
    ('reboot', 'test_shell.'),

    ('end_test', '[TEST_END]'),
)

CMDS_CLEANTERM = {
    (CONTROL_C, PROMPT),
}

CMDS_REGEX = {'ps'}

BOARD = os.environ['BOARD']

# there's an issue with some boards' serial that causes lost characters.
LINE_EXCEEDED_BLACKLIST = {
    # There is an issue with nrf52dk when the Virtual COM port is connected
    # and sending more than 64 bytes over UART. If no terminal is connected
    # to the Virtual COM and interfacing directly to the nrf52832 UART pins
    # the issue is not present. See issue #10639 on GitHub.
    'nrf52dk',
    'z1',
}


def print_error(message):
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    print(FAIL + message + ENDC)


def check_cmd(child, cmd, expected):
    regex = cmd in CMDS_REGEX
    child.expect(PROMPT)
    child.sendline(cmd)
    for line in expected:
        if regex:
            child.expect(line)
        else:
            child.expect_exact(line)


def check_help(child):
    """
    Runs the `help_json` and `help` command to check if the list of commands
    and descriptions match and contain a list of expected commands.
    """

    # Run help_json to get the list of commands present
    child.expect(PROMPT)
    child.sendline('help_json')
    # expect JSON object as response the covers the whole line
    child.expect(r"(\{[^\n\r]*\})\r\n")

    # use a set to track which expected commands were already covered
    cmds_expected = set(EXPECTED_CMDS)

    # record actually present commands (which may be more than the expected
    # ones) and their descriptions in here
    cmds_actual = set()
    desc_actual = {}

    # parse the commands and iterate over the list
    cmdlist = json.loads(child.match.group(1))["cmds"]
    for item in cmdlist:
        # for expected commands, validate the description and ensure they
        # are listed exactly once
        if item['cmd'] in EXPECTED_CMDS:
            assert item['cmd'] in cmds_expected, f"command {item['cmd']} listed twice"
            assert item['desc'] == EXPECTED_CMDS[item['cmd']], f"description of {item['cmd']} not expected"
            cmds_expected.remove(item['cmd'])

        # populate the list of actually present commands and their description
        cmds_actual.add(item['cmd'])
        desc_actual[item['cmd']] = item['desc']

    assert len(cmds_expected) == 0, f"commands {cmds_expected} missing"

    # Now: Run regular help and expect the same commands as help_json
    child.expect(PROMPT)
    child.sendline('help')

    # expect the header first
    child.expect_exact('Command              Description\r\n')
    child.expect_exact('---------------------------------------\r\n')

    # loop until the set of actually present commands assembled from the JSON
    # is empty. We remove each command from the set when we process it, so that
    # we can detect duplicates
    while len(cmds_actual) > 0:
        # parse line into command and description
        child.expect(r"([a-z0-9_-]*)[ \t]*([^\r\n]*)\r\n")
        cmd = child.match.group(1)
        desc = child.match.group(2)

        # expect the command to be in the set got from the JSON. Then remove
        # it, so that a duplicated line would trigger the assert
        assert cmd in cmds_actual, f"Command \"{cmd}\" unexpected or listed twice in help"
        cmds_actual.remove(cmd)

        # description should match the one got from JSON
        assert desc == desc_actual[cmd], f"Description for \"{cmd}\" not matching"


def check_startup(child):
    child.sendline(CONTROL_C)
    child.expect_exact(PROMPT)


def check_and_get_bufsize(child):
    child.sendline('bufsize')
    child.expect('([0-9]+)\r\n')
    bufsize = int(child.match.group(1))

    return bufsize


def check_line_exceeded(child, longline):

    if BOARD in LINE_EXCEEDED_BLACKLIST:
        print_error('test case "check_line_exceeded" blacklisted, SKIP')
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

    if BOARD in ['native', 'native32']:
        longline_erased = longline + "\b"*len(longline) + "echo"
        child.sendline(longline_erased)
        child.expect_exact('"echo"')


def check_control_d(child):
    # The current shell instance was initiated by shell_run_once(). The shell will exit.
    child.sendline(CONTROL_D)
    child.expect_exact('shell exited')

    # The current shell instance was initiated by shell_run(). The shell will respawn
    # automatically except on native. On native, RIOT is shut down completely,
    # therefore exclude this part.
    if BOARD not in ['native', 'native32']:
        child.sendline(CONTROL_D)
        child.expect_exact(PROMPT)


def check_preempt(child):
    child.expect(PROMPT)
    child.sendline('periodic 5')

    child.expect_exact('test')
    child.expect_exact('test')
    child.expect_exact('test')
    child.expect_exact('test')
    child.expect_exact('test')
    child.sendline('')


def testfunc(child):
    # avoid sending an extra empty line on native.
    if BOARD in ['native', 'native32']:
        child.crlf = '\n'

    bufsize = check_and_get_bufsize(child)
    longline = "_" * (bufsize - len("verylong")) + "verylong"

    check_line_exceeded(child, longline)

    if RIOT_TERMINAL in CLEANTERMS:
        check_line_canceling(child)
    else:
        print("skipping check_line_canceling()")

    check_erase_long_line(child, longline)

    check_control_d(child)

    check_preempt(child)

    # loop other defined commands and expected output
    for cmd, expected in CMDS:

        if cmd == "reboot" and TESTRUNNER_SHELL_SKIP_REBOOT:
            continue

        check_cmd(child, cmd, expected)

    check_help(child)

    if RIOT_TERMINAL in CLEANTERMS:
        for cmd, expected in CMDS_CLEANTERM:
            check_cmd(child, cmd, expected)
    else:
        print("skipping cleanterm tests")


if __name__ == "__main__":
    sys.exit(run(testfunc))
