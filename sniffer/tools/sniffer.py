#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
(C) 2012, Mariano Alvira <mar@devl.org>
(C) 2014, Oliver Hahm <oliver.hahm@inria.fr>
(C) 2015, Hauke Petersen <hauke.petersen@fu-berlin.de>
(C) 2015, Martine Lenders <mlenders@inf.fu-berlin.de>
(C) 2015, Cenk Gündoğan <cnkgndgn@gmail.com>

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of the Institute nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
"""

from __future__ import print_function
import argparse
import sys
import re
import socket
from time import sleep, time
from struct import pack
from serial import Serial

# PCAP setup
MAGIC = 0xA1B2C3D4
MAJOR = 2
MINOR = 4
ZONE = 0
SIG = 0
SNAPLEN = 0xFFFF
NETWORK = 230  # 802.15.4 no FCS

DEFAULT_BAUDRATE = 115200


def configure_interface(port, channel):
    line = ""
    iface = 0
    port.write("ifconfig\n".encode())
    while True:
        line = port.readline()
        if line == b"":
            print("Application has no network interface defined", file=sys.stderr)
            sys.exit(2)
        match = re.search(r"^Iface +(\d+)", line.decode(errors="ignore"))
        if match is not None:
            iface = int(match.group(1))
            break

    # set channel, raw mode, and promiscuous mode
    print("ifconfig %d set chan %d" % (iface, channel), file=sys.stderr)
    print("ifconfig %d raw" % iface, file=sys.stderr)
    print("ifconfig %d promisc" % iface, file=sys.stderr)
    port.write(("ifconfig %d set chan %d\n" % (iface, channel)).encode())
    port.write(("ifconfig %d raw\n" % iface).encode())
    port.write(("ifconfig %d promisc\n" % iface).encode())


def generate_pcap(port, out):
    # count incoming packets
    count = 0
    # output overall PCAP header
    out.write(pack("<LHHLLLL", MAGIC, MAJOR, MINOR, ZONE, SIG, SNAPLEN, NETWORK))
    sys.stderr.write("RX: %i\r" % count)
    while True:
        line = port.readline().rstrip()

        pkt_header = re.match(
            r">? *rftest-rx --- len (\w+).*", line.decode(errors="ignore")
        )
        if pkt_header:
            now = time()
            sec = int(now)
            usec = int((now - sec) * 1000000)
            length = int(pkt_header.group(1), 16)
            out.write(pack("<LLLL", sec, usec, length, length))
            out.flush()
            count += 1
            sys.stderr.write("RX: %i\r" % count)
            continue

        pkt_data = re.match(r"(\w\w )+", line.decode(errors="ignore"))
        if pkt_data:
            for part in line.decode(errors="ignore").split(" "):
                byte = re.match(r"(\w\w)", part)
                if byte:
                    out.write(pack("<B", int(byte.group(1), 16)))
            out.flush()


def connect(args):
    conn = None
    if args.conn.startswith("/dev/tty") or args.conn.startswith("COM"):
        # open serial port
        try:
            conn = Serial(args.conn, args.baudrate, dsrdtr=0, rtscts=0, timeout=1)
        except IOError:
            print("error opening serial port %s" % args.conn, file=sys.stderr)
            sys.exit(2)
    else:
        try:
            port = args.conn.split(":")[-1]
            host = args.conn[: -(len(port) + 1)]
            port = int(port)
        except (IndexError, ValueError):
            print("Can't parse host:port pair %s" % args.conn, file=sys.stderr)
            sys.exit(2)
        try:
            sock = socket.socket()
            sock.connect((host, port))
            conn = sock.makefile("r+b", bufsize=0)
        except IOError:
            print("error connecting to %s:%s" % (host, port), file=sys.stderr)
            sys.exit(2)
    return conn


def main():
    if sys.version_info > (3,):
        default_outfile = sys.stdout.buffer
    else:
        default_outfile = sys.stdout
    p = argparse.ArgumentParser()
    p.add_argument(
        "-b",
        "--baudrate",
        type=int,
        default=DEFAULT_BAUDRATE,
        help="Baudrate of the serial port (only evaluated "
        "for non TCP-terminal, default: %d)" % DEFAULT_BAUDRATE,
    )
    p.add_argument(
        "conn",
        metavar="tty/host:port",
        type=str,
        help="Serial port or TCP (host, port) tuple to "
        "terminal with sniffer application",
    )
    p.add_argument("channel", type=int, help="Channel to sniff on")
    p.add_argument(
        "outfile",
        type=argparse.FileType("w+b"),
        default=default_outfile,
        nargs="?",
        help="PCAP file to output to (default: stdout)",
    )
    args = p.parse_args()

    conn = connect(args)

    sleep(1)
    configure_interface(conn, args.channel)
    sleep(1)

    try:
        generate_pcap(conn, args.outfile)
    except KeyboardInterrupt:
        conn.close()
        print()
        sys.exit(2)


if __name__ == "__main__":
    main()
