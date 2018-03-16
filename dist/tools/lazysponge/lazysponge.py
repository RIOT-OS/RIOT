#! /usr/bin/env python3

#
# Copyright (C) 2018 Gaëtan Harter <gaetan.harter@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

"""
lazysponge

Adaptation of moreutils `sponge` with added functionnality that it does not
modify the output file if the content would be unchanged.

Description
-----------

Reads standard input and writes it to the specified file if its content was
different.

The file is not changed if the content is the same so modification timestamp is
unchanged.

Note
----

It only works with input provided by a `pipe` and not interractive input.
The reason is that `ctrl+c` would not be handled properly in that case.

Usage
-----

    usage: lazysponge.py [-h] outfile

    Soak up all input from stdin and write it to <outfile> if it differs from
    previous content. If the content is the same, file is not modified.

    positional arguments:
    outfile     Output file

    optional arguments:
    -h, --help  show this help message and exit
"""

import os
import sys
import argparse
import hashlib

DESCRIPTION = ('Soak up all input from stdin and write it to <outfile>'
               ' if it differs from previous content.\n'
               ' If the content is the same, file is not modified.')
PARSER = argparse.ArgumentParser(description=DESCRIPTION)
PARSER.add_argument('outfile', help='Output file')
PARSER.add_argument('--verbose', '-v', help='Verbose output', default=False,
                    action='store_true')


def _print_hash_debug_info(outfilename, oldbytes, newbytes):
    """Print debug information on hashs."""
    oldhash = hashlib.md5(oldbytes).hexdigest() if oldbytes is not None else ''
    newhash = hashlib.md5(newbytes).hexdigest()
    if oldbytes == newbytes:
        msg = 'Keeping old {} ({})'.format(outfilename, oldhash)
    else:
        msg = 'Replacing {} ({} != {})'.format(outfilename, oldhash, newhash)
    print(msg, file=sys.stderr)


def main():
    """Write stdin to given <outfile> if it would change its content."""
    opts = PARSER.parse_args()

    # No support for 'interactive' input as catching Ctrl+c breaks in 'read'
    if os.isatty(sys.stdin.fileno()):
        print('Interactive input not supported. Use piped input',
              file=sys.stderr)
        print('  echo message | {}'.format(' '.join(sys.argv)),
              file=sys.stderr)
        exit(1)

    try:
        with open(opts.outfile, 'rb') as outfd:
            oldbytes = outfd.read()
    except FileNotFoundError:
        oldbytes = None

    stdinbytes = sys.stdin.buffer.read()
    if opts.verbose:
        _print_hash_debug_info(opts.outfile, oldbytes, stdinbytes)

    if oldbytes == stdinbytes:
        return

    with open(opts.outfile, 'wb') as outfd:
        outfd.write(stdinbytes)


if __name__ == '__main__':
    main()
