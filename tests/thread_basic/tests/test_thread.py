#!/usr/bin/python

import pexpect
import os
import subprocess

child = pexpect.spawn("board/msba2/tools/bin/pseudoterm %s" % os.environ["PORT"])

null = open('/dev/null', 'wb')
subprocess.call(['jam', 'reset'], stdout=null)

child.expect ('first thread\r\n')
child.expect ('second thread\r\n')
print("Test successful!")
