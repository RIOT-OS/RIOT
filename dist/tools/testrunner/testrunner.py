#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os, signal, sys, subprocess
from pexpect import spawnu, TIMEOUT, EOF

def run(testfunc, timeout=10, echo=True):
    env = os.environ.copy()
    child = spawnu("make term", env=env, timeout=timeout)
    if echo:
        child.logfile = sys.stdout

    try:
        subprocess.check_output(('make', 'reset'), env=env,
                                stderr=subprocess.PIPE)
    except subprocess.CalledProcessError:
        # make reset yields error on some boards even if successful
        pass
    try:
        testfunc(child)
    except TIMEOUT:
        print("Timeout in expect script")
        return 1
    finally:
        print("")
        os.killpg(os.getpgid(child.pid), signal.SIGKILL)
        child.close()

    return 0
