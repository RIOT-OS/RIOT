#!/usr/bin/env python3

# Copyright (C) 2020   Simon Brummer <simon.brummer@posteo.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import os

from testrunner import run
from shared_func import sudo_guard


def test_build_unspec(child):
    valid = '[::]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: ::')
    child.expect_exact('Port: 0')
    child.expect_exact('Netif: 0')


def test_build_unspec_with_port(child):
    valid = '[::]:8080'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: ::')
    child.expect_exact('Port: 8080')
    child.expect_exact('Netif: 0')


def test_build_unspec_with_interface_and_port(child):
    valid = '[::%5]:8080'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: ::')
    child.expect_exact('Port: 8080')
    child.expect_exact('Netif: 5')


def test_build_addr_with_interface_and_port(child):
    valid = '[fe80::68bf:dbff:fe05:c35e%5]:8080'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(valid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns 0')
    child.expect_exact('Family: AF_INET6')
    child.expect_exact('Addr: fe80::68bf:dbff:fe05:c35e')
    child.expect_exact('Port: 8080')
    child.expect_exact('Netif: 5')


def test_einval_no_brackets(child):
    invalid = '::'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_much_brackets(child):
    invalid = '[:]:]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_swaped_brackets(child):
    invalid = ']::['
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_no_addr_in_brackets(child):
    invalid = '[]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_first_char_no_open_bracket(child):
    invalid = 'a[]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_port_token_but_no_number(child):
    invalid = '[::]:'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_port_inval_char(child):
    invalid = '[::]:103f2'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_port_out_of_range(child):
    invalid = '[::]:65536'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_interface_id_after_bracket(child):
    invalid = '[::]%5'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_interface_id_token_but_no_number(child):
    invalid = '[::%]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_interface_id_inval_char(child):
    invalid = '[::%5a]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def test_einval_interface_addr_to_short(child):
    invalid = '[:%5]'
    child.sendline('gnrc_tcp_ep_from_str {}'.format(invalid))
    child.expect_exact('gnrc_tcp_ep_from_str: returns -EINVAL')


def main(child):
    # Read and run all test functions.
    script = sys.modules[__name__]
    tests = [getattr(script, t) for t in script.__dict__
             if type(getattr(script, t)).__name__ == "function"
             and t.startswith("test_")]

    res = 0

    for test in tests:
        try:
            test(child)
            print('- {} SUCCESS'.format(test.__name__))

        except Exception:
            res = -1
            print('- {} FAILED'.format(test.__name__))

    return res


if __name__ == '__main__':
    sudo_guard()
    res = run(main, timeout=0.5, echo=False)
    if res != 0:
        sys.exit(res)

    print(os.path.basename(sys.argv[0]) + ": success\n")
