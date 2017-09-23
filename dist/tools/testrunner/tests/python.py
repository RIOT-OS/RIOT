#!/usr/bin/env python3

# Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import tests.map

from .base import BaseTest

class PythonTest(BaseTest):
    name = "python script test class"
    def __init__(s, nodes, args=None):
        args = args or {}
        super().__init__(nodes, args=args, min_nodes=1)

        s.result = 0

    def run(s):
        filename = s.args.get("python_file")
        if filename:
            with open(filename, 'r') as f:
                code = f.read()

        else:
            filename = 'nofile'

            code = s.args["python"]
            if type(code) == list:
                code = "\n".join(code)

        compiled = compile(code, filename, 'exec')

        exec(compiled, { "test" : s })

        for node in s.nodes:
            node.close()

tests.map.register("python", PythonTest)
