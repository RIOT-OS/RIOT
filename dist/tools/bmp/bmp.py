#!/usr/bin/python3

# Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author   Maximilian Deubel <maximilian.deubel@ovgu.de>

# Black Magic Probe helper script
# This script can detect connected Black Magic Probes and can be used as a flashloader and much more

import argparse
import os
import re
import sys

import humanize
import serial.tools.list_ports
from progressbar import Bar, Percentage, ProgressBar
from pygdbmi.gdbcontroller import GdbController
import distutils.spawn

parser = argparse.ArgumentParser(description='Black Magic Tool helper script.')
parser.add_argument('--jtag', action='store_true', help='use JTAG transport')
parser.add_argument('--swd', action='store_true', help='use SWD transport (default)')
parser.add_argument('--connect-srst', action='store_true', help='reset target while connecting')
parser.add_argument('--tpwr', action='store_true', help='enable target power')
parser.add_argument('--serial', help='choose specific probe by serial number')
parser.add_argument('--port', help='choose specific probe by port')
parser.add_argument('--attach', help='choose specific target by number', default='1')
parser.add_argument('--gdb-path', help='path to GDB', default='gdb-multiarch')
parser.add_argument('--term-cmd', help='serial terminal command',
                    default='picocom --nolock --imap lfcrlf --baud 115200 %s')
parser.add_argument('action', help='choose a task to perform', nargs='?',
                    choices=['list', 'flash', 'erase', 'debug', 'term', 'reset'],
                    default='list')
parser.add_argument('file', help='file to load to target (hex or elf)', nargs='?')

TIMEOUT = 100  # seconds


# find a suitable gdb executable, falling back to defaults if needed
def find_suitable_gdb(gdb_path):
    if distutils.spawn.find_executable(gdb_path):
        return gdb_path
    else:
        for p in ['arm-none-eabi-gdb', 'gdb-multiarch']:
            p = distutils.spawn.find_executable(p)
            if p:
                print("GDB EXECUTABLE NOT FOUND! FALLING BACK TO %s" % p, file=sys.stderr)
                return p
    print("CANNOT LOCATE SUITABLE GDB EXECUTABLE!", file=sys.stderr)
    sys.exit(-1)


# find all connected BMPs and store both GDB and UART interfaces
def detect_probes():
    gdb_ports = []
    uart_ports = []
    for p in serial.tools.list_ports.comports():
        if p.vid == 0x1D50 and p.pid in {0x6018, 0x6017}:
            if re.fullmatch(r'COM\d\d', p.device):
                p.device = '//./' + p.device
            if 'GDB' in str(p.interface) \
                    or re.fullmatch(r'/dev/cu\.usbmodem([A-F0-9]*)1', p.device) \
                    or p.location[-1] == '0' and os.name == 'nt':
                gdb_ports.append(p)
            else:
                uart_ports.append(p)
    return gdb_ports, uart_ports


# search device with specific serial number <snr> in list <l>
def search_serial(snr, l):
    for p in l:
        if snr in p.serial_number:
            return p.device


# parse GDB output for targets
def detect_targets(gdbmi, res):
    targets = []
    while True:
        for msg in res:
            if msg['type'] == 'target':
                m = re.fullmatch(pattern=r"\s*(\d)+\s*(.*)\\n", string=msg['payload'])
                if m:
                    targets.append(m.group(2))
            elif msg['type'] == 'result':
                assert msg['message'] == 'done', str(msg)
                return targets

        res = gdbmi.get_gdb_response(timeout_sec=TIMEOUT)


def gdb_write_and_wait_for_result(gdbmi, cmd, description, expected_result='done'):
    res = gdbmi.write(cmd, timeout_sec=TIMEOUT)
    while True:
        for msg in res:
            if msg['type'] == 'result':
                if msg['message'] == expected_result:
                    print(description, "successful.")
                    return True
                else:
                    print(description, "failed.", file=sys.stderr)
                    return False
        res = gdbmi.get_gdb_response(timeout_sec=TIMEOUT)


def parse_download_msg(msg):
    m = re.fullmatch(
        pattern=r"\+download,"
                r"\{(?:section=\"(.*?)\")?,?(?:section-sent=\"(.*?)\")?,?"
                r"(?:section-size=\"(.*?)\")?,?(?:total-sent=\"(.*?)\")?,?"
                r"(?:total-size=\"(.*?)\")?,?\}",
        string=msg['payload'])
    if m:
        section_name = m.group(1)
        section_sent = int(m.group(2)) if m.group(2) else None
        section_size = int(m.group(3)) if m.group(3) else None
        total_sent = int(m.group(4)) if m.group(4) else None
        total_size = int(m.group(5)) if m.group(5) else None
        return section_name, section_sent, section_size, total_sent, total_size


def download_to_flash(gdbmi):
    res = gdbmi.write('-target-download', timeout_sec=TIMEOUT)
    first = True  # whether this is the first status message
    current_sec = None  # name of current section
    pbar = ProgressBar()
    while True:
        for msg in res:
            if msg['type'] == 'result':
                assert msg['message'] == 'done', "download failed: %s" % str(msg)
                if pbar.start_time:
                    pbar.finish()
                print("downloading finished")
                return
            elif msg['type'] == 'output':
                section_name, section_sent, section_size, total_sent, total_size = parse_download_msg(msg)
                if section_name:
                    if first:
                        first = False
                        print("downloading... total size: %s"
                              % humanize.naturalsize(total_size, gnu=True))
                    if section_name != current_sec:
                        if pbar.start_time:
                            pbar.finish()
                        current_sec = section_name
                        print("downloading section [%s] (%s)" % (
                            section_name, humanize.naturalsize(section_size, gnu=True)))
                        pbar = ProgressBar(widgets=[Percentage(), Bar()], maxval=section_size).start()
                    if section_sent:
                        pbar.update(section_sent)
        res = gdbmi.get_gdb_response(timeout_sec=TIMEOUT)


def check_flash(gdbmi):
    res = gdbmi.write('compare-sections', timeout_sec=TIMEOUT)
    while True:
        for msg in res:
            if msg['type'] == 'result':
                assert msg['message'] == 'done', "checking failed: %s" % str(msg)
                print("checking successful")
                return
            elif msg['type'] == 'console':
                assert 'matched' in msg['payload'] and 'MIS-MATCHED' not in msg['payload'], \
                    "checking failed: %s" % str(msg)
        res = gdbmi.get_gdb_response(timeout_sec=TIMEOUT)


def choose_bmp_port(gdb_ports):
    print("found following Black Magic GDB servers:")
    for i, s in enumerate(gdb_ports):
        print("\t[%s]" % s.device, end=' ')
        if len(s.serial_number) > 1:
            print("Serial:", s.serial_number, end=' ')
        if i == 0:
            print("<- default", end=' ')
        print('')
    port = gdb_ports[0].device
    if args.port:
        port = args.port
    elif args.serial:
        port = search_serial(args.serial, gdb_ports)
        assert port, "no BMP with this serial found"
    print('connecting to [%s]...' % port)
    return port


# terminal mode, opens TTY program
def term_mode(uart_ports):
    port = uart_ports[0].device
    if args.port:
        port = args.port
    elif args.serial:
        port = search_serial(args.serial, uart_ports)
        assert port, "no BMP with this serial found"
    os.system(args.term_cmd % port)
    sys.exit(0)


# debug mode, opens GDB shell with options
def debug_mode(port):
    gdb_args = ['-ex \'target extended-remote %s\'' % port]
    if args.tpwr:
        gdb_args.append('-ex \'monitor tpwr enable\'')
    if args.connect_srst:
        gdb_args.append('-ex \'monitor connect_srst enable\'')
    if args.jtag:
        gdb_args.append('-ex \'monitor jtag_scan\'')
    else:
        gdb_args.append('-ex \'monitor swdp_scan\'')
    gdb_args.append('-ex \'attach %s\'' % args.attach)
    os.system(" ".join(['\"' + args.gdb_path + '\"'] + gdb_args + [args.file]))


def connect_to_target(port):
    # open GDB in machine interface mode
    try:
        # try old API first
        gdbmi = GdbController(gdb_path=args.gdb_path, gdb_args=["--nx", "--quiet", "--interpreter=mi2", args.file])
    except TypeError:
        # and then new API
        gdbmi = GdbController(command=[args.gdb_path, "--nx", "--quiet", "--interpreter=mi2", args.file])
    assert gdb_write_and_wait_for_result(gdbmi, '-target-select extended-remote %s' % port, 'connecting',
                                         expected_result='connected')
    # set options
    if args.connect_srst:
        gdbmi.write('monitor connect_srst enable', timeout_sec=TIMEOUT)
    if args.tpwr:
        gdbmi.write('monitor tpwr enable', timeout_sec=TIMEOUT)
    # scan for targets
    if not args.jtag:
        print("scanning using SWD...")
        res = gdbmi.write('monitor swdp_scan', timeout_sec=TIMEOUT)
    else:
        print("scanning using JTAG...")
        res = gdbmi.write('monitor jtag_scan', timeout_sec=TIMEOUT)
    targets = detect_targets(gdbmi, res)
    assert len(targets) > 0, "no targets found"
    print("found following targets:")
    for t in targets:
        print("\t%s" % t)
    print("")
    return gdbmi


if __name__ == '__main__':
    args = parser.parse_args()
    assert not (args.swd and args.jtag), "you may only choose one protocol"
    assert not (args.serial and args.port), "you may only specify the probe by port or by serial"
    g, u = detect_probes()
    assert len(g) > 0, "no Black Magic Probes found 😔"

    if args.action == 'term':
        term_mode(u)
    else:
        port = choose_bmp_port(g)

        args.file = args.file if args.file else ''
        args.gdb_path = find_suitable_gdb(args.gdb_path)

        if args.action == 'debug':
            debug_mode(port)
            sys.exit(0)

        gdbmi = connect_to_target(port)

        if args.action == 'list':
            sys.exit(0)

        assert gdb_write_and_wait_for_result(gdbmi, '-target-attach %s' % args.attach, 'attaching to target')

        # reset mode: reset device using reset pin
        if args.action == 'reset':
            assert gdb_write_and_wait_for_result(gdbmi, 'monitor hard_srst', 'resetting target')
            sys.exit(0)
        # erase mode
        elif args.action == 'erase':
            print('erasing...')
            assert gdb_write_and_wait_for_result(gdbmi, '-target-flash-erase', 'erasing target')
            sys.exit(0)
        # flashloader mode: flash, check and restart
        elif args.action == 'flash':
            download_to_flash(gdbmi)
            check_flash(gdbmi)

            # kill and reset
            assert gdb_write_and_wait_for_result(gdbmi, 'kill', 'killing')
