#!/usr/bin/env python
# -*- coding: utf-8 -*-

try:
    import configparser
except ImportError:
    import ConfigParser as configparser

import cmd, serial, sys, threading, readline, time, logging, os, codecs

defaultport     = "/dev/ttyUSB0"
pytermdir       = os.environ['HOME'] + os.path.sep + '.pyterm'

class SerCmd(cmd.Cmd):

    def __init__(self, port=None):
        cmd.Cmd.__init__(self)
        self.port = port
        self.aliases = dict()
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
        logging.basicConfig(filename=pytermdir + os.path.sep + date_str + '.log', level=logging.DEBUG, format=fmt_str)
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
        self.ser = serial.Serial(port=self.port, baudrate=115200, dsrdtr=0, rtscts=0)
        self.ser.setDTR(0)
        self.ser.setRTS(0)

        # start serial->console thread
        receiver_thread = threading.Thread(target=reader, args=(self.ser,self.logger))
        receiver_thread.setDaemon(1)
        receiver_thread.start()

    def default(self, line):
        for tok in line.split(';'):
            tok = self.get_alias(tok)
            self.ser.write((tok.strip() + "\n").encode("utf-8"))

    def do_help(self, line):
        self.ser.write("help\n".encode("utf-8"))

    def complete_date(self, text, line, begidx, endidm):
        date = time.strftime("%Y-%m-%d %H:%M:%S")
        return ["{}".format(date)]

    def do_reset(self, line):
        self.ser.setDTR(1)
        self.ser.setDTR(0)

    def do_exit(self, line):
        readline.write_history_file()
        sys.exit(0)

    def do_save(self, line):
        if not self.config.has_section("general"):
            self.config.add_section("general")
        self.config.set("general", "port", self.port)
        if len(self.aliases):
            if not self.config.has_section("aliases"):
                self.config.add_section("aliases")
            for alias in self.aliases:
                self.config.set("aliases", alias, self.aliases[alias])

        with open(os.path.expanduser('~/.pyterm'), 'wb') as config_fd:
            self.config.write(config_fd)
            print("Config saved")

    def do_show_config(self, line):
        for key in self.__dict__:
            print(str(key) + ": " + str(self.__dict__[key]))

    def do_alias(self, line):
        if line.endswith("list"):
            for alias in self.aliases:
                print("{} = {}".format(alias, self.aliases[alias]))
            return
        if not line.count("="):
            sys.stderr.write("Usage: alias <ALIAS> = <CMD>\n")
            return
        self.aliases[line.split('=')[0].strip()] = line.split('=')[1].strip()

    def do_rmalias(self, line):
        if not self.aliases.pop(line, None):
            sys.stderr.write("Alias not found")

    def get_alias(self, tok):
        for alias in self.aliases:
            if tok.split()[0] == alias:
                return self.aliases[alias] + tok[len(alias):]
        return tok

    def load_config(self):
        self.config = configparser.SafeConfigParser()
        self.config.read([pytermdir + os.path.sep + 'pyterm.conf'])

        for sec in self.config.sections():
            if sec == "aliases":
                for opt in self.config.options(sec):
                    self.aliases[opt] = self.config.get(sec, opt)
            else:
                for opt in self.config.options(sec):
                    if opt not in self.__dict__:
                        self.__dict__[opt] = self.config.get(sec, opt)


def reader(ser, logger):
    sr = codecs.getreader("UTF-8")(ser, errors='replace')
    output = ""
    while (1):
        c = sr.read(1)
        if c == '\n' or c == '\r':
            logger.info(output)
            output = ""
        else:
            output += c
        #sys.stdout.write(c)
        #sys.stdout.flush()

if __name__ == "__main__":
    if not os.path.exists(pytermdir):
        os.makedirs(pytermdir)

    if (len(sys.argv) > 1):
        port = sys.argv[1]
    else:
        port = None

    myshell = SerCmd(port)
    myshell.prompt = ''

    try:
        myshell.cmdloop("Welcome to pyterm!\nType 'exit' to exit.")
    except KeyboardInterrupt:
        myshell.do_exit(0)
