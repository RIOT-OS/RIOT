#!/usr/bin/env python3

# -*- coding: utf-8 -*-
# vim: sw=4:ts=4:si:et:enc=utf-8

# Author: Ivan A-R <ivan@tuxotronic.org>
# With hacky error recovery by Gordon Williams <gw@pur3.co.uk>
# Project page: http://tuxotronic.org/wiki/projects/stm32loader
#
# This file is part of stm32loader.
#
# stm32loader is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 3, or (at your option) any later
# version.
#
# stm32loader is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with stm32loader; see the file COPYING3.  If not see
# <http://www.gnu.org/licenses/>.

from __future__ import print_function

import sys
import getopt
import serial
import time
import glob
import tempfile
import os
import subprocess

try:
    from progressbar import *
    usepbar = 1
except:
    usepbar = 0

# Verbose level
QUIET = 0

def mdebug(level, message):
    if QUIET >= level:
        print(message, file=sys.stderr)

# Takes chip IDs (obtained via Get ID command) to human-readable names
CHIP_ID_STRS = {0x410: 'STM32F1, performance, medium-density',
                0x411: 'STM32F2',
                0x412: 'STM32F1, performance, low-density',
                0x413: 'STM32F4',
                0x414: 'STM32F1, performance, high-density',
                0x416: 'STM32L1, performance, medium-density',
                0x418: 'STM32F1, connectivity',
                0x420: 'STM32F1, value, medium-density',
                0x428: 'STM32F1, value, high-density',
                0x429: 'STM32L1',
                0x430: 'STM32F1, performance, XL-density'}

class CmdException(Exception):
    pass

class CommandInterface(object):
    def open(self, aport='/dev/tty.usbserial-FTD3TMCH', abaudrate=115200) :
        self.sp = serial.Serial(
            port=aport,
            baudrate=abaudrate,     # baudrate
            bytesize=8,             # number of databits
            parity=serial.PARITY_EVEN,
            stopbits=1,
            xonxoff=0,              # enable software flow control
            rtscts=0,               # disable RTS/CTS flow control
            timeout=0.5             # set a timeout value, None for waiting forever
        )


    def _wait_for_ack(self, info="", timeout=0):
        stop = time.time() + timeout
        got = None
        while not got:
            got = self.sp.read(1)
            if time.time() > stop:
                break

        if not got:
            raise CmdException("No response to %s" % info)

        # wait for ask
        ask = ord(got)

        if ask == 0x79:
            # ACK
            return 1
        elif ask == 0x1F:
            # NACK
            raise CmdException("Chip replied with a NACK during %s" % info)

        # Unknown response
        raise CmdException("Unrecognised response 0x%x to %s" % (ask, info))

    def reset(self, swapRtsDtr=False):
        if swapRtsDtr:
            self.sp.setRTS(1)
            time.sleep(0.1)
            self.sp.setRTS(0)
            time.sleep(0.5)
        else:
            self.sp.setDTR(1)
            time.sleep(0.1)
            self.sp.setDTR(0)
            time.sleep(0.5)

    def initChip(self, swapRtsDtr=False):
        # Set boot
        if swapRtsDtr:
            self.sp.setDTR(0)
        else:
            self.sp.setRTS(0)

        self.reset(swapRtsDtr)

        # Be a bit more persistent when trying to initialise the chip
        stop = time.time() + 5.0

        while time.time() <= stop:
            self.sp.write(bytes([0x7f]))

            got = self.sp.read()

            # The chip will ACK a sync the very first time and
            # NACK it every time afterwards
            if got and got in bytes([0x79,0x1f]):
                # Synced up
                return

        raise CmdException('No response while trying to sync')

    def releaseChip(self, swapRtsDtr=False):
        if swapRtsDtr:
            self.sp.setDTR(1)
        else:
            self.sp.setRTS(1)

        self.reset()

    def cmdGeneric(self, cmd):
        cmdByte = bytes([cmd])
        ctrlByte = bytes([cmd ^ 0xFF])
        self.sp.write(cmdByte)
        self.sp.write(ctrlByte) # Control byte
        return self._wait_for_ack(hex(cmd))

    def cmdGet(self):
        if self.cmdGeneric(0x00):
            mdebug(10, "*** Get command");
            len = ord(self.sp.read())
            version = ord(self.sp.read())
            mdebug(10, "    Bootloader version: "+hex(version))
            dat = map(lambda c: hex(ord(c)), self.sp.read(len))
            mdebug(10, "    Available commands: "+str(dat))
            self._wait_for_ack("0x00 end")
            return version
        else:
            raise CmdException("Get (0x00) failed")

    def cmdGetVersion(self):
        if self.cmdGeneric(0x01):
            mdebug(10, "*** GetVersion command")
            version = ord(self.sp.read())
            self.sp.read(2)
            self._wait_for_ack("0x01 end")
            mdebug(10, "    Bootloader version: "+hex(version))
            return version
        else:
            raise CmdException("GetVersion (0x01) failed")

    def cmdGetID(self):
        if self.cmdGeneric(0x02):
            mdebug(10, "*** GetID command")
            len = ord(self.sp.read())
            id = self.sp.read(len+1)
            self._wait_for_ack("0x02 end")
            return id
        else:
            raise CmdException("GetID (0x02) failed")


    def _encode_addr(self, addr):
        byte3 = (addr >> 0) & 0xFF
        byte2 = (addr >> 8) & 0xFF
        byte1 = (addr >> 16) & 0xFF
        byte0 = (addr >> 24) & 0xFF
        crc = byte0 ^ byte1 ^ byte2 ^ byte3
        return [byte0, byte1, byte2, byte3, crc]


    def cmdReadMemory(self, addr, lng):
        assert(lng <= 256)
        if self.cmdGeneric(0x11):
            mdebug(10, "*** ReadMemory command")
            self.sp.write(self._encode_addr(addr))
            self._wait_for_ack("0x11 address failed")
            N = (lng - 1) & 0xFF
            crc = N ^ 0xFF
            self.sp.write(bytes([N, crc]))
            self._wait_for_ack("0x11 length failed")
            return self.sp.read(lng)
        else:
            raise CmdException("ReadMemory (0x11) failed")


    def cmdGo(self, addr):
        if self.cmdGeneric(0x21):
            mdebug(10, "*** Go command")
            self.sp.write(self._encode_addr(addr))
            self._wait_for_ack("0x21 go failed")
        else:
            raise CmdException("Go (0x21) failed")


    def cmdWriteMemory(self, addr, data):
        assert(len(data) <= 256)
        if self.cmdGeneric(0x31):
            mdebug(10, "*** Write memory command")
            self.sp.write(bytes(self._encode_addr(addr)))
            self._wait_for_ack("0x31 address failed")
            #map(lambda c: hex(ord(c)), data)
            lng = (len(data)-1) & 0xFF
            mdebug(10, "    %s bytes to write" % [lng+1]);
            self.sp.write(bytes([lng])) # len really
            crc = 0xFF
            try:
              dataBytes = []
              for c in data:
                  crc = crc ^ c
                  dataBytes.append(c)
              dataBytes.append(crc)
              self.sp.write(bytes(dataBytes))
              self._wait_for_ack("0x31 programming failed")
              mdebug(10, "    Write memory done")
            except:
              mdebug(5, "    WRITE FAIL - try and recover")
              for c in data:
                self.sp.write(bytes([255]))
              mdebug(5, "    WRITE FAIL - wait")
              stop = time.time() + 1
              while time.time() < stop:
                if self.sp.inWaiting()>0: self.sp.read(self.sp.inWaiting())
              mdebug(5, "    WRITE FAIL - retry")
              self.cmdWriteMemory(addr, data)
        else:
            raise CmdException("Write memory (0x31) failed")


    def cmdEraseMemory(self, sectors = None):
        if self.cmdGeneric(0x43):
            mdebug(10, "*** Erase memory command")
            if sectors is None:
                # Global erase
                self.sp.write(chr(0xFF))
                self.sp.write(chr(0x00))
            else:
                # Sectors erase
                self.sp.write(chr((len(sectors)-1) & 0xFF))
                crc = 0xFF
                for c in sectors:
                    crc = crc ^ c
                    self.sp.write(chr(c))
                self.sp.write(chr(crc))
            self._wait_for_ack("0x43 erasing failed")
            mdebug(10, "    Erase memory done")
        else:
            raise CmdException("Erase memory (0x43) failed")


    GLOBAL_ERASE_TIMEOUT_SECONDS = 20   # This takes a while
    def cmdExtendedEraseMemory(self, useSectorErase = False, amountOfSectors = 0x1ff):
        if self.cmdGeneric(0x44):
            if not useSectorErase:
                mdebug(10, "*** Extended erase memory command")
                # Global mass erase
                mdebug(5, "Global mass erase; this may take a while")
                self.sp.write(chr(0xFF))
                self.sp.write(chr(0xFF))
                # Checksum
                self.sp.write(chr(0x00))
                self._wait_for_ack("0x44 extended erase failed",
                                timeout=self.GLOBAL_ERASE_TIMEOUT_SECONDS)
                mdebug(10, "    Extended erase memory done")
            else:
                mdebug(10, "    Performing non global erase")

                # Data to be sent
                data = []

                crc = 0
                msb = (amountOfSectors >> 8) & 0xff
                lsb = amountOfSectors & 0xff

                crc ^= msb
                crc ^= lsb

                data.append(msb)
                data.append(lsb)

                for sector in range(0, amountOfSectors+1):
                    msb = sector >> 8 & 0xff
                    lsb = sector & 0xff
                    crc ^= msb
                    crc ^= lsb
                    data.append(msb)
                    data.append(lsb)

                data.append(crc)

                for b in data:
                    self.sp.write(bytes([b]))

            self._wait_for_ack("0x44 erasing failed", timeout=self.GLOBAL_ERASE_TIMEOUT_SECONDS)
            mdebug(10, "    Erase memory done")
        else:
            raise CmdException("Extended erase memory (0x44) failed")


    def cmdWriteProtect(self, sectors):
        if self.cmdGeneric(0x63):
            mdebug(10, "*** Write protect command")
            self.sp.write(chr((len(sectors)-1) & 0xFF))
            crc = 0xFF
            for c in sectors:
                crc = crc ^ c
                self.sp.write(chr(c))
            self.sp.write(chr(crc))
            self._wait_for_ack("0x63 write protect failed")
            mdebug(10, "    Write protect done")
        else:
            raise CmdException("Write Protect memory (0x63) failed")

    def cmdWriteUnprotect(self):
        if self.cmdGeneric(0x73):
            mdebug(10, "*** Write Unprotect command")
            self._wait_for_ack("0x73 write unprotect failed")
            self._wait_for_ack("0x73 write unprotect 2 failed")
            mdebug(10, "    Write Unprotect done")
        else:
            raise CmdException("Write Unprotect (0x73) failed")

    def cmdReadoutProtect(self):
        if self.cmdGeneric(0x82):
            mdebug(10, "*** Readout protect command")
            self._wait_for_ack("0x82 readout protect failed")
            self._wait_for_ack("0x82 readout protect 2 failed")
            mdebug(10, "    Read protect done")
        else:
            raise CmdException("Readout protect (0x82) failed")

    def cmdReadoutUnprotect(self):
        if self.cmdGeneric(0x92):
            mdebug(10, "*** Readout Unprotect command")
            self._wait_for_ack("0x92 readout unprotect failed")
            self._wait_for_ack("0x92 readout unprotect 2 failed")
            mdebug(10, "    Read Unprotect done")
        else:
            raise CmdException("Readout unprotect (0x92) failed")


# Complex commands section

    def readMemory(self, addr, lng):
        data = bytes([])
        if usepbar:
            widgets = ['Reading: ', Percentage(),', ', ETA(), ' ', Bar()]
            pbar = ProgressBar(widgets=widgets,maxval=lng, term_width=79).start()

        while lng > 256:
            if usepbar:
                pbar.update(pbar.maxval-lng)
            else:
                mdebug(5, "Read %(len)d bytes at 0x%(addr)X" % {'addr': addr, 'len': 256})
            data += self.cmdReadMemory(addr, 256)
            addr = addr + 256
            lng = lng - 256
        if usepbar:
            pbar.update(pbar.maxval-lng)
            pbar.finish()
        else:
            mdebug(5, "Read %(len)d bytes at 0x%(addr)X" % {'addr': addr, 'len': 256})
        data += self.cmdReadMemory(addr, lng)
        return data

    def writeMemory(self, addr, data):
        lng = len(data)

        mdebug(5, "Writing %(lng)d bytes to start address 0x%(addr)X" %
               { 'lng': lng, 'addr': addr})

        if usepbar:
            widgets = ['Writing: ', Percentage(),' ', ETA(), ' ', Bar()]
            pbar = ProgressBar(widgets=widgets, maxval=lng, term_width=79).start()

        offs = 0
        while lng > 256:
            if usepbar:
                pbar.update(pbar.maxval-lng)
            else:
                mdebug(5, "Write %(len)d bytes at 0x%(addr)X" % {'addr': addr, 'len': 256})
            self.cmdWriteMemory(addr, data[offs:offs+256])
            offs = offs + 256
            addr = addr + 256
            lng = lng - 256
        if usepbar:
            pbar.update(pbar.maxval-lng)
            pbar.finish()
        else:
            mdebug(5, "Write %(len)d bytes at 0x%(addr)X" % {'addr': addr, 'len': 256})
        self.cmdWriteMemory(addr, data[offs:offs+lng] + bytes([0xFF] * (256-lng)) )

    def PCLKHack(self):
        RCC_CFGR = 0x40021004
        mdebug(5, "Modifying PCLK speed at 0x%(addr)X" % {'addr': RCC_CFGR})
#        reg = self.cmdReadMemory(RCC_CFGR, 4)
#        reg[1] = (reg[1] & 0xF8) | 0x04
        reg = [10, 60, 29, 0]
#        self.cmdWriteMemory(RCC_CFGR, reg)
        if self.cmdGeneric(0x31):
          self.sp.write(self._encode_addr(RCC_CFGR))
          self._wait_for_ack("0x31 address failed")
          self.sp.write(chr(3)) # len really
          self.sp.write(chr(reg[0]))
          self.sp.write(chr(reg[1]))
          self.sp.write(chr(reg[2]))
          self.sp.write(chr(reg[3]))
          crc = 3^reg[0]^reg[1]^reg[2]^reg[3];
          self.sp.write(chr(crc))
          self._wait_for_ack("0x31 programming failed")
          mdebug(10, "    PCLK write memory done")

    def resetDevice(self):
        AIRCR = 0xE000ED0C
        mdebug(5, "Writing to Reset Register")
        reg = [0x04,0x00,0xFA,0x05]
        if self.cmdGeneric(0x31):
          self.sp.write(self._encode_addr(AIRCR))
          self._wait_for_ack("0x31 address failed")
          self.sp.write(chr(3)) # len really
          self.sp.write(chr(reg[0]))
          self.sp.write(chr(reg[1]))
          self.sp.write(chr(reg[2]))
          self.sp.write(chr(reg[3]))
          crc = 3^reg[0]^reg[1]^reg[2]^reg[3];
          self.sp.write(chr(crc))
          # don't wait for ack - device will have rebooted
          mdebug(10, "    reset done")


def usage():
    print("""Usage: %s [-hqVewvrXuS] [-l length] [-p port] [-b baud] [-a addr] [file.bin]
    -h          This help
    -q          Quiet
    -V          Verbose
    -e          Erase
    -w          Write
    -v          Verify
    -X          Reset after
    -r          Read
    -u          Use sector erase instead of global erase. You need to specify the amount of sectors with '-l'
    -l length   Length of read or to erase when using sector erase
    -S          Swap RTS and DTR: use RTS for reset and DTR for boot0
    -p port     Serial port (default: first USB-like port in /dev)
    -b baud     Baud speed (default: 115200)
    -a addr     Target address
    -s n        Skip writing N bytes from beginning of the binary (does not affect start address)
    -k          Change PCLK frequency to make USB stable on Espruino 1v43 bootloaders

    Example: ./stm32loader.py -e -w -v example/main.bin

    To use sector erase instead of global: ./stm32loader.py -e -u -w -v -l 0x1ff example/main.bin

    """ % sys.argv[0])

def read(filename):
    """Read the file to be programmed and turn it into a binary"""
    with open(filename, 'rb') as f:
        bytes = f.read()

    if bytes.startswith(b'\x7FELF'):
        # Actually an ELF file.  Convert to binary
        handle, path = tempfile.mkstemp(suffix='.bin', prefix='stm32loader')

        try:
            os.close(handle)

            # Try a couple of options for objcopy
            for name in ['arm-none-eabi-objcopy', 'arm-linux-gnueabi-objcopy']:
                try:
                    code = subprocess.call([name, '-Obinary', filename, path])

                    if code == 0:
                        return read(path)
                except OSError:
                    pass
            else:
                raise Exception('Error %d while converting to a binary file' % code)
        finally:
            # Remove the temporary file
            os.unlink(path)
    else:
        return bytes

if __name__ == "__main__":

    had_error = False

    conf = {
            'port': 'auto',
            'baud': 115200,
            'address': 0x08000000,
            'skip' : 0,
            'erase': 0,
            'useSectorErase': False,
            'swapRtsDtr': False,
            'write': 0,
            'verify': 0,
            'read': 0,
            'reset': 0,
            'len': 1000,
            'fname':'',
            'pclk_hack':0,
        }

# http://www.python.org/doc/2.5.2/lib/module-getopt.html

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hqVewvrXudSp:b:a:c:s:l:k")
    except getopt.GetoptError as err:
        # print help information and exit:
        print(str(err)) # will print something like "option -a not recognized"
        usage()
        sys.exit(2)

    for o, a in opts:
        if o == '-V':
            QUIET = 10
        elif o == '-q':
            QUIET = 0
        elif o == '-h':
            usage()
            sys.exit(0)
        elif o == '-e':
            conf['erase'] = 1
        elif o == '-w':
            conf['write'] = 1
        elif o == '-v':
            conf['verify'] = 1
        elif o == '-r':
            conf['read'] = 1
        elif o == '-X':
            conf['reset'] = 1
        elif o == '-p':
            conf['port'] = a
        elif o == '-u':
            conf['useSectorErase'] = True
        elif o == '-b':
            conf['baud'] = eval(a)
        elif o == '-a':
            conf['address'] = eval(a)
        elif o == '-s':
            conf['skip'] = eval(a)
        elif o == '-l':
            conf['len'] = eval(a)
        elif o == '-k':
            conf['pclk_hack'] = 1
        elif o == '-S':
            conf['swapRtsDtr'] = True
        else:
            assert False, "unhandled option"
    # Try and find the port automatically
    if conf['port'] == 'auto':
        ports = []

        # Get a list of all USB-like names in /dev
        for name in ['tty.usbserial', 'ttyUSB']:
            ports.extend(glob.glob('/dev/%s*' % name))

        ports = sorted(ports)

        if ports:
            # Found something - take it
            conf['port'] = ports[0]

    cmd = CommandInterface()
    cmd.open(conf['port'], conf['baud'])
    mdebug(10, "Open port %(port)s, baud %(baud)d" % {'port':conf['port'],
                                                      'baud':conf['baud']})
    try:
        if (conf['write'] or conf['verify']):
            mdebug(5, "Reading data from %s" % args[0])
            data = read(args[0])

            if conf['skip']:
                mdebug(5, "Skipping %d bytes" % conf['skip'])
                data = data[conf['skip']:]

        try:
            cmd.initChip(conf['swapRtsDtr'])
        except CmdException:
            print("Can't init. Ensure BOOT0=1, BOOT1=0, and reset device")

        bootversion = cmd.cmdGet()

        mdebug(0, "Bootloader version 0x%X" % bootversion)

        if bootversion < 20 or bootversion >= 100:
            raise Exception('Unreasonable bootloader version %d' % bootversion)

        chip_id = cmd.cmdGetID()
        assert len(chip_id) == 2, "Unreasonable chip id: %s" % repr(chip_id)
        chip_id_num = (chip_id[0] << 8) | chip_id[1]
        chip_id_str = CHIP_ID_STRS.get(chip_id_num, None)

        if chip_id_str is None:
            mdebug(0, 'Warning: unrecognised chip ID 0x%x' % chip_id_num)
        else:
            mdebug(0, "Chip id 0x%x, %s" % (chip_id_num, chip_id_str))

        if conf['pclk_hack']:
            cmd.PCLKHack()

        if conf['erase']:
            # Pre-3.0 bootloaders use the erase memory
            # command. Starting with 3.0, extended erase memory
            # replaced this command.
            if bootversion < 0x30:
                cmd.cmdEraseMemory()
            else:
                cmd.cmdExtendedEraseMemory(conf['useSectorErase'], conf['len'])

        if conf['write']:
            print("Writing binary")
            cmd.writeMemory(conf['address'], data)

        if conf['verify']:
            verify = cmd.readMemory(conf['address'], len(data))
            if(data == verify):
                print("Verification OK")
            else:
                print("Verification FAILED")
                print(str(len(data)) + ' vs ' + str(len(verify)))
                for i in range(0, len(data)):
                    if data[i] != verify[i]:
                        print(hex(i) + ': ' + hex(data[i]) + ' vs ' + hex(verify[i]))
                had_error = True

        if not conf['write'] and conf['read']:
            rdata = cmd.readMemory(conf['address'], conf['len'])
            file(args[0], 'wb').write(''.join(map(chr,rdata)))

        if conf['reset']:
            cmd.resetDevice()

    finally:
        if not conf['reset']:
            cmd.releaseChip(conf['swapRtsDtr'])

    if had_error: exit(1)
