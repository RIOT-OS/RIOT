#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Technische Universität Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('RTT interrupt latency test')
    idx = child.expect([r'OK \(\d+ tests\)', r'run \d+ failures \d+',
                        r'Error: [^\r\n]*'], timeout=60)
    assert idx == 0, child.match.group(0)


if __name__ == "__main__":
    sys.exit(run(testfunc))
