#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2016 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import pexpect
from subprocess import check_output
from subprocess import STDOUT
from subprocess import CalledProcessError
import shlex

class DefaultBuildStrategy:
    def __init__(self, options=None):
        self.options = options

    def build(self, test_name, options):
        try:
            cmd = 'make -C ../{0} -B clean all BOARD={1}'.format(test_name, options['board'])
            print('({}) '.format(cmd), end='')
            make = check_output(shlex.split(cmd), universal_newlines=True, stderr=STDOUT)
            return True
        except CalledProcessError as makeexc:
            #print('error code {}'.format(makeexc.returncode))# makeexc.output
            #print(makeexc.output)
            return False

class DefaultTestStrategy:
    def __init__(self, options=None):
        self.options = options

    def test(self, test_name, options):
        flash = ''
        if not options['board'] == 'native':
            flash = 'flash '
        cmd = 'make -C ../{0} {1}term BOARD={2}'.format(test_name, flash, options['board'])
        print('({}) '.format(cmd), end='')
        make = pexpect.spawn(cmd)
        if make.expect([pexpect.TIMEOUT, "TEST: SUCCESS"], timeout=60) != 0:
            return True
        close(make)
        return False
