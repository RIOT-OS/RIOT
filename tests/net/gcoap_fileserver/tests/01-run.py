#!/usr/bin/env python3

# Copyright (C) 2022 Benjamin Valentin
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

    # linear topology with 3 nodes and 25% packet loss
    topology = ("A C 0.75\n"
                "C B 0.75\n")
    zep_dispatch.stdin.write(topology.encode())
    zep_dispatch.stdin.close()

    # create native instances
    nodes = factory.get_shells(3)
    A = nodes[0]
    B = nodes[2]

    # add prefix to root node
    A.cmd("nib prefix add 7 2001:db8::/32")
    A.cmd("ifconfig 7 add 2001:db8::1/32")
    A.cmd("rpl root 0 2001:db8::1")

    # wait for the creation of the DODAG
    time.sleep(10)

    # Download file from CoAP server
    B.cmd("vfs rm /nvm0/song.txt")
    B.cmd("vfs rm /nvm0/song2.txt")
    B.cmd("ncget coap://[2001:db8::1]/const/song.txt", timeout=60)

    # make sure the content matches
    assert A.cmd("md5sum /const/song.txt").split()[2] == B.cmd("md5sum /nvm0/song.txt").split()[2]

    # upload the file to node B (only one node should write MEMORY.bin)
    A.cmd("ncput /const/song.txt coap://[" + global_addr(B.cmd("ifconfig 7"))[1] + "]/vfs/song2.txt", timeout=60)

    # make sure the content matches
    assert B.cmd("md5sum /nvm0/song.txt").split()[2] == B.cmd("md5sum /nvm0/song2.txt").split()[2]

    # clean up after run
    B.cmd("vfs rm /nvm0/song.txt")
    B.cmd("vfs rm /nvm0/song2.txt")

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
