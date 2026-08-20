#!/usr/bin/env python3

# Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

ANSI_CYAN = '\033[1;36m'
ANSI_RESET = '\033[0m'

PREFIX = 'prefix'
THREAD = '@main'

prefixes = [
    '',
    ' (noprefix_func)',
    ' (' + THREAD + ')',
    ' (noprefix_thread_func' + THREAD + ')',
    PREFIX,
    PREFIX + ' (prefix_func)',
    PREFIX + ' (' + THREAD + ')',
    PREFIX + ' (prefix_thread_func' + THREAD + ')',
    ' (@<isr>)',
    'custom (' + THREAD + ')',
]

def colored_prefix(prefix):
    if len(prefix) == 0:
        return ""
    return ANSI_CYAN + prefix + " # " + ANSI_RESET

def testfunc(child):
    for prefix in prefixes:
        child.expect_exact(colored_prefix(prefix) + "debug puts")
        child.expect_exact(colored_prefix(prefix) + "debug printf number '42' ... continued")


if __name__ == "__main__":
    sys.exit(run(testfunc))
