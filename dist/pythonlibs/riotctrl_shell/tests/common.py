# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


class MockSpawn():
    def __init__(self, *args, **kwargs):
        # set some expected attributes
        self.before = None
        self.echo = False

    def sendline(self, line, *args, **kwargs):
        # just echo last input for before (what replwrap is assembling output
        # from)
        self.before = line

    def expect_exact(self, *args, **kwargs):
        # always match on prompt with replwrap
        return 0


class MockRIOTCtrl():
    """
    Mock RIOT ctrl
    """
    def __init__(self, *args, **kwargs):
        self.term = MockSpawn()


def init_ctrl():
    rc = MockRIOTCtrl("foobar", env={"BOARD": "native"})
    return rc
