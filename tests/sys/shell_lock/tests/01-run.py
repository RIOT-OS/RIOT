#!/usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import time

from testrunner import run

PASSWORD_CORRECT = "password"
PASSWORDS_INCORRECT = [
    "pass",
    "word",
    "asswor",
    "passw0rd",
    "password_",
    "_password"
]

AUTO_LOCK_TIMEOUT_MS = 7000
SHELL_PROMPT = '> '
PASSWORD_PROMPT = 'Password: '
BOARD = os.environ['BOARD']


def testfunc(child):

    # avoid sending an extra empty line on native.
    if BOARD in ['native', 'native32']:
        child.crlf = '\n'

    # unlock
    child.sendline(PASSWORD_CORRECT)
    child.expect_exact('Shell was unlocked.')
    child.expect_exact(SHELL_PROMPT)

    # check we have access
    child.sendline('ping')
    child.expect_exact("PONG!")

    # lock
    child.sendline('lock')
    child.expect(SHELL_PROMPT)

    # trigger password prompt
    child.sendline('ping')
    child.expect('The shell is locked. Enter a valid password to unlock.')

    # test different incorrect passwords
    for i, pwd in enumerate(PASSWORDS_INCORRECT):

        # every third incorrect attempt leads to 7 second of sleep, otherwise
        # just 1 second
        if i > 0 and i % 3 == 0:
            timeout = 7
        else:
            timeout = 1

        # some boards react quite slow, give them 2 extra seconds
        child.expect_exact(PASSWORD_PROMPT, timeout=(timeout + 2))
        child.sendline(pwd)
        child.expect_exact('Wrong password')

    # unlock
    child.sendline(PASSWORD_CORRECT)
    child.expect_exact('Shell was unlocked.')
    child.expect_exact(SHELL_PROMPT)

    # check we have access
    child.sendline('ping')
    child.expect_exact("PONG!")

    # wait until auto_lock locks the shell after
    # CONFIG_SHELL_LOCK_AUTO_LOCK_TIMEOUT_MS (+ 1 second buffer time)
    time.sleep((AUTO_LOCK_TIMEOUT_MS / 1000.0) + 1)

    # trigger password prompt
    child.sendline('ping')
    child.expect('The shell is locked. Enter a valid password to unlock.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
