#!/usr/bin/env python2
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

from __future__ import print_function

from itertools import groupby, ifilter, imap
from os import devnull, environ, listdir
from os.path import abspath, dirname, isdir, join
from subprocess import PIPE, Popen
from sys import exit

riotbase = environ.get('RIOTBASE') or abspath(join(dirname(abspath(__file__)), '../' * 3))

null = open(devnull, 'w')

success = []
failed = []
exceptions = []

for folder in ('examples', 'tests'):
    print('Building all applications in: \033[1;34m{}\033[0m'.format(folder))
    for application in sorted(listdir(join(riotbase, folder))):
        if not isdir(join(riotbase, folder, application)):
            continue

        print('\tBuilding application: \033[1;34m{}\033[0m'.format(application))
        try:
            subprocess = Popen(('make', 'buildtest'),
                               stdout=PIPE,
                               stderr=null,
                               cwd=join(riotbase, folder, application))
            lines = imap(str.rstrip, subprocess.stdout)
            lines = ifilter(bool, lines)
            lines = imap(lambda s: s.split(' .. '), lines)
            lines = ifilter(lambda t: len(t) == 2, lines)
            lines = imap(lambda (board, outcome): (outcome, board[len('Building for '):]), lines)
            lines = groupby(sorted(lines), lambda (outcome, board): outcome)
            for group, results in lines:
                print('\t\t{}: {}'.format(group, ', '.join(sorted(board for outcome, board in results))))

            returncode = subprocess.wait()
            (failed if returncode else success).append(application)
        except Exception, e:
            print('\t\tException: {}'.format(e))
            exceptions.append(application)
        finally:
            try:
                subprocess.kill()
            except:
                pass

print('Outcome:')
for color, group, applications in (('2', 'success', success), ('1', 'failed', failed), ('4', 'exceptions', exceptions)):
    if applications:
        print('\t\033[1;3{}m{}\033[0m: {}'.format(color, group, ', '.join(applications)))

if exceptions:
    exit(2)
elif failed:
    exit(1)
else:
    exit(0)
