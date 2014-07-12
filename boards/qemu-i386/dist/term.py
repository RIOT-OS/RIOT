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
import readline
import socket
import signal
import subprocess
import sys
import termios
import threading

from datetime import datetime
from distutils.util import split_quoted
from time import time


def get_timestamp():
    return datetime.fromtimestamp(time()).strftime('%Y-%m-%d %H:%M:%S.%f')

def popen(args, **kw):
    null = open(os.devnull, 'wb', 0)
    return subprocess.Popen(args, stdin=null, stdout=null, stderr=null, **kw)

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
            print('{}: {}'.format(get_timestamp(), line.decode('UTF-8', 'replace').rstrip("\r\n")))

    if isinstance(QEMU, str):
        QEMU = split_quoted(QEMU)
    if DEBUGGER and isinstance(DEBUGGER, str):
        DEBUGGER = split_quoted(DEBUGGER)

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
        if DEBUGGER:
            args += ['-s', '-S']

        try:
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
                print('Interrupted ...')
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
    print("Type 'exit' to exit.")

    atexit.register(termios.tcsetattr, 0, termios.TCSAFLUSH, termios.tcgetattr(0))

    sys.exit(main(*sys.argv[1:]))
