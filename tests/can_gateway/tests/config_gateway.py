#!/usr/bin/env python3

# Copyright (c) 2020 Jannes Volkens, for HAW Hamburg  <jannes.volkens@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import argparse
import connect_if

DEFAULT_INPUT = 'None'
DEFAULT_BAUDRATE = 115200
DEFAULT_PORT = '/dev/ttyACM1'
DEFAULT_BITRATE = 83333
DEFAULT_SAMPLE_POINT = 875
DEFAULT_CAN_INTERFACE = 0
DEFAULT_CONFIG_FILE = 'configIPv6.json'


def main_func():
    p = argparse.ArgumentParser()
    p.add_argument("-p", "--port", type=str, default=DEFAULT_PORT,
                   help="Set serial port, default: %s" % DEFAULT_PORT)
    p.add_argument("-b", "--baudrate", type=int, default=DEFAULT_BAUDRATE,
                   help="Set baudrate of the serial port, default: %d" % DEFAULT_BAUDRATE)
    p.add_argument("-i", "--input", type=str, default=DEFAULT_INPUT,
                   help="possible input: [add_config], [set_bitrate], [can_recv], [can_recv_stop]")
    p.add_argument("-br", "--bitrate", type=int, default=DEFAULT_BITRATE,
                   help="Set CAN bitrate, default: %d" % DEFAULT_BITRATE)
    p.add_argument("-sp", "--sample_point", type=int, default=DEFAULT_SAMPLE_POINT,
                   help="Set CAN sample point, default: %d" % DEFAULT_SAMPLE_POINT)
    p.add_argument("-if", "--interface", type=int, default=DEFAULT_CAN_INTERFACE,
                   help="Set CAN interface, default: %d" % DEFAULT_CAN_INTERFACE)
    p.add_argument("-cf", "--config_file", type=str, default=DEFAULT_CONFIG_FILE,
                   help="Set config file, default: %s" % DEFAULT_CONFIG_FILE)
    args = p.parse_args()

    connection = connect_if.Connect(port=args.port, baud=args.baudrate)

    if args.input == 'add_config':
        connection.add_config(config_file=args.config_file)
    elif args.input == 'set_bitrate':
        connection.can_set_bitrate(iface=args.interface, bitrate=args.bitrate, sample_point=args.sample_point)
    elif args.input == 'can_recv':
        connection.can_recv()
    elif args.input == 'can_recv_stop':
        connection.can_recv_stop()
    else:
        print("Nothing to do")


if __name__ == "__main__":
    main_func()
