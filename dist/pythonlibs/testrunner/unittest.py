# SPDX-FileCopyrightText: 2018-2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import unittest
from testrunner import flush_child, setup_child, teardown_child


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

    @classmethod
    def flush_spawn(cls, expect_timeout=0.1):
        """Flush spawn from garbage that might be sent by device on start."""
        flush_child(cls.spawn, expect_timeout=expect_timeout)
