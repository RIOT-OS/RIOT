#!/usr/bin/python

import pexpect

term = pexpect.spawn("make term")

term.expect('first thread\r\n')
term.expect('second thread\r\n')
