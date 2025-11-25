#!/usr/bin/env python3

# Copyright (C) 2021 Benjamin Valentin
# Copyright (C) 2023 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import contextlib
import os
import subprocess
import sys
import time

from subprocess import Popen
from riotctrl_shell.gnrc import GNRCICMPv6Echo, GNRCICMPv6EchoParser, GNRCIPv6NIB
from riotctrl_shell.netif import Ifconfig
from riotctrl.ctrl import RIOTCtrlBoardFactory
from riotctrl_ctrl import native
from riotctrl_shell.netif import IfconfigListParser

RIOTBASE = os.getenv("RIOTBASE", os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../")))
ZEP_DISPATCH_PATH = os.path.join(RIOTBASE, "dist/tools/zep_dispatch/bin/zep_dispatch")
PARSERS = {
    "ping6": GNRCICMPv6EchoParser(),
    "ifconfig": IfconfigListParser(),
}


class RIOTCtrlAppFactory(RIOTCtrlBoardFactory):

    def __init__(self, board='native'):
        super().__init__(board_cls={
            board: native.NativeRIOTCtrl,
        })
        self.board = board
        self.ctrl_list = list()

    def __enter__(self):
        return self

    def __exit__(self, *exc):
        for ctrl in self.ctrl_list:
            ctrl.stop_term()

    def get_shell(self, application_directory='.', env=None):
        if env is None:
            env = {'BOARD': self.board}
        # retrieve a RIOTCtrl Object
        ctrl = super().get_ctrl(
            env=env,
            application_directory=application_directory
        )
        # append ctrl to list
        self.ctrl_list.append(ctrl)
        # start terminal
        ctrl.start_term()
        ctrl.term.logfile = sys.stdout
        # return ctrl with started terminal
        return Shell(ctrl)

    def get_shells(self, num=1):
        terms = []
        for i in range(num):
            terms.append(self.get_shell())
        return terms


class Shell(Ifconfig, GNRCICMPv6Echo, GNRCIPv6NIB):
    pass


def first_netif_and_addr_by_scope(ifconfig_out, scope):
    netifs = PARSERS["ifconfig"].parse(ifconfig_out)
    key = next(iter(netifs))
    netif = netifs[key]
    return (
        key,
        [addr["addr"] for addr in netif["ipv6_addrs"] if addr["scope"] == scope][0],
    )


def global_addr(ifconfig_out):
    return first_netif_and_addr_by_scope(ifconfig_out, "global")


@contextlib.contextmanager
def rpl_nodes(factory, zep_dispatch):
    # linear topology with 4 nodes
    topology = ("A B\n"
                "B C\n"
                "C D\n")
    zep_dispatch.stdin.write(topology.encode())
    zep_dispatch.stdin.close()

    # create native instances
    nodes = factory.get_shells(4)
    A, _, _, D = nodes

    netifs = PARSERS["ifconfig"].parse(A.ifconfig_list())
    iface = next(iter(netifs))
    # add prefix to root node
    A.nib_prefix_add(iface, "2001:db8::/64")
    A.ifconfig_add(iface, "2001:db8::1/64")
    A.cmd("rpl root 0 2001:db8::1")
    root_addr = global_addr(A.ifconfig_list())[1]

    # wait for the creation of the DODAG
    time.sleep(10)
    yield root_addr, D
    for node in nodes:
        node.stop_term()


def assert_result(result, exp_packet_loss, exp_responses, exp_ttl):
    assert result['stats']['packet_loss'] < exp_packet_loss
    assert result['stats']['rx'] >= exp_responses
    assert result['replies'][0]['ttl'] == exp_ttl


def test_fragmentation(factory, zep_dispatch):
    with rpl_nodes(factory, zep_dispatch) as (root_addr, D):
        # ping root node from last node
        parser = PARSERS["ping6"]
        # test reachability
        result = parser.parse(D.ping6(root_addr))
        # assert packetloss is under 34% (1 packet may get lost)
        # assert at least one response
        # 2 intermediate hops, 64 - 2
        assert_result(result, 34, 1, 64 - 2)

        result = parser.parse(D.ping6(root_addr, count=100, interval=30, packet_size=500))
        # assert packetloss is under 90%
        # assert at least one response
        # 2 intermediate hops, 64 - 2
        assert_result(result, 90, 1, 64 - 2)


@contextlib.contextmanager
def run_zep_dispatch():
    zep_dispatch = Popen(
        [
            ZEP_DISPATCH_PATH,
            '-t',
            '-',
            '127.0.0.1',
            '17754'
        ],
        stdin=subprocess.PIPE
    )
    try:
        yield zep_dispatch
    finally:
        zep_dispatch.terminate()


if __name__ == "__main__":
    board = os.environ.get('BOARD', 'native')
    if board not in ['native', 'native64']:
        print('\x1b[1;31mThis test requires a native board.\x1b[0m\n',
              file=sys.stderr)
        sys.exit(1)

    with RIOTCtrlAppFactory(board) as factory, run_zep_dispatch() as zep_dispatch:
        test_fragmentation(factory, zep_dispatch)
