#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os, signal, sys, subprocess
from pexpect import spawnu, TIMEOUT, EOF
from traceback import print_tb


# Patterns to detect if 'make term' is started
MAKE_TERM_STARTED = {
    'pyterm': r"Type '/exit' to exit.",
    'native': r"main\(\): This is RIOT",
    'native_noshell': r"RIOT .* hardware initialization complete",
}


def spawnu_make_term(echo=True, **kwargs):
    """Spawn make term and try to wait until started.

    :param echo: enable echo to stdout
    :param **kwargs: additional parameters given to `spawnu`

    If 'make term' start not detected, keep going anyway.
    """
    logfile = sys.stdout if echo else None
    child = spawnu("make term", logfile=logfile, **kwargs)

    _lazywait_term_started(child)

    return child


def _lazywait_term_started(child, start_timeout=5, error=False):
    """Wait until `term` child is started or keep going.
    The goal is just to be, when possible, faster than `sleep(timeout)`.

    Try to recognize some started message from child output.
    Uses 'started' patterns are taken from MAKE_TERM_STARTED dict values.

    On timeout, a debug message is printed.
    If `error`, raise the TIMEOUT exception.
    """
    started_patterns = list(MAKE_TERM_STARTED.values())
    try:
        child.expect(started_patterns, timeout=start_timeout)
    except TIMEOUT:
        print("Timeout waiting for 'make term' to be started")
        if error:
            raise
        # Keep going anyway


def run(testfunc, timeout=10, echo=True, traceback=True):
    env = os.environ.copy()

    child = spawnu_make_term(echo=echo, env=env, timeout=timeout)

    try:
        subprocess.check_output(('make', 'reset'), env=env,
                                stderr=subprocess.PIPE)
    except subprocess.CalledProcessError:
        # make reset yields error on some boards even if successful
        pass
    try:
        testfunc(child)
    except TIMEOUT:
        print("Timeout in expect script")
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    finally:
        print("")
        os.killpg(os.getpgid(child.pid), signal.SIGKILL)
        child.close()

    return 0
