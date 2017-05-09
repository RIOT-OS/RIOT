#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.

import os, sys
import math
import re
import signal
import pexpect

DEFAULT_TIMEOUT = 1

def get_node(port=""):
    if port != "":
        os.environ['PORT'] = port
    return pexpect.spawn("make reset term", timeout=DEFAULT_TIMEOUT)

def get_nodes(ports=[]):
    if not ports:
        for i in range(1):      # needs to be in loop to be a generator
            yield get_node()
    else:
        for port in ports:
            yield get_node(port)

def wait_init(node):
    node.expect("This is RIOT!")
    node.expect_exact("> ")

def get_ll_addr(node):
    node.send("ifconfig\n")
    node.expect(r"inet6 addr: (?P<ll_addr>fe80::[0-9a-f:]+)/64")
    return node.match.group('ll_addr')

def setup_server(node, protocol):
    node.send("ip server start %d\n" % protocol)
    node.expect_exact("Success: started IP server on protocol %d" % protocol)
    return get_ll_addr(node)

def send_packet(node, dst_addr, protocol, data):
    data_len = int(math.ceil(len(re.findall(r'[0-9a-fA-F]', data)) * 0.5))
    node.send("ip send %s %d \"%s\"\n" % (dst_addr, protocol, data))
    node.expect_exact("Success: send %d byte to %s (next header: %d)" % \
                      (data_len, dst_addr, protocol))

def check_received(node, exps):
    for exp in exps:
        node.expect(exp)

