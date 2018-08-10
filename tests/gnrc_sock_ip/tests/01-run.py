#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact(u"Calling test_sock_ip_create__EAFNOSUPPORT()")
    child.expect_exact(u"Calling test_sock_ip_create__EINVAL_addr()")
    child.expect_exact(u"Calling test_sock_ip_create__EINVAL_netif()")
    child.expect_exact(u"Calling test_sock_ip_create__no_endpoints()")
    child.expect_exact(u"Calling test_sock_ip_create__only_local()")
    child.expect_exact(u"Calling test_sock_ip_create__only_local_reuse_ep()")
    child.expect_exact(u"Calling test_sock_ip_create__only_remote()")
    child.expect_exact(u"Calling test_sock_ip_create__full()")
    child.expect_exact(u"Calling test_sock_ip_recv__EADDRNOTAVAIL()")
    child.expect_exact(u"Calling test_sock_ip_recv__ENOBUFS()")
    child.expect_exact(u"Calling test_sock_ip_recv__EPROTO()")
    child.expect_exact(u"Calling test_sock_ip_recv__ETIMEDOUT()")
    child.expect_exact(u" * Calling sock_ip_recv()")
    child.expect(r" \* \(timed out with timeout \d+\)")
    child.expect_exact(u"Calling test_sock_ip_recv__socketed()")
    child.expect_exact(u"Calling test_sock_ip_recv__socketed_with_remote()")
    child.expect_exact(u"Calling test_sock_ip_recv__unsocketed()")
    child.expect_exact(u"Calling test_sock_ip_recv__unsocketed_with_remote()")
    child.expect_exact(u"Calling test_sock_ip_recv__with_timeout()")
    child.expect_exact(u"Calling test_sock_ip_send__EAFNOSUPPORT()")
    child.expect_exact(u"Calling test_sock_ip_send__EINVAL_addr()")
    child.expect_exact(u"Calling test_sock_ip_send__EINVAL_netif()")
    child.expect_exact(u"Calling test_sock_ip_send__ENOTCONN()")
    child.expect_exact(u"Calling test_sock_ip_send__socketed_no_local_no_netif()")
    child.expect_exact(u"Calling test_sock_ip_send__socketed_no_netif()")
    child.expect_exact(u"Calling test_sock_ip_send__socketed_no_local()")
    child.expect_exact(u"Calling test_sock_ip_send__socketed()")
    child.expect_exact(u"Calling test_sock_ip_send__socketed_other_remote()")
    child.expect_exact(u"Calling test_sock_ip_send__unsocketed_no_local_no_netif()")
    child.expect_exact(u"Calling test_sock_ip_send__unsocketed_no_netif()")
    child.expect_exact(u"Calling test_sock_ip_send__unsocketed_no_local()")
    child.expect_exact(u"Calling test_sock_ip_send__unsocketed()")
    child.expect_exact(u"Calling test_sock_ip_send__no_sock_no_netif()")
    child.expect_exact(u"Calling test_sock_ip_send__no_sock()")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")


if __name__ == "__main__":
    sys.exit(run(testfunc))
