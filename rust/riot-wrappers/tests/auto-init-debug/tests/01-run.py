#!/usr/bin/env python3

import os
import sys
from testrunner import run

def test(child):
    # avoiding parentheses in the numbers -- these are regexps!
    child.expect("auto_init: auto_early .1.")
    child.expect("Early auto initialization")
    child.expect("auto_init: auto_late .65535.")
    child.expect("Late auto initialization")
    child.expect("Main running")

if __name__ == "__main__":
    if not os.environ['BOARD'].startswith('native'):
        print("Automated test only works on native (other boards' early output is lost)", file=sys.stderr)
        sys.exit(1)
    sys.exit(run(test))
