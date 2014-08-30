import sys
import os
import pty, shlex
import signal
import subprocess
import socket
import time
import atexit
import re
import string
import logging

import random  
from .vif import VirtualInterface
from .vnet import VirtualNet
from .riotnative import RIOT
from string import Template

try:
    import libvirt
except:
    print "libvirt not found, you won't be able to work with qemu"
import hashlib

all_domains = None

class VMException(Exception):
    def __init__(s, msg=None):
        if not msg:
            s.message = "Unknown VM Error."
        else:
            s.message = msg

class VM():
    def __init__(self, name, nodeType, nics=None, binary=None, vmgroup_name=""):
        self.name = name
        self.nodeType = nodeType
        self.binary = binary
        self.nics = nics
        if not nics:
            self.nics = []

        self.vmgroup_name = vmgroup_name
        self.vm_instance = None
        
        self.fullname = self.name
        if self.vmgroup_name:
            self.fullname = "%s_%s" % (self.vmgroup_name, name)


    def lookup(self, conn=None):
        global all_domains
        if self.nodeType == "meshrouter":
            if not all_domains:
                all_domains = {}
                for id in conn.listDomainsID():
                   dom = conn.lookupByID(id)
                   all_domains[dom.name()] = dom

                for id in conn.listDefinedDomains():
                   all_domains[id] = conn.lookupByName(id)
       
            try:
                self.vm_instance = all_domains[self.fullname]
                logging.getLogger("").debug("Domain %s already defined." % self.fullname)
                self.conn = conn
                return True
            except libvirt.libvirtError:
                return False
            except KeyError:
                return False
        elif self.nodeType == "riot_native":
            logging.getLogger("Looking up this node")
            self.vm_instance = RIOT(self.fullname, self.binary, self.vmgroup_name, self.nics[0].tap)
            return True

    def define(self, conn=None):
        if self.nodeType == "meshrouter":
            if not self.lookup(conn):
                logging.getLogger("").info("Defining VM %s" %(self.fullname))
                self.vm_instance = conn.defineXML(self.create_vm_xml())
        else:
            logging.getLogger("").info("Defining RIOT native process %s" % (self.fullname))
            if not self.binary:
                logging.getLogger("").error("No binary for RIOT native given. Exiting...")
                sys.exit(1)
            self.vm_instance = RIOT(self.fullname, self.binary, self.vmgroup_name, self.nics[0].tap)

    def undefine(self, conn=None):
        # TODO: needs here anything to be done for RIOT native?
        if self.nodeType == "meshrouter":
            if self.vm_instance or self.lookup(conn):
                self.vm_instance.undefine()

    def start(self):
        if self.vm_instance:
            if not self.vm_instance.isActive():
                self.vm_instance.create()

    def stop(self):
        if self.vm_instance:
            logging.getLogger("").debug("stopping %s (%s)" % (self.name, self.vm_instance.pid))
            if self.vm_instance.isActive():
                logging.getLogger("").debug("destroying %s" % self.vm_instance.pid)
                self.vm_instance.destroy()

    def getType(self):
        return self.nodeType

    def create_interfaces_xml(self):
        if len(self.nics)<1:
            return ""

        ifxml = ""
        nic_options = ''
        for nic in self.nics:
            macaddr = ""
            if nic.macaddr:
                macaddr = macaddr_template.substitute(mac=nic.macaddr)
            
            ifxml = ifxml + if_tmpl.substitute(mac=macaddr,tap=nic.tap)

        return ifxml

    def create_vm_xml(self):
        ifxml = self.create_interfaces_xml()
        
        return vm_xml_tmpl.substitute(name=self.fullname,memory=262144,interfaces=ifxml)

vm_xml_tmpl = Template('''
<domain type='kvm'>
  <name>$name</name>
  <memory>$memory</memory>
  <vcpu>1</vcpu>
  <os>
    <type arch='i686'>hvm</type>
    <boot dev='hd'/>
  </os>
  <features>
    <acpi/>
    <pae/>
  </features>
  <clock offset='utc'/>
  <on_poweroff>restart</on_poweroff>
  <on_reboot>restart</on_reboot>
  <on_crash>restart</on_crash>
  <devices>
    <disk type='file' device='disk'>
      <source file='/usr/local/share/qemu/gpxe-serial.bin'/>
      <target dev='hda' bus='ide'/>
    </disk>
    <controller type='ide' index='0'/>
    $interfaces
    <serial type='pty'>
      <target port='0'/>
    </serial>
    <console type='pty'>
      <target port='0'/>
    </console>
  </devices>
</domain>
''')

if_tmpl = Template('''
    <interface type='ethernet'>
      $mac
      <target dev='$tap'/>
      <model type='e1000'/>
      <script path='/bin/true'/>
    </interface>
''');

# if_tmpl = Template('''
#     <interface type='bridge'>
#       <source bridge='$bridge'/>
#       $mac
#       <target dev='$tap'/>
#       <model type='e1000'/>
#     </interface>
# ''');

macaddr_template = Template('''
      <mac address='$mac'/>
''');

