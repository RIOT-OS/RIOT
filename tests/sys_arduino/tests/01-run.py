#!/usr/bin/env python3

# Copyright (C) 2018 Federico Pellegrin <fede@evolware.org>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    # Wait for serial.begin() to complete
    child.expect_exact("Hello Arduino!")

    # 1 Basic read+write test on serial with error command
    child.sendline("wrang")
    child.expect_exact("UNK")

    # 2 Test serial echo
    child.sendline("echo quite long string echoing on arduino module test")
    child.expect("ECHO: quite long string echoing on arduino module test")

    # 3 Test on various print base conversions
    child.sendline("numb 4242")
    child.expect_exact("4242 4242 1092 10222")

    # 4 Test if time is running and being printed
    child.sendline("time")
    child.expect("OK END")

    # 5 Test print
    child.sendline("print")
    child.expect("1111101011000111")  # Prefix depends on sizeof(int)
    child.expect("1111101011000111")  # Prefix depends on sizeof(int)
    child.expect("75307")  # Prefix depends on sizeof(int)
    child.expect("75307")  # Prefix depends on sizeof(int)
    child.expect_exact("print(int, DEC): -1337")
    child.expect_exact("println(int, DEC): -1337")
    child.expect("fac7")  # Prefix depends on sizeof(int)
    child.expect("fac7")  # Prefix depends on sizeof(int)
    child.expect_exact("print(unsigned int, BIN): 101010")
    child.expect_exact("println(unsigned int, BIN): 101010")
    child.expect_exact("print(unsigned int, OCT): 52")
    child.expect_exact("println(unsigned int, OCT): 52")
    child.expect_exact("print(unsigned int, DEC): 42")
    child.expect_exact("println(unsigned int, DEC): 42")
    child.expect_exact("print(unsigned int, HEX): 2a")
    child.expect_exact("println(unsigned int, HEX): 2a")
    child.expect_exact("print(long, BIN): 10110110011010011111110100101110")
    child.expect_exact("println(long, BIN): 10110110011010011111110100101110")
    child.expect_exact("print(long, OCT): 26632376456")
    child.expect_exact("println(long, OCT): 26632376456")
    child.expect_exact("print(long, DEC): -1234567890")
    child.expect_exact("println(long, DEC): -1234567890")
    child.expect_exact("print(long, HEX): b669fd2e")
    child.expect_exact("println(long, HEX): b669fd2e")
    child.expect_exact("print(unsigned long, BIN): 1001001100101100000001011010010")
    child.expect_exact("println(unsigned long, BIN): 1001001100101100000001011010010")
    child.expect_exact("print(unsigned long, OCT): 11145401322")
    child.expect_exact("println(unsigned long, OCT): 11145401322")
    child.expect_exact("print(unsigned long, DEC): 1234567890")
    child.expect_exact("println(unsigned long, DEC): 1234567890")
    child.expect_exact("print(unsigned long, HEX): 499602d2")
    child.expect_exact("println(unsigned long, HEX): 499602d2")
    child.expect_exact("print(float): 3.14")
    child.expect_exact("print(float): 3")
    child.expect_exact("print(float): 3.1")
    child.expect_exact("print(float): 3.14")
    child.expect_exact("print(float): 3.141")


if __name__ == "__main__":
    sys.exit(run(testfunc))
