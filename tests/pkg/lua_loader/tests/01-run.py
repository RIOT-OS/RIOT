#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

# Tell the lua interpreter running in riot to load some modules and print
# the value of a variable inside that module.

import sys
from testrunner import run

MODULE_QUERIES = [
    ("m1", "a", "Quando uma lua"),
    ("m2", "a", "chega de repente"),
    ("c1", "X", "E se deixa no céu,"),
    ("c2", "X", "como esquecida"),
]


def test(child):
    # check startup message
    child.expect_exact('I am a module, hi!')

    # loop other defined commands and expected output
    for mod, attr, val in MODULE_QUERIES:
        child.sendline('print((require"{}").{})'.format(mod, attr))
        child.expect_exact(val)


if __name__ == "__main__":
    sys.exit(run(test))
