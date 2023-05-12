#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run
from ipaddress import (
    IPv6Address,
    IPv6Network,
)


IA_NA_ADDRESS_POOL_PREFIX = "2001:db8:1::"
IA_PD_PREFIX = "2001:db8:8000::"


def testfunc(child):
    child.expect(r"Iface\s+\d+")
    child.expect(r"inet6 addr:\sfe80:[0-9a-f:]+\s+scope: link")
    child.expect(r"Iface\s+\d+")
    child.expect(r"inet6 addr:\s+fe80:[0-9a-f:]+\s+scope: link")

    global_addr_1 = extract_global_address(child)
    global_addr_2 = extract_global_address(child)
    assert global_addr_1 != global_addr_2

    global_pfx_1 = extract_global_prefix(child)
    global_pfx_2 = extract_global_prefix(child)
    assert global_pfx_1 != global_pfx_2

    assert check_ia_na_addr(global_addr_1, global_pfx_1) or \
           check_ia_pd_addr(global_addr_1, global_pfx_1)
    assert check_ia_na_addr(global_addr_2, global_pfx_2) or \
           check_ia_pd_addr(global_addr_2, global_pfx_2)


def extract_global_prefix(child):
    child.expect(
        r"(?P<global_pfx>[0-9a-f:]+)\/(64|128)\s+dev #\d\s+"
        r"expires \d+ sec\s+"
        r"deprecates \d+ sec"
    )
    global_pfx = child.match.group("global_pfx")

    if global_pfx.endswith("::"):
        # remove one trailing : in case there are no 0s between prefix and
        # suffix
        global_pfx = global_pfx[0:-1]

    return global_pfx


def extract_global_address(child):
    """Expect and extract a global address from the command line."""
    child.expect(r"inet6 addr:\s+(?P<global_addr>[0-9a-f:]+)\s+scope: global")
    return child.match.group("global_addr")


def check_prefix(addr, prefix):
    return addr.startswith(prefix)


def check_ia_na_addr(ia_na_addr, global_pfx):
    """Check if the expected IA_NA address has been assigned"""
    result = IPv6Address(ia_na_addr) in IPv6Network(f"{IA_NA_ADDRESS_POOL_PREFIX}/64")
    result = result and check_prefix(ia_na_addr, global_pfx)
    return result


def check_ia_pd_addr(ia_pd_addr, global_pfx):
    """Check if the expected IA_PD address has been assigned"""
    result = IPv6Address(ia_pd_addr) in IPv6Network(f"{IA_PD_PREFIX}/33")
    result = result and check_prefix(ia_pd_addr, global_pfx)
    return result


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=6))
