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
from pexpect import TIMEOUT

from util import listify

class ExpectTest(tests.shell.ShellTest):
    name = "expect test class"
    def __init__(s, nodes, args=None):
        default_args = {
                "terms" : "TEST SUCCESSFUL",
                "timeout" : 10,
                "shell" : False,
                }

        if args:
            default_args.update(args)
        args = default_args

        super().__init__(nodes, args=args, min_nodes=len(nodes))

        s.terms = listify(args["terms"])
        s.timeout = args["timeout"]

    def run(s):
        if s.network:
            s.network.setup_all(s.nodes)

        if len(s.nodes) == 1:
            terms = []
            for term in s.terms:
                terms.append((0, term))
            s.terms = terms
        else:
            for terms in s.terms:
                if len(terms) != 2:
                    raise Exception("ExpectTest(): terms need to be lists: [ <node_num>, <term> ]")

        for i, terms in enumerate(s.terms):
            nodenum, term = terms
            child = s.nodes[nodenum].child
            if not len(term):
                continue

            try:
                if term[0] == '>':
                    child.sendline(term[1:])
                elif term[0] == '<':
                    child.expect(term[1:], timeout=s.timeout)
                else:
                    child.expect_exact(term, timeout=s.timeout)
            except TIMEOUT as e:
                print("TIMEOUT while processing term %s (\"%s\")." % (i, term))
                raise e

        s.result = 0

        return s

tests.map.register("expect", ExpectTest)
