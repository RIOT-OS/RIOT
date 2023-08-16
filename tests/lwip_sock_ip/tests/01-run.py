#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def _ipv6_tests(code):
    return code & (1 << 6)


def _ipv4_tests(code):
    return code & (1 << 4)


def testfunc(child):
    child.expect(r"code (0x[0-9a-f]{2}\s)")
    code = int(child.match.group(1), base=16)
    if _ipv4_tests(code):
        child.expect_exact(u"Calling test_sock_ip_create4__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_ip_create4__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_ip_create4__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_ip_create4__no_endpoints()")
        child.expect_exact(u"Calling test_sock_ip_create4__only_local()")
        child.expect_exact(u"Calling test_sock_ip_create4__only_local_reuse_ep()")
        child.expect_exact(u"Calling test_sock_ip_create4__only_remote()")
        child.expect_exact(u"Calling test_sock_ip_create4__full()")
        child.expect_exact(u"Calling test_sock_ip_recv4__EADDRNOTAVAIL()")
        child.expect_exact(u"Calling test_sock_ip_recv4__EAGAIN()")
        child.expect_exact(u"Calling test_sock_ip_recv4__ENOBUFS()")
        child.expect_exact(u"Calling test_sock_ip_recv4__ETIMEDOUT()")
        child.expect_exact(u" * Calling sock_ip_recv()")
        child.expect(r" \* \(timed out with timeout \d+\)")
        child.expect_exact(u"Calling test_sock_ip_recv4__socketed()")
        child.expect_exact(u"Calling test_sock_ip_recv4__socketed_with_remote()")
        child.expect_exact(u"Calling test_sock_ip_recv4__unsocketed()")
        child.expect_exact(u"Calling test_sock_ip_recv4__unsocketed_with_remote()")
        child.expect_exact(u"Calling test_sock_ip_recv4__with_timeout()")
        child.expect_exact(u"Calling test_sock_ip_recv4__non_blocking()")
        child.expect_exact(u"Calling test_sock_ip_send4__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_ip_send4__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_ip_send4__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_ip_send4__ENOTCONN()")
        child.expect_exact(u"Calling test_sock_ip_send4__socketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send4__socketed_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send4__socketed_no_local()")
        child.expect_exact(u"Calling test_sock_ip_send4__socketed()")
        child.expect_exact(u"Calling test_sock_ip_send4__socketed_other_remote()")
        child.expect_exact(u"Calling test_sock_ip_send4__unsocketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send4__unsocketed_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send4__unsocketed_no_local()")
        child.expect_exact(u"Calling test_sock_ip_send4__unsocketed()")
        child.expect_exact(u"Calling test_sock_ip_send4__no_sock_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send4__no_sock()")
    if _ipv6_tests(code):
        child.expect_exact(u"Calling test_sock_ip_create6__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_ip_create6__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_ip_create6__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_ip_create6__no_endpoints()")
        child.expect_exact(u"Calling test_sock_ip_create6__only_local()")
        child.expect_exact(u"Calling test_sock_ip_create6__only_local_reuse_ep()")
        child.expect_exact(u"Calling test_sock_ip_create6__only_remote()")
        child.expect_exact(u"Calling test_sock_ip_create6__full()")
        child.expect_exact(u"Calling test_sock_ip_recv6__EADDRNOTAVAIL()")
        child.expect_exact(u"Calling test_sock_ip_recv6__EAGAIN()")
        child.expect_exact(u"Calling test_sock_ip_recv6__ENOBUFS()")
        child.expect_exact(u"Calling test_sock_ip_recv6__ETIMEDOUT()")
        child.expect_exact(u" * Calling sock_ip_recv()")
        child.expect(r" \* \(timed out with timeout \d+\)")
        child.expect_exact(u"Calling test_sock_ip_recv6__socketed()")
        child.expect_exact(u"Calling test_sock_ip_recv6__socketed_with_remote()")
        child.expect_exact(u"Calling test_sock_ip_recv6__unsocketed()")
        child.expect_exact(u"Calling test_sock_ip_recv6__unsocketed_with_remote()")
        child.expect_exact(u"Calling test_sock_ip_recv6__with_timeout()")
        child.expect_exact(u"Calling test_sock_ip_recv6__non_blocking()")
        child.expect_exact(u"Calling test_sock_ip_send6__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_ip_send6__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_ip_send6__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_ip_send6__ENOTCONN()")
        child.expect_exact(u"Calling test_sock_ip_send6__socketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send6__socketed_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send6__socketed_no_local()")
        child.expect_exact(u"Calling test_sock_ip_send6__socketed()")
        child.expect_exact(u"Calling test_sock_ip_send6__socketed_other_remote()")
        child.expect_exact(u"Calling test_sock_ip_send6__unsocketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send6__unsocketed_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send6__unsocketed_no_local()")
        child.expect_exact(u"Calling test_sock_ip_send6__unsocketed()")
        child.expect_exact(u"Calling test_sock_ip_send6__no_sock_no_netif()")
        child.expect_exact(u"Calling test_sock_ip_send6__no_sock()")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")


if __name__ == "__main__":
    sys.exit(run(testfunc))
