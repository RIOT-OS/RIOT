# Copyright (C) 20 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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
