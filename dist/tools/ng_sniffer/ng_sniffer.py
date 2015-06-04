#!/usr/bin/env python2
'''
(C) 2012, Mariano Alvira <mar@devl.org>
(C) 2014, Oliver Hahm <oliver.hahm@inria.fr>
(C) 2015, Hauke Petersen <hauke.petersen@fu-berlin.de>
(C) 2015, Martine Lenders <mlenders@inf.fu-berlin.de>

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
'''

import sys
import re
from time import sleep, time
from struct import pack
from serial import Serial

# PCAP setup
MAGIC = 0xa1b2c3d4
MAJOR = 2
MINOR = 4
ZONE = 0
SIG = 0
SNAPLEN = 0xffff
NETWORK = 230       # 802.15.4 no FCS


def configure_interface(port, channel):
    line = ""
    iface = 0
    port.write('ifconfig\n')
    while True:
        line = port.readline()
        if line == '':
            print >> sys.stderr, "Application has no network interface defined"
            sys.exit(2)
        match = re.search(r'^Iface +(\d+)', line)
        if match is not None:
            iface = int(match.group(1))
            break

    # set channel, raw mode, and promiscuous mode
    print >> sys.stderr, 'ifconfig %d set chan %d' % (iface, channel)
    print >> sys.stderr, 'ifconfig %d raw' % iface
    print >> sys.stderr, 'ifconfig %d promisc' % iface
    port.write('ifconfig %d set chan %d\n' % (iface, channel))
    port.write('ifconfig %d raw\n' % iface)
    port.write('ifconfig %d promisc\n' % iface)


def generate_pcap(port, out):
    # count incoming packets
    count = 0
    # output overall PCAP header
    out.write(pack('<LHHLLLL', MAGIC, MAJOR, MINOR, ZONE, SIG, SNAPLEN,
                   NETWORK))
    sys.stderr.write("RX: %i\r" % count)
    while True:
        line = port.readline().rstrip()

        pkt_header = re.match(r">? *rftest-rx --- len 0x(\w\w).*", line)
        if pkt_header:
            now = time()
            sec = int(now)
            usec = (now - sec) * 100000
            length = int(pkt_header.group(1), 16)
            out.write(pack('<LLLL', sec, usec, length, length))
            out.flush()
            count += 1
            sys.stderr.write("RX: %i\r" % count)
            continue

        pkt_data = re.match(r"(0x\w\w )+", line)
        if pkt_data:
            for part in line.split(' '):
                byte = re.match(r"0x(\w\w)", part)
                if byte:
                    out.write(pack('<B', int(byte.group(1), 16)))
            out.flush()


def main(argv):
    if len(argv) < 4:
        print >> sys.stderr, "Usage: %s tty baudrate channel [outfile]" % \
            (argv[0])
        print >> sys.stderr, "       channel = 11-26"
        sys.exit(2)

    # open serial port
    try:
        serport = Serial(argv[1], argv[2], dsrdtr=0, rtscts=0,
                         timeout=1)
    except IOError:
        print >> sys.stderr, "error opening port"
        sys.exit(2)

    sleep(1)
    configure_interface(serport, int(argv[3]))
    sleep(1)

    # figure out where to write
    try:
        sys.stderr.write('trying to open file %s\n' % argv[4])
        outfile = open(argv[4], 'w+b')
    except IndexError:
        outfile = sys.stdout

    try:
        generate_pcap(serport, outfile)
    except KeyboardInterrupt:
        print ""
        sys.exit(2)


if __name__ == "__main__":
    main(sys.argv)
