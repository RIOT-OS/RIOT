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

import re
from itertools import groupby
from os import devnull, environ, listdir
from os.path import abspath, dirname, isfile, join
from subprocess import CalledProcessError, check_call, check_output, PIPE, Popen
from sys import exit, stdout, argv

riotbase = environ.get('RIOTBASE') or abspath(join(dirname(abspath(__file__)), '../' * 3))

if len(argv) > 1:
    base_branch = argv[1]
    diff_files = check_output(('git', 'diff', '--name-only', base_branch, 'HEAD'))
    diff_files = set(diff_files.split())
else:
    base_branch = ''

null = open(devnull, 'w', 0)

success = []
failed = []
skipped = []
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
        if (len(result) == 2) or (len(result) == 3 and 'retrying' in result[1]):
            stdout.write('.')
            stdout.flush()
            yield (' .. '.join(result[:-1]), result[-1])

def _get_common_user(common):
    return [f for f in check_output(r'grep -l "{}" cpu/*/Makefile* boards/*/Makefile*'.format(common),
            shell=True).split() if "common" not in f]

def _get_boards_from_files(files):
    boards = set()
    if any("boards/" in s for s in files):
        for f in files:
            if "boards/" not in f:
                continue
            board = re.sub(r"^boards/([^/]+)/.*$", r"\1", f)

            if "common" in board:
                boards |= _get_boards_from_files(_get_common_user(board))
            else:
                boards |= { board }

    return boards

def _get_cpus_from_files(files):
    cpus = set()

    if any("cpu/" in s for s in files):
        for f in files:
            if "cpu/" not in f:
                continue

            cpu = re.sub(r"^cpu/([^/]+)/.*", r"\1", f)

            if "common" in cpu:
                cpus |= _get_cpus_from_files(_get_common_user(cpu))
            else:
                cpus |= { cpu }

    return cpus

def is_updated(application_folder, subprocess_env):
    try:
        if base_branch == '':
            return True

        if ".travis.yml" in diff_files or \
           any("dist/" in s for s in diff_files) or \
           any("core/" in s for s in diff_files) or \
           any("sys/" in s for s in diff_files):
            return True

        boards_changes = set()

        boards_changes |= _get_boards_from_files(diff_files)

        for cpu in _get_cpus_from_files(diff_files):
            board_files = check_output(r'grep -l "^\(export \)*CPU[ :?=]\+{}" boards/*/Makefile.include'.format(cpu),
                                       shell=True).split()
            boards_changes |= _get_boards_from_files(board_files)

        if len(boards_changes) > 0:
            app_files = set()

            for board in boards_changes:
                env = { "BOARD": board }
                env.update(subprocess_env)
                tmp = check_output(('make', 'info-files'), stderr=null,
                                         cwd=application_folder, env=env)
                app_files |= set(tmp.split())

                if (len(diff_files & app_files) > 0):
                    return True
        else:
            app_files = check_output(('make', 'info-files'), stderr=null,
                                     cwd=application_folder, env=subprocess_env)
            app_files = set(app_files.split())

        return (len(diff_files & app_files) > 0)
    except CalledProcessError as e:
        return True

for folder in ('examples', 'tests'):
    print('Building all applications in: \033[1;34m{}\033[0m'.format(folder))

    applications = listdir(join(riotbase, folder))
    applications = filter(lambda app: is_tracked(join(riotbase, folder, app)), applications)
    applications = sorted(applications)

    subprocess_env = environ.copy()
    subprocess_env['RIOT_DO_RETRY'] = '1'

    for nth, application in enumerate(applications, 1):
        stdout.write('\tBuilding application: \033[1;34m{}\033[0m ({}/{}) '.format(application, nth, len(applications)))
        stdout.flush()
        try:
            if not is_updated(join(riotbase, folder, application), subprocess_env):
                print("\033[1;33m(skipped)\033[0m")
                skipped.append(application)
                continue
            subprocess = Popen(('make', 'buildtest'),
                               bufsize=1, stdin=null, stdout=PIPE, stderr=null,
                               cwd=join(riotbase, folder, application),
                               env=subprocess_env)

            lines = get_lines(subprocess.stdout.readline, 'Building for ')
            lines = groupby(sorted(lines), lambda (outcome, board): outcome)

            print()
            for group, results in lines:
                print('\t\t{}: {}'.format(group, ', '.join(sorted(board for outcome, board in results))))

            returncode = subprocess.wait()
            if returncode == 0:
                success.append(application)
            else:
                failed.append(application)
                subprocess_env.pop('RIOT_DO_RETRY', None)
        except Exception, e:
            print('\n\t\tException: {}'.format(e))
            exceptions.append(application)
        finally:
            try:
                subprocess.kill()
            except:
                pass

print('Outcome:')
for color, group in (('3', 'skipped'), ('2', 'success'), ('1', 'failed'), ('4', 'exceptions')):
    applications = locals()[group]
    if applications:
        print('\t\033[1;3{}m{}\033[0m: {}'.format(color, group, ', '.join(applications)))

if exceptions:
    exit(2)
elif failed:
    exit(1)
else:
    exit(0)
