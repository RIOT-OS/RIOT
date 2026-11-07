# SPDX-FileCopyrightText: 2022 Francisco Molina <femolina@uc.cl>
# SPDX-License-Identifier: LGPL-2.1-only

import riotctrl_shell.suit

from .common import init_ctrl


def test_suit_fetch():
    rc = init_ctrl()
    si = riotctrl_shell.suit.SUIT(rc)
    res = si.suit_fetch("coap://[2001:db8::2:1]/manifest")
    # mock just returns last input
    assert res == 'suit fetch "coap://[2001:db8::2:1]/manifest"'


def test_suit_sequence_no():
    rc = init_ctrl(
        output="""
seq_no: 0x12345678
    """
    )
    si = riotctrl_shell.suit.SUIT(rc)
    res = si.suit_sequence_no()
    parser = riotctrl_shell.suit.SUITSequenceNoParser()
    # mock just returns last input
    assert parser.parse(res) == 0x12345678
