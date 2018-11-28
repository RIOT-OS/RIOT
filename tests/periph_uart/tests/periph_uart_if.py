# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
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
    FW_ID = 'periph_uart'
    DEFAULT_DEV = 1
    DEFAULT_BAUD = 115200

    def uart_init(self, dev=DEFAULT_DEV, baud=DEFAULT_BAUD):
        """Initialize DUT's UART."""
        return self.send_cmd("init {} {}".format(dev, baud))

    def uart_send_string(self, test_string, dev=DEFAULT_DEV):
        """Send data via DUT's UART."""
        return self.send_cmd("send {} {}".format(dev, test_string))

    def uart_get_id(self):
        """Get the id of the fw."""
        return self.send_cmd('uart_get_id')
