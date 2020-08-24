#!/usr/bin/env python3

# Copyright (c) 2020 Jannes Volkens, for HAW Hamburg  <jannes.volkens@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import serial
import time
import json

DEFAULT_SLEEP_TIME = 0.01


class Connect:

    def __init__(self, port, baud):
        self.__dev = serial.Serial(port, baud)
        if(self.__dev.isOpen() is False):
            self.__dev.open()

    def add_config(self, config_file):
        with open("{}".format(config_file)) as json_file:
            data = json.load(json_file)
            for entry in data['Entries']:
                self.__dev.write("can_gateway add_id {} {} {} {} {} {}\n".format(entry['can_id'],
                                 entry['dst_port'],
                                 entry['ip_addr'],
                                 entry['eth_send'],
                                 entry['dst_ifnum'],
                                 entry['src_ifnum']).encode())
                time.sleep(DEFAULT_SLEEP_TIME)

    def can_set_bitrate(self, iface, bitrate, sample_point):
        self.__dev.write("can_gateway set_bitrate {} {} {}\n".format(iface, bitrate, sample_point).encode())

    def can_recv(self):
        self.__dev.write("can_gateway start_can_recv\n".encode())

    def can_recv_stop(self):
        self.__dev.write("can_gateway close\n".encode())
        self.__dev.write("can_gateway delete_list\n".encode())

    def __del__(self):
        self.__dev.close()
