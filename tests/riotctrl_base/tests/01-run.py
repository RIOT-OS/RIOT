#! /usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys

from riotctrl.ctrl import RIOTCtrl
from riotctrl.shell import ShellInteraction
from riotctrl.shell.json import RapidJSONShellInteractionParser


def test_riotctrl():
    ctrl = RIOTCtrl()
    with ctrl.run_term(reset=True):
        ctrl.term.logfile = sys.stdout
        ctrl.term.expect(r'This is RIOT! \(Version: (.+)\)\r\n')
        version = ctrl.term.match.group(1)
        shell = ShellInteraction(ctrl)
        parser = RapidJSONShellInteractionParser()
        res = shell.cmd('empty')
        assert not res.strip()
        res = shell.cmd('version')
        assert res.strip() == version
        count = 0
        for line in shell.cmd('app_metadata').strip().splitlines():
            line = line.strip()
            assert line
            count += 1
            # just check if parsable
            parser.parse(line)
        assert count > 0


if __name__ == "__main__":
    test_riotctrl()
