#!/usr/bin/env python3

# Copyright (C) 2018   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
import sys
import os
import re
import socket
import random


class TcpServer:
    def __init__(self, port, shutdown_event):
        self._port = port
        self._shutdown = shutdown_event

    def __enter__(self):
        self.sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(('::', self._port))
        self.sock.listen(1)
        self.conn, self.addr = self.sock.accept()
        return self

    def __exit__(self, exc, exc_val, exc_trace):
        self._shutdown.wait()
        self.conn.close()
        self.sock.close()

    def send(self, data):
        self.conn.send(data.encode('utf-8'))

    def recv(self, number_of_bytes):
        return self.conn.recv(number_of_bytes, socket.MSG_WAITALL).decode('utf-8')


def generate_port_number():
    return random.randint(1024, 65535)


def get_host_tap_device():
    # Check if given tap device is part of a network bridge
    # if so use bridged interface instead of given tap device
    tap = os.environ["TAPDEV"]
    result = os.popen('bridge link show dev {}'.format(tap))
    bridge = re.search('master (.*) state', result.read())

    return bridge.group(1).strip() if bridge else tap


def get_host_ll_addr(interface):
    result = os.popen('ip addr show dev ' + interface + ' scope link')
    return re.search('inet6 (.*)/64', result.read()).group(1).strip()


def get_riot_l2_addr(child):
    child.sendline('ifconfig')
    child.expect(r'HWaddr: ([A-F-a-f0-9:]+)\s')
    return child.match.group(1)


def get_riot_ll_addr(child):
    child.sendline('ifconfig')
    child.expect(r'(fe80:[0-9a-f:]+)\s')
    return child.match.group(1).strip()


def get_riot_if_id(child):
    child.sendline('ifconfig')
    child.expect(r'Iface\s+(\d+)\s')
    return child.match.group(1).strip()


def setup_internal_buffer(child):
    child.sendline('buffer_init')
    child.sendline('buffer_get_max_size')
    child.expect(r'returns (\d+)\s')
    return int(child.match.group(1).strip())


def write_data_to_internal_buffer(child, data):
    # This is a workaround to avoid the restrictions of the RIOT Shell Buffersize by
    # filling an internal buffer piece by piece. The internal buffers current content is via tcp
    # by calling gnrc_tcp_send.
    CHUNK_SIZE = 100

    for i in range(0, len(data), CHUNK_SIZE):
        child.sendline('buffer_write ' + str(i) + ' ' + data[i: CHUNK_SIZE + i])


def read_data_from_internal_buffer(child, bytes_to_read):
    child.sendline('buffer_read 0 ' + str(bytes_to_read))
    child.expect('<begin>(.*)<end>')
    return child.match.group(1)


def verify_pktbuf_empty(child):
    child.sendline('pktbuf')
    child.expect(r'first byte: (0x[0-9a-fA-F]+).*\(size: (\d+)\)')

    pktbuf_addr = child.match.group(1)
    pktbuf_size = child.match.group(2)

    child.expect(r'~ unused: {} \(next: (\(nil\)|0), size: {}\) ~'.format(pktbuf_addr, pktbuf_size))


def sudo_guard(uses_scapy=False):
    sudo_required = uses_scapy or (os.environ.get("BOARD", "") != "native")
    if sudo_required and os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It uses `./dist/tools/ethos/start_networking.sh` as term" +
              (" and it's constructing and sending Ethernet frames."
               if uses_scapy else "") + "\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
