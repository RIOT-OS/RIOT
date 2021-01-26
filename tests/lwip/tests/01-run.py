#!/usr/bin/env python3

# Copyright (C) 2016 Martine Lenders <mail@martine-lenders.eu>
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import itertools
import logging
import os
import random
import re
import socket
import sys
import time

from riotctrl.ctrl import RIOTCtrl
from riotctrl.shell import ShellInteraction, ShellInteractionParser

from riotctrl_shell.sys import Reboot

DEFAULT_TIMEOUT = 5

MAKE = os.environ.get('MAKE', 'make')


def _inc_port(port1):
    """
    Increments the number in port1 by 1

    >>> _inc_port("/dev/ttyACM0")
    '/dev/ttyACM1'
    >>> _inc_port("tap0")
    'tap1'
    """
    m = re.search(r"\D(\d+)$", port1)
    if m is None:
        raise ValueError("Unable auto-generate second device port. "
                         "Please set PORT2 environment variable.")
    port1_num = int(m.group(1))
    port2_num = port1_num + 1
    return re.sub(r"{}$".format(port1_num), str(port2_num), port1)


def get_riot_ctrls():
    board = os.environ["BOARD"]
    port1 = os.environ["PORT"]
    port2 = os.environ.get("PORT2", _inc_port(port1))
    ctrl1 = RIOTCtrl(env={"BOARD": board, "PORT": port1})
    ctrl2 = RIOTCtrl(env={"BOARD": board, "PORT": port2})
    return ctrl1, ctrl2


def iter_groupby_n(n, iterable):
     args = [iter(iterable)] * n
     return ([e for e in t if e != None] for t in itertools.zip_longest(*args))


class Shell(Reboot):
    @staticmethod
    def get_shells(ctrls):
        return [Shell(ctrl) for ctrl in ctrls]

    @ShellInteraction.check_term
    def ifconfig(self, timeout=-1, async_=False):
        return self.cmd("ifconfig", timeout, async_)

    @ShellInteraction.check_term
    def _server_start(self, prot, param, timeout=-1, async_=False):
        res = self.cmd("{} server start {}".format(prot, param),
                       timeout, async_)
        if "Success:" not in res:
            raise RuntimeError(res)
        return res

    @ShellInteraction.check_term
    def _server_stop(self, prot, timeout=-1, async_=False):
        res = self.cmd("{} server stop".format(prot),
                       timeout, async_)
        if "Success:" not in res:
            raise RuntimeError(res)
        return res

    @ShellInteraction.check_term
    def _send(self, prot, dst, data, num=None, delay=None,
              timeout=-1, async_=False):
        cmd = "{prot} send {dst} {data}".format(prot=prot, dst=dst,
                                                data=data.hex())
        if num is not None:
            cmd += " {}".format(num)
        if delay is not None:
            cmd += " {}".format(int(delay) * 1000000)
        res = self.cmd(cmd, timeout, async_)
        if "Success:" not in res:
            raise RuntimeError(res)
        return res

    @staticmethod
    def _format_ipv6_addr(addr):
        try:
            socket.inet_pton(socket.AF_INET6, addr)
            return "[{}]".format(addr)
        except socket.error:    # not an IPv6 address
            return addr

    def ip_server_start(self, proto, timeout=-1, async_=False):
        return self._server_start("ip", proto, timeout, async_)

    def ip_server_stop(self, timeout=-1, async_=False):
        return self._server_stop("ip", timeout, async_)

    def ip_send(self, dst_addr, proto, data, num=None, delay=None,
                timeout=-1, async_=False):
        return self._send("ip", "{} {}".format(dst_addr, proto), data,
                          num, delay, timeout, async_)

    def tcp_server_start(self, port, timeout=-1, async_=False):
        return self._server_start("tcp", port, timeout, async_)

    def tcp_server_stop(self, timeout=-1, async_=False):
        return self._server_stop("tcp", timeout, async_)

    def tcp_client_connect(self, dst_addr, dport, timeout=-1, async_=False):
        dst_addr = self._format_ipv6_addr(dst_addr)
        res = self.cmd("tcp connect {}:{}".format(dst_addr, dport),
                       timeout, async_)
        if "Error:" in res or "Client already connected" in res:
            raise RuntimeError(res)
        return res

    def tcp_client_disconnect(self, timeout=-1, async_=False):
        return self.cmd("tcp disconnect", timeout, async_)

    def tcp_send(self, data, num=None, delay=None,
                 timeout=-1, async_=False):
        return self._send("tcp", "", data, num, delay, timeout, async_)

    def udp_server_start(self, port, timeout=-1, async_=False):
        return self._server_start("udp", port, timeout, async_)

    def udp_server_stop(self, timeout=-1, async_=False):
        return self._server_stop("udp", timeout, async_)

    def udp_send(self, dst_addr, dport, data, num=None, delay=None,
                 timeout=-1, async_=False):
        dst_addr = self._format_ipv6_addr(dst_addr)
        return self._send("udp", "{}:{}".format(dst_addr, dport), data,
                          num, delay, timeout, async_)

    @classmethod
    def _pattern_from_ip(cls, ip=None):
        if ip is None:
            return r"(\[[0-9a-f:]+\]|[0-9\.]+)"
        else:
            return cls._format_ipv6_addr(ip) \
                .replace(r"[", r"\[") \
                .replace(r"]", r"\]")

    def tcp_server_client_expect(self, client_ip=None):
        self.riotctrl.term.expect(
            r"TCP client (?P<client_addr>{}):(?P<client_port>\d+) "
            r"connected".format(self._pattern_from_ip(client_ip))
        )
        res = self.riotctrl.term.match.groupdict()
        res["client_addr"] = res["client_addr"].strip("[]")
        res["client_port"] = int(res["client_port"])
        return res

    def tcp_server_connection_reset_expect(self, client_ip=None):
        self.riotctrl.term.expect(
            r"TCP connection to "
            r"(?P<client_addr>{}):(?P<client_port>\d+) reset"
            .format(self._pattern_from_ip(client_ip))
        )
        res = self.riotctrl.term.match.groupdict()
        res["client_addr"] = res["client_addr"].strip("[]")
        res["client_port"] = int(res["client_port"])
        return res

    def od_expect(self, data, width=16):
        addr = 0
        for byte_line in iter_groupby_n(width, data):
            self.riotctrl.term.expect_exact(
                "{:08X}  ".format(addr) +
                "  ".join("{:02X}".format(byte) for byte in byte_line)
            )
            addr += width


class LwIPIfconfigParser(ShellInteractionParser):
    def __init__(self):
        self.iface_c = re.compile(r"(?P<name>.._\d\d):\s")
        self.ipv6_c = re.compile(r"inet6\s+(?P<addr>[0-9a-f:]+)$")
        self.ipv4_c = re.compile(r"inet\s+(?P<addr>[\d\.]+)$")

    def parse(self, cmd_output):
        netifs = None
        current = None
        for line in cmd_output.splitlines():
            m = self.iface_c.search(line)
            if m is not None:
                name = m.group("name")
                if netifs is None:
                    netifs = {}
                current = netifs[name] = {}
                line = line[m.end():]
            if current is None:
                continue
            m = self.ipv6_c.search(line)
            if m is not None:
                if "ipv6_addrs" in current:
                    current["ipv6_addrs"].append(m.group("addr"))
                else:
                    current["ipv6_addrs"] = [m.group("addr")]
            m = self.ipv4_c.search(line)
            if m is not None:
                current["ipv4_addr"] = m.group("addr")
        return netifs


def get_ip_addr(node, ifconfig_parser):
    res = ifconfig_parser.parse(node.ifconfig())
    assert res
    key = next(iter(res))
    # prefer IPv6
    if "ipv6_addrs" in res[key]:
        assert len(res[key]["ipv6_addrs"]) > 0
        return res[key]["ipv6_addrs"][0]
    assert "ipv4_addr" in res[key]
    assert res[key]["ipv4_addr"] != "0.0.0.0"
    return res[key]["ipv4_addr"]


def tear_down(nodes):
    for node in nodes:
        node.reboot()
    time.sleep(node.riotctrl.TERM_STARTED_DELAY)


def test_ip_send(nodes, ifconfig_parser):
    sender = nodes[0]
    receiver = nodes[1]

    ipprot = random.randint(0x00, 0xff)
    # data still needs to fit shell limit
    data = os.urandom(random.randint(1, 20))
    receiver_ip = get_ip_addr(receiver, ifconfig_parser)
    receiver.ip_server_start(ipprot)
    sender.ip_send(receiver_ip, ipprot, data)
    receiver.od_expect(data)


def test_udp_send(nodes, ifconfig_parser):
    sender = nodes[0]
    receiver = nodes[1]

    port = random.randint(0x0000, 0xffff)
    # data still needs to fit shell limit
    data = os.urandom(random.randint(1, 20))
    receiver_ip = get_ip_addr(receiver, ifconfig_parser)
    receiver.udp_server_start(port)
    sender.udp_send(receiver_ip, port, data)
    receiver.od_expect(data)


def test_tcp_send(nodes, ifconfig_parser):
    server = nodes[0]
    client = nodes[1]

    port = random.randint(0x0000, 0xffff)
    # data still needs to fit shell limit
    data = os.urandom(random.randint(1, 20))
    server_ip = get_ip_addr(server, ifconfig_parser)
    client_ip = get_ip_addr(client, ifconfig_parser)

    server.tcp_server_start(port)
    client.tcp_client_connect(server_ip, port)
    server.tcp_server_client_expect(client_ip)
    client.tcp_send(data)
    server.od_expect(data)
    client.tcp_client_disconnect()
    try:
        client.tcp_send(data)
        assert False
    except RuntimeError:
        pass


def test_tcpv6_multiconnect(nodes, ifconfig_parser):
    if any(n.riotctrl.board() != "native" for n in nodes):
        # run test only with native
        logging.info("SKIP_TEST: found non-native board")
        return
    server = nodes[0]
    client = nodes[1]

    port = random.randint(0x0000, 0xffff)
    server_ip = get_ip_addr(server, ifconfig_parser)
    client_ip = get_ip_addr(client, ifconfig_parser)

    try:
        connect_addr = socket.getaddrinfo(
            "%s%%tapbr0" % server_ip, port)[0][4]
    except socket.gaierror as e:
        logging.info("SKIP_TEST: %s" % e)
        return

    server.tcp_server_start(port)
    client.tcp_client_connect(server_ip, port)
    server.tcp_server_client_expect(client_ip)
    with socket.socket(socket.AF_INET6) as sock:
        sock.connect(connect_addr)
        server.riotctrl.term.expect(r"Error on TCP accept \[-[0-9]+\]")
    client.tcp_client_disconnect()
    server.tcp_server_connection_reset_expect(client_ip)
    client.tcp_client_connect(server_ip, port)
    server.tcp_server_client_expect(client_ip)
    client.tcp_client_disconnect()
    server.tcp_server_connection_reset_expect(client_ip)
    with socket.socket(socket.AF_INET6) as sock:
        sock.connect(connect_addr)
        res = server.tcp_server_client_expect()
    server.tcp_server_connection_reset_expect(res["client_addr"])


def test_triple_send(nodes, ifconfig_parser):
    sender = nodes[0]
    receiver = nodes[1]

    udp_port = random.randint(0x0000, 0xffff)
    tcp_port = random.randint(0x0000, 0xffff)
    ipprot = random.randint(0x00, 0xff)
    receiver_ip = get_ip_addr(receiver, ifconfig_parser)
    sender_ip = get_ip_addr(sender, ifconfig_parser)

    receiver.ip_server_start(ipprot)
    receiver.udp_server_start(udp_port)
    receiver.tcp_server_start(tcp_port)
    # data still needs to fit shell limit
    data = os.urandom(random.randint(1, 20))
    sender.udp_send(receiver_ip, udp_port, data)
    receiver.od_expect(data)

    data = os.urandom(random.randint(1, 20))
    sender.ip_send(receiver_ip, ipprot, data)
    receiver.od_expect(data)

    data = os.urandom(random.randint(1, 20))
    sender.tcp_client_connect(receiver_ip, tcp_port)
    receiver.tcp_server_client_expect(sender_ip)
    sender.tcp_send(data)
    receiver.od_expect(data)


if __name__ == "__main__":
    testfunctions = [value for key, value
                     in sys.modules[__name__].__dict__.items()
                     if key.startswith("test")]
    nodes = Shell.get_shells(get_riot_ctrls())
    ifconfig_parser = LwIPIfconfigParser()
    with nodes[0].riotctrl.run_term(reset=True, logfile=sys.stdout), \
         nodes[1].riotctrl.run_term(reset=True, logfile=sys.stdout):
        for func in testfunctions:
            func(nodes, ifconfig_parser)
            tear_down(nodes)
