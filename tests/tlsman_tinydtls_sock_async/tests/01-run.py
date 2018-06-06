#!/usr/bin/env python3

# Copyright (C) 2018 Raul Fuentes <raul.fuentes-samaniego@inria.fr>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys


def testfunc(child):
    child.expect_exact(u"Calling test_load_stack")
    child.expect_exact(u"Calling test_creating_context")
    child.expect_exact(u"Calling test_creating_channel")
    child.expect_exact(u"Calling test_sending_upper_data")
    child.expect_exact(u"Calling test_release_resources_IGNORING_ALERT")
    child.expect_exact(u"Calling test_reconnect_COMMON_ERROR")
    child.expect_exact(u"Calling test_sending_upper_data_MULTIPLE")
    child.expect_exact(u"Calling test_reconnect_CLOSING_SOCKETS")
    child.expect_exact(u"Calling test_closing_channel")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    timer = 30  # NOTE: DTLS timeout is around 10 seconds (and happens one time)
    sys.exit(run(testfunc, timeout=timer))
