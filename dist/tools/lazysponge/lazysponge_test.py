#! /usr/bin/env python3

#
# Copyright (C) 2018 Gaëtan Harter <gaetan.harter@fu-berlin.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

"""Test script for lazysponge."""

import os
import sys
import shutil
import tempfile
from io import StringIO, BytesIO

import unittest
from unittest import mock

import lazysponge


class TestLazysponge(unittest.TestCase):
    """Test the lazysponge script.

    Tested using mocks for stdin.
    """

    def setUp(self):
        self.isatty_ret = False
        self.isatty = mock.patch.object(
            os, 'isatty', lambda _: self.isatty_ret).start()

        self.tmpdir = tempfile.mkdtemp()
        self.outfile = os.path.join(self.tmpdir, 'outfile')

        self.argv = ['lazysponge', self.outfile]
        mock.patch.object(sys, 'argv', self.argv).start()

        self.stdin = mock.Mock()
        self.stdin.fileno.return_value = 0
        mock.patch.object(sys, 'stdin', self.stdin).start()
        self.stdin.buffer = BytesIO()

    def tearDown(self):
        shutil.rmtree(self.tmpdir, ignore_errors=True)
        mock.patch.stopall()

    def test_write_one_file(self):
        """Test a simple case where we write one file without quiet output."""
        first_input = b'First input\n'

        # Write input once
        self.stdin.buffer.write(first_input)
        self.stdin.buffer.seek(0)
        stderr = StringIO()
        with mock.patch('sys.stderr', stderr):
            lazysponge.main()
        self.assertEqual(stderr.getvalue(), '')
        # no errors
        os.stat(self.outfile)
        with open(self.outfile, 'rb') as outfd:
            self.assertEqual(outfd.read(), first_input)

    def test_write_two_times_and_update(self):
        """Test writing two times the same output plus a new one."""
        first_input = b'First input\n'
        updated_input = b'Second input\n'
        stderr = StringIO()

        self.argv.append('--verbose')

        # File does not exist
        with self.assertRaises(OSError):
            os.stat(self.outfile)

        # Write input once
        self.stdin.buffer.write(first_input)
        self.stdin.buffer.seek(0)
        with mock.patch('sys.stderr', stderr):
            lazysponge.main()
        first_stat = os.stat(self.outfile)
        with open(self.outfile, 'rb') as outfd:
            self.assertEqual(outfd.read(), first_input)
        self._truncate(self.stdin.buffer)

        # compare stderr verbose output
        errmsg = 'Replacing %s ( != 96022020c795ee69653958a3cb4bb083)\n'
        self.assertEqual(stderr.getvalue(), errmsg % self.outfile)
        self._truncate(stderr)

        # Re-Write the same input
        self.stdin.buffer.write(first_input)
        self.stdin.buffer.seek(0)
        with mock.patch('sys.stderr', stderr):
            lazysponge.main()
        second_stat = os.stat(self.outfile)
        with open(self.outfile, 'rb') as outfd:
            self.assertEqual(outfd.read(), first_input)
        self._truncate(self.stdin.buffer)

        # File has not been modified
        self.assertEqual(first_stat, second_stat)
        # compare stderr verbose output
        errmsg = 'Keeping old %s (96022020c795ee69653958a3cb4bb083)\n'
        self.assertEqual(stderr.getvalue(), errmsg % self.outfile)
        self._truncate(stderr)

        # Update with a new input
        self.stdin.buffer.write(updated_input)
        self.stdin.buffer.seek(0)
        with mock.patch('sys.stderr', stderr):
            lazysponge.main()
        third_stat = os.stat(self.outfile)
        with open(self.outfile, 'rb') as outfd:
            self.assertEqual(outfd.read(), updated_input)
        self._truncate(self.stdin.buffer)

        # File is newer
        self.assertGreater(third_stat, second_stat)
        # compare stderr verbose output
        errmsg = ('Replacing %s (96022020c795ee69653958a3cb4bb083'
                  ' != 1015f2c7f2fc3d575b7aeb1e92c0f6bf)\n')
        self.assertEqual(stderr.getvalue(), errmsg % self.outfile)
        self._truncate(stderr)

    @staticmethod
    def _truncate(filefd):
        filefd.seek(0)
        filefd.truncate(0)

    def test_no_tty_detection(self):
        """Test detecting that 'stdin' is not a tty."""
        self.isatty_ret = True
        stderr = StringIO()

        with mock.patch('sys.stderr', stderr):
            with self.assertRaises(SystemExit):
                lazysponge.main()

        not_a_tty = ('Interactive input not supported. Use piped input\n'
                     '  echo message | {}\n'.format(' '.join(self.argv)))
        self.assertEqual(stderr.getvalue(), not_a_tty)


if __name__ == '__main__':
    unittest.main()
