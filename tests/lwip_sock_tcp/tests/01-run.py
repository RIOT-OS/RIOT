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
            child.expect_exact("Calling test_tcp_connect4__EADDRINUSE()")
        child.expect_exact("Calling test_tcp_connect4__EAFNOSUPPORT()")
        child.expect_exact("Calling test_tcp_connect4__EINVAL_addr()")
        child.expect_exact("Calling test_tcp_connect4__EINVAL_netif()")
        child.expect_exact("Calling test_tcp_connect4__success_without_port()")
        child.expect_exact("Calling test_tcp_connect4__success_local_port()")
        if _reuse_tests(code):
            child.expect_exact("Calling test_tcp_listen4__EADDRINUSE()")
        child.expect_exact("Calling test_tcp_listen4__EAFNOSUPPORT()")
        child.expect_exact("Calling test_tcp_listen4__EINVAL()")
        child.expect_exact("Calling test_tcp_listen4__success_any_netif()")
        child.expect_exact("Calling test_tcp_listen4__success_spec_netif()")
        child.expect_exact("Calling test_tcp_accept4__EAGAIN()")
        child.expect_exact("Calling test_tcp_accept4__EINVAL()")
        child.expect_exact("Calling test_tcp_accept4__ETIMEDOUT()")
        start = datetime.now()
        child.expect_exact(" * Calling sock_tcp_accept()")
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
        child.expect_exact("Calling test_tcp_accept4__success()")
        child.expect_exact("Calling test_tcp_read4__EAGAIN()")
        child.expect_exact("Calling test_tcp_read4__ECONNRESET()")
        child.expect_exact("Calling test_tcp_read4__ENOTCONN()")
        child.expect_exact("Calling test_tcp_read4__ETIMEDOUT()")
        start = datetime.now()
        child.expect_exact(" * Calling sock_tcp_read()")
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
        child.expect_exact("Calling test_tcp_read4__success()")
        child.expect_exact("Calling test_tcp_read4__success_with_timeout()")
        child.expect_exact("Calling test_tcp_read4__success_non_blocking()")
        child.expect_exact("Calling test_tcp_write4__ENOTCONN()")
        child.expect_exact("Calling test_tcp_write4__success()")
    if _ipv6_tests(code):
        if _reuse_tests(code):
            child.expect_exact("Calling test_tcp_connect6__EADDRINUSE()")
        child.expect_exact("Calling test_tcp_connect6__EAFNOSUPPORT()")
        child.expect_exact("Calling test_tcp_connect6__EINVAL_addr()")
        child.expect_exact("Calling test_tcp_connect6__EINVAL_netif()")
        child.expect_exact("Calling test_tcp_connect6__success_without_port()")
        child.expect_exact("Calling test_tcp_connect6__success_local_port()")
        if _reuse_tests(code):
            child.expect_exact("Calling test_tcp_listen6__EADDRINUSE()")
        child.expect_exact("Calling test_tcp_listen6__EAFNOSUPPORT()")
        child.expect_exact("Calling test_tcp_listen6__EINVAL()")
        child.expect_exact("Calling test_tcp_listen6__success_any_netif()")
        child.expect_exact("Calling test_tcp_listen6__success_spec_netif()")
        child.expect_exact("Calling test_tcp_accept6__EAGAIN()")
        child.expect_exact("Calling test_tcp_accept6__EINVAL()")
        child.expect_exact("Calling test_tcp_accept6__ETIMEDOUT()")
        start = datetime.now()
        child.expect_exact(" * Calling sock_tcp_accept()")
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
        child.expect_exact("Calling test_tcp_accept6__success()")
        child.expect_exact("Calling test_tcp_read6__EAGAIN()")
        child.expect_exact("Calling test_tcp_read6__ECONNRESET()")
        child.expect_exact("Calling test_tcp_read6__ENOTCONN()")
        child.expect_exact("Calling test_tcp_read6__ETIMEDOUT()")
        start = datetime.now()
        child.expect_exact(" * Calling sock_tcp_read()")
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
        child.expect_exact("Calling test_tcp_read6__success()")
        child.expect_exact("Calling test_tcp_read6__success_with_timeout()")
        child.expect_exact("Calling test_tcp_read6__success_non_blocking()")
        child.expect_exact("Calling test_tcp_write6__ENOTCONN()")
        child.expect_exact("Calling test_tcp_write6__success()")
    child.expect_exact(u"ALL TESTS SUCCESSFUL")

if __name__ == "__main__":
    sys.exit(testrunner.run(testfunc, timeout=60))
