#!/usr/bin/env python

# Copyright (C) 2015 INRIA
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os, signal, sys
from pexpect import TIMEOUT, EOF
if sys.version_info[0] == 2:
    from pexpect import spawn
else:
    from pexpect import spawnu as spawn

DEFAULT_TIMEOUT = 2

def main():
    p = None

    try:
        p = spawn("make term", timeout=DEFAULT_TIMEOUT)
        p.logfile = sys.stdout

        for i in range(10):
            p.expect("Message received: 44")
            p.expect("Timeout")
    except TIMEOUT as exc:
        print(exc)
        return 1
    finally:
        if p and not p.terminate():
            print("SUCCESS")
            os.killpg(p.pid, signal.SIGKILL)

    return 0

if __name__ == "__main__":
    sys.exit(main())
