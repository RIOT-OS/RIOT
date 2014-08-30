import atexit
import random
import re
import shlex
import subprocess
import time
import getpass
import logging
import sys

from .vnet import VirtualNet

class VirtualInterface():
    def __init__(self, macaddr=None, up=True, net=None, nicname=None,create=True,node=None,tap=None):
        self.tap = tap

        if create:
            if (tap == None):
                if (node.name == None):
                    logging.getLogger("").logger.error("Cannot create tap for non-existent VM")
                    sys.exit(1)
                if (net == None):
                    logging.getLogger("").logger.warn("Something strange is going on, net is undefined while creating tap for %s" % node.name)
                    tap = "desvirt_%s" % node.name
                else:
                    tap = "%s_%s" % (net.name, node.name)

            self.tap = mktap(tap)

        self.state = 'down'
        self.nicname = nicname

        if not macaddr:
            macaddr = genmac()

        self.macaddr = macaddr

        if up and create:
            self.up()

        if net:
            self.net = net
            net.addif(self.tap,setup=create)

    def create(self):
        mktap(self.tap)

    def __str__(self):
        return self.tap
    
    def __repr__(self):
        return self.tap

    def delete(self):
        if self.state=='up':
            try:
                self.down()
            except Exception as e:
                logging.getLogger("").logger.warn(e)

        for i in range(0,20):
            if rmtap(self.tap):
                break
            logging.getLogger("").debug("tap %s busy, retrying..." % self.tap)
            time.sleep(1)

    def up(self):
        self.ifconfig('up')
        self.state='up'

    def down(self):
        self.ifconfig('down')
        self.state='down'

    def ifconfig(self, cmd):
        subprocess.call(shlex.split("sudo ip link set %s %s" % (self.tap, cmd)))

#if __name__='__main__':
#    print('vif test:')

def mktap(tap=None):
    logging.getLogger("").info("creating %s for %s" % (tap, getpass.getuser()))

    args = ['sudo', 'ip', 'tuntap', 'add', 'mode', 'tap', 'user', getpass.getuser()]
    if tap:
        args.extend(['dev', tap])
    logging.getLogger("").info("Creating tap: %s" % tap)
    subprocess.call(args, stdout=subprocess.PIPE)

    return tap
    
def rmtap(name):
    null = open('/dev/null', 'wb')
    retcode = subprocess.call(['sudo', 'ip', 'tuntap', 'del', name, 'mode', 'tap'], stdout=null)
    null.close()
    return retcode == 0

def genmac():
    mac = [ 0x50, 0x51, 0x52,
    random.randint(0x00, 0x7f),
    random.randint(0x00, 0xff),
    random.randint(0x00, 0xff) ]

    return (':'.join(map(lambda x: "%02x" % x, mac)))

