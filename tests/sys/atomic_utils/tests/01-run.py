#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
from testrunner import run


def testfunc(child):
    fns = ["fetch_add", "fetch_sub", "fetch_or", "fetch_xor", "fetch_and"]
    postfixes = ["_u8", "_u16", "_u32", "_u64"]
    tests = ["tearing_test", "lost_update_test"]
    prefixes = {
        "tearing_test": ["atomic_", "semi_atomic_"],
        "lost_update_test": ["atomic_"]
    }
    timeout = "1"

    for test in tests:
        for prefix in prefixes[test]:
            for postfix in postfixes:
                for fn in fns:
                    child.sendline(test + " " + prefix + fn + postfix + " "
                                   + timeout)
                    child.expect("OK")


if __name__ == "__main__":
    sys.exit(run(testfunc))
