# SPDX-FileCopyrightText: 2020 Freie Universität Berlin
# SPDX-License-Identifier: LGPL-2.1-only

import riotctrl_shell.sys

from .common import init_ctrl


def test_help():
    rc = init_ctrl()
    si = riotctrl_shell.sys.Help(rc)
    res = si.help()
    # mock just returns last input
    assert res == "help"


def test_reboot():
    rc = init_ctrl()
    si = riotctrl_shell.sys.Reboot(rc)
    res = si.reboot()
    # mock just returns last input
    assert res == "reboot"


def test_version():
    rc = init_ctrl()
    si = riotctrl_shell.sys.Version(rc)
    res = si.version()
    # mock just returns last input
    assert res == "version"
