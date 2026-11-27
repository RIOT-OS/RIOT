#!/usr/bin/env python3

import sys
from testrunner import run


def test(child):
    match_1 = child.expect("That took")


if __name__ == "__main__":
    sys.exit(run(test))
