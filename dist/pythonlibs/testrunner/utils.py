# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""Utility functions for writing tests."""

import pexpect


def test_utils_interactive_sync(child, retries=5, delay=1):
    """Synchronisation for 'test_utils_interactive_sync' function.

    Interacts through input to wait for node being ready.
    """
    for _ in range(0, retries):
        child.sendline('r')
        ret = child.expect_exact(['READY', pexpect.TIMEOUT], timeout=delay)
        if ret == 0:
            break
    else:
        # Last call to make it fail her,
        child.expect_exact('READY', timeout=0)

    child.sendline('s')
    child.expect_exact('START')
