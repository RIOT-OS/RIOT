#!/usr/bin/env python3

# Copyright (C) 2018   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from shared_func import Runner, RiotTcpServer, HostTcpClient, generate_port_number, \
                        sudo_guard


@Runner(timeout=10)
def test_lifecycle_as_server(child):
    """ Open/close a single connection as tcp server """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Setup Host as client
        with HostTcpClient(riot_srv.addr, riot_srv.listen_port):
            # Accept and close connection
            riot_srv.accept(timeout_ms=1000)
            riot_srv.close()


if __name__ == '__main__':
    sudo_guard()

    # Read and run all test functions.
    script = sys.modules[__name__]
    tests = [getattr(script, t) for t in script.__dict__
             if type(getattr(script, t)).__name__ == 'function'
             and t.startswith('test_')]

    for test in tests:
        res = test()
        if (res != 0):
            sys.exit(res)

    print(os.path.basename(sys.argv[0]) + ': success\n')
