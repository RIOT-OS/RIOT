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

# By default never reset after the terminal is open unless explicitly requested
# through an environment variable.
TESTRUNNER_RESET_AFTER_TERM = int(os.environ.get('TESTRUNNER_RESET_AFTER_TERM')
                                  or '0')


def _reset_board(env):
    try:
        subprocess.check_output(('make', 'reset'), env=env,
                                stderr=subprocess.PIPE)
    except subprocess.CalledProcessError:
        # make reset yields error on some boards even if successful
        pass


def list_until(l, cond):
    return l[:([i for i, e in enumerate(l) if cond(e)][0])]


def find_exc_origin(exc_info):
    pos = list_until(extract_tb(exc_info),
                     lambda frame: frame[0].startswith(PEXPECT_PATH)
                     )[-1]
    return (pos[3], os.path.relpath(os.path.abspath(pos[0]), RIOTBASE), pos[1])


def setup_child(timeout=10, spawnclass=pexpect.spawnu, env=None, logfile=None):
    # Some boards can't be reset after a terminal is open. Therefore reset
    # before `cleanterm`.
    _reset_board(env)

    child = spawnclass("make cleanterm", env=env, timeout=timeout,
                       codec_errors='replace', echo=False)

    # on many platforms, the termprog needs a short while to be ready...
    time.sleep(MAKE_TERM_STARTED_DELAY)

    child.logfile = logfile

    # Handle synchronization if requested by the build system
    sync_child(child, env)

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


def sync_child(child, env):
    # Do a child synchronization if used by a module
    modules = modules_list()
    if 'test_utils_interactive_sync' in modules:
        utils.test_utils_interactive_sync(child,
                                          TEST_INTERACTIVE_RETRIES,
                                          TEST_INTERACTIVE_DELAY)
    # If requested also reset after opening the terminal, this should not be used
    # by any application since it breaks the tests for boards that do not support
    # this feature.
    elif TESTRUNNER_RESET_AFTER_TERM:
        _reset_board(env)
