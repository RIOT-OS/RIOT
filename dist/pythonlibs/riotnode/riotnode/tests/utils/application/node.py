#! /usr/bin/env python3
"""Wrap an application to behave like a board firmware.

+ Start a command given as argument
+ Handle 'reset' the firmware when receiving `SIGUSR1`


Ideas for extensions:

* resetting or not on reset
* See how to implement loosing some of the output on first startup
"""


import sys
import signal
import threading
import argparse
import subprocess

PARSER = argparse.ArgumentParser()
PARSER.add_argument('argument', nargs='+', default=[])

# Signals sent by 'pexpect' + SIGTERM
FORWARDED_SIGNALS = (signal.SIGHUP, signal.SIGCONT, signal.SIGINT,
                     signal.SIGTERM)


def forward_signal(signum, proc):
    """Forward signal to child."""
    if not proc.poll():
        proc.send_signal(signum)


def _run_cmd(args, termonsig=signal.SIGUSR1, **popenkwargs):
    """Run a subprocess of `args`.

    It will be terminated on `termonsig` signal.

    :param args: command arguments
    :param termonsig: terminate the process on `termonsig` signal
    :param **popenkwargs: Popen kwargs
    :return: True if process should be restarted
    """
    restart_process = threading.Event()
    proc = subprocess.Popen(args, **popenkwargs)

    # Forward cleanup processes to child
    for sig in FORWARDED_SIGNALS:
        signal.signal(sig, lambda signum, _: forward_signal(signum, proc))

    # set 'termonsig' handler for reset
    def _reset(*_):
        """Terminate process and set the 'restart_process' flag."""
        restart_process.set()
        proc.terminate()
    signal.signal(termonsig, _reset)

    proc.wait()
    return restart_process.is_set()


def main():
    """Run an application in a loop.

    On 'SIGUSR1' the application will be reset.
    """
    args = PARSER.parse_args()
    while _run_cmd(args.argument):
        pass


if __name__ == '__main__':
    sys.exit(main())
