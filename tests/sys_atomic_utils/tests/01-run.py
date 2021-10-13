#!/usr/bin/env python3

#  Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
