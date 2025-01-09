#!/usr/bin/env python3

# Copyright (C) 2019 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author   Maximilian Deubel <maximilian.deubel@ovgu.de>
# @author   Bas Stottelaar <basstottelaar@gmail.com>

# Black Magic Probe helper script
# This script can detect connected Black Magic Probes and can be used as a flashloader and much more

import argparse
import os
import re
import shutil
import sys

import humanize
import serial.tools.list_ports
from packaging.version import Version
from progressbar import Bar, Percentage, ProgressBar
from pygdbmi.gdbcontroller import GdbController

TIMEOUT = 100  # seconds


# find a suitable gdb executable, falling back to defaults if needed
def find_suitable_gdb(gdb_path):
    if shutil.which(gdb_path):
        return gdb_path
    else:
        for p in ['arm-none-eabi-gdb', 'gdb-multiarch']:
            p = shutil.which(p)
            if p:
                print(f"GDB EXECUTABLE NOT FOUND! FALLING BACK TO {p}", file=sys.stderr)
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


# print all found ports to console.
def enumerate_probes(ports):
    print("found following Black Magic GDB servers:")
    for i, s in enumerate(ports):
        print(f"\t[{s.device}]", end=' ')
        if len(s.serial_number) > 1:
            print(f"Serial: {s.serial_number}", end=' ')
        if i == 0:
            print("<- default", end=' ')
        print('')


# search device with specific serial number <snr> in a list of ports <ports>
def search_serial(snr, ports):
    for port in ports:
        if snr in port.serial_number:
            return port.device


# parse GDB output for targets
def detect_targets(gdbmi, res):
    targets = []
    while True:
        for msg in res:
            if msg['type'] == 'target':
                m = re.fullmatch(pattern=r"\s*(\d+)\s*(.*)\s*", string=msg['payload'])
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
                assert msg['message'] == 'done', f"download failed: {msg}"
                if pbar.start_time:
                    pbar.finish()
                print("downloading finished")
                return
            elif msg['type'] == 'output':
                section_name, section_sent, section_size, total_sent, total_size = parse_download_msg(msg)
                if section_name:
                    if first:
                        first = False
                        print(f"downloading... total size: {humanize.naturalsize(total_size, gnu=True)}")
                    if section_name != current_sec:
                        if pbar.start_time:
                            pbar.finish()
                        current_sec = section_name
                        print(f"downloading section [{section_name}] ({humanize.naturalsize(section_size, gnu=True)})")
                        pbar = ProgressBar(widgets=[Percentage(), Bar()], maxval=section_size).start()
                    if section_sent:
                        pbar.update(section_sent)
        res = gdbmi.get_gdb_response(timeout_sec=TIMEOUT)


def check_flash(gdbmi):
    res = gdbmi.write('compare-sections', timeout_sec=TIMEOUT)
    while True:
        for msg in res:
            if msg['type'] == 'result':
                assert msg['message'] == 'done', f"checking failed: {msg}"
                print("checking successful")
                return
            elif msg['type'] == 'console':
                assert 'matched' in msg['payload'] and 'MIS-MATCHED' not in msg['payload'], \
                    f"checking failed: {msg}"
        res = gdbmi.get_gdb_response(timeout_sec=TIMEOUT)


# choose GDB or UART port, based on available ports and application arguments
def choose_port(args, ports):
    if args.port:
        port = args.port
    else:
        enumerate_probes(ports)
        if args.serial:
            port = search_serial(args.serial, ports)
            assert port, "no BMP with this serial found"
        else:
            assert len(ports) > 0, "no ports found"
            port = ports[0].device
    print(f'connecting to [{port}]...')
    return port


# terminal mode, opens TTY program
def term_mode(args, uart_port):
    os.system(args.term_cmd % uart_port)
    sys.exit(0)


# debug mode, opens GDB shell with options
def debug_mode(args, port):
    gdb_args = [f'-ex \'target extended-remote {port}\'']
    if args.tpwr:
        gdb_args.append('-ex \'monitor tpwr enable\'')
    if args.connect_srst:
        if args.bmp_version >= Version('1.9.0'):
            gdb_args.append('-ex \'monitor connect_rst enable\'')
        else:
            gdb_args.append('-ex \'monitor connect_srst enable\'')
    if args.jtag:
        gdb_args.append('-ex \'monitor jtag_scan\'')
    else:
        if args.bmp_version >= Version('1.10.0'):
            gdb_args.append('-ex \'monitor swd_scan\'')
        else:
            gdb_args.append('-ex \'monitor swdp_scan\'')
    gdb_args.append(f'-ex \'attach {args.attach}\'')
    os.system(" ".join([f'\"{args.gdb_path}\"'] + gdb_args + [args.file]))


def connect_to_target(args, port):
    # open GDB in machine interface mode
    try:
        # try old API first
        gdbmi = GdbController(gdb_path=args.gdb_path, gdb_args=["--nx", "--quiet", "--interpreter=mi2", args.file])
    except TypeError:
        # and then new API
        gdbmi = GdbController(command=[args.gdb_path, "--nx", "--quiet", "--interpreter=mi2", args.file])
    assert gdb_write_and_wait_for_result(gdbmi, f'-target-select extended-remote {port}', 'connecting',
                                         expected_result='connected')
    # set options
    if args.connect_srst:
        if args.bmp_version >= Version('1.9.0'):
            gdbmi.write('monitor connect_rst enable', timeout_sec=TIMEOUT)
        else:
            gdbmi.write('monitor connect_srst enable', timeout_sec=TIMEOUT)
    if args.tpwr:
        gdbmi.write('monitor tpwr enable', timeout_sec=TIMEOUT)
    # scan for targets
    if not args.jtag:
        print("scanning using SWD...")
        if args.bmp_version >= Version('1.10.0'):
            res = gdbmi.write('monitor swd_scan', timeout_sec=TIMEOUT)
        else:
            res = gdbmi.write('monitor swdp_scan', timeout_sec=TIMEOUT)
    else:
        print("scanning using JTAG...")
        res = gdbmi.write('monitor jtag_scan', timeout_sec=TIMEOUT)
    targets = detect_targets(gdbmi, res)
    assert len(targets) > 0, "no targets found"
    print("found following targets:")
    for t in targets:
        print(f"\t{t}")
    print("")
    return gdbmi


def parse_args():
    parser = argparse.ArgumentParser(description='Black Magic Tool helper script.')

    parser.add_argument('--jtag', action='store_true', help='use JTAG transport')
    parser.add_argument('--swd', action='store_true', help='use SWD transport (default)')
    parser.add_argument('--connect-srst', action='store_true', help='reset target while connecting')
    parser.add_argument('--tpwr', action='store_true', help='enable target power')
    parser.add_argument('--serial', help='choose specific probe by serial number')
    parser.add_argument('--port', help='choose specific probe by port (overrides auto selection)')
    parser.add_argument('--attach', help='choose specific target by number', type=int, default=1)
    parser.add_argument('--gdb-path', help='path to GDB', default='gdb-multiarch')
    parser.add_argument('--bmp-version', help='choose specific firmware version', default='1.10.0')
    parser.add_argument('--term-cmd', help='serial terminal command',
                        default='picocom --nolock --imap lfcrlf --baud 115200 %s')

    parser.add_argument('action', help='choose a task to perform', nargs='?',
                        choices=['list', 'flash', 'erase', 'debug', 'term', 'reset'],
                        default='list')
    parser.add_argument('file', help='file to load to target (hex or elf)', nargs='?')

    return parser.parse_args()


def main():
    args = parse_args()
    assert not (args.swd and args.jtag), "you may only choose one protocol"
    assert not (args.serial and args.port), "you may only specify the probe by port or by serial"
    g, u = detect_probes()

    if args.action == 'term':
        port = choose_port(args, u)

        term_mode(args, port)
    else:
        port = choose_port(args, g)

        args.file = args.file if args.file else ''
        args.bmp_version = Version(args.bmp_version)
        args.gdb_path = find_suitable_gdb(args.gdb_path)

        if args.action == 'debug':
            debug_mode(args, port)
            sys.exit(0)

        gdbmi = connect_to_target(args, port)

        if args.action == 'list':
            sys.exit(0)

        assert gdb_write_and_wait_for_result(gdbmi, f'-target-attach {args.attach}', 'attaching to target')

        # reset mode: reset device using reset pin
        if args.action == 'reset':
            print('resetting...')
            if args.bmp_version >= Version('1.9.0'):
                assert gdb_write_and_wait_for_result(gdbmi, 'monitor reset', 'resetting target')
            else:
                assert gdb_write_and_wait_for_result(gdbmi, 'monitor hard_srst', 'resetting target')
            sys.exit(0)
        # erase mode
        elif args.action == 'erase':
            print('erasing...')
            assert gdb_write_and_wait_for_result(gdbmi, '-target-flash-erase', 'erasing target')
            sys.exit(0)
        # flashloader mode: flash, check and restart
        elif args.action == 'flash':
            print('flashing...')
            download_to_flash(gdbmi)
            check_flash(gdbmi)

            # kill and reset
            assert gdb_write_and_wait_for_result(gdbmi, 'kill', 'killing')


if __name__ == '__main__':
    main()
