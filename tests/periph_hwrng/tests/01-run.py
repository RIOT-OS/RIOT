#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""periph_hwrng test

Automatic checking of the periph_hwrng firmware.
It checks the output is valid but not the number themselves.
"""

import sys
from testrunner import run


def testfunc(child):
    """Test generating several numbers

    It prints hardware generated random bytes buffer from increasing size.
    The generated random numbers are not checked.
    """
    child.expect_exact('HWRNG peripheral driver test')
    child.expect(r'This test will print from 1 to (\d+)'
                 ' random bytes about every second')
    iterations = int(child.match.group(1))

    for num in range(1, iterations + 1):
        child.expect_exact('generating %u random byte(s)' % num)
        generated = 'Got:' + num * (r' 0x[0-9a-fA-F]{2}')
        child.expect(generated)


if __name__ == "__main__":
    sys.exit(run(testfunc))
