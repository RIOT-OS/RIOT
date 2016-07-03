#!/usr/bin/env python3
# encoding: UTF-8
# vim: ts=4 sw=4 sts=4 et

# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from collections import OrderedDict
from contextlib import closing, contextmanager
from multiprocessing.pool import ThreadPool
from os import environ, makedirs, devnull, unlink, fdopen
from os.path import basename, dirname, abspath, join
from re import match
from socket import socket, AF_INET, SOCK_STREAM, SOL_SOCKET, SO_REUSEADDR
from subprocess import check_call, CalledProcessError
from sys import stderr, stdout, argv
from tempfile import mkstemp
from threading import Thread, Event


_devnull = open(devnull, 'wb', 0)


@contextmanager
def tempfile():
    fd, name = mkstemp()
    unlink(name)
    with fdopen(fd, 'r+t', 1) as tmpfile:
        yield tmpfile


class OnlyOnce(Thread):
    def __init__(self, host, port, stderr):
        super().__init__(name='ONLY_ONCE_DAEMON', daemon=True)
        self.__host_port = (host, port)
        self.__stderr = stderr
        self.__event = Event()
        self.__success = None
        self.start()

    def await(self, timeout):
        if self.__event.wait(timeout):
            return self.__success

    def run(self):
        kept_waiting = 0
        try:
            while True:
                with closing(socket(AF_INET, SOCK_STREAM)) as sock:
                    sock.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)

                    try:
                        sock.bind(self.__host_port)
                        sock.listen(1)
                    except OSError:
                        print('Another build process is collecting data for '
                              'remote-pkg. Waiting for it to finish ...',
                              file=self.__stderr)
                        sock.connect(self.__host_port)
                        sock.recv(1)
                        kept_waiting += 1
                        continue

                    if kept_waiting:
                        print('Collecting data for remote-pkg.',
                              file=self.__stderr)

                    self.__success = True
                    self.__event.set()

                    ignored_clients = []  # prevent garbage collection
                    while True:
                        client, addr = sock.accept()
                        ignored_clients.append(client)
                        print('Another build process is waiting for us to '
                              'finish:', addr, file=self.__stderr)
        finally:
            self.__success = False
            self.__event.set()


def main(argv, lines, stdout=stdout, stderr=stderr, *,
         _ONCE_HOST='127.0.0.1',
         _ONCE_PORT=0x524f,  # "RO" -- "RIOT-OS"
         _ONCE_TIMEOUT=30,  # seconds
         _PATH=join(dirname(abspath(__file__)), 'cache'),  # git checkout root
         _DEFAULT_GROUP='RIOT-OS',
         _WORKERS=4):  # number of checkouts in parallel
    # Only allow one instance of remote-pkg.py to run at once.
    # Wait for other instance(s) to finish.

    only_once = OnlyOnce(_ONCE_HOST, int(_ONCE_PORT), stderr)
    await_result = only_once.await(_ONCE_TIMEOUT)
    if not await_result:
        if await_result is None:
            print('Waited for too long for another build process to finish '
                  'collecting data for remote-pkg.', file=stderr)
        else:
            print('Could not open socket for remote-pkg.', file=stderr)
        print('Aborting.', file=stderr)
        return 3

    # Handle lines that consist of a single project name,
    # without a git URL, but possibly with a revision.
    # Yields to process_git.

    def process_github(makefile_strm, argument):
        split_argument = argument.split('@', 1)
        if len(split_argument) == 2:
            name, revision = split_argument
        else:
            name, revision = argument, 'master'

        split_name = name.split('@', 1)
        if len(split_argument) == 2:
            group, repo = split_name
        else:
            group, repo = _DEFAULT_GROUP, name

        argument = 'https://github.com/{}/{}.git@{}'.format(group, repo, revision)
        process_git(makefile_strm, argument)

    # Handle lines that contain a full git repo URL, and possibly a revision.
    # Checks out repo and yields to process_local.

    def process_git(makefile_strm, argument):
        m = match(r'^(?P<url>[^:-][^:]*:(?:[^@]+?[:/])?(?P<folder>[^:/@.]'
                  r'[^:/@]*?)(?:\.git)?)(?:@(?P<revision>[^-].*))?$', argument)
        if not m:
            raise ValueError('Could not interpret argument: ', argument)
        url, folder, revision = m.groups()
        revision = revision or 'master'

        folder = join(_PATH, folder)
        makedirs(folder, mode=0o700, exist_ok=True)

        with tempfile() as err_buf:
            def git(*args):
                args = ('git',) + args
                err_buf.write('>>> Running: ' + repr(args) + '\n')
                check_call(args, stdin=_devnull, stdout=err_buf,
                           stderr=err_buf, cwd=folder)
                err_buf.write('\n')

            try:
                git('init', '.')

                try:
                    git('remote', 'remove', 'origin')
                except CalledProcessError:
                    pass
                git('remote', 'add', 'origin', url)

                git('fetch', 'origin', revision)
                git('reset', '--hard', 'FETCH_HEAD')
            except Exception:
                err_buf.seek(0, 0)
                raise Exception('git output:\n' + err_buf.read())

        process_local(makefile_strm, folder)

    # Handle a local folder, i.e. just invoke ./riot-pkg in the folder.

    def process_local(makefile_strm, argument):
        with tempfile() as err_buf:
            try:
                check_call(['sh', '-c', './riot-pkg'], stdin=_devnull,
                           stdout=makefile_strm, stderr=err_buf, cwd=argument)
            except Exception:
                err_buf.seek(0, 0)
                raise Exception('riot-pkg errors:\n' + err_buf.read())

    # Process a single line by calling appropriate handler function.
    # Returns (None, Makefile_content) on success,
    # and (Exception_instance, line) on error.

    def process_line(line):
        try:
            with tempfile() as makefile_strm:
                print('# REMOTE_PKG:', line, file=makefile_strm)

                split_line = line.split('+', 1)
                if len(split_line) == 2:
                    function, argument = split_line
                elif line[0] in './~':
                    function, argument = 'local', line
                else:
                    function, argument = 'github', line

                fun = {
                    'riot': process_github,
                    'github': process_github,
                    'git': process_git,
                    'local': process_local,
                }.get(function)
                if not fun:
                    raise ValueError('There is no such remote-pkg function: ' +
                                     function)

                fun(makefile_strm, argument)
                makefile_strm.seek(0, 0)
                return None, makefile_strm.read()
        except Exception as ex:
            return ex, line

    # Handle multiple lines in parallel, but retain the order
    # while printing the results.

    lines = OrderedDict((line, line) for line in lines.split())
    with ThreadPool(int(_WORKERS)) as pool:
        for ex, output in pool.map(process_line, lines):
            if ex is None:
                print(output, file=stdout)
            else:
                print('Error while processing REMOTE_PKG line:', output,
                      file=stderr)
                raise ex
    print('# REMOTE_PKG done', file=stdout)

    return 0


if __name__ == '__main__':
    argv_prefix = 'REMOTE_PKG'
    kwargs = {
        k[len(argv_prefix):] or 'lines': v
        for k, v in environ.items()
        if k.startswith(argv_prefix)
    }
    raise SystemExit(main(argv, **kwargs))
