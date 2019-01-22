# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module handles parsing of information from RIOT xtimer_cli test.
"""
import logging

from riot_pal import DutShell
from robot.version import get_version


class Xtimer(DutShell):
    """Interface to the a node with xtimer_cli firmware."""

    ROBOT_LIBRARY_SCOPE = 'TEST SUITE'
    ROBOT_LIBRARY_VERSION = get_version()

    FW_ID = 'xtimer_cli'

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def is_connected_to_board(self):
        """Checks if board is connected."""
        return self.i2c_get_id()["data"] == [self.FW_ID]

    def xtimer_now(self):
        """Get current timer ticks."""
        return self.send_cmd('xtimer_now')

    def get_metadata(self):
        """Get the metadata of the firmware."""
        return self.send_cmd('get_metadata')

    def get_command_list(self):
        """List of all commands."""
        cmds = list()
        cmds.append(self.get_metadata)
        cmds.append(self.xtimer_now)
        return cmds


def main():
    """Test for Xtimer."""

    logging.getLogger().setLevel(logging.DEBUG)
    try:
        xtimer = Xtimer()
        cmds = xtimer.get_command_list()
        logging.debug("======================================================")
        for cmd in cmds:
            cmd()
            logging.debug("--------------------------------------------------")
        logging.debug("======================================================")
    except Exception as exc:
        logging.debug(exc)


if __name__ == "__main__":
    main()
