"""riotnode.node test module."""

import os
import sys
import signal
import tempfile

import pytest
import pexpect

import riotnode.node

CURDIR = os.path.dirname(__file__)
APPLICATIONS_DIR = os.path.join(CURDIR, 'utils', 'application')


def test_riotnode_application_dir():
    """Test the creation of a riotnode with an `application_dir`."""
    riotbase = os.path.abspath(os.environ['RIOTBASE'])
    application = os.path.join(riotbase, 'examples/hello-world')
    board = 'native'

    env = {'BOARD': board}
    node = riotnode.node.RIOTNode(application, env)

    assert node.application_directory == application
    assert node.board() == board

    clean_cmd = ['make', '--no-print-directory', '-C', application, 'clean']
    assert node.make_command(['clean']) == clean_cmd


def test_riotnode_curdir():
    """Test the creation of a riotnode with current directory."""
    riotbase = os.path.abspath(os.environ['RIOTBASE'])
    application = os.path.join(riotbase, 'examples/hello-world')
    board = 'native'

    _curdir = os.getcwd()
    _environ = os.environ.copy()
    try:
        os.environ['BOARD'] = board
        os.chdir(application)

        node = riotnode.node.RIOTNode()

        assert node.application_directory == application
        assert node.board() == board
        assert node.make_command(['clean']) == ['make', 'clean']
    finally:
        os.chdir(_curdir)
        os.environ.clear()
        os.environ.update(_environ)


@pytest.fixture(name='app_pidfile_env')
def fixture_app_pidfile_env():
    """Environment to use application pidfile"""
    with tempfile.NamedTemporaryFile() as tmpfile:
        yield {'PIDFILE': tmpfile.name}


def test_running_echo_application(app_pidfile_env):
    """Test basic functionnalities with the 'echo' application."""
    env = {'BOARD': 'board', 'APPLICATION': './echo.py'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')

        # Test multiple echo
        for i in range(16):
            child.sendline('Hello Test {}'.format(i))
            child.expect(r'Hello Test (\d+)', timeout=1)
            num = int(child.match.group(1))
            assert i == num


def test_running_error_cases(app_pidfile_env):
    """Test basic functionnalities with the 'echo' application.

    This tests:
    * stopping already stopped child
    """
    # Use only 'echo' as process to exit directly
    env = {'BOARD': 'board',
           'NODE_WRAPPER': 'echo', 'APPLICATION': 'Starting RIOT node'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')

        # Term is already finished and expect should trigger EOF
        with pytest.raises(pexpect.EOF):
            child.expect('this should eof')

    # Exiting the context manager should not crash when node is killed


def test_expect_not_matching_stdin(app_pidfile_env):
    """Test that expect does not match stdin."""
    env = {'BOARD': 'board', 'APPLICATION': './hello.py'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')
        child.expect_exact('Hello World')

        msg = "This should not be matched as it is on stdin"
        child.sendline(msg)
        matched = child.expect_exact([pexpect.TIMEOUT, msg], timeout=1)
        assert matched == 0
        # This would have matched with `node.run_term(echo=True)`


def test_expect_value(app_pidfile_env):
    """Test that expect value is being changed to the pattern."""
    env = {'BOARD': 'board', 'APPLICATION': './echo.py'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Starting RIOT node')

        # Exception is 'exc_info.value' and pattern is in 'exc.value'
        child.sendline('lowercase')
        with pytest.raises(pexpect.TIMEOUT) as exc_info:
            child.expect('UPPERCASE', timeout=0.5)
        assert str(exc_info.value) == 'UPPERCASE'

        # value updated and old value saved
        assert exc_info.value.value == 'UPPERCASE'
        assert exc_info.value.pexpect_value.startswith('Timeout exceeded.')

        child.sendline('lowercase')
        with pytest.raises(pexpect.TIMEOUT) as exc_info:
            child.expect_exact('UPPERCASE', timeout=0.5)
        assert str(exc_info.value) == 'UPPERCASE'


def test_term_cleanup(app_pidfile_env):
    """Test a terminal that does a cleanup after kill.

    The term process should be able to run its cleanup.
    """
    # Always run as 'deleted=True' to deleted even on early exception
    # File must exist at the end of the context manager
    with tempfile.NamedTemporaryFile(delete=True) as tmpfile:
        env = {'BOARD': 'board'}
        env.update(app_pidfile_env)
        env['APPLICATION'] = './create_file.py %s' % tmpfile.name

        node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
        node.TERM_STARTED_DELAY = 1
        with node.run_term(logfile=sys.stdout) as child:
            child.expect_exact('Running')
            # Ensure script is started correctly
            content = open(tmpfile.name, 'r', encoding='utf-8').read()
            assert content == 'Running\n'

        # File should not exist anymore so no error to create one
        # File must exist to be cleaned by tempfile
        open(tmpfile.name, 'x')


def test_killing_a_broken_term(app_pidfile_env):
    """Test killing a terminal that can only be killed with SIGKILL."""
    env = {'BOARD': 'board', 'APPLICATION': './sigkill_script.py'}
    env.update(app_pidfile_env)

    node = riotnode.node.RIOTNode(APPLICATIONS_DIR, env)
    node.TERM_STARTED_DELAY = 1

    with node.run_term(logfile=sys.stdout) as child:
        child.expect_exact('Kill me with SIGKILL!')
        child.expect(r'My PID: (\d+)')
        term_pid = int(child.match.group(1))

        msg = 'Some term process where not stopped'
        with pytest.raises(RuntimeError, match=msg):
            node.stop_term()

    # Send a SIGKILL to the process, it should raise an error as it is stopped
    # And if it was running, it will be cleaned
    with pytest.raises(ProcessLookupError):
        os.kill(term_pid, signal.SIGKILL)
