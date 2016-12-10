#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.

class Board(object):
    def __init__(self, name, port=None, serial=None):
        self.name = name
        self.port = port
        self.serial = serial

    def __repr__(self):
        return ("<Board %s,port=%s,serial=%s>" %
                (repr(self.name),repr(self.port),repr(self.serial)))

    def to_env(self):
        env = {}
        if self.name:
            env['BOARD'] = self.name
        if self.port:
            env['PORT'] = self.port
        if self.serial:
            env['SERIAL'] = self.serial
        return env
