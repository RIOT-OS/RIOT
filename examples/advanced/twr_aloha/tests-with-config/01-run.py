#! /usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import logging
import sys
import unittest

from riotctrl.ctrl import RIOTCtrl
from riotctrl_shell.sys import Reboot
from twr_shell import TwrCmd, TwrIfconfigParser


class TwrShell(Reboot, TwrCmd):
    """Convenience class inheriting from the Reboot and TwrCmd shell"""

    _netif = {
        "netif": None,
        "hwaddr": None,
        "hwaddr64": None,
        "panid": None,
        "channel": None,
    }

    def parse_netif(self):
        parser = TwrIfconfigParser()
        self._netif = parser.parse(self.ifconfig())

    def hwaddr(self):
        return self._netif["hwaddr"]

    def netif(self):
        return self._netif["netif"]

    def hwaddr64(self):
        return self._netif["hwaddr64"]

    def panid(self):
        return self._netif["panid"]

    def channel(self):
        return self._netif["channel"]


class TestTWRBase(unittest.TestCase):
    DEBUG = False

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        cls.shell = TwrShell(cls.ctrl)
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()


class TestTWR(TestTWRBase):
    def test_ifconfig(self):
        self.shell.parse_netif()
        assert self.shell.panid() == "DE:CA"
        assert self.shell.hwaddr() is not None
        assert self.shell.hwaddr64() is not None
        assert self.shell.channel() == "5"  # default channel is 5

    def test_listen(self):
        assert "[twr]: start listening" in self.shell.twr_listen(on=True)
        assert "[twr]: stop listening" in self.shell.twr_listen(on=False)

    def test_req(self):
        assert "[twr]: start ranging" in self.shell.twr_req()


if __name__ == "__main__":
    unittest.main()
