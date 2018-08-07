# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module handles parsing of information from RIOT periph_uart test.
"""
from if_lib.dut_shell import DutShell


class PeriphUartIf(DutShell):
    """Interface to the node with periph_uart firmware."""

    def uart_init(self, dev, baud):
        """Initialize DUT's UART."""
        return self.send_cmd("init {} {}".format(dev, baud))

    def uart_send_string(self, dev, test_string):
        """Send data via DUT's UART."""
        return self.send_cmd("send {} {}".format(dev, test_string))
