#!/usr/bin/python

import pexpect
import os
import subprocess

child = pexpect.spawn ("pseudoterm %s" % os.environ["PORT"])

null = open('/dev/null', 'wb')
subprocess.call(['jam', 'reset'], stdout=null)

child.expect ('callback\r\n');
child.expect ('wakeup\r\n');
child.expect ('message\r\n');
print("Test successful!")
