#!/usr/bin/python

import pexpect
import os
import subprocess

child = pexpect.spawn ("board/msba2/tools/bin/pseudoterm %s" % os.environ["PORT"])

null = open('/dev/null', 'wb')
subprocess.call(['jam', 'reset'], stdout=null)

child.expect ('[OK].\r\n');
child.expect ('callback1\r\n');
child.expect ('callback3\r\n');
child.expect ('callback2\r\n');
print("Test successful!")

