#!/usr/bin/python
# -*- coding: utf-8 -*-

import cmd, serial, sys, threading, readline, time, ConfigParser
from datetime import datetime
from os import path

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

	def preloop(self):
		if not self.port:
			sys.stderr.write("No port specified!\n")
			sys.exit(-1)
		self.ser = serial.Serial(port=self.port, baudrate=115200, dsrdtr=0, rtscts=0)
		self.ser.setDTR(0)
		self.ser.setRTS(0)

		# start serial->console thread
		receiver_thread = threading.Thread(target=reader, args=(self.ser,))
		receiver_thread.setDaemon(1)
		receiver_thread.start()

	def default(self, line):
		for tok in line.split(';'):
			tok = self.get_alias(tok)
			self.ser.write(tok.strip() + "\n")

	def do_help(self, line):
		self.ser.write("help\n")

	def complete_date(self, text, line, begidx, endidm):
		date = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
		return ["%s" % date]

	def do_reset(self, line):
		self.ser.setDTR(1)
		self.ser.setRTS(1)
		self.ser.setDTR(0)
		self.ser.setRTS(0)

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

		with open(path.expanduser('~/.pyterm'), 'wb') as config_fd:
			self.config.write(config_fd)
			print("Config saved")

	def do_show_config(self, line):
		for key in self.__dict__:
			print(str(key) + ": " + str(self.__dict__[key]))

	def do_alias(self, line):
		if line.endswith("list"):
			for alias in self.aliases:
				print("%s = %s" % (alias, self.aliases[alias]))
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
		self.config = ConfigParser.SafeConfigParser()
		self.config.read([path.expanduser('~/.pyterm')])

		for sec in self.config.sections():
			if sec == "aliases":
				for opt in self.config.options(sec):
					self.aliases[opt] = self.config.get(sec, opt)
			else:
				for opt in self.config.options(sec):
					if not self.__dict__.has_key(opt):
						self.__dict__[opt] = self.config.get(sec, opt)


def reader(ser):
	while (1):
		c = ser.read(1)
		sys.stdout.write(c)
		sys.stdout.flush()

if __name__ == "__main__":
	if (len(sys.argv) > 1):
		port = sys.argv[1]
	else:
		port = None

	myshell = SerCmd(port)
	myshell.prompt = ''

	myshell.cmdloop("Welcome to pyterm")
