#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.sendline("hint")
    child.expect(r"Run \"nice (\d+) (\d+)\"\r\n")
    pid = int(child.match.group(1))
    prio = int(child.match.group(2))
    child.sendline("nice {} {}".format(pid, prio))
    child.expect_exact('[t3] Running again.')


if __name__ == "__main__":
    sys.exit(run(testfunc))
