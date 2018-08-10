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
    child.sendline("seed 1337")

    child.sendline("fips")
    child.expect("Running FIPS 140-2 test, with seed 1337 using Tiny Mersenne Twister PRNG.")
    child.expect("Monobit test: passed")
    child.expect("Poker test: passed")
    child.expect("Run test: passed")
    child.expect("Longrun test: passed")

    child.sendline("dump 10")
    child.expect("1555530734")
    child.expect("2178333451")
    child.expect("2272913641")
    child.expect("3790481823")
    child.expect("3190025502")
    child.expect("798555366")
    child.expect("1918982324")
    child.expect("1550167154")
    child.expect("3454972398")
    child.expect("1034066532")

    child.sendline("entropy")
    child.expect(re.compile(r"Calculated 7\.994\d{3} bits of entropy from 10000 samples\."))

    # Constant source
    child.sendline("source 1")
    child.sendline("seed 1337")

    child.sendline("fips")
    child.expect("Running FIPS 140-2 test, with seed 1337 using constant value.")
    child.expect("- Monobit test: failed")
    child.expect("- Poker test: failed")
    child.expect("- Run test: failed")
    child.expect("- Longrun test: passed")

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

    child.sendline("entropy")
    child.expect(re.compile(r"Calculated 0\.017\d{3} bits of entropy from 10000 samples\."))


if __name__ == "__main__":
    sys.exit(run(testfunc))
