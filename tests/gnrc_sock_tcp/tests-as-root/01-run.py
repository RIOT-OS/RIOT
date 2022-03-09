#!/usr/bin/env python3

# Copyright (C) 2021   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import random

from helpers import Runner, SockTcpServer, SockTcpClient, HostTcpServer, HostTcpClient, \
                    generate_port_number, sudo_guard


@Runner(timeout=5)
def test_connection_lifecycle_as_client(child):
    """ Open/close a single connection as sock tcp client """
    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot Node as client
        local_port = 54321
        with SockTcpClient(child, host_srv, local_port) as sock_cli:
            # Accept, verify Endpoints and close connection
            host_srv.accept()

            # Verify connection endpoints from sock perspective
            sock_cli.get_local()
            child.expect_exact('Endpoint: addr.ipv6={} netif={} port={}'.format(
                sock_cli.address, sock_cli.interface, sock_cli.local_port)
            )

            sock_cli.get_remote()
            child.expect_exact('Endpoint: addr.ipv6={} netif=0 port={}'.format(
                host_srv.address, host_srv.listen_port)
            )

            # Close Connection
            host_srv.close()


@Runner(timeout=10)
def test_connection_lifecycle_as_server(child):
    """ Open/close a single connection as sock tcp server """
    # Setup Riot Node as server
    with SockTcpServer(child, generate_port_number()) as sock_srv:

        # Verify listen parameters
        sock_srv.queue_get_local()
        child.expect_exact('Endpoint: addr.ipv6=:: netif=0 port={}'.format(
            sock_srv.listen_port)
        )

        # Setup Host as client
        with HostTcpClient(sock_srv) as host_cli:
            # Accept connection
            sock_srv.accept(timeout_ms=1000)

            # Verify connection endpoints from sock perspective
            sock_srv.get_local()
            child.expect_exact('Endpoint: addr.ipv6={} netif={} port={}'.format(
                sock_srv.address, sock_srv.interface, sock_srv.listen_port)
            )

            sock_srv.get_remote()
            child.expect_exact('Endpoint: addr.ipv6={}'.format(host_cli.address))

            # Close connection
            sock_srv.disconnect()


@Runner(timeout=5)
def test_send_data_from_riot_to_host(child):
    """ Send Data from RIOT Node to Host system """
    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot as client
        with SockTcpClient(child, host_srv) as sock_cli:
            # Accept and close connection
            host_srv.accept()

            # Send Data from RIOT to Host system and verify reception
            data = '0123456789'
            sock_cli.write(data)
            host_srv.receive(data)

            # Teardown connection
            host_srv.close()


@Runner(timeout=5)
def test_send_data_from_host_to_riot(child):
    """ Send Data from Host system to RIOT node """
    # Setup Riot Node as server
    with SockTcpServer(child, generate_port_number()) as sock_srv:
        # Setup Host as client
        with HostTcpClient(sock_srv) as host_cli:
            # Accept and close connection
            sock_srv.accept(timeout_ms=1000)

            # Send Data from Host system to RIOT
            data = '0123456789'
            host_cli.send(data)
            sock_srv.read(data, 1000)

            sock_srv.disconnect()


@Runner(timeout=10)
def test_connection_listen_accept_cycle(child, iterations=10):
    """ This test verifies sock_tcp in a typical server role by
        accepting a connection, exchange data, teardown connection, handle the next one
    """
    # Setup RIOT Node as server
    with SockTcpServer(child, generate_port_number()) as sock_srv:
        # Establish multiple connections iterativly
        for i in range(iterations):
            print('\n    Running listen/accept iteration {}'.format(i), end='')

            with HostTcpClient(sock_srv) as host_cli:
                # Accept connection from host system
                sock_srv.accept(timeout_ms=0)

                # Send data from host to RIOT
                data = '0123456789'
                host_cli.send(data)
                sock_srv.read(data, 1000)

                # Send data from RIOT to host
                sock_srv.write(data)
                host_cli.receive(data)

                # Randomize connection teardown: The connections
                # can be closed from either Regardless the type of the connection teardown
                # sock_srv must be able to accept the next connection
                dice_throw = random.randint(0, 1)
                if dice_throw == 0:
                    sock_srv.disconnect()
                    host_cli.close()

                elif dice_throw == 1:
                    host_cli.close()
                    sock_srv.disconnect()


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

    print('\n' + os.path.basename(sys.argv[0]) + ': success\n')
