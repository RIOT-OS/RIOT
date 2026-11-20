#!/usr/bin/env python3

import os
import sys
from testrunner import run

def test(child):
    child.expect("Done")

if __name__ == "__main__":
    sys.exit(run(test))
