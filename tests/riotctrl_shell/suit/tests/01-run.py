#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.suit with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import unittest

import riotctrl_shell.suit

import riotctrl_shell_tests.testcase_base


class TestSUIT(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = riotctrl_shell.suit.SUIT(cls.ctrl)

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
