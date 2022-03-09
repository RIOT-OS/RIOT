#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import argparse
import os

import kea
import util

__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__version__ = "0.0.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"
__status__ = "Experimental"


DEFAULT_NEXT_HOP = "fe80::2"
DEFAULT_DELEGATED_LEN = 64


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--daemonized", action="store_true",
                        help="Run server in background")
    parser.add_argument("-p", "--pidfile", nargs="?",
                        help="PID file for the server. Required with -d.")
    parser.add_argument(
        "-n", "--next-hop", default=DEFAULT_NEXT_HOP, nargs="?",
        help="Next hop address for application (default: fe80::2)"
    )
    parser.add_argument(
        "-g", "--delegated-len", default=DEFAULT_DELEGATED_LEN, nargs="?",
        type=int,
        help="The prefix length delegated by the DHCPv6 server. "
             "Must be greater or equal to the prefix length of the subnet. "
             "This may differ from the prefix length provided in subnet more "
             "to understand as a template from which to generate the "
             "delegated prefixes from. "
             "(default: 64)"
    )
    parser.add_argument(
        "interface", help="Interface to bind DHCPv6 server to"
    )
    parser.add_argument(
            "subnet", type=util.split_prefix,
            help="Subnet to delegate (must have format <prefix>/<prefix_len>)"
    )
    args = parser.parse_args()
    if "SUDO_USER" not in os.environ:
        raise PermissionError("Must be run with sudo")
    if args.delegated_len < args.subnet[1]:
        raise ValueError("delegated_len {} is lesser than prefix length {}"
                         .format(args.delegated_len, args.subnet[1]))
    config = kea.KeaConfig(args.interface, args.subnet[0], args.subnet[1],
                           args.delegated_len)
    server = kea.KeaServer(config, args.next_hop, daemonized=args.daemonized,
                           pidfile=args.pidfile)
    server.run()


if __name__ == "__main__":
    main()
