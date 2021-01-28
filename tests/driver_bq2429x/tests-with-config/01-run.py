#!/usr/bin/env python3

# Copyright (C) 2020 Locha Inc
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys

from testrunner import run


def testfunc(child):
    # VLIM
    child.sendline("bq2429x_set_vlim 15")
    child.expect(r"VLIM set to 5080 mV")
    child.sendline("bq2429x_get_vlim")
    child.expect(r"VLIM: 5080 mV")
    # ILIM
    child.sendline("bq2429x_set_ilim 2")
    child.expect(r"ILIM set to 500 mA")
    child.sendline("bq2429x_get_ilim")
    child.expect(r"ILIM: 500 mA")
    # ICHG
    child.sendline("bq2429x_set_ichg 0")
    child.expect(r"ICHG set to 512 mA")
    child.sendline("bq2429x_get_ichg")
    child.expect(r"ICHG: 512 mA")
    # VREG
    child.sendline("bq2429x_set_vreg 6")
    child.expect(r"VREG set to 4208 mV")
    child.sendline("bq2429x_get_vreg")
    child.expect(r"VREG: 4208 mV")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=5, echo=True))
