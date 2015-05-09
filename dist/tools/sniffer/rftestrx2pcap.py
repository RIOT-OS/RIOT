#!/usr/bin/env python2
'''
(C) 2012, Mariano Alvira <mar@devl.org>
(C) 2014, Oliver Hahm <oliver.hahm@inria.fr>
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

import sys,os,time
from struct import pack
import re
import serial

if len(sys.argv) < 3:
    sys.stderr.write( "Usage: %s tty channel [outfile]\n" %(sys.argv[0]))
    sys.stderr.write( "       channel = 11-26\n")
    sys.exit(2)

# change the channel

try:
    serport = serial.Serial(sys.argv[1], baudrate=115200, dsrdtr=0, rtscts=0, timeout=1)
    serport.setDTR(0)
    serport.setRTS(0)
except IOError:
    print "error opening port"
    sys.exit(2)

time.sleep(1)
chanstr = ''
sys.stderr.write('chan %s\n' % sys.argv[2])
serport.write('chan %s\n' % sys.argv[2])
while 1:
    c = serport.read(1)
    if (c == '\n'):
        chanstr = ''
        continue
    chanstr += c
    m = re.match(".*channel: (\w+)", chanstr)
    if m:
        chan = int(m.group(1))
        sys.stderr.write(chanstr + '\n')
        break

try:
    sys.stderr.write('writing to file %s \n' % (sys.argv[3]))
    outfile = open(sys.argv[3], 'w+b')
except IndexError:
    outfile = sys.stdout

sys.stderr.write("RX: 0\r")

### PCAP setup
MAGIC = 0xa1b2c3d4
MAJOR = 2
MINOR = 4
ZONE = 0
SIG = 0
SNAPLEN = 0xffff
NETWORK = 230 # 802.15.4 no FCS

# output overall PCAP header
outfile.write(pack('<LHHLLLL', MAGIC, MAJOR, MINOR, ZONE, SIG, SNAPLEN, NETWORK))

count = 0
fileempty = 1
newpacket = 0

try:
    while 1:
        line = serport.readline().rstrip()

        m_rftestline = re.match(".*rftest-rx --- len 0x(\w\w).*", line)

        if m_rftestline:
            newpacket = 1
            t = time.time()
            sec = int(t)
            usec = (t - sec) * 100000
            length = int(m_rftestline.group(1), 16)
            continue

        # if this is a new packet, add a packet header
        if newpacket == 1:
            newpacket = 0
            outfile.write(pack('<LLLL',sec,usec,length,length))
            outfile.flush()

            count += 1
            sys.stderr.write("RX: %d\r" % count)

            # clear file empty flag
            if fileempty:
                fileempty = 0
        if fileempty == 0 :
            # write payload
            for d in line.split(' '):
                # do a match because their might be a \r floating around
                m = re.match('.*(\w\w).*', d)
                if m:
                    outfile.write(pack('<B', int(m.group(1),16)))
                    outfile.flush()
except KeyboardInterrupt:
#        cn.close()
        sys.exit(2)
