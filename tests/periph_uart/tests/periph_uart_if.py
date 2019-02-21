# Copyright (c) 2018 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module handles parsing of information from RIOT periph_uart test.
"""
try:
    from riot_pal import DutShell
except ImportError:
    raise ImportError('Cannot find riot_pal, try "pip install riot_pal"')


class PeriphUartIf(DutShell):
    """Interface to the node with periph_uart firmware."""

    def uart_init(self, dev, baud):
        """Initialize DUT's UART."""
        return self.send_cmd("init {} {}".format(dev, baud))

    def uart_mode(self, dev, data_bits, parity, stop_bits):
        """Setup databits, parity and stopbits."""
        return self.send_cmd(
            "mode {} {} {} {}".format(dev, data_bits, parity, stop_bits))

    def uart_send_string(self, dev, test_string):
        """Send data via DUT's UART."""
        return self.send_cmd("send {} {}".format(dev, test_string))
