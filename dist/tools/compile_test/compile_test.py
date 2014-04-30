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

from itertools import groupby
from os import devnull, environ, listdir
from os.path import abspath, dirname, isdir, join
from subprocess import PIPE, Popen
from sys import exit, stdout

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

        stdout.write('\tBuilding application: \033[1;34m{}\033[0m '.format(application))
        stdout.flush()
        try:
            subprocess = Popen(('make', 'buildtest'),
                               bufsize=1,
                               stdin=null,
                               stdout=PIPE,
                               stderr=null,
                               cwd=join(riotbase, folder, application))

            def lines(readline, prefix):
                while 1:
                    result = readline()
                    if not result:
                        break
                    elif not result.startswith(prefix):
                        continue

                    result = result[len(prefix):].rstrip().split(' .. ')[::-1]
                    if len(result) == 2:
                        stdout.write('.')
                        stdout.flush()
                        yield result

            lines = lines(subprocess.stdout.readline, 'Building for ')
            lines = groupby(sorted(lines), lambda (outcome, board): outcome)
            for group, results in lines:
                print('\n\t\t{}: {}'.format(group, ', '.join(sorted(board for outcome, board in results))))

            returncode = subprocess.wait()
            (failed if returncode else success).append(application)
        except Exception, e:
            print('\n\t\tException: {}'.format(e))
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
