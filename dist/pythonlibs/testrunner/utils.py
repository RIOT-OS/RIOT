# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

"""Utility functions for writing tests."""

import pexpect


def _test_utils_interactive_sync(child, retries, delay, ready_cmd='r',
                                 ready_exp='READY'):

    for _ in range(retries):
        child.sendline(ready_cmd)
        ret = child.expect_exact([ready_exp, pexpect.TIMEOUT], timeout=delay)
        if ret == 0:
            break
    else:
        # Last call to make it fail here,
        child.expect_exact(ready_exp, timeout=0)


def test_utils_interactive_sync(child, retries, delay):
    """
    Wait for the node to be ready and start the test.

    Sends `r` until the node answers with `READY`, then sends `s` and waits
    for `START`. This pairs with the `test_utils_interactive_sync()` function
    on the node.
    """
    _test_utils_interactive_sync(child, retries, delay)
    child.sendline('s')
    child.expect_exact('START')


def test_utils_interactive_sync_shell(child, retries, delay):
    """
    Wait for the shell of the node to be ready.

    Sends a newline until the node answers with the `>` prompt. This is used
    when the node uses the `shell` and `test_utils_interactive_sync` modules,
    but does not call the `test_utils_interactive_sync()` function.
    """
    _test_utils_interactive_sync(child, retries, delay, '\n', '>')
