#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# SPDX-FileCopyrightText: 2019 Gunar Schorcht <gunar@schorcht.net>
# SPDX-FileCopyrightText: 2014 Oliver Hahm <oliver.hahm@inria.fr>
# SPDX-License-Identifier: LGPL-2.1-only

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

import serial
import sys
import time
import argparse

try:
    serial.Serial
except AttributeError:
    print("\033[1;37;41m\n")
    print("Something went terribly wrong when loading the pyserial package.")
    print("There is a good chance that you installed the 'serial' package instead")
    print("of 'pyserial'. Try running 'pip uninstall serial && pip install pyserial'")
    print("\033[0m")
    sys.exit(1)

# default serial port
defaultport = "/dev/ttyUSB0"

# default baudrate for serial connection
defaultbaud = 115200

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="espreset - ESP reset program")
    parser.add_argument("-p", "--port",
                        help="Specifies the serial port to use, default is %s"
                        % defaultport,
                        default=defaultport)
    parser.add_argument("-b", "--baudrate",
                        help="Specifies baudrate for the serial port, default "
                        "is %s" % defaultbaud,
                        default=defaultbaud)

    args = parser.parse_args()

    ser = serial.Serial(port=args.port, dsrdtr=0, rtscts=0)
    ser.baudrate = args.baudrate
    # set RST low and GPIO0 high
    ser.setRTS(1)
    ser.setDTR(0)
    # keep the RST line low for 0.1 seconds
    time.sleep(0.1)
    # set the RST high
    ser.setRTS(1)
    ser.setDTR(1)
    # wait 1 second for boot
    time.sleep(1)
    sys.exit(0)
