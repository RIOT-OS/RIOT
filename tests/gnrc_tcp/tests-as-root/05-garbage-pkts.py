#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import base64
import pexpect
import os
import socket
import sys

from scapy.all import Ether, IPv6, TCP, raw, \
                      sendp
from testrunner import run

from shared_func import sudo_guard, get_host_tap_device, get_host_ll_addr, \
                        get_riot_if_id, get_riot_l2_addr, get_riot_ll_addr, \
                        generate_port_number, verify_pktbuf_empty


def testfunc(func):
    def runner(child):
        tap = get_host_tap_device()
        host_ll = get_host_ll_addr(tap)
        dst_if = get_riot_if_id(child)
        dst_ll = get_riot_ll_addr(child)
        dst_l2 = get_riot_l2_addr(child)
        port = generate_port_number()

        # Setup RIOT Node wait for incoming connections from host system
        child.sendline('gnrc_tcp_tcb_init')
        child.expect_exact('gnrc_tcp_tcb_init: argc=1, argv[0] = gnrc_tcp_tcb_init')
        child.sendline('gnrc_tcp_open_passive [::]:{}'.format(port))
        child.expect(r'gnrc_tcp_open_passive: argc=2, '
                     r'argv\[0\] = gnrc_tcp_open_passive, '
                     r'argv\[1\] = \[::\]:(\d+)\r\n')
        assert int(child.match.group(1)) == port

        try:
            print("- {} ".format(func.__name__), end="")
            if child.logfile == sys.stdout:
                func(child, tap, host_ll, dst_if, dst_l2, dst_ll, port)
                print("")
            else:
                try:
                    func(child, tap, host_ll, dst_if, dst_l2, dst_ll, port)
                    print("SUCCESS")
                except Exception as e:
                    print("FAILED")
                    raise e
        finally:
            child.sendline('gnrc_tcp_close')

    return runner


@testfunc
def test_short_payload(child, src_if, src_ll, dst_if, dst_l2, dst_ll, dst_port):
    # see https://github.com/RIOT-OS/RIOT/issues/11999
    with socket.socket(socket.AF_INET6, socket.SOCK_STREAM) as sock:
        sock.settimeout(child.timeout)
        addr_info = socket.getaddrinfo(dst_ll + '%' + src_if, dst_port,
                                       type=socket.SOCK_STREAM)
        sock.connect(addr_info[0][-1])
        child.expect_exact('gnrc_tcp_open_passive: returns 0')
        child.sendline('gnrc_tcp_recv 1000000 1')
        child.expect_exact('gnrc_tcp_recv: argc=3, '
                           'argv[0] = gnrc_tcp_recv, '
                           'argv[1] = 1000000, argv[2] = 1')
        assert 1 == sock.send(b"f")
        child.expect_exact('gnrc_tcp_recv: received 1', timeout=20)
    verify_pktbuf_empty(child)


@testfunc
def test_short_header(child, src_if, src_ll, dst_if, dst_l2, dst_ll, dst_port):
    # see https://github.com/RIOT-OS/RIOT/issues/12086
    tcp_hdr = TCP(dport=dst_port, flags="S", sport=2342, seq=1, dataofs=6)
    # shorten header
    tcp_hdr = raw(tcp_hdr)[:-2]
    sendp(Ether(dst=dst_l2) / IPv6(src=src_ll, dst=dst_ll) / tcp_hdr,
          iface=src_if, verbose=0)

    # let server command return to later check packet buffer
    with socket.socket(socket.AF_INET6, socket.SOCK_STREAM) as sock:
        sock.settimeout(child.timeout)
        addr_info = socket.getaddrinfo(dst_ll + '%' + src_if, dst_port,
                                       type=socket.SOCK_STREAM)
        sock.connect(addr_info[0][-1])
        child.expect_exact('gnrc_tcp_open_passive: returns 0')
    verify_pktbuf_empty(child)


@testfunc
def test_send_ack_instead_of_syn(child, src_if, src_ll,
                                 dst_if, dst_l2, dst_ll, dst_port):
    # check if any debug output is generated during `@testfunc` decorator
    # execution. If so, send fewer packets to prevent the target node
    # from being overwhelmed by packets and output.
    debug = child.expect([pexpect.TIMEOUT,
                          r'GNRC_TCP: Enter "\S+", File: .+\(\d+\)\s'], timeout=1)
    if debug:
        count = 10
    else:
        count = 1000

    # see https://github.com/RIOT-OS/RIOT/pull/12001
    provided_data = base64.b64decode("rwsQf2pekYLaU+exUBBwgPDKAAA=")
    tcp_hdr = TCP(provided_data)
    assert provided_data == raw(tcp_hdr)
    # set destination port to application specific port
    tcp_hdr.dport = dst_port
    sendp(Ether(dst=dst_l2) / IPv6(src=src_ll, dst=dst_ll) / tcp_hdr,
          iface=src_if, verbose=0, count=count)

    # check if server actually still works
    with socket.socket(socket.AF_INET6, socket.SOCK_STREAM) as sock:
        sock.settimeout(child.timeout)
        addr_info = socket.getaddrinfo(dst_ll + '%' + src_if, dst_port,
                                       type=socket.SOCK_STREAM)
        sock.connect(addr_info[0][-1])
        child.expect_exact('gnrc_tcp_open_passive: returns 0')
    verify_pktbuf_empty(child)


@testfunc
def test_option_parsing_term(child, src_if, src_ll,
                             dst_if, dst_l2, dst_ll, dst_port):
    # see https://github.com/RIOT-OS/RIOT/issues/12086
    tcp_hdr = TCP(dport=dst_port, flags="S", sport=2342, seq=1, dataofs=6)
    sendp(Ether(dst=dst_l2) / IPv6(src=src_ll, dst=dst_ll) / tcp_hdr /
          b"\x50\x00\x00\x00", iface=src_if, verbose=0)

    # check if server actually still works
    with socket.socket(socket.AF_INET6, socket.SOCK_STREAM) as sock:
        sock.settimeout(child.timeout)
        addr_info = socket.getaddrinfo(dst_ll + '%' + src_if, dst_port,
                                       type=socket.SOCK_STREAM)
        sock.connect(addr_info[0][-1])
        child.expect_exact('gnrc_tcp_open_passive: returns 0')
    verify_pktbuf_empty(child)


if __name__ == "__main__":
    sudo_guard(uses_scapy=True)
    script = sys.modules[__name__]
    tests = [getattr(script, t) for t in script.__dict__
             if type(getattr(script, t)).__name__ == "function"
             and t.startswith("test_")]
    for test in tests:
        res = run(test, timeout=10, echo=False)
        if res != 0:
            sys.exit(res)
    print(os.path.basename(sys.argv[0]) + ": success\n")
