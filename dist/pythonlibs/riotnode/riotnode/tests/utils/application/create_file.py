#! /usr/bin/env python3
"""Implement creating a file and deleting at exit

This should show a terminal program doing a cleanup.
"""

import os
import sys
import atexit
import signal
import argparse

PARSER = argparse.ArgumentParser()
PARSER.add_argument('running_file')


def main():
    """Create a file and delete it after program exits."""
    args = PARSER.parse_args()

    # Trigger atexit on SIGHUP
    signal.signal(signal.SIGHUP, (lambda *_: sys.exit(0)))

    # Delete file after program closes
    # This should be the case if normally terminated
    atexit.register(os.remove, args.running_file)

    with open(args.running_file, 'w', encoding='utf-8') as rfile:
        rfile.write('Running\n')
    print('Running')
    while True:
        signal.pause()


if __name__ == '__main__':
    sys.exit(main())
