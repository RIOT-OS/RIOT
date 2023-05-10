#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("Spawning pthreads")
    child.expect(r"created (\d+) pthreads")
    pthread_num = int(child.match.group(1))
    child.expect(r"created (\d+) threads")
    thread_num = int(child.match.group(1))
    assert thread_num == pthread_num
    child.expect_exact("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
