#!/usr/bin/python2
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
from subprocess import call, Popen, PIPE


class Testbed():
    log_dir_name = 'log'
    
    def __init__(self):
        pass
    
    def initCleanWithFlash(self):
        self.stop()
        self.cleanLogs()
        self.flashNodes()
        self.start()
        
    def initClean(self):
        self.cleanLogs()
        self.start()
    
    def flashNodes(self):
        raise NotImplementedError("Inherit from Testbed and implement flashNodes") 
    
    def cleanLogs(self):
        raise NotImplementedError("Inherit from Testbed and implement flashNodes") 
    
    def archiveLogs(self, experiment = None):
        raise NotImplementedError("Inherit from Testbed and implement flashNodes") 
    
    def start(self):
        raise NotImplementedError("Inherit from Testbed and implement flashNodes")  
    
    def stop(self):
        raise NotImplementedError("Inherit from Testbed and implement flashNodes") 
    
    def defaultArchivePostfix(self, experimentName = None):
        if not experimentName:
            experimentName = "unknown"
        time = datetime.datetime.now().strftime("%Y-%m-%d_%H_%M_%S")
        postfix = "-" + experimentName +"_" + time  
        return postfix     
    
    def printAndCall(self, cmdString):
        print(cmdString) 
        call(cmdString, shell=True)         
    
    
class DESTestbed(Testbed):    
    def __init__(self, serverHost = None, serverPort=None, userName = None, flasher = None, 
                 hexfilePath = None, pyterm = None, logFilePath = None, hostFile = None):
        self.serverHost = serverHost
        self.serverPort = str(serverPort)
        self.userName = userName
        self.flasher = flasher
        self.hexFilePath = hexfilePath
        self.pyterm = pyterm
        self.logFilePath = logFilePath
        self.hostFile = hostFile
        
    def flashNodes(self):       
        self.printAndCall("parallel-ssh -h %s -l %s 'python %s'" % (self.hostFile, self.userName, self.flasher))
        
    def cleanLogs(self):        
        self.printAndCall("rm -rf %s/*.log" % (self.logFilePath))
        
    def archiveLogs(self, postfix = None): 
        postfix = self.defaultArchivePostfix(postfix)
        logDir = self.logFilePath.split("/")[-1]
        self.printAndCall("cd %s/..; tar -cjf archived_logs%s.tar.bz2 %s/*.log" % (self.logFilePath, postfix, logDir))
        
    def start(self):        
        self.printAndCall("parallel-ssh -h %s -l %s 'screen -S pyterm -d -m python %s -ln %s'" % (self.hostFile, self.userName, self.pyterm, self.log_dir_name))
        
    def stop(self):        
        self.printAndCall("parallel-ssh -h %s -l %s 'screen -X -S pyterm quit'" % (self.hostFile, self.userName))
        
class LocalTestbed(Testbed):    
    def __init__(self, serverHost = None, serverPort=None, flasher = None, hexfilePath = None, pyterm = None, logFilePath = None):
        self.serverHost = serverHost
        self.serverPort = str(serverPort)
        self.flasher = flasher
        self.hexFilePath = hexfilePath
        self.pyterm = pyterm
        self.logFilePath = logFilePath
        
    def findPorts(self):
        devlist = os.listdir("/dev/")        
        regex = re.compile('^ttyUSB')        
        return sorted([port for port in devlist if regex.match(port)])

    def flashNodes(self):       
        self.printAndCall("python %s %s" % (self.flasher, self.hexFilePath))
        
    def cleanLogs(self):      
        self.printAndCall("rm -rf %s/*.log" % (self.logFilePath))
        
    def archiveLogs(self, postfix = None): 
        postfix = self.defaultArchivePostfix(postfix)
        logDir = self.logFilePath.split("/")[-1]
        self.printAndCall("cd %s/..; tar -cjf archived_logs%s.tar.bz2 %s/*.log" % (self.logFilePath, postfix, logDir))
        
    def start(self):
        portList = self.findPorts()
        for port in portList:           
            self.printAndCall("screen -S pyterm-%s -d -m python %s -H %s -rn %s -p /dev/%s -ln %s" % (port, self.pyterm, port, port, port, self.log_dir_name))
            
    def stop(self):
        portList = self.findPorts()
        for port in portList:         
            self.printAndCall("screen -X -S pyterm-%s quit" % (port))
            
class DesVirtTestbed(Testbed):       
    def __init__(self, serverHost = None, serverPort=None, desvirtPath = None, topologyName = None, pyterm = None, logFilePath = None):
        self.serverHost = serverHost
        self.serverPort = str(serverPort)
        self.desvirtPath = desvirtPath
        self.topologyName = topologyName
        self.pyterm = pyterm
        self.logFilePath = logFilePath
        self.namePortList = []
	if not os.path.isdir(self.desvirtPath):
		print("desvirt not found! Try running 'make' in RIOT/dist/tools/desvirt")
		sys.exit(-1)
        
    def findPorts(self):        
        return self.namePortList
    
    def startDesVirtNetwork(self):
        print "executing: " + "./vnet --start --name " + self.topologyName + " in: " + self.desvirtPath
        call("sh -c \"./vnet --define --name " + self.topologyName + "\"", cwd=self.desvirtPath, shell=True)
        stream = Popen("sh -c \"./vnet --start --name " + self.topologyName + "\"", cwd=self.desvirtPath, shell=True, stderr=PIPE).stderr
        pats = r'.*riotnative.*\.elf (\S+) -t (\S+)'
        pattern = re.compile(pats)
        for line in stream:
            match = pattern.match(line)
            if(match):
                tuple = match.groups()
                self.namePortList.append((tuple[0], int(tuple[1])))
        self.namePortList = sorted(self.namePortList)
        for tuple in self.namePortList:
            print "name: " + tuple[0] + " port: " + str(tuple[1])
            
    def stopDesVirtNetwork(self):
        call("sh -c \"./vnet --stop --name " + self.topologyName + "\"", cwd=self.desvirtPath, shell=True)                

    def flashNodes(self):       
        pass
        
    def cleanLogs(self): 
        self.printAndCall("rm -rf %s/*.log" % (self.logFilePath))
        
    def archiveLogs(self, postfix = None): 
        postfix = self.defaultArchivePostfix(postfix)
        logDir = self.logFilePath.split("/")[-1]
        self.printAndCall("cd %s/..; tar -cjf archived_logs%s.tar.bz2 %s/*.log" % (self.logFilePath, postfix, logDir))
        
    def start(self):
        for node in self.namePortList:           
            self.printAndCall("screen -S pyterm-%s -d -m python %s -H %s -rn %s -ts %s -ln %s" % (node[0], self.pyterm, node[0], node[0], node[1], self.log_dir_name))
            
    def stop(self):
        print "stop called"
        for node in self.namePortList:         
            self.printAndCall("screen -X -S pyterm-%s quit" % (node[0]))
        self.stopDesVirtNetwork()


            
