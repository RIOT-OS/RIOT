# Copyright (C) 2018-19 Freie Universität Berlin
#               2017 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
#               2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import pexpect
import signal
import subprocess
import time
from traceback import extract_tb
from . import utils

PEXPECT_PATH = os.path.dirname(pexpect.__file__)
RIOTBASE = (os.environ.get('RIOTBASE') or
            os.path.abspath(os.path.join(os.path.dirname(__file__),
                                         "..", "..", "..")))

# Setting an empty 'TESTRUNNER_START_DELAY' environment variable use the
# default value (3)
MAKE_TERM_STARTED_DELAY = int(os.environ.get('TESTRUNNER_START_DELAY') or 3)

# Allow customizing test interactive settings with environment variables
TEST_INTERACTIVE_RETRIES = int(os.environ.get('TEST_INTERACTIVE_RETRIES') or 5)
TEST_INTERACTIVE_DELAY = int(os.environ.get('TEST_INTERACTIVE_DELAY') or 1)


def list_until(l, cond):
    return l[:([i for i, e in enumerate(l) if cond(e)][0])]


def find_exc_origin(exc_info):
    pos = list_until(extract_tb(exc_info),
                     lambda frame: frame[0].startswith(PEXPECT_PATH)
                     )[-1]
    return (pos[3], os.path.relpath(os.path.abspath(pos[0]), RIOTBASE), pos[1])


def setup_child(timeout=10, spawnclass=pexpect.spawnu, env=None, logfile=None):
    child = spawnclass("make cleanterm", env=env, timeout=timeout,
                       codec_errors='replace', echo=False)

    # on many platforms, the termprog needs a short while to be ready...
    time.sleep(MAKE_TERM_STARTED_DELAY)

    child.logfile = logfile

    try:
        subprocess.check_output(('make', 'reset'), env=env,
                                stderr=subprocess.PIPE)
    except subprocess.CalledProcessError:
        # make reset yields error on some boards even if successful
        pass

    # Handle synchronization if requested by the build system
    sync_child(child)

    return child


def teardown_child(child):
    try:
        os.killpg(os.getpgid(child.pid), signal.SIGKILL)
    except ProcessLookupError:
        print("Process already stopped")

    child.close()


def modules_list():
    modules = set(os.environ.get('USEMODULE', '').split(' '))
    modules.discard('')
    return modules


def sync_child(child):
    # Do a child synchronization if used by a module
    modules = modules_list()
    _test_utils_interactive_sync(child, modules)


def _test_utils_interactive_sync(child, modules):
    if 'test_utils_interactive_sync' not in modules:
        return

    utils.test_utils_interactive_sync(child,
                                      TEST_INTERACTIVE_RETRIES,
                                      TEST_INTERACTIVE_DELAY)
