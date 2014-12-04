#!/usr/bin/env python

# Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os, signal, sys
from pexpect import spawn, TIMEOUT, EOF


DEFAULT_TIMEOUT = 5

def main():
    p = None

    try:
        p = spawn("make term", timeout=DEFAULT_TIMEOUT)
        p.logfile = sys.stdout

        p.expect("All tests successful.")
    except TIMEOUT as exc:
        print(exc)
        return 1
    finally:
        if p and not p.terminate():
            os.killpg(p.pid, signal.SIGKILL)

    return 0

if __name__ == "__main__":
    sys.exit(main())
