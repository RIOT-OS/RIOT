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


import sys
import os
import re
import datetime
from os.path import expanduser

pytermDir = os.path.join(os.path.dirname(__file__), os.pardir, os.pardir,
                         os.pardir, "dist", "tools", "pyterm")

sys.path.append(os.path.join(pytermDir, "pytermcontroller"))
sys.path.append(os.path.join(pytermDir, "testbeds"))

from pytermcontroller import Experiment, ExperimentRunner
from testbeds import DesVirtTestbed
from clocksyncExperiment import DesVirtClockSyncExperiment

serverHost = "localhost"
serverPort = 1025
desvirtPath = os.path.join(os.path.dirname(__file__), "desvirt")
topologyName = "ctest"
pytermArgs = " -s" + serverHost + " -P " + str(serverPort)
pyterm = os.path.join(pytermDir, "pyterm") + pytermArgs
logFilePath = os.path.join(expanduser("~"), ".pyterm", "log")


class GTSPDesVirt(DesVirtClockSyncExperiment):

    def enableProtocol(self):
        self.sendToAll("gtsp delay 0")
        self.sendToAll("gtsp on")

    def disableProtocol(self):
        self.sendToAll("gtsp off")

    def postHook(self):
        self.runner.testbed.archiveLogs("gtsp-desvirt")

testbed = DesVirtTestbed(serverHost, serverPort, desvirtPath,
                         topologyName, pyterm, logFilePath)
testbed.flashNodes()
experiment = ExperimentRunner(GTSPDesVirt, testbed)
experiment.run()
