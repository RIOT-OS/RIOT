# SPDX-FileCopyrightText: 2019 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

"""
sys-related shell interactions

Defines sys-related shell command interactions
"""

from riotctrl.shell import ShellInteraction


class Help(ShellInteraction):
    """Help ShellInteraction"""

    @ShellInteraction.check_term
    def help(self, timeout=-1, async_=False):
        """Sends the reboot command via the terminal"""
        return self.cmd("help", timeout, async_)


class Reboot(ShellInteraction):
    """Reboot ShellInteraction"""

    @ShellInteraction.check_term
    def reboot(self, timeout=-1, async_=False):
        """Sends the reboot command via the terminal"""
        return self.cmd("reboot", timeout, async_)


class Version(ShellInteraction):
    """Version ShellInteraction"""

    @ShellInteraction.check_term
    def version(self, timeout=-1, async_=False):
        """Sends the reboot command via the terminal"""
        return self.cmd("version", timeout, async_)
