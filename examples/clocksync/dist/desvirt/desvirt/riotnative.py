import logging
import subprocess
import sys
import socket
import re

reserved_ports = []

def get_free_tcp_port(start_port=4711,logger=None):
    for i in range(1000):
        if ((start_port + i) not in reserved_ports):
            try:
                # AF_INET and SOCK_STREAM are default
                s = socket.socket()
                s.bind(('', start_port + i))
                s.close()
                logger.debug("Found free port at %d" % (start_port + i))
                reserved_ports.append(start_port + i)
                return (start_port + i)
            except socket.error as oe:
                if (oe.errno == 98):
                    logger.debug("Port %d is already in use, try another one" % (start_port + i))
                else:
                    logger.error("Fatal error while searching a free TCP port: %s" % str(oe))
                    sys.exit(1)


class RIOT():

    def __init__(self, fullname, binary, session_name, tap):
        self.fullname = fullname
        self.binary = binary
        self.session_name = session_name
        self.tap = tap
        self.pid = None

        self.logger = logging.getLogger("")

    def create(self):
        port_number = get_free_tcp_port(logger=self.logger)
        start_riot = "%s %s -t %d -d" % (self.binary, self.tap, port_number)
        self.logger.info("Start the RIOT: %s" % start_riot)
        try:
            output = subprocess.check_output(start_riot, shell=True)
            re_riot = re.compile("^RIOT pid: (?P<pid>[0-9]+)$")
            m = re_riot.match(output)
            self.pid = int(m.group('pid'))
            self.logger.info("PID: %d" % self.pid)
        except subprocess.CalledProcessError:
            self.logger.error("creating RIOT native process failed")
            sys.exit(1)

        self.is_active = True

    def destroy(self):
        self.logger.info("Kill the RIOT: %s (%s)" % (self.binary, self.pid))
        kill_string = ['kill %d' % self.pid]
        if subprocess.call(kill_string, stderr=subprocess.PIPE, shell=True):
            self.logger.error("killing RIOT native process failed")
            sys.exit(1)
        self.is_active = False

    def isActive(self):
        if (not self.pid):
            return False
        find_process = ['ps h %d' % (self.pid)]
        try:
            output = subprocess.check_output(find_process, stderr=subprocess.PIPE, shell=True)
            if (len(output)):
                return True
            else:
                return False
        except subprocess.CalledProcessError:
            self.logger.debug("Process not found")
            return False

    def exist(self):
        return True

    def __str__(self):
        return "%s %s" % (self.binary, self.tap)
