import subprocess
import shlex
import atexit

class VirtualNetException(Exception):
        def __init__(s, errorstr):
            s.message = "Virtual Net Error: %s " % errorstr

"""
defines a virtual net and bind it optionally to a bridge
"""
class VirtualNet():

    def __init__(self, name, create=False):
        self.name = name
        self.managed = create

        if create:
            self.create()

    def create(self):
        self.brctl('addbr %s' % self.name)
        self.ifconfig('up')

    def delete(self):
        if self.managed:
            self.ifconfig('down')
            self.brctl('delbr %s' % self.name)

    def addif(self, ifname, setup=True):
        if setup:
            self.brctl('addif %s %s' % (self.name, ifname))
    
    def delif(self, ifname):
        self.brctl('delif %s %s' % (self.name, ifname))

    def brctl(self, command):
        cmd = 'sudo brctl %s' % command
        status = subprocess.call(shlex.split(cmd.encode("utf-8")))
    
    def ifconfig(self, command):
        cmd = 'sudo ip link set %s %s' % (self.name, command)
        status = subprocess.call(shlex.split(cmd.encode("utf-8")))

