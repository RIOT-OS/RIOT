#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
# SPDX-License-Identifier: LGPL-2.1-only

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
import json
from testrunner import run


def testfunc(child):
    child.expect(r"Testing with a stack sized (\d+) and an alignment up to (\d+)\r\n")
    stack_size = int(child.match.group(1))
    alignment = int(child.match.group(2))
    stack_used_min = stack_size
    stack_used_max = 0
    for i in range(alignment):
        child.expect_exact(f"Testing for alignment {i}: OK")
        child.expect(r"(\{[^\n\r]*\})\r\n")
        stats = json.loads(child.match.group(1))["threads"][0]
        assert stats["name"] == "test"
        assert stats["stack_used"] < stats["stack_size"]
        if stack_used_max < stats["stack_used"]:
            stack_used_max = stats["stack_used"]
        if stack_used_min > stats["stack_used"]:
            stack_used_min = stats["stack_used"]

    child.expect_exact("TEST PASSED")
    alignment_loss = stack_used_max - stack_used_min
    if alignment_loss > 0:
        print(f"NOTE: Up to {alignment_loss} B of RAM is lost when thread " +
              "stacks are not properly aligned")


if __name__ == "__main__":
    sys.exit(run(testfunc))
