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
from os.path import abspath, dirname, isfile, join
from subprocess import CalledProcessError, check_call, PIPE, Popen
from sys import exit, stdout

null = open(devnull, 'w', 0)

success = []
failed = []
exceptions = []

def is_tracked(application_folder):
    if not isfile(join(application_folder, 'Makefile')):
        return False
    try:
        check_call(('git', 'ls-files', '--error-unmatch', 'Makefile'),
                   stdin=null, stdout=null, stderr=null, cwd=application_folder)
    except CalledProcessError:
        return False
    else:
        return True

def get_lines(readline, prefix):
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

def main(riotbase):
    for folder in ('examples', 'tests'):
        print('Building all applications in: \033[1;34m{}\033[0m'.format(folder))

        applications = listdir(join(riotbase, folder))
        applications = filter(lambda app: is_tracked(join(riotbase, folder, app)), applications)
        applications = sorted(applications)

        for nth, application in enumerate(applications, 1):
            stdout.write('\tBuilding application: \033[1;34m{}\033[0m ({}/{}) '.format(application, nth, len(applications)))
            stdout.flush()
            try:
                subprocess = Popen(('make', 'buildtest'),
                                   bufsize=1, stdin=null, stdout=PIPE, stderr=null,
                                   cwd=join(riotbase, folder, application))

                lines = get_lines(subprocess.stdout.readline, 'Building for ')
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
    for color, group in (('2', 'success'), ('1', 'failed'), ('4', 'exceptions')):
        applications = locals()[group]
        if applications:
            print('\t\033[1;3{}m{}\033[0m: {}'.format(color, group, ', '.join(applications)))

    if exceptions:
        return 2
    elif failed:
        return 1
    else:
        return 0

if __name__ == '__main__':
    riotbase = environ.get('RIOTBASE') or abspath(join(dirname(abspath(__file__)), '../' * 3))
    exit(main(riotbase))
