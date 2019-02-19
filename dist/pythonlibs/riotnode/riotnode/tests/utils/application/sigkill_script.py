#! /usr/bin/env python3
"""Ignore SIGINT/SIGTERM/SIGHUP, kill with SIGKILL."""
import os
import signal


def main():
    """Only kill this program with SIGKILL."""
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    signal.signal(signal.SIGTERM, signal.SIG_IGN)
    signal.signal(signal.SIGHUP, signal.SIG_IGN)
    print('Kill me with SIGKILL!')
    print('My PID: %u' % os.getpid())
    signal.pause()


if __name__ == '__main__':
    main()
