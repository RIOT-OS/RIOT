#!/usr/bin/env python3

# Copyright (C) 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
#               2017 Martine Lenders <m.lenders@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.sendline('ifconfig')
    child.expect(r'       Statistics for Layer 2')
    child.expect(r'        RX packets \d+  bytes \d+')
    child.expect(r'        TX packets \d+ \(Multicast: \d+\)  bytes \d+')
    child.expect(r'        TX succeeded \d+ errors \d+')


if __name__ == "__main__":
    sys.exit(run(testfunc))
