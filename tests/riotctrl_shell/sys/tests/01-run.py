#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of riotctrl_shell.sys with RIOT shell.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import re
import unittest

from riotctrl.shell import ShellInteraction

import riotctrl_shell.sys

import riotctrl_shell_tests.testcase_base


class TestBase(riotctrl_shell_tests.testcase_base.TestBase):
    SHELL_CMD_NOT_FOUND = "shell: command not found: "


class TestNonExistingCommand(TestBase):
    """This class serves to establish ground truth for non-existing commands"""
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = ShellInteraction(cls.ctrl)

    def test_foobar23216(self):
        """Test if a random command returns the SHELL_CMD_NOT_FOUND output"""
        cmd_text = self.shell.cmd("foobar23216")
        assert self.SHELL_CMD_NOT_FOUND in cmd_text


class TestHelp(TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = riotctrl_shell.sys.Help(cls.ctrl)

    def test_help(self):
        help_text = self.shell.help()
        assert help_text
        assert self.SHELL_CMD_NOT_FOUND not in help_text
        self.logger.debug(help_text)


class TestReboot(TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = riotctrl_shell.sys.Reboot(cls.ctrl)

    def test_help(self):
        reboot_output = self.shell.reboot()
        assert self.SHELL_CMD_NOT_FOUND not in reboot_output
        # check for occurrence of test start-up print in output until next prompt
        assert re.search(r"Starting riotctrl_shell.sys test\s*", reboot_output)
        self.logger.debug(reboot_output)


class TestVersion(TestBase):
    @classmethod
    def init_shell_interaction(cls):
        cls.shell = riotctrl_shell.sys.Version(cls.ctrl)

    def test_help(self):
        version_text = self.shell.version()
        assert version_text
        assert self.SHELL_CMD_NOT_FOUND not in version_text
        self.logger.debug(version_text)


if __name__ == "__main__":
    unittest.main()
