# Copyright (C) 2017 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
#               2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import signal
import sys
import subprocess
import time
from traceback import extract_tb, print_tb

import pexpect

PEXPECT_PATH = os.path.dirname(pexpect.__file__)
RIOTBASE = os.environ['RIOTBASE'] or \
    os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "..", ".."))

# Setting an empty 'TESTRUNNER_START_DELAY' environment variable use the
# default value (3)
MAKE_TERM_STARTED_DELAY = int(os.environ.get('TESTRUNNER_START_DELAY') or 3)


def list_until(l, cond):
    return l[:([i for i, e in enumerate(l) if cond(e)][0])]


def find_exc_origin(exc_info):
    pos = list_until(extract_tb(exc_info),
                     lambda frame: frame[0].startswith(PEXPECT_PATH)
                     )[-1]
    return (pos[3], os.path.relpath(os.path.abspath(pos[0]), RIOTBASE), pos[1])


def run(testfunc, timeout=10, echo=True, traceback=False):
    env = os.environ.copy()
    child = pexpect.spawnu("make term", env=env, timeout=timeout, codec_errors='replace')

    # on many platforms, the termprog needs a short while to be ready...
    time.sleep(MAKE_TERM_STARTED_DELAY)

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
    except pexpect.TIMEOUT:
        trace = find_exc_origin(sys.exc_info()[2])
        print("Timeout in expect script at \"%s\" (%s:%d)" % trace)
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    except pexpect.EOF:
        trace = find_exc_origin(sys.exc_info()[2])
        print("Unexpected end of file in expect script at \"%s\" (%s:%d)" % trace)
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    finally:
        print("")
        try:
            os.killpg(os.getpgid(child.pid), signal.SIGKILL)
        except ProcessLookupError:
            print("Process already stopped")

        child.close()
    return 0
