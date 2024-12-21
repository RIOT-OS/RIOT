#!/usr/bin/env python3

# Copyright (C) 2021 Benjamin Valentin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import time
import sys

from subprocess import Popen
from riotctrl_shell.gnrc import GNRCICMPv6Echo, GNRCICMPv6EchoParser
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
        # return ctrl with started terminal
        return Shell(ctrl)

    def get_shells(self, num=1):
        terms = []
        for i in range(num):
            terms.append(self.get_shell())
        return terms


class Shell(Ifconfig, GNRCICMPv6Echo):
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


def test_linear_topology(factory, zep_dispatch):

    # linear topology with 4 nodes
    topology = ("A B\n"
                "B C\n"
                "C D\n")
    zep_dispatch.stdin.write(topology.encode())
    zep_dispatch.stdin.close()

    # create native instances
    nodes = factory.get_shells(4)
    A = nodes[0]
    D = nodes[3]

    # add prefix to root node
    A.cmd("nib prefix add 5 2001:db8::/32")
    A.cmd("ifconfig 5 add 2001:db8::1/32")
    A.cmd("rpl root 0 2001:db8::1")
    root_addr = global_addr(A.ifconfig_list())[1]

    # wait for the creation of the DODAG
    time.sleep(10)

    # ping root node from last node
    parser = GNRCICMPv6EchoParser()
    result = parser.parse(D.ping6(root_addr))

    # assert packetloss is under 10%"))
    assert result['stats']['packet_loss'] < 10
    # assert at least one responder
    assert result['stats']['rx'] > 0
    # 2 intermediate hops, 64 - 2
    assert result['replies'][0]['ttl'] == 62

    # terminate nodes
    for n in nodes:
        n.stop_term()


def test_alternative_route(factory, zep_dispatch):

    # topology with 5 nodes with alternative routes
    #   A
    #  / \
    # B ― C
    # |   |
    # D ― E
    topology = ("A B\n"
                "A C\n"
                "B C\n"
                "B D\n"
                "C E\n"
                "D E\n")
    zep_dispatch.stdin.write(topology.encode())
    zep_dispatch.stdin.close()

    # create native instances
    nodes = factory.get_shells(5)
    A = nodes[0]
    D = nodes[3]

    # add prefix to root node
    A.cmd("nib prefix add 5 2001:db8::/32")
    A.cmd("ifconfig 5 add 2001:db8::1/32")
    A.cmd("rpl root 0 2001:db8::1")
    root_addr = global_addr(A.ifconfig_list())[1]

    # wait for the creation of the DODAG
    time.sleep(10)

    # ping root node from last node
    parser = GNRCICMPv6EchoParser()
    result = parser.parse(D.ping6(root_addr))

    # assert packetloss is under 10%"))
    assert result['stats']['packet_loss'] < 10
    # assert at least one responder
    assert result['stats']['rx'] > 0
    # 1 intermediate hop, 64 - 1
    assert result['replies'][0]['ttl'] == 63

    # terminate nodes
    for n in nodes:
        n.stop_term()


def run_test(func, factory):
    with Popen([ZEP_DISPATCH_PATH, '-t', '-', '127.0.0.1', '17754'], stdin=subprocess.PIPE) as zep_dispatch:
        try:
            func(factory, zep_dispatch)
        finally:
            zep_dispatch.terminate()


if __name__ == "__main__":
    board = os.environ.get('BOARD', 'native')
    if board not in ['native', 'native32']:
        print('\x1b[1;31mThis test requires a native board.\x1b[0m\n',
              file=sys.stderr)
        sys.exit(1)
    with RIOTCtrlAppFactory(board) as factory:
        run_test(test_linear_topology, factory)
        run_test(test_alternative_route, factory)
