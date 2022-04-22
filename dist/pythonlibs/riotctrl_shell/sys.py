# Copyright (C) 20 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
sys-related shell interactions

Defines sys-related shell command interactions
"""

import re
from riotctrl.shell import ShellInteraction, ShellInteractionParser


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


class SUITSequenceNoParser(ShellInteractionParser):
    def __init__(self):
        self.c_seq_no = re.compile(r"seq_no: (?P<seq_no>0x[0-9a-fA-F:]+)$")

    def parse(self, cmd_output):
        for line in cmd_output.splitlines():
            m = self.c_seq_no.search(line)
            if m is not None:
                return int(m.group("seq_no"), 16)
        return None


class SUIT(ShellInteraction):
    @ShellInteraction.check_term
    def suit_cmd(self, args=None, timeout=-1, async_=False):
        cmd = "suit"
        if args is not None:
            cmd += " {args}".format(args=" ".join(str(a) for a in args))
        return self.cmd(cmd, timeout=timeout, async_=False)

    def suit_sequence_no(self, timeout=-1, async_=False):
        return self.suit_cmd(args=("seq_no",), timeout=timeout, async_=async_)

    def suit_fetch(self, manifest, timeout=-1, async_=False):
        return self.suit_cmd(
            args=("fetch", f'"{manifest}"'), timeout=timeout, async_=async_
        )
