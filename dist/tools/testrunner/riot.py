# Copyright (C) 2018 Gaëtan Harter <gaetan.harter@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import time

import pexpect


RIOT_PROMPT = '>'


def expect_shell_prompt(child, sendline=True, *args, **kwargs):
    """Wait for shell prompt. It sends a newline if `sendline`.

    :param child: pexpect spawn object
    :param sendline: set it to True to send a newline before expecting
                     shell prompt
    """
    if sendline:
        child.sendline('')
    child.expect(RIOT_PROMPT, *args, **kwargs)


def expect_working_shell_prompt(child, timeout=None, sendlinestep=0.5):
    """Wait for the shell prompt to be working.

    It waits for a first prompt character and sends newline until it is active

    :param child: pexpect spawn object
    :param timeout: expect timeout, defaults to child.timeout
    :param sendlinestep: steps for retrying sending newline
    """
    timeout = timeout if timeout is not None else child.timeout
    t_end = time.time() + timeout

    # First prompt character
    expect_shell_prompt(child, sendline=True, timeout=timeout)

    # Wait until the shell answers
    while time.time() < (t_end - sendlinestep):
        child.sendline('')
        if 0 == child.expect([RIOT_PROMPT, pexpect.TIMEOUT], timeout=sendlinestep):
            return

    # Do a last one that can timeout
    expect_shell_prompt(child, sendline=True, timeout=sendlinestep)
