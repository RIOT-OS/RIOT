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


import sys, signal, threading

from twisted.internet import reactor
from twisted.internet.protocol import Protocol, Factory


class PubProtocol(Protocol):
    def __init__(self, factory):
        self.factory = factory

    def connectionMade(self):
        print("new connection made")   

    def connectionLost(self, reason):
        self.factory.numProtocols = self.factory.numProtocols - 1

    def connectionLost(self, reason):
        self.factory.clients = {key: value for key, value in self.factory.clients.items() 
             if value is not self.transport}

    def dataReceived(self, data):
        if data.startswith("hostname: "):
            remoteHostname = data.split()[1].strip()
            print("dataReceived added: " + remoteHostname)
            self.factory.clients[remoteHostname] = self.transport
        else:
            print("received some useless data...")

class PubFactory(Factory):
    def __init__(self):
        self.clients = dict()

    def buildProtocol(self, addr):
        return PubProtocol(self)
  
    
class ExperimentRunner():
    def __init__(self, experiment, testbed):
        self.publisher = PubFactory()
        self.port = int(testbed.serverPort)
        self.experiment = experiment(self.publisher, self)
        self.testbed = testbed
    
    def run(self):
        signal.signal(signal.SIGINT, self.handle_sigint)
        self.experiment.run()
        reactor.listenTCP(self.port, self.publisher)
        # clean logfiles and start nodes but don't flash nodes
        self.testbed.initClean() 
        reactor.run() 
        
    def stop(self):
        self.testbed.stop()
        reactor.callFromThread(self.safeReactorStop)
            
    def safeReactorStop(self):
        if reactor.running:
            try:
                reactor.stop()
            except:
                print("tried to shutdown reactor twice!")
                
        
    def handle_sigint(self, signal, frame):        
        self.experiment.stop()
        self.testbed.stop()
        self.stop() # shutdown if experiment didn't already
         

class Experiment():
    def __init__(self, factory, runner):
        self.factory = factory
        self.runner = runner
        self.hostid = dict()
        self.sumDelay = 0
        
    def run(self):
        print("Running preHook")
        self.preHook()
        print("Running experiment")
        self.start()        

    def start(self):
        raise NotImplementedError("Inherit from Experiment and implement start")  
    
    def stop(self): 
        print("Running postHook")       
        self.postHook()  
        self.runner.stop()       
      
    def preHook(self):
        pass   
    
    def postHook(self):
        pass 
 
    def readHostFile(self, path):
        id = 1
        with open(path) as f:
            for line in f:
                self.hostid[line.strip()] = id
                id += 1
    
    def sendToHost(self, host=None, cmd=""):
        if host in self.factory.clients:
            self.factory.clients[host].write(cmd + "\n")
        else:
            print("sendToHost: no such host known: " + host + " !")
    
    def sendToAll(self, cmd=""):
       for host, transport in self.factory.clients.items():
            self.sendToHost(host, cmd)        
         
    def pauseInSeconds(self, seconds=0):
        from time import time, sleep
        start = time()
        while (time() - start < seconds):
            sleep(seconds - (time() - start))
            
    def callLater(self, absoluteDelay = 0.0, function = None):
        reactor.callLater(absoluteDelay, function)
        
    def waitAndCall(self, relativeDelay = 0.0, function = None):
        self.sumDelay += relativeDelay
        self.callLater(self.sumDelay, function)
        
    def clientIterator(self):
        return self.factory.clients.items()
    
    def connectionByHostname(self, host=None):
        if host in self.factory.clients:
            return self.factory.clients[host] 
    
    @staticmethod 
    def sendToConnection(connection, line):
        connection.write(line + "\n")   
  
