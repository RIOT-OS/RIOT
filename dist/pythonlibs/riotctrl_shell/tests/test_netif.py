# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pytest
import riotctrl_shell.netif

from .common import init_ctrl


def test_ifconfig_stats_parser():
    cmd_output = """
          Statistics for Layer 2
            RX packets 4  bytes 400
            TX packets 1 (Multicast: 1)  bytes 78
            TX succeeded 1 errors 0
          Statistics for IPv6
            RX packets 4  bytes 344
            TX packets 1 (Multicast: 1)  bytes 64
            TX succeeded 1 errors 0"""
    parser = riotctrl_shell.netif.IfconfigStatsParser()
    res = parser.parse(cmd_output)
    assert len(res) == 2
    assert res["Layer 2"]["rx"]["packets"] == 4
    assert res["Layer 2"]["rx"]["bytes"] == 400
    assert res["Layer 2"]["tx"]["packets"] == 1
    assert res["Layer 2"]["tx"]["multicast"] == 1
    assert res["Layer 2"]["tx"]["bytes"] == 78
    assert res["Layer 2"]["tx"]["succeeded"] == 1
    assert res["Layer 2"]["tx"]["errors"] == 0
    assert res["IPv6"]["rx"]["packets"] == 4
    assert res["IPv6"]["rx"]["bytes"] == 344
    assert res["IPv6"]["tx"]["packets"] == 1
    assert res["IPv6"]["tx"]["multicast"] == 1
    assert res["IPv6"]["tx"]["bytes"] == 64
    assert res["IPv6"]["tx"]["succeeded"] == 1
    assert res["IPv6"]["tx"]["errors"] == 0


@pytest.mark.parametrize(
    "args,expected",
    [((), "ifconfig"), (("foobar",), "ifconfig foobar")]
)
def test_ifconfig_list(args, expected):
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_list(*args)
    # mock just returns last input
    assert res == expected


@pytest.mark.parametrize(
    "args,expected",
    [((), "ifconfig"), (("foobar",), "ifconfig foobar")]
)
def test_ifconfig_cmd_empty(args, expected):
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_cmd(*args)
    assert res == expected


def test_ifconfig_cmd_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(ValueError):
        si.ifconfig_cmd(args=("test", "12345"))


def test_ifconfig_help():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_help("foobar")
    assert res == "ifconfig foobar help"


@pytest.mark.parametrize(
    "option,value,expected",
    [("addr", "42:de:ad:c0:ff:ee",
      "ifconfig foobar set addr 42:de:ad:c0:ff:ee"),
     ("chan", 17, "ifconfig foobar set chan 17")]
)
def test_ifconfig_set(option, value, expected):
    rc = init_ctrl(output="success: option set")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_set("foobar", option, value)
    assert res == "success: option set"
    assert rc.term.last_command == expected


def test_ifconfig_set_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_set("foobar", "addr", "42:de:ad:c0:ff:ee")
    assert rc.term.last_command == "ifconfig foobar set addr 42:de:ad:c0:ff:ee"


def test_ifconfig_up():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    si.ifconfig_up("foobar")
    assert rc.term.last_command == "ifconfig foobar up"


def test_ifconfig_up_error():
    rc = init_ctrl("error: unable to set link foobar")
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_up("foobar")
    assert rc.term.last_command == "ifconfig foobar up"


def test_ifconfig_down():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    si.ifconfig_down("foobar")
    assert rc.term.last_command == "ifconfig foobar down"


def test_ifconfig_down_error():
    rc = init_ctrl("error: unable to set link foobar")
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_down("foobar")
    assert rc.term.last_command == "ifconfig foobar down"


@pytest.mark.parametrize(
    "kwargs,expected",
    [({"netif": "foobar", "addr": "dead:coff:ee::/64"},
      "ifconfig foobar add dead:coff:ee::/64"),
     ({"netif": "foobar", "addr": "dead:coff:ee::/64", "anycast": False},
      "ifconfig foobar add dead:coff:ee::/64"),
     ({"netif": "foobar", "addr": "dead:coff:ee::/64", "anycast": True},
      "ifconfig foobar add dead:coff:ee::/64 anycast")]
)
def test_ifconfig_add(kwargs, expected):
    rc = init_ctrl(output="success: added address to interface")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_add(**kwargs)
    assert res == "success: added address to interface"
    assert rc.term.last_command == expected


@pytest.mark.parametrize(
    "kwargs,expected",
    [({"netif": "foobar", "addr": "dead:coff:ee::/64"},
      "ifconfig foobar add dead:coff:ee::/64"),
     ({"netif": "foobar", "addr": "dead:coff:ee::/64", "anycast": False},
      "ifconfig foobar add dead:coff:ee::/64"),
     ({"netif": "foobar", "addr": "dead:coff:ee::/64", "anycast": True},
      "ifconfig foobar add dead:coff:ee::/64 anycast")]
)
def test_ifconfig_add_error(kwargs, expected):
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_add(**kwargs)
    assert rc.term.last_command == expected


def test_ifconfig_del():
    rc = init_ctrl(output="success: removed address from interface")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_del("foobar", "dead:coff:ee::/64")
    assert res == "success: removed address from interface"
    assert rc.term.last_command == "ifconfig foobar del dead:coff:ee::/64"


def test_ifconfig_del_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_del("foobar", "dead:coff:ee::/64")
    assert rc.term.last_command == "ifconfig foobar del dead:coff:ee::/64"


@pytest.mark.parametrize(
    "kwargs,expected",
    [({"netif": "foobar", "flag": "6lo"}, "ifconfig foobar 6lo"),
     ({"netif": "foobar", "flag": "6lo", "enable": True},
      "ifconfig foobar 6lo"),
     ({"netif": "foobar", "flag": "6lo", "enable": False},
      "ifconfig foobar -6lo")]
)
def test_ifconfig_flag(kwargs, expected):
    rc = init_ctrl(output="success: set option")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_flag(**kwargs)
    assert res == "success: set option"
    assert rc.term.last_command == expected


def test_ifconfig_flag_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_flag("foobar", "6lo", False)
    assert rc.term.last_command == "ifconfig foobar -6lo"


def test_ifconfig_l2filter_add():
    rc = init_ctrl(output="successfully added address to filter")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_l2filter_add("foobar", "ab:cd:ef:01:23:45")
    assert res == "successfully added address to filter"
    assert rc.term.last_command == \
        "ifconfig foobar l2filter add ab:cd:ef:01:23:45"


def test_ifconfig_l2filter_add_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_l2filter_add("foobar", "ab:cd:ef:01:23:45")
    assert rc.term.last_command == \
        "ifconfig foobar l2filter add ab:cd:ef:01:23:45"


def test_ifconfig_l2filter_del():
    rc = init_ctrl(output="successfully removed address to filter")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_l2filter_del("foobar", "ab:cd:ef:01:23:45")
    assert res == "successfully removed address to filter"
    assert rc.term.last_command == \
        "ifconfig foobar l2filter del ab:cd:ef:01:23:45"


def test_ifconfig_l2filter_del_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_l2filter_del("foobar", "ab:cd:ef:01:23:45")
    assert rc.term.last_command == \
        "ifconfig foobar l2filter del ab:cd:ef:01:23:45"


def test_ifconfig_stats():
    rc = init_ctrl(output="          Statistics for Layer 2\n          RX ...")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_stats("foobar", "l2")
    assert res.startswith("          Statistics for ")
    assert rc.term.last_command == \
        "ifconfig foobar stats l2"


def test_ifconfig_stats_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_stats("foobar", "l2")
    assert rc.term.last_command == \
        "ifconfig foobar stats l2"


def test_ifconfig_stats_reset():
    rc = init_ctrl(output="Reset statistics for module Layer 2!")
    si = riotctrl_shell.netif.Ifconfig(rc)
    res = si.ifconfig_stats_reset("foobar", "l2")
    assert res == "Reset statistics for module Layer 2!"
    assert rc.term.last_command == \
        "ifconfig foobar stats l2 reset"


def test_ifconfig_stats_reset_error():
    rc = init_ctrl()
    si = riotctrl_shell.netif.Ifconfig(rc)
    with pytest.raises(RuntimeError):
        si.ifconfig_stats_reset("foobar", "l2")
    assert rc.term.last_command == \
        "ifconfig foobar stats l2 reset"


def test_txtsnd():
    rc = init_ctrl()
    si = riotctrl_shell.netif.TXTSnd(rc)
    res = si.netif_txtsnd("foobar", "bcast", "abcdef")
    assert res == "txtsnd foobar bcast abcdef"


@pytest.mark.parametrize(
    "error_msg",
    ["error: foobar", "usage: txtsnd foobar"]
)
def test_txtsnd_error(error_msg):
    rc = init_ctrl(output=error_msg)
    si = riotctrl_shell.netif.TXTSnd(rc)
    with pytest.raises(RuntimeError):
        si.netif_txtsnd("foobar", "bcast", "abcdef")
    assert rc.term.last_command == "txtsnd foobar bcast abcdef"
