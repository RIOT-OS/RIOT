#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

from __future__ import print_function

class TermColor(object):
    red = '\033[1;31m'
    green = '\033[1;32m'
    yellow = '\033[1;33m'
    blue = '\033[1;34m'
    purple = '\033[1;35m'
    end = '\033[0m'

def colorize(string, color):
    return '%s%s%s' % (color, string, TermColor.end)

class Board(object):
    def __init__(self, name, port=None, serial=None):
        self.name = name
        self.port = port
        self.serial = serial

    def to_env(self):
        env = {}
        if self.name:
            env['BOARD'] = self.name
        if self.port:
            env['PORT'] = self.port
        if self.serial:
            env['SERIAL'] = self.serial
        return env

class TestCase(object):
    def __init__(self, name, build_strategy = None, test_strategy = None):
        self.name = name
        if build_strategy:
            self.build_action = build_strategy
        else:
            self.build_action = None

        if test_strategy:
            self.test_action = test_strategy
        else:
            self.test_action = None

    def build(self):
        if self.build_action:
            print(' - {} '.format(colorize('Building', TermColor.purple)), end='')
            if self.build_action.build(self.name):
                print('{}'.format(colorize('successfull', TermColor.green)))
                return True
            else:
                print('{}'.format(colorize('failed', TermColor.red)))
                return False

    def test(self):
        if self.test_action:
            print(' - {} '.format(colorize('Testing', TermColor.purple)), end='')
            if self.test_action.test(self.name):
                print('{}'.format(colorize('successfull', TermColor.green)))
                return True
            else:
                print('{}'.format(colorize('failed', TermColor.red)))
                return False

    def execute(self):
        print('Executing: {}'.format(colorize(self.name, TermColor.blue)))
        if self.build():
            return self.test()
        return False
