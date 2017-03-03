#!/usr/bin/env python

'''
MIT License

Copyright (c) 2014 Gregoire Passault

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
'''

# This script sends a program on a robotis board (OpenCM9.04 or CM900)
# using the robotis bootloader (used in OpenCM IDE)
#
# Usage:
# python robotis-loader.py <serial port> <binary>
#
# Example:
# python robotis-loader.py /dev/ttyACM0 firmware.bin
#
# https://github.com/Gregwar/robotis-loader

import serial
import sys
import os
import time

print('~~ Robotis loader ~~')
print('')
print('Please, make sure to connect the USB cable WHILE holding down the "USER SW" button.')
print('Status LED should stay lit and the board should be able to load the program.')
print('')


# Helper function for bytes conversion
if sys.version_info[:1][0] == 3:
    def to_ord(val):
        return ord(chr(val))
else:
    def to_ord(val):
        return ord(val)


# Reading command line
#if len(sys.argv) != 3:
#    exit('! Usage: robotis-loader.py <serial-port> <binary>')
#pgm, port, binary = sys.argv

pgm = sys.argv[0]
port = os.environ["PORT"]
binary = os.environ["HEXFILE"]


def progressBar(percent, precision=65):
    """Prints a progress bar."""

    threshold = precision*percent / 100.0
    sys.stdout.write('[ ')
    for x in range(precision):
        if x < threshold:
            sys.stdout.write('#')
        else:
            sys.stdout.write(' ')
    sys.stdout.write(' ] ')
    sys.stdout.flush()


# Opening the firmware file
try:
    stat = os.stat(binary)
    size = stat.st_size
    firmware = open(binary, 'rb')
    print('* Opening %s, size=%d' % (binary, size))
except:
    exit('! Unable to open file %s' % binary)

# Opening serial port
try:
    s = serial.Serial(port, baudrate=115200)
except:
    exit('! Unable to open serial port %s' % port)

print('* Resetting the board')
s.setRTS(True)
s.setDTR(False)
time.sleep(0.1)
s.setRTS(False)
s.write(b'CM9X')
s.close()
time.sleep(1.0);

print('* Connecting...')
s = serial.Serial(port, baudrate=115200)
s.write(b'AT&LD')
print('* Download signal transmitted, waiting...')

# Entering bootloader sequence
while True:
    line = s.readline().strip()
    if line.endswith(b'Ready..'):
        print('* Board ready, sending data')
        cs = 0
        pos = 0
        while True:
            c = firmware.read(2048)
            if len(c):
                pos += len(c)
                sys.stdout.write("\r")
                progressBar(100 * float(pos) / float(size))
                s.write(c)
                for k in range(0, len(c)):
                    cs = (cs + to_ord(c[k])) % 256
            else:
                firmware.close()
                break
        print('')
        s.setDTR(True)
        print('* Checksum: %d' % (cs))
        import struct
        s.write(struct.pack('B', cs))
        # s.write('{0}'.format(chr(cs)).encode('ascii'))
        print('* Firmware was sent')
    else:
        if line == b'Success..':
            print('* Success, running the code')
            print('')
            s.write(b'AT&RST')
            s.close()
            exit()
        else:
            print('Board -> {}'.format(line))
