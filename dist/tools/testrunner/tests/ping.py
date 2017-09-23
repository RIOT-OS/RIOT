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
from .shell import ShellTest, get_link_local_addr, get_interfaces

class PingTest(ShellTest):
    name = "ping test class"
    def __init__(s, nodes, args=None):
        default_args = {
                "target" : "link-local",
                "count" : 10,
                "payload_len" : 0,
                "delay" : 100
                }

        default_args.update(args or {})

        args = default_args

        super().__init__(nodes, args=args, min_nodes=2)

        s.target = args.get("target")
        s.count = args.get("count")
        s.payload_len = args.get("payload_len")
        s.delay = args.get("delay")

        s.timeout = s.count * s.delay * 1.2
        s.result = 0

    def run(s):
        pinger = s.nodes[0].child
        pingee = s.nodes[1].child

        if s.target=="link-local":
            addr = get_link_local_addr(pingee)

            if not addr:
                raise Exception("no link-local address found")

            # get first interface's interface number if pinging link-local
            interfaces = get_interfaces(pinger)
            if not interfaces:
                print("pinger: cannot get interfaces!")
                s.result = 1
                return

            ifnum, _ = interfaces[0]

            target = "%s%%%s" % (addr, ifnum)

        else:
            target = s.target
            addr = target

        print("pinger: pinging", target)

        pinger.sendline("ping6 %i %s %i %i" % (s.count, target, s.payload_len, s.delay))

        while pinger.expect(["--- %s ping statistics ---" % addr, "\d+ bytes from"], timeout = max(s.timeout, s.delay*2)):
            pass

        pinger.expect("(\d+) packets transmitted, (\d+) received, (\d)% packet loss, time (\d+\.\d+) s")

        s.set_results({
                "test" : "ping",
                "results" : {
                    "tx" : int(pinger.match.group(1)),
                    "rx" : int(pinger.match.group(2)),
                    "loss" : int(pinger.match.group(3)),
                    "time" : float(pinger.match.group(4)),
                    },
                })


        print("pinger: done")

        for node in s.nodes:
            node.close()

tests.map.register("ping", PingTest)
