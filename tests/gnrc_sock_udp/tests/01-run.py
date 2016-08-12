#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys

from datetime import datetime

sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
import testrunner

class InvalidTimeout(Exception):
    pass

def testfunc(child):
    child.expect_exact(u"Calling test_sock_udp_create__EADDRINUSE()")
    child.expect_exact(u"Calling test_sock_udp_create__EAFNOSUPPORT()")
    child.expect_exact(u"Calling test_sock_udp_create__EINVAL_addr()")
    child.expect_exact(u"Calling test_sock_udp_create__EINVAL_netif()")
    child.expect_exact(u"Calling test_sock_udp_create__no_endpoints()")
    child.expect_exact(u"Calling test_sock_udp_create__only_local()")
    child.expect_exact(u"Calling test_sock_udp_create__only_local_reuse_ep()")
    child.expect_exact(u"Calling test_sock_udp_create__only_remote()")
    child.expect_exact(u"Calling test_sock_udp_create__full()")
    child.expect_exact(u"Calling test_sock_udp_recv__EADDRNOTAVAIL()")
    child.expect_exact(u"Calling test_sock_udp_recv__EAGAIN()")
    child.expect_exact(u"Calling test_sock_udp_recv__ENOBUFS()")
    child.expect_exact(u"Calling test_sock_udp_recv__EPROTO()")
    child.expect_exact(u"Calling test_sock_udp_recv__ETIMEDOUT()")
    child.match         # get to ensure program reached that point
    start = datetime.now()
    child.expect_exact(u" * Calling sock_udp_recv()")
    child.expect(u" \\* \\(timed out with timeout (\\d+)\\)")
    exp_diff = int(child.match.group(1))
    stop = datetime.now()
    diff = (stop - start)
    diff = (diff.seconds * 1000000) + diff.microseconds
    # fail within 5% of expected
    if diff > (exp_diff + (exp_diff * 0.05)) or \
       diff < (exp_diff - (exp_diff * 0.05)):
        raise InvalidTimeout("Invalid timeout %d (expected %d)" % (diff, exp_diff));
    else:
        print("Timed out correctly: %d (expected %d)" % (diff, exp_diff))
    child.expect_exact(u"Calling test_sock_udp_recv__socketed()")
    child.expect_exact(u"Calling test_sock_udp_recv__socketed_with_remote()")
    child.expect_exact(u"Calling test_sock_udp_recv__unsocketed()")
    child.expect_exact(u"Calling test_sock_udp_recv__unsocketed_with_remote()")
    child.expect_exact(u"Calling test_sock_udp_recv__with_timeout()")
    child.expect_exact(u"Calling test_sock_udp_recv__non_blocking()")
    child.expect_exact(u"Calling test_sock_udp_send__EAFNOSUPPORT()")
    child.expect_exact(u"Calling test_sock_udp_send__EINVAL_addr()")
    child.expect_exact(u"Calling test_sock_udp_send__EINVAL_netif()")
    child.expect_exact(u"Calling test_sock_udp_send__EINVAL_port()")
    child.expect_exact(u"Calling test_sock_udp_send__ENOTCONN()")
    child.expect_exact(u"Calling test_sock_udp_send__socketed_no_local_no_netif()")
    child.expect_exact(u"Calling test_sock_udp_send__socketed_no_netif()")
    child.expect_exact(u"Calling test_sock_udp_send__socketed_no_local()")
    child.expect_exact(u"Calling test_sock_udp_send__socketed()")
    child.expect_exact(u"Calling test_sock_udp_send__socketed_other_remote()")
    child.expect_exact(u"Calling test_sock_udp_send__unsocketed_no_local_no_netif()")
    child.expect_exact(u"Calling test_sock_udp_send__unsocketed_no_netif()")
    child.expect_exact(u"Calling test_sock_udp_send__unsocketed_no_local()")
    child.expect_exact(u"Calling test_sock_udp_send__unsocketed()")
    child.expect_exact(u"Calling test_sock_udp_send__no_sock_no_netif()")
    child.expect_exact(u"Calling test_sock_udp_send__no_sock()")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
