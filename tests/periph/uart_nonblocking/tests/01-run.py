#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Benjamin Valentin
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


PRECISION = 1.005


def testfunc(child):
    child.expect_exact("puts with disabled interrupts and a full transmit buffer")
    child.expect(r'== printed in (\d+)/(\d+) µs ==')
    time_actual = int(child.match.group(1))
    time_expect = int(child.match.group(2))

    assert time_actual / time_expect < PRECISION

    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))
