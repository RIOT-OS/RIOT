#!/usr/bin/env python3
#
# Copyright (c) 2020 Kaspar Schleiser <kaspar@schleiser.de>
#                    Freie Universität Berlin
#                    Inria
#
# This utiliy turns a string into a 16bit hex value.
#
# If the resulting value is smaller than MIN_VALUE, the concatination of the
# value as hex string and the original string will be mangleded again, until
# `res > MIN_VALUE`.
#
# Usage example:
#
#     $ ./define2u16.py MSG_XTIMER
#    re-doing crc16("0x24fdMSG_XTIMER")
#    #define MSG_XTIMER 0xe202 /* created with dist/tools/define2u16.py */
#

import binascii
import sys

# folds upper and lower crc32() bits using XOR
def crc16(_bytes):
    tmp = binascii.crc32(_bytes)
    return (tmp >> 16) ^ tmp & 0xFFFF

# arbitrary minimum value, to keep some human scale numbers free
MIN_VAL = 0xa000

string = sys.argv[1]
if len(string) < 1:
    print("usage: %s <define>", file=sys.stderr)
    sys.exit(1)

again = string
res = crc16(string.encode("ascii"))
while res < MIN_VAL:
    again = hex(res) + again
    print("re-doing crc16(\"%s\")" % (again))
    res = crc16(again.encode("ascii"))

print("#define %s %s /* created with dist/tools/define2u16.py */" % (string, hex(res)))
