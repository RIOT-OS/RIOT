#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys

from testrunner import run


def testfunc(child):
    child.sendline("run_test")
    child.expect(r"Forwarded Ethernet frame:")
    child.expect(r"00000000  57  44  33  22  11  00  CE  AB  FE  AD  F7  26  86  DD  60  00")
    child.expect(r"00000010  00  00  00  10  11  3F  20  01  0D  B8  00  00  EF  01  02  CA")
    child.expect(r"00000020  4B  EF  F4  C2  DE  01  20  01  0D  B8  00  00  AB  CD  55  44")
    child.expect(r"00000030  33  FF  FE  22  11  00  54  B8  59  AF  3A  B4  5C  85  1E  CE")
    child.expect(r"00000040  E2  EB  05  4E  A3  85")
    child.expect(r"Forwarded Ethernet frame:")
    child.expect(r"00000000  57  44  33  22  11  00  CE  AB  FE  AD  F7  26  86  DD  60  00")
    child.expect(r"00000010  00  00  00  10  11  3F  20  01  0D  B8  00  00  EF  01  02  CA")
    child.expect(r"00000020  4B  EF  F4  C2  DE  01  20  01  0D  B8  00  00  AB  CD  55  44")
    child.expect(r"00000030  33  FF  FE  22  11  00  54  B8  59  AF  3A  B4  5C  85  1E  CE")
    child.expect(r"00000040  E2  EB  05  4E  A3  85")
    child.expect(r"I got a subscription!")
    child.expect(r"00000000  60  00  00  00  00  10  11  40  20  01  0D  B8  00  00  EF  01")
    child.expect(r"00000010  02  CA  4B  EF  F4  C2  DE  01  20  01  0D  B8  00  00  AB  CD")
    child.expect(r"00000020  55  44  33  FF  FE  22  11  00  54  B8  59  AF  3A  B4  5C  85")
    child.expect(r"00000030  1E  CE  E2  EB  05  4E  A3  85")
    child.sendline("pktbuf")
    child.expect(r"packet buffer: first byte: ((0x)?[0-9a-fA-F]+), "
                 r"last byte: (0x)?[0-9a-fA-F]+ \(size: (\d+)\)")
    start_addr = child.match.group(1)
    size = child.match.group(4)
    print(start_addr, size)
    child.expect(r"  position of last byte used: \d+")
    child.expect(r"~ unused: {} \(next: (\(nil\)|0(x0+)?), size: +{}\) ~"
                 .format(start_addr, size))


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=5, echo=True))
