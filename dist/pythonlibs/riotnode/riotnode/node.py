"""RIOTNode abstraction.

Define class to abstract a node over the RIOT build system.
"""

import os
import time
import logging
import subprocess
import contextlib
import signal

import pexpect

DEVNULL = open(os.devnull, 'w')


class TermSpawn(pexpect.spawn):
    """Subclass to adapt the behaviour to our need.

    * change default `__init__` values
      * disable local 'echo' to not match send messages
      * 'utf-8/replace' by default
      * default timeout
    """

    def __init__(self,  # pylint:disable=too-many-arguments
                 command, timeout=10, echo=False,
                 encoding='utf-8', codec_errors='replace', **kwargs):
        super().__init__(command, timeout=timeout, echo=echo,
                         encoding=encoding, codec_errors=codec_errors,
                         **kwargs)


class RIOTNode():
    """Class abstracting a RIOTNode in an application.

    This should abstract the build system integration.

    :param application_directory: relative directory to the application.
    :param env: dictionary of environment variables that should be used.
                These overwrites values coming from `os.environ` and can help
                define factories where environment comes from a file or if the
                script is not executed from the build system context.

    Environment variable configuration

    :environment BOARD: current RIOT board type.
    :environment RIOT_TERM_START_DELAY: delay before `make term` is said to be
                                        ready after calling.
    """

    TERM_SPAWN_CLASS = TermSpawn
    TERM_STOP_SIGNAL = signal.SIGKILL
    TERM_STARTED_DELAY = int(os.environ.get('RIOT_TERM_START_DELAY') or 3)

    MAKE_ARGS = ()
    RESET_TARGETS = ('reset',)

    def __init__(self, application_directory='.', env=None):
        self._application_directory = application_directory

        # TODO I am not satisfied by this, but would require changing all the
        # environment handling, just put a note until I can fix it.
        # I still want to show a PR before this
        # I would prefer getting either no environment == os.environ or the
        # full environment to use.
        # It should also change the `TERM_STARTED_DELAY` thing.
        self.env = os.environ.copy()
        self.env.update(env or {})

        self.term = None  # type: pexpect.spawn

        self.logger = logging.getLogger(__name__)

    @property
    def application_directory(self):
        """Absolute path to the current directory."""
        return os.path.abspath(self._application_directory)

    def board(self):
        """Return board type."""
        return self.env['BOARD']

    def reset(self):
        """Reset current node."""
        # Ignoring 'reset' return value was taken from `testrunner`.
        # For me it should not be done for all boards as it should be an error.
        # I would rather fix it in the build system or have a per board
        # configuration.

        # Make reset yields error on some boards even if successful
        # Ignore printed errors and returncode
        self.make_run(self.RESET_TARGETS, stdout=DEVNULL, stderr=DEVNULL)

    @contextlib.contextmanager
    def run_term(self, reset=True, **startkwargs):
        """Terminal context manager."""
        try:
            self.start_term(**startkwargs)
            if reset:
                self.reset()
            yield self.term
        finally:
            self.stop_term()

    def start_term(self, **spawnkwargs):
        """Start the terminal.

        The function is blocking until it is ready.
        It waits some time until the terminal is ready and resets the node.
        """
        self.stop_term()

        term_cmd = self.make_command(['term'])
        self.term = self.TERM_SPAWN_CLASS(term_cmd[0], args=term_cmd[1:],
                                          env=self.env, **spawnkwargs)

        # on many platforms, the termprog needs a short while to be ready
        time.sleep(self.TERM_STARTED_DELAY)

    def stop_term(self):
        """Stop the terminal."""
        self._safe_term_close()

    def _safe_term_close(self):
        """Safe 'term.close'.

        Handles possible exceptions.
        """
        try:
            self._kill_term()
        except AttributeError:
            # Not initialized
            return
        except ProcessLookupError:
            self.logger.warning('Process already stopped')

        self.term.close()

    def _kill_term(self):
        """Kill the current terminal."""
        # killpg(SIGKILL) was taken from `testrunner`.
        # I do not really like direct `SIGKILL` as it prevents script cleanup.
        # I kept it as I do not want to break an edge case that rely on it.

        # Using 'killpg' shows that our shell script do not correctly kill
        # programs they started. So this is more a hack than a real solution.
        os.killpg(os.getpgid(self.term.pid), self.TERM_STOP_SIGNAL)

    def make_run(self, targets, *runargs, **runkwargs):
        """Call make `targets` for current RIOTNode context.

        It is using `subprocess.run` internally.

        :param targets: make targets
        :param *runargs: args passed to subprocess.run
        :param *runkwargs: kwargs passed to subprocess.run
        :return: subprocess.CompletedProcess object
            """
        command = self.make_command(targets)
        return subprocess.run(command, env=self.env, *runargs, **runkwargs)

    def make_command(self, targets):
        """Make command for current RIOTNode context.

        :return: list of command arguments (for example for subprocess)
        """
        command = ['make']
        command.extend(self.MAKE_ARGS)
        if self._application_directory != '.':
            dir_cmd = '--no-print-directory', '-C', self.application_directory
            command.extend(dir_cmd)
        command.extend(targets)
        return command
