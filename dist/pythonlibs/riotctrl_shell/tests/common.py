# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import contextlib


class MockSpawn():
    def __init__(self, ctrl, *args, **kwargs):
        self.ctrl = ctrl
        self.last_command = None
        # set some expected attributes
        self.before = None
        self.echo = False

    def sendline(self, line, *args, **kwargs):
        self.last_command = line
        if self.ctrl.output is None:
            # just echo last input for before (what replwrap is assembling
            # output from)
            self.before = line
        else:
            # use pre-configured output in case command expects a specific
            # output
            self.before = self.ctrl.output

    def expect_exact(self, *args, **kwargs):
        # always match on prompt with replwrap
        return 0


class MockRIOTCtrl():
    """
    Mock RIOT ctrl
    """
    def __init__(self, *args, **kwargs):
        self.term = None
        self.output = None
        self.last_command = None

    @contextlib.contextmanager
    def run_term(self, reset=True, **startkwargs):
        try:
            self.start_term(**startkwargs)
            yield self.term
        finally:
            self.stop_term()

    def start_term(self, **spawnkwargs):
        self.term = MockSpawn(self)

    def stop_term(self):
        pass


def init_ctrl(output=None):
    rc = MockRIOTCtrl("foobar", env={"BOARD": "native"})
    rc.output = output
    return rc
