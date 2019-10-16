#!/usr/bin/env python3

#
# Copyright (C) 2019 Inria
#               2019 FU Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

import sys
import ed25519


def main():
    if len(sys.argv) != 3:
        print("usage: gen_key.py <secret filename> <public filename>")
        sys.exit(1)

    _signing_key, _verifying_key = ed25519.create_keypair()
    with open(sys.argv[1], "wb") as f:
        f.write(_signing_key.to_bytes())

    with open(sys.argv[2], "wb") as f:
        f.write(_verifying_key.to_bytes())

    vkey_hex = _verifying_key.to_ascii(encoding="hex")
    print("Generated public key: '{}'".format(vkey_hex.decode()))


if __name__ == '__main__':
    main()
