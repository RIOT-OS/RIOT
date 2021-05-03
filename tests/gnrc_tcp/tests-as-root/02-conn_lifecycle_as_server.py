#!/usr/bin/env python3

# Copyright (C) 2018   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
import socket
import threading

from testrunner import run
from shared_func import generate_port_number, get_host_tap_device, get_riot_ll_addr, \
                        verify_pktbuf_empty, sudo_guard


def tcp_client(addr, port, shutdown_event):
    sock = socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    addr_info = socket.getaddrinfo(addr + '%' + get_host_tap_device(), port, type=socket.SOCK_STREAM)

    sock.connect(addr_info[0][-1])

    shutdown_event.wait()

    sock.close()


def testfunc(child):
    port = generate_port_number()
    shutdown_event = threading.Event()

    client_handle = threading.Thread(target=tcp_client, args=(get_riot_ll_addr(child), port, shutdown_event))

    # Setup RIOT Node wait for incoming connections from host system
    child.sendline('gnrc_tcp_tcb_init')
    child.sendline('gnrc_tcp_open_passive [::]:{}'.format(str(port)))

    client_handle.start()
    child.expect_exact('gnrc_tcp_open_passive: returns 0')

    # Close connection and verify that pktbuf is cleared
    shutdown_event.set()
    child.sendline('gnrc_tcp_close')
    client_handle.join()

    verify_pktbuf_empty(child)

    print(os.path.basename(sys.argv[0]) + ': success')


if __name__ == '__main__':
    sudo_guard()
    sys.exit(run(testfunc, timeout=7, echo=False, traceback=True))
