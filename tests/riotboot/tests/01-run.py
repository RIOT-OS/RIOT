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


class RiotbootDevice:
    def __init__(self):
        self.slot_num = None
        self.app_ver = None

    def flash_current_slot_and_app_ver(self):
        cmd = [
            MAKE,
            "RIOTBOOT_SKIP_COMPILE=1",
            "riotboot/flash-slot{}".format(self.slot_num),
            "APP_VER={}".format(self.app_ver),
        ]
        assert(self.slot_num is not None)
        assert(self.app_ver is not None)
        assert subprocess.call(cmd) == 0

    def get_current_slot_and_app_ver(self, child):
        # Ask for current slot, should be 0 or 1
        child.expect_exact('>')
        child.sendline("curslotnr")
        child.expect(r"Current slot=([0-1])")
        self.slot_num = int(child.match.group(1))

        # Ask for current APP_VER
        child.expect_exact('>')
        child.sendline("curslothdr")
        child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F]{8})\r\n")
        self.app_ver = int(child.match.group("app_ver"), 16)

    def check_current_slot_and_app_ver(self, child):
        # Check if it's running on the expected slot
        child.expect_exact('>')
        child.sendline("curslotnr")
        child.expect_exact("Current slot={}".format(self.slot_num))

        # Ask for current slot header info and check for basic output integrity
        child.expect_exact('>')
        child.sendline("curslothdr")
        # Magic number is "RIOT" (always in little endian)
        child.expect_exact("Image magic_number: 0x544f4952")
        # Other info is hardware/app dependent so we just check basic compliance
        child.expect_exact("Image Version: {0:#0{1}x}".format(self.app_ver, 10))
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


def testfunc():
    # This test requires flashing slot binaries. Some flashers like
    # cc2538-bsl need to attach to the serial terminal in order to be
    # able to flash. Since `run(func)` spawns a terminal a different
    # `run(func)` is called after every flash in order to detach and
    # re-attach the terminal between flashes
    dev = RiotbootDevice()
    # Get the current slot number and application version
    ret = run(dev.get_current_slot_and_app_ver)
    if ret != 0:
        return ret

    # flash to both slots and verify
    for version in [dev.app_ver + 1, dev.app_ver + 2]:
        # update slot number and application version
        dev.slot_num = dev.slot_num ^ 1
        dev.app_ver = version
        # flash
        dev.flash_current_slot_and_app_ver()
        # verify its running from the correct slot with the right version
        ret = run(dev.check_current_slot_and_app_ver)
        if ret != 0:
            return ret

    print("TEST PASSED")
    return 0


if __name__ == "__main__":
    sys.exit(testfunc())
