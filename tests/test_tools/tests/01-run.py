#!/usr/bin/env python3
"""Test behaviour of the test running and the term program interaction."""

import sys
import pexpect
from testrunner import run


def _shellping(child, timeout=1):
    """Issue a 'shellping' command.

    Raises a pexpect exception on failure.
    :param timeout: timeout for the answer
    """
    child.sendline('shellping')
    child.expect_exact('shellpong\r\n', timeout=timeout)


def _wait_shell_ready(child, numtries=5):
    """Wait until the shell is ready by using 'shellping'."""
    for _ in range(numtries - 1):
        try:
            _shellping(child)
        except pexpect.TIMEOUT:
            pass
        else:
            break
    else:
        # This one should fail
        _shellping(child)


def _test_no_local_echo(child):
    """Verify that there is not local echo while testing."""
    msg = 'true this should not be echoed'
    child.sendline(msg)
    res = child.expect_exact([pexpect.TIMEOUT, msg], timeout=1)
    assert res == 0, "There should have been a timeout and not match stdin"


def testfunc(child):
    """Run some tests to verify the board under test behaves correctly.

    It currently tests:

    * local echo
    """
    child.expect_exact("Running 'tests_tools' application")

    _wait_shell_ready(child)

    # Verify there is no local and remote echo as it is disabled
    _test_no_local_echo(child)

    # The node should still answer after the previous one
    _shellping(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))
