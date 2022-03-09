#!/usr/bin/env python3

# Copyright (C) 2018 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.sendline('test')
    child.expect('SUCCESS')

    # reboot the device to ensure data are still available on EEPROM
    child.sendline('reboot')
    child.expect(r'EEPROM size:		(\d+)')
    eeprom_size = int(child.match.group(1))
    child.expect_exact('>')
    child.sendline('read 0 4')
    child.expect_exact('Data read from EEPROM (4 bytes): AAAA')
    child.sendline('read {} 4'.format(eeprom_size - 4))
    child.expect_exact('Data read from EEPROM (4 bytes): AAAA')


if __name__ == "__main__":
    sys.exit(run(testfunc))
