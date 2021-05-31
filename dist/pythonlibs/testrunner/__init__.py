# Copyright (C) 2018-19 Freie Universität Berlin
#               2017 Cenk Gündoğan <cenk.guendogan@haw-hamburg.de>
#               2016 Kaspar Schleiser <kaspar@schleiser.de>
#               2014 Martine Lenders <mlenders@inf.fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from functools import partial
from traceback import print_tb
import pexpect

from .spawn import find_exc_origin, setup_child, teardown_child
from .unittest import PexpectTestCase   # noqa, F401 expose to users
from .utils import test_utils_interactive_sync # noqa, F401 expose to users

# Timeout for tests can be changed by setting RIOT_TEST_TIMEOUT to the desired
# value in the environment variables
# default value (10)
TIMEOUT = int(os.environ.get('RIOT_TEST_TIMEOUT') or 10)


def run(testfunc, timeout=TIMEOUT, echo=True, traceback=False):
    child = setup_child(timeout, env=os.environ,
                        logfile=sys.stdout if echo else None)
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
        print("Unexpected end of file in expect script at \"%s\" (%s:%d)" %
              trace)
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    finally:
        print("")
        teardown_child(child)
    return 0


def check_unittests(child, timeout=TIMEOUT, nb_tests=None):
    """ Check the number of unit tests that passed, and return the amount.

        If the amount of expected tests to pass is known, nd_tests can be set
        to perform an exact match against that number.
    """
    if nb_tests is None:
        # need escape sequence so don't use raw string
        res = child.expect(['OK(\033\\[21m)? \\((\\d+) tests\\)',
                            r'<Tests>(\d+)</Tests>'], timeout=timeout)
        if res == 0:
            _tests = int(child.match.group(2))
        else:
            _tests = int(child.match.group(1))
    else:
        _tests = int(nb_tests)
        # need escape sequence so don't use raw string
        res = child.expect(['OK(\033\\[21m)? \\({} tests\\)'.format(_tests),
                            r'<Tests>{}</Tests>'.format(_tests)],
                           timeout=timeout)
    if res == 1:
        res = child.expect([r'<Failures>\d+</Failures>', '</Statistics>'],
                           timeout=timeout)
        if res == 0:
            raise AssertionError("Unittests failed")
    return _tests


def run_check_unittests(timeout=TIMEOUT, echo=True, traceback=False,
                        nb_tests=None):
    _unittests_func = partial(check_unittests,
                              timeout=timeout, nb_tests=nb_tests)

    return run(_unittests_func, timeout, echo, traceback)
