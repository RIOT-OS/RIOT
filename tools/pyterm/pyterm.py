#!/usr/bin/python
# -*- coding: utf-8 -*-

import cmd, serial, sys, threading, readline, time
from datetime import datetime

class MyCmd(cmd.Cmd):

	def default(self, line):
		for tok in line.split(';'):
			self.stdout.write(tok.strip() + "\n")

	def do_help(self, line):
		self.stdout.write("help\n")

	def complete_date(self, text, line, begidx, endidx):
		date = datetime.now().strftime("%Y-%M-%d %H:%m:%S")
		return ["%s\n" % date]

	def do_exit(self, line):
		sys.exit(0)

def reader(ser):
	while (1):
		c = ser.read(1)
		sys.stdout.write(c)
		sys.stdout.flush()

if __name__ == "__main__":
	
	if (len(sys.argv) != 2):
		print("Usage: %s <PORT>" % sys.argv[0])
		sys.exit(-1)

	p = sys.argv[1]
	ser = serial.Serial(port=p, baudrate=115200, dsrdtr=0, rtscts=0)

	ser.setDTR(0)
	ser.setRTS(0)

	# start serial->console thread
	receiver_thread = threading.Thread(target=reader, args=(ser,))
	receiver_thread.setDaemon(1)
	receiver_thread.start()

	myshell = MyCmd(stdout=ser)
	myshell.prompt = ""

	myshell.cmdloop("Opening port: %s" % sys.argv[1])
