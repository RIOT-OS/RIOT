# Copyright (C) 2020 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pytest
import riotctrl_shell.cord_ep

from .common import init_ctrl


@pytest.mark.parametrize(
    "input,expected",
    [("the registration interface is '/resourcedirectory'",
      "/resourcedirectory"),
     ("error: unable to discover registration interface", None)]
)
def test_cord_ep_discover_parser(input, expected):
    parser = riotctrl_shell.cord_ep.CordEpDiscoverParser()
    discover_res = parser.parse(input)
    assert discover_res == expected


@pytest.mark.parametrize(
    "proto,port", [("coap", ":5683"), ("coaps", ":5684")]
)
def test_cord_ep_reg_info_parser(proto, port):
    test_output = f"""
Registering with RD now, this may take a short while...
RD endpoint event: now registered with a RD
registration successful

CoAP RD connection status:
RD address: {proto}://[fe80::4494:71ff:fec4:9cac]{port}
   ep name: RIOT-3F2423233F242323
  lifetime: 60s
    reg if: /resourcedirectory
  location: /reg/1/"""

    parser = riotctrl_shell.cord_ep.CordEpRegistrationInfoParser()
    reg_res = parser.parse(test_output)
    assert reg_res
    assert reg_res["rdaddr"] == f"{proto}://[fe80::4494:71ff:fec4:9cac]{port}"
    assert reg_res["ltime"] == 60
    assert reg_res["regif"] == "/resourcedirectory"
    assert reg_res["location"] == "/reg/1/"


def test_cord_ep_parser_empty():
    reginfo_parser = riotctrl_shell.cord_ep.CordEpRegistrationInfoParser()
    assert not reginfo_parser.parse("")
    discover_parser = riotctrl_shell.cord_ep.CordEpDiscoverParser()
    assert not discover_parser.parse("")


@pytest.mark.parametrize(
    "uri,regif,expected",
    [
        ("[fe80::1]", None, "cord_ep register [fe80::1]"),
        ("[fe80::1%iface0]", None, "cord_ep register [fe80::1%iface0]"),
        ("[fe80::1]:5684", None, "cord_ep register [fe80::1]:5684"),
        ("[fe80::1]", "/regif", "cord_ep register [fe80::1] /regif"),
    ]
)
def test_cord_ep_register(uri, regif, expected):
    rc = init_ctrl()
    si = riotctrl_shell.cord_ep.CordEp(rc)
    res = si.cord_ep_register(uri, regif)
    assert res == expected


@pytest.mark.parametrize(
    "uri,expected",
    [
        ("[fe80::1]", "cord_ep discover [fe80::1]"),
        ("[fe80::1]:5684", "cord_ep discover [fe80::1]:5684"),
        ("[fe80::1%iface0]", "cord_ep discover [fe80::1%iface0]"),
    ]
)
def test_cord_ep_discover(uri, expected):
    rc = init_ctrl()
    si = riotctrl_shell.cord_ep.CordEp(rc)
    res = si.cord_ep_discover(uri)
    assert res == expected


@pytest.mark.parametrize(
    "method,expected",
    [
        ("cord_ep_info", "cord_ep info"),
        ("cord_ep_update", "cord_ep update"),
        ("cord_ep_remove", "cord_ep remove")
    ]
)
def test_cord_ep(method, expected):
    rc = init_ctrl()
    si = riotctrl_shell.cord_ep.CordEp(rc)
    res = getattr(si, method)()
    assert res == expected


@pytest.mark.parametrize(
    "error_msg",
    ["error: foobar", "usage: cord_ep register foobar"]
)
def test_cord_ep_error(error_msg):
    rc = init_ctrl(error_msg)
    si = riotctrl_shell.cord_ep.CordEp(rc)
    with pytest.raises(RuntimeError):
        si.cord_ep_register("[abcde]:1234", "lalala")
    assert rc.term.last_command == "cord_ep register [abcde]:1234 lalala"
