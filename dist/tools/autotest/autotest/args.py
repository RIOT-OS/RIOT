#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.

import argparse
from autotest.utils import Borg
import autotest.board
import sys

def _string_to_dict(string):
    tmp = string.split(",")
    tmp = [(key.strip(), value.strip()) for key, value in [t.split('=', 1) for t in tmp]]
    return dict(tmp)

class ArgBoard(autotest.board.Board):
    def __init__(self, string):
        if string.startswith("name="):
            tmp = _string_to_dict(string)
        else:
            tmp = _string_to_dict("name="+string)
        super(ArgBoard, self).__init__(**tmp)

class ArgParser(Borg):
    def __init__(self):
        super(ArgParser, self).__init__()
        if not hasattr(self, 'parser'):
            self.parser = argparse.ArgumentParser()
            self.parser.add_argument("-b", "--board", action='append', type=ArgBoard)
            self.ns = self.parser.parse_args(sys.argv[1:])
            if not hasattr(self.ns, "board") or not self.ns.board:
                # due to action='append' it is not possible to set default
                # see: https://bugs.python.org/issue16399
                setattr(self.ns, 'board', [ArgBoard("name=native")])

    @property
    def boards(self):
        return self.ns.board

