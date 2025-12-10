#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
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

from scapy.all import Ether, ICMPv6PacketTooBig, IPv6, IPv6ExtHdrFragment, \
                      UDP, raw, sendp, srp1
from testrunner import run, check_unittests


BOARD = os.environ.get("BOARD", "")
RECV_BUFSIZE = 2 * 1500
TEST_SAMPLE = b"This is a test. Failure might sometimes be an option, but " \
              b"not today. "
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


def udp_send(child, addr, iface, port, length, num=1, delay=1000000):
    child.sendline("udp send {addr}%{iface} {port} {length} {num} {delay}"
                   .format(**vars()))
    child.expect(r"Success: send {length} byte to \[[0-9a-f:]+\]:{port}"
                 .format(**vars()))


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
                r"~~ SNIP  0 - size: {} byte, type: NETTYPE_UNDEF \(\d+\)"
                .format(payload_len)
            )
        # 4 snips: payload, UDP header, IPv6 header, netif header
        # (fragmentation header was removed)
        child.expect(
                r"~~ PKT    -  4 snips, total size: (\d+) byte"
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


def test_reass_empty_fragment(child, iface, hw_dst, ll_dst, ll_src):
    # Originally proposed by Nils Bernsdorf (Uni Saarland), adapted by Martine Lenders
    # send the first packet (without payload) to initialize the reassembly buffer
    # with a null pointer
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment(id=0xabcd, nh=0, m=1, offset=0),
          iface=iface, verbose=0)
    # send the second packet to potentially trigger a memcpy
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment(id=0xabcd, nh=0, m=1, offset=0) /
          (b"A" * (24 - 8)),
          iface=iface, verbose=0)
    time.sleep(11)  # let reassembly buffer garbage collect
    pktbuf_empty(child)


def test_reass_first_fragment_repeat(child, iface, hw_dst, ll_dst, ll_src):
    # Originally proposed by Nils Bernsdorf (Uni Saarland), adapted by Martine Lenders
    # send the first packet to initialize the reassembly buffer
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment(id=0xabcd, nh=0, m=1, offset=0) /
          (b"A"*(24-8)),
          iface=iface, verbose=0)
    # send the a second larger packet (also with offset 0) to trigger the buffer
    # overflow
    sendp(Ether(dst=hw_dst) / IPv6(dst=ll_dst, src=ll_src) /
          IPv6ExtHdrFragment(id=0xabcd, nh=0, m=1, offset=0) /
          (b"A"*(128-8)),
          iface=iface, verbose=0)
    # the fragments should be discarded
    pktbuf_empty(child)


def test_ipv6_ext_frag_shell_test_0(child, s, iface, ll_dst):
    child.sendline("test {} 0".format(ll_dst))
    data, _ = s.recvfrom(RECV_BUFSIZE)
    assert data == TEST_SAMPLE
    pktbuf_empty(child)


def test_ipv6_ext_frag_shell_test_1(child, s, iface, ll_dst):
    child.sendline("test {} 1".format(ll_dst))
    data, _ = s.recvfrom(RECV_BUFSIZE)
    offset = 0
    while (offset < len(data)):
        assert data[offset:(offset + len(TEST_SAMPLE))] == TEST_SAMPLE
        offset += len(TEST_SAMPLE)
    pktbuf_empty(child)


def _check_iface(child):
    # get TAP MAC address
    child.sendline("ifconfig")
    ethos_id = None
    mock_id = None
    hwaddr = None
    for _ in range(2):
        child.expect(r"Iface\s+(\d+)\s+.*\n")
        match = re.search(r"HWaddr:\s+([0-9A-F:]{17})\s+",
                          child.match.group(0))
        if match is not None:
            # interface has a hardware address
            ethos_id = int(child.match.group(1))
            hwaddr = match.group(1)
        else:
            mock_id = int(child.match.group(1))
        # consume MTU for later calls of `ifconfig {mock_id}`
        child.expect(r"MTU:(\d+)")
    # check if interface is configured properly
    assert ethos_id is not None
    assert mock_id is not None
    assert hwaddr is not None
    return ethos_id, mock_id, hwaddr


def test_ipv6_ext_frag_send_success(child, s, iface, ll_dst):
    length = get_host_mtu(iface)
    port = s.getsockname()[1]
    ethos_id, _, _ = _check_iface(child)
    udp_send(child, ll_dst, ethos_id, port, length)
    data, _ = s.recvfrom(length)
    assert len(data) == length
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_last_fragment_filled(child, s, iface, ll_dst):
    # every fragment has an IPv6 header and a fragmentation header so subtract
    # them
    mtu = get_host_mtu(iface) - len(IPv6() / IPv6ExtHdrFragment())
    # first fragment has UDP header (so subtract it) and is rounded down to
    # the nearest multiple of 8
    length = (mtu - len(UDP())) & 0xfff8
    # second fragment fills the whole available MTU
    length += mtu
    port = s.getsockname()[1]
    ethos_id, _, _ = _check_iface(child)
    udp_send(child, ll_dst, ethos_id, port, length)
    data, _ = s.recvfrom(length)
    assert len(data) == length
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_last_fragment_only_one_byte(child, s,
                                                        iface, ll_dst):
    mtu = get_host_mtu(iface)
    # subtract IPv6 and UDP header as they are not part of the UDP payload
    length = (mtu - len(IPv6() / UDP()))
    length += 1
    port = s.getsockname()[1]
    ethos_id, _, _ = _check_iface(child)
    udp_send(child, ll_dst, ethos_id, port, length)
    data, _ = s.recvfrom(length)
    assert len(data) == length
    pktbuf_empty(child)


def test_ipv6_ext_frag_send_full_pktbuf(child, s, iface, ll_dst):
    length = pktbuf_size(child)
    # remove some slack for meta-data and header and 1 addition fragment header
    slack = 96
    if BOARD in ["native64"]:
        # size_t and pointers are 4 bytes larger in 64-bit architectures, so add some
        # more slack per snip, since they are larger
        slack += 96
    length -= (len(IPv6() / IPv6ExtHdrFragment() / UDP()) +
               (len(IPv6() / IPv6ExtHdrFragment())) + slack)
    port = s.getsockname()[1]
    ethos_id, _, _ = _check_iface(child)
    # trigger neighbor discovery so it doesn't fill the packet buffer
    udp_send(child, ll_dst, ethos_id, port, 1)
    data, _ = s.recvfrom(1)
    last_nd = time.time()
    count = 0
    while True:
        if (time.time() - last_nd) > 5:
            # trigger neighbor discovery so it doesn't fill the packet buffer
            udp_send(child, ll_dst, ethos_id, port, 1)
            data, _ = s.recvfrom(1)
            last_nd = time.time()
        udp_send(child, ll_dst, ethos_id, port, length)
        count += 1
        try:
            data, _ = s.recvfrom(length)
        except socket.timeout:
            # 8 is the alignment unit of the packet buffer
            # and 20 the size of a packet snip, so take next multiple of 8 to
            # 28
            length -= 24
        else:
            break
        finally:
            pktbuf_empty(child)
    assert(count > 1)


def _fwd_setup(child, ll_dst, g_src, g_dst):
    ethos_id, mock_id, hwaddr = _check_iface(child)
    child.sendline("ifconfig {}".format(mock_id))
    child.expect(r"MTU:(\d+)")
    mtu = int(child.match.group(1))
    # configure routes
    child.sendline("nib route add {} {}/128 fe80::1".format(mock_id, g_dst))
    child.sendline("nib route add {} {}/128 {}"
                   .format(ethos_id, g_src, ll_dst))
    child.sendline("nib route")
    child.expect(r"{}/128 via fe80::1 dev #{}".format(g_dst, mock_id))
    child.expect(r"{}/128 via {} dev #{}".format(g_src, ll_dst, ethos_id))
    child.sendline("nib neigh add {} fe80::1".format(mock_id))
    child.sendline("nib neigh")
    child.expect(r"fe80::1 dev #{} lladdr\s+-".format(mock_id))
    return mock_id, mtu, hwaddr


def _fwd_teardown(child, mock_id):
    # remove route
    child.sendline("nib neigh del {} fe80::1".format(mock_id))
    child.sendline("nib route del {} affe::/64".format(mock_id))


def test_ipv6_ext_frag_fwd_success(child, s, iface, ll_dst):
    mock_id, mtu, dst_mac = _fwd_setup(child, ll_dst, "beef::1", "affe::1")
    payload_fit = mtu - len(IPv6() / IPv6ExtHdrFragment() / UDP())
    pkt = Ether(dst=dst_mac) / IPv6(src="beef::1", dst="affe::1") / \
        IPv6ExtHdrFragment(m=True, id=0x477384a9) / \
        UDP(sport=1337, dport=1337) / ("x" * payload_fit)
    # fill missing fields
    pkt = Ether(raw(pkt))
    sendp(pkt, verbose=0, iface=iface)
    # check hexdump of mock device
    ipv6 = pkt[IPv6]
    ipv6.hlim -= 1  # the packet will have passed a hop
    # segment packet as GNRC does
    segments = [bytes(ipv6)[:40], bytes(ipv6.payload)]
    for seg in segments:
        addr = 0
        for i in range(0, len(seg), 16):
            bs = seg[i:i+16]
            exp_str = ("{:08X}" + ("  {:02X}") * len(bs)).format(addr, *bs)
            child.expect_exact(exp_str)
            addr += 16
    _fwd_teardown(child, mock_id)


def test_ipv6_ext_frag_fwd_too_big(child, s, iface, ll_dst):
    mock_id, mtu, dst_mac = _fwd_setup(child, ll_dst, "beef::1", "affe::1")
    assert(get_host_mtu(iface) > mtu)
    payload_fit = get_host_mtu(iface) - len(IPv6() / IPv6ExtHdrFragment() /
                                            UDP())
    pkt = srp1(Ether(dst=dst_mac) / IPv6(src="beef::1", dst="affe::1") /
               IPv6ExtHdrFragment(m=True, id=0x477384a9) /
               UDP(sport=1337, dport=1337) / ("x" * payload_fit),
               timeout=2, verbose=0, iface=iface)
    # packet should not be fragmented further but an ICMPv6 error should be
    # returned instead
    assert(pkt is not None)
    assert(ICMPv6PacketTooBig in pkt)
    assert(IPv6ExtHdrFragment in pkt)
    assert(pkt[IPv6ExtHdrFragment].id == 0x477384a9)
    _fwd_teardown(child, mock_id)


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])

    child.sendline("unittests")
    # wait for and check result of unittests
    print("." * check_unittests(child), end="", flush=True)

    lladdr_src = get_host_lladdr(tap)

    def run_sock_test(func, s):
        if child.logfile == sys.stdout:
            func(child, s, tap, lladdr_src)
        else:
            try:
                func(child, s, tap, lladdr_src)
                print(".", end="", flush=True)
            except PermissionError:
                print("\n\x1b[1;33mSkipping {} because of missing "
                      "privileges\x1b[0m".format(func.__name__))
            except Exception as e:
                print("FAILED")
                raise e

    child.sendline("send-test-pkt")
    child.expect(r"Sending UDP test packets to port (\d+)\r\n")

    port = int(child.match.group(1))
    with socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) as s:
        res = socket.getaddrinfo("{}%{}".format(lladdr_src, tap), port)
        s.bind(res[0][4])
        s.settimeout(.3)
        run_sock_test(test_ipv6_ext_frag_shell_test_0, s)
        run_sock_test(test_ipv6_ext_frag_shell_test_1, s)
        run_sock_test(test_ipv6_ext_frag_send_success, s)
        run_sock_test(test_ipv6_ext_frag_send_last_fragment_filled, s)
        run_sock_test(test_ipv6_ext_frag_send_last_fragment_only_one_byte, s)
        run_sock_test(test_ipv6_ext_frag_send_full_pktbuf, s)
        run_sock_test(test_ipv6_ext_frag_fwd_success, s)
        run_sock_test(test_ipv6_ext_frag_fwd_too_big, s)

    if BOARD not in ['native', 'native32', 'native64']:
        # ethos currently can't handle the larger, rapidly sent packets by the
        # IPv6 fragmentation of the Linux Kernel
        print("SUCCESS")
        print("Skipping datagram reception tests due to ethos bug.")
        return

    # datagram reception tests
    res = 1
    count = 0
    while res:
        # check `ifconfig` and also get addresses from it until
        # link-local address becomes valid
        time.sleep(1)
        child.sendline("ifconfig")
        child.expect(r"HWaddr: (?P<hwaddr>[A-Fa-f:0-9]+)\s")
        hwaddr_dst = child.match.group("hwaddr").lower()
        res = child.expect([
            r"(?P<lladdr>fe80::[A-Fa-f:0-9]+)\s+scope:\s+link\s+VAL",
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
    run(test_reass_empty_fragment)
    run(test_reass_first_fragment_repeat)
    print("SUCCESS")


if __name__ == "__main__":
    if os.geteuid() != 0:
        print("\x1b[1;31mThis test requires root privileges.\n"
              "It's constructing and sending Ethernet frames.\x1b[0m\n",
              file=sys.stderr)
        sys.exit(1)
    sys.exit(run(testfunc, timeout=2, echo=False))
