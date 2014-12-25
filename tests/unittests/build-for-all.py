#!/usr/bin/env python3
# -*- coding: utf-8 -*-

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

from curses import setupterm, tigetnum
from glob import iglob
from os import chdir, devnull, environ
from os.path import basename, dirname, join
from subprocess import check_output, PIPE, Popen
from sys import exit, stderr, stdout
from time import time
from concurrent.futures import ThreadPoolExecutor


_null = open(devnull, 'wb', 0)


def build(test, board):
    start = time()

    make_process = Popen(['make', 'BOARD=' + board, test, '-j2'],
                         stdin=_null, stdout=_null, stderr=PIPE)
    make_out, make_err = make_process.communicate()
    retcode = make_process.poll()

    duration = time() - start
    make_err = make_err.decode('UTF-8').rstrip()
    return board, retcode, make_err, duration


def build_for_all(colored=False, map=map):
    errored = []

    # http://blog.mathieu-leplatre.info/colored-output-in-console-with-python.html
    if colored:
        BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE = ['\x1b[1;{}m'.format(30+c) for c in range(8)]
        RESET = '\x1b[0m'
    else:
        BLACK, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE, RESET = [''] * 9

    for test in sorted(basename(dirname(d)) for d in iglob(join('tests-*', 'Makefile'))):
        boards = check_output(['make', 'info-boards-supported', 'UNIT_TESTS=' + test],
                              stdin=_null, stderr=_null).decode('UTF-8').split()
        for board, retcode, make_err, duration in map(lambda board: build(test, board), boards):
            print('Built {neutral}{test}{reset} '
                  'for {neutral}{board}{reset} .. '
                  '{color}{outcome}{reset} in '
                  '{duration:.3f}s'.format(neutral=MAGENTA,
                                           test=test[6:],
                                           board=board,
                                           reset=RESET,
                                           color=GREEN if retcode == 0 else RED,
                                           outcome='success' if retcode == 0 else 'fail',
                                           duration=duration))
            if retcode:
                errored.append((board, test))
                print(make_err, file=stderr)

    if errored:
        print('{}Errored: {}{}'.format(RED, RESET, ', '.join('{} on {}'.format(test[6:], board)
                                                             for board, test in errored)))
    else:
        print('{}Successful{}'.format(GREEN, RESET))

    return errored


if __name__ == '__main__':
    chdir(dirname(__file__))

    if 'TRAVIS' in environ:
        colored = True
    else:
        try:
            setupterm()
            colored = tigetnum('colors') >= 16
        except:
            colored = False

    with ThreadPoolExecutor(max_workers=8) as executor:
        errored = build_for_all(colored, map=executor.map)
    exit(0 if not errored else 1)
