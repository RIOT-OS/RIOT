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

import sys, os
from subprocess import call
from os.path import expanduser

scriptDir = os.path.dirname(__file__)
makeFilePath = os.path.join(scriptDir, os.pardir)
desvirtPath =  os.path.join(scriptDir, "desvirt")
logFilePath =  os.path.join(expanduser("~"), ".pyterm", "log")
possibleModules = ["gtsp", "ftsp", "pulsesync"]
scriptPostFix = "DesVirt.py"

devnull = open('/dev/null', 'w')

def runTestFor(moduleName):
    print "--- testing " + moduleName + " ---"
    runOrAbort(buildModule(moduleName))
    runOrAbort(startDesVirtFor(moduleName))
    runOrAbort(evaluateLogFilesFor(moduleName))


def buildModule(moduleName):
    print "! building " + moduleName
    envvars = "export BOARD=native; export SYNCMOD=" + moduleName +";"
    return call(envvars + "make clean; make", shell=True,
                cwd=makeFilePath, stdout=devnull, stderr=devnull)

def startDesVirtFor(moduleName):
    print "! running desvirt for " + moduleName
    if moduleName in possibleModules:
        scriptPath = os.path.join(scriptDir, moduleName + scriptPostFix)
        return call(scriptPath, shell=True, stdout=devnull, stderr=devnull)
    else:
        return -1

def evaluateLogFilesFor(moduleName):
    print "! evaluating log files for " + moduleName
    minErrorCmd = os.path.join(makeFilePath, "dist", "getMinError.py") + " -m 1000 " + logFilePath
    return call(minErrorCmd, stdout=devnull, stderr=devnull, shell=True)

def runOrAbort(retVal):
    if retVal == 0:
        print "-> success"
    else:
        print "-> fail"
        sys.exit(1)

if __name__ == "__main__":
    if sys.argv[1:]:
        for module in sys.argv[1:]:
            if module in possibleModules:
                runTestFor(module)
            else:
                print "unkown module: " + module
    else:
        for module in possibleModules:
            runTestFor(module)
