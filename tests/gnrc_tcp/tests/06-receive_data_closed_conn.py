#!/usr/bin/env python3

# Copyright (C) 2019   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import threading

from testrunner import run
from shared_func import TcpServer, generate_port_number, get_host_tap_device, \
                        get_host_ll_addr, get_riot_if_id, setup_internal_buffer, \
                        read_data_from_internal_buffer, verify_pktbuf_empty, \
                        sudo_guard


def tcp_server(port, shutdown_event, data):
    with TcpServer(port, shutdown_event) as tcp_srv:
        tcp_srv.send(data)


def testfunc(child):
    port = generate_port_number()
    shutdown_event = threading.Event()

    # Try to receive 10 bytes sent from the Host System.
    data = 'test_data_'
    data_len = len(data)

    assert (data_len % 2) == 0

    # Verify that RIOT Applications internal buffer can hold test data.
    assert setup_internal_buffer(child) >= data_len

    server_handle = threading.Thread(target=tcp_server, args=(port, shutdown_event, data))
    server_handle.start()

    target_addr = get_host_ll_addr(get_host_tap_device()) + '%' + get_riot_if_id(child)

    # Setup RIOT Node to connect to Hostsystems TCP Server
    child.sendline('gnrc_tcp_tcb_init')
    child.sendline('gnrc_tcp_open_active [{}]:{} 0'.format(target_addr, str(port)))
    child.expect_exact('gnrc_tcp_open_active: returns 0')

    # Initiate connection teardown from test host
    shutdown_event.set()

    half_data_len = int(data_len / 2)

    # Read half the test data with timeout. Verify Buffer contents
    child.sendline('gnrc_tcp_recv 1000000 ' + str(half_data_len))
    child.expect_exact('gnrc_tcp_recv: received ' + str(half_data_len))
    assert read_data_from_internal_buffer(child, half_data_len) == data[:half_data_len]

    # Read half the test data without timeout. Verify Buffer contents
    child.sendline('gnrc_tcp_recv 0 ' + str(half_data_len))
    child.expect_exact('gnrc_tcp_recv: received ' + str(half_data_len))
    assert read_data_from_internal_buffer(child, half_data_len) == data[half_data_len:]

    # Buffer should have been read entirely and the connection was closed, there can be no new data.
    # Reading with a timeout must return 0 not -ETIMEOUT
    child.sendline('gnrc_tcp_recv 1000000 ' + str(half_data_len))
    child.expect_exact('gnrc_tcp_recv: returns 0')

    # Reading without a timeout must return 0 not -EAGAIN.
    child.sendline('gnrc_tcp_recv 0 ' + str(half_data_len))
    child.expect_exact('gnrc_tcp_recv: returns 0')

    # Close connection
    child.sendline('gnrc_tcp_close')
    server_handle.join()

    verify_pktbuf_empty(child)

    # Verify received Data
    print(os.path.basename(sys.argv[0]) + ': success')


if __name__ == '__main__':
    sudo_guard()
    sys.exit(run(testfunc, timeout=5, echo=False, traceback=True))
