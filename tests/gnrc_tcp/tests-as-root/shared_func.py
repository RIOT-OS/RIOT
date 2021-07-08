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
import testrunner


class Runner:
    def __init__(self, timeout, echo=False, skip=False):
        self.timeout = timeout
        self.echo = echo
        self.skip = skip

    def __call__(self, fn):
        if self.skip:
            print('- Test "{}": SKIPPED'.format(fn.__name__))
            return 0

        res = -1
        try:
            res = testrunner.run(fn, self.timeout, self.echo)

        finally:
            if res == 0:
                print('- Test "{}": SUCCESS'.format(fn.__name__))
            else:
                print('- Test "{}": FAILED'.format(fn.__name__))

        return res


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


class HostTcpNode:
    def __init__(self):
        self.opened = False
        self.sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.interface = get_host_tap_device()

    def send(self, payload_to_send):
        self.sock.send(payload_to_send.encode('utf-8'))

    def receive(self, sent_payload):
        total_bytes = len(sent_payload)
        assert self.sock.recv(total_bytes, socket.MSG_WAITALL).decode('utf-8') == sent_payload

    def close(self):
        self.sock.close()
        self.opened = False


class HostTcpClient(HostTcpNode):
    def __init__(self, target_addr, target_port):
        super().__init__()
        self.target_addr = target_addr
        self.target_port = target_port

    def __enter__(self):
        if not self.opened:
            self.open()
        return self

    def __exit__(self, _1, _2, _3):
        if self.opened:
            self.close()

    def open(self):
        addrinfo = socket.getaddrinfo(
            self.target_addr + '%' + self.interface,
            self.target_port,
            type=socket.SOCK_STREAM
        )
        self.sock.connect(addrinfo[0][-1])
        self.opened = True


class RiotTcpNode:
    def __init__(self, child):
        self.child = child
        self.opened = False

        # Query local address of RIOT Node
        self.addr = get_riot_ll_addr(self.child)

    def tcb_init(self):
        self.child.sendline('gnrc_tcp_tcb_init')
        self.child.expect_exact('gnrc_tcp_tcb_init: returns')

    def send(self, timeout_ms, payload_to_send):
        total_bytes = len(payload_to_send)

        # Verify that internal buffer can hold the given amount of data
        assert setup_internal_buffer(self.child) >= total_bytes

        # Write data to RIOT nodes internal buffer
        write_data_to_internal_buffer(self.child, payload_to_send)

        # Send buffer contents via tcp
        self.child.sendline('gnrc_tcp_send {}'.format(str(timeout_ms)))
        self.child.expect_exact('gnrc_tcp_send: sent {}'.format(str(total_bytes)))

        # Verify that packet buffer is empty
        verify_pktbuf_empty(self.child)

    def receive(self, timeout_ms, sent_payload):
        total_bytes = len(sent_payload)

        # Verify that internal Buffer can hold the test data
        assert setup_internal_buffer(self.child) >= total_bytes

        # Receive Data
        self.child.sendline('gnrc_tcp_recv {} {}'.format(timeout_ms, str(total_bytes)))
        self.child.expect_exact('gnrc_tcp_recv: received ' + str(total_bytes), timeout=20)

        # Readout internal buffer content of RIOT Note
        assert read_data_from_internal_buffer(self.child, total_bytes) == sent_payload

        # Verify that packet buffer is empty
        verify_pktbuf_empty(self.child)

    def close(self):
        self.child.sendline('gnrc_tcp_close')
        self.child.expect_exact('gnrc_tcp_close: returns')
        verify_pktbuf_empty(self.child)
        self.opened = False

    def abort(self):
        self.child.sendline('gnrc_tcp_abort')
        self.child.expect_exact('gnrc_tcp_abort: returns')
        verify_pktbuf_empty(self.child)
        self.opened = False


class RiotTcpServer(RiotTcpNode):
    def __init__(self, child, listen_port, listen_addr='[::]'):
        super().__init__(child)

        self.listening = False
        self.listen_port = str(listen_port)
        self.listen_addr = str(listen_addr)

        self.tcb_init()

    def __enter__(self):
        if not self.listening:
            self.listen()
            self.listening = True
        return self

    def __exit__(self, _1, _2, _3):
        if self.listening:
            self.stop_listen()
            self.listening = False

    def listen(self):
        self.child.sendline('gnrc_tcp_listen {}:{}'.format(self.listen_addr, self.listen_port))
        self.child.expect_exact('gnrc_tcp_listen: returns 0')

    def accept(self, timeout_ms):
        self.child.sendline('gnrc_tcp_accept {}'.format(str(timeout_ms)))
        self.child.expect_exact('gnrc_tcp_accept: returns 0')
        self.opened = True

    def stop_listen(self):
        self.child.sendline('gnrc_tcp_stop_listen')
        self.child.expect_exact('gnrc_tcp_stop_listen: returns')
        verify_pktbuf_empty(self.child)


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
