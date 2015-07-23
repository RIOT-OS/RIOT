#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
# Copyright (C) 2015  Philipp Rosenkranz  <philipp.rosenkranz@fu-berlin.de>
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
from sys import exit, stdout, argv, exc_info
from StringIO import StringIO
from itertools import tee

class Termcolor:
    red = '\033[1;31m'
    green = '\033[1;32m'
    yellow = '\033[1;33m'
    blue = '\033[1;34m'
    purple = '\033[1;35m'
    end = '\033[0m'

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

def get_results_and_output_from(fd):
    results_prefix = 'Building for '
    output_prefix = 'Building application '
    prev_results = False
    result = ['']
    output = StringIO()
    while 1:
        line = fd.readline()
        if not line:
            if prev_results:
                yield (' .. '.join(result[:-1]), result[-1], output)
            break
        elif line.startswith(results_prefix):
            read_more_output = False
            if prev_results:
                yield (' .. '.join(result[:-1]), result[-1], output)
            prev_results = True
            output = StringIO()
            result = line[len(results_prefix):].rstrip().split(' .. ')[::-1]
            if (len(result) > 1) and ('success' in result[0] or 'failed' in result[0]):
                stdout.write('.')
                stdout.flush()
        elif line.startswith(output_prefix):
            output.write(line)
            read_more_output = True
        elif read_more_output:
            output.write(line)

def _get_common_user(common):
    return [f for f in check_output(r'grep -l "{}" cpu/*/Makefile* boards/*/Makefile*'.format(common),
            shell=True).split() if 'common' not in f]

def _get_boards_from_files(files):
    boards = set()
    if any('boards/' in s for s in files):
        for f in files:
            if 'boards/' not in f:
                continue
            board = re.sub(r'^boards/([^/]+)/.*$', r'\1', f)

            if 'common' in board:
                boards |= _get_boards_from_files(_get_common_user(board))
            else:
                boards |= { board }

    return boards

def _get_cpus_from_files(files):
    cpus = set()

    if any('cpu/' in s for s in files):
        for f in files:
            if 'cpu/' not in f:
                continue

            cpu = re.sub(r'^cpu/([^/]+)/.*', r'\1', f)

            if 'common' in cpu:
                cpus |= _get_cpus_from_files(_get_common_user(cpu))
            else:
                cpus |= { cpu }

    return cpus

def is_updated(application_folder, subprocess_env):
    try:
        if base_branch == '':
            return True

        if '.travis.yml' in diff_files or \
           any('dist/' in s for s in diff_files):
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
                env = { 'BOARD': board }
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

def build_all():
    riotbase = environ.get('RIOTBASE') or abspath(join(dirname(abspath(__file__)), '../' * 3))
    for folder in ('examples', 'tests'):
        print('Building all applications in: {}'.format(colorize_str(folder, Termcolor.blue)))

        applications = listdir(join(riotbase, folder))
        applications = filter(lambda app: is_tracked(join(riotbase, folder, app)), applications)
        applications = sorted(applications)

        subprocess_env = environ.copy()
        subprocess_env['RIOT_DO_RETRY'] = '1'
        subprocess_env['BUILDTEST_VERBOSE'] = '1'

        for nth, application in enumerate(applications, 1):
            stdout.write('\tBuilding application: {} ({}/{}) '.format(colorize_str(application, Termcolor.blue), nth, len(applications)))
            stdout.flush()
            try:
                if not is_updated(join(riotbase, folder, application), subprocess_env):
                    print(colorize_str('(skipped)', Termcolor.yellow))
                    skipped.append(application)
                    continue
                subprocess = Popen(('make', 'buildtest'),
                                   bufsize=1, stdin=null, stdout=PIPE, stderr=null,
                                   cwd=join(riotbase, folder, application),
                                   env=subprocess_env)

                results, results_with_output = tee(get_results_and_output_from(subprocess.stdout))
                results = groupby(sorted(results), lambda (outcome, board, output): outcome)
                results_with_output = filter(lambda (outcome, board, output): output.getvalue(), results_with_output)
                failed_with_output = filter(lambda (outcome, board, output): 'failed' in outcome, results_with_output)
                success_with_output = filter(lambda (outcome, board, output): 'success' in outcome, results_with_output)
                print()
                for group, results in results:
                    print('\t\t{}: {}'.format(group, ', '.join(sorted(board for outcome, board, output in results))))
                returncode = subprocess.wait()
                if success_with_output:
                    warnings.append((application, success_with_output))
                if returncode == 0:
                    success.append(application)
                else:
                    failed.append(application)
                    errors.append((application, failed_with_output))
            except Exception, e:
                print('\n\t\tException: {}'.format(e))
                exceptions.append(application)
            finally:
                try:
                    subprocess.kill()
                except:
                    pass

def colorize_str(string, color):
    return '%s%s%s' % (color, string, Termcolor.end)

def print_output_for(buf, name, color):
    if buf:
        print('%s:' % name)
        for application, details in buf:
            for outcome, board, output in details:
                print()
                print(colorize_str('%s:%s:' % (application, board), color))
                print('%s'  % output.getvalue())

def print_outcome(outputListDescription):
    print()
    print('Outcome:')
    for color, group, name in outputListDescription:
        applications = group
        if applications:
            print('\t{}{}{}: {}'.format(color, name, Termcolor.end, ', '.join(applications)))

def print_num_of_errors_and_warnings():
    stdout.write('Errors: ')
    if errors:
        num_of_errors = sum(map(lambda x: len(x[1]), errors))
        stdout.write('%s' % colorize_str(str(num_of_errors), Termcolor.red))
    else:
        stdout.write('0')
    stdout.write(' Warnings: ')
    if warnings:
        num_of_warnings = sum(map(lambda x: len(x[1]), warnings))
        stdout.write('%s' % colorize_str(str(num_of_warnings), Termcolor.yellow))
    else:
        stdout.write('0')
    stdout.write('\n')


if __name__ == '__main__':
    success = []
    failed = []
    skipped = []
    exceptions = []
    warnings = []
    errors = []
    null = open(devnull, 'w', 0)

    if len(argv) > 1:
        base_branch = argv[1]
        diff_files = check_output(('git', 'diff', '--name-only', base_branch, 'HEAD'))
        diff_files = set(diff_files.split())
    else:
        base_branch = ''

    build_all()

    print_output_for(warnings, 'Warnings', Termcolor.yellow)
    print_output_for(errors, 'Errors', Termcolor.red)

    outputListDescription = [(Termcolor.yellow, skipped, 'skipped'), (Termcolor.green, success, 'success'),
                             (Termcolor.red, failed, 'failed'), (Termcolor.blue, exceptions, 'exceptions')]
    print_outcome(outputListDescription)

    print_num_of_errors_and_warnings()

    if exceptions:
        exit(2)
    elif failed:
        exit(1)
    else:
        exit(0)
