#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.suit with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
"""

import logging
import os
import sys
import unittest

from riotctrl.ctrl import RIOTCtrl

import riotctrl_shell.suit

from testrunner.spawn import flush_child


class TestSUIT(unittest.TestCase):
    DEBUG = bool(int(os.environ.get("DEBUG_TEST", "0")))

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        flush_child(cls.ctrl.term)
        cls.shell = riotctrl_shell.suit.SUIT(cls.ctrl)
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()

    def setUp(self):
        flush_child(self.ctrl.term)

    def test_suit_fetch(self):
        res = self.shell.suit_fetch("test://")
        # there is no output to parse so don't expect any specific
        assert res

    def test_suit_sequence_no(self):
        res = self.shell.suit_sequence_no()
        parser = riotctrl_shell.suit.SUITSequenceNoParser()
        seq_no = parser.parse(res)
        # check if value is the one in mock suit_storage_get_highest_seq_no()
        assert seq_no == 0xc0ffee


if __name__ == "__main__":
    unittest.main()
