#!/usr/bin/env python3

import os
import sys
from testrunner import run

def test(child):
    # Cant' make any predictions about network addresses, but showing them
    # should not crash.
    for _ in range(3):
        child.expect("Netif at ")
        child.expect("Cache entries")

if __name__ == "__main__":
    sys.exit(run(test))
