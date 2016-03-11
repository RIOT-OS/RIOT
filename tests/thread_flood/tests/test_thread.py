#!/usr/bin/python

import pexpect

term = pexpect.spawn("make term")

term.expect('Start spawning\r\n')
term.expect('Thread creation successful aborted\r\n')
