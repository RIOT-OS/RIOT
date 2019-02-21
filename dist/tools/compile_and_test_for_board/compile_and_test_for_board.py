#! /usr/bin/env python3

"""
This script handles building all applications and tests for one board and also
execute tests if they are available.

An incremental build can selected using `--incremental` to not rerun successful
compilation and tests. But then it should be run on a fixed version of the
repository as no verification is done if results are up to date with the RIOT
repository.

It by defaults finds all tests in `examples` and `tests` but list of tests can
be provided by command line options with also an exclude list, to for example
not rerun a long failing test every time.


It is a temporary solution until an equivalent is implemented in the build
system. It is also a showcase of what could be dummy file output for
compilation and tests.


Example
-------

By default it should be run as

    ./compile_and_test_for_board.py path_to_riot_directory board_name [results]


Usage
-----


```
usage: compile_and_test_for_board.py [-h] [--applications APPLICATIONS]
                                     [--applications-exclude
                                      APPLICATIONS_EXCLUDE]
                                     [--no-test]
                                     [--loglevel {debug,info,warning,error,
                                                  fatal,critical}]
                                     [--incremental] [--clean-after]
                                     [--jobs JOBS]
                                     riot_directory board [result_directory]

positional arguments:
  riot_directory        RIOT directory to test
  board                 Board to test
  result_directory      Result directory, by default "results"

optional arguments:
  -h, --help            show this help message and exit
  --applications APPLICATIONS
                        List of applications to test, overwrites default
                        configuration of testing all applications
  --applications-exclude APPLICATIONS_EXCLUDE
                        List of applications to exclude from tested
                        applications. Also applied after "--applications".
  --no-test             Disable executing tests
  --loglevel {debug,info,warning,error,fatal,critical}
                        Python logger log level, defauts to "info"
  --incremental         Do not rerun successful compilation and tests
  --clean-after         Clean after running each test
  --jobs JOBS, -j JOBS  Parallel building (0 means not limit, like '--jobs')
```
"""

import os
import sys
import glob
import shutil
import logging
import argparse
import subprocess
import collections

LOG_HANDLER = logging.StreamHandler()
LOG_HANDLER.setFormatter(logging.Formatter(logging.BASIC_FORMAT))

LOG_LEVELS = ('debug', 'info', 'warning', 'error', 'fatal', 'critical')


class ErrorInTest(Exception):
    """Custom exception for a failed test.

    It contains the step that failed in 'message', the 'application' and the
    'errorfile' path to the execution error.
    """
    def __init__(self, message, application, errorfile):
        super().__init__(message)
        self.application = application
        self.errorfile = errorfile


def apps_directories(riotdir, apps_dirs=None, apps_dirs_skip=None):
    """Return sorted list of test directories relative to `riotdir`.

    By default it uses RIOT 'info-applications' command to list them.

    :param riotdir: base riot directory
    :param apps_dirs: use this applications list instead of the RIOT list
    :param apps_dirs_skip: list of application directories to remove, applied
                           on the RIOT list or `apps_dirs`
    """
    apps_dirs = apps_dirs or _riot_tracked_applications_dirs(riotdir)
    apps_dirs_skip = apps_dirs_skip or []

    # Remove applications to skip
    apps_dirs = set(apps_dirs) - set(apps_dirs_skip)

    return sorted(list(apps_dirs))


def _is_git_repo(riotdir):
    """Check if directory is a git repository."""
    cmd = ['git', 'rev-parse', '--git-dir']
    ret = subprocess.call(cmd, cwd=riotdir,
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL)
    return ret == 0


def _is_git_tracked(appdir):
    """Check if directory is a git repository."""
    cmd = ['git', 'ls-files', '--error-unmatch', 'Makefile']
    ret = subprocess.call(cmd, cwd=appdir,
                          stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL)
    return ret == 0


def _riot_tracked_applications_dirs(riotdir):
    """Applications directories in the RIOT repository with relative path.

    Only return 'tracked' applications if riotdir is a git repository.
    """
    apps_dirs = _riot_applications_dirs(riotdir)

    # Only keep tracked directories
    if _is_git_repo(riotdir):
        apps_dirs = [dir for dir in apps_dirs
                     if _is_git_tracked(os.path.join(riotdir, dir))]
    return apps_dirs


def _riot_applications_dirs(riotdir):
    """Applications directories in the RIOT repository with relative path."""
    cmd = ['make', 'info-applications']

    out = subprocess.check_output(cmd, cwd=riotdir)
    out = out.decode('utf-8', errors='replace')
    return out.split()


def check_is_board(riotdir, board):
    """Verify if board is a RIOT board.

    :raises ValueError: on invalid board
    :returns: board name
    """
    if board == 'common':
        raise ValueError("'%s' is not a board" % board)
    board_dir = os.path.join(riotdir, 'boards', board)
    if not os.path.isdir(board_dir):
        raise ValueError("Cannot find '%s' in %s/boards" % (board, riotdir))
    return board


def create_directory(directory, clean=False, mode=0o755):
    """Directory creation helper with `clean` option.

    :param clean: tries deleting the directory before re-creating it
    """
    if clean:
        try:
            shutil.rmtree(directory)
        except OSError:
            pass
    os.makedirs(directory, mode=mode, exist_ok=True)


class RIOTApplication():
    """RIOT Application representation.

    Allows calling make commands on an application for a board.

    :param board: board name
    :param riotdir: RIOT repository directory
    :param appdir: directory of the application, can be relative to riotdir
    :param resultdir: base directory where to put execution results
    """

    MAKEFLAGS = ('RIOT_CI_BUILD=1', 'CC_NOCOLOR=1', '--no-print-directory')

    COMPILE_TARGETS = ('clean', 'all',)
    TEST_TARGETS = ('test',)

    def __init__(self, board, riotdir, appdir, resultdir):
        self.board = board
        self.riotdir = riotdir
        self.appdir = appdir
        self.resultdir = os.path.join(resultdir, appdir)
        self.logger = logging.getLogger('%s.%s' % (board, appdir))

    # Extract values from make
    def name(self):
        """Get application name."""
        appname = self.make(['info-debug-variable-APPLICATION'],
                            log_error=True).strip()
        self.logger.debug('APPLICATION: %s', appname)
        return appname

    def has_test(self):
        """Detect if the application has tests.

        Use '--silent' to disable the message from packages:

            make[1]: Nothing to be done for 'Makefile.include'
        """
        tests = self.make(['--silent', 'info-debug-variable-TESTS'],
                          log_error=True).strip()
        return bool(tests)

    def board_is_supported(self):
        """Return if current board is supported."""
        env = {'BOARDS': self.board}
        cmd = ['info-boards-supported']
        ret = self.make(cmd, env=env, log_error=True).strip()

        supported = ret == self.board
        self.logger.info('Board supported: %s', supported)
        return supported

    def board_has_enough_memory(self):
        """Return if current board has enough memory."""
        cmd = ['info-debug-variable-BOARD_INSUFFICIENT_MEMORY']
        boards = self.make(cmd, log_error=True).strip().split()

        has_enough_memory = self.board not in boards
        self.logger.info('Board has enough memory: %s', has_enough_memory)
        return has_enough_memory

    def clean(self):
        """Clean build and packages."""
        try:
            cmd = ['clean', 'clean-pkg']
            self.make(cmd)
        except subprocess.CalledProcessError as err:
            self.logger.warning('Got an error during clean, ignore: %r', err)

    def clean_intermediates(self):
        """Clean intermediates only."""
        try:
            cmd = ['clean-intermediates']
            self.make(cmd)
        except subprocess.CalledProcessError as err:
            self.logger.warning('Got an error during clean-intermediates,'
                                ' ignore: %r', err)

    def run_compilation_and_test(self, **test_kwargs):
        """Same as `compilation_and_test` but handles exception.

        :returns: 0 on success and 1 on error.
        """
        try:
            self.compilation_and_test(**test_kwargs)
            return None
        except ErrorInTest as err:
            self.logger.error('Failed during: %s', err)
            return (str(err), err.application.appdir, err.errorfile)

    def compilation_and_test(self, clean_after=False, runtest=True,
                             incremental=False, jobs=False):
        """Compile and execute test if available.

        Checks for board supported/enough memory, compiles.
        If there are tests, also flash the device and run them.

        Output files are written in `self.resultdir`

        When `clean_after` is set, clean intermediates files not required for
        the possible following steps. It keeps the elffile after compiling in
        case test would be run later and does a full clean after the test
        succeeds.

        :param incremental: Do not rerun successful compilation and tests
        :raises ErrorInTest: on execution failed during one step
        """

        # Ignore incompatible APPS
        if not self.board_is_supported():
            create_directory(self.resultdir, clean=True)
            self._write_resultfile('skip', 'not_supported')
            return

        if not self.board_has_enough_memory():
            create_directory(self.resultdir, clean=True)
            self._write_resultfile('skip', 'not_enough_memory')
            return

        # Normal case for supported apps
        create_directory(self.resultdir, clean=not incremental)

        # Run compilation and flash+test
        # It raises ErrorInTest on error which is handled outside

        compilation_cmd = list(self.COMPILE_TARGETS)
        if jobs is not None:
            compilation_cmd += ['--jobs']
            if jobs:
                compilation_cmd += [str(jobs)]
        self.make_with_outfile('compilation', compilation_cmd)
        if clean_after:
            self.clean_intermediates()

        if runtest:
            if self.has_test():
                setuptasks = collections.OrderedDict(
                    [('flash', ['flash-only'])])
                self.make_with_outfile('test', self.TEST_TARGETS,
                                       save_output=True, setuptasks=setuptasks)
                if clean_after:
                    self.clean()
            else:
                self._write_resultfile('test', 'skip.no_test')

        self.logger.info('Success')

    def make(self, args, env=None, log_error=False):
        """Run make command in appdir."""
        env = env or {}
        # HACK: BOARD should be set for make in environment and not command
        # line either it break the `BOARD=none` for global commands
        env['BOARD'] = self.board

        full_env = os.environ.copy()
        full_env.update(env)

        cmd = ['make']
        cmd.extend(self.MAKEFLAGS)
        cmd.extend(['-C', os.path.join(self.riotdir, self.appdir)])
        cmd.extend(args)

        self.logger.debug('%r ENV %s', cmd, env)
        # Call without 'universal_newlines' to have bytes and handle decoding
        # (encoding and errors are only  supported after python 3.6)
        try:
            out = subprocess.check_output(cmd, env=full_env,
                                          stderr=subprocess.STDOUT)
            out = out.decode('utf-8', errors='replace')
        except subprocess.CalledProcessError as err:
            err.output = err.output.decode('utf-8', errors='replace')
            if log_error:
                self.logger.error('Error during command: \n%s', err.output)
            raise err
        return out

    def make_with_outfile(self, name, args, save_output=False,
                          setuptasks=None):
        """Run make but save result in an outfile.

        It will be saved in `self.resultdir/name.[success|failure]`.

        :param name: basename to use for the result file.
        :param save_output: output should be saved in the outfile and returned,
                            if not, return an empty string.
        :param setuptasks: OrderedDict of tasks to run before the main one
        """
        self.logger.info('Run %s', name)
        setuptasks = setuptasks or {}

        # Do not re-run if success
        output = self._make_get_previous_output(name)
        if output is not None:
            return output

        # Run setup-tasks, output is only kept in case of error
        for taskname, taskargs in setuptasks.items():
            taskname = '%s.%s' % (name, taskname)
            self.logger.info('Run %s', taskname)
            try:
                self.make(taskargs)
            except subprocess.CalledProcessError as err:
                self._make_handle_error(taskname, err)

        # Run make command
        try:
            output = self.make(args)
            if not save_output:
                output = ''
            self._write_resultfile(name, 'success', output)
            return output
        except subprocess.CalledProcessError as err:
            self._make_handle_error(name, err)

    def _make_get_previous_output(self, name):
        """Get previous result output for step `name`.

        Returns `output` if it is there, None if not.
        """
        try:
            with open(self._outfile('%s.success' % name),
                      encoding='utf-8') as outputfd:
                self.logger.info('Nothing to be done for %s', name)
                return outputfd.read()
        except OSError:
            pass
        return None

    def _make_handle_error(self, name, err):
        """Handle exception during make step `name`."""
        output = ' '.join(err.cmd) + '\n'
        output += err.output + '\n'
        output += 'Return value: %s\n' % err.returncode
        outfile = self._write_resultfile(name, 'failed', output)

        self.logger.warning(output)
        self.logger.error('Error during %s, writing to %s', name, outfile)
        raise ErrorInTest(name, self, outfile)

    def _write_resultfile(self, name, status, body=''):
        """Write `body` to result file `name.status`.

        It also deletes other `name.*` files before.
        """

        # Delete previous status files
        resultfiles = glob.glob(self._outfile('%s.*' % name))
        for resultfile in resultfiles:
            try:
                os.remove(resultfile)
            except OSError:
                pass

        # Create new file
        filename = '%s.%s' % (name, status)
        outfile = self._outfile(filename)

        with open(outfile, 'w+', encoding='utf-8',
                  errors='replace') as outfd:
            outfd.write(body)
            outfd.flush()
        return outfile

    def _outfile(self, filename):
        """Give path to `filename` with `self.resultdir`."""
        return os.path.join(self.resultdir, filename)


TOOLCHAIN_SCRIPT = 'dist/tools/ci/print_toolchain_versions.sh'


def print_toolchain(riotdir):
    """Print toolchain using RIOT script.

    Does not handle any execution error
    """
    toolchain_script = os.path.join(riotdir, TOOLCHAIN_SCRIPT)
    out = subprocess.check_output([toolchain_script])
    return out.decode('utf-8', errors='replace')


def save_toolchain(riotdir, resultdir):
    """Save toolchain in 'resultdir/toolchain'."""
    outfile = os.path.join(resultdir, 'toolchain')
    create_directory(resultdir)

    toolchain = print_toolchain(riotdir)
    with open(outfile, 'w+', encoding='utf-8', errors='replace') as outputfd:
        outputfd.write(toolchain)


def _test_failed_summary(errors, relpathstart=None):
    """Generate a test summary for failures."""
    if not errors:
        return ''

    errors_dict = {}
    for step, appdir, errorfile in errors:
        if relpathstart:
            errorfile = os.path.relpath(errorfile, relpathstart)
        errors_dict.setdefault(step, []).append((appdir, errorfile))

    summary = ''
    for step, errs in sorted(errors_dict.items()):
        summary += 'Failures during %s:\n' % step
        for appdir, errorfile in errs:
            summary += '- [%s](%s)\n' % (appdir, errorfile)
        # Separate sections with a new line
        summary += '\n'

    # Remove last new line
    summary = summary[:-1]
    return summary


def save_failure_summary(resultdir, summary):
    """Save test summary in 'resultdir/board/failuresummary'."""
    outfile = os.path.join(resultdir, 'failuresummary.md')

    with open(outfile, 'w+', encoding='utf-8', errors='replace') as outputfd:
        outputfd.write(summary)


# Parsing functions


def list_from_string(list_str=None):
    """Get list of items from `list_str`

    >>> list_from_string(None)
    []
    >>> list_from_string("")
    []
    >>> list_from_string("  ")
    []
    >>> list_from_string("a")
    ['a']
    >>> list_from_string("a  ")
    ['a']
    >>> list_from_string("a b  c")
    ['a', 'b', 'c']
    """
    value = (list_str or '').split(' ')
    return [v for v in value if v]


def _strip_board_equal(board):
    """Sanitizy board if given as BOARD=board.

    Increase RIOT compatibility.
    """
    if board.startswith('BOARD='):
        board = board.replace('BOARD=', '')
    return board


PARSER = argparse.ArgumentParser(
    formatter_class=argparse.ArgumentDefaultsHelpFormatter)
PARSER.add_argument('riot_directory', help='RIOT directory to test')
PARSER.add_argument('board', help='Board to test', type=_strip_board_equal)
PARSER.add_argument('result_directory', nargs='?', default='results',
                    help='Result directory, by default "results"')
PARSER.add_argument(
    '--applications', type=list_from_string,
    help=('List of applications to test, overwrites default configuration of'
          ' testing all applications'),
)
PARSER.add_argument(
    '--applications-exclude', type=list_from_string,
    help=('List of applications to exclude from tested applications.'
          ' Also applied after "--applications".'),
)
PARSER.add_argument('--no-test', action='store_true', default=False,
                    help='Disable executing tests')
PARSER.add_argument('--loglevel', choices=LOG_LEVELS, default='info',
                    help='Python logger log level, defauts to "info"')
PARSER.add_argument('--incremental', action='store_true', default=False,
                    help='Do not rerun successful compilation and tests')
PARSER.add_argument('--clean-after', action='store_true', default=False,
                    help='Clean after running each test')

PARSER.add_argument('--compile-targets', type=list_from_string,
                    default=' '.join(RIOTApplication.COMPILE_TARGETS),
                    help='List of make targets to compile')
PARSER.add_argument('--test-targets', type=list_from_string,
                    default=' '.join(RIOTApplication.TEST_TARGETS),
                    help='List of make targets to run test')

PARSER.add_argument(
    '--jobs', '-j', type=int, default=None,
    help="Parallel building (0 means not limit, like '--jobs')")


def main():
    """For one board, compile all examples and tests and run test on board."""
    args = PARSER.parse_args()

    logger = logging.getLogger(args.board)
    if args.loglevel:
        loglevel = logging.getLevelName(args.loglevel.upper())
        logger.setLevel(loglevel)

    logger.addHandler(LOG_HANDLER)

    logger.info('Saving toolchain')
    save_toolchain(args.riot_directory, args.result_directory)

    board = check_is_board(args.riot_directory, args.board)
    logger.debug('board: %s', board)

    app_dirs = apps_directories(args.riot_directory,
                                apps_dirs=args.applications,
                                apps_dirs_skip=args.applications_exclude)

    logger.debug('app_dirs: %s', app_dirs)
    logger.debug('resultdir: %s', args.result_directory)
    board_result_directory = os.path.join(args.result_directory, args.board)

    # Overwrite the compile/test targets from command line arguments
    RIOTApplication.COMPILE_TARGETS = args.compile_targets
    RIOTApplication.TEST_TARGETS = args.test_targets

    # List of applications for board
    applications = [RIOTApplication(board, args.riot_directory, app_dir,
                                    board_result_directory)
                    for app_dir in app_dirs]

    # Execute tests
    errors = [app.run_compilation_and_test(clean_after=args.clean_after,
                                           runtest=not args.no_test,
                                           incremental=args.incremental,
                                           jobs=args.jobs)
              for app in applications]
    errors = [e for e in errors if e is not None]
    num_errors = len(errors)

    summary = _test_failed_summary(errors, relpathstart=board_result_directory)
    save_failure_summary(board_result_directory, summary)

    if num_errors:
        logger.error('Tests failed: %d', num_errors)
        print(summary, end='')
    else:
        logger.info('Tests successful')
    sys.exit(num_errors)


if __name__ == '__main__':
    main()
