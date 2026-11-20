#!/usr/bin/env python3

import sys
from testrunner import run

def test(child):
    child.expect("SUCCESS")

if __name__ == "__main__":
    sys.exit(run(test))
