#!/usr/bin/env python
# -*- coding: utf-8 -*-

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

from twisted.internet import reactor
from twisted.internet.protocol import Protocol, ReconnectingClientFactory
import cmd, serial, sys, threading, readline, time, logging, os, argparse, re, codecs, signal

### set some default options
defaultport     = "/dev/ttyUSB0"
defaultbaud     = 115200
defaultdir      = os.environ['HOME'] + os.path.sep + '.pyterm'
defaultfile     = "pyterm.conf"

class SerCmd(cmd.Cmd):

    def __init__(self, port=None, baudrate=None, confdir=None, conffile=None):
        cmd.Cmd.__init__(self)
        self.port = port
        self.baudrate = baudrate
        self.configdir = confdir
        self.configfile = conffile

        if not os.path.exists(self.configdir):
            os.makedirs(self.configdir)

        self.aliases = dict()
        self.filters = []
        self.ignores = []
        self.json_regs = dict()
        self.load_config()

        try:
            readline.read_history_file()
        except IOError:
            pass

        ### create Logging object
        my_millis = "{:.4f}".format(time.time())
        date_str = '{}.{}'.format(time.strftime('%Y%m%d-%H:%M:%S'), my_millis[-4:])
        # create formatter
        fmt_str = '%(asctime)s - %(levelname)s # %(message)s'
        formatter = logging.Formatter(fmt_str)
        logging.basicConfig(filename=self.configdir + os.path.sep + date_str + '.log', level=logging.DEBUG, format=fmt_str)
        ch = logging.StreamHandler()
        ch.setLevel(logging.DEBUG)

        # create logger
        self.logger = logging.getLogger('')
        self.logger.setLevel(logging.DEBUG)

        # add formatter to ch
        ch.setFormatter(formatter)
        # add ch to logger
        self.logger.addHandler(ch)


    def preloop(self):
        if not self.port:
            sys.stderr.write("No port specified, using default (%s)!\n" % (defaultport))
            self.port = defaultport
        self.ser = serial.Serial(port=self.port, baudrate=self.baudrate, dsrdtr=0, rtscts=0)
        self.ser.setDTR(0)
        self.ser.setRTS(0)

        # start serial->console thread
        receiver_thread = threading.Thread(target=self.reader)
        receiver_thread.setDaemon(1)
        receiver_thread.start()

    def precmd(self, line):
        if (line.startswith("/")):
                return "PYTERM_" + line[1:]
        return line

    def default(self, line):
        for tok in line.split(';'):
            tok = self.get_alias(tok)
            self.ser.write((tok.strip() + "\n").encode("utf-8"))

    def do_help(self, line):
        self.ser.write("help\n".encode("utf-8"))

    def complete_date(self, text, line, begidx, endidm):
        date = time.strftime("%Y-%m-%d %H:%M:%S")
        return ["{}".format(date)]

    def do_PYTERM_reset(self, line):
        self.ser.setDTR(1)
        self.ser.setDTR(0)

    def do_PYTERM_exit(self, line, unused=None):
        print("Exiting Pyterm")
        readline.write_history_file()
        if reactor.running:
            reactor.callFromThread(reactor.stop)
        return True

    def do_PYTERM_save(self, line):
        if not self.config.has_section("general"):
            self.config.add_section("general")
        self.config.set("general", "port", self.port)
        if len(self.aliases):
            if not self.config.has_section("aliases"):
                self.config.add_section("aliases")
            for alias in self.aliases:
                self.config.set("aliases", alias, self.aliases[alias])
        if len(self.json_regs):
            if not self.config.has_section("json_regs"):
                self.config.add_section("json_regs")
            for j in self.json_regs:
                self.config.set("json_regs", j, self.json_regs[j].pattern)
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

        with open(self.configdir + os.path.sep + self.configfile, 'wb') as config_fd:
            self.config.write(config_fd)
            print("Config saved")

    def do_PYTERM_show_config(self, line):
        for key in self.__dict__:
            print(str(key) + ": " + str(self.__dict__[key]))

    def do_PYTERM_alias(self, line):
        if line.endswith("list"):
            for alias in self.aliases:
                print("{} = {}".format(alias, self.aliases[alias]))
            return
        if not line.count("="):
            sys.stderr.write("Usage: alias <ALIAS> = <CMD>\n")
            return
        self.aliases[line.split('=')[0].strip()] = line.split('=')[1].strip()

    def do_PYTERM_rmalias(self, line):
        if not self.aliases.pop(line, None):
            sys.stderr.write("Alias not found")

    def get_alias(self, tok):
        for alias in self.aliases:
            if tok.split()[0] == alias:
                return self.aliases[alias] + tok[len(alias):]
        return tok

    def do_PYTERM_ignore(self, line):
        self.ignores.append(re.compile(line.strip()))

    def do_PYTERM_unignore(self, line):
        for r in self.ignores:
            if (r.pattern == line.strip()):
                print("Remove ignore for %s" % r.pattern)
                self.ignores.remove(r)
                return
        sys.stderr.write("Ignore for %s not found\n" % line.strip())

    def do_PYTERM_filter(self, line):
        self.filters.append(re.compile(line.strip()))

    def do_PYTERM_unfilter(self, line):
        for r in self.filters:
            if (r.pattern == line.strip()):
                print("Remove filter for %s" % r.pattern)
                self.filters.remove(r)
                return
        sys.stderr.write("Filter for %s not found\n" % line.strip())

    def do_PYTERM_json(self, line):
        self.json_regs[line.split(' ')[0].strip()] = re.compile(line.partition(' ')[2].strip())

    def do_PYTERM_unjson(self, line):
        if not self.aliases.pop(line, None):
            sys.stderr.write("JSON regex with ID %s not found" % line)

    def load_config(self):
        self.config = configparser.SafeConfigParser()
        self.config.read([self.configdir + os.path.sep + self.configfile])

        for sec in self.config.sections():
            if sec == "filters":
                for opt in self.config.options(sec):
                    self.filters.append(re.compile(self.config.get(sec, opt)))
            if sec == "ignores":
                for opt in self.config.options(sec):
                    self.ignores.append(re.compile(self.config.get(sec, opt)))
            if sec == "json_regs":
                for opt in self.config.options(sec):
                    print("add json regex for %s" % self.config.get(sec, opt))
                    self.json_regs[opt] = re.compile(self.config.get(sec, opt))
            if sec == "aliases":
                for opt in self.config.options(sec):
                    self.aliases[opt] = self.config.get(sec, opt)
            else:
                for opt in self.config.options(sec):
                    if opt not in self.__dict__:
                        self.__dict__[opt] = self.config.get(sec, opt)

    def reader(self):
        sr = codecs.getreader("UTF-8")(self.ser, errors='replace')
        output = ""
        while (1):
            c = sr.read(1)
            if c == '\n' or c == '\r':
                ignored = False
                if (len(self.ignores)):
                    for i in self.ignores:
                        if i.search(output):
                            ignored = True
                            break
                if (len(self.filters)):
                    for r in self.filters:
                        if r.search(output):
                            if not ignored:
                                self.logger.info(output)
                else:
                    if not ignored:
                        self.logger.info(output)

                if (len(self.json_regs)) and self.factory and self.factory.myproto:
                    for j in self.json_regs:
                        m = self.json_regs[j].search(output)
                        if m:
                            json_obj = '{"id":%d, ' % int(j)
                            json_obj += '"raw":"%s", ' % output
                            for g in m.groupdict():
                                json_obj += '"%s":"%s", ' % (g, m.groupdict()[g])

                            # eliminate the superfluous last ", "
                            json_obj = json_obj[:-2]

                            json_obj += "}"
                            self.factory.myproto.sendMessage(json_obj)

                output = ""
            else:
                output += c
            #sys.stdout.write(c)
            #sys.stdout.flush()

class PytermProt(Protocol):
    def dataReceived(self, data):
        sys.stdout.write(data)

    def sendMessage(self, msg):
        self.transport.write("%d#%s\n" % (len(msg), msg))

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
        ReconnectingClientFactory.clientConnectionLost(self, connector, reason)

    def clientConnectionFailed(self, connector, reason):
        print('Connection failed. Reason:', reason)
        ReconnectingClientFactory.clientConnectionFailed(self, connector,
                                                         reason)

def __stop_reactor(signum, stackframe):
    sys.stderr.write("Ctrl-C is disabled, type '/exit' instead\n")

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description="Pyterm - The Python terminal program")
    parser.add_argument("-p", "--port",
            help="Specifies the serial port to use, default is %s" % defaultport,
            default=defaultport)
    parser.add_argument("-b", "--baudrate",
            help="Specifies baudrate for the serial port, default is %s" % defaultbaud,
            default=defaultbaud)
    parser.add_argument('-d', '--directory',
            help="Specify the Pyterm directory, default is %s" % defaultdir,
            default=defaultdir)
    parser.add_argument("-c", "--config",
            help="Specify the config filename, default is %s" % defaultfile,
            default=defaultfile)
    parser.add_argument("-s", "--server",
            help="Connect via TCP to this server to send output as JSON")
    parser.add_argument("-P", "--tcp_port", type=int,
            help="Port at the JSON server")
    args = parser.parse_args()

    myshell = SerCmd(args.port, args.baudrate, args.directory, args.config)
    myshell.prompt = ''

    if args.server and args.tcp_port:
        myfactory = PytermClientFactory()
        reactor.connectTCP(args.server, args.tcp_port, myfactory)
        myshell.factory = myfactory
        reactor.callInThread(myshell.cmdloop, "Welcome to pyterm!\nType '/exit' to exit.")
        signal.signal(signal.SIGINT, __stop_reactor)
        reactor.run()
    else:
        myshell.cmdloop("Welcome to pyterm!\nType 'exit' to exit.")
