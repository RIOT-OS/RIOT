#!/usr/bin/env python3

# Copyright (c) 2018 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import serial
import time


class PufSram:

    def __init__(self, port, baud):
        self.__dev = serial.Serial(port, baud, timeout=10)
        if(self.__dev.isOpen() is False):
            self.__dev.open()

    def repower(self, shutdown_time):
        self.__dev.setRTS(True)
        time.sleep(shutdown_time)
        self.__dev.setRTS(False)

    def read_data(self):
        data = None
        start = False
        str = 'no_exit'
        while (str != ''):
            str = self.__dev.readline()
            if (b'Start: ' in str):
                start = True
            if ((b'Success: ' in str) and (start is True)):
                if (b'[' in str) and (b']' in str):
                    data_str = str[str.find(b"[")+1:str.find(b"]")]
                    data = int(data_str, 0)
            if ((b'End: ' in str) and (data is not None)):
                return data
        return None

    def get_seed_list(self, n, off_time, allow_print):
        data = list()
        for i in range(0, n):
            self.repower(off_time)
            data.append(self.read_data())
            if (allow_print):
                print('Iteration %i/%i' % (i, n))
                print(data[-1])
        return data

    def connect(self, dev):
        if (dev.isOpen()):
            dev.close()
        self.__dev = self
        if(self.__dev.isOpen() is False):
            self.__dev.open()

    def disconnect(self):
        self.__dev.close()

    def __del__(self):
        self.__dev.close()
