#!/usr/bin/env python3

# Copyright (C) 2020 Gunar Schorcht
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r'Visual phydat test application\r\n')
    child.expect(r'Data:\t          12.34 °C\r\n')
    child.expect(r'Data:\t          12.34 °F\r\n')
    child.expect(r'Data:\t          123.4 K\r\n')
    child.expect(r'Data:\t          123.4 lx\r\n')
    child.expect(r'Data:\t\[0\]        1234 m\r\n')
    child.expect(r'\t\[1\]       13456 m\r\n')
    child.expect(r'\t\[2\]      -14567 m\r\n')
    child.expect(r'Data:\t\[0\]      1234e1 m\r\n')
    child.expect(r'\t\[1\]     13456e1 m\r\n')
    child.expect(r'\t\[2\]    -14567e1 m\r\n')
    child.expect(r'Data:\t\[0\]        1234 km\r\n')
    child.expect(r'\t\[1\]       13456 km\r\n')
    child.expect(r'\t\[2\]      -14567 km\r\n')
    child.expect(r'Data:\t\[0\]       12.34 m\r\n')
    child.expect(r'\t\[1\]      134.56 m\r\n')
    child.expect(r'\t\[2\]     -145.67 m\r\n')
    child.expect(r'Data:\t\[0\]        1234 mm\r\n')
    child.expect(r'\t\[1\]       13456 mm\r\n')
    child.expect(r'\t\[2\]      -14567 mm\r\n')
    child.expect(r'Data:\t       -0.12345 m\^2\r\n')
    child.expect(r'Data:\t      -12345e-6 m\^3\r\n')
    child.expect(r'Data:\t\[0\]        0.12 g\r\n')
    child.expect(r'\t\[1\]        0.34 g\r\n')
    child.expect(r'\t\[2\]       56.78 g\r\n')
    child.expect(r'Data:\t\[0\]         123 mdps\r\n')
    child.expect(r'\t\[1\]         345 mdps\r\n')
    child.expect(r'\t\[2\]         678 mdps\r\n')
    child.expect(r'Data:\t         1234.5 G\r\n')
    child.expect(r'Data:\t          12345 kA\r\n')
    child.expect(r'Data:\t          12345 MV\r\n')
    child.expect(r'Data:\t        12345e7 W\r\n')
    child.expect(r'Data:\t         1234.5 Gs\r\n')
    child.expect(r'Data:\t         12.345 dBm\r\n')
    child.expect(r'Data:\t          12345 C\r\n')
    child.expect(r'Data:\t          12345 uF\r\n')
    child.expect(r'Data:\t       12345e-7 F\r\n')
    child.expect(r'Data:\t         123.45 pH\r\n')
    child.expect(r'Data:\t          12345 Bar\r\n')
    child.expect(r'Data:\t          12345 hPa\r\n')
    child.expect(r'Data:\t         123.45 cd\r\n')
    child.expect(r'Data:\t          12345 mcts\r\n')
    child.expect(r'Data:\t         12.345 %\r\n')
    child.expect(r'Data:\t         1.2345 permille\r\n')
    child.expect(r'Data:\t          12345 uppm\r\n')
    child.expect(r'Data:\t          12345 nppb\r\n')
    child.expect(r'Data:\t         1234.5 g/m\^3\r\n')
    child.expect(r'Data:\t         1234.5 #/m\^3\r\n')
    child.expect(r'Data:\t12:34:56\r\n')
    child.expect(r'Data:\t2020-02-27\r\n')


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))
