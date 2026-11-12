# SPDX-FileCopyrightText: 2020 Freie Universität Berlin
# SPDX-FileCopyrightText: 2026 TU Dresden
# SPDX-License-Identifier: LGPL-2.1-only

import pexpect
import pytest

import riotctrl_shell.udp

from .common import init_ctrl, MockSpawn, MockRIOTCtrl

__author__ = "Martine S. Lenders <martine.lenders@tu-dresden.de>"


class ExpectMockSpawn(MockSpawn):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self._expect_sequence = None

    @property
    def expect_sequence(self):
        return self._expect_sequence  # pragma: no cover

    @expect_sequence.setter
    def expect_sequence(self, expect_sequence):
        self._expect_sequence = expect_sequence
        self._count = 0

    def expect(self, *args, **kwargs):
        if self._expect_sequence is None:
            raise RuntimeError("No expect_sequence given")  # pragma: no cover
        if self._count < len(self._expect_sequence):
            res = self._expect_sequence[self._count]
            if res is pexpect.TIMEOUT:
                raise res  # pragma: no cover
            self._count += 1
            return res
        raise RuntimeError("State error")  # pragma: no cover


class ExpectMockRIOTCtrl(MockRIOTCtrl):
    def start_term(self, **spawnkwargs):
        self.term = ExpectMockSpawn(ctrl=self)


def test_udp_server_start():
    ctrl = init_ctrl(output="Success: server was started")
    shell = riotctrl_shell.udp.UDPServer(ctrl)
    res = shell.udp_server_start(1337)
    assert res.startswith("Success:")
    assert ctrl.term.last_command == "udp server start 1337"


def test_udp_server_start_error():
    ctrl = init_ctrl()
    shell = riotctrl_shell.udp.UDPServer(ctrl)
    with pytest.raises(RuntimeError):
        shell.udp_server_start(1337)
    assert ctrl.term.last_command == "udp server start 1337"


def test_udp_server_stop():
    ctrl = init_ctrl()
    shell = riotctrl_shell.udp.UDPServer(ctrl)
    res = shell.udp_server_stop()
    # mock just returns last input
    assert res == "udp server stop"


@pytest.mark.parametrize(
    "expect_sequence,count,delay,expected",
    [
        ([0, 0, 0, 0, 0], 5, 10, 100.0),
        ([1, 1, 1, 1, 1], 5, 10, 0.0),
        ([1, 1, 1, 1, 0], 5, 0, 20.0),
        ([1] + ([0] * 9), 10, 10, 90.0),
        ([2] + ([0] * 5), 1, 10, 0.0),
        ([2] + ([0] * 5) + [2, 0, 0, pexpect.TIMEOUT], 2, 10, 50.0),
        ([2] + ([0] * 5) + [2, pexpect.TIMEOUT], 2, 10, 50.0),
    ],
)
def test_udp_server_check_output(expect_sequence, count, delay, expected):
    ctrl = ExpectMockRIOTCtrl("foobar", env={"BOARD": "native"})
    shell = riotctrl_shell.udp.UDPServer(ctrl)
    ctrl.start_term()
    ctrl.term.expect_sequence = expect_sequence
    assert shell.udp_server_check_output(count, delay) == expected
    ctrl.stop_term()


@pytest.mark.parametrize(
    "dest_addr,port,payload,count,delay_ms,expected",
    [
        (
            "ff02::1",
            1337,
            "Hallo World",
            None,
            None,
            'udp send [ff02::1]:1337 "Hallo World"',
        ),
        (
            "ff02::1",
            1337,
            "Hallo World",
            1000,
            None,
            'udp send [ff02::1]:1337 "Hallo World" 1000',
        ),
        (
            "ff02::1",
            1337,
            "Hallo World",
            1000,
            1000000,
            'udp send [ff02::1]:1337 "Hallo World" 1000 1000000',
        ),
        (
            "ff02::1",
            1337,
            b"\xab\xcd\xef",
            1000,
            1000000,
            'udp send -b [ff02::1]:1337 "abcdef" 1000 1000000',
        ),
        ("affe::1", 61616, 15, 1000, 0, 'udp send [affe::1]:61616 "15" 1000'),
        ("fe80::1", 52, 1000, 1000, 10, 'udp send [fe80::1]:52 "1000" 1000 10'),
    ],
)
def test_udp_client_send(dest_addr, port, payload, count, delay_ms, expected):
    ctrl = init_ctrl()
    shell = riotctrl_shell.udp.UDPClient(ctrl)
    res = shell.udp_client_send(dest_addr, port, payload, count, delay_ms)
    # mock just returns last input
    assert res == expected


def test_udp_client_send_error():
    ctrl = init_ctrl(output="Error: we don't want to send")
    shell = riotctrl_shell.udp.UDPClient(ctrl)
    with pytest.raises(RuntimeError):
        shell.udp_client_send("ff02::1", 1337, 10, 1000, 1000)
