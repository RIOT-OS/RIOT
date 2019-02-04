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
from traceback import print_tb
import inspect

import pexpect
import riotnode.node

from .spawn import find_exc_origin, setup_child, teardown_child
from .unittest import PexpectTestCase   # noqa, F401 expose to users


def run(testfunc, timeout=10, echo=True, traceback=False):
    node = riotnode.node.RIOTNode()
    node.start_term(timeout=timeout, logfile=sys.stdout if echo else None)
    node.reset()
    # 'expect/expect_exact' so path should be set
    pexpect_path = os.path.dirname(inspect.getfile(node.term.__class__))

    try:
        testfunc(node.term)
    except pexpect.TIMEOUT:
        trace = find_exc_origin(sys.exc_info()[2], pexpect_path=pexpect_path)
        print("Timeout in expect script at \"%s\" (%s:%d)" % trace)
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    except pexpect.EOF:
        trace = find_exc_origin(sys.exc_info()[2], pexpect_path=pexpect_path)
        print("Unexpected end of file in expect script at \"%s\" (%s:%d)" %
              trace)
        if traceback:
            print_tb(sys.exc_info()[2])
        return 1
    finally:
        print("")
        node.stop_term()
    return 0
