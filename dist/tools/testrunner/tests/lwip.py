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
import tests.shell

class LwipTest(tests.shell.ShellTest):
    name = "lwip test class"
    def __init__(s, nodes, args=None):
        default_args = {
                "timeout" : 10,
                }

        if args:
            default_args.update(args)
        args = default_args

        super().__init__(nodes, args=args, min_nodes=len(nodes))

        s.timeout = args["timeout"]

    def run(s):
        s.launch_nodes()

        return s

tests.map.register("lwip", LwipTest)
