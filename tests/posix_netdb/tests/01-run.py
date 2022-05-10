#!/usr/bin/env python3

# Copyright (C) 2022 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__EAI_BADFLAGS()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__EAI_FAIL()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__EAI_FAMILY()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__EAI_NONAME()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__EAI_SERVICE()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__AI_NUMERICHOST()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__AI_NUMERICSERV()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__AF_UNSPEC()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__AF_INET()")
    child.expect_exact("Calling test_posix_netdb_getaddrinfo__AF_INET6()")
    child.expect_exact("ALL TESTS SUCCESSFUL")


if __name__ == "__main__":
    sys.exit(run(testfunc))
