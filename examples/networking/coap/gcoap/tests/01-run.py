#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Martine S. Lenders <mlenders@riot-os.org>
# SPDX-License-Identifier: LGPL-2.1-only

"""
Integration test to test interaction of gcoap ShellInteractions with RIOT shell.

This covers and copies the behaviour of the Shell ShellInteraction in
https://github.com/RIOT-OS/Release-Specs/blob/master/09-coap/test_spec09.py to
test its coverage with the gcoap example.

IF YOU NEED TO CHANGE ANYTHING IN THIS TEST,
YOU ALSO MIGHT NEED TO CHANGE THE RELEASE TEST FRAMEWORK.
See https://github.com/RIOT-OS/Release-Specs/
"""

import os
import re
import unittest

from riotctrl_shell.netif import Ifconfig
from riotctrl_shell.cord_ep import CordEp

import riotctrl_shell_tests.testcase_base


# CordEP and Ifconfig are tested in `tests/riotctrl_shell`, they are just
# included for maintainability with
# https://github.com/RIOT-OS/Release-Specs/blob/master/09-coap/test_spec09.py
class Shell(Ifconfig, CordEp):
    # pylint: disable=R0913
    def coap_get(
        self, addr, port, resource, confirmable=False, timeout=-1, async_=False
    ):
        cmd = "coap get "
        if confirmable:
            cmd += "-c "
        cmd += f"coap://[{addr}]:{port:d}{resource}"
        return self.cmd(cmd, timeout=timeout, async_=async_)


class TestCoAPGET(riotctrl_shell_tests.testcase_base.TestBase):
    @classmethod
    def setUpClass(cls):
        # Make sure `socket_zep` is used even if `USE_ZEP` gets lost
        # through the build system (e.g. with `TEST_EXECUTOR`)
        os.environ["USE_ZEP"] = "1"
        super().setUpClass()

    @classmethod
    def reset_board(cls):
        # rebuild application so that ZEP is used
        cls.ctrl.env = {
          "USE_ZEP": "1",
        }
        cls.ctrl.make_run(("flash", "-j", "reset"))

    @classmethod
    def init_shell_interaction(cls):
        cls.shell = Shell(cls.ctrl)

    def test_coap_get(self):
        # use intentionally random address, we don't want anything to be reachable
        # here, just test if the command works.
        res = self.shell.coap_get("2001:db8:535c::cc8d:92f0", 5683, "/time")
        assert re.search(r"gcoap_cli: sending msg ID \d+, \d+ bytes", res)

        res = self.shell.coap_get(
            "2001:db8:535c::cc8d:92f0", 5683, "/time", confirmable=True
        )
        assert re.search(r"gcoap_cli: sending msg ID \d+, \d+ bytes", res)


if __name__ == "__main__":
    unittest.main()
