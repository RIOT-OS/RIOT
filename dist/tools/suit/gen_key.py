#!/usr/bin/env python3

#
# Copyright (C) 2020 Kaspar Schleiser <kaspar@schleiser.de>
#               2020 Inria
#               2020 Freie Universität Berlin

#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

import sys

from cryptography.hazmat.primitives.asymmetric.ed25519 import Ed25519PrivateKey
from cryptography.hazmat.primitives.serialization import Encoding
from cryptography.hazmat.primitives.serialization import PrivateFormat
from cryptography.hazmat.primitives.serialization import NoEncryption
from cryptography.hazmat.primitives.serialization import BestAvailableEncryption


def main():
    if len(sys.argv) < 2:
        print("usage: gen_key.py <secret filename> [password]")
        sys.exit(1)

    if len(sys.argv) > 2:
        pw = str.encode(sys.argv[2])
        crypt = BestAvailableEncryption(pw)
    else:
        crypt = NoEncryption()

    pk = Ed25519PrivateKey.generate()
    pem = pk.private_bytes(encoding=Encoding.PEM,
                           format=PrivateFormat.PKCS8,
                           encryption_algorithm=crypt,
                           )

    with open(sys.argv[1], "wb") as f:
        f.write(pem)


if __name__ == '__main__':
    main()
