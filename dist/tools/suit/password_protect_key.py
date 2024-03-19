#!/usr/bin/env python3

#
# Copyright (C) 2021_ML!PA Consulting GmbH
#

import sys
import os

from cryptography.hazmat.primitives.serialization import Encoding
from cryptography.hazmat.primitives.serialization import PrivateFormat
from cryptography.hazmat.primitives.serialization import BestAvailableEncryption
from cryptography.hazmat.primitives import serialization


def main():
    if len(sys.argv) < 2:
        print(f"usage: {os.path.basename(sys.argv[0])} <password> <key .pem file>")
        sys.exit(1)
    if len(sys.argv) > 1:
        file = sys.argv[2]
        with open(file, "rb") as key_file:
            pk = serialization.load_pem_private_key(key_file.read(),
                                                    password=None,
                                                    )
        pw = str.encode(sys.argv[1])
        crypt = BestAvailableEncryption(pw)
        pem = pk.private_bytes(encoding=Encoding.PEM,
                               format=PrivateFormat.PKCS8,
                               encryption_algorithm=crypt,
                               )

        with open(file, "wb") as f:
            f.write(pem)


if __name__ == '__main__':
    main()
