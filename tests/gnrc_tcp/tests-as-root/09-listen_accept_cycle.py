#!/usr/bin/env python3

# Copyright (C) 2021   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import os
import random

from shared_func import Runner, RiotTcpServer, HostTcpClient, generate_port_number, \
                        sudo_guard


@Runner(timeout=10)
def test_listen_accept_cycle(child, iterations=10):
    """ This test verifies gnrc_tcp in a typical server role by
        accepting a connection, exchange data, teardown connection, handle the next one
    """
    # Setup RIOT Node as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Establish multiple connections iterativly
        for i in range(iterations):
            print('    Running iteration {}'.format(i))

            with HostTcpClient(riot_srv.addr, riot_srv.listen_port) as host_cli:
                # Accept connection from host system
                riot_srv.accept(timeout_ms=0)

                # Send data from host to RIOT
                data = '0123456789'
                host_cli.send(payload_to_send=data)
                riot_srv.receive(timeout_ms=500, sent_payload=data)

                # Send data from RIOT to host
                riot_srv.send(timeout_ms=500, payload_to_send=data)
                host_cli.receive(sent_payload=data)

                # Randomize connection teardown: The connections
                # can't be either closed or aborted from either
                # side. Regardless type of the connection teardown
                # riot_srv must be able to accept the next connection
                # Note: python sockets don't offer abort...
                dice_throw = random.randint(0, 3)
                if dice_throw == 0:
                    riot_srv.close()
                    host_cli.close()

                elif dice_throw == 1:
                    riot_srv.abort()
                    host_cli.close()

                elif dice_throw == 2:
                    host_cli.close()
                    riot_srv.close()

                elif dice_throw == 3:
                    host_cli.close()
                    riot_srv.abort()


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
