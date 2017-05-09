#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.

from __future__ import print_function
import sys, os
import helpers
import socket
import pexpect

PROTOCOL = socket.IPPROTO_UDP

def main(ports):
    if len(ports) < 2:
        print("Need at least to nodes", file=sys.stderr)
        sys.exit(1)
    nodes = list(helpers.get_nodes(ports))

    try:
        helpers.wait_init(nodes[0])
        dst_addr = helpers.get_ll_addr(nodes[0])
        helpers.setup_server(nodes[0], PROTOCOL)
        for node in nodes[1:]:
            # ports abcd, length 8, checksum 0x1234 (unchecked anyways, because these are raw conns)
            helpers.send_packet(node, dst_addr, PROTOCOL, "abcdabcd00081234")
            helpers.check_received(nodes[0], ["000000 ab cd ab cd 00 08 12 34"])
            # ports 1234 and 4567, length 9, checksum 0x1234
            # (unchecked anyways, because these are raw conns)
            helpers.send_packet(node, dst_addr, PROTOCOL, "01234567001112340123456789abcdefab")
            helpers.check_received(nodes[0],
                                   ["000000 01 23 45 67 00 11 12 34 01 23 45 67 89 ab cd ef", 
                                    "000010 ab"])
    finally:
        for node in nodes:
            pexpect.run("kill %d" % node.pid) # only way to also end all childs of `node`

if __name__ == "__main__":
    old_port = os.environ.get('PORT')
    main(sys.argv[1:])
    if old_port:
        os.environ['PORT'] = old_port
    elif 'PORT' in os.environ:
        del os.environ['PORT']
