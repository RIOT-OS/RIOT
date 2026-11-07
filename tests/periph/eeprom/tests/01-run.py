#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2018 Inria
# SPDX-License-Identifier: LGPL-2.1-only

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
