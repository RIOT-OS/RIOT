# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

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


def test_suit_fetch():
    rc = init_ctrl()
    si = riotctrl_shell.sys.SUIT(rc)
    res = si.suit_fetch("coap://[2001:db8::2:1]/manifest")
    # mock just returns last input
    assert res == 'suit fetch "coap://[2001:db8::2:1]/manifest"'


def test_suit_sequence_no():
    rc = init_ctrl(
        output="""
seq_no: 0x12345678
    """
    )
    si = riotctrl_shell.sys.SUIT(rc)
    res = si.suit_sequence_no()
    parser = riotctrl_shell.sys.SUITSequenceNoParser()
    # mock just returns last input
    assert parser.parse(res) == 0x12345678
