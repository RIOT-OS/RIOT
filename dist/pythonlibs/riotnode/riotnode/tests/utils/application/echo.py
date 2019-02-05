#! /usr/bin/env python3
"""Firmware implementing echoing line inputs."""

import sys


def main():
    """Print some header and echo the output."""
    print('Starting RIOT node')
    print('This example will echo')
    while True:
        print(input())


if __name__ == '__main__':
    sys.exit(main())
