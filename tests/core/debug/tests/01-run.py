#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 TU Dresden
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run

ANSI_FOR_PREFIX = '\x1b[96;1m'
ANSI_FOR_THREAD_FUNC = '\x1b[37;2m'
ANSI_RESET = '\x1b[0m'

PREFIX = 'prefix'
THREAD = 'main'

prefixes = [
    None,
    (PREFIX, None, None),
    (PREFIX, 'prefix_func', None),
    (PREFIX, None, THREAD),
    (PREFIX, 'prefix_thread_func', THREAD),
    ('main', None, '<isr>'),
    ('custom', None, THREAD),
]

def colored_prefix(prefix):
    if prefix is None:
        return ""
    prefix,func,thread = prefix
    colored = ANSI_FOR_PREFIX + prefix + ANSI_FOR_THREAD_FUNC
    if func != None and thread != None:
        colored += ' (' + func + '@' + thread + ')'
    elif func != None:
        colored += ' (' + func + ')'
    elif thread != None:
        colored += ' (@' + thread + ')'
    colored += ': ' + ANSI_RESET
    return colored

def testfunc(child):
    for prefix in prefixes:
        child.expect_exact(colored_prefix(prefix) + "debug puts")
        child.expect_exact(colored_prefix(prefix) + "debug printf number '42' ... continued")


if __name__ == "__main__":
    sys.exit(run(testfunc))
