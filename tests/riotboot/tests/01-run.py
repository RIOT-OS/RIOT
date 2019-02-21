#!/usr/bin/env python3

# Copyright (C) 2018 Federico Pellegrin <fede@evolware.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # Ask for current slot, should be 0 or 1
    child.sendline("curslotnr")
    child.expect("Current slot=[0-1]")
    child.expect('>')

    # Ask for current slot header info and checks for basic output integrity
    child.sendline("curslothdr")
    # Magic number is "RIOT" (always in little endian)
    child.expect_exact("Image magic_number: 0x544f4952")
    # Other info is hardware/app dependant so we just check basic compliance
    child.expect("Image Version: 0x[0-9a-fA-F]{8}")
    child.expect("Image start address: 0x[0-9a-fA-F]{8}")
    child.expect("Header chksum: 0x[0-9a-fA-F]{8}")
    child.expect('>')

    # Ask for address of slot 0
    child.sendline("getslotaddr 0")
    child.expect("Slot 0 address=0x[0-9a-fA-F]{8}")
    child.expect('>')

    # Ask for data of all slots
    child.sendline("dumpaddrs")
    child.expect("slot 0: metadata: 0x[0-9a-fA-F]{1,8} image: 0x[0-9a-fA-F]{8}")
    child.expect('>')


if __name__ == "__main__":
    sys.exit(run(testfunc))
