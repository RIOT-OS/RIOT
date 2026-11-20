#!/usr/bin/env python3

import os
import sys
from testrunner import run

def test(child):
    child.expect("Early auto initialization")
    child.expect("Late auto initialization")
    child.expect("Main running")

if __name__ == "__main__":
    if not os.environ['BOARD'].startswith('native'):
        print("Automated test only works on native (other boards' early output is lost)", file=sys.stderr)
        sys.exit(1)
    sys.exit(run(test))
