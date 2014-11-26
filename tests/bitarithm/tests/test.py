#! /usr/bin/env python

# Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

from __future__ import print_function

from contextlib import contextmanager
from os import devnull, getpid, killpg, setsid
from re import compile
from signal import alarm, signal, SIG_IGN, SIGALRM, SIGKILL, SIGTERM
from sys import exit, stderr
from subprocess import Popen, PIPE


def msb(val):
    return len('{:b}'.format(val)) - 1

def lsb(val):
    return '{:b}'.format(val)[::-1].index('1')

match_line = compile(r'(\d+) msb=(\d+) lsb=(\d+)').match


@contextmanager
def on_timeout(fun, duration_seconds):
    signal(SIGALRM, lambda signum, frame: fun)
    alarm(duration_seconds)
    try:
        yield
    except BaseException:
        alarm(0)
        signal(getpid(), SIGALRM)
        raise
    finally:
        alarm(0)
        signal(SIGALRM, SIG_IGN)


@contextmanager
def start_riot_process():
    def kill_riot_processgroup():
        if riot_process.poll() is None:
            print('RIOT term process did not end gracefully, terminating.')
            killpg(riot_process.pid, SIGTERM)

    riot_process = Popen(['make', 'term'],
                         stdin=open(devnull, 'wb'),
                         stdout=PIPE,
                         stderr=stderr,
                         shell=False,
                         preexec_fn=setsid)
    try:
        yield riot_process
    finally:
        if riot_process.poll() is None:
            with on_timeout(kill_riot_processgroup, 10):
                killpg(riot_process.pid, SIGKILL)
                riot_process.wait()


def test(read_riot_line):
    errored = 0

    test_started = False
    while True:
        line = read_riot_line().decode('utf8').strip()

        if not test_started:
            if line == '.Start.':
                test_started = True
        elif line == '.Done.':
            break

        m = match_line(line)
        if not m:
            continue

        i, read_msb, read_lsb = (int(g) for g in m.groups())
        real_msb, real_lsb = msb(i), lsb(i)
        if read_msb != real_msb or read_lsb != read_lsb:
            if errored < 10:
                s = '{}: msb(read/real): {}={}, lsb(read/real): {}={}'.format(
                        i, read_msb, real_msb, read_lsb, read_lsb)
                print(s)
            elif errored == 10:
                print('(Not printing further errors)')
            errored += 1
    else:
        errored += 1
        if test_started:
            print('Test finished without printing ".Start."')
        else:
            print('Test finished without printing ".Done."')

    return errored == 0


if __name__ == '__main__':
    with start_riot_process() as riot_process:
        result_okay = test(riot_process.stdout.readline)
        if not result_okay:
            print('TEST HAD ERRORS')
        else:
            print('Test succeded')
        exit(0 if result_okay else 42)
