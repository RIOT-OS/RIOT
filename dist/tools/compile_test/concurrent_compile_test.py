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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

from concurrent.futures import ThreadPoolExecutor
from itertools import groupby
from os import devnull, environ, listdir
from os.path import abspath, dirname, isfile, join
from subprocess import CalledProcessError, check_call, PIPE, Popen
from sys import argv, exit, stdout

null = open(devnull, 'wb', 0)

success = []
failed = []

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

def get_lines(application, readline, prefix):
    while 1:
        result = readline()
        if not result:
            break

        result = result.decode('UTF-8').rstrip()
        if not result.startswith(prefix):
            continue

        stdout.write('.')
        stdout.flush()

        result = result[len(prefix):].split(' .. ')[::-1]
        if len(result) == 2:
            yield result

def format_line(group_results):
    group, results = group_results
    return '\t\t{}: {}'.format(group,
                                 ', '.join(sorted(board for outcome, board in results)))

def compile_app(riotbase, folder, application):
    try:
        subprocess = Popen(('make', 'buildtest'),
                           bufsize=1, stdin=null, stdout=PIPE, stderr=null,
                           cwd=join(riotbase, folder, application))

        lines = get_lines(application, subprocess.stdout.readline, 'Building for ')
        lines = groupby(sorted(lines), lambda outcome_board: outcome_board[0])
        lines = map(format_line, lines)

        return subprocess.wait(), '\n'.join(lines)
    except Exception as e:
        return ('exception', e)
    finally:
        try:
            subprocess.kill()
        except:
            pass

def main(riotbase, concurrent_apps):
    for folder in ('examples', 'tests'):
        print('Compiling {}:'.format(folder))
        applications = listdir(join(riotbase, folder))
        applications = filter(lambda app: is_tracked(join(riotbase, folder, app)), applications)
        applications = sorted(applications)
        compiler = lambda application: compile_app(riotbase, folder, application)
        with ThreadPoolExecutor(max_workers=concurrent_apps) as executor:
            for application, result in zip(applications, executor.map(compiler, applications)):
                if result[0] == 'exception':
                    print(result)
                    raise result[1]
                (failed if result[0] else success).append((application, result[1]))
        print()

    for color, applications, text in ((2, success, 'success'), (1, failed, 'failure')):
        if applications:
            print('Outcome \033[1;3{}m{}\033[0m:'.format(color, text))
            for application, lines in applications:
                print('\t\033[1;3{}m{}\033[0m:\n{}'.format(color, application, lines))

    return 1 if failed else 0

if __name__ == '__main__':
    if len(argv) != 2:
        print('Usage: {} <ConcurrencyLevel>'.format(argv[0]))
        exit(3)

    riotbase = environ.get('riotbase') or abspath(join(dirname(abspath(__file__)), '../' * 3))
    concurrent_apps = int(argv[1])
    exit(main(riotbase, concurrent_apps))
