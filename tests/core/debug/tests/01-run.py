#!/usr/bin/env python3

# Copyright (C) 2019 Alexandre Abadie <alexandre.abadie@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run

ANSI_CYAN = '\033[36m'
ANSI_RESET = '\033[0m'

PREFIX = 'prefix'
THREAD = '@main'

prefixes = [
    None,
    ':noprefix_func',
    THREAD,
    ':noprefix_thread_func' + THREAD,
    PREFIX,
    PREFIX + ':prefix_func',
    PREFIX + THREAD,
    PREFIX + ':prefix_thread_func' + THREAD,
]

def colored_prefix(prefix):
    if prefix is None:
        return ""
    return ANSI_CYAN + prefix + " # " + ANSI_RESET

def testfunc(child):
    for prefix in prefixes:
        child.expect_exact(colored_prefix(prefix) + "debug puts")
        child.expect_exact(colored_prefix(prefix) + "debug printf number '42' ... continued")
    child.expect_exact(colored_prefix('@(isr)') + "puts from isr")
    child.expect_exact(colored_prefix('@(isr)') + "printf from isr: 42")


if __name__ == "__main__":
    sys.exit(run(testfunc))
