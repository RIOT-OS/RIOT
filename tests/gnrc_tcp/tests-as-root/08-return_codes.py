#!/usr/bin/env python3

# Copyright (C) 2021   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import os

from shared_func import Runner, RiotTcpServer, HostTcpClient, generate_port_number, \
                        sudo_guard


@Runner(timeout=0.5)
def test_gnrc_tcp_accept_returns_EAGAIN(child):
    """ gnrc_tcp_accept must return with -EAGAIN
        if no connection is ready and timeout is 0
    """
    riot_srv = RiotTcpServer(child, generate_port_number())
    riot_srv.listen()

    child.sendline('gnrc_tcp_accept 0')
    child.expect_exact('gnrc_tcp_accept: returns -EAGAIN')


@Runner(timeout=1.5)
def test_gnrc_tcp_accept_returns_ETIMEDOUT(child):
    """ gnrc_tcp_accept must return with -ETIMEDOUT
        if no connection is ready and timeout is not 0
    """
    riot_srv = RiotTcpServer(child, generate_port_number())
    riot_srv.listen()

    child.sendline('gnrc_tcp_accept 1000')
    child.expect_exact('gnrc_tcp_accept: returns -ETIMEDOUT')


@Runner(timeout=1)
def test_gnrc_tcp_accept_returns_ENOMEM(child):
    """ gnrc_tcp_accept must return with -ENOMEM
        if all TCBs already handle a connection
    """
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Establish connection to ensure that all TCBs are in use
        with HostTcpClient(riot_srv.addr, riot_srv.listen_port):
            riot_srv.accept(timeout_ms=0)

            # Faulty accept should return immediately despite a huge timeout.
            child.sendline('gnrc_tcp_accept 100000000')
            child.expect_exact('gnrc_tcp_accept: returns -ENOMEM')


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
