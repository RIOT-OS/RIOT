#!/usr/bin/env python

# Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os, signal, sys, subprocess
from pexpect import spawn, TIMEOUT, EOF

DEFAULT_TIMEOUT = 5

def main():
    env = os.environ.copy()
    child = spawn("make term", env=env, timeout=DEFAULT_TIMEOUT,
                  encoding="utf-8")
    child.logfile = sys.stdout

    try:
        subprocess.check_output(('make', 'reset'), env=env,
                                stderr=subprocess.PIPE)
    except subprocess.CalledProcessError:
        # make reset yields error on some boards even if successful
        pass
    try:
        child.expect(u"OK \\([0-9]+ tests\\)")
    except TIMEOUT:
        print("There where errors in the unittests")
        return 1
    finally:
        print("")
        child.close()

    return 0

if __name__ == "__main__":
    sys.exit(main())
