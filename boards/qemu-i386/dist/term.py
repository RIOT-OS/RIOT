#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import atexit
import os
import re
import readline
import socket
import signal
import subprocess
import sys
import termios
import threading

from datetime import datetime
from shlex import split
from time import time

try:
    from shlex import quote
except ImportError:
    from pipes import quote

def join_quoted(args):
    return ' '.join(quote(arg) for arg in args)


_null = open(os.devnull, 'wb', 0)

def get_timestamp():
    return datetime.fromtimestamp(time()).strftime('%Y-%m-%d %H:%M:%S.%f')

def popen(args, **kw):
    return subprocess.Popen(args, stdin=_null, stdout=_null, stderr=_null, **kw)

def main(QEMU, BINDIRBASE, HEXFILE, DEBUGGER=None):
    def run_shell():
        nonlocal result
        while 1:
            try:
                line = input()
                if line == 'quit':
                    result = 0
                    raise EOFError()
            except EOFError:
                qemu.kill()
                return
            client_file.write((line + '\r\n').encode('UTF-8'))

    def read_terminal():
        for line in client_file:
            sys.stderr.write(get_timestamp() + ': ')
            sys.stderr.flush()
            sys.stdout.write(line.decode('UTF-8', 'replace').rstrip('\r\n') + '\n')
            sys.stdout.flush()

    if isinstance(QEMU, str):
        QEMU = split(QEMU)
    if DEBUGGER and isinstance(DEBUGGER, str):
        DEBUGGER = split(DEBUGGER)

    qemu_version = subprocess.check_output(QEMU + ['-version'], stdin=_null, stderr=_null)
    qemu_version = re.match(br'.*version ((?:\d+\.?)+).*', qemu_version).group(1).split(b'.')
    qemu_version = list(int(v) for v in qemu_version)

    histfile = os.path.join(BINDIRBASE, '.qemu-term.hist')
    try:
        readline.read_history_file(histfile)
    except IOError:
        pass

    result = 1
    try:
        sock = socket.socket()
        sock.settimeout(60)
        sock.bind(('', 0))
        sock.listen(1)
        host, port = sock.getsockname()

        args = QEMU + [
            '-serial', 'tcp:{}:{}'.format(host, port),
            '-nographic',
            '-monitor', '/dev/null',
            '-kernel', HEXFILE,
        ]
        if qemu_version >= [2, 1]:
            args += ['-m', 'size=512']
        else:
            args += ['-m', '512m']
        if DEBUGGER:
            args += ['-s', '-S']

        try:
            sys.stderr.write('Starting QEMU: {}\n\n'.format(join_quoted(args)))
            qemu = popen(args)

            if DEBUGGER:
                ignore_sig_int = lambda: signal.signal(signal.SIGINT, signal.SIG_IGN)
                popen(DEBUGGER, preexec_fn=ignore_sig_int)

            client_sock, _ = sock.accept()
            client_file = client_sock.makefile('rwb', 1)
            sock.close()

            threading.Thread(target=run_shell, daemon=True).start()
            threading.Thread(target=read_terminal, daemon=True).start()

            try:
                result = qemu.wait() and result
            except KeyboardInterrupt:
                sys.stderr.write('\nInterrupted ...\n')
                result = 0
        finally:
            try:
                qemu.kill()
            except:
                pass
    finally:
        try:
            readline.write_history_file(histfile)
        except:
            pass

        try:
            client_sock.close()
        except:
            pass
    return result


if __name__ == '__main__':
    sys.stderr.write("Type 'exit' to exit.\n")

    atexit.register(termios.tcsetattr, 0, termios.TCSAFLUSH, termios.tcgetattr(0))

    sys.exit(main(*sys.argv[1:]))
