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
            print('\n- "{}": SKIPPED'.format(fn.__name__), end='')
            return 0

        res = -1
        try:
            res = testrunner.run(fn, self.timeout, self.echo)

        finally:
            if res == 0:
                print('- "{}": SUCCESS'.format(fn.__name__), end='')
            else:
                print('- "{}": FAILED'.format(fn.__name__), end='')

        return res


class _HostTcpNode:
    def __init__(self):
        self.opened = False
        self.sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.interface = self._get_interface()
        self.address = self._get_ip_address(self.interface)

    def send(self, payload_to_send):
        self.sock.send(payload_to_send.encode('utf-8'))

    def receive(self, sent_payload):
        total_bytes = len(sent_payload)
        assert self.sock.recv(total_bytes, socket.MSG_WAITALL).decode('utf-8') == sent_payload

    def close(self):
        self.sock.close()
        self.opened = False

    def _get_interface(self):
        # Check if given tap device is part of a network bridge
        # if so use bridged interface instead of given tap device
        tap = os.environ["TAPDEV"]
        result = os.popen('bridge link show dev {}'.format(tap))
        bridge = re.search('master (.*) state', result.read())

        return bridge.group(1).strip() if bridge else tap

    def _get_ip_address(self, interface):
        result = os.popen('ip addr show dev ' + interface + ' scope link')
        return re.search('inet6 (.*)/64', result.read()).group(1).strip()


class HostTcpServer(_HostTcpNode):
    def __init__(self, listen_port):
        super().__init__()
        self.listening = False
        self.listen_sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
        self.listen_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.listen_port = listen_port

    def __enter__(self):
        if not self.listening:
            self.listen()
        return self

    def __exit__(self, _1, _2, _3):
        if self.listening:
            self.stop_listen()

    def listen(self):
        self.listen_sock.bind(('::', self.listen_port))
        self.listen_sock.listen(1)
        self.listening = True

    def accept(self):
        self.sock, _ = self.listen_sock.accept()

    def stop_listen(self):
        self.listen_sock.close()
        self.listening = False


class HostTcpClient(_HostTcpNode):
    def __init__(self, target):
        super().__init__()
        self.target_addr = str(target.address)
        self.target_port = str(target.listen_port)

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


class _RiotTcpNode:
    def __init__(self, child):
        self.child = child
        self.opened = False
        self.interface = self._get_interface()
        self.address = self._get_ip_address()
        self.mac = self._get_mac_address()

    def tcb_init(self):
        self.child.sendline('gnrc_tcp_tcb_init')
        self.child.expect_exact('gnrc_tcp_tcb_init: returns')

    def send(self, timeout_ms, payload_to_send, bytes_to_send=None):
        total_bytes = len(payload_to_send)

        if bytes_to_send is None:
            bytes_to_send = total_bytes

        # Verify that internal buffer can hold the given amount of data
        assert self._setup_internal_buffer() >= total_bytes

        # Write data to RIOT nodes internal buffer
        self._write_data_to_internal_buffer(payload_to_send)

        # Send buffer contents via tcp
        self.child.sendline('gnrc_tcp_send {} {}'.format(timeout_ms, bytes_to_send))
        self.child.expect_exact('gnrc_tcp_send: sent {}'.format(bytes_to_send))

        # Verify that packet buffer is empty
        self._verify_pktbuf_empty()

    def receive(self, timeout_ms, sent_payload):
        total_bytes = len(sent_payload)

        # Verify that internal Buffer can hold the test data
        assert self._setup_internal_buffer() >= total_bytes

        # Receive Data
        self.child.sendline('gnrc_tcp_recv {} {}'.format(timeout_ms, str(total_bytes)))
        self.child.expect_exact('gnrc_tcp_recv: received ' + str(total_bytes), timeout=20)

        # Readout internal buffer content of RIOT Note
        assert self._read_data_from_internal_buffer(total_bytes) == sent_payload

        # Verify that packet buffer is empty
        self._verify_pktbuf_empty()

    def close(self):
        self.child.sendline('gnrc_tcp_close')
        self.child.expect_exact('gnrc_tcp_close: returns')
        self._verify_pktbuf_empty()
        self.opened = False

    def abort(self):
        self.child.sendline('gnrc_tcp_abort')
        self.child.expect_exact('gnrc_tcp_abort: returns')
        self._verify_pktbuf_empty()
        self.opened = False

    def get_local(self):
        self.child.sendline('gnrc_tcp_get_local')
        self.child.expect_exact('gnrc_tcp_get_local: returns 0')

    def get_remote(self):
        self.child.sendline('gnrc_tcp_get_remote')
        self.child.expect_exact('gnrc_tcp_get_remote: returns 0')

    def _get_interface(self):
        self.child.sendline('ifconfig')
        self.child.expect(r'Iface\s+(\d+)\s')
        return self.child.match.group(1).strip()

    def _get_ip_address(self):
        self.child.sendline('ifconfig')
        self.child.expect(r'(fe80:[0-9a-f:]+)\s')
        return self.child.match.group(1).strip()

    def _get_mac_address(self):
        self.child.sendline('ifconfig')
        self.child.expect(r'HWaddr: ([A-F-a-f0-9:]+)\s')
        return self.child.match.group(1)

    def _setup_internal_buffer(self):
        self.child.sendline('buffer_init')
        self.child.sendline('buffer_get_max_size')
        self.child.expect(r'returns (\d+)\s')
        return int(self.child.match.group(1).strip())

    def _write_data_to_internal_buffer(self, data):
        # This is a workaround to avoid the restrictions of the RIOT Shell
        # Buffersize by filling an internal buffer piece by piece.
        # The internal buffers current content is via tcp
        # by calling gnrc_tcp_send.
        CHUNK_SIZE = 100

        for i in range(0, len(data), CHUNK_SIZE):
            self.child.sendline('buffer_write ' + str(i) + ' ' + data[i: CHUNK_SIZE + i])

    def _read_data_from_internal_buffer(self, bytes_to_read):
        self.child.sendline('buffer_read 0 ' + str(bytes_to_read))
        self.child.expect('<begin>(.*)<end>')
        return self.child.match.group(1)

    def _verify_pktbuf_empty(self):
        self.child.sendline('pktbuf')
        self.child.expect(r'first byte: (0x[0-9a-fA-F]+).*\(size: (\d+)\)')

        pktbuf_addr = self.child.match.group(1)
        pktbuf_size = self.child.match.group(2)

        self.child.expect(r'~ unused: {} \(next: (\(nil\)|0), size: {}\) ~'.format(
            pktbuf_addr, pktbuf_size)
        )


class RiotTcpServer(_RiotTcpNode):
    def __init__(self, child, listen_port, listen_addr='::'):
        super().__init__(child)
        self.listening = False
        self.listen_port = str(listen_port)
        self.listen_addr = str(listen_addr)
        self.tcb_init()

    def __enter__(self):
        if not self.listening:
            self.listen()
        return self

    def __exit__(self, _1, _2, _3):
        if self.listening:
            self.stop_listen()

    def listen(self):
        self.child.sendline('gnrc_tcp_listen [{}]:{}'.format(
            self.listen_addr, self.listen_port)
        )
        self.child.expect_exact('gnrc_tcp_listen: returns 0')
        self.listening = True

    def accept(self, timeout_ms):
        self.child.sendline('gnrc_tcp_accept {}'.format(str(timeout_ms)))
        self.child.expect_exact('gnrc_tcp_accept: returns 0')
        self.opened = True

    def stop_listen(self):
        self.child.sendline('gnrc_tcp_stop_listen')
        self.child.expect_exact('gnrc_tcp_stop_listen: returns')
        self._verify_pktbuf_empty()
        self.listening = False

    def queue_get_local(self):
        self.child.sendline('gnrc_tcp_queue_get_local')
        self.child.expect_exact('gnrc_tcp_queue_get_local: returns 0')


class RiotTcpClient(_RiotTcpNode):
    def __init__(self, child, target, port=0):
        super().__init__(child)
        self.target_addr = target.address + '%' + self.interface
        self.target_port = str(target.listen_port)
        self.port = port
        self.tcb_init()

    def __enter__(self):
        if not self.opened:
            self.open()
        return self

    def __exit__(self, _1, _2, _3):
        if self.opened:
            self.close()

    def open(self):
        self.child.sendline('gnrc_tcp_open [{}]:{} {}'.format(
            self.target_addr, self.target_port, self.port)
        )
        self.child.expect_exact('gnrc_tcp_open: returns 0')
        self.opened = True


def generate_port_number():
    return random.randint(1024, 65535)


def sudo_guard(uses_scapy=False):
    sudo_required = uses_scapy or (os.environ.get("BOARD", "") != "native")
    if sudo_required and os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It uses `./dist/tools/ethos/start_networking.sh` as term" +
              (" and it's constructing and sending Ethernet frames."
               if uses_scapy else "") + "\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
