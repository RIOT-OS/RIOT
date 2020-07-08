#!/usr/bin/env python3

# Copyright (C) 2018 Federico Pellegrin <fede@evolware.org>
# Copyright (C) 2019 Francisco Molina <francois-xavier.molina@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import subprocess
from testrunner import run
from testrunner.spawn import MAKE


def flash_slot(slotnum, version):
    cmd = [
        MAKE,
        "RIOTBOOT_SKIP_COMPILE=1",
        "riotboot/flash-slot{}".format(slotnum),
        "APP_VER={}".format(version),
    ]
    assert subprocess.call(cmd) == 0


def assert_check_slot(child, slotnum, version):
    # Check if it's running on the expected slot
    child.expect_exact('>')
    child.sendline("curslotnr")
    child.expect_exact("Current slot={}".format(slotnum))

    # Ask for current slot header info and check for basic output integrity
    child.expect_exact('>')
    child.sendline("curslothdr")
    # Magic number is "RIOT" (always in little endian)
    child.expect_exact("Image magic_number: 0x544f4952")
    # Other info is hardware/app dependent so we just check basic compliance
    child.expect_exact("Image Version: {0:#0{1}x}".format(version, 10))
    child.expect(r"Image start address: 0x[0-9a-fA-F]{8}\r\n")
    child.expect(r"Header chksum: 0x[0-9a-fA-F]{8}\r\n")

    # Ask for address of slot 0
    child.expect_exact('>')
    child.sendline("getslotaddr 0")
    child.expect(r"Slot 0 address=0x[0-9a-fA-F]{8}\r\n")

    # Ask for data of all slots
    child.expect_exact('>')
    child.sendline("dumpaddrs")
    child.expect(r"slot 0: metadata: 0x[0-9a-fA-F]{1,8} "
                 r"image: 0x[0-9a-fA-F]{8}\r\n")
    child.expect(r"slot 1: metadata: 0x[0-9a-fA-F]{1,8} "
                 r"image: 0x[0-9a-fA-F]{8}\r\n")
    child.expect_exact('>')


def testfunc(child):
    # Ask for current slot, should be 0 or 1
    child.expect_exact('>')
    child.sendline("curslotnr")
    child.expect(r"Current slot=([0-1])")
    slotnum = int(child.match.group(1))

    # Ask for current APP_VER
    child.expect_exact('>')
    child.sendline("curslothdr")
    child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F]{8})\r\n")
    current_app_ver = int(child.match.group("app_ver"), 16)

    # Flash to both slots and verify basic functions
    for version in [current_app_ver + 1, current_app_ver + 2]:
        slotnum = slotnum ^ 1
        flash_slot(slotnum, version)
        assert_check_slot(child, slotnum, version)

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc))
