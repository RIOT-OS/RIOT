#!/usr/bin/env python3

# Copyright (C) 2016 Freie Universität Berlin
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

def _reuse_tests(code):
    return code & 1

def _ipv6_tests(code):
    return code & (1 << 6)

def _ipv4_tests(code):
    return code & (1 << 4)

def testfunc(child):
    child.expect(u"code (0x[0-9a-f]{2})")
    code = int(child.match.group(1), base=16)
    if _ipv4_tests(code):
        if _reuse_tests(code):
            child.expect_exact(u"Calling test_sock_udp_create4__EADDRINUSE()")
        child.expect_exact(u"Calling test_sock_udp_create4__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_udp_create4__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_udp_create4__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_udp_create4__no_endpoints()")
        child.expect_exact(u"Calling test_sock_udp_create4__only_local()")
        child.expect_exact(u"Calling test_sock_udp_create4__only_local_reuse_ep()")
        child.expect_exact(u"Calling test_sock_udp_create4__only_remote()")
        child.expect_exact(u"Calling test_sock_udp_create4__full()")
        child.expect_exact(u"Calling test_sock_udp_recv4__EADDRNOTAVAIL()")
        child.expect_exact(u"Calling test_sock_udp_recv4__EAGAIN()")
        child.expect_exact(u"Calling test_sock_udp_recv4__ENOBUFS()")
        child.expect_exact(u"Calling test_sock_udp_recv4__ETIMEDOUT()")
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
        child.expect_exact(u"Calling test_sock_udp_recv4__socketed()")
        child.expect_exact(u"Calling test_sock_udp_recv4__socketed_with_remote()")
        child.expect_exact(u"Calling test_sock_udp_recv4__unsocketed()")
        child.expect_exact(u"Calling test_sock_udp_recv4__unsocketed_with_remote()")
        child.expect_exact(u"Calling test_sock_udp_recv4__with_timeout()")
        child.expect_exact(u"Calling test_sock_udp_recv4__non_blocking()")
        child.expect_exact(u"Calling test_sock_udp_send4__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_udp_send4__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_udp_send4__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_udp_send4__EINVAL_port()")
        child.expect_exact(u"Calling test_sock_udp_send4__EHOSTUNREACH()")
        child.expect_exact(u"Calling test_sock_udp_send4__ENOTCONN()")
        child.expect_exact(u"Calling test_sock_udp_send4__socketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send4__socketed_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send4__socketed_no_local()")
        child.expect_exact(u"Calling test_sock_udp_send4__socketed()")
        child.expect_exact(u"Calling test_sock_udp_send4__socketed_other_remote()")
        child.expect_exact(u"Calling test_sock_udp_send4__unsocketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send4__unsocketed_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send4__unsocketed_no_local()")
        child.expect_exact(u"Calling test_sock_udp_send4__unsocketed()")
        child.expect_exact(u"Calling test_sock_udp_send4__no_sock_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send4__no_sock()")
    if _ipv6_tests(code):
        if _reuse_tests(code):
            child.expect_exact(u"Calling test_sock_udp_create6__EADDRINUSE()")
        child.expect_exact(u"Calling test_sock_udp_create6__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_udp_create6__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_udp_create6__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_udp_create6__no_endpoints()")
        child.expect_exact(u"Calling test_sock_udp_create6__only_local()")
        child.expect_exact(u"Calling test_sock_udp_create6__only_local_reuse_ep()")
        child.expect_exact(u"Calling test_sock_udp_create6__only_remote()")
        child.expect_exact(u"Calling test_sock_udp_create6__full()")
        child.expect_exact(u"Calling test_sock_udp_recv6__EADDRNOTAVAIL()")
        child.expect_exact(u"Calling test_sock_udp_recv6__EAGAIN()")
        child.expect_exact(u"Calling test_sock_udp_recv6__ENOBUFS()")
        child.expect_exact(u"Calling test_sock_udp_recv6__ETIMEDOUT()")
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
        child.expect_exact(u"Calling test_sock_udp_recv6__socketed()")
        child.expect_exact(u"Calling test_sock_udp_recv6__socketed_with_remote()")
        child.expect_exact(u"Calling test_sock_udp_recv6__unsocketed()")
        child.expect_exact(u"Calling test_sock_udp_recv6__unsocketed_with_remote()")
        child.expect_exact(u"Calling test_sock_udp_recv6__with_timeout()")
        child.expect_exact(u"Calling test_sock_udp_recv6__non_blocking()")
        child.expect_exact(u"Calling test_sock_udp_send6__EAFNOSUPPORT()")
        child.expect_exact(u"Calling test_sock_udp_send6__EINVAL_addr()")
        child.expect_exact(u"Calling test_sock_udp_send6__EINVAL_netif()")
        child.expect_exact(u"Calling test_sock_udp_send6__EINVAL_port()")
        child.expect_exact(u"Calling test_sock_udp_send6__EHOSTUNREACH()")
        child.expect_exact(u"Calling test_sock_udp_send6__ENOTCONN()")
        child.expect_exact(u"Calling test_sock_udp_send6__socketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send6__socketed_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send6__socketed_no_local()")
        child.expect_exact(u"Calling test_sock_udp_send6__socketed()")
        child.expect_exact(u"Calling test_sock_udp_send6__socketed_other_remote()")
        child.expect_exact(u"Calling test_sock_udp_send6__unsocketed_no_local_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send6__unsocketed_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send6__unsocketed_no_local()")
        child.expect_exact(u"Calling test_sock_udp_send6__unsocketed()")
        child.expect_exact(u"Calling test_sock_udp_send6__no_sock_no_netif()")
        child.expect_exact(u"Calling test_sock_udp_send6__no_sock()")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc))
