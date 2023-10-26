# Copyright (C) 2021 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from statistics import mean
import riotctrl_shell.twr

from .common import init_ctrl

TEST_TWR_DATA = '{"t": 16236, "src": "98:9C", "dst": "02:A1", "d_cm": 477}'


def test_TwrData_from_json_str():
    data = riotctrl_shell.twr.TwrData.from_json_str(TEST_TWR_DATA)
    assert data.t == 16236
    assert data.src == "98:9C"
    assert data.dst == "02:A1"
    assert data.d_cm == 477


def test_twr_listen_on():
    rc = init_ctrl(output=riotctrl_shell.twr.TWR_LISTEN_ON_CMD_OUTPUT)
    si = riotctrl_shell.twr.TwrCmd(rc)
    cmd_output = si.twr_listen(on=True)
    assert riotctrl_shell.twr.TWR_LISTEN_ON_CMD_OUTPUT in cmd_output


def test_twr_listen_off():
    rc = init_ctrl(output=riotctrl_shell.twr.TWR_LISTEN_OFF_CMD_OUTPUT)
    si = riotctrl_shell.twr.TwrCmd(rc)
    cmd_output = si.twr_listen(on=False)
    assert riotctrl_shell.twr.TWR_LISTEN_OFF_CMD_OUTPUT in cmd_output


def test_twr_request_and_parser():
    _output = riotctrl_shell.twr.TWR_REQUEST_CMD_OUTPUT
    count = 10
    for _ in range(0, count):
        _output += f"\n\r{TEST_TWR_DATA}"
    rc = init_ctrl(output=_output)
    si = riotctrl_shell.twr.TwrCmd(rc)
    cmd_output = si.twr_request(count=count)
    assert riotctrl_shell.twr.TWR_REQUEST_CMD_OUTPUT in cmd_output
    parser = riotctrl_shell.twr.TwrRequestParser()
    d_cm = parser.parse(cmd_output)
    assert len(d_cm) == 10
    assert mean(d_cm) == 477


def test_twr_ifconfig_and_parser():
    output = """
Iface  3    HWaddr: 57:81  Channel: 5  NID: DE:CA

            Long HWaddr: 08:22:61:44:4D:83:57:81
            TX-Power: 8.0dBm  TC-PGdelay: 0xb5
"""
    rc = init_ctrl(output=output)
    si = riotctrl_shell.twr.TwrCmd(rc)
    cmd_output = si.ifconfig()
    parser = riotctrl_shell.twr.TwrIfconfigParser()
    netif = parser.parse(cmd_output)
    assert netif["netif"] == "3"
    assert netif["hwaddr"] == "57:81"
    assert netif["hwaddr64"] == "08:22:61:44:4D:83:57:81"
    assert netif["panid"] == "DE:CA"
    assert netif["channel"] == "5"
