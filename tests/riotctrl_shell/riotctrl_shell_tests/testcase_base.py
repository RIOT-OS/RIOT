# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

import logging
import os
import sys
import unittest

from riotctrl.ctrl import RIOTCtrl
from testrunner.spawn import flush_child

"""Base for the riotctrl_shell test cases"""


class TestBase(unittest.TestCase):
    DEBUG = bool(int(os.environ.get("DEBUG_TEST", "0")))

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        flush_child(cls.ctrl.term)
        cls.init_shell_interaction()
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()

    def setUp(self):
        flush_child(self.ctrl.term)

    @classmethod
    def init_shell_interaction(cls):
        pass
