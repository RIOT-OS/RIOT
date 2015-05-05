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

# Add pytermcontroller and testbeds to class path
pytermDir = os.path.join(os.path.dirname(__file__), os.pardir, os.pardir)
sys.path.append(os.path.join(pytermDir, "pytermcontroller"))
sys.path.append(os.path.join(pytermDir, "testbeds"))

from pytermcontroller import Experiment, ExperimentRunner
from testbeds import DesVirtTestbed

serverHost = "localhost"
serverPort = 1025
desvirtPath = os.path.join(os.path.dirname(__file__), os.pardir, os.pardir,
                           os.pardir, os.pardir, os.pardir, "dist", "tools",
                           "desvirt", "desvirt")
topologyName = "example"
pytermArgs = " -s" + serverHost + " -P " + str(serverPort)
pyterm = os.path.join(pytermDir, "pyterm") + pytermArgs
logFilePath = os.path.join(expanduser("~"), ".pyterm", "log")


class MyExperiment(Experiment):

    def start(self):
        """ This Method will be executed by the ExperimentRunner as soon as the
        the control server is up.

        It should be used to describe the chronology of actions executed by the
        server.
        This can easily achieved by using the waitAndCall method.

        Keep in mind that starting pyterms and connecting those to the server
        will take time. This function is executed regardless of how many
        pyterms are connected to the server.
        """
        self.waitAndCall(10, self.setup)  # assign addresses to all nodes
                                          # after 10s
        self.waitAndCall(2, self.sendMessages)  # send a message on all nodes
        self.waitAndCall(2, self.stop)  # shutdown the experiment / server

    def setup(self):
        for host, connection in self.clientIterator():
            if self.hostid[host]:
                self.sendToConnection(
                    connection, "addr " + str(self.hostid[host]))

    def sendMessages(self):
        """ Sends a simple command to all connected pyterm instances which
        causes all nodes to broadcast a message over their transceiver.

        Check the log files in your home directory (.pyterm/log) for the
        result.
        """
        self.sendToAll("txtsnd 0 \"hello world\"")

    def postHook(self):
        """ Will be run after self.stop was called.
        """
        # archive log files (in your ~/.pyterm directory) with the name
        # MyExperiment and the current date/time.
        self.runner.testbed.archiveLogs("MyExperiment")


class DesVirtMyExperiment(MyExperiment):

    """ Some boilerplate code.

    In newer versions of pytermcontroller this will be unnecessary.
    """

    def preHook(self):
        """ will run before the start method is executed """
        self.runner.testbed.startDesVirtNetwork()
        namePortList = self.runner.testbed.findPorts()
        address = 1
        for tuple in namePortList:
            self.hostid[tuple[0]] = address
            address += 1


testbed = DesVirtTestbed(serverHost, serverPort, desvirtPath, topologyName,
                         pyterm, logFilePath)

testbed.flashNodes()  # Note: this has no effect on DesVirtTestbed
experiment = ExperimentRunner(DesVirtMyExperiment, testbed)
experiment.run()
