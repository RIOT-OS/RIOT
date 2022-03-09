#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2014  René Kijewski  <rene.kijewski@fu-berlin.de>
# Copyright (C) 2015  Philipp Rosenkranz  <philipp.rosenkranz@fu-berlin.de>
# Copyright (C) 2016  Eistec AB
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

from collections import defaultdict
from itertools import groupby
from os import devnull, environ
from os.path import abspath, dirname, isfile, join
from subprocess import CalledProcessError, check_call, check_output, PIPE, Popen
from sys import argv, exit, stdout
from io import StringIO
from itertools import tee


MAKE = environ.get("MAKE", "make")


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
    read_more_output = True
    results_prefix = 'Building for '
    output_prefix = 'Building application '
    prev_results = False
    result = ['']
    output = StringIO()
    while 1:
        line = fd.readline().decode('utf-8', errors='replace')
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


def get_app_dirs():
    return check_output([MAKE, "-f", "makefiles/app_dirs.inc.mk", "info-applications"]) \
            .decode("utf-8", errors="ignore")\
            .split()


def split_apps_by_dir(app_dirs):
    """ creates a dictionary as follows:
    { "examples": ["hello_world", "gnrc_networking" ],
      "tests": ["minimal", "fmt_print" ]
      }
    """
    res = defaultdict(list)
    for app_dir in app_dirs:
        folder, app = app_dir.split("/", 1)
        res[folder].append(app)

    return res


def build_all():
    riotbase = environ.get('RIOTBASE') or abspath(join(dirname(abspath(__file__)), '../' * 3))
    app_folders = split_apps_by_dir(get_app_dirs())
    for folder in sorted(app_folders):
        print('Building all applications in: {}'.format(colorize_str(folder, Termcolor.blue)))

        applications = app_folders[folder]
        applications = filter(lambda app: is_tracked(join(riotbase, folder, app)), applications)
        applications = sorted(applications)

        subprocess_env = environ.copy()
        subprocess_env['RIOT_DO_RETRY'] = '1'
        subprocess_env['BUILDTEST_VERBOSE'] = '1'

        for nth, application in enumerate(applications, 1):
            stdout.write('\tBuilding application: {} ({}/{}) '.format(
                colorize_str(application, Termcolor.blue),
                nth, len(applications)))
            stdout.flush()
            try:
                app_dir = join(riotbase, folder, application)
                subprocess = Popen((MAKE, 'buildtest'),
                                   bufsize=1, stdin=null, stdout=PIPE, stderr=null,
                                   cwd=app_dir,
                                   env=subprocess_env)

                results, results_with_output = tee(get_results_and_output_from(subprocess.stdout))
                results = groupby(sorted(results), lambda res: res[0])
                results_with_output = list(filter(lambda res: res[2].getvalue(), results_with_output))
                failed_with_output = list(filter(lambda res: 'failed' in res[0], results_with_output))
                success_with_output = list(filter(lambda res: 'success' in res[0], results_with_output))
                # check if bin-directory isn't in system's PATH to not accidentally
                # delete some valuable system executable ;-)
                if join(app_dir, "bin") not in environ.get("PATH", "/bin:/usr/bin:/usr/local/bin:"):
                    check_call(["rm", "-rf", join(app_dir, "bin")])
                print()
                for group, result in results:
                    print('\t\t{}: {}'.format(group, ', '.join(sorted(board for outcome, board, output in result))))
                returncode = subprocess.wait()
                if success_with_output:
                    warnings.append((application, success_with_output))
                if returncode == 0:
                    success.append(application)
                else:
                    if not failed_with_output:
                        print(colorize_str('\t\tmake buildtest error!', Termcolor.red))
                    failed.append(application)
                    errors.append((application, failed_with_output))
            except Exception as e:
                print('\n\t\tException: {}'.format(e))
                exceptions.append(application)
            finally:
                try:
                    subprocess.kill()
                except Exception:
                    pass


def colorize_str(string, color):
    return '%s%s%s' % (color, string, Termcolor.end)


def print_output_for(buf, name, color):
    if buf:
        print('%s:' % name)
        for application, details in buf:
            for _, board, output in details:
                print()
                print(colorize_str('%s:%s:' % (application, board), color))
                print('%s' % output.getvalue())


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
    exceptions = []
    warnings = []
    errors = []
    null = open(devnull, 'wb', 0)

    if len(argv) > 1:
        base_branch = argv[1]
        diff_files = check_output(('git', 'diff', '--name-only', base_branch, 'HEAD'))
        diff_files = set(diff_files.split())
    else:
        base_branch = ''

    build_all()

    print_output_for(warnings, 'Warnings', Termcolor.yellow)
    print_output_for(errors, 'Errors', Termcolor.red)

    outputListDescription = [(Termcolor.green, success, 'success'),
                             (Termcolor.red, failed, 'failed'),
                             (Termcolor.blue, exceptions, 'exceptions')]
    print_outcome(outputListDescription)

    print_num_of_errors_and_warnings()

    if exceptions:
        exit(2)
    elif failed:
        exit(1)
    else:
        exit(0)
