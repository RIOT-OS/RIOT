#!/usr/bin/env python3

import os
import sys
from testrunner import run

def test(child):
    match_1 = child.expect(["A: Done", "B: Done"])
    match_2 = child.expect(["A: Done", "B: Done"])
    assert {match_1, match_2} == {0, 1}, "One 'done' showed up twice"

if __name__ == "__main__":
    sys.exit(run(test))
