#!/usr/bin/env python3

# Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import re
from testrunner import run


def testfunc(child):
    # RNG source
    child.sendline("source 0")
    child.expect_exact("> ")
    child.sendline("seed 1337")
    child.expect_exact("Seed set to 1337")
    child.expect_exact("> ")

    child.sendline("fips")
    child.expect_exact("Running FIPS 140-2 test, with seed 1337 using Musl C PRNG.")
    child.expect("Monobit test: passed")
    child.expect("Poker test: passed")
    child.expect("Run test: passed")
    child.expect("Longrun test: passed")
    child.expect_exact("> ")

    child.sendline("dump 10")
    child.expect_exact("Running dump test, with seed 1337 using Musl C PRNG.")
    child.expect("3954330727")
    child.expect("188121509")
    child.expect("1069019535")
    child.expect("1187072490")
    child.expect("4172651676")
    child.expect("2421580254")
    child.expect("2126354391")
    child.expect("1784094822")
    child.expect("751721632")
    child.expect("679714536")
    child.expect_exact("> ")

    child.sendline("entropy")
    child.expect(re.compile(r"Calculated 7\.995\d{3} bits of entropy from 10000 samples\."))
    child.expect_exact("> ")

    # Constant source
    child.sendline("source 1")
    child.expect_exact("> ")
    child.sendline("seed 1337")
    child.expect_exact("Seed set to 1337")
    child.expect_exact("> ")

    child.sendline("fips")
    child.expect("Running FIPS 140-2 test, with seed 1337 using constant value.")
    child.expect("- Monobit test: failed")
    child.expect("- Poker test: failed")
    child.expect("- Run test: failed")
    child.expect("- Longrun test: passed")
    child.expect_exact("> ")

    child.sendline("dump 10")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect("1337")
    child.expect_exact("> ")

    child.sendline("entropy")
    child.expect_exact("Running entropy test, with seed 1337 using constant value.")
    child.expect(re.compile(r"Calculated 0\.017\d{3} bits of entropy from 10000 samples\."))
    child.expect_exact("> ")


if __name__ == "__main__":
    sys.exit(run(testfunc))
