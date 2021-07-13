#!/usr/bin/env python3

# Copyright (C) 2021   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import random
import pexpect
import base64

from scapy.all import Ether, IPv6, TCP, raw, sendp

from helpers import Runner, RiotTcpServer, RiotTcpClient, HostTcpServer, HostTcpClient, \
                    generate_port_number, sudo_guard


@Runner(timeout=5)
def test_connection_lifecycle_as_client(child):
    """ Open/close a single connection as tcp client """
    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot as client
        with RiotTcpClient(child, host_srv):
            # Accept and close connection
            host_srv.accept()
            host_srv.close()


@Runner(timeout=10)
def test_connection_lifecycle_as_server(child):
    """ Open/close a single connection as tcp server """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Setup Host as client
        with HostTcpClient(riot_srv):
            # Accept and close connection
            riot_srv.accept(timeout_ms=1000)
            riot_srv.close()


@Runner(timeout=5)
def test_send_data_from_riot_to_host(child):
    """ Send Data from RIOT Node to Host system """
    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot as client
        with RiotTcpClient(child, host_srv) as riot_cli:
            # Accept and close connection
            host_srv.accept()

            # Send Data from RIOT to Host system and verify reception
            data = '0123456789' * 200
            riot_cli.send(timeout_ms=0, payload_to_send=data)
            host_srv.receive(data)

            # Teardown connection
            host_srv.close()


@Runner(timeout=5)
def test_send_data_from_host_to_riot(child):
    """ Send Data from Host system to RIOT node """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Setup Host as client
        with HostTcpClient(riot_srv) as host_cli:
            # Accept and close connection
            riot_srv.accept(timeout_ms=1000)

            # Send Data from Host system to RIOT
            data = '0123456789' * 200
            host_cli.send(data)
            riot_srv.receive(timeout_ms=1000, sent_payload=data)

            riot_srv.close()


@Runner(timeout=5)
def test_gnrc_tcp_garbage_packets_short_payload(child):
    """ Receive unusually short payload with timeout. Verifies fix for
        https://github.com/RIOT-OS/RIOT/issues/11999
    """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Setup Host as client
        with HostTcpClient(riot_srv) as host_cli:
            # Accept new connection
            riot_srv.accept(timeout_ms=2000)

            # Receive 1 byte with timeout, this should block
            child.sendline('gnrc_tcp_recv 1000000 1')
            child.expect_exact('gnrc_tcp_recv: argc=3, '
                               'argv[0] = gnrc_tcp_recv, '
                               'argv[1] = 1000000, argv[2] = 1')

            # Send 1 byte from host to RIOT
            assert 1 == host_cli.sock.send(b"f")

            # Receive 1 byte
            child.expect_exact('gnrc_tcp_recv: received 1', timeout=20)

            # Close connection
            riot_srv.close()


@Runner(timeout=5)
def test_gnrc_tcp_garbage_packets_short_header(child):
    """ This test verifies fix malformed tcp header. See
        https://github.com/RIOT-OS/RIOT/issues/12086
    """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:

        # Construct HostTcpClient to lookup node properties
        host_cli = HostTcpClient(riot_srv)

        # Try to accept incoming connection from host system.
        child.sendline('gnrc_tcp_accept 2000')

        # Build malformed short header with SYN Flag
        tcp_hdr = TCP(
            dport=int(riot_srv.listen_port), flags="S", sport=2342, seq=1, dataofs=6
        )
        tcp_hdr = raw(tcp_hdr)[:-2]
        sendp(
            Ether(dst=riot_srv.mac) / IPv6(src=host_cli.address, dst=riot_srv.address) /
            tcp_hdr, iface=host_cli.interface, verbose=0
        )

        # Establish normal tcp connection from host system to check if RIOT node
        # was able to recover from malformed packet
        with host_cli:
            child.expect_exact('gnrc_tcp_accept: returns 0')

        riot_srv.close()


@Runner(timeout=5)
def test_gnrc_tcp_garbage_packets_ack_instead_of_sym(child):
    """ This test verfies that sending and ACK instead of a SYN.
        doesn't break GNRC_TCP.
    """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Construct HostTcpClient to lookup node properties
        host_cli = HostTcpClient(riot_srv)

        # Try to accept incoming connection from host system.
        # Use timeout of 15s discarding 1000 packages can take a while on smaller platforms
        child.sendline('gnrc_tcp_accept 15000')

        # Check if debug output is enabled. Send fewer packets on if it is disabled
        # To ensure that the amount of generated output doesn't break the test
        debug = child.expect(
            [pexpect.TIMEOUT, r'GNRC_TCP: Enter "\S+", File: .+\(\d+\)\s'], timeout=1
        )

        if debug:
            count = 10
        else:
            count = 1000

        # see https://github.com/RIOT-OS/RIOT/pull/12001
        provided_data = base64.b64decode("rwsQf2pekYLaU+exUBBwgPDKAAA=")
        tcp_hdr = TCP(provided_data)
        assert provided_data == raw(tcp_hdr)

        # set destination port to application specific port
        tcp_hdr.dport = int(riot_srv.listen_port)
        sendp(
            Ether(dst=riot_srv.mac) / IPv6(src=host_cli.address, dst=riot_srv.address) /
            tcp_hdr, iface=host_cli.interface, verbose=0, count=count
        )

        # check if server actually still works
        with host_cli:
            child.expect_exact('gnrc_tcp_accept: returns 0')

        riot_srv.close()


@Runner(timeout=5)
def test_gnrc_tcp_garbage_packets_option_parsing(child):
    """ This test verfies that malformed option don't break TCP
        doesn't break GNRC_TCP. See: https://github.com/RIOT-OS/RIOT/issues/12086
    """
    # Setup RIOT as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Construct HostTcpClient to lookup node properties
        host_cli = HostTcpClient(riot_srv)

        # Try to accept incoming connection from host system.
        child.sendline('gnrc_tcp_accept 2000')

        tcp_hdr = TCP(
            dport=int(riot_srv.listen_port), flags="S", sport=2342, seq=1, dataofs=6
        )

        sendp(
            Ether(dst=riot_srv.mac) / IPv6(src=host_cli.address, dst=riot_srv.address) /
            tcp_hdr / b"\x50\x00\x00\x00", iface=host_cli.interface, verbose=0
        )

        # check if server actually still works
        with host_cli:
            child.expect_exact('gnrc_tcp_accept: returns 0')

        riot_srv.close()


@Runner(timeout=5)
def test_gnrc_tcp_recv_behavior_on_closed_connection(child):
    """ This test ensures that a gnrc_tcp_recv doesn't block if a connection
        was closed already.
    """
    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot as client
        with RiotTcpClient(child, host_srv) as riot_cli:
            # Accept connection
            host_srv.accept()

            # Transmit data to RIOT client and close connection from host side
            data = 'test_data_'
            host_srv.send(payload_to_send=data)
            host_srv.close()

            # Read half amount of data with huge timeout.
            # Expectency: direct with verified test data. Timeout doesn't matter
            half_data_len = int(len(data) / 2)
            huge_timeout_ms = 1000000000
            no_timeout_ms = 0
            riot_cli.receive(timeout_ms=huge_timeout_ms, sent_payload=data[:half_data_len])

            # Read half amount of data without timeout.
            # Expectency: direct return with verified test data
            riot_cli.receive(timeout_ms=no_timeout_ms, sent_payload=data[half_data_len:])

            # All received data is read, try to read 1 byte with timeout.
            # Expectency: fast return despite timeout since the connection is already closed
            child.sendline('gnrc_tcp_recv {} 1'.format(huge_timeout_ms))
            child.expect_exact('gnrc_tcp_recv: returns 0')

            # All received data is read, try to read 1 byte without timeout.
            # Expectency: fast return since the connection is already closed
            child.sendline('gnrc_tcp_recv {} 1'.format(no_timeout_ms))
            child.expect_exact('gnrc_tcp_recv: returns 0')


@Runner(timeout=1)
def test_gnrc_tcp_ep_from_str(child):
    """ Verify Endpoint construction from string """
    # Build funspec
    valid = '[::]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: ::')
    child.expect_exact('Port: 0')
    child.expect_exact('Netif: 0')

    # Build unspec with port
    valid = '[::]:8080'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: ::')
    child.expect_exact('Port: 8080')
    child.expect_exact('Netif: 0')

    # Build unspec with interface and port
    valid = '[::%5]:8080'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: ::')
    child.expect_exact('Port: 8080')
    child.expect_exact('Netif: 5')

    # Build addr, interface and port
    valid = '[fe80::68bf:dbff:fe05:c35e%5]:8080'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: fe80::68bf:dbff:fe05:c35e')
    child.expect_exact('Port: 8080')
    child.expect_exact('Netif: 5')


@Runner(timeout=0.5)
def test_gnrc_tcp_ep_from_str_returns_EINVAL(child):
    """ Verify faulty endpoint construction from string return -EINVAL"""
    # Error: No brackets
    invalid = '::'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error: Too much brackets
    invalid = '[:]:]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error: Swapped bracket order
    invalid = ']::['
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error: Empty brackets
    invalid = '[]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error First char before brackets
    invalid = 'a[]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error token but no port
    invalid = '[::]:'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error invalid char in port
    invalid = '[::]:103f2'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error port out of range
    invalid = '[::]:65536'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error interface after bracket
    invalid = '[::]%5'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error interface token bur no interface
    invalid = '[::%]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error: invalid interface
    invalid = '[::%5a]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')

    # Error: address to short
    invalid = '[:%5]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


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
        with HostTcpClient(riot_srv):
            riot_srv.accept(timeout_ms=0)

            # Out of memory accept should return immediately despite a huge timeout.
            child.sendline('gnrc_tcp_accept 100000000')
            child.expect_exact('gnrc_tcp_accept: returns -ENOMEM')


@Runner(timeout=0.5)
def test_gnrc_tcp_accept_returns_EINVAL(child):
    """ gnrc_tcp_accept must return with -EINVAL
        if listen was not called before.
    """
    child.sendline('gnrc_tcp_accept 0')
    child.expect_exact('gnrc_tcp_accept: returns -EINVAL')


@Runner(timeout=5)
def test_gnrc_tcp_get_local_returns_0(child):
    """ This test verifies that get_local returns 0 in a connected state
        and the used endpoint contains the expected connection parameters
    """
    local_port = 30423

    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot as client
        with RiotTcpClient(child, host_srv, local_port) as riot_cli:

            # Accept connection
            host_srv.accept()

            # Get and verify local endpoint
            riot_cli.get_local()
            child.expect_exact('Endpoint: addr.ipv6={} netif={} port={}'.format(
                riot_cli.address, riot_cli.interface, riot_cli.port)
            )

            # Close connection
            host_srv.close()


@Runner(timeout=1)
def test_gnrc_tcp_get_local_returns_EADDRNOTAVAIL(child):
    """ This Test verifies that get_local returns -EADDRNOTAVAIL
        then not connected
    """
    child.sendline('gnrc_tcp_get_local')
    child.expect_exact('gnrc_tcp_get_local: returns -EADDRNOTAVAIL')


@Runner(timeout=5)
def test_gnrc_tcp_get_remote_returns_0(child):
    """ This test verifies that get_remote returns 0 in a connected state
        and the used endpoint contains the expected connection parameters
    """
    # Setup Host as server
    with HostTcpServer(generate_port_number()) as host_srv:
        # Setup Riot as client
        with RiotTcpClient(child, host_srv) as riot_cli:

            # Accept connection
            host_srv.accept()

            # Get and verify local endpoint
            riot_cli.get_remote()
            child.expect_exact('Endpoint: addr.ipv6={} netif=0 port={}'.format(
                host_srv.address, host_srv.listen_port)
            )

            # Close connection
            host_srv.close()


@Runner(timeout=1)
def test_gnrc_tcp_get_remote_returns_ENOTCONN(child):
    """ This test verifies that get_remote returns -ENOTCONN
        then not connected
    """
    child.sendline('gnrc_tcp_get_remote')
    child.expect_exact('gnrc_tcp_get_remote: returns -ENOTCONN')


@Runner(timeout=1)
def test_gnrc_tcp_queue_get_local_returns_0(child):
    """ This test verifies that queue_get_local returns 0 then put after listen.
        And the endpoint content is as are as expected.
    """
    # Enter listen with accept all address
    listen_addr = '::'
    listen_port = generate_port_number()
    riot_srv = RiotTcpServer(child, listen_port)
    riot_srv.listen()
    riot_srv.queue_get_local()
    child.expect_exact('Endpoint: addr.ipv6=:: netif=0 port={}'.format(
        listen_port)
    )
    riot_srv.stop_listen()

    # Enter listen with specified address
    listen_addr = 'fe80::4c49:c7ff:fecd:34a3'
    listen_port = generate_port_number()
    riot_srv = RiotTcpServer(child, listen_port, listen_addr)
    riot_srv.listen()
    riot_srv.queue_get_local()
    child.expect_exact('Endpoint: addr.ipv6={} netif=0 port={}'.format(
        listen_addr, listen_port)
    )
    riot_srv.stop_listen()


@Runner(timeout=1)
def test_gnrc_tcp_queue_get_local_returns_EADDRNOTAVAIL(child):
    """ This Test verifies that queue_get_local returns -EADDRNOTAVAIL
        then not listening
    """
    child.sendline('gnrc_tcp_queue_get_local')
    child.expect_exact('gnrc_tcp_queue_get_local: returns -EADDRNOTAVAIL')


@Runner(timeout=10)
def test_connection_listen_accept_cycle(child, iterations=10):
    """ This test verifies gnrc_tcp in a typical server role by
        accepting a connection, exchange data, teardown connection, handle the next one
    """
    # Setup RIOT Node as server
    with RiotTcpServer(child, generate_port_number()) as riot_srv:
        # Establish multiple connections iterativly
        for i in range(iterations):
            print('\n    Running listen/accept iteration {}'.format(i), end='')

            with HostTcpClient(riot_srv) as host_cli:
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
                # side. Regardless the type of the connection teardown
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
    sudo_guard(uses_scapy=True)

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
