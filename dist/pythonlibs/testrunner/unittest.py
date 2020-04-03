# Copyright (C) 2018-19 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import unittest
from testrunner import setup_child, teardown_child


class PexpectTestCase(unittest.TestCase):
    TIMEOUT = 10
    LOGFILE = None

    """A unittest TestCase providing a pexpect spawn object to it's tests
    """
    @classmethod
    def setUpClass(cls):
        cls.spawn = setup_child(cls.TIMEOUT, logfile=cls.LOGFILE)

    @classmethod
    def tearDownClass(cls):
        teardown_child(cls.spawn)
