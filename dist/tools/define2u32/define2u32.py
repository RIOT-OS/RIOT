#!/usr/bin/env python3
#
# Copyright (c) 2020 Kaspar Schleiser <kaspar@schleiser.de>
#                    Freie Universität Berlin
#                    Inria
#
# This utiliy turns a string into it's crc32 as hex value.
#
# If the resulting value is smaller than MIN_VALUE, the concatination of the
# value as hex string and the original string will be crc32()ed again, until
# `res > MIN_VALUE`.
#

import binascii
import sys

# arbitrary minimum value, to keep some human scale numbers free
MIN_VAL = 100001

string = sys.argv[1]
if len(string) < 1:
    print("usage: %s <define>", file=sys.stderr)
    sys.exit(1)

again = string
res = binascii.crc32(string.encode("ascii"))
while res < MIN_VAL:
    again = hex(res) + again
    print("re-doing crc32(\"%s\")" % (again))
    res = binascii.crc32(again.encode("ascii"))

print("#define %s %s /* created with dist/tools/define2u32.py */" % (string, hex(res)))
