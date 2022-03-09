#!/usr/bin/env python3

import sys
from testrunner import run

THREAD_NAMES = ("nr1", "nr2", "nr3")


def testfunc(child):
    child.expect_exact("START")
    # Collect pids
    thread_pids = []
    for name in THREAD_NAMES:
        child.expect(r"THREAD {} \(pid:(\d+)\) start".format(name))
        thread_pids.append(int(child.match.group(1)))
    child.expect_exact("THREADS CREATED")

    for index, name in enumerate(THREAD_NAMES):
        child.expect(r"THREAD {} \(pid:(\d+)\) end.".format(name))
        thread_pid = int(child.match.group(1))
        assert thread_pid == thread_pids[index]
        child.expect(r'Got msg from pid (\d+): "{}"'.format(name))
        thread_pid = int(child.match.group(1))
        assert thread_pid == thread_pids[index]

    child.expect_exact("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))
