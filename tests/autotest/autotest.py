#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from testCases import tests

class Termcolor:
    red = '\033[1;31m'
    green = '\033[1;32m'
    yellow = '\033[1;33m'
    blue = '\033[1;34m'
    purple = '\033[1;35m'
    end = '\033[0m'

def colorize(string, color):
    return '%s%s%s' % (color, string, Termcolor.end)

class TestCase:
    def __init__(self, test_name=None, build_strategy=None, test_strategy=None):
        self.test_name = test_name
        self.build_action = None
        self.test_action = None
        if build_strategy:
            self.build_action = build_strategy()
        if test_strategy:
            self.test_action = test_strategy()

    def build(self, board):
        if self.build_action:
            print('Building: {} '.format(colorize(self.test_name, Termcolor.blue)), end='')
            if self.build_action.build(self.test_name, board):
                print('{}'.format(colorize('successfull', Termcolor.green)))
            else:
                print('{}'.format(colorize('failed', Termcolor.red)))
        return

    def test(self, board):
        if self.test_action:
            print('Testing: {} '.format(colorize(self.test_name, Termcolor.blue)), end='')
            if self.test_action.test(self.test_name, board):
                print('{}'.format(colorize('successfull', Termcolor.green)))
            else:
                print('{}'.format(colorize('failed', Termcolor.red)))
        return

    def execute(self, board):
        self.build(board)
        self.test(board)
        return

def run_tests(tests):
    for test in tests:
        tc = TestCase(test_name=test[0], build_strategy=test[2], test_strategy=test[3])
        tc.execute(test[1])

if __name__ == '__main__':
    run_tests(tests)
