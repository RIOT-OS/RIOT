#! /usr/bin/env python3
"""Firmware implementing a simple hello-world."""

import sys
import signal


def main():
    """Print some header and do nothing."""
    print('Starting RIOT node')
    print('Hello World')
    while True:
        signal.pause()


if __name__ == '__main__':
    sys.exit(main())
