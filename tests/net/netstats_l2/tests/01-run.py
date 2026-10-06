#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Alexandre Abadie <alexandre.abadie@inria.fr>
# SPDX-FileCopyrightText: 2017 Martine Lenders <m.lenders@fu-berlin.de>
# SPDX-License-Identifier: LGPL-2.1-only

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
