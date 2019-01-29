#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import re
import socket
import sys
import subprocess
import threading

from scapy.all import DNS, DNSQR, DNSRR, Raw, raw
from testrunner import run


SERVER_TIMEOUT = 5
SERVER_PORT = 5335  # 53 requires root and 5353 is used by e.g. Chrome for MDNS


DNS_RR_TYPE_A = 1
DNS_RR_TYPE_AAAA = 28
DNS_RR_TYPE_A_DLEN = 4
DNS_RR_TYPE_AAAA_DLEN = 16
DNS_MSG_COMP_MASK = b"\xc0"


TEST_NAME = "example.org"
TEST_A_DATA = "10.0.0.1"
TEST_AAAA_DATA = "2001:db8::1"
TEST_QDCOUNT = 2
TEST_ANCOUNT = 2


class Server(threading.Thread):
    def __init__(self, family=socket.AF_INET, type=socket.SOCK_DGRAM,
                 proto=0, bind_addr=None, bind_port=None, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.socket = socket.socket(family, type, proto)
        if bind_port is not None:
            if (bind_addr is not None):
                res = socket.getaddrinfo(bind_addr, bind_port)
                sockaddr = res[0][4]
            else:
                sockaddr = ("", bind_port)
            self.socket.bind(sockaddr)
        self.stopped = False
        self.enter_loop = threading.Event()

    def run(self):
        while True:
            self.enter_loop.wait()
            self.enter_loop.clear()
            if self.stopped:
                return
            p, remote = self.socket.recvfrom(1500)
            p = DNS(raw(p))
            # check received packet for correctness
            assert(p is not None)
            assert(p[DNS].qr == 0)
            assert(p[DNS].opcode == 0)
            # has two queries
            assert(p[DNS].qdcount == TEST_QDCOUNT)
            qdcount = p[DNS].qdcount
            # both for TEST_NAME
            assert(p[DNS].qd[0].qname == TEST_NAME.encode("utf-8") + b".")
            assert(p[DNS].qd[1].qname == TEST_NAME.encode("utf-8") + b".")
            assert(any(p[DNS].qd[i].qtype == DNS_RR_TYPE_A
                       for i in range(qdcount)))    # one is A
            assert(any(p[DNS].qd[i].qtype == DNS_RR_TYPE_AAAA
                       for i in range(qdcount)))    # one is AAAA
            if self.reply is not None:
                self.socket.sendto(raw(self.reply), remote)
                self.reply = None

    def listen(self, reply=None):
        self.reply = reply
        self.enter_loop.set()

    def stop(self):
        self.stopped = True
        self.enter_loop.set()
        self.socket.close()
        self.join()


server = None


def check_and_search_output(cmd, pattern, res_group, *args, **kwargs):
    output = subprocess.check_output(cmd, *args, **kwargs).decode("utf-8")
    for line in output.splitlines():
        m = re.search(pattern, line)
        if m is not None:
            return m.group(res_group)
    return None


def get_bridge(tap):
    res = check_and_search_output(
            ["bridge", "link"],
            r"{}.+master\s+(?P<master>[^\s]+)".format(tap),
            "master"
        )
    return tap if res is None else res


def get_host_lladdr(tap):
    res = check_and_search_output(
            ["ip", "addr", "show", "dev", tap, "scope", "link"],
            r"inet6\s+(?P<lladdr>[0-9A-Fa-f:]+)/\d+",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def dns_server(child, server, port=53):
    child.sendline("dns server {} {:d}".format(server, port))
    child.sendline("dns server")
    child.expect(r"DNS server: \[{}\]:{:d}".format(server, port))


def successful_dns_request(child, name, exp_addr=None):
    child.sendline("dns request {}".format(name))
    res = child.expect(["error resolving {}".format(name),
                        "{} resolves to {}".format(name, exp_addr)],
                       timeout=3)
    return ((res > 0) and (exp_addr is not None))


def test_success(child):
    server.listen(DNS(qr=1, qdcount=TEST_QDCOUNT, ancount=TEST_ANCOUNT,
                      qd=(DNSQR(qname=TEST_NAME, qtype=DNS_RR_TYPE_AAAA) /
                          DNSQR(qname=TEST_NAME, qtype=DNS_RR_TYPE_A)),
                      an=(DNSRR(rrname=TEST_NAME, type=DNS_RR_TYPE_AAAA,
                                rdlen=DNS_RR_TYPE_AAAA_DLEN,
                                rdata=TEST_AAAA_DATA) /
                          DNSRR(rrname=TEST_NAME, type=DNS_RR_TYPE_A,
                                rdlen=DNS_RR_TYPE_A_DLEN, rdata=TEST_A_DATA))))
    assert(successful_dns_request(child, TEST_NAME, TEST_AAAA_DATA))


def testfunc(child):
    global server
    tap = get_bridge(os.environ["TAP"])
    lladdr = get_host_lladdr(tap)

    try:
        server = Server(family=socket.AF_INET6, type=socket.SOCK_DGRAM,
                        bind_addr=lladdr + "%" + tap, bind_port=SERVER_PORT)
        server.start()
        dns_server(child, lladdr, SERVER_PORT)

        def run(func):
            if child.logfile == sys.stdout:
                print(func.__name__)
                func(child)
            else:
                try:
                    func(child)
                    print(".", end="", flush=True)
                except Exception as e:
                    print("FAILED")
                    raise e

        run(test_success)
        print("SUCCESS")
    finally:
        if server is not None:
            server.stop()


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=False))
