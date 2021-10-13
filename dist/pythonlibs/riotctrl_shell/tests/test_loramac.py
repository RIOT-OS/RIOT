# Copyright (C) 2021 Inria
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pytest
import riotctrl_shell.loramac

from .common import init_ctrl


def test_loramac_join_otaa_failed():
    rc = init_ctrl(output="""
loramac join otaa
Join procedure failed!
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    with pytest.raises(RuntimeError) as error:
        shell.loramac_join('otaa')
    assert "Join procedure failed!" in str(error.value)


def test_loramac_join_otaa_succeed():
    rc = init_ctrl(output="""
loramac join otaa
Join procedure succeeded!
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    res = shell.loramac_join('otaa')
    assert "Join procedure succeeded!" in res


def test_loramac_join_abp_succeed():
    rc = init_ctrl(output="""
loramac join abp
Join procedure succeeded!
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    res = shell.loramac_join('abp')
    assert "Join procedure succeeded!" in res


def test_loramac_tx_not_joined():
    rc = init_ctrl(output="""
loramac tx "Hello RIOT!"
Cannot send: not joined
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    with pytest.raises(RuntimeError) as error:
        shell.loramac_tx("Hello RIOT!")
    assert "Cannot send: not joined" in str(error.value)


def test_loramac_tx_succeed():
    rc = init_ctrl(output="""
loramac tx "Hello RIOT!"
Message sent with success
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    res = shell.loramac_tx("Hello RIOT!")
    assert "Message sent with success" in res


def test_loramac_help_parser_has_eeprom_no():
    rc = init_ctrl(output="""
loramac help
Usage: loramac <get|set|join|tx|link_check>
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    parser = riotctrl_shell.loramac.LoramacHelpParser()
    assert not parser.has_eeprom(shell.loramac_help())


def test_loramac_help_parser_has_eeprom_yes():
    rc = init_ctrl(output="""
loramac help
Usage: loramac <get|set|join|tx|link_check|save|erase>
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    parser = riotctrl_shell.loramac.LoramacHelpParser()
    assert parser.has_eeprom(shell.loramac_help())


def test_loramac_set_fail():
    rc = init_ctrl(output="""
loramac set dr 123
Usage: loramac set dr <0..16>
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    with pytest.raises(RuntimeError) as error:
        shell.loramac_set('dr', 123)
    assert "Usage: loramac set dr <0..16>" in str(error.value)


def test_loramac_set_succeed():
    rc = init_ctrl(output="loramac set dr 5")
    shell = riotctrl_shell.loramac.Loramac(rc)
    shell.loramac_set('dr', 5)
    assert rc.term.last_command == "loramac set dr 5"


def test_loramac_get_fail():
    rc = init_ctrl(output="""
loramac get dxe
Usage: loramac get <deveui|appeui|appkey|appskey|nwkskey|devaddr|class|dr|adr|public|netid|tx_power|rx2_freq|rx2_dr|ul_cnt>
    """)  # noqa: E501
    shell = riotctrl_shell.loramac.Loramac(rc)
    with pytest.raises(RuntimeError) as error:
        shell.loramac_get('dxe')
    assert "Usage: loramac get" in str(error.value)


def test_loramac_get_succeed():
    rc = init_ctrl(output="""
loramac get dr
DATARATE: 5
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    res = shell.loramac_get('dr')
    assert "Usage: loramac get" not in res


def test_loramac_uplink_counter_parser():
    rc = init_ctrl(output="""
loramac get ul_cnt
Uplink Counter: 32
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    parser = riotctrl_shell.loramac.LoramacUpLinkCounterParser()
    assert parser.uplink_count(shell.loramac_get('ul_cnt')) == '32'


def test_loramac_uplink_counter_parser_fail():
    rc = init_ctrl(output="""
loramac get ul_cnt
Uplink Counter: asdfasdf
    """)
    shell = riotctrl_shell.loramac.Loramac(rc)
    parser = riotctrl_shell.loramac.LoramacUpLinkCounterParser()
    with pytest.raises(RuntimeError):
        parser.uplink_count(shell.loramac_get('ul_cnt'))
