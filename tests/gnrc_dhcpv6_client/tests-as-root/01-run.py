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


def testfunc(child):
    child.expect(r"Iface\s+\d+")
    child.expect(r"inet6 addr:\sfe80:[0-9a-f:]+\s+scope: link")
    child.expect(r"Iface\s+\d+")
    child.expect(r"inet6 addr:\s+fe80:[0-9a-f:]+\s+scope: link")

    global_addr_1, global_addr_2 = extract_global_addresses(child)

    global_pfx = extract_global_prefix(child)

    test_global_addrs(global_addr_1, global_addr_2, global_pfx)


def extract_global_prefix(child):
    child.expect(r"(?P<global_pfx>[0-9a-f:]+)/64\s+dev #\d\s+"
                 r"expires \d+ sec\s+"
                 r"deprecates \d+ sec")
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


def extract_global_addresses(child):
    """Extract two global addresses and return them as a tuple."""
    return extract_global_address(child), extract_global_address(child)


def check_ia_na_addr(ia_na_addr):
    """Check if the expected IA_NA address has been assigned"""
    return IPv6Address(ia_na_addr) in IPv6Network("{}/64".format(IA_NA_ADDRESS_POOL_PREFIX))


def check_ia_pd_addr(ia_pd_addr, global_pfx):
    """Check if the expected IA_PD address has been assigned"""
    return ia_pd_addr.startswith(global_pfx)


def check_global_addrs(ia_na_addr, ia_pd_addr, global_pfx):
    """Perform IA_NA check for the first and IA_PD for the second address"""
    return {
        "ia_na_check": check_ia_na_addr(ia_na_addr),
        "ia_pd_check": check_ia_pd_addr(ia_pd_addr, global_pfx),
    }


def test_global_addrs(global_addr_1, global_addr_2, global_pfx):
    """Assert that one global address is the IA_NA and the other one is the IA_PD address"""
    result_1 = check_global_addrs(global_addr_1, global_addr_2, global_pfx)
    result_2 = check_global_addrs(global_addr_2, global_addr_1, global_pfx)
    assert result_1 != result_2
    assert result_1["ia_na_check"] != result_2["ia_na_check"]
    assert result_1["ia_pd_check"] != result_2["ia_pd_check"]


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=6))
