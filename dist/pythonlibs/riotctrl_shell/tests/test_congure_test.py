# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pytest
import riotctrl_shell.congure_test

from .common import init_ctrl


@pytest.fixture
def shell():
    rc = init_ctrl()
    yield riotctrl_shell.congure_test.CongureTest(rc)


@pytest.fixture
def success_shell():
    rc = init_ctrl()
    rc.output = '{"success":null}'
    yield riotctrl_shell.congure_test.CongureTest(rc)


def test_congure_setup(shell):
    res = shell.setup(0)
    assert res == 'cong_setup 0'


def test_congure_clear(shell):
    res = shell.clear()
    assert res == 'cong_clear'


def test_congure_init(shell):
    res = shell.init(0xabcdef)
    assert res == 'cong_init 0xabcdef'


def test_congure_inter_msg_interval(shell):
    res = shell.inter_msg_interval(12345)
    assert res == 'cong_imi 12345'


def test_congure_add_msg(shell):
    res = shell.add_msg(send_time=543234, size=664343, resends=43)
    assert res == 'cong_add_msg 543234 664343 43'


def test_congure_msgs_reset(shell):
    res = shell.msgs_reset()
    assert res == 'cong_msgs_reset'


def test_congure_report(shell):
    res = shell.report('foobar', 'a', 'b', 'c')
    assert res == 'cong_report foobar a b c'


def test_congure_report_msg_sent(shell):
    res = shell.report_msg_sent(12364)
    assert res == 'cong_report msg_sent 12364'


def test_congure_report_msg_discarded(shell):
    res = shell.report_msg_discarded(12364)
    assert res == 'cong_report msg_discarded 12364'


def test_congure_report_msgs_lost_base(shell):
    res = shell.report_msgs_lost_base()
    assert res == 'cong_report msgs_lost '


def test_congure_report_msgs_lost(success_shell):
    res = success_shell.report_msgs_lost([
        {'send_time': 1545245236, 'size': 12434, 'resends': 32},
        {'send_time': 175344346, 'size': 34323, 'resends': 42},
    ])
    assert res == '{"success":null}'
    assert success_shell.riotctrl.term.commands == [
        'cong_add_msg 1545245236 12434 32',
        'cong_add_msg 175344346 34323 42',
        'cong_report msgs_lost ',
    ]


def test_congure_report_msgs_timeout_base(shell):
    res = shell.report_msgs_timeout_base()
    assert res == 'cong_report msgs_timeout '


def test_congure_report_msgs_timeout(success_shell):
    res = success_shell.report_msgs_timeout([
        {'send_time': 1545245236, 'size': 12434, 'resends': 32},
        {'send_time': 175344346, 'size': 34323, 'resends': 42},
    ])
    assert res == '{"success":null}'
    assert success_shell.riotctrl.term.commands == [
        'cong_add_msg 1545245236 12434 32',
        'cong_add_msg 175344346 34323 42',
        'cong_report msgs_timeout ',
    ]


def test_congure_report_msg_acked_base(shell):
    res = shell.report_msg_acked_base(
        ack_recv_time=12354356, ack_id=4314, ack_size=5632, ack_clean=True,
        ack_wnd=45645, ack_delay=12345
    )
    assert res == 'cong_report msg_acked 12354356 4314 5632 1 45645 12345'


def test_congure_report_msg_acked(success_shell):
    res = success_shell.report_msg_acked(
        msg={'send_time': 325242435, 'size': 34321, 'resends': 4},
        ack={'recv_time': 325243543, 'id': 455763452, 'size': 4242,
             'clean': 0, 'wnd': 45645, 'delay': 12345},
    )
    assert res == '{"success":null}'
    assert success_shell.riotctrl.term.commands == [
        'cong_add_msg 325242435 34321 4',
        'cong_report msg_acked 325243543 455763452 4242 0 45645 12345',
    ]


def test_congure_report_ecn_ce(shell):
    res = shell.report_ecn_ce(583424522)
    assert res == 'cong_report ecn_ce 583424522'
