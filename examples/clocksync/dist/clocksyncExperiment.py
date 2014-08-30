#!/usr/bin/env python2
# -*- coding: utf-8 -*-

# Copyright (C) 2014  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

import sys, os, re, datetime
from os.path import expanduser
sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir,
                             os.pardir, "dist", "tools", "pyterm", "pytermcontroller"))
sys.path.append(os.path.join(os.path.dirname(__file__), os.pardir, os.pardir,
                             os.pardir, "dist", "tools", "pyterm", "testbeds"))

from pytermcontroller import Experiment

class ClockSyncExperiment(Experiment):
    def start(self):
        self.waitAndCall(0.1*60, self.setup)
        self.waitAndCall(0.1*60, self.enableProtocol)
        self.waitAndCall(1.0*60, self.disableProtocol)
        self.waitAndCall(0.1*60, self.stop)

    def setup(self):
        for host, connection in self.clientIterator():
            if self.hostid[host]:
                self.sendToConnection(connection, "addr " + str(self.hostid[host]))

        self.sendToAll("clocksynce beacon interval 5000 5000")
        self.sendToAll("clocksynce beacon on")
        self.sendToAll("clocksynce heartbeat on")


class LocalClockSyncExperiment(ClockSyncExperiment):
    def preHook(self):
        devlist = os.listdir("/dev/")
        regex = re.compile('^ttyUSB')
        self.portList = sorted([port for port in devlist if regex.match(port)])
        address = 1
        for port in self.portList:
            self.hostid[port] = address
            address += 1

class DesVirtClockSyncExperiment(ClockSyncExperiment):
    def preHook(self):
        self.runner.testbed.startDesVirtNetwork()
        namePortList = self.runner.testbed.findPorts()
        address = 1
        for tuple in namePortList:
            self.hostid[tuple[0]] = address
            address += 1
