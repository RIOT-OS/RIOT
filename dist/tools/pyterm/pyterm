#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2014  Oliver Hahm <oliver.hahm@inria.fr>
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


try:
    import configparser
except ImportError:
    import ConfigParser as configparser


import cmd, serial, socket, sys, threading, readline, time, logging, \
       os, argparse, re, codecs, signal

### import twisted if available, define dummy classes otherwise
try:
    from twisted.internet import reactor
    from twisted.internet.protocol import Protocol, \
                                          ReconnectingClientFactory
except ImportError:
    logging.getLogger("").warn("Twisted not available, please install "
                               "it if you want to use pyterm's JSON "
                               "capabilities")

    class Protocol():
        def __init__(self):
            pass
    class ReconnectingClientFactory():
        def __init__(self):
            pass

### set some default options
import platform
defaulthostname = platform.node()

### default serial port
defaultport     = "/dev/ttyUSB0"

### default baudrate for serial connection
defaultbaud     = 115200

### directory to store configuration and log files
defaultdir      = os.environ['HOME'] + os.path.sep + '.pyterm'

### configuration file name
defaultfile     = "pyterm-" + defaulthostname + ".conf"

### logging subfolder
defaultrunname  = "default-run"

### default logging prefix format string
default_fmt_str = '%(asctime)s - %(levelname)s # %(message)s'

class SerCmd(cmd.Cmd):
    """Main class for pyterm based on Python's Cmd class.

    Runs an interactive terminal that transfer between stdio and serial
    port.
    """

    def __init__(self, port=None, baudrate=None, tcp_serial=None,
                 confdir=None, conffile=None, host=None, run_name=None):
        """Constructor.

        Args:
            port (str):         serial port
            baudrate (int):     serial baudrate
            tcp_serial (iht):   TCP port to connect to (alternatively)
            confdir (str):      configuration directory
            conffile (str):     configuration file name
            host (str):         local host name
            run_name (str):     identifier for log filer subdirectory

        """

        # initialize class members
        cmd.Cmd.__init__(self)
        self.port = port
        self.baudrate = baudrate
        self.tcp_serial = tcp_serial
        self.configdir = confdir
        self.configfile = conffile
        self.host = host
        self.run_name = run_name

        if not self.host:
            self.host = defaulthostname

        if not self.run_name:
            self.run_name = defaultrunname

        if not os.path.exists(self.configdir):
            os.makedirs(self.configdir)

        self.aliases = dict()
        self.triggers = dict()
        self.filters = []
        self.ignores = []
        self.json_regs = dict()
        self.init_cmd = []
        self.load_config()
        self.fmt_str = None
        if not self.fmt_str:
            self.fmt_str = default_fmt_str

        # check for a history file
        try:
            readline.read_history_file()
        except IOError:
            pass

        ### create Logging object
        my_millis = "{:.4f}".format(time.time())
        date_str = '{}.{}'.format(time.strftime('%Y%m%d-%H:%M:%S'),
                                  my_millis[-4:])
        self.startup = date_str
        # create formatter
        formatter = logging.Formatter(self.fmt_str)

        directory = self.configdir + os.path.sep + self.host
        if not os.path.exists(directory):
            os.makedirs(directory)
        logging.basicConfig(filename = directory + os.path.sep +
                                       self.run_name + '.log',
                                       level=logging.DEBUG,
                                       format=self.fmt_str)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)

        # create logger
        self.logger = logging.getLogger('')
        self.logger.setLevel(logging.INFO)

        # add formatter to ch
        ch.setFormatter(formatter)
        # add ch to logger
        self.logger.addHandler(ch)


    def preloop(self):
        """Executed bat program start.
        """

        # if no serial or TCP is specified use default serial port
        if not self.port and not self.tcp_serial:
            sys.stderr.write("No port specified, using default (%s)!\n"
                             % (defaultport))
            self.port = defaultport
        # if a TCP port is specified try to connect
        if self.tcp_serial:
            self.logger.info("Connect to localhost:%s"
                             % self.tcp_serial)
            for res in socket.getaddrinfo('localhost', self.tcp_serial,
                                          socket.AF_UNSPEC, 
                                          socket.SOCK_STREAM):
                af, socktype, proto, canonname, sa = res
                try:
                    s = fdsocket(af, socktype, proto)
                except socket.error as msg:
                    s = None
                    continue
                try:
                    s.connect(sa)
                except socket.error as msg:
                    s.close()
                    s = None
                    continue
                break
            if s:
                self.ser = s
            else:
                self.logger.error("Something went wrong connecting to "
                                  "localhost:%s" % self.tcp_serial)
                sys.exit(1)
        # otherwise go for the serial port
        elif self.port:
            self.logger.info("Connect to serial port %s" % self.port)
            self.serial_connect()

        # wait until connection is established and fire startup
        # commands to the node
        time.sleep(1)
        for cmd in self.init_cmd:
            self.logger.debug("WRITE ----->>>>>> '" + cmd + "'\n")
            self.onecmd(self.precmd(cmd))

        # start serial->console thread
        receiver_thread = threading.Thread(target=self.reader)
        receiver_thread.setDaemon(1)
        receiver_thread.start()

    def precmd(self, line):
        """Check for command prefixes to distinguish between Pyterm
        interal commands and commands that should be send to the node.
        """
        self.logger.debug("processing line #%s#" % line)
        if (line.startswith("/")):
                return "PYTERM_" + line[1:]
        return line

    def default(self, line):
        """In case of no Pyterm specific prefix is detected, split
        string by colons and send it to the node.
        """
        self.logger.debug("%s is no pyterm command, sending to default "
                          "out" % line)
        for tok in line.split(';'):
            tok = self.get_alias(tok)
            self.ser.write((tok.strip() + "\n").encode("utf-8"))

    def do_help(self, line):
        """Do not use Cmd's internal help function, but redirect to the
        node.
        """
        self.ser.write("help\n".encode("utf-8"))

    def complete_date(self, text, line, begidx, endidm):
        """Auto completion for date string.
        """
        date = time.strftime("%Y-%m-%d %H:%M:%S")
        return ["{}".format(date)]

    def do_PYTERM_reset(self, line):
        """Pyterm command: Send a reset to the node.
        """
        self.ser.setDTR(1)
        self.ser.setDTR(0)

    def do_PYTERM_exit(self, line, unused=None):
        """Pyterm command: Exit Pyterm.
        """
        self.logger.info("Exiting Pyterm")
        # save history file
        readline.write_history_file()
        # shut down twisted if running
        if reactor.running:
            reactor.callFromThread(reactor.stop)
        if self.tcp_serial:
            self.ser.close()
        return True

    def do_PYTERM_save(self, line):
        """Pyterm command: Save Pyterm configuration to file.
        """
        if not self.config.has_section("general"):
            self.config.add_section("general")
        self.config.set("general", "port", self.port)
        if len(self.aliases):
            if not self.config.has_section("aliases"):
                self.config.add_section("aliases")
            for alias in self.aliases:
                self.config.set("aliases", alias, self.aliases[alias])
        if len(self.triggers):
            if not self.config.has_section("triggers"):
                self.config.add_section("triggers")
            for trigger in self.triggers:
                self.config.set("triggers", trigger.pattern,
                                self.triggers[trigger])
        if len(self.json_regs):
            if not self.config.has_section("json_regs"):
                self.config.add_section("json_regs")
            for j in self.json_regs:
                self.config.set("json_regs", j,
                                self.json_regs[j].pattern)
        if len(self.filters):
            if not self.config.has_section("filters"):
                self.config.add_section("filters")
            i = 0
            for r in self.filters:
                self.config.set("filters", "filter%i" % i, r.pattern)
                i += 1
        if len(self.ignores):
            if not self.config.has_section("ignores"):
                self.config.add_section("ignores")
            i = 0
            for r in self.ignores:
                self.config.set("ignores", "ignore%i" % i, r.pattern)
                i += 1
        if len(self.init_cmd):
            if not self.config.has_section("init_cmd"):
                self.config.add_section("init_cmd")
            i = 0
            for ic in self.init_cmd:
                self.config.set("init_cmd", "init_cmd%i" % i, ic)
                i += 1

        with open(self.configdir + os.path.sep + self.configfile, 'wb')\
        as config_fd:
            self.config.write(config_fd)
            self.logger.info("Config saved")

    def do_PYTERM_show_config(self, line):
        """Pyterm command: Show current configuration.
        """
        for key in self.__dict__:
            print(str(key) + ": " + str(self.__dict__[key]))

    def do_PYTERM_alias(self, line):
        """Pyterm command: Register an alias or show an list of all
        registered aliases.
        """
        if line.endswith("list"):
            for alias in self.aliases:
                self.logger.info("{} = {}".format(alias,
                                 self.aliases[alias]))
            return
        if not line.count("="):
            sys.stderr.write("Usage: /alias <ALIAS> = <CMD>\n")
            return
        alias = line.split('=')[0].strip()
        command = line[line.index('=')+1:].strip()
        self.logger.info("adding command %s for alias %s"
                         % (command, alias))
        self.aliases[alias] = command

    def do_PYTERM_rmalias(self, line):
        """Pyterm command: Unregister an alias.
        """
        if not self.aliases.pop(line, None):
            sys.stderr.write("Alias not found")

    def get_alias(self, tok):
        """Internal function to check for aliases.
        """
        for alias in self.aliases:
            if tok.split()[0] == alias:
                return self.aliases[alias] + tok[len(alias):]
        return tok

    def do_PYTERM_trigger(self, line):
        """Pyterm command: Register an trigger Regex.
        """
        if not line.count("="):
            sys.stderr.write("Usage: /trigger <regex> = <CMD>\n")
            return
        trigger = line.split('=')[0].strip()
        action = line[line.index('=')+1:].strip()
        self.logger.info("adding action %s for trigger %s" % (action,
                         trigger))
        self.triggers[re.compile(trigger)] = action

    def do_PYTERM_rmtrigger(self, line):
        """Pyterm command: Unregister an trigger.
        """
        if not self.triggers.pop(line, None):
            sys.stderr.write("Trigger not found")

    def do_PYTERM_ignore(self, line):
        """Pyterm command: Ignore lines with these Regexes matching.
        """
        self.ignores.append(re.compile(line.strip()))

    def do_PYTERM_unignore(self, line):
        """Pyterm command: Remote an ignore Regex.
        """
        for r in self.ignores:
            if (r.pattern == line.strip()):
                self.logger.info("Remove ignore for %s" % r.pattern)
                self.ignores.remove(r)
                return
        sys.stderr.write("Ignore for %s not found\n" % line.strip())

    def do_PYTERM_filter(self, line):
        """Pyterm command: Show only lines matching this Regex.
        """
        self.filters.append(re.compile(line.strip()))

    def do_PYTERM_unfilter(self, line):
        """Pyterm command: Remove a filter.
        """
        for r in self.filters:
            if (r.pattern == line.strip()):
                self.logger.info("Remove filter for %s" % r.pattern)
                self.filters.remove(r)
                return
        sys.stderr.write("Filter for %s not found\n" % line.strip())

    def do_PYTERM_json(self, line):
        """Pyterm command: Transfer lines matching this Regex as JSON
        object.
        """
        self.json_regs[line.split(' ')[0].strip()] = \
        re.compile(line.partition(' ')[2].strip())

    def do_PYTERM_unjson(self, line):
        """Pyterm command: Remove a JSON filter.
        """
        if not self.aliases.pop(line, None):
            sys.stderr.write("JSON regex with ID %s not found" % line)

    def do_PYTERM_init(self, line):
        """Pyterm command: Add an startup command. (Only useful in
        addition with /save).
        """
        self.init_cmd.append(line.strip())

    def load_config(self):
        """Internal function to laod configuration from file.
        """
        self.config = configparser.SafeConfigParser()
        self.config.read([self.configdir + os.path.sep + \
                         self.configfile])

        for sec in self.config.sections():
            if sec == "filters":
                for opt in self.config.options(sec):
                    self.filters.append(
                            re.compile(self.config.get(sec, opt)))
            if sec == "ignores":
                for opt in self.config.options(sec):
                    self.ignores.append(
                            re.compile(self.config.get(sec, opt)))
            if sec == "json_regs":
                for opt in self.config.options(sec):
                    self.logger.info("add json regex for %s"
                                     % self.config.get(sec, opt))
                    self.json_regs[opt] = \
                        re.compile(self.config.get(sec, opt))
            if sec == "aliases":
                for opt in self.config.options(sec):
                    self.aliases[opt] = self.config.get(sec, opt)
            if sec == "triggers":
                for opt in self.config.options(sec):
                    self.triggers[re.compile(opt)] = \
                        self.config.get(sec, opt)
            if sec == "init_cmd":
                for opt in self.config.options(sec):
                    self.init_cmd.append(self.config.get(sec, opt))
            else:
                for opt in self.config.options(sec):
                    if opt not in self.__dict__:
                        self.__dict__[opt] = self.config.get(sec, opt)

    def process_line(self, line):
        """Processes a valid line from node that should be printed and
        possibly forwarded.

        Args:
            line (str): input from node.
        """
        self.logger.info(line)
        # check if line matches a trigger and fire the command(s)
        for trigger in self.triggers:
            self.logger.debug("comparing input %s to trigger %s"
                              % (line, trigger.pattern))
            m = trigger.search(line)
            if m:
                self.onecmd(self.precmd(self.triggers[trigger]))

        # ckecking if the line should be sent as JSON object to a tcp 
        # server
        if (len(self.json_regs)) and self.factory and \
            self.factory.myproto:
            for j in self.json_regs:
                m = self.json_regs[j].search(line)
                if m:
                    try:
                        json_obj = '{"jid":%d, ' % int(j)
                    except ValueError:
                        sys.stderr.write("Invalid JID: %s\n" % j)
                        break
                    json_obj += '"raw":"%s", ' % line
                    json_obj += '"date":%s, ' % int(time.time()*1000)
                    for g in m.groupdict():
                        try:
                            json_obj += '"%s":%d, ' \
                                        % (g, int(m.groupdict()[g]))
                        except ValueError:
                            json_obj += '"%s":"%s", ' \
                                        % (g, m.groupdict()[g])

                    # eliminate the superfluous last ", "
                    json_obj = json_obj[:-2]

                    json_obj += "}"
                    self.factory.myproto.sendMessage(json_obj)

    def handle_line(self, line):
        """Handle line from node and check for further processing
        requirements.

        Args:
            line (str): input line from node.
        """
        # First check if line should be ignored
        ignored = False
        if (len(self.ignores)):
            for i in self.ignores:
                if i.search(line):
                    ignored = True
                    break
        # now check if filter rules should be applied
        if (len(self.filters)):
            for r in self.filters:
                if r.search(line):
                    if not ignored:
                        self.process_line(line)
        # if neither nor applies print the line
        else:
            if not ignored:
                self.process_line(line)


    def serial_connect(self):
        self.ser = serial.Serial(port=self.port, dsrdtr=0, rtscts=0)
        self.ser.baudrate = self.baudrate
        self.ser.setDTR(0)
        self.ser.setRTS(0)

    def reader(self):
        """Serial or TCP reader.
        """
        output = ""
        while (1):
            # check if serial port can be accessed.
            try:
                sr = codecs.getreader("UTF-8")(self.ser,
                                               errors='replace')
                c = sr.read(1)
            # try to re-open it with a timeout of 1s otherwise
            except (serial.SerialException, ValueError) as se:
                self.logger.warn("Serial port disconnected, waiting to "
                                 "get reconnected...")
                self.ser.close()
                time.sleep(1)
                if os.path.exists(self.port):
                    self.logger.warn("Try to reconnect to %s again..."
                                     % (self.port))
                    self.serial_connect()
                continue
            if c == '\n' or c == '\r':
                self.handle_line(output)
                output = ""
            else:
                output += c
            #sys.stdout.write(c)
            #sys.stdout.flush()

class PytermProt(Protocol):
    def dataReceived(self, data):
        sys.stdout.write(data)

    def sendMessage(self, msg):
        self.transport.writeSomeData("%d#%s\n" % (len(msg), msg))

class PytermClientFactory(ReconnectingClientFactory):

    def __init__(self):
        self.myproto = None

    def buildProtocol(self, addr):
        print('Connected.')
        self.resetDelay()
        self.myproto = PytermProt()
        return self.myproto

    def clientConnectionLost(self, connector, reason):
        if reactor.running:
            print('Lost connection.  Reason:', reason)
        ReconnectingClientFactory.clientConnectionLost(self, connector,
                                                       reason)

    def clientConnectionFailed(self, connector, reason):
        print('Connection failed. Reason:', reason)
        ReconnectingClientFactory.clientConnectionFailed(self,
                                                         connector,
                                                         reason)

def __stop_reactor(signum, stackframe):
    sys.stderr.write("Ctrl-C is disabled, type '/exit' instead\n")

class fdsocket(socket.socket):
    def read(self, bufsize):
        return self.recv(bufsize)

    def write(self, string):
        try:
            return self.sendall(string)
        except socket.error as e:
            logging.getLogger("").warn("Error in TCP connection (%s), "
                                       "closing down" % str(e))
            self.close()
            sys.exit(0)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Pyterm - The Python "
                                                 "terminal program")
    parser.add_argument("-p", "--port",
            help="Specifies the serial port to use, default is %s"
                 % defaultport,
            default=defaultport)
    parser.add_argument("-ts", "--tcp-serial",
            help="Connect to a TCP port instead of a serial port")
    parser.add_argument("-b", "--baudrate",
            help="Specifies baudrate for the serial port, default is %s"
                 % defaultbaud,
            default=defaultbaud)
    parser.add_argument('-d', '--directory',
            help="Specify the Pyterm directory, default is %s"
                 % defaultdir,
            default=defaultdir)
    parser.add_argument("-c", "--config",
            help="Specify the config filename, default is %s"
                 % defaultfile,
            default=defaultfile)
    parser.add_argument("-s", "--server",
            help="Connect via TCP to this server to send output as "
                 "JSON")
    parser.add_argument("-P", "--tcp_port", type=int,
            help="Port at the JSON server")
    parser.add_argument("-H", "--host",
            help="Hostname of this maschine")
    parser.add_argument("-rn", "--run-name",
            help="Run name, used for logfile")
    args = parser.parse_args()

    myshell = SerCmd(args.port, args.baudrate, args.tcp_serial,
                     args.directory, args.config, args.host, args.run_name)
    myshell.prompt = ''

    if args.server and args.tcp_port:
        myfactory = PytermClientFactory()
        reactor.connectTCP(args.server, args.tcp_port, myfactory)
        myshell.factory = myfactory
        reactor.callInThread(myshell.cmdloop, "Welcome to pyterm!\n"
                                              "Type '/exit' to exit.")
        signal.signal(signal.SIGINT, __stop_reactor)
        reactor.run()
    else:
        myshell.cmdloop("Welcome to pyterm!\nType '/exit' to exit.")
