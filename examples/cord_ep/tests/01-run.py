#! /usr/bin/env python3

# Copyright (C) 2022 Christian Amsüss <chrysn@fsfe.org>
#               2020-2022 HAW Hamburg
#               2020-2022 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import logging
import sys
import unittest
import subprocess
import time

from riotctrl.ctrl import RIOTCtrl
from riotctrl_shell.cord_ep import CordEp, CordEpRegistrationInfoParser

# Any additional parsers like riotctrl_shell.netif.Ifconfig would be added as
# base classes.
class Shell(CordEp):
    pass

# Copied from the turo tests -- it's likely this could be generalized away.
class RiotctrlBasedTest(unittest.TestCase):
    DEBUG = False

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        cls.shell = Shell(cls.ctrl)
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()


class TestCoRDEp(RiotctrlBasedTest):
    def test_cord(self):
        """
        Start a Resource Directory externally, and register with it, and wait
        long enough to see that a reregistration happens
        """
        aiocoap_rd = subprocess.Popen(
            ["aiocoap-rd"],
            stdout=None if self.DEBUG else subprocess.DEVNULL,
            stderr=None if self.DEBUG else subprocess.DEVNULL,
        )
        # Starting aiocoap-rd as a subprocess gives no readiness indication;
        # ginving some time to reach it on the first request rather than
        # waiting even longer for a retransmit
        time.sleep(0.5)

        try:
            res = self.shell.cord_ep_register(f"[ff02::1]")
            parser = CordEpRegistrationInfoParser()
            core_reg = parser.parse(res)
            ltime = core_reg['ltime']
            if ltime > 300:
                pytest.xfail(
                    f"CoRE RD lifetime is configured for {ltime}s "
                    "(> 5min). That's way to long for a test!"
                )
            # Wait long enough that the client would need to reregister
            time.sleep(ltime)
        finally:
            aiocoap_rd.terminate()
            aiocoap_rd.wait()
        self.ctrl.term.expect_exact(
            "RD endpoint event: successfully updated client registration",
        )


if __name__ == '__main__':
    unittest.main()
