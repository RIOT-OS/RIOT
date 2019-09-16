#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re
import os
import pexpect
import socket
import sys
import subprocess
import time

from scapy.all import Ether, IPv6, IPv6ExtHdrFragment, sendp
from testrunner import run


EXT_HDR_NH = {
    IPv6ExtHdrFragment: 44,
  }


def pktbuf_empty(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    first_byte = child.match.group("first_byte")
    size = child.match.group("size")
    child.expect(
            r"~ unused: {} \(next: (\(nil\)|0), size: {}\) ~".format(
                first_byte, size))


def pktbuf_size(child):
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: (?P<first_byte>0x[0-9a-fA-F]+), "
                 r"last byte: 0x[0-9a-fA-F]+ \(size: (?P<size>\d+)\)")
    size = child.match.group("size")
    return int(size)


def start_udp_server(child, port):
    child.sendline("udp server start {}".format(port))
    child.expect_exact("Success: started UDP server on port {}".format(port))


def stop_udp_server(child):
    child.sendline("udp server stop")
    # either way: it is stopped
    child.expect(["Success: stopped UDP server",
                  "Error: server was not running"])


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
            r"inet6 (?P<lladdr>[0-9A-Fa-f:]+)/64",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return res


def get_host_mtu(tap):
    res = check_and_search_output(
            ["ip", "link", "show", tap],
            r"mtu (?P<mtu>1500)",
            "mtu"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}".format(tap)
            )
    else:
        return int(res)


def test_reass_successful_udp(child, iface, hw_dst, ll_dst, ll_src):
    port = 1337
    mtu = get_host_mtu(iface)
    byte_max = 0xff
    payload_len = (byte_max * ((mtu // byte_max) + 1))
    if not (mtu % byte_max):
        payload_len += 1
    start_udp_server(child, port)
    with socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) as s:
        res = socket.getaddrinfo("{}%{}".format(ll_src, iface), None)
        s.bind(res[0][4])
        s.sendto(bytes(i for i in range(byte_max)) * (payload_len // byte_max),
                 (ll_dst, port))
        child.expect(
                "~~ SNIP  0 - size: {} byte, type: NETTYPE_UNDEF \(\d+\)"
                .format(payload_len)
            )
        # 4 snips: payload, UDP header, IPv6 header, netif header
        # (fragmentation header was removed)
        child.expect(
                "~~ PKT    -  4 snips, total size: (\d+) byte"
            )
        size = int(child.match.group(1))
        # 40 = IPv6 header length; 8 = UDP header length
        # >=  since netif header also has a length
        assert size >= (payload_len + 40 + 8)
    stop_udp_server(child)
    pktbuf_empty(child)


def test_reass_too_short_header(child, iface, hw_dst, ll_dst, ll_src):
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src,
          nh=EXT_HDR_NH[IPv6ExtHdrFragment]) / "\x11",
          iface=iface, verbose=0)
    pktbuf_empty(child)


def test_reass_offset_too_large(child, iface, hw_dst, ll_dst, ll_src):
    size = pktbuf_size(child)
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment(offset=((size * 2) // 8)) / "x" * 128,
          iface=iface, verbose=0)
    pktbuf_empty(child)


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])

    child.expect(r"OK \((\d+) tests\)")     # wait for and check result of unittests
    print("." * int(child.match.group(1)), end="", flush=True)

    lladdr_src = get_host_lladdr(tap)
    if os.environ.get("BOARD", "") != "native":
        # ethos currently can't handle the larger, rapidly sent packets by the
        # IPv6 fragmentation of the Linux Kernel
        print("SUCCESS for unittests.")
        print("Skipping interaction tests due to ethos bug.")
        return
    res = 1
    count = 0
    while res:
        # check `ifconfig` and also get addresses from it until
        # link-local address becomes valid
        time.sleep(1)
        child.sendline("ifconfig")
        child.expect("HWaddr: (?P<hwaddr>[A-Fa-f:0-9]+)")
        hwaddr_dst = child.match.group("hwaddr").lower()
        res = child.expect([
            r"(?P<lladdr>fe80::[A-Fa-f:0-9]+)\s+scope:\s+local\s+VAL",
            pexpect.TIMEOUT
        ])
        count += 1
        if res and (count > 5):
            raise pexpect.TIMEOUT("Link-local address did not become valid")
    lladdr_dst = child.match.group("lladdr").lower()

    def run(func):
        if child.logfile == sys.stdout:
            func(child, tap, hwaddr_dst, lladdr_dst, lladdr_src)
        else:
            try:
                func(child, tap, hwaddr_dst, lladdr_dst, lladdr_src)
                print(".", end="", flush=True)
            except PermissionError:
                print("\n\x1b[1;33mSkipping {} because of missing "
                      "privileges\x1b[0m".format(func.__name__))
            except Exception as e:
                print("FAILED")
                raise e

    run(test_reass_successful_udp)
    run(test_reass_too_short_header)
    run(test_reass_offset_too_large)
    print("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=1, echo=False))
